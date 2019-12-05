#include "x86/codegen.hpp"
#include "conversion.hpp"


X86CodeGen::X86CodeGen(InstFun& fun, Alloc& alloc, Set<Reg>& usedRegs) :
	fun(fun),
	alloc(alloc),
	calleeSavedRegs(usedRegs & calleeSaved),
	callerSavedRegs(usedRegs & callerSaved)
{}


X86Fun X86CodeGen::run() {
	prologue();

	for (auto& inst : fun.insts)
		visit(inst);

	epilogue();

	return {as, std::move(fun.params)};
}


void X86CodeGen::emitLabel(const std::string& label) {
	as.emplace_back(label);
}


void X86CodeGen::emit(X86Asm::OpCode opcode) {
	as.emplace_back(opcode);
}


void X86CodeGen::emit(X86Asm::OpCode opcode, const Operand& dst) {
	as.emplace_back( opcode, dst );
}


void X86CodeGen::emit(X86Asm::OpCode opcode, const Operand& dst, const Operand& src) {
	as.emplace_back( opcode, dst, src );
}


void X86CodeGen::visit(Inst& inst) {
	switch (inst.getOpcode()) {
		case Inst::LBL:
			return visitLabel(inst.getDst());
		case Inst::ADD:
		case Inst::MUL:
		case Inst::AND:
		case Inst::OR:
		case Inst::XOR:
			return visitBinary(inst.getOpcode(), inst.getDst(), inst.getSrc1(), inst.getSrc2());
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
			return visitCJmp(inst.getOpcode(), inst.getDst(), inst.getSrc1(), inst.getSrc2());
		case Inst::RET:
			return visitRet(inst.getDst());
		case Inst::ENTER:
			return visitEnter();
		case Inst::CALL:
			return visitCall(inst.getDst(), inst.getSrc1(), inst.getSrc2());
		case Inst::ARG:
			return visitArg(inst.getDst(), inst.getSrc1());
	}
}


void X86CodeGen::prologue() {
	// setup stack frame
	emit(X86Asm::PUSH, Reg::RBP);
	emit(X86Asm::MOV, Reg::RBP, Reg::RSP);

	// save callee-saved regs
	uint n = numSlots(calleeSavedRegs);

	if (n > 0)
		emit(X86Asm::SUB, Reg::RSP, 8 * n);
	int i = 1;
	for (auto reg : calleeSavedRegs)
		emit(X86Asm::MOV, Operand(Reg::RBP, -8 * i++), reg);
}


void X86CodeGen::epilogue() {
	// restore callee saved regs
	uint n = numSlots(calleeSavedRegs);

	int i = 1;
	for (auto reg : calleeSavedRegs)
		emit(X86Asm::MOV, reg, Operand(Reg::RBP, -8 * i++));
	if (n > 0)
		emit(X86Asm::ADD, Reg::RSP, 8*n);

	// restore stack frame and return control to caller
	emit(X86Asm::POP, Reg::RBP);
	emit(X86Asm::RET);
}


void X86CodeGen::visitLabel(Operand&& op) {
	emitLabel(op.getLabel());
}


void X86CodeGen::visitSub(Operand&& dst, Operand&& src1, Operand&& src2) {
	if (src2 == dst) {
		emit(X86Asm::NEG, dst);
		emit(X86Asm::ADD, dst, src1);
	} else {
		if (src1 != dst)
			emit(X86Asm::MOV, dst, src1);
		emit(X86Asm::SUB, dst, src2);
	}
}


void X86CodeGen::visitDiv(Operand&& dst, Operand&& src1, Operand&& src2) {
	Operand t;
	if (src2.is(Operand::IMM)) {
		t = Reg::R15D;
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
		t = Reg::R15D;
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


void X86CodeGen::visitEnter() {
	int i = 1;
	for (const auto &param : fun.params) {
		Reg reg = static_cast<Reg>(i++);
		emit(X86Asm::MOV, alloc[param], reg);
	}
}


void X86CodeGen::visitCall(Operand&& dst, Operand&& name, Operand&& n) {
	// save caller-saved regs
	uint numCalleeSlots = numSlots(calleeSavedRegs);
	uint numCallerSlots = numSlots(callerSavedRegs);

	if (numCallerSlots > 0)
		emit(X86Asm::SUB, Reg::RSP, 8 * numCallerSlots);
	int i = 1;
	for (auto reg : callerSavedRegs)
			emit(X86Asm::MOV, Operand(Reg::RBP, -8 * (numCalleeSlots + i++)), reg);


	std::string label = "_" + name.getLabel();
	emit(X86Asm::CALL, Operand::label(label), n);

	i = 1;
	for (auto reg : callerSavedRegs)
		emit(X86Asm::MOV, reg, Operand(Reg::RBP, -8 * (numCalleeSlots + i++)));

	if (numCallerSlots > 0)
		emit(X86Asm::ADD, Reg::RSP, 8*numCallerSlots);

	emit( X86Asm::MOV, dst, Reg::EAX);
}


void X86CodeGen::visitArg(Operand&& i, Operand&& op) {
	Reg reg = static_cast<Reg>(i.getImm());
	emit(X86Asm::MOV, reg, op);
}


void X86CodeGen::visitBinary(Inst::OpCode opcode, Operand&& dst, Operand&& src1, Operand&& src2) {
	X86Asm::OpCode op = toOpcode(opcode);

	if ( src2 == dst ) {
		emit(op, dst, src1);
	}
	else {
		if ( src1 != dst )
			emit(X86Asm::MOV, dst, src1);
		emit(op, dst, src2);
	}
}


void X86CodeGen::visitCJmp(Inst::OpCode opcode, Operand&& operand, Operand&& t, Operand&& f) {
	X86Asm::OpCode op = toOpcode(opcode);

	emit(X86Asm::CMP, operand, 0);
	emit(op, t);
	emit(X86Asm::JMP, f);
}


uint X86CodeGen::numSlots(Set<Reg>& regs) {
	// ensure stack is 16-byte aligned
	return (regs.size() + 1) & ~1U;
}
