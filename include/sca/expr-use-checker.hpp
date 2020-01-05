#pragma once
#include "expr/visitor.hpp"
#include "expr/expr.hpp"
#include "set.hpp"
#include "errors.hpp"


class ExprUseChecker : public ExprVisitor {
private:
	ExprPtr& root;
	Set<std::string>& scope;

	void visit(IdExpr&) override;
public:
	Errors errors;

	ExprUseChecker(ExprPtr&, Set<std::string>&);
	void run();
};
