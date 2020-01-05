#include <sstream>
#include "expr/printer.hpp"
#include "type/printer.hpp"
#include "type/comparison.hpp"
#include "print-utils.hpp"


void ExprPrinter::visit(CallExpr& expr) {
	std::string id = expr.identifier;
	std::vector<std::string> args;

	for (auto& arg : expr.args)
		args.push_back( get(arg) );

	ret("(call " + id + " " + join(args, ", ") + ")");
}


void ExprPrinter::visit(TernaryExpr& expr) {
	ret("(" + get(expr.cond) + " ? " + get(expr.then) + " : " + get(expr.otherwise) + ")");
}


void ExprPrinter::visit(BinaryExpr& expr) {
	ret("(" + to_string(expr.op) + " " + get(expr.left) + " " + get(expr.right) + ")");
}


void ExprPrinter::visit(UnaryExpr& unary) {
	ret("(" + to_string(unary.op) + " " + get(unary.expr) + ")");
}


void ExprPrinter::visit(IdExpr& expr) {
	ret(expr.identifier);
}


void ExprPrinter::visit(LiteralExpr& expr) {
	std::string s;

	if (eq(expr.type, Type::INT))
		s = std::to_string(expr.as.i);
	else if (eq(expr.type, Type::BOOL))
		s = expr.as.b ? "true" : "false";
	else if (eq(expr.type, Type::INDEF))
		s = "NULL";
	else
		throw 1;  // should never get here

	ret(s);
}


void ExprPrinter::visit(SubscriptExpr& expr) {
	ret( "(" + get(expr.left) + "[" + get(expr.right) + "])" );
}


void ExprPrinter::visit(ArrowExpr& expr) {
	ret( "(" + get(expr.expr) + "->" + expr.identifier + ")" );
}


void ExprPrinter::visit(DotExpr& expr) {
	ret( "(" + get(expr.expr) + "." + expr.identifier + ")" );
}


void ExprPrinter::visit(DerefExpr& ptr) {
	ret( "(*" + get(ptr.expr) + ")" );
}


void ExprPrinter::visit(AllocExpr& alloc) {
	std::string type = get(alloc.typeParam);

	if (alloc.expr)
		ret( "alloc_array(<" + type + ">, " + get(alloc.expr) + ")" );
	else
		ret( "alloc(<" + type + ">)" );
}


void ExprPrinter::ret(std::string s) {
	retval = std::move(s);
}


std::string ExprPrinter::get(ExprPtr& expr) {
	expr->accept(*this);
	return std::move(retval);
}


std::string ExprPrinter::get(TypePtr& type) {
	TypePrinter printer;
	return printer.get(type);
}


std::string to_string(UnOp op) {
	std::stringstream buffer;
	buffer << op;
	return buffer.str();
}


std::string to_string(BinOp op) {
	std::stringstream buffer;
	buffer << op;
	return buffer.str();
}
