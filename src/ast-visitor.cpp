#include "ast-visitor.hpp"
#include "ast.hpp"


void ASTVisitor::visit(AssignNode* node) {}
void ASTVisitor::visit(ReturnNode* node) {}
void ASTVisitor::visit(NopNode* node) {}
void ASTVisitor::visit(ExprNode* node) {}


void ASTVisitor::visit(FunNode* node) {
	node->body->accept(*this);
}


void ASTVisitor::visit(IfNode* node) {
	node->then->accept(*this);
	node->otherwise->accept(*this);
}


void ASTVisitor::visit(WhileNode* node) {
	node->body->accept(*this);
}


void ASTVisitor::visit(SeqNode* node) {
	node->head->accept(*this);
	node->rest->accept(*this);
}


void ASTVisitor::visit(DeclNode* node) {
	node->scope->accept(*this);
}
