#pragma once
#include <vector>
#include "parse-tree-visitor.hpp"
#include "irt.hpp"
#include "translator/expr.hpp"


class BoolExprTranslator : public Visitor {
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
