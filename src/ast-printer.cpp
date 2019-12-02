#include <sstream>
#include "expr-printer.hpp"
#include "ast-printer.hpp"
#include "print-utils.hpp"


ASTPrinter::ASTPrinter(std::vector<FunNode*>& nodes) :
	nodes(nodes)
{}


std::string ASTPrinter::run() {
	std::stringstream buf;
	for (auto const& node : nodes)
		buf << get(node) << std::endl;
	return buf.str();
}


void ASTPrinter::visit(FunNode* node) {
	std::stringstream buf;

	buf << "<(";
	std::vector<std::string> pairs;
	for (Param& param : node->params) {
		std::stringstream b;
		b << param.name << ":" << param.type;
		pairs.push_back(b.str());
	}
	buf << join(pairs, ", ");
	buf << ") -> " << node->type << ">";

	ret("fun(" + node->id + " " + buf.str() + " " + get(node->body) + ")");
}


void ASTPrinter::visit(AssignNode* node) {
	ret("assign(" + node->id + " " + get(node->expr) + ")");
}


void ASTPrinter::visit(IfNode* node) {
	ret("if(" + get(node->cond) + " " + get(node->then) + " " + get(node->otherwise) + ")");
}


void ASTPrinter::visit(WhileNode* node) {
	ret("while(" + get(node->cond) + " " + get(node->body) + ")");
}


void ASTPrinter::visit(ReturnNode* node) {
	ret("return(" + get(node->expr) + ")");
}


void ASTPrinter::visit(NopNode* node) {
	ret("()");
}


void ASTPrinter::visit(SeqNode* node) {
	ret("seq(" + get(node->head) + " " + get(node->rest) + ")");
}


void ASTPrinter::visit(DeclNode* node) {
	// TODO: move to debug
	std::string type;
	switch (node->type) {
		case Type::BOOL:
			type = "bool";
			break;
		case Type::ERROR:
			type = "error";
			break;
		case Type::INT:
			type = "int";
			break;
		case Type::VOID:
			type = "void";
			break;
		case Type::UNKNOWN:
			type = "unknown";
			break;
	}

	ret("declare(" + node->id + " " + type + " " + get(node->scope) + ")");
}


void ASTPrinter::visit(ExprNode* node) {
	ret( get(node->expr) );
}



void ASTPrinter::ret(std::string s) {
	retval = std::move(s);
}


std::string ASTPrinter::get(ASTNode* node) {
	node->accept(*this);
	std::string s = retval;
	retval = "";
	return s;
}


std::string ASTPrinter::get(Expr* expr) {
	ExprPrinter printer(expr);
	return printer.run();
}
