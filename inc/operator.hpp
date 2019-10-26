#pragma once
#include <iostream>


enum class UnOp {
	NEG,
};


enum class BinOp {
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
};


enum class AsnOp {
	ADD_EQL,
	SUB_EQL,
	MUL_EQL,
	DIV_EQL,
	MOD_EQL,
	EQL,
};


std::ostream& operator<<(std::ostream& output, const UnOp& op);
std::ostream& operator<<(std::ostream& output, const BinOp& op);
std::ostream& operator<<(std::ostream& output, const AsnOp& op);
