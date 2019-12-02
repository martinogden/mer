#include <iostream>
#include "files.hpp"
#include "compiler.hpp"


Stage toStage(std::string s) {
	if (s == "")
		return Stage::ASM;

	if (s == "-parse")
		return Stage::PARSE;
	if (s == "-ast")
		return Stage::AST;
	if (s == "-hir")
		return Stage::HIR;
	if (s == "-lir")
		return Stage::LIR;
	if (s == "-codegen")
		return Stage::CODEGEN;
	if (s == "-regalloc")
		return Stage::REGALLOC;
	if (s == "-asm")
		return Stage::ASM;

	throw std::invalid_argument("Invalid stage: " + s);
}


int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Error: please supply a filename" << std::endl;
		return 1;
	}

	std::string fn(argv[1]);
	std::string arg;
	if (argc > 2)
		arg = argv[2];

	std::string src;
	Stage stage;

	try {
		src = read(fn);
		stage = toStage(arg);
	} catch (std::invalid_argument& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}

	auto result = compile(src, stage);

	if (result.first) {
		std::cout << result.second << std::endl;
		return 0;
	}
	else {
		std::cerr << result.second << std::endl;
		return 1;
	}
}
