#include <sstream>
#include <set>
#include "compiler.hpp"
#include "lexer.hpp"
#include "parser/parser.hpp"
#include "return-checker.hpp"
#include "type-checker.hpp"
#include "elaborator.hpp"
#include "codegen.hpp"
#include "parse-tree-printer.hpp"
#include "ast-printer.hpp"
#include "init-checker.hpp"
#include "use-checker.hpp"
#include "translator/translator.hpp"
#include "irt-printer.hpp"
#include "generator.hpp"
#include "graph.hpp"
#include "x86/codegen.hpp"
#include "x86/regalloc.hpp"
#include "print-utils.hpp"


const std::string MAIN_PREFIX("_c0_");


std::vector<Reg> getRegs(const std::vector<X86Asm>& code, const std::vector<Reg>& from) {
	std::vector<Reg> out;
	std::set<Reg> regs;
	std::set<Reg> used;

	for (const auto &reg : from)
		regs.insert(reg);

	for (const auto& as : code) {
		if (as.parity > 1 && as.src.getType() == Operand::REG)
			used.insert(as.src.getReg());
		if (as.parity > 0 && as.dst.getType() == Operand::REG)
			used.insert(as.dst.getReg());
	}

	for (const auto& reg : used) {
		if (regs.find(reg) != regs.end())
			out.push_back(reg);
	}

	return out;
}


std::pair<bool, std::string> compile(std::string src, Stage stage) {
	Lexer lexer(std::move(src));

	Parser parser(lexer);
	ParseTree* tree = parser.run();
	if (parser.errors.exist())
		return {false, join( parser.errors.get(), "\n" )};

	if (stage == Stage::PARSE) {
		TreePrinter printer(tree);
		return {true, printer.run()};
	}


	Elaborator elab(tree);
	std::vector<FunNode*> defns = elab.run();
	SymTab<FunType> decls = elab.getDecls();

	if (elab.errors.exist())
		return {false, join( elab.errors.get(), "\n" )};

	if (stage == Stage::AST) {
		ASTPrinter printer(defns);
		return {true, printer.run()};
	}

	// Semantic analysis stage
	Errors errors;

	for (auto const& ast : defns) {
		// TODO check main: () -> int is defined

		ReturnChecker retChecker(ast);
		retChecker.run();
		if (retChecker.errors.exist()) {
			errors.append(retChecker.errors);
			continue;
		}

		InitChecker initChecker(ast);
		initChecker.run();
		if (initChecker.errors.exist()) {
			errors.append(initChecker.errors);
			continue;
		}

		UseChecker useChecker(ast);
		useChecker.run();
		if (useChecker.errors.exist()) {
			errors.append(useChecker.errors);
			continue;
		}

		TypeChecker typeChecker(ast, decls);
		typeChecker.run();
		if (typeChecker.errors.exist()) {
			errors.append(typeChecker.errors);
			continue;
		}
	}

	if (errors.exist())
		return {false, join( errors.get(), "\n" )};


	// Middle end (intermediate code)
	Generator gen;

	if (stage == Stage::ASM) {
		// TODO: tidy up
		std::cout << ".section __TEXT,__text" << std::endl;
		std::cout << ".globl __c0_main" << std::endl;
	}

	for (auto const& ast : defns) {
		Translator tr(gen);
		IRTFun* irt = tr.get(ast);

		if (stage == Stage::HIR) {
			IRTPrinter printer;
			std::cout << printer.get(irt) << std::endl;
			continue;
		}


		CodeGen codegen(irt, gen);
		InstFun fun = codegen.run();

		std::string prefix = (fun.id == "main") ? MAIN_PREFIX : "";
		std::cout << "\n" << "_" << prefix << fun.id << ":" << std::endl;

		if (stage == Stage::LIR) {
			for (Inst& inst : fun.insts)
				std::cout << inst << std::endl;
			continue;
		}

		X86CodeGen x86codegen(fun, gen);
		std::vector<X86Asm> code = x86codegen.run();

		if (stage == Stage::CODEGEN) {
			for (auto& as : code)
				std::cout << as << std::endl;
			continue;
		}

		Alloc regs = regAlloc(code);

		if (stage == Stage::REGALLOC) {
			for (const auto& pair : regs) {
				if (pair.first.is(Operand::TMP))
					std::cout << pair.first << " -> " << pair.second << std::endl;
			}
			continue;
		}

		if (stage == Stage::ASM) {
			code = regAssign(code, regs);

			std::vector<Reg> savedRegs = getRegs(code, calleeSaved);

			// TODO: extract to x86
			std::cout << X86Asm(X86Asm::PUSH, Reg::RBP) << std::endl;
			std::cout << X86Asm(X86Asm::MOV, Reg::RBP, Reg::RSP) << std::endl;

			// ensure stack is 16-byte aligned
			uint num_slots = (savedRegs.size() + 1) & ~1U;

			if (num_slots > 0) {
				std::cout << X86Asm(X86Asm::SUB, Reg::RSP, 8 * num_slots) << std::endl;
				uint i = 1;
				for (auto it = savedRegs.rbegin(); it != savedRegs.rend(); ++it)
					std::cout << X86Asm(X86Asm::MOV, Operand(Reg::RBP, -8 * i++), *it) << std::endl;
			}

			for (auto& as : code)
				std::cout << as << std::endl;

			if (num_slots > 0)
				std::cout << X86Asm(X86Asm::ADD, Reg::RSP, 8*num_slots) << std::endl;

			std::cout << X86Asm(X86Asm::POP, Reg::RBP) << std::endl;
			std::cout << X86Asm(X86Asm::RET) << std::endl;
			continue;
		}
	}

	return {true, ""};
}
