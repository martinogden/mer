#pragma once
#include <vector>
#include "cst/visitor.hpp"
#include "irt/irt.hpp"
#include "expr.hpp"


class BoolExprTranslator : public CSTVisitor {
private:
	IRTCmdPtr retval;
	ExprTranslator& tr;

	std::string l1;
	std::string l2;

	void ret(IRTCmdPtr);
	void ret(std::vector<IRTCmdPtr>);

	std::string getTrueLabel();
	std::string getFalseLabel();

public:
	BoolExprTranslator(ExprTranslator&);

	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;

	IRTCmdPtr get(ExprPtr, std::string, std::string);
};
