#include <memory>
#include "expr/lvalue/canonicaliser.hpp"


LValueCanon::LValueCanon(ExprPtr& expr, Map<FunTypePtr>& funTypes, Generator& gen) :
	root(expr),
	funTypes(funTypes),
	gen(gen)
{}


std::pair<std::vector<StmtPtr>, ExprPtr> LValueCanon::run() {
	root->accept(*this);
	return {std::move(stmts), std::move(retval)};
}


void LValueCanon::ret(ExprPtr expr) {
	retval = std::move(expr);
}


ExprPtr LValueCanon::get(ExprPtr& expr) {
	expr->accept(*this);
	return std::move(retval);
}


void LValueCanon::emit(StmtPtr stmt) {
	stmts.push_back( std::move(stmt) );
}


void LValueCanon::visit(CallExpr& expr) {
	std::string id = expr.identifier;
	std::string tmp = gen.tmp();

	ExprPtr f = std::make_unique<CallExpr>(expr.token, id, std::move(expr.args));

	TypePtr type;
	if (funTypes.lookup(id))
		type = funTypes.lookup(id)->codomain;
	else
		type = Type::ERROR;

	emit( std::make_unique<DeclStmt>(expr.token, tmp, std::move(type), std::move(f)) );
	ret( std::make_unique<IdExpr>(expr.token, tmp));
}


void LValueCanon::visit(TernaryExpr& expr) {
	ExprPtr e = std::make_unique<TernaryExpr>(
		expr.token,
		get(expr.cond),
		get(expr.then),
		get(expr.otherwise)
	);
	ret( std::move(e) );
}


void LValueCanon::visit(BinaryExpr& expr) {
	ExprPtr e = std::make_unique<BinaryExpr>(
		expr.token,
		expr.op,
		get(expr.left),
		get(expr.right)
	);

	ret( std::move(e) );
}


void LValueCanon::visit(UnaryExpr& unary) {
	ExprPtr e = std::make_unique<UnaryExpr>(unary.token, unary.op, get(unary.expr));
	ret( std::move(e) );
}


void LValueCanon::visit(LiteralExpr& expr) {
	ret( std::make_unique<LiteralExpr>(expr.token, expr.type, expr.as ) );
}


void LValueCanon::visit(IdExpr& expr) {
	ret( std::make_unique<IdExpr>(expr.token, expr.identifier) );
}


void LValueCanon::visit(SubscriptExpr& expr) {
	ExprPtr e = std::make_unique<SubscriptExpr>(expr.token, get(expr.left), get(expr.right));
	ret( std::move(e) );
}


void LValueCanon::visit(ArrowExpr& expr) {
	std::string id = expr.identifier;
	ExprPtr e = std::make_unique<ArrowExpr>(expr.token, get(expr.expr), id);
	ret( std::move(e) );
}


void LValueCanon::visit(DotExpr& expr) {
	std::string id = expr.identifier;
	ExprPtr e = std::make_unique<DotExpr>(expr.token, get(expr.expr), id);
	ret( std::move(e) );
}


void LValueCanon::visit(DerefExpr& expr) {
	ret( std::make_unique<DerefExpr>(expr.token, get(expr.expr)) );
}


void LValueCanon::visit(AllocExpr& expr) {
	throw 1;  // TODO: we should never get here
}
