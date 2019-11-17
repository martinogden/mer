#include <sstream>
#include "expr-printer.hpp"


ExprPrinter::ExprPrinter(Expr* expr) :
	expr(expr)
{}


std::string ExprPrinter::run() {
	assert(expr);
	return get(expr);
}


void ExprPrinter::visit(TernaryExpr* expr) {
	ret("(" + get(expr->cond) + " ? " + get(expr->then) + " : " + get(expr->otherwise) + ")");
}


void ExprPrinter::visit(BinaryExpr* expr) {
	// TODO: extract to to_string helper
	std::stringstream buf;
	buf << expr->op;
	ret("(" + buf.str() + " " + get(expr->left) + " " + get(expr->right) + ")");
}


void ExprPrinter::visit(UnaryExpr* unary) {
	// TODO: extract to to_string helper
	std::stringstream buf;
	buf << unary->op;
	ret("(" + buf.str() + " " + get(unary->expr) + ")");
}


void ExprPrinter::visit(IdExpr* expr) {
	ret(expr->identifier);
}


void ExprPrinter::visit(LiteralExpr* expr) {
	std::string s;

	switch (expr->type) {
		case Type::INT:
			s = std::to_string(expr->as.i);
			break;
		case Type::BOOL:
			s = std::to_string(expr->as.b);
			break;
		default:
			throw 1;  // should never get here
	}

	ret(s);
}


void ExprPrinter::ret(std::string s) {
	retval = s;
}


std::string ExprPrinter::get(Expr* expr) {
	expr->accept(*this);
	std::string s = retval;
	retval = "";
	return s;
}