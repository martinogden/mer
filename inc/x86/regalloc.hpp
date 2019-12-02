#pragma once
#include <vector>
#include <unordered_map>
#include "operand.hpp"
#include "x86/asm.hpp"


typedef std::unordered_map<Operand, Reg> Alloc;

Alloc regAlloc(X86Fun& code);

X86Fun regAssign(X86Fun& code, Alloc& regs);
