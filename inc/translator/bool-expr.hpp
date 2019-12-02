#pragma once
#include <vector>
#include "parse-tree-visitor.hpp"
#include "irt.hpp"
#include "translator/expr.hpp"


class BoolExprTranslator : public Visitor {
private:
	IRTCmd* retval;
	ExprTranslator& tr;

	std::string l1;
	std::string l2;

	void ret(IRTCmd*);
	void ret(std::vector<IRTCmd*>);

	std::string getTrueLabel();
	std::string getFalseLabel();

public:
	BoolExprTranslator(ExprTranslator&);

	void visit(TernaryExpr*) override;
	void visit(BinaryExpr*) override;
	void visit(UnaryExpr*) override;
	void visit(LiteralExpr*) override;
	void visit(IdExpr*) override;

	IRTCmd* get(Expr*, std::string, std::string);
};
