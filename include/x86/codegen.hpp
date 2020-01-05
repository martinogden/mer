#pragma once
#include <string>
#include <vector>
#include "generator.hpp"
#include "inst/inst.hpp"
#include "regalloc/regalloc.hpp"
#include "x86/asm.hpp"
#include "set.hpp"


// Emit MACH-O x86-64 asm for OS X
class X86CodeGen {
private:
	std::vector<Reg> callee_saved_regs;
	std::vector<Reg> caller_saved_regs;
	uint num_spilled;

	const InstFun& fun;
	const Alloc& alloc;
	std::vector<X86Asm> as;

	void prologue();
	void epilogue();

	void emitLabel(const std::string& label);
	void emit(X86Asm::OpCode opcode);
	void emit(X86Asm::OpCode opcode, const Operand& dst);
	void emit(X86Asm::OpCode opcode, const Operand& dst, const Operand& src);

	void visit(const Inst& inst);
	void visitLabel(const Operand& op);
	void visitSub(const Operand& dst, const Operand& src1, const Operand& src2);
	void visitDiv(const Operand& dst, const Operand& src1, const Operand& src2);
	void visitMod(const Operand& dst, const Operand& src1, const Operand& src2);
	void visitMov(const Operand& dst, const Operand& src);
	void visitJmp(const Operand& op);
	void visitRet(const Operand& op);
	void visitEnter();
	void visitCall(const Operand& dst, const Operand& name, const Operand& src);
	void visitArg(const Operand& i, const Operand& op);
	void visitBinary(Inst::OpCode opcode, const Operand& dst, const Operand& src1, const Operand& src2);
	void visitCJmp(Inst::OpCode opcode, const Operand& operand, const Operand& t, const Operand& f);

	Operand getDst(const Operand& dst);
	void storeDst(const Operand& dst);
	uint getNumSlots(uint n);

public:
	X86CodeGen(const InstFun& fun, const Alloc& alloc);
	X86Fun run();
};
