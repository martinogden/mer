#include "init-checker.hpp"


InitChecker::InitChecker(ASTNode* node) :
	node(node)
{}


void InitChecker::run() {
	node->accept(*this);
}


void InitChecker::visit(ReturnNode* node) {}
void InitChecker::visit(NopNode* node) {}
void InitChecker::visit(ExprNode* node) {}


void InitChecker::visit(AssignNode* node) {
	std::string id = node->id;
	if (!scope.contains(id))
		errors.add("attempt to initialize an undeclared variable: " + id, node->token);
}


void InitChecker::visit(IfNode* node) {
	node->then->accept(*this);
	node->otherwise->accept(*this);
}


void InitChecker::visit(WhileNode* node) {
	node->body->accept(*this);
}


void InitChecker::visit(SeqNode* node) {
	node->head->accept(*this);
	node->rest->accept(*this);
}


void InitChecker::visit(DeclNode* node) {
	std::string id = node->id;

	if (scope.contains(id))
		errors.add("attempt to redeclare a variable: " + id, node->token);

	Set<std::string> vars({id});
	Set<std::string> encl = scope;
	scope = encl | vars;
	node->scope->accept(*this);
	scope = encl;
}
