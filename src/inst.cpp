#include "inst.hpp"


Inst::Type Inst::getType() const {
	switch (getOpcode()) {
		case ADD:
		case SUB:
		case MUL:
		case DIV:
		case MOD:
		case AND:
		case OR:
		case XOR:
			return Type::BINARY;
		case MOV:
			return Type::UNARY;
		case RET:
			return Type::RET;
		case JMP:
			return Type::JMP;
		case JEQ:
		case JNE:
		case JLT:
		case JLE:
		case JGT:
		case JGE:
			return Type::CJMP;
		case LBL:
			return Type::LABEL;
	}
}


uint Inst::getParity() const {
	return parity;
}


Inst::OpCode Inst::getOpcode() const {
	return opcode;
}


Operand Inst::getDst() const {
	if (parity > 0)
		return dst;
	throw 1;  // TODO: handle properly
}


Operand Inst::getSrc1() const {
	if (parity > 1)
		return src1;
	throw 1;  // TODO: handle properly
}


Operand Inst::getSrc2() const {
	if (parity > 2)
		return src2;
	throw 1;  // TODO: handle properly
}


bool Inst::is(OpCode op) const {
	return op == opcode;
}


bool Inst::is(std::set<OpCode> ops) const {
	return ops.find(opcode) != ops.end();
}
