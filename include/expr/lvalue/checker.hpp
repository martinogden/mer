#pragma once
#include "expr/visitor.hpp"
#include "expr/expr.hpp"
#include "errors.hpp"


class LValueChecker : public ExprVisitor {
private:
	bool retval;
	void ret(bool b);

public:
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

	bool get(ExprPtr& expr);
};
