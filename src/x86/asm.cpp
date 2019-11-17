#include "x86/asm.hpp"


std::ostream& operator<<(std::ostream& output, const X86Asm& as) {
	if (as.opcode == X86Asm::LBL) {
		output << as.dst << ":";
		return output;
	}

	switch (as.parity) {
		case 0:
			output << '\t' << as.opcode;
			break;
		case 1:
			output << '\t' << as.opcode << " " << as.dst;
			break;
		case 2:
			output << '\t' << as.opcode << " " << as.src << ", " << as.dst;
			break;
	}

	return output;
}


std::ostream& operator<<(std::ostream& output, const X86Asm::OpCode& op) {
	switch (op) {
		case X86Asm::LBL:
			output << "<label>"; break;
		case X86Asm::NEG:
			output << "neg"; break;
		case X86Asm::ADD:
			output << "add"; break;
		case X86Asm::SUB:
			output << "sub"; break;
		case X86Asm::IMUL:
			output << "imul"; break;
		case X86Asm::IDIV:
			output << "idiv"; break;
		case X86Asm::MOD:
			output << "mod"; break;
		case X86Asm::CDQ:
			output << "cdq"; break;
		case X86Asm::AND:
			output << "and"; break;
		case X86Asm::OR:
			output << "or"; break;
		case X86Asm::XOR:
			output << "xor"; break;
		case X86Asm::MOV:
			output << "mov"; break;
		case X86Asm::JMP:
			output << "jmp"; break;
		case X86Asm::JE:
			output << "je"; break;
		case X86Asm::JNE:
			output << "jne"; break;
		case X86Asm::JL:
			output << "jl"; break;
		case X86Asm::JLE:
			output << "jle"; break;
		case X86Asm::JG:
			output << "jg"; break;
		case X86Asm::JGE:
			output << "jge"; break;
		case X86Asm::RET:
			output << "ret"; break;
		case X86Asm::PUSH:
			output << "push"; break;
		case X86Asm::POP:
			output << "pop"; break;
		case X86Asm::CMP:
			output << "cmp"; break;
	}

	return output;
}