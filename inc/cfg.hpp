#pragma once

#include "ir.hpp"
#include <vector>


struct BasicBlock {
	std::vector<Inst> insts;

	BasicBlock() {}
	BasicBlock(std::vector<Inst> insts) :
		insts(insts)
	{}

	size_t size() {
		return insts.size();
	}

	Inst& operator[](int i) {
		return insts[i];
	}
};


// straight-line code -> (single) basic block
BasicBlock toBasicBlock(std::vector<Inst> insts);
