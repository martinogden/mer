#pragma once
#include <iostream>


enum class UnOp {
	LOG_NOT,
	BIT_NOT,
	NEG,
};


enum class BinOp {
	// arithmetic
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,

	// relational
	LT,
	LT_EQL,
	GT,
	GT_EQL,
	EQL,
	NOT_EQL,

	// logical
	LOG_AND,
	LOG_OR,

	// bitwise
	BIT_AND,
	BIT_OR,
	XOR,
};


enum class OpType {
	ARITH,
	BITWISE,
	LOGICAL,
	REL,
	EQL,
};


OpType getOpType(BinOp op);


std::ostream& operator<<(std::ostream& output, const UnOp& op);
std::ostream& operator<<(std::ostream& output, const BinOp& op);
