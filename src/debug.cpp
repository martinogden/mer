#include <iomanip>
#include <sstream>
#include "token.hpp"
#include "parse-tree.hpp"
#include "operator.hpp"
#include "graph.hpp"
#include "operand.hpp"


bool Token::operator==(const Token& other) const {
	return (
		type == other.type &&
		lexeme == other.lexeme &&
		line == other.line &&
		col == other.col
	);
}


std::ostream& operator<<(std::ostream& output, const Token& token) {
	output << '<' << token.type << " lexeme=\"" << token.lexeme;
	output << "\", value=" << token.value;
	output << ", line=" << token.line << ", col=" << token.col << ">";
	return output;
}


std::ostream& operator<<(std::ostream& output, const TokenType& type) {
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
		case TokenType::BANG:
			str = "BANG"; break;
		case TokenType::TILDE:
			str = "TILDE"; break;
		case TokenType::AMP:
			str = "AMP"; break;
		case TokenType::PIPE:
			str = "PIPE"; break;
		case TokenType::CARET:
			str = "CARET"; break;
		case TokenType::LT:
			str = "LT"; break;
		case TokenType::GT:
			str = "GT"; break;
		case TokenType::LT_LT:
			str = "LT_LT"; break;
		case TokenType::GT_GT:
			str = "GT_GT"; break;
		case TokenType::AMP_AMP:
			str = "AMP_AMP"; break;
		case TokenType::PIPE_PIPE:
			str = "PIPE_PIPE"; break;
		case TokenType::SUB_SUB:
			str = "SUB_SUB"; break;
		case TokenType::ADD_ADD:
			str = "ADD_ADD"; break;

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
		case TokenType::EQL_EQL:
			str = "EQL_EQL"; break;
		case TokenType::BANG_EQL:
			str = "BANG_EQL"; break;
		case TokenType::TILDE_EQL:
			str = "TILDE_EQL"; break;
		case TokenType::AMP_EQL:
			str = "AMP_EQL"; break;
		case TokenType::PIPE_EQL:
			str = "PIPE_EQL"; break;
		case TokenType::CARET_EQL:
			str = "CARET_EQL"; break;
		case TokenType::LT_EQL:
			str = "LT_EQL"; break;
		case TokenType::GT_EQL:
			str = "GT_EQL"; break;
		case TokenType::LT_LT_EQL:
			str = "LT_LT_EQL"; break;
		case TokenType::GT_GT_EQL:
			str = "GT_GT_EQL"; break;

		case TokenType::QUESTION:
			str = "QUESTION"; break;
		case TokenType::COLON:
			str = "COLON"; break;
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
		case TokenType::IF:
			str = "IF"; break;
		case TokenType::ELSE:
			str = "ELSE"; break;
		case TokenType::WHILE:
			str = "WHILE"; break;
		case TokenType::FOR:
			str = "FOR"; break;
		case TokenType::TRUE:
			str = "TRUE"; break;
		case TokenType::FALSE:
			str = "FALSE"; break;

		case TokenType::TYPE:
			str = "TYPE"; break;
	}

	output << str;
	return output;
}


std::ostream& operator<<(std::ostream& output, const UnOp& op) {
	std::string str;

	switch(op) {
		case UnOp::NEG:
			str = "-"; break;
		case UnOp::BIT_NOT:
			str = "~"; break;
		case UnOp::LOG_NOT:
			str = "!"; break;
	}

	output << str;
	return output;
}


std::ostream& operator<<(std::ostream& output, const BinOp& op) {
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
		case BinOp::LT:
			str = "<"; break;
		case BinOp::LT_EQL:
			str = "<="; break;
		case BinOp::GT:
			str = ">"; break;
		case BinOp::GT_EQL:
			str = ">="; break;
		case BinOp::EQL:
			str = "=="; break;
		case BinOp::NOT_EQL:
			str = "!="; break;
		case BinOp::LOG_AND:
			str = "&&"; break;
		case BinOp::LOG_OR:
			str = "||"; break;
		case BinOp::BIT_AND:
			str = "&"; break;
		case BinOp::BIT_OR:
			str = "|"; break;
		case BinOp::XOR:
			str = "^"; break;
		case BinOp::LS:
			str = "<<"; break;
		case BinOp::RS:
			str = ">>"; break;
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

	output << '%' << name;
	return output;
}


