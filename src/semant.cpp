#include "semant.hpp"


constexpr bool DECLARED = false;
constexpr bool DEFINED = true;


Semant::Semant(Prog* program) :
	program(program),
	hasReturned(false)
{}


bool Semant::run() {
	program->accept(*this);
	return errors.size() == 0;
}


void Semant::visit(VarExpr* expr) {
	std::string id = expr->identifier;
	if (!env.exists(id))
		addError("attempt to use undeclared variable: " + id, expr->token);

	else if (!hasReturned && !env.lookup(id))
		addError("attempt to use undefined variable: " + id, expr->token);
}


void Semant::visit(DeclStmt* stmt) {
	if (stmt->expr)
		stmt->expr->accept(*this);

	std::string id = stmt->identifier;
	if (env.exists(id))
		addError("attempt to redeclare variable: " + id, stmt->token);
	else
		env.define(id, stmt->expr ? DEFINED : DECLARED);
}


void Semant::visit(AssignStmt* stmt) {
	stmt->rvalue->accept(*this);
	std::string id = stmt->lvalue;

	if (!env.exists(id))
		addError("attempt to assign to undeclared variable: " + id, stmt->token);
	else
		env.assign(id, DEFINED);
}


void Semant::addError(std::string msg, Token& token) {
	errors.push_back(error(msg, token));
}


void Semant::visit(ReturnStmt* stmt) {
	stmt->expr->accept(*this);
	hasReturned = true;
}


void Semant::visit(Prog* prog) {
	for (auto stmt : prog->statements)
		stmt->accept(*this);

	if (!hasReturned)
		addError("Missing return statement", prog->token);
}



std::vector<std::string>& Semant::getErrors() {
	return errors;
}


// TODO find out how to override this, we don't
// have to redefine them from visitor.cpp
void Semant::visit(BinaryExpr* expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
}


void Semant::visit(UnaryExpr* unary) {
	unary->expr->accept(*this);
}


void Semant::visit(LiteralExpr* expr) {}


void Semant::visit(BlockStmt* block) {
	for (auto stmt : block->statements)
		stmt->accept(*this);
}
