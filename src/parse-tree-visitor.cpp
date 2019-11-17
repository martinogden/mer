#include "parse-tree.hpp"


// The 'identity' visitor

void Visitor::visit(TernaryExpr* expr) {
	expr->cond->accept(*this);
	expr->then->accept(*this);
	expr->otherwise->accept(*this);
}


void Visitor::visit(BinaryExpr* expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
}


void Visitor::visit(UnaryExpr* unary) {
	unary->expr->accept(*this);
}


void Visitor::visit(LiteralExpr* expr) {}


void Visitor::visit(IdExpr* expr) {}


void Visitor::visit(DeclStmt* stmt) {
	stmt->expr->accept(*this);
}


void Visitor::visit(IfStmt* stmt) {
	stmt->cond->accept(*this);
	stmt->then->accept(*this);

	if (stmt->otherwise)
		stmt->otherwise->accept(*this);
}


void Visitor::visit(WhileStmt* stmt) {
	stmt->cond->accept(*this);
	stmt->body->accept(*this);
}


void Visitor::visit(ForStmt* stmt) {
	if (stmt->init)
		stmt->init->accept(*this);
	stmt->cond->accept(*this);
	if (stmt->step)
		stmt->step->accept(*this);
	stmt->body->accept(*this);
}


void Visitor::visit(ReturnStmt* stmt) {
	stmt->expr->accept(*this);
}


void Visitor::visit(BlockStmt* block) {
	for (auto stmt : block->statements)
		stmt->accept(*this);
}


void Visitor::visit(ExprStmt* stmt) {
	stmt->expr->accept(*this);
}


void Visitor::visit(AssignStmt* stmt) {
	stmt->lvalue->accept(*this);
	stmt->rvalue->accept(*this);
}


void Visitor::visit(PostOpStmt* stmt) {
	stmt->expr->accept(*this);
}

