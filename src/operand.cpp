#include "operand.hpp"


std::string promote(Reg reg) {
	switch(reg) {
		case Reg::EAX:
			return "%rax";
		case Reg::EBX:
			return "%rbx";
		case Reg::ECX:
			return "%rcx";
		case Reg::EDX:
			return "%rdx";
		case Reg::EDI:
			return "%rdi";
		case Reg::ESI:
			return "%rsi";
		case Reg::R8D:
			return "%r8";
		case Reg::R9D:
			return "%r9";
		case Reg::R10D:
			return "%r10";
		case Reg::R11D:
			return "%r11";
		case Reg::R12D:
			return "%r12";
		case Reg::R13D:
			return "%r13";
		case Reg::R14D:
			return "%r14";
		case Reg::R15D:
			return "%r15";
		case Reg::RBP:
			return "%rbp";
		case Reg::RSP:
			return "%rsp";

	}
}


Mem::Mem(Reg reg, int offset) :
	reg(reg), offset(offset)
{}


Operand::Operand() {}


Operand::Operand(Tmp tmp) :
	Operand(TMP, tmp)
{}


Operand::Operand(Imm imm) :
	type(IMM),
	imm(imm)
{}


Operand::Operand(Reg reg) :
	type(REG),
	reg(reg)
{}


Operand::Operand(Reg reg, int off) :
	type(MEM),
	mem({reg, off})
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
