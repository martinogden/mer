#include "ast.hpp"


// The 'identity' visitor

void Visitor::visit(BinaryExpr* expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
}


void Visitor::visit(UnaryExpr* unary) {
	unary->expr->accept(*this);
}


void Visitor::visit(LiteralExpr* expr) {}


void Visitor::visit(VarExpr* expr) {}


void Visitor::visit(DeclStmt* stmt) {
	stmt->expr->accept(*this);
}


void Visitor::visit(ReturnStmt* stmt) {
	stmt->expr->accept(*this);
}


void Visitor::visit(BlockStmt* block) {
	for (auto stmt : block->statements)
		stmt->accept(*this);
}


void Visitor::visit(AssignStmt* stmt) {
	stmt->rvalue->accept(*this);
}


void Visitor::visit(Prog* prog) {
	for (auto stmt : prog->statements)
		stmt->accept(*this);
}
