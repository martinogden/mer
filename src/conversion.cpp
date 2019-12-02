#include "conversion.hpp"


UnOp toUnOp(TokenType type) {
	switch(type) {
		case TokenType::BANG:
			return UnOp::LOG_NOT;
		case TokenType::TILDE:
			return UnOp::BIT_NOT;
		case TokenType::SUB:
			return UnOp::NEG;
		default:
			throw 1;  // TODO: we should never get here
	}
}


BinOp toBinOp(TokenType type) {
	switch(type) {
		// arithmetic
		case TokenType::ADD:
			return BinOp::ADD;
		case TokenType::SUB:
			return BinOp::SUB;
		case TokenType::MUL:
			return BinOp::MUL;
		case TokenType::DIV:
			return BinOp::DIV;
		case TokenType::MOD:
			return BinOp::MOD;

		// comparision
		case TokenType::LT:
			return BinOp::LT;
		case TokenType::LT_EQL:
			return BinOp::LT_EQL;
		case TokenType::GT:
			return BinOp::GT;
		case TokenType::GT_EQL:
			return BinOp::GT_EQL;
		case TokenType::EQL_EQL:
			return BinOp::EQL;
		case TokenType::BANG_EQL:
			return BinOp::NOT_EQL;

		// logic
		case TokenType::AMP_AMP:
			return BinOp::LOG_AND;
		case TokenType::PIPE_PIPE:
			return BinOp::LOG_OR;

		// bitwise
		case TokenType::AMP:
			return BinOp::BIT_AND;
		case TokenType::PIPE:
			return BinOp::BIT_OR;
		case TokenType::CARET:
			return BinOp::XOR;

		default:
			throw 1;  // TODO: we should never get here
	}
}



BinOp asnOpToBinOp(TokenType type) {
	switch(type) {
		// arithmetic
		case TokenType::EQL:
			return BinOp::EQL;
		case TokenType::ADD_EQL:
			return BinOp::ADD;
		case TokenType::SUB_EQL:
			return BinOp::SUB;
		case TokenType::MUL_EQL:
			return BinOp::MUL;
		case TokenType::DIV_EQL:
			return BinOp::DIV;
		case TokenType::MOD_EQL:
			return BinOp::MOD;

		// bitwise
		case TokenType::AMP_EQL:
			return BinOp::BIT_AND;
		case TokenType::PIPE_EQL:
			return BinOp::BIT_OR;
		case TokenType::CARET_EQL:
			return BinOp::XOR;

		default:
			throw 1;  // TODO: we should never get here
	}
}


BinOp postOpToBinOp(TokenType type) {
	switch(type) {
		case TokenType::ADD_ADD:
			return BinOp::ADD;
		case TokenType::SUB_SUB:
			return BinOp::SUB;
		default:
			throw 1;  // TODO: we should never get here
	}
}


Inst::OpCode toOpCode(BinOp op) {
	switch (op) {
		case BinOp::ADD:
			return Inst::ADD;
		case BinOp::SUB:
			return Inst::SUB;
		case BinOp::MUL:
			return Inst::MUL;
		case BinOp::DIV:
			return Inst::DIV;
		case BinOp::MOD:
			return Inst::MOD;

		case BinOp::BIT_AND:
			return Inst::AND;
		case BinOp::BIT_OR:
			return Inst::OR;
		case BinOp::XOR:
			return Inst::XOR;

		case BinOp::EQL:
			return Inst::JEQ;
		case BinOp::NOT_EQL:
			return Inst::JNE;
		case BinOp::LT:
			return Inst::JLT;
		case BinOp::LT_EQL:
			return Inst::JLE;
		case BinOp::GT:
			return Inst::JGT;
		case BinOp::GT_EQL:
			return Inst::JGE;

		case BinOp::LOG_AND:
		case BinOp::LOG_OR:
			throw 1;  // TODO: we should never get here
	}
}


X86Asm::OpCode toOpcode(Inst::OpCode opcode) {
	switch (opcode) {
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
		case Inst::CALL:
			return X86Asm::CALL;
		case Inst::ARG:
			return X86Asm::CALL;  // !TODO fix
		case Inst::LBL:
			return X86Asm::LBL;
	}
};
