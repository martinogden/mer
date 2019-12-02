#pragma once
#include <string>
#include "parse-tree-visitor.hpp"
#include "parse-tree.hpp"


class ExprPrinter : public Visitor {
private:
	Expr* expr;
	std::string retval;

	void visit(CallExpr*);
	void visit(TernaryExpr*);
	void visit(BinaryExpr*);
	void visit(UnaryExpr*);
	void visit(LiteralExpr*);
	void visit(IdExpr*);

	void ret(std::string);
	std::string get(Expr* expr);

public:
	ExprPrinter(Expr* expr);
	std::string run();
};