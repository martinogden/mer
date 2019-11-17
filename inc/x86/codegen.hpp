#pragma once
#include <string>
#include <vector>
#include "x86/asm.hpp"
#include "generator.hpp"
#include "inst.hpp"


// Emit MACH-O x86-64 asm for OS X
class X86CodeGen {
private:
	std::vector<Inst>& insts;
	std::vector<X86Asm> as;
	Generator& gen;

	void emitLabel(std::string label);
	void emit(X86Asm::OpCode opcode);
	void emit(X86Asm::OpCode opcode, Operand dst);
	void emit(X86Asm::OpCode opcode, Operand dst, Operand src);

	void prologue();
	void visit(Inst& inst);
	void visitLabel(Operand&& op);
	void visitSub(Operand&& dst, Operand&& src1, Operand&& src2);
	void visitDiv(Operand&& dst, Operand&& src1, Operand&& src2);
	void visitMod(Operand&& dst, Operand&& src1, Operand&& src2);
	void visitMov(Operand&& dst, Operand&& src);
	void visitJmp(Operand&& op);
	void visitRet(Operand&& op);
	void visitBinary(X86Asm::OpCode opcode, Operand&& dst, Operand&& src1, Operand&& src2);
	void visitCJmp(X86Asm::OpCode opcode, Operand&& op, Operand&& t, Operand&& f);

public:
	X86CodeGen(std::vector<Inst>& insts, Generator& gen)
		: insts(insts), gen(gen) {}

	std::vector<X86Asm>& run();
};
