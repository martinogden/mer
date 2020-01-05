#pragma once
#include <vector>
#include "expr/visitor.hpp"
#include "irt/irt.hpp"
#include "expr.hpp"


class BoolExprTranslator : public ExprVisitor {
private:
	IRTCmdPtr retval;
	ExprTranslator& tr;

	std::string l1;
	std::string l2;

	void ret(IRTCmdPtr);
	void ret(std::vector<IRTCmdPtr>);
	IRTCmdPtr toRValue(Expr&, const std::string&, const std::string&);

	std::string getTrueLabel();
	std::string getFalseLabel();

public:
	BoolExprTranslator(ExprTranslator&);

	void visit(CallExpr&) override;
	void visit(AllocExpr&) override;
	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;
	void visit(SubscriptExpr&) override;
	void visit(ArrowExpr&) override;
	void visit(DotExpr&) override;
	void visit(DerefExpr&) override;

	IRTCmdPtr get(ExprPtr, std::string, std::string);
};
