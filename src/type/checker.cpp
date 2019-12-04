#include "type/checker.hpp"


TypeChecker::TypeChecker(FunNodePtr& node, SymTab<FunType>& decls) :
	func(node),
	decls(decls),
	errors("Type error")
{}


void TypeChecker::run() {
	func->accept(*this);
}


void TypeChecker::visit(FunNode& node) {
	for (Param& param : node.params) {
		if (env.exists(param.name)) {
			errors.add("function params must have distinct names", node.token);
			return;
		}
		else
			env.define(param.name, param.type);
	}

	node.body->accept(*this);
}


void TypeChecker::visit(AssignNode& node) {
	Type type = env.lookup(node.id);

	if (annotate(node.expr) != type)
		errors.add("types of both side of assignment must match", node.token);
}


void TypeChecker::visit(IfNode& node) {
	if (annotate(node.cond) != Type::BOOL)
		errors.add("if condition must be boolean", node.token);

	node.then->accept(*this);
	node.otherwise->accept(*this);
}


void TypeChecker::visit(WhileNode& node) {
	if (annotate(node.cond) != Type::BOOL)
		errors.add("while condition must be boolean", node.token);

	node.body->accept(*this);
}


void TypeChecker::visit(ReturnNode& node) {
	Type ftype = func->type;
	Type rtype;

	if (node.expr)
		rtype = annotate(node.expr);
	else
		rtype = Type::VOID;

	if (rtype != ftype)
		errors.add("invalid return type", node.token);
}


void TypeChecker::visit(NopNode& node) {}


void TypeChecker::visit(SeqNode& node) {
	node.head->accept(*this);
	node.rest->accept(*this);
}


void TypeChecker::visit(DeclNode& node) {
	Type type = Type::UNKNOWN;

	env.enter();

	if (node.type == Type::INT || node.type == Type::BOOL)
		type = node.type;
	else {
		errors.add("invalid type for var declaration", node.token);
		type = Type::ERROR;
	}

	env.define(node.id, type);
	node.scope->accept(*this);
	env.exit();
}


void TypeChecker::visit(ExprNode& node) {
	annotate(node.expr);
}


Type TypeChecker::annotate(ExprPtr& expr) {
	TypeAnnotator annotator(env, decls);
	annotator.get(expr);

	if (annotator.errors.exist())
		errors.append(annotator.errors);

	return expr->type;
}
