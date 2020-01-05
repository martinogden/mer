#include "ast/elaborator/expr.hpp"
#include "type/canonicaliser.hpp"


ExprElaborator::ExprElaborator(SymTab<TypePtr>& env, Map<FunTypePtr>& funTypes, Map<StructTypePtr>& structTypes) :
	env(env),
	funTypes(funTypes),
	structTypes(structTypes)
{}


ExprPtr ExprElaborator::get(ExprPtr& expr) {
	expr->accept(*this);
	return std::move(retval);
}


TypePtr ExprElaborator::get(TypePtr& type) {
	TypeCanonicaliser canon(env, structTypes);
	return canon.get(type);
}


void ExprElaborator::ret(ExprPtr expr) {
	retval = std::move(expr);
}


void ExprElaborator::visit(CallExpr& expr) {
	std::string id = expr.identifier;
	// FIXME: temporary hack
	std::string label = (id == "main") ? "__c0_main" : id;

	if (!funTypes.exists(id))
		errors.add("attempt to call undeclared function: " + id, expr.token);
	std::vector<ExprPtr> args;
	for (auto& arg : expr.args)
		args.push_back( get(arg) );

	ret( std::make_unique<CallExpr>(expr.token, label, std::move(args)) );
}


void ExprElaborator::visit(TernaryExpr& expr) {
	ret( std::make_unique<TernaryExpr>(expr.token, get(expr.cond), get(expr.then), get(expr.otherwise)) );
}


void ExprElaborator::visit(BinaryExpr& expr) {
	ret( std::make_unique<BinaryExpr>(expr.token, expr.op, get(expr.left), get(expr.right)) );
}


void ExprElaborator::visit(UnaryExpr& unary) {
	ret( std::make_unique<UnaryExpr>(unary.token, unary.op, get(unary.expr)) );
}


void ExprElaborator::visit(LiteralExpr& expr) {
	ret( std::make_unique<LiteralExpr>(expr.token, expr.type, expr.as) );
}


void ExprElaborator::visit(IdExpr& expr) {
	ret( std::make_unique<IdExpr>(expr.token, expr.identifier) );
}


void ExprElaborator::visit(SubscriptExpr& expr) {
	ret( std::make_unique<SubscriptExpr>(expr.token, get(expr.left), get(expr.right)) );
}


void ExprElaborator::visit(ArrowExpr& expr) {
	ExprPtr e = std::make_unique<DerefExpr>(expr.expr->token, get(expr.expr));
	ret( std::make_unique<DotExpr>(expr.token, std::move(e), expr.identifier) );
}


void ExprElaborator::visit(DotExpr& expr) {
	ret( std::make_unique<DotExpr>(expr.token, get(expr.expr), expr.identifier) );
}


void ExprElaborator::visit(DerefExpr& ptr) {
	ret( std::make_unique<DerefExpr>(ptr.token, get(ptr.expr)) );
}


void ExprElaborator::visit(AllocExpr& alloc) {
	ExprPtr e = alloc.expr != nullptr ? get(alloc.expr) : nullptr;
	ret( std::make_unique<AllocExpr>(alloc.token, get(alloc.typeParam), std::move(e)) );
}
