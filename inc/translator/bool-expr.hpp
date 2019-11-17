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

	void visit(TernaryExpr*);
	void visit(BinaryExpr*);
	void visit(UnaryExpr*);
	void visit(LiteralExpr*);
	void visit(IdExpr*);

	IRTCmd* get(Expr*, std::string, std::string);
};
