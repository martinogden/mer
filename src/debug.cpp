#include "token.hpp"
#include "ast.hpp"
#include "operator.hpp"
#include "ir.hpp"


bool Token::operator==(const Token& other) const {
	return (
		type == other.type &&
		lexeme == other.lexeme &&
		line == other.line &&
		col == other.col
	);
}


std::ostream& operator<<(std::ostream &output, const Token &token) {
	output << '<' << token.type << " lexeme=\"" << token.lexeme;
	output << "\", value=" << token.value;
	output << ", line=" << token.line << ", col=" << token.col << ">";
	return output;
}


std::ostream& operator<<(std::ostream &output, const TokenType &type) {
	std::string str;

	switch(type) {
		case TokenType::END:
			str = "END"; break;
		case TokenType::ERROR:
			str = "ERROR"; break;

		case TokenType::IDENT:
			str = "IDENT"; break;
		case TokenType::NUM:
			str = "NUM"; break;
			str = "ASNOP"; break;
		case TokenType::RESERVED:
			str = "RESERVED"; break;

		case TokenType::ADD:
			str = "ADD"; break;
		case TokenType::SUB:
			str = "SUB"; break;
		case TokenType::MUL:
			str = "MUL"; break;
		case TokenType::DIV:
			str = "DIV"; break;
		case TokenType::MOD:
			str = "MOD"; break;
		case TokenType::EQL:
			str = "EQL"; break;

		case TokenType::ADD_EQL:
			str = "ADD_EQL"; break;
		case TokenType::SUB_EQL:
			str = "SUB_EQL"; break;
		case TokenType::MUL_EQL:
			str = "MUL_EQL"; break;
		case TokenType::DIV_EQL:
			str = "DIV_EQL"; break;
		case TokenType::MOD_EQL:
			str = "MOD_EQL"; break;


		case TokenType::SEMICOLON:
			str = "SEMICOLON"; break;
		case TokenType::LPAREN:
			str = "LPAREN"; break;
		case TokenType::RPAREN:
			str = "RPAREN"; break;
		case TokenType::LBRACE:
			str = "LBRACE"; break;
		case TokenType::RBRACE:
			str = "RBRACE"; break;

		case TokenType::RETURN:
			str = "RETURN"; break;
		case TokenType::INT:
			str = "INT"; break;
	}

	output << str;
	return output;
}


std::ostream& operator<<(std::ostream &output, const UnOp &op) {
	std::string str;

	switch(op) {
		case UnOp::NEG:
			str = "-"; break;
	}

	output << str;
	return output;
}


std::ostream& operator<<(std::ostream &output, const BinOp &op) {
	std::string str;

	switch(op) {
		case BinOp::ADD:
			str = "+"; break;
		case BinOp::SUB:
			str = "-"; break;
		case BinOp::MUL:
			str = "*"; break;
		case BinOp::DIV:
			str = "/"; break;
		case BinOp::MOD:
			str = "%"; break;
	}

	output << str;
	return output;
}


std::ostream& operator<<(std::ostream &output, const AsnOp &op) {
	std::string str;

	switch(op) {
		case AsnOp::ADD_EQL:
			str = "+="; break;
		case AsnOp::SUB_EQL:
			str = "-="; break;
		case AsnOp::MUL_EQL:
			str = "*="; break;
		case AsnOp::DIV_EQL:
			str = "/="; break;
		case AsnOp::MOD_EQL:
			str = "%="; break;
		case AsnOp::EQL:
			str = "="; break;
	}

	output << str;
	return output;
}


std::ostream& operator<<(std::ostream& output, const Reg& reg) {
	std::string name;

	switch (reg) {
		case Reg::RAX:
			name = "rax";
			break;
		case Reg::RBX:
			name = "rbx";
			break;
		case Reg::RCX:
			name = "rcx";
			break;
		case Reg::RDX:
			name = "rdx";
			break;
		case Reg::RSI:
			name = "rsi";
			break;
		case Reg::RDI:
			name = "rdi";
			break;
		case Reg::RSP:
			name = "rsp";
			break;
		case Reg::RBP:
			name = "rbp";
			break;

		case Reg::R8:
			name = "r8";
			break;
		case Reg::R9:
			name = "r9";
			break;
		case Reg::R10:
			name = "r10";
			break;
		case Reg::R11:
			name = "r1";
			break;
		case Reg::R12:
			name = "r12";
			break;
		case Reg::R13:
			name = "r13";
			break;
		case Reg::R14:
			name = "r14";
			break;
		case Reg::R15:
			name = "r15";
			break;

	}

	output << '%' << name;
	return output;
}


std::ostream& operator<<(std::ostream& output, const Operand& op) {

	switch (op.type) {
		case OpType::REG:
			output << static_cast<Reg>(op.value);
			break;
		case OpType::IMM:
			output << op.value;
			break;
		case OpType::TMP:
			output << "#" << op.value;
			break;
	}

	return output;
}


std::ostream& operator<<(std::ostream& output, const Inst& inst) {
	std::string name;
	int arity;
	char op;

	switch (inst.opcode) {
		case OpCode::ADD:
			arity = 3;
			op = '+';
			break;
		case OpCode::SUB:
			arity = 3;
			op = '-';
			break;
		case OpCode::MUL:
			arity = 3;
			op = '*';
			break;
		case OpCode::DIV:
			arity = 3;
			op = '+';
			break;
		case OpCode::MOD:
			arity = 3;
			op = '%';
			break;

		case OpCode::MOV:
			arity = 2;
			break;

		case OpCode::RET:
			arity = 0;
			output << "ret";
			break;
	}

	switch (arity) {
		case 3:
			output << inst.dst << " <- " << inst.s1 << " + " << inst.s2;
			break;
		case 2:
			output << inst.dst << " <- " << inst.s1;
			break;
		default:
			break;
	}

	return output;
}
