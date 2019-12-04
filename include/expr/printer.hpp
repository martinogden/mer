#pragma once
#include <string>
#include "cst/visitor.hpp"
#include "cst/cst.hpp"


class ExprPrinter : public CSTVisitor {
private:
	ExprPtr& root;
	std::string retval;

	void visit(CallExpr&) override;
	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;

	void ret(std::string);
	std::string get(ExprPtr&);

public:
	ExprPrinter(ExprPtr&);
	std::string run();
};