#include <sstream>
#include "compiler.hpp"
#include "lexer.hpp"
#include "parser/parser.hpp"
#include "return-checker.hpp"
#include "type-checker.hpp"
#include "elaborator.hpp"
#include "codegen.hpp"
#include "tree-printer.hpp"
#include "ast-printer.hpp"
#include "init-checker.hpp"
#include "use-checker.hpp"
#include "translator/translator.hpp"
#include "irt-printer.hpp"
#include "generator.hpp"
#include "graph.hpp"
#include "x86/codegen.hpp"
#include "x86/regalloc.hpp"


std::string join(std::vector<std::string>& lines) {
	if (lines.size() == 0)
		return "";

	std::stringstream buf;
	for (auto it=lines.begin(); it != --lines.end(); ++it)
		buf << (*it) << "\n";
	buf << *(--lines.end());
	return buf.str();
}


std::pair<bool, std::string> compile(std::string src, Stage stage) {
	Lexer lexer(src);

	Parser parser(lexer);
	ParseTree* tree = parser.run();
	if (parser.errors.exist())
		return {false, join( parser.errors.get() )};

	if (stage == Stage::PARSE) {
		TreePrinter printer(tree);
		return {true, printer.run()};
	}


	Elaborator elab(tree);
	ASTNode* ast = elab.run();
	if (elab.errors.exist())
		return {false, join( elab.errors.get() )};

	if (stage == Stage::AST) {
		ASTPrinter printer(ast);
		return {true, printer.run()};
	}

	ReturnChecker retChecker(ast);
	retChecker.run();
	if (retChecker.errors.exist())
		return {false, join( retChecker.errors.get() )};

	InitChecker initChecker(ast);
	initChecker.run();
	if (initChecker.errors.exist())
		return {false, join( initChecker.errors.get() )};

	UseChecker useChecker(ast);
	useChecker.run();
	if (useChecker.errors.exist())
		return {false, join( useChecker.errors.get() )};

	TypeChecker tc(ast);
	tc.run();
	if (tc.errors.exist())
		return {false, join( tc.errors.get() )};


	Generator gen;
	Translator tr(gen);
	IRTCmd* cmd = tr.get(ast);

	if (stage == Stage::HIR) {
		IRTPrinter printer;
		return {true, printer.get(cmd)};
	}


	CodeGen codegen(cmd, gen);
	std::vector<Inst>& insts = codegen.run();

	if (stage == Stage::LIR) {
		for (Inst& inst : insts)
			std::cout << inst << std::endl;
		return {true, ""};
	}



	X86CodeGen x86codegen(insts, gen);
	std::vector<X86Asm> code = x86codegen.run();

	if (stage == Stage::CODEGEN) {
		for (auto& as : code)
			std::cout << as << std::endl;
		return {true, ""};
	}


	Alloc regs = regAlloc(code);

	if (stage == Stage::REGALLOC) {
		for (const auto& pair : regs) {
			if (pair.first.is(Operand::TMP))
				std::cout << pair.first << " -> " << pair.second << std::endl;
		}
		return {true, ""};
	}

	code = regAssign(code, regs);

	// TODO: tidy up
	std::cout << ".section __TEXT,__text" << std::endl;
	std::cout << ".globl __c0_main" << std::endl;
	std::cout << "__c0_main:" << std::endl;

	for (auto& as : code)
		std::cout << as << std::endl;

	return {true, ""};
}
