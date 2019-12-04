#include "sca/expr-use-checker.hpp"


ExprUseChecker::ExprUseChecker(ExprPtr& expr, Set<std::string>& scope) :
	root(expr),
	scope(scope)
{}


void ExprUseChecker::run() {
	root->accept(*this);
}


void ExprUseChecker::visit(IdExpr& expr) {
	std::string id = expr.identifier;

	if (!scope.contains(id))
		errors.add("use of uninitialized variable: " + id, expr.token);
}
