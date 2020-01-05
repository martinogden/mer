#include "type/checker.hpp"
#include "type/comparison.hpp"


TypeChecker::TypeChecker(FunNodePtr& node, Map<FunTypePtr>& funTypes, Map<StructTypePtr>& structTypes) :
	func(node),
	funTypes(funTypes),
	structTypes(structTypes),
	errors("Type error")
{}


void TypeChecker::run() {
	func->accept(*this);
}


void TypeChecker::visit(FunNode& node) {
	if (!is_sml(node.type->codomain))
		errors.add("return values must have small type", node.token);

	for (uint i=0; i<node.params.size(); ++i) {
		TypePtr type = node.type->domain[i];
		if (!is_sml(type))
			errors.add("function parameters must have small type", node.params[i].token);
		env.define(node.params[i].name, type);
	}

	node.body->accept(*this);
}


void TypeChecker::visit(AssignNode& node) {
	TypePtr ltype = annotate(node.lvalue);
	TypePtr rtype = annotate(node.rvalue);

	if (!is_sml(ltype))
		errors.add("lhs of assignment must have small type", node.lvalue->token);
	if (!is_sml(rtype))
		errors.add("rhs of assignment must have small type", node.rvalue->token);
	if (!is_sub(rtype, ltype))
		errors.add("types of both side of assignment must match", node.token);
}


void TypeChecker::visit(IfNode& node) {
	if (!eq(annotate(node.cond), Type::BOOL))
		errors.add("if condition must be boolean", node.token);

	node.then->accept(*this);
	node.otherwise->accept(*this);
}


void TypeChecker::visit(WhileNode& node) {
	if (!eq(annotate(node.cond), Type::BOOL))
		errors.add("while condition must be boolean", node.token);

	node.body->accept(*this);
}


void TypeChecker::visit(ReturnNode& node) {
	TypePtr ftype = func->type->codomain;
	TypePtr rtype = node.expr ? annotate(node.expr) : Type::VOID;

	if (!is_sub(rtype, func->type->codomain)) {
		Token& token = node.expr ? node.expr->token : node.token;
		errors.add("invalid return type", token);
	}
}


void TypeChecker::visit(SeqNode& node) {
	node.head->accept(*this);
	node.rest->accept(*this);
}


void TypeChecker::visit(DeclNode& node) {
	TypePtr type = node.type;

	env.enter();

	if (eq(type, Type::ERROR))
		;  // propagate errors
	else if (eq(type, Type::VOID)) {
		errors.add("invalid type for var declaration", node.token);
		type = Type::ERROR;
	}
	else if (!is_sml(type)) {
		errors.add("declarations must have small type", node.token);
		type = Type::ERROR;
	}
	else if ( auto s = dynamic_cast<StructType*>(type.get()) ) {
		errors.add("structs must be dynamically allocated", node.token);
		type = Type::ERROR;
	}

	env.define(node.id, type);
	node.scope->accept(*this);

	env.exit();
}


void TypeChecker::visit(ExprNode& node) {
	annotate(node.expr);
	if (!is_sml(node.expr->type))
		errors.add("expression stmts must have small type", node.expr->token);
}


TypePtr& TypeChecker::annotate(ExprPtr& expr) {
	TypeAnnotator annotator(env, funTypes, structTypes);
	annotator.get(expr);

	if (annotator.errors.exist())
		errors.append(annotator.errors);

	return expr->type;
}
