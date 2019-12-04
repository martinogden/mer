#include <memory>
#include <sstream>
#include <set>
#include "compiler.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"
#include "sca/return-checker.hpp"
#include "type/checker.hpp"
#include "ast/elaborator.hpp"
#include "inst/codegen.hpp"
#include "cst/printer.hpp"
#include "ast/printer.hpp"
#include "sca/init-checker.hpp"
#include "sca/use-checker.hpp"
#include "irt/translator/translator.hpp"
#include "irt/printer.hpp"
#include "inst/generator.hpp"
#include "graph.hpp"
#include "x86/codegen.hpp"
#include "x86/regalloc.hpp"
#include "print-utils.hpp"


const std::string MAIN_PREFIX("_c0_");


std::vector<Reg> getUsedRegs(X86Fun& fun, const std::vector<Reg>& filter) {
	std::vector<Reg> out;
	std::set<Reg> regs;
	std::set<Reg> used;

	for (const auto &reg : filter)
		regs.insert(reg);

	for (const auto& as : fun.code) {
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
	ParseTree tree = parser.run();
	if (parser.errors.exist())
		return {false, join( parser.errors.get(), "\n" )};

	if (stage == Stage::PARSE) {
		CSTPrinter printer(tree);
		return {true, printer.run()};
	}


	Elaborator elab(tree);
	std::vector<FunNodePtr> defns = elab.run();
	SymTab<FunType> decls = elab.getDecls();

	if (elab.errors.exist())
		return {false, join( elab.errors.get(), "\n" )};

	if (stage == Stage::AST) {
		for (auto& ast : defns) {
			ASTPrinter printer(ast);
			std::cout << printer.run() << std::endl;
		}
		return {true, ""};
	}

	// Semantic analysis stage
	Errors errors;

	for (auto& ast : defns) {
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

	for (auto& ast : defns) {
		Translator tr(gen);
		IRTFunPtr irt = tr.get(ast);

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
		X86Fun x86fun = x86codegen.run();

		if (stage == Stage::CODEGEN) {
			for (auto& as : x86fun.code)
				std::cout << as << std::endl;
			continue;
		}

		Alloc alloc = regAlloc(x86fun);

		if (stage == Stage::REGALLOC) {
			for (const auto& pair : alloc) {
				if (pair.first.is(Operand::TMP))
					std::cout << pair.first << " -> " << pair.second << std::endl;
			}
			continue;
		}

		if (stage == Stage::ASM) {
			x86fun = regAssign(x86fun, alloc);

			std::vector<Reg> saved_regs = getUsedRegs(x86fun, calleeSaved);

			// TODO: extract to x86
			std::cout << X86Asm(X86Asm::PUSH, Reg::RBP) << std::endl;
			std::cout << X86Asm(X86Asm::MOV, Reg::RBP, Reg::RSP) << std::endl;

			// ensure stack is 16-byte aligned
			uint num_slots = (saved_regs.size() + 1) & ~1U;

			if (num_slots > 0) {
				std::cout << X86Asm(X86Asm::SUB, Reg::RSP, 8 * num_slots) << std::endl;
				int i = 1;
				for (auto reg : saved_regs)
					std::cout << X86Asm(X86Asm::MOV, Operand(Reg::RBP, -8 * i++), reg) << std::endl;
			}

			for (auto& as : x86fun.code)
				std::cout << as << std::endl;

			if (num_slots > 0) {
				int i = 1;
				for (auto reg : saved_regs)
					std::cout << X86Asm(X86Asm::MOV, reg, Operand(Reg::RBP, -8 * i++)) << std::endl;

				std::cout << X86Asm(X86Asm::ADD, Reg::RSP, 8*num_slots) << std::endl;
			}

			std::cout << X86Asm(X86Asm::POP, Reg::RBP) << std::endl;
			std::cout << X86Asm(X86Asm::RET) << std::endl;
			continue;
		}
	}

	return {true, ""};
}
