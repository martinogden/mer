#include "inst/operand.hpp"


Set<Reg> callerSaved ({
	//Reg::RAX,
	Reg::RDI, Reg::RSI,
	Reg::RDX, Reg::RCX, Reg::R8,
	Reg::R9, Reg::R10, Reg::R11,
});


Set<Reg> calleeSaved({
	Reg::RBX, Reg::R12, Reg::R13,
	// Reg::R14, Reg::R15,
});


Operand::Operand(Type type, std::string tmp) :
	type(type),
	tmp(std::move(tmp))
{}


Operand::Operand() {}


Operand::Operand(Tmp tmp) :
	Operand(TMP, std::move(tmp))
{}


Operand::Operand(Imm imm) :
	type(IMM),
	imm(imm)
{}


Operand::Operand(Reg reg) :
	type(REG),
	reg(reg)
{}


Operand::Operand(Tmp tmp, int offset) :
	offset(offset),
	memType(TMP),
	type(MEM),
	tmp(std::move(tmp))
{}


Operand::Operand(Reg reg, int offset) :
	offset(offset),
	memType(REG),
	type(MEM),
	reg(reg)
{}


Operand::Type Operand::getType() const {
	return type;
}


bool Operand::is(Type ty) const {
	return ty == type;
}


std::string Operand::getLabel() const {
	assert(type == LBL);
	return tmp;
}


std::string Operand::getTmp() const {
	assert(type == TMP);
	return tmp;
}


int Operand::getImm() const {
	assert(type == IMM);
	return imm;
}


Reg Operand::getReg() const {
	assert(type == REG);
	return reg;
}


Operand Operand::getMemOperand() const {
	assert(type == MEM);
	switch (memType) {
		case REG:
			return Operand(reg);
		case TMP:
			return Operand(tmp);
		default:
			throw 1;  // we should never get here
	}
}


int Operand::getMemOffset() const {
	assert(type == MEM);
	return offset;
}


std::string Operand::to_string() const {
	std::stringstream buf;
	buf << *this;
	return buf.str();
}


bool operator==(Operand a, Operand b) {
	if (a.getType() != b.getType())
		return false;

	return a.to_string() == b.to_string();
}

bool operator!=(Operand a, Operand b) {
	return !(a == b);
}
