#pragma once
#include <unordered_map>
#include "inst/inst.hpp"


constexpr int MAX_REG = 11;  // %r13d


struct Alloc {
	std::unordered_map<Operand, Operand> map;
	uint num_used_regs;
	uint num_spilled;

	Alloc();
	Operand lookup(const Operand& operand) const;
	void assign(const Operand& src, const Operand& dst);
};


Alloc regAlloc(const InstFun& fun);
InstFun regAssign(const InstFun& fun, const Alloc& regs);
