#include <sstream>
#include "use-checker.hpp"
#include "expr-use-checker.hpp"


UseChecker::UseChecker(ASTNode* node) :
	node(node)
{}


void UseChecker::run() {
	node->accept(*this);
}


void UseChecker::visit(FunNode* node) {
	std::unordered_set<std::string> ids;

	for (Param& param : node->params)
		ids.insert(param.name);

	VarSet vars(std::move(ids));

	setDecl(vars);
	setInit(vars);
	node->body->accept(*this);
}


void UseChecker::visit(AssignNode* node) {
	VarSet init = getInit();

	check(node->expr);

	VarSet vars({node->id});
	setInit(init | vars);
}


void UseChecker::visit(IfNode* node) {
	VarSet init = getInit();
	VarSet decl = getDecl();

	check(node->cond);

	node->then->accept(*this);
	VarSet then = getInit();

	setInit(init);
	setDecl(decl);
	node->otherwise->accept(*this);
	VarSet ow = getInit();

	setInit(then & ow);
	setDecl(decl);
}


void UseChecker::visit(WhileNode* node) {
	VarSet init = getInit();
	VarSet decl = getDecl();

	check(node->cond);
	node->body->accept(*this);

	setInit(init);
	setDecl(decl);
}


void UseChecker::visit(ReturnNode* node) {
	VarSet decl = getDecl();

	if (node->expr)
		check(node->expr);

	setInit(decl);
}


void UseChecker::visit(NopNode* node) {}


void UseChecker::visit(SeqNode* node) {
	node->head->accept(*this);
	node->rest->accept(*this);
}


void UseChecker::visit(DeclNode* node) {
	VarSet decl = getDecl();
	VarSet vars({node->id});

	setDecl(decl | vars);
	node->scope->accept(*this);

	setInit(getInit() - vars);
	setDecl(decl);
}


void UseChecker::visit(ExprNode* node) {
	check(node->expr);
}


void UseChecker::check(Expr* expr) {
	ExprUseChecker checker(expr, init);
	checker.run();

	if (checker.errors.exist())
		errors.append(checker.errors);
}


void UseChecker::setInit(VarSet set) {
	init = set;
}


void UseChecker::setDecl(VarSet set) {
	decl = set;
}


VarSet& UseChecker::getInit() {
	return init;
}


VarSet& UseChecker::getDecl() {
	return decl;
}
