#include <sstream>
#include "expr/printer.hpp"
#include "ast/printer.hpp"
#include "print-utils.hpp"


void ASTPrinter::visit(FunNode& node) {
	std::vector<std::string> domain;

	for (uint i=0; i<node.params.size(); ++i)
		domain.push_back( node.params[i].name + ":" + get(node.type->domain[i]) );

	std::string sig = " <(" + join(domain, ", ") + ") -> " + get(node.type->codomain) + "> ";
	ret("fun(" + node.id + sig + get(node.body) + ")");
}


void ASTPrinter::visit(AssignNode& node) {
	ret("assign(" + get(node.lvalue) + " " + get(node.rvalue) + ")");
}


void ASTPrinter::visit(IfNode& node) {
	ret("if(" + get(node.cond) + " " + get(node.then) + " " + get(node.otherwise) + ")");
}


void ASTPrinter::visit(WhileNode& node) {
	ret("while(" + get(node.cond) + " " + get(node.body) + ")");
}


void ASTPrinter::visit(ReturnNode& node) {
	ret("return(" + get(node.expr) + ")");
}


void ASTPrinter::visit(NopNode& node) {
	ret("()");
}


void ASTPrinter::visit(SeqNode& node) {
	ret("seq(" + get(node.head) + " " + get(node.rest) + ")");
}


void ASTPrinter::visit(DeclNode& node) {
	ret("declare(" + node.id + " <" + get(node.type) + "> " + get(node.scope) + ")");
}


void ASTPrinter::visit(ExprNode& node) {
	ret( get(node.expr) );
}



void ASTPrinter::ret(std::string s) {
	retval = std::move(s);
}


std::string ASTPrinter::get(FunNodePtr& node) {
	node->accept(*this);
	return retval;
}


std::string ASTPrinter::get(ASTNodePtr& node) {
	node->accept(*this);
	return retval;
}


std::string ASTPrinter::get(ExprPtr& expr) {
	ExprPrinter printer;
	return printer.get(expr);
}


std::string ASTPrinter::get(TypePtr& type) {
	TypePrinter printer;
	return printer.get(type);
}
