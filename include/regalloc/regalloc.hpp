#pragma once
#include <unordered_map>
#include "inst/inst.hpp"


constexpr uint MAX_REG = 12;  // %r14d
typedef std::unordered_map<Operand, Reg> Alloc;

Alloc regAlloc(InstFun& fun);
InstFun regAssign(InstFun& fun, Alloc& regs);
