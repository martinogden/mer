#pragma once
#include <sstream>
#include <unordered_map>
#include <set>
#include <vector>
#include "operand.hpp"


const std::string TAB("\t");

std::set<Reg> calleeSaved = {
	Reg::EBX, Reg::R12D, Reg::R13D, Reg::R14D,
	Reg::R15D, Reg::RSP,  Reg::RBP
};


// Emit MACH-O x86-64 asm for OS X
class X86Generator {
private:
	std::stringstream buf;
	std::vector<Inst>& insts;
	std::unordered_map<Operand, Reg>& regs;

	Operand assign(Operand& op) {
		switch (op.getType()) {
			case Operand::MEM:
				throw 1;  // TODO
			case Operand::LBL:
			case Operand::REG:
			case Operand::IMM:
				return op;
			case Operand::TMP:
				return regs[op];
		}
	}

	void emit(std::string mnem, std::string prefix=TAB) {
		buf << prefix << mnem << std::endl;
	}

	void emit(std::string mnem, Operand dst, std::string prefix=TAB) {
		buf << prefix << mnem << " " << assign(dst) << std::endl;
	}

	void emit(std::string mnem, Operand src, Operand dst, std::string prefix=TAB) {
		// easy opt
		// if (mnem == "mov" && assign(src) == assign(dst))
		// 	return;

		buf << prefix << mnem << " " << assign(src) << ", " << assign(dst) << std::endl;
	}

	void header() {
		// OS X specific
		emit(".section __TEXT,__text", "");
		emit(".globl __c0_main", "");
		emit("__c0_main:", "");
	}

	void prologue() {
		emit("push", Reg::RBP);
		emit("mov", Reg::RSP, Reg::RBP);

		// TODO calculate used regs and push any callee saved regs on to stack
	}

	void visit(Inst& inst) {
		std::string mnem = toMnemonic( inst.getOpcode() );

		if (!inst.is(Inst::LBL)) {
			std::stringstream buf;
			buf << "\n//" << inst;
			emit(buf.str(), "");
		}

		switch (inst.getOpcode()) {
			case Inst::LBL:
				return visitLabel(inst.getDst());
			case Inst::ADD:
			case Inst::MUL:
			case Inst::AND:
			case Inst::OR:
			case Inst::XOR:
				return visitBinary(mnem, inst.getDst(), inst.getSrc1(), inst.getSrc2());
			case Inst::SUB:
				return visitSub(inst.getDst(), inst.getSrc1(), inst.getSrc2());
			case Inst::DIV:
				return visitDiv(inst.getDst(), inst.getSrc1(), inst.getSrc2());
			case Inst::MOD:
				return visitMod(inst.getDst(), inst.getSrc1(), inst.getSrc2());
			case Inst::MOV:
				return visitMov(inst.getDst(), inst.getSrc1());
			case Inst::JMP:
				return visitJmp(inst.getDst());
			case Inst::JEQ:
			case Inst::JNE:
			case Inst::JLT:
			case Inst::JLE:
			case Inst::JGT:
			case Inst::JGE:
				return visitCJmp(mnem, inst.getDst(), inst.getSrc1(), inst.getSrc2());
			case Inst::RET:
				return visitRet();
			default:
				emit("TODO: " + mnem);
				break;
		}
	}

	void visitLabel(Operand&& op) {
		emit(op.getLabel() + ":", "");
	}

	void visitSub(Operand&& dst, Operand&& src1, Operand&& src2) {
		if ( assign(src2) == assign(dst) ) {
			emit("neg", dst);
			emit("add", src1, dst);
		}
		else {
			if ( assign(src1) != assign(dst) )
				emit("mov", src1, dst);
			emit("sub", src2, dst);
		}
	}

	void visitDiv(Operand&& dst, Operand&& src1, Operand&& src2) {
		emit("mov", src1, Reg::EAX);
		emit("xor", Reg::EDX, Reg::EDX);  // must zero out EDX
		emit("idiv", src2);  // EAX -> EAX / src2, EDX -> EAX % src2
	}

	void visitMod(Operand&& dst, Operand&& src1, Operand&& src2) {
		emit("mov", src1, Reg::EAX);
		emit("xor", Reg::EDX, Reg::EDX);  // must zero out EDX
		emit("idiv", src2);  // EAX -> EAX / src2, EDX -> EAX % src2
		emit("mov", Reg::EDX, dst);
	}

	void visitMov(Operand&& dst, Operand&& src) {
		emit("mov", src, dst);
	}

	void visitJmp(Operand&& op) {
		emit("jmp", op);
	}

	void visitRet() {
		// TODO: restore saved regs
		// restore stack pointer
		emit("pop", Reg::RBP);
		emit("ret");
	}

	void visitBinary(std::string mnem, Operand&& dst, Operand&& src1, Operand&& src2) {
		if ( assign(src2) == assign(dst) ) {
			emit(mnem, src1, dst);
		}
		else {
			if ( assign(src1) != assign(dst) )
				emit("mov", src1, dst);
			emit(mnem, src2, dst);
		}
	}

	void visitCJmp(std::string mnem, Operand&& op, Operand&& t, Operand&& f) {
		emit("cmp", 0, op);
		emit(mnem, t);
		emit("jmp", f);
	}

	std::string toMnemonic(Inst::OpCode opcode) {
		switch (opcode) {
			case Inst::ADD:
				return "add";
			case Inst::SUB:
				return "sub";
			case Inst::MUL:
				return "imul";
			case Inst::DIV:
			case Inst::MOD:
				return "idivq";

			case Inst::AND:
				return "and";
			case Inst::OR:
				return "or";
			case Inst::XOR:
				return "xor";

			case Inst::MOV:
				return "mov";

			case Inst::JMP:
				return "jmp";
			case Inst::JEQ:
				return "je";
			case Inst::JNE:
				return "jne";
			case Inst::JLT:
				return "jl";
			case Inst::JLE:
				return "jle";
			case Inst::JGT:
				return "jg";
			case Inst::JGE:
				return "jge";

			case Inst::RET:
				return "ret";
			case Inst::LBL:
				return "";  // TODO: better way to handle this?
		}
	};

public:
	X86Generator(std::vector<Inst>& insts, std::unordered_map<Operand, Reg>& regs) :
		insts(insts),
		regs(regs)
	{}

	std::string run() {
		header();
		prologue();

		for (auto& inst : insts)
			visit(inst);

		return buf.str();
	}
};
