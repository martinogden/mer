#include "inst/inst.hpp"


Inst::Inst(OpCode opcode, Operand dst) :
	parity(1),
	opcode(opcode),
	dst(std::move(dst)),
	src1(0),
	src2(0)
{}


Inst::Inst(OpCode opcode, Operand dst, Operand src1) :
	parity(2),
	opcode(opcode),
	dst(std::move(dst)),
	src1(std::move(src1)),
	src2(0)
{}


Inst::Inst(OpCode opcode, Operand dst, Operand src1, Operand src2) :
	parity(3),
	opcode(opcode),
	dst(std::move(dst)),
	src1(std::move(src1)),
	src2(std::move(src2))
{}


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


InstFun::InstFun(std::string id, std::vector<std::string> params, std::vector<Inst> insts) :
	id(std::move(id)),
	params(std::move(params)),
	insts(std::move(insts))
{}
