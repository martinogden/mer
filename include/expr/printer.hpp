#pragma once
#include <string>
#include "visitor.hpp"
#include "expr.hpp"


std::string to_string(UnOp op);
std::string to_string(BinOp op);


class ExprPrinter : public ExprVisitor {
private:
	std::string retval;

	void visit(CallExpr&) override;
	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;
	void visit(SubscriptExpr&) override;
	void visit(ArrowExpr&) override;
	void visit(DotExpr&) override;
	void visit(DerefExpr&) override;
	void visit(AllocExpr&) override;

	void ret(std::string);
	std::string get(TypePtr&);

public:
	std::string get(ExprPtr&);
};
