#pragma once
#include <vector>
#include <unordered_map>
#include "operand.hpp"
#include "x86asm.hpp"


typedef std::unordered_map<Operand, Reg> Alloc;

Alloc regAlloc(std::vector<X86Asm>& code);

std::vector<X86Asm> regAssign(std::vector<X86Asm>& code, Alloc& regs);