std::ostream& operator<<(std::ostream& output, const Inst::OpCode& opcode) {
	switch (opcode) {
		case Inst::ADD:
			output << "add";
			break;

		case Inst::SUB:
			output << "sub";
			break;

		case Inst::MUL:
			output << "mul";
			break;

		case Inst::DIV:
			output << "div";
			break;

		case Inst::MOD:
			output << "mod";
			break;

		case Inst::AND:
			output << "and";
			break;
		case Inst::OR:
			output << "or";
			break;
		case Inst::XOR:
			output << "xor";
			break;

		case Inst::MOV:
			output << "mov";
			break;

		case Inst::JMP:
			output << "jmp";
			break;
		case Inst::JEQ:
			output << "jeq";
			break;
		case Inst::JNE:
			output << "jne";
			break;
		case Inst::JLT:
			output << "jlt";
			break;
		case Inst::JLE:
			output << "jle";
			break;
		case Inst::JGT:
			output << "jgt";
			break;
		case Inst::JGE:
			output << "jge";
			break;

		case Inst::RET:
			output << "ret";
			break;

		case Inst::LBL:
			break;
	}

	return output;
}


// std::ostream& operator<<(std::ostream& output, const Operand& op) {

// 	switch (op.type) {
// 		case OperandType::REG:
// 			output << '%' << static_cast<Reg>(op.value);
// 			break;
// 		case OperandType::IMM:
// 			output << '$' << op.value;
// 			break;
// 		case OperandType::TMP:
// 			output << '#' << op.value;
// 			break;
// 	}

// 	return output;
// }


// std::ostream& operator<<(std::ostream& output, const Inst& inst) {
// 	output << inst.opcode;
// 	if (inst.arity == 2)
// 		output << " " << inst.s1 << " -> " << inst.dst;
// 	if (inst.arity == 3)
// 		output << " " << inst.s1 << ", " << inst.s2 << " -> " << inst.dst;

// 	return output;
// }


// template <typename T>
// std::ostream& operator<<(std::ostream& output, const Graph<T>& G) {
// 	output << "graph G {" << std::endl;

// 	for (auto& adj : G.adj)
// 		output << "\t" << adj.first << std::endl;

// 	for (auto& adj : G.adj) {
// 		T u = adj.first;

// 		for (T v : adj.second) {
// 			if (u < v)
// 				output << "\t" << u << " -- " << v << std::endl;
// 		}
// 	}

// 	output << "}";

// 	return output;
// }


std::string toHexColor(uint c) {
	std::stringstream output;
	output << std::setfill('0') << std::setw(2) << std::hex << (202*c)%255;
	output << std::setfill('0') << std::setw(2) << std::hex << (157*c)%255;
	output << std::setfill('0') << std::setw(2) << std::hex << (7*c)%255;
	return output.str();
}


std::string printGraph(Graph<Operand>* G, std::unordered_map<Operand, uint>& coloring) {
	std::stringstream output;
	output << "graph G {" << std::endl;

	std::unordered_set<Operand> V = G->getVertices();
	std::unordered_set<std::string> added;

	for (auto& u : V) {
		uint c = coloring[u];
		output << "\"" << u.to_string() << "\" [fillcolor=\"#" << toHexColor(c+1);
		output << "\",label=\"";
		output << u.to_string();
		output << "->" << static_cast<Reg>(c) << "\",style=filled]" << std::endl;
	}

	for (auto& u : V) {
		for (auto& v : G->getAdj(u)) {
			if (added.find(u.to_string() + v.to_string()) == added.end()) {
				output << "\t\"" << u.to_string() << "\" -- \"" << v.to_string() << "\"" << std::endl;
				added.insert(u.to_string() + v.to_string());
				added.insert(v.to_string() + u.to_string());
			}
		}
	}
	output << "}";

	return output.str();
}


std::ostream& operator<<(std::ostream& output, const Operand& op) {
	switch (op.type) {
		case Operand::TMP:
			output << op.tmp;
			break;
		case Operand::IMM:
			output << '$' << op.imm;
			break;
		case Operand::REG:
			output << op.reg;
			break;
		case Operand::MEM:
			output << op.mem.offset << '(' << op.mem.reg << ')';
			break;
		case Operand::LBL:
			output << op.tmp;
			break;
	}

	return output;
}


std::ostream& operator<<(std::ostream& output, const Inst& inst) {
	if (inst.is(Inst::LBL))
		output << '.' << inst.getDst() << ':';

	else if (inst.is({ Inst::JEQ, Inst::JNE, Inst::JLT, Inst::JLE, Inst::JGT, Inst::JGE }))
		output << '\t' << inst.getOpcode() << ' ' << inst.getDst()\
			<< ", " << inst.getSrc1() << ", " << inst.getSrc2();
	else {
		uint parity = inst.getParity();
		output << '\t' << inst.getOpcode();

		if (parity > 0)
			output << ' ' << inst.getDst();

		if (parity > 1)
			output << " <- " << inst.getSrc1();

		if (parity > 2)
			output << ", " << inst.getSrc2();
	}

	return output;
}