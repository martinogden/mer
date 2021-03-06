#include <sstream>
#include "sca/use-checker.hpp"
#include "sca/expr-use-checker.hpp"


UseChecker::UseChecker(FunNodePtr& node) :
	root(node)
{}


void UseChecker::run() {
	root->accept(*this);
}


void UseChecker::visit(FunNode& node) {
	std::unordered_set<std::string> ids;

	for (const auto& param : node.params)
		ids.insert(param.name);

	VarSet vars( std::move(ids) );

	setDecl(vars);
	setInit(vars);
	node.body->accept(*this);
}


void UseChecker::visit(AssignNode& node) {
	VarSet init_ = getInit();

	check(node.rvalue);

	if (auto lvalue = dynamic_cast<IdExpr*>(node.lvalue.get())) {
		VarSet vars({lvalue->identifier});
		setInit(init_ | vars);
	}
	else
		setInit(init_);
}


void UseChecker::visit(IfNode& node) {
	VarSet init_ = getInit();
	VarSet decl_ = getDecl();

	check(node.cond);

	node.then->accept(*this);
	VarSet then = getInit();

	setInit(init_);
	setDecl(decl_);
	node.otherwise->accept(*this);
	VarSet ow = getInit();

	setInit(then & ow);
	setDecl(decl);
}


void UseChecker::visit(WhileNode& node) {
	VarSet init_ = getInit();
	VarSet decl_ = getDecl();

	check(node.cond);
	node.body->accept(*this);

	setInit(init_);
	setDecl(decl_);
}


void UseChecker::visit(ReturnNode& node) {
	VarSet decl_ = getDecl();

	if (node.expr)
		check(node.expr);

	setInit(decl_);
}


void UseChecker::visit(NopNode& node) {}


void UseChecker::visit(SeqNode& node) {
	node.head->accept(*this);
	node.rest->accept(*this);
}


void UseChecker::visit(DeclNode& node) {
	VarSet decl_ = getDecl();
	VarSet vars({node.id});

	setDecl(decl_ | vars);
	node.scope->accept(*this);

	setInit(getInit() - vars);
	setDecl(decl_);
}


void UseChecker::visit(ExprNode& node) {
	check(node.expr);
}


void UseChecker::check(ExprPtr& expr) {
	ExprUseChecker checker(expr, init);
	checker.run();

	if (checker.errors.exist())
		errors.append(checker.errors);
}


void UseChecker::setInit(VarSet set) {
	init = std::move(set);
}


void UseChecker::setDecl(VarSet set) {
	decl = std::move(set);
}


VarSet& UseChecker::getInit() {
	return init;
}


VarSet& UseChecker::getDecl() {
	return decl;
}
