#pragma once
#include <sstream>
#include <iostream>
#include <string>
#include <set>


typedef unsigned int uint;
typedef uint Imm;
typedef std::string Tmp;
typedef std::string Lbl;


/*
@link https://stackoverflow.com/a/1753627/107987

64-bit register | Lower 32 bits | Lower 16 bits | Lower 8 bits
==============================================================
rax             | eax           | ax            | al
rbx             | ebx           | bx            | bl
rcx             | ecx           | cx            | cl
rdx             | edx           | dx            | dl
rsi             | esi           | si            | sil
rdi             | edi           | di            | dil
rbp             | ebp           | bp            | bpl
rsp             | esp           | sp            | spl
r8              | r8d           | r8w           | r8b
r9              | r9d           | r9w           | r9b
r10             | r10d          | r10w          | r10b
r11             | r11d          | r11w          | r11b
r12             | r12d          | r12w          | r12b
r13             | r13d          | r13w          | r13b
r14             | r14d          | r14w          | r14b
r15             | r15d          | r15w          | r15b
 */
enum class Reg {
	EAX, EDI, ESI, EDX, ECX, R8D, R9D, R10D, R11D,  // caller-saved
	EBX, R12D, R13D, R14D, R15D, RSP, RBP, // callee-saved
};

std::ostream& operator<<(std::ostream&, const Reg&);


struct Mem {
	Reg reg;
	int offset;
	Mem(Reg reg, int offset) : reg(reg), offset(offset) {}
};


struct Operand {
public:
	enum Type {
		TMP,
		IMM,
		REG,
		MEM,
		LBL,
	};

	Operand() {}  // TODO: get rid of this
	Operand(Tmp tmp) : Operand(TMP, tmp) {}
	Operand(Imm imm) : type(IMM), imm(imm) {}
	Operand(Reg reg) : type(REG), reg(reg) {}
	Operand(Reg reg, int off) : type(MEM), mem({reg, off}) {}

	static Operand label(Lbl lbl) {
		return Operand(LBL, lbl);
	}

	Type getType() const {
		return type;
	}

	bool is(Type ty) const {
		return ty == type;
	}

	std::string getLabel() const {
		assert(type == LBL);
		return tmp;
	}

	std::string getTmp() const {
		assert(type == TMP);
		return tmp;
	}

	uint getImm() const {
		assert(type == IMM);
		return imm;
	}

	Reg getReg() const {
		assert(type == REG);
		return reg;
	}

	friend bool operator==(Operand a, Operand b) {
		if (a.getType() != b.getType())
			return false;

		return a.to_string() == b.to_string();
	}

	friend bool operator!=(Operand a, Operand b) {
		return !(a == b);
	}

	friend std::ostream& operator<<(std::ostream&, const Operand&);

	std::string to_string() const {
		switch (type) {
			case Operand::LBL:
				return getLabel();
			case Operand::IMM:
				return "$" + std::to_string(getImm());
			case Operand::REG: {
				std::stringstream buf;
				buf << getReg();
				return buf.str();
			}
			case Operand::TMP:
				return getTmp();
			case Operand::MEM:
				throw 1;  // TODO
		}
	}

private:
	Operand(Type type, std::string tmp) : type(type), tmp(tmp) {}

	Type type;
	Tmp tmp;
	union {
		Imm imm;
		Reg reg;
		Mem mem;
	};
};


namespace std {

// https://marknelson.us/posts/2011/09/03/hash-functions-for-c-unordered-containers.html
template <>
struct hash<Operand>{
public :
	size_t operator()(const Operand &op) const {
		uint type = static_cast<uint>( op.getType() );
		return hash<uint>()(type) ^ hash<std::string>()(op.to_string());
	}
};

};


struct Inst {
public:
	enum OpCode {
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,

		AND,
		OR,
		XOR,

		MOV,

		LBL,
		JMP,
		JEQ,
		JNE,
		JLT,
		JLE,
		JGT,
		JGE,

		RET,
	};

	enum class Type {
		LABEL,
		BINARY,
		UNARY,
		RET,
		JMP,
		CJMP,
	};

	Inst(OpCode opcode, Operand dst)
		: parity(1), opcode(opcode), dst(dst), src1(0), src2(0) {}
	Inst(OpCode opcode, Operand dst, Operand src1)
		: parity(2), opcode(opcode), dst(dst), src1(src1), src2(0) {}
	Inst(OpCode opcode, Operand dst, Operand src1, Operand src2)
		: parity(3), opcode(opcode), dst(dst), src1(src1), src2(src2) {}

	Type getType() {
		switch (getOpcode()) {
			case ADD:
			case SUB:
			case MUL:
			case DIV:
			case MOD:
			case AND:
			case OR:
			case XOR:
				return Type::BINARY;
			case MOV:
				return Type::UNARY;
			case RET:
				return Type::RET;
			case JMP:
				return Type::JMP;
			case JEQ:
			case JNE:
			case JLT:
			case JLE:
			case JGT:
			case JGE:
				return Type::CJMP;
			case LBL:
				return Type::LABEL;
		}
	}

	uint getParity() const {
		return parity;
	}

	OpCode getOpcode() const {
		return opcode;
	}

	Operand getDst() const {
		if (parity > 0)
			return dst;
		throw 1;  // TODO: handle properly
	}

	Operand getSrc1() const {
		if (parity > 1)
			return src1;
		throw 1;  // TODO: handle properly
	}

	Operand getSrc2() const {
		if (parity > 2)
			return src2;
		throw 1;  // TODO: handle properly
	}

	bool is(OpCode op) const {
		return op == opcode;
	}

	// TODO: hash opcodes so we can use unordered_set here
	bool is(std::set<OpCode> ops) const {
		return ops.find(opcode) != ops.end();
	}

	friend std::ostream& operator<<(std::ostream&, const Inst&);

private:
	uint parity;
	OpCode opcode;
	Operand dst;
	Operand src1;
	Operand src2;
};
