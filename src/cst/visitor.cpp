#include "cst/cst.hpp"


void CSTVisitor::visit(CallExpr& expr) {
	for (auto const& arg : expr.args)
		arg->accept(*this);
}


void CSTVisitor::visit(TernaryExpr& expr) {
	expr.cond->accept(*this);
	expr.then->accept(*this);
	expr.otherwise->accept(*this);
}


void CSTVisitor::visit(BinaryExpr& expr) {
	expr.left->accept(*this);
	expr.right->accept(*this);
}


void CSTVisitor::visit(UnaryExpr& unary) {
	unary.expr->accept(*this);
}


void CSTVisitor::visit(LiteralExpr& expr) {}


void CSTVisitor::visit(IdExpr& expr) {}


void CSTVisitor::visit(FunDecl& decl) {
	for (auto const& param : decl.params)
		param->accept(*this);
}


void CSTVisitor::visit(FunDefn& defn) {
	defn.decl->accept(*this);
	defn.body->accept(*this);
}



void CSTVisitor::visit(TypedefStmt& stmt) {}


void CSTVisitor::visit(DeclStmt& stmt) {
	stmt.expr->accept(*this);
}


void CSTVisitor::visit(IfStmt& stmt) {
	stmt.cond->accept(*this);
	stmt.then->accept(*this);

	if (stmt.otherwise)
		stmt.otherwise->accept(*this);
}


void CSTVisitor::visit(WhileStmt& stmt) {
	stmt.cond->accept(*this);
	stmt.body->accept(*this);
}


void CSTVisitor::visit(ForStmt& stmt) {
	if (stmt.init)
		stmt.init->accept(*this);
	stmt.cond->accept(*this);
	if (stmt.step)
		stmt.step->accept(*this);
	stmt.body->accept(*this);
}


void CSTVisitor::visit(ReturnStmt& stmt) {
	if (stmt.expr)
		stmt.expr->accept(*this);
}


void CSTVisitor::visit(BlockStmt& block) {
	for (auto const& stmt : block.statements)
		stmt->accept(*this);
}


void CSTVisitor::visit(ExprStmt& stmt) {
	stmt.expr->accept(*this);
}


void CSTVisitor::visit(AssignStmt& stmt) {
	stmt.lvalue->accept(*this);
	stmt.rvalue->accept(*this);
}


void CSTVisitor::visit(PostOpStmt& stmt) {
	stmt.expr->accept(*this);
}
