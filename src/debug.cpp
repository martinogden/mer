#include <iomanip>
#include <sstream>
#include "token.hpp"
#include "ast.hpp"
#include "operator.hpp"
#include "ir.hpp"
#include "graph.hpp"


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
		case Reg::EAX:
			name = "eax";
			break;
		case Reg::EBX:
			name = "ebx";
			break;
		case Reg::ECX:
			name = "ecx";
			break;
		case Reg::EDX:
			name = "edx";
			break;
		case Reg::ESI:
			name = "esi";
			break;
		case Reg::EDI:
			name = "edi";
			break;

		case Reg::R8D:
			name = "r8d";
			break;
		case Reg::R9D:
			name = "r9d";
			break;
		case Reg::R10D:
			name = "r10d";
			break;
		case Reg::R11D:
			name = "r11d";
			break;
		case Reg::R12D:
			name = "r12d";
			break;
		case Reg::R13D:
			name = "r13d";
			break;
		case Reg::R14D:
			name = "r14d";
			break;
		case Reg::R15D:
			name = "r15d";
			break;

		case Reg::RSP:
			name = "rsp";
			break;
		case Reg::RBP:
			name = "rbp";
			break;
	}

	output << name;
	return output;
}


std::ostream& operator<<(std::ostream& output, const OpCode& opcode) {
	switch (opcode) {
		case OpCode::ADD:
			output << "add";
			break;
		case OpCode::SUB:
			output << "sub";
			break;

		case OpCode::MUL:
			output << "mul";
			break;

		case OpCode::DIV:
			output << "div";
			break;

		case OpCode::MOD:
			output << "mod";
			break;

		case OpCode::MOV:
			output << "mov";
			break;

		case OpCode::RET:
			output << "ret";
			break;
	}

	return output;
}


std::ostream& operator<<(std::ostream& output, const Operand& op) {

	switch (op.type) {
		case OpType::REG:
			output << '%' << static_cast<Reg>(op.value);
			break;
		case OpType::IMM:
			output << '$' << op.value;
			break;
		case OpType::TMP:
			output << '#' << op.value;
			break;
	}

	return output;
}


std::ostream& operator<<(std::ostream& output, const Inst& inst) {
	output << inst.opcode;
	if (inst.arity == 2)
		output << " " << inst.s1 << " -> " << inst.dst;
	if (inst.arity == 3)
		output << " " << inst.s1 << ", " << inst.s2 << " -> " << inst.dst;

	return output;
}


std::ostream& operator<<(std::ostream& output, const Graph& G) {
	output << "graph G {" << std::endl;

	for (auto& adj : G.adj)
		output << "\t" << adj.first << std::endl;

	for (auto& adj : G.adj) {
		Vertex u = adj.first;

		for (Vertex v : adj.second) {
			if (u < v)
				output << "\t" << u << " -- " << v << std::endl;
		}
	}

	output << "}";

	return output;
}


std::string toHexColor(uint c) {
	std::stringstream output;
	output << std::setfill('0') << std::setw(2) << std::hex << (202*c)%255;
	output << std::setfill('0') << std::setw(2) << std::hex << (157*c)%255;
	output << std::setfill('0') << std::setw(2) << std::hex << (3*c)%255;
	return output.str();
}


std::string printGraph(Graph* G, std::unordered_map<Vertex, uint>& coloring) {
	std::stringstream output;
	output << "graph G {" << std::endl;

	std::set<Vertex> V = G->getVertices();

	for (uint u : V) {
		uint c = coloring[u];
		output << u << " [fillcolor=\"#" << toHexColor(c+1);
		output << "\",label=\"";
		if (u < 16)
			output << static_cast<Reg>(u);
		else
			output << u;
		output << "//" << c << "\",style=filled]" << std::endl;
	}

	for (uint u : V) {
		for (uint v : G->getNeighbors(u)) {
			if (u < v)
				output << "\t" << u << " -- " << v << std::endl;
		}
	}
	output << "}";

	return output.str();
}