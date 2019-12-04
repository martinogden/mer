#pragma once
#include <vector>
#include "operand.hpp"


struct Inst {
public:
	enum OpCode {
		LBL,

		ADD, SUB, MUL,
		DIV, MOD,
		AND, OR, XOR,
		MOV,
		JMP, JEQ, JNE, JLT, JLE, JGT, JGE,
		RET, CALL, ARG,
	};

	enum class Type {
		LABEL,
		BINARY,
		UNARY,
		RET,
		CALL,
		JMP,
		CJMP,
	};

	Inst(OpCode opcode, Operand dst);
	Inst(OpCode opcode, Operand dst, Operand src1);
	Inst(OpCode opcode, Operand dst, Operand src1, Operand src2);

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


// TODO: better name
struct InstFun {
public:
	std::string id;
	std::vector<std::string> params;
	std::vector<Inst> insts;

	InstFun(std::string id, std::vector<std::string> params, std::vector<Inst> insts);
};
