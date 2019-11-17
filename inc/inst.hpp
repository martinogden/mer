#pragma once
#include "operand.hpp"


struct Inst {
public:
	enum OpCode {
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,

		AND,
		OR,
		XOR,

		MOV,

		LBL,
		JMP,
		JEQ,
		JNE,
		JLT,
		JLE,
		JGT,
		JGE,

		RET,
	};

	enum class Type {
		LABEL,
		BINARY,
		UNARY,
		RET,
		JMP,
		CJMP,
	};

	Inst(OpCode opcode, Operand dst)
		: parity(1), opcode(opcode), dst(dst), src1(0), src2(0) {}
	Inst(OpCode opcode, Operand dst, Operand src1)
		: parity(2), opcode(opcode), dst(dst), src1(src1), src2(0) {}
	Inst(OpCode opcode, Operand dst, Operand src1, Operand src2)
		: parity(3), opcode(opcode), dst(dst), src1(src1), src2(src2) {}

	Type getType() const;
	uint getParity() const;
	OpCode getOpcode() const;
	Operand getDst() const;
	Operand getSrc1() const;
	Operand getSrc2() const;
	bool is(OpCode op) const;
	// TODO: hash opcodes so we can use unordered_set here
	bool is(std::set<OpCode> ops) const;

	friend std::ostream& operator<<(std::ostream&, const Inst&);

private:
	uint parity;
	OpCode opcode;
	Operand dst;
	Operand src1;
	Operand src2;
};
