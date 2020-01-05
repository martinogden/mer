#include <memory>
#include <sstream>
#include <set>
#include "compiler.hpp"
#include "generator.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"
#include "sca/return-checker.hpp"
#include "sca/init-checker.hpp"
#include "sca/use-checker.hpp"
#include "type/checker.hpp"
#include "inst/codegen.hpp"
#include "cst/printer.hpp"
#include "ast/elaborator/stmt.hpp"
#include "ast/printer.hpp"
#include "irt/translator/translator.hpp"
#include "irt/printer.hpp"
#include "irt/translator/structs.hpp"
#include "graph.hpp"
#include "x86/codegen.hpp"
#include "regalloc/regalloc.hpp"


#include "print-utils.hpp"


const std::string MAIN_PREFIX("_c0_");


std::pair<bool, std::string> compile(std::string src, Stage stage) {
	Generator gen;

	// Front end
	Lexer lexer(std::move(src));

	Parser parser(lexer);
	ParseTree tree = parser.run();
	if (parser.errors.exist())
		return {false, join( parser.errors.get(), "\n" )};

	if (stage == Stage::PARSE) {
		CSTPrinter printer;
		return {true, printer.get(tree)};
	}


	Elaborator elab(tree, gen);
	std::vector<FunNodePtr> defns = elab.run();

	Map<FunTypePtr> funTypes = elab.getFunTypes();
	Map<StructTypePtr> structTypes = elab.getStructTypes();

	if (elab.errors.exist())
		return {false, join( elab.errors.get(), "\n" )};

	if (stage == Stage::AST) {
		ASTPrinter printer;
		for (auto& ast : defns)
			std::cout << printer.get(ast) << std::endl;

		return {true, ""};
	}


	// Semantic analysis stage
	Errors errors;

	for (auto& ast : defns) {
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

		ReturnChecker retChecker(ast);
		retChecker.run();
		if (retChecker.errors.exist()) {
			errors.append(retChecker.errors);
			continue;
		}

		TypeChecker typeChecker(ast, funTypes, structTypes);
		typeChecker.run();
		if (typeChecker.errors.exist()) {
			errors.append(typeChecker.errors);
			continue;
		}
	}

	if (errors.exist())
		return {false, join( errors.get(), "\n" )};


	Map<IRTStruct> structs = toIRTStructs(structTypes);


	// Middle end (intermediate code gen)

	if (stage == Stage::ASM) {
		// TODO: tidy up
		std::cout << ".section __TEXT,__text" << std::endl;
		std::cout << ".globl __c0_main" << std::endl;
	}

	for (auto& ast : defns) {
		Translator tr(gen, structs);
		IRTFun irt = tr.get(ast);

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


		// Back end
		Alloc alloc = regAlloc(fun);

		if (stage == Stage::REGALLOC) {
			for (const auto& pair : alloc.map) {
				if (pair.first.is(Operand::TMP))
					std::cout << pair.first << " -> " << pair.second << std::endl;
			}
		}

		fun = regAssign(fun, alloc);

		if (stage == Stage::REGALLOC) {
			for (Inst& inst : fun.insts)
				std::cout << inst << std::endl;
			continue;
		}

		X86CodeGen x86codegen(fun, alloc);
		X86Fun x86fun = x86codegen.run();

		if (stage == Stage::ASM) {
			for (auto& as : x86fun.code)
				std::cout << as << std::endl;
		}
	}

	return {true, ""};
}
