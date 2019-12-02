#include "x86/codegen.hpp"
#include "conversion.hpp"


X86CodeGen::X86CodeGen(InstFun& fun, Generator& gen) :
	fun(fun),
	gen(gen)
{}


void X86CodeGen::emitLabel(const std::string& label) {
	as.emplace_back(label);
}


void X86CodeGen::emit(X86Asm::OpCode opcode) {
	as.push_back(opcode);
}


	as.push_back({ opcode, dst });
void X86CodeGen::emit(X86Asm::OpCode opcode, const Operand& dst) {
}


	as.push_back({ opcode, dst, src });
void X86CodeGen::emit(X86Asm::OpCode opcode, const Operand& dst, const Operand& src) {
}


void X86CodeGen::prologue() {
	int i = 1;
	for (const auto &param : fun.params) {
		Reg reg = static_cast<Reg>(i++);
		emit(X86Asm::MOV, param, reg);
	}
}


void X86CodeGen::epilogue() {
}


void X86CodeGen::visit(Inst& inst) {
	X86Asm::OpCode opcode = toOpcode(inst.getOpcode());

	switch (inst.getOpcode()) {
		case Inst::LBL:
			return visitLabel(inst.getDst());
		case Inst::ADD:
		case Inst::MUL:
		case Inst::AND:
		case Inst::OR:
		case Inst::XOR:
			return visitBinary(opcode, inst.getDst(), inst.getSrc1(), inst.getSrc2());
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
			return visitCJmp(opcode, inst.getDst(), inst.getSrc1(), inst.getSrc2());
		case Inst::RET:
			return visitRet(inst.getDst());
		case Inst::CALL:
			return visitCall(inst.getDst(), inst.getSrc1());
		case Inst::ARG:
			return visitParam(inst.getDst(), inst.getSrc1());

	}
}


void X86CodeGen::visitLabel(Operand&& op) {
	emitLabel(op.getLabel());
}


void X86CodeGen::visitSub(Operand&& dst, Operand&& src1, Operand&& src2) {
	if ( src2 == dst ) {
		emit(X86Asm::NEG, dst);
		emit(X86Asm::ADD, dst, src1);
	}
	else {
		if ( src1 != dst )
			emit(X86Asm::MOV, dst, src1);
		emit(X86Asm::SUB, dst, src2);
	}
}


void X86CodeGen::visitDiv(Operand&& dst, Operand&& src1, Operand&& src2) {
	Operand t;
	if (src2.is(Operand::IMM)) {
		t = gen.tmp();
		emit(X86Asm::MOV, t, src2);
	}
	else
		t = src2;

	emit(X86Asm::MOV, Reg::EAX, src1);
	emit(X86Asm::CDQ);  // must zero out EDX
	emit(X86Asm::IDIV, t);  // EAX -> EAX / src2, EDX -> EAX % src2
	emit(X86Asm::MOV, dst, Reg::EAX);
}


void X86CodeGen::visitMod(Operand&& dst, Operand&& src1, Operand&& src2) {
	Operand t;
	if (src2.is(Operand::IMM)) {
		t = gen.tmp();
		emit(X86Asm::MOV, t, src2);
	}
	else
		t = src2;

	emit(X86Asm::MOV, Reg::EAX, src1);
	emit(X86Asm::CDQ);  // must zero out EDX
	emit(X86Asm::IDIV, t);  // EAX -> EAX / src2, EDX -> EAX % src2
	emit(X86Asm::MOV, dst, Reg::EDX);
}


void X86CodeGen::visitMov(Operand&& dst, Operand&& src) {
	emit(X86Asm::MOV, dst, src);
}


void X86CodeGen::visitJmp(Operand&& op) {
	emit(X86Asm::JMP, op);
}


void X86CodeGen::visitRet(Operand&& op) {
	emit(X86Asm::MOV, Reg::EAX, op);
}


void X86CodeGen::visitCall(Operand&& name, Operand&& n) {
	std::string label = "_" + name.getLabel();
	emit(X86Asm::CALL, Operand::label(label), n);
}


void X86CodeGen::visitParam(Operand&& i, Operand&& op) {
	Reg reg = static_cast<Reg>(i.getImm());
	emit(X86Asm::MOV, reg, op);
}


void X86CodeGen::visitBinary(X86Asm::OpCode opcode, Operand&& dst, Operand&& src1, Operand&& src2) {
	if ( src2 == dst ) {
		emit(opcode, dst, src1);
	}
	else {
		if ( src1 != dst )
			emit(X86Asm::MOV, dst, src1);
		emit(opcode, dst, src2);
	}
}


void X86CodeGen::visitCJmp(X86Asm::OpCode opcode, Operand&& op, Operand&& t, Operand&& f) {
	emit(X86Asm::CMP, op, 0);
	emit(opcode, t);
	emit(X86Asm::JMP, f);
}


std::vector<X86Asm>& X86CodeGen::run() {
	prologue();

	for (auto& inst : fun.insts)
		visit(inst);

	epilogue();

	return as;
}
