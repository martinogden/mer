#pragma once
#include <string>
#include "parse-tree-visitor.hpp"
#include "parse-tree.hpp"


class ExprPrinter : public Visitor {
private:
	Expr* expr;
	std::string retval;

	void visit(CallExpr*) override;
	void visit(TernaryExpr*) override;
	void visit(BinaryExpr*) override;
	void visit(UnaryExpr*) override;
	void visit(LiteralExpr*) override;
	void visit(IdExpr*) override;

	void ret(std::string);
	std::string get(Expr* expr);

public:
	ExprPrinter(Expr* expr);
	std::string run();
};