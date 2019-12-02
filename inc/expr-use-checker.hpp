#pragma once
#include "parse-tree-visitor.hpp"
#include "parse-tree.hpp"
#include "set.hpp"
#include "errors.hpp"


class ExprUseChecker : public Visitor {
private:
	Expr* root;
	Set<std::string>& scope;

	void visit(IdExpr*) override;
public:
	Errors errors;

	ExprUseChecker(Expr*, Set<std::string>&);
	void run();
};
