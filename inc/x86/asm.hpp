#pragma once
#include <iostream>
#include <string>
#include "operand.hpp"


constexpr uint MAX_REG = 15;


struct X86Asm {
	enum OpCode {
		LBL,

		NEG,
		ADD, SUB, IMUL,
		IDIV, MOD, CDQ,
		AND, OR, XOR,
		MOV,
		JMP, JE, JNE, JL, JLE, JG, JGE,
		RET,
		PUSH, POP,
		CMP,
	};

	OpCode opcode;
	Operand dst;
	Operand src;
	uint parity;

	X86Asm(std::string label)
		: opcode(LBL), dst(label), parity(1) {}
	X86Asm(OpCode opcode)
		: opcode(opcode), parity(0) {}
	X86Asm(OpCode opcode, Operand dst)
		: opcode(opcode), dst(dst), parity(1) {}
	X86Asm(OpCode opcode, Operand dst, Operand src)
		: opcode(opcode), dst(dst), src(src), parity(2) {}

	friend std::ostream& operator<<(std::ostream& output, const X86Asm& as);
	friend std::ostream& operator<<(std::ostream& output, const OpCode& op);
};
