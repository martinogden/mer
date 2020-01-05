#include "expr/lvalue/checker.hpp"


void LValueChecker::visit(CallExpr& expr) {
	ret(false);
}


void LValueChecker::visit(TernaryExpr& expr) {
	ret(false);
}


void LValueChecker::visit(BinaryExpr& expr) {
	ret(false);
}


void LValueChecker::visit(UnaryExpr& expr) {
	ret(false);
}


void LValueChecker::visit(LiteralExpr& expr) {
	ret(false);
}


void LValueChecker::visit(IdExpr& expr) {
	ret(true);
}


void LValueChecker::visit(SubscriptExpr& expr) {
	ret( get(expr.left) );
}


void LValueChecker::visit(ArrowExpr& expr) {
	ret( get(expr.expr) );
}


void LValueChecker::visit(DotExpr& sel) {
	ret( get(sel.expr) );
}


void LValueChecker::visit(DerefExpr& ptr) {
	ret( get(ptr.expr) );
}


void LValueChecker::visit(AllocExpr& ptr) {
	ret(false);
}


void LValueChecker::ret(bool b) {
	retval = b;
}


bool LValueChecker::get(ExprPtr& expr) {
	expr->accept(*this);
	return retval;
}
