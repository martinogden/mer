#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "operand.hpp"


struct BasicBlock {
	std::vector<Inst> insts;
	std::vector<BasicBlock*> succ;
};


struct CFG {
	std::unordered_map<std::string, BasicBlock*> vertices;
};