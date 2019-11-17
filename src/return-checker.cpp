#include "return-checker.hpp"


ReturnChecker::ReturnChecker(ASTNode* node) :
	node(node)
{}


void ReturnChecker::run() {
	if (!get(node))
		errors.add("main may not return an int");
}


void ReturnChecker::visit(AssignNode* node) {
	ret(false);
}


void ReturnChecker::visit(IfNode* node) {
	ret( get(node->then) & get(node->otherwise) );
}


void ReturnChecker::visit(WhileNode* node) {
	ret(false);
}


void ReturnChecker::visit(ReturnNode* node) {
	ret(true);
}


void ReturnChecker::visit(NopNode* node) {
	ret(false);
}


void ReturnChecker::visit(SeqNode* node) {
	ret( get(node->head) | get(node->rest) );
}


void ReturnChecker::visit(DeclNode* node) {
	ret( get(node->scope) );
}


void ReturnChecker::ret(bool b) {
	retval = b;
}


bool ReturnChecker::get(ASTNode* node) {
	node->accept(*this);
	return retval;
}
