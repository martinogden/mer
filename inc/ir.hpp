#pragma once
#include <iostream>


/*
@link https://stackoverflow.com/a/1753627/107987

64-bit register | Lower 32 bits | Lower 16 bits | Lower 8 bits
==============================================================
rax             | eax           | ax            | al
rbx             | ebx           | bx            | bl
rcx             | ecx           | cx            | cl
rdx             | edx           | dx            | dl
rsi             | esi           | si            | sil
rdi             | edi           | di            | dil
rbp             | ebp           | bp            | bpl
rsp             | esp           | sp            | spl
r8              | r8d           | r8w           | r8b
r9              | r9d           | r9w           | r9b
r10             | r10d          | r10w          | r10b
r11             | r11d          | r11w          | r11b
r12             | r12d          | r12w          | r12b
r13             | r13d          | r13w          | r13b
r14             | r14d          | r14w          | r14b
r15             | r15d          | r15w          | r15b
 */
enum class Reg {
	EAX, EDI, ESI, EDX, ECX, R8D, R9D, R10D, R11D,  // caller-saved
	EBX, R12D, R13D, R14D, R15D, RSP, RBP, // callee-saved
};


enum class OpCode {
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,

	MOV,
	RET,
};


enum class OpType {
	REG,
	IMM,
	TMP,
};


struct Operand {
	OpType type;
	int value;

	Operand(OpType type, int value) :
		type(type),
		value(value)
	{}

	Operand(Reg reg) :
		Operand(OpType::REG, static_cast<int>(reg))
	{}

	Operand(int value) :
		Operand(OpType::TMP, value)
	{}
};


struct Inst {
	OpCode opcode;
	Operand dst;
	Operand s1;
	Operand s2;
	uint arity;

	Inst(OpCode opcode, Operand dst, Operand s1, Operand s2, uint arity) :
		opcode(opcode),
		dst(dst),
		s1(s1),
		s2(s2),
		arity(arity)
	{}

	Inst(OpCode opcode, Operand dst, Operand s1, Operand s2) :
		Inst(opcode, dst, s1, s2, 3)
	{}

	Inst(OpCode opcode, Operand dst, Operand src) :
		Inst(opcode, dst, src, 0, 2)
	{}

	Inst(OpCode opcode) :
		Inst(opcode, 0, 0, 0, 0)
	{}
};


std::ostream& operator<<(std::ostream& output, const OpCode& opcode);
std::ostream& operator<<(std::ostream& output, const Reg& reg);
std::ostream& operator<<(std::ostream& output, const Operand& op);
std::ostream& operator<<(std::ostream& output, const Inst& inst);
