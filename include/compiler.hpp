#pragma once
#include <string>


enum class Stage {
	PARSE,
	AST,
	HIR,
	LIR,
	CODEGEN,
	REGALLOC,
	ASM,
};


std::pair<bool, std::string> compile(std::string fn, Stage stage=Stage::ASM);
