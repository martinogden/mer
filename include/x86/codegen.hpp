#pragma once
#include <string>
#include <vector>
#include "inst/generator.hpp"
#include "inst/inst.hpp"
#include "regalloc/regalloc.hpp"
#include "x86/asm.hpp"
#include "set.hpp"


// Emit MACH-O x86-64 asm for OS X
class X86CodeGen {
private:
	InstFun& fun;
	Alloc& alloc;
	std::vector<X86Asm> as;

	Set<Reg> calleeSavedRegs;
	Set<Reg> callerSavedRegs;

	void prologue();
	void epilogue();

	void emitLabel(const std::string& label);
	void emit(X86Asm::OpCode opcode);
	void emit(X86Asm::OpCode opcode, const Operand& dst);
	void emit(X86Asm::OpCode opcode, const Operand& dst, const Operand& src);

	void visit(Inst& inst);
	void visitLabel(Operand&& op);
	void visitSub(Operand&& dst, Operand&& src1, Operand&& src2);
	void visitDiv(Operand&& dst, Operand&& src1, Operand&& src2);
	void visitMod(Operand&& dst, Operand&& src1, Operand&& src2);
	void visitMov(Operand&& dst, Operand&& src);
	void visitJmp(Operand&& op);
	void visitRet(Operand&& op);
	void visitEnter();
	void visitCall(Operand&& dst, Operand&& name, Operand&& src);
	void visitArg(Operand&& i, Operand&& op);
	void visitBinary(Inst::OpCode opcode, Operand&& dst, Operand&& src1, Operand&& src2);
	void visitCJmp(Inst::OpCode opcode, Operand&& operand, Operand&& t, Operand&& f);

	uint numSlots(Set<Reg>& regs);

public:
	X86CodeGen(InstFun& fun, Alloc& alloc, Set<Reg>& usedRegs);
	X86Fun run();
};
