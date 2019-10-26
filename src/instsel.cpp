#include <sstream>
#include <unordered_map>
#include "instsel.hpp"


std::string mnemonic(OpCode opcode) {
	switch (opcode) {
		case OpCode::ADD:
			return "add";
		case OpCode::SUB:
			return "sub";
		case OpCode::MUL:
			return "imul";
		case OpCode::DIV:
			return "idiv";
		case OpCode::MOD:
			return "mod";
		case OpCode::MOV:
			return "mov";
		case OpCode::RET:
			return "ret";
	}
};


std::set<Reg> calleeSaved = {
	Reg::EBX, Reg::R12D, Reg::R13D, Reg::R14D,
	Reg::R15D, Reg::RSP,  Reg::RBP
};


std::vector<std::string> instsel(std::vector<Inst>& insts, std::set<Reg>& usedRegs) {
	std::vector<std::string> as;

	// OS X specific
	as.push_back(".section __TEXT,__text");
	as.push_back(".globl __c0_main");
	as.push_back("__c0_main:");

	// prologue
	as.push_back("push %rbp");
	as.push_back("mov %rsp, %rbp");

	std::vector<Reg> savedRegs;
	for (Reg reg : calleeSaved) {
		if (usedRegs.find(reg) != usedRegs.end()) {
			std::stringstream buf;
			savedRegs.push_back(reg);
			buf << "push " << Operand(reg);
			as.push_back(buf.str());
		}
	}

	for (Inst& inst : insts) {
		std::stringstream buf;
		std::string mnem = mnemonic(inst.opcode);
		std::string mov  = mnemonic(OpCode::MOV);

		Operand eax(Reg::EAX);
		Operand edx(Reg::EDX);

		switch (inst.opcode) {
			case OpCode::SUB:
				if (inst.s2.value == inst.dst.value) {
					buf << "neg" << " " << inst.dst << std::endl;
					buf << "add" << " " << inst.s1 << ", " << inst.dst;
				}
				else {
					// first instr not actually necc if s1 == dst
					buf << mov  << " " << inst.s1 << ", " << inst.dst << std::endl;
					buf << "sub" << " " << inst.s2 << ", " << inst.dst;
				}
				break;
			case OpCode::ADD:
			case OpCode::MUL:
				if (inst.s2.value == inst.dst.value)
					buf << mnem << " " << inst.s1 << ", " << inst.dst;
				else {
					// first instr not actually necc if s1 == dst
					buf << mov  << " " << inst.s1 << ", " << inst.dst << std::endl;
					buf << mnem << " " << inst.s2 << ", " << inst.dst;
				}
				break;
			case OpCode::DIV:
				buf << mov << " " << inst.s1 << ", " << eax << std::endl;
				buf << "cdq" << std::endl;
				buf << mnem << " " << inst.s2 << std::endl;
				buf << mov << " " << eax << ", " << inst.dst;
				break;
			case OpCode::MOD:
				buf << mov << " " << inst.s1 << ", " << eax << std::endl;
				buf << "cdq" << std::endl;
				buf << "idivl " << " " << inst.s2 << std::endl;
				buf << mov << " " << edx << ", " << inst.dst;  // only diff to div on this line
				break;
			case OpCode::MOV:
				buf << mov  << " " << inst.s1 << ", " << inst.dst;
				break;
			case OpCode::RET:
				// epilogue
				for (uint i=savedRegs.size(); i-- > 0;)
					buf << "pop " << Operand(savedRegs[i]) << std::endl;

				buf << "pop " << Operand(Reg::RBP) << std::endl;
				buf << mnem;
				break;
		}

		as.push_back(buf.str());
	}

	return as;
}
