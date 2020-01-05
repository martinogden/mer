#include "expr/expr.hpp"


void ExprVisitor::visit(CallExpr& expr) {
	for (auto const& arg : expr.args)
		arg->accept(*this);
}


void ExprVisitor::visit(AllocExpr& alloc) {
	if (alloc.expr)
		alloc.expr->accept(*this);
}


void ExprVisitor::visit(TernaryExpr& expr) {
	expr.cond->accept(*this);
	expr.then->accept(*this);
	expr.otherwise->accept(*this);
}


void ExprVisitor::visit(BinaryExpr& expr) {
	expr.left->accept(*this);
	expr.right->accept(*this);
}


void ExprVisitor::visit(UnaryExpr& unary) {
	unary.expr->accept(*this);
}


void ExprVisitor::visit(LiteralExpr& expr) {}


void ExprVisitor::visit(IdExpr& expr) {}


void ExprVisitor::visit(SubscriptExpr& expr) {
	expr.left->accept(*this);
	expr.right->accept(*this);
}


void ExprVisitor::visit(ArrowExpr& expr) {
	expr.expr->accept(*this);
}


void ExprVisitor::visit(DotExpr& expr) {
	expr.expr->accept(*this);
}


void ExprVisitor::visit(DerefExpr& ptr) {
	ptr.expr->accept(*this);
}
