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
		RET, CALL,
		PUSH, POP,
		CMP,
	};

	OpCode opcode;
	Operand dst;
	Operand src;
	uint parity;

	X86Asm(std::string label);
	X86Asm(OpCode opcode);
	X86Asm(OpCode opcode, Operand dst);
	X86Asm(OpCode opcode, Operand dst, Operand src);

	friend std::ostream& operator<<(std::ostream& output, const X86Asm& as);
	friend std::ostream& operator<<(std::ostream& output, const OpCode& op);
};


struct X86Fun {
	std::vector<X86Asm> code;
	std::vector<std::string> params;
	X86Fun(std::vector<X86Asm> code, std::vector<std::string> params);
};