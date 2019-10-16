#include <iostream>
#include "compiler.hpp"
#include "parser.hpp"
#include "semant.hpp"
#include "codegen.hpp"


void printMsgs(std::vector<std::string>& msgs) {
	for (std::string& msg : msgs)
		std::cout << msg << std::endl;
}


bool compile(std::string src) {
	Parser parser(src);
	Prog* program = parser.run();

	if (!program) {
		printMsgs(parser.getErrors());
		return false;
	}

	Semant semant(program);
	bool is_valid = semant.run();

	if (!is_valid) {
		printMsgs(semant.getErrors());
		return false;
	}

	CodeGen codegen(program);
	std::vector<Inst>& insts = codegen.run();

	for (Inst& inst : insts)
		std::cout << inst << std::endl;

	return true;
}
