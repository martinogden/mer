#pragma once
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "operand.hpp"
#include "set.hpp"
#include "generator.hpp"


// std::set<Reg> calleeSaved = {
// 	Reg::EBX, Reg::R12D, Reg::R13D, Reg::R14D,
// 	Reg::R15D, Reg::RSP,  Reg::RBP
// };


struct X86Asm {
	enum OpCode {
		LBL,

		NEG,
		ADD, SUB, IMUL,
		IDIV, MOD, CDQ,
		AND, OR, XOR,
		MOV,
		JMP, JE, JNE, JL, JLE, JG, JGE,
		RET,
		PUSH, POP,
		CMP,
	};

	OpCode opcode;
	Operand dst;
	Operand src;
	uint parity;

	X86Asm(std::string label)
		: opcode(LBL), dst(label), parity(1) {}
	X86Asm(OpCode opcode)
		: opcode(opcode), parity(0) {}
	X86Asm(OpCode opcode, Operand dst)
		: opcode(opcode), dst(dst), parity(1) {}
	X86Asm(OpCode opcode, Operand dst, Operand src)
		: opcode(opcode), dst(dst), src(src), parity(2) {}

	friend std::ostream& operator<<(std::ostream& output, const X86Asm& as) {
		if (as.opcode == LBL) {
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

	friend std::ostream& operator<<(std::ostream& output, const OpCode& op) {
		switch (op) {
			case LBL:
				output << "<label>"; break;
			case NEG:
				output << "neg"; break;
			case ADD:
				output << "add"; break;
			case SUB:
				output << "sub"; break;
			case IMUL:
				output << "imul"; break;
			case IDIV:
				output << "idiv"; break;
			case MOD:
				output << "mod"; break;
			case CDQ:
				output << "cdq"; break;
			case AND:
				output << "and"; break;
			case OR:
				output << "or"; break;
			case XOR:
				output << "xor"; break;
			case MOV:
				output << "mov"; break;
			case JMP:
				output << "jmp"; break;
			case JE:
				output << "je"; break;
			case JNE:
				output << "jne"; break;
			case JL:
				output << "jl"; break;
			case JLE:
				output << "jle"; break;
			case JG:
				output << "jg"; break;
			case JGE:
				output << "jge"; break;
			case RET:
				output << "ret"; break;
			case PUSH:
				output << "push"; break;
			case POP:
				output << "pop"; break;
			case CMP:
				output << "cmp"; break;
		}

		return output;
	}
};


// Emit MACH-O x86-64 asm for OS X
class X86CodeGen {
private:
	std::vector<Inst>& insts;
	std::vector<X86Asm> as;
	Generator& gen;

	void emitLabel(std::string label) {
		as.push_back(label);
	}

	void emit(X86Asm::OpCode opcode) {
		as.push_back(opcode);
	}

	void emit(X86Asm::OpCode opcode, Operand dst) {
		as.push_back({ opcode, dst });
	}

	void emit(X86Asm::OpCode opcode, Operand dst, Operand src) {
		as.push_back({ opcode, dst, src });
	}

	void prologue() {
		emit(X86Asm::PUSH, Reg::RBP);
		emit(X86Asm::MOV, Reg::RBP, Reg::RSP);

		// TODO calculate used regs and push any callee saved regs on to stack
	}

	void visit(Inst& inst) {
		X86Asm::OpCode opcode = toOpcode(inst);

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
		}
	}

	void visitLabel(Operand&& op) {
		emitLabel(op.getLabel());
	}

	void visitSub(Operand&& dst, Operand&& src1, Operand&& src2) {
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

	void visitDiv(Operand&& dst, Operand&& src1, Operand&& src2) {
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

	void visitMod(Operand&& dst, Operand&& src1, Operand&& src2) {
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

	void visitMov(Operand&& dst, Operand&& src) {
		emit(X86Asm::MOV, dst, src);
	}

	void visitJmp(Operand&& op) {
		emit(X86Asm::JMP, op);
	}

	void visitRet(Operand&& op) {
		// TODO: restore saved regs
		// restore stack pointer
		emit(X86Asm::MOV, Reg::EAX, op);
		emit(X86Asm::POP, Reg::RBP);
		emit(X86Asm::RET);
	}

	void visitBinary(X86Asm::OpCode opcode, Operand&& dst, Operand&& src1, Operand&& src2) {
		if ( src2 == dst ) {
			emit(opcode, dst, src1);
		}
		else {
			if ( src1 != dst )
				emit(X86Asm::MOV, dst, src1);
			emit(opcode, dst, src2);
		}
	}

	void visitCJmp(X86Asm::OpCode opcode, Operand&& op, Operand&& t, Operand&& f) {
		emit(X86Asm::CMP, op, 0);
		emit(opcode, t);
		emit(X86Asm::JMP, f);
	}

	X86Asm::OpCode toOpcode(Inst& inst) {
		switch (inst.getOpcode()) {
			case Inst::ADD:
				return X86Asm::ADD;
			case Inst::SUB:
				return X86Asm::SUB;
			case Inst::MUL:
				return X86Asm::IMUL;
			case Inst::DIV:
			case Inst::MOD:
				return X86Asm::IDIV;

			case Inst::AND:
				return X86Asm::AND;
			case Inst::OR:
				return X86Asm::OR;
			case Inst::XOR:
				return X86Asm::XOR;

			case Inst::MOV:
				return X86Asm::MOV;

			case Inst::JMP:
				return X86Asm::JMP;
			case Inst::JEQ:
				return X86Asm::JE;
			case Inst::JNE:
				return X86Asm::JNE;
			case Inst::JLT:
				return X86Asm::JL;
			case Inst::JLE:
				return X86Asm::JLE;
			case Inst::JGT:
				return X86Asm::JG;
			case Inst::JGE:
				return X86Asm::JGE;

			case Inst::RET:
				return X86Asm::RET;
			case Inst::LBL:
				return X86Asm::LBL;
		}
	};

public:
	X86CodeGen(std::vector<Inst>& insts, Generator& gen)
		: insts(insts), gen(gen) {}

	std::vector<X86Asm>& run() {
		prologue();

		for (auto& inst : insts)
			visit(inst);

		return as;
	}
};


class DefUseAnalyser {
private:
	const uint n;

	std::vector<X86Asm>& code;
	std::vector<Set<Operand>> def;
	std::vector<Set<Operand>> use;
	std::vector<Set<uint>> succ;
	std::unordered_map<std::string, uint> line;

	void visit(X86Asm& as, uint l) {
		succ[l] = {{ nextLine(l) }};

		switch (as.opcode) {
			case X86Asm::LBL:
				break;

			case X86Asm::NEG:
				setDef(l, {{ as.dst }});
				setUse(l, {{ as.dst }});
				break;
			case X86Asm::ADD:
			case X86Asm::SUB:
			case X86Asm::IMUL:
			case X86Asm::AND:
			case X86Asm::OR:
			case X86Asm::XOR:
				setDef(l, {{ as.dst }});
				setUse(l, {{ as.dst, as.src }});
				break;
			case X86Asm::IDIV:
			case X86Asm::MOD:
				setDef(l, {{ Reg::EAX, Reg::EDX }});
				setUse(l, {{ Reg::EAX, Reg::EDX, as.dst }});
				break;
			case X86Asm::CDQ:
				setDef(l, {{ Reg::EDX }});
				break;
			case X86Asm::MOV:
				setDef(l, {{ as.dst }});
				setUse(l, {{ as.src }});
				break;
			case X86Asm::JMP:
				succ[l] = succ[l] | Set<uint>({ label2Line(as.dst) });
				break;
			case X86Asm::JE:
			case X86Asm::JNE:
			case X86Asm::JL:
			case X86Asm::JLE:
			case X86Asm::JG:
			case X86Asm::JGE:
				setUse(l, {{ as.dst }});
				succ[l] = succ[l] | Set<uint>({ label2Line(as.dst) });
				break;
			case X86Asm::RET:
				setUse(l, {{ Reg::EAX }});
				break;
			case X86Asm::PUSH:
				setDef(l, {{ Reg::RSP }});
				setUse(l, {{ Reg::RSP }});
				break;
			case X86Asm::POP:
				setDef(l, {{ Reg::RSP, as.dst }});
				setUse(l, {{ Reg::RSP }});
				break;
			case X86Asm::CMP:
				setUse(l, {{ as.dst, as.src }});
				break;
		}
	}

	void setDef(uint l, Set<Operand>&& ops) {
		std::unordered_set<Operand> valid;

		for (auto& op : ops) {
			if (op.is(Operand::REG) || op.is(Operand::TMP))
				valid.insert(op);
		}

		def[l] = std::move(valid);
	}

	void setUse(uint l, Set<Operand>&& ops) {
		std::unordered_set<Operand> valid;

		for (auto& op : ops) {
			if (op.is(Operand::REG) || op.is(Operand::TMP))
				valid.insert(op);
		}

		use[l] = std::move(valid);
	}

	uint label2Line(Operand& op) {
		assert(op.is(Operand::LBL));
		assert(line.find(op.to_string()) != line.end());
		return line[op.to_string()];
	}

	uint nextLine(uint i) {
		while (++i < n) {
			if (code[i].opcode != X86Asm::LBL)
				break;
		}
		return i;
	}

public:
	DefUseAnalyser(std::vector<X86Asm>& code)
		: n(code.size()), code(code), def(n), use(n), succ(n) {}

	void run() {
		loadLabels();

		uint l = 0;
		for (auto& as : code)
			visit(as, l++);
	}

	void loadLabels() {
		uint l = 0;
		for(auto& as : code) {
			if (as.opcode == X86Asm::LBL)
				line[as.dst.to_string()] = l;
			l++;
		}
	}

	Set<Operand>& getDef(uint l) {
		assert(l < n);
		return def[l];
	}

	Set<Operand>& getUse(uint l) {
		assert(l < n);
		return use[l];
	}

	Set<uint>& getSucc(uint l) {
		assert(l < n);
		return succ[l];
	}
};


class LivenessAnalyser {
private:
	const uint n;
	std::vector<X86Asm>& code;
	DefUseAnalyser dua;
	std::vector<Set<Operand>> live;
	bool changed;

	void visit(X86Asm& as, uint l) {
		update(l, dua.getUse(l));

		for (uint i : dua.getSucc(l)) {
			if (i < n)
				update(l, live[i] - dua.getDef(l));
		}
	}

	void update(uint i, Set<Operand> opnds) {
		if ( !(opnds - live[i]).empty() ) {
			live[i] = live[i] | opnds;
			changed = true;
		}
	}

public:
	LivenessAnalyser(std::vector<X86Asm>& code)
		: n(code.size()), code(code), dua(code), live(n) {}

	void run() {
		dua.run();

		// DEBUG
		// uint i = 0;
		// for (auto& as : code) {
		// 	if (as.opcode == X86Asm::LBL)
		// 		std::cout << as << std::endl;
		// 	else
		// 		std::cout << i << ' ' << dua.getSucc(i) << std::endl;
		// 	i++;
		// }

		changed = true;
		while(changed) {
			changed = false;
			uint l = 0;
			for (auto& as : code)
				visit(as, l++);
		}
	}

	Set<Operand>& get(uint l) {
		assert(l < n);
		return live[l];
	}
};
