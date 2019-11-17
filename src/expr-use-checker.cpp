#include "expr-use-checker.hpp"


ExprUseChecker::ExprUseChecker(Expr* expr, Set<std::string>& scope) :
	expr(expr),
	scope(scope)
{}


void ExprUseChecker::run() {
	expr->accept(*this);
}


void ExprUseChecker::visit(IdExpr* expr) {
	std::string id = expr->identifier;

	if (!scope.contains(id))
		errors.add("use of uninitialized variable: " + id, expr->token);
}
