#pragma once
#include "cst/visitor.hpp"
#include "cst/cst.hpp"
#include "set.hpp"
#include "errors.hpp"


class ExprUseChecker : public CSTVisitor {
private:
	ExprPtr& root;
	Set<std::string>& scope;

	void visit(IdExpr&) override;
public:
	Errors errors;

	ExprUseChecker(ExprPtr&, Set<std::string>&);
	void run();
};
