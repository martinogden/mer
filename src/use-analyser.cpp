#include "use-analyser.hpp"
// TODO: deprecated, remove


void UseAnalyser::visit(TernaryExpr* expr) {
	ret( get(expr->cond) | get(expr->then) | get(expr->otherwise) );
}


void UseAnalyser::visit(BinaryExpr* expr) {
	ret( get(expr->left) | get(expr->right) );
}


void UseAnalyser::visit(UnaryExpr* unary) {
	ret( get(unary->expr) );
}


void UseAnalyser::visit(LiteralExpr* expr) {
	ret({});
}


void UseAnalyser::visit(IdExpr* expr) {
	ret({{expr->identifier}});
}


/*
The dynamic use of a variable is not computable (easy cor. of
Rice's thm.), so we make a conservative static approximation
here. We say:

	var x in use in expr e if x occurs in e

e.g. use(y>0 || x>0) = {x, y}, even though x may not actually
be used at runtime

e.g.: use(true || x>0) = {x}
*/
Set<std::string> UseAnalyser::get(Expr* expr) {
	expr->accept(*this);
	Set<std::string> vars = retval;
	retval = {};
	return vars;
}


void UseAnalyser::ret(Set<std::string> vars) {
	retval = vars;
}
