#include "operand.hpp"


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


uint Operand::getImm() const {
	assert(type == IMM);
	return imm;
}


Reg Operand::getReg() const {
	assert(type == REG);
	return reg;
}


std::string Operand::to_string() const {
	switch (type) {
		case Operand::LBL:
			return getLabel();
		case Operand::IMM:
			return "$" + std::to_string(getImm());
		case Operand::REG: {
			std::stringstream buf;
			buf << getReg();
			return buf.str();
		}
		case Operand::TMP:
			return getTmp();
		case Operand::MEM:
			throw 1;  // TODO
	}
}


bool operator==(Operand a, Operand b) {
	if (a.getType() != b.getType())
		return false;

	return a.to_string() == b.to_string();
}

bool operator!=(Operand a, Operand b) {
	return !(a == b);
}
