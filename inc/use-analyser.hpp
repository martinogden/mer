#pragma once
#include <string>
#include <set>
#include "parse-tree-visitor.hpp"
#include "parse-tree.hpp"
#include "set.hpp"
#include "errors.hpp"


class UseAnalyser : public Visitor {
private:
	Set<std::string> retval;
	Set<std::string>& scope;

	void visit(TernaryExpr*);
	void visit(BinaryExpr*);
	void visit(UnaryExpr*);
	void visit(LiteralExpr*);
	void visit(IdExpr*);

	void ret(Set<std::string> vars);
public:
	Errors errors;

	UseAnalyser(Set<std::string>& scope);
	Set<std::string> get(Expr* expr);
};
