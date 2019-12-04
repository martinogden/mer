#include "sca/return-checker.hpp"


ReturnChecker::ReturnChecker(FunNodePtr& node) :
	root(node)
{}


void ReturnChecker::run() {
	if (root->type == Type::VOID)
		return;

	if (!get(root->body))
		errors.add("Function `" + root->id + "` may not return.", root->token);
}


void ReturnChecker::visit(AssignNode& node) {
	ret(false);
}


void ReturnChecker::visit(IfNode& node) {
	ret( get(node.then) && get(node.otherwise) );
}


void ReturnChecker::visit(WhileNode& node) {
	ret(false);
}


void ReturnChecker::visit(ReturnNode& node) {
	ret(true);
}


void ReturnChecker::visit(NopNode& node) {
	ret(false);
}


void ReturnChecker::visit(SeqNode& node) {
	ret( get(node.head) || get(node.rest) );
}


void ReturnChecker::visit(DeclNode& node) {
	ret( get(node.scope) );
}


void ReturnChecker::visit(ExprNode& node) {
	ret(false);
}


void ReturnChecker::ret(bool b) {
	retval = b;
}


bool ReturnChecker::get(ASTNodePtr& node) {
	node->accept(*this);
	return retval;
}
