#pragma once
#include "expr/visitor.hpp"
#include "irt/translator/expr.hpp"


class LValueTranslator : public ExprVisitor {
private:
	CmdExprPtr retval;
	ExprTranslator& tr;

	void ret(IRTCmdPtr, IRTExprPtr);
public:

	LValueTranslator(ExprTranslator&);

	void visit(SubscriptExpr&) override;
	void visit(ArrowExpr&) override;
	void visit(DotExpr&) override;
	void visit(DerefExpr&) override;

	void visit(CallExpr&) override;
	void visit(AllocExpr&) override;
	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;

	// TODO: take ExprPtr rather than raw ptr
	CmdExprPtr get(Expr* expr);
};