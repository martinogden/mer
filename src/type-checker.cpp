#include "type-checker.hpp"


TypeChecker::TypeChecker(ASTNode* node) :
	node(node),
	errors("Type error")
{}


void TypeChecker::run() {
	node->accept(*this);
}


void TypeChecker::visit(AssignNode* node) {
	Type type = env.lookup(node->id);

	if (annotate(node->expr) != type)
		errors.add("types of both side of assignment must match", node->token);
}


void TypeChecker::visit(IfNode* node) {
	if (annotate(node->cond) != Type::BOOL)
		errors.add("if condition must be boolean", node->token);

	node->then->accept(*this);
	node->otherwise->accept(*this);
}


void TypeChecker::visit(WhileNode* node) {
	if (annotate(node->cond) != Type::BOOL)
		errors.add("while condition must be boolean", node->token);

	node->body->accept(*this);
}


void TypeChecker::visit(ReturnNode* node) {
	if (annotate(node->expr) != Type::INT)
		errors.add("must return int", node->token);
}


void TypeChecker::visit(NopNode* node) {}


void TypeChecker::visit(SeqNode* node) {
	node->head->accept(*this);
	node->rest->accept(*this);
}


void TypeChecker::visit(DeclNode* node) {
	env.enter();
	env.define(node->id, node->type);
	node->scope->accept(*this);
	env.exit();
}


void TypeChecker::visit(ExprNode* node) {
	annotate(node->expr);
}


Type TypeChecker::annotate(Expr* expr) {
	TypeAnnotator annotator(env);
	annotator.get(expr);

	if (annotator.errors.exist())
		errors.append(annotator.errors);

	return expr->type;
}
