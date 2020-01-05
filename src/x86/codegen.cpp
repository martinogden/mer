#include "x86/codegen.hpp"
#include "conversion.hpp"


// TODO: tidy up / rename
const uint OFFSET = 9;  // index of first callee-saved reg


X86CodeGen::X86CodeGen(const InstFun& fun, const Alloc& alloc) :
	num_spilled(alloc.num_spilled),
	fun(fun),
	alloc(alloc)
{
	for (auto reg : alloc.used_regs) {
		if (callerSaved.contains(reg))
			caller_saved_regs.push_back(reg);
		if (calleeSaved.contains(reg))
			callee_saved_regs.push_back(reg);
	}
}


X86Fun X86CodeGen::run() {
	prologue();

	for (const auto& inst : fun.insts)
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
	if (opcode == X86Asm::MOV && dst == src) return;  // ignore redundant moves
	as.emplace_back( opcode, dst, src );
}


void X86CodeGen::visit(const Inst& inst) {
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


// If dst is a memory address, we instead use
// %r15d and then move the result afterwards
Operand X86CodeGen::getDst(const Operand& dst) {
	if (dst.is(Operand::MEM))
		return Reg::R15;
	else
		return dst;
}


void X86CodeGen::storeDst(const Operand& dst) {
	if (dst.is(Operand::MEM))
		emit(X86Asm::MOV, dst, Reg::R15);
}


void X86CodeGen::prologue() {
	// setup stack frame
	emit(X86Asm::PUSH, Reg::RBP);
	emit(X86Asm::MOV, Reg::RBP, Reg::RSP);

	// save callee-saved regs
	uint n = getNumSlots(callee_saved_regs.size() + num_spilled);

	if (n > 0)
		emit(X86Asm::SUB, Reg::RSP, 8 * n);

	int i = 1;
	for (auto it=callee_saved_regs.begin(); it!=callee_saved_regs.end(); ++it) {
		Operand mem = Operand(Reg::RBP, -8 * i++);
		emit(X86Asm::MOV, mem, *it);
	}
}


void X86CodeGen::epilogue() {
	// restore callee saved regs
	uint n = getNumSlots(callee_saved_regs.size() + num_spilled);

	uint i = callee_saved_regs.size();
	for (auto it=callee_saved_regs.rbegin(); it!=callee_saved_regs.rend(); ++it) {
		Operand mem = Operand(Reg::RBP, -8 * i--);
		emit(X86Asm::MOV, *it, mem);
	}

	if (n > 0)
		emit(X86Asm::ADD, Reg::RSP, 8*n);

	// restore stack frame and return control to caller
	emit(X86Asm::POP, Reg::RBP);
	emit(X86Asm::RET);
}


void X86CodeGen::visitLabel(const Operand& op) {
	emitLabel(op.getLabel());
}


void X86CodeGen::visitBinary(Inst::OpCode opcode, const Operand& dst, const Operand& src1, const Operand& src2) {
	Operand d = getDst(dst);
	Reg reg = dst.getReg();
	if ( src2.is(Operand::REG) && src2.getReg() == reg ) // assumes binop is commutative
		emit(toOpcode(opcode), d, src1);
	else if ( src1.is(Operand::REG) && src1.getReg() == reg )
		emit(toOpcode(opcode), d, src2);
	else {
		emit(X86Asm::MOV, d, src1);
		emit(toOpcode(opcode), d, src2);
	}

	storeDst(dst);
}


void X86CodeGen::visitSub(const Operand& dst, const Operand& src1, const Operand& src2) {
	Operand d = getDst(dst);
	Reg reg = dst.getReg();

	if ( src2.is(Operand::REG) && src2.getReg() == reg ) {
		emit(X86Asm::NEG, dst);
		emit(X86Asm::ADD, dst, src1);
	}
	else if ( src1.is(Operand::REG) && src1.getReg() == reg )
		emit(X86Asm::SUB, d, src2);
	else {
		emit(X86Asm::MOV, dst, src1);
		emit(X86Asm::SUB, dst, src2);
	}

	storeDst(dst);
}


void X86CodeGen::visitDiv(const Operand& dst, const Operand& src1, const Operand& src2) {
	Operand d = getDst(dst);
	Operand t;

	if (src2.is(Operand::IMM)) {
		t = Reg::R14;
		emit(X86Asm::MOV, t, src2);
	}
	else
		t = src2;

	emit(X86Asm::MOV, Reg::RAX, src1);
	emit(X86Asm::CQO);  // must zero out EDX
	emit(X86Asm::IDIV, t);  // EAX -> EAX / src2, EDX -> EAX % src2
	emit(X86Asm::MOV, dst, Reg::RAX);

	storeDst(dst);
}


void X86CodeGen::visitMod(const Operand& dst, const Operand& src1, const Operand& src2) {
	Operand d = getDst(dst);
	Operand t;

	if (src2.is(Operand::IMM)) {
		t = Reg::R14;
		emit(X86Asm::MOV, t, src2);
	}
	else
		t = src2;

	emit(X86Asm::MOV, Reg::RAX, src1);
	emit(X86Asm::CQO);  // must zero out EDX
	emit(X86Asm::IDIV, t);  // EAX -> EAX / src2, EDX -> EAX % src2
	emit(X86Asm::MOV, dst, Reg::RDX);

	storeDst(dst);
}


void X86CodeGen::visitMov(const Operand& dst, const Operand& src) {
	emit(X86Asm::MOV, dst, src);
}


void X86CodeGen::visitJmp(const Operand& op) {
	emit(X86Asm::JMP, op);
}


void X86CodeGen::visitRet(const Operand& op) {
	emit(X86Asm::MOV, Reg::RAX, op);
}


void X86CodeGen::visitEnter() {
	int i = 1;
	for (const auto &param : fun.params) {
		Reg reg = static_cast<Reg>(i++);
		emit(X86Asm::MOV, alloc.lookup(param), reg);
	}
}


void X86CodeGen::visitCall(const Operand& dst, const Operand& name, const Operand& n) {
	// save caller-saved regs
	uint offset = getNumSlots(callee_saved_regs.size() + alloc.num_spilled);
	uint num_slots = getNumSlots(caller_saved_regs.size());

	if (num_slots > 0)
		emit(X86Asm::SUB, Reg::RSP, 8 * num_slots);

	uint i = 1;
	for (auto it=caller_saved_regs.begin(); it!=caller_saved_regs.end(); ++it) {
		Operand mem = Operand(Reg::RBP, -8 * (offset + i++));
		emit(X86Asm::MOV, mem, *it);
	}

	std::string label = "_" + name.getLabel();
	emit(X86Asm::CALL, Operand::label(label), n);

	// restore caller-saved regs
	i = caller_saved_regs.size();
	for (auto it=caller_saved_regs.rbegin(); it!=caller_saved_regs.rend(); ++it) {
		Operand mem = Operand(Reg::RBP, -8 * (offset + i--));
		emit(X86Asm::MOV, *it, mem);
	}

	if (num_slots > 0)
		emit(X86Asm::ADD, Reg::RSP, 8 * num_slots);

	// move call result from %rax to dst
	emit( X86Asm::MOV, dst, Reg::RAX);
}


void X86CodeGen::visitArg(const Operand& i, const Operand& op) {
	Reg reg = static_cast<Reg>(i.getImm());
	emit(X86Asm::MOV, reg, op);
}


void X86CodeGen::visitCJmp(Inst::OpCode opcode, const Operand& operand, const Operand& t, const Operand& f) {
	X86Asm::OpCode op = toOpcode(opcode);

	emit(X86Asm::CMP, operand, 0);
	emit(op, t);
	emit(X86Asm::JMP, f);
}


uint X86CodeGen::getNumSlots(uint n) {
	// ensure stack is 16-byte aligned
	return (n + 1) & ~1U;
}
