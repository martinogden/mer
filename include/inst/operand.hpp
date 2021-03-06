#pragma once
#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <set>
#include "set.hpp"


typedef unsigned int uint;
typedef int64_t Imm;
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
//	EAX, EDI, ESI, EDX, ECX, R8D, R9D, R10D, R11D,
//	EBX, R12D, R13D, R14D, R15D,
//	ESP, EBP,

	RAX, RDI, RSI, RDX, RCX, R8, R9, R10, R11,  // caller-saved
	RBX, R12, R13, R14, R15, // callee-saved
	RSP, RBP,  // stack
};

std::ostream& operator<<(std::ostream&, const Reg&);


extern Set<Reg> callerSaved;
extern Set<Reg> calleeSaved;


class Operand {
public:
	enum Type {
		TMP,
		IMM,
		REG,
		MEM,
		LBL,
	};

	Operand();  // TODO: get rid of this
	Operand(Tmp tmp);
	Operand(Imm imm);
	Operand(Reg reg);
	Operand(Tmp tmp, int offset);
	Operand(Reg reg, int offset);

	static Operand label(Lbl lbl) {
		return Operand(LBL, std::move(lbl));
	}

	Type getType() const;
	bool is(Type ty) const;
	std::string getLabel() const;
	std::string getTmp() const;
	int getImm() const;
	Reg getReg() const;
	Operand getMemOperand() const;
	int getMemOffset() const;

	friend bool operator==(Operand a, Operand b);
	friend bool operator!=(Operand a, Operand b);
	friend std::ostream& operator<<(std::ostream&, const Operand&);

	std::string to_string() const;

private:
	Operand(Type type, std::string tmp);
	int offset;
	Type memType;
	Type type;
	Tmp tmp;

	union {
		Imm imm;
		Reg reg;
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
