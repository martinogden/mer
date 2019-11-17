#pragma once
#include "parse-tree-visitor.hpp"
#include "ast.hpp"
#include "irt.hpp"
#include "generator.hpp"


class ExprTranslator : public Visitor {
private:
	Generator& gen;

	CmdExpr* retval;
	void ret(CmdExpr* e);
	void ret(IRTCmd* cmd, IRTExpr* e);

	CmdExpr* transBool(Expr*);

public:
	ExprTranslator(Generator& gen);

	void visit(TernaryExpr*);
	void visit(BinaryExpr*);
	void visit(UnaryExpr*);
	void visit(LiteralExpr*);
	void visit(IdExpr*);

	std::string freshLabel();
	std::string freshTmp();

	CmdExpr* get(Expr* expr);
};
