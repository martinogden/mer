#include "x86/asm.hpp"


X86Asm::X86Asm(std::string label) :
	opcode(LBL),
	dst(std::move(label)),
	parity(1)
{}


X86Asm::X86Asm(OpCode opcode) :
	opcode(opcode),
	parity(0)
{}


X86Asm::X86Asm(OpCode opcode, Operand dst) :
	opcode(opcode),
	dst(std::move(dst)),
	parity(1)
{}


X86Asm::X86Asm(OpCode opcode, Operand dst, Operand src) :
	opcode(opcode),
	dst(std::move(dst)),
	src(std::move(src)),
	parity(2)
{}


X86Fun::X86Fun(std::vector<X86Asm> code, std::vector<std::string> params) :
	code(std::move(code)),
	params(std::move(params))
{}


std::ostream& operator<<(std::ostream& output, const X86Asm& as) {
	switch (as.opcode) {
		case X86Asm::LBL:
			output << as.dst << ":";
			return output;
		case X86Asm::CALL:
			output << '\t' << as.opcode << " " << as.dst;
			return output;
		case X86Asm::PUSH:
		case X86Asm::POP:
			if (as.dst.getType() == Operand::REG) {
				output << '\t' << as.opcode << " " << promote(as.dst.getReg());
				return output;
			}
			else
				break;
		default:
			break;
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
		case X86Asm::MOVQ:
			output << "movq"; break;
		case X86Asm::MOVL:
			output << "movl"; break;
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
		case X86Asm::CALL:
			output << "call"; break;
		case X86Asm::PUSH:
			output << "push"; break;
		case X86Asm::POP:
			output << "pop"; break;
		case X86Asm::CMP:
			output << "cmp"; break;
	}

	return output;
}
