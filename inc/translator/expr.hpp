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

	void visit(CallExpr*) override;
	void visit(TernaryExpr*) override;
	void visit(BinaryExpr*) override;
	void visit(UnaryExpr*) override;
	void visit(LiteralExpr*) override;
	void visit(IdExpr*) override;

	std::string freshLabel();
	std::string freshTmp();

	CmdExpr* get(Expr* expr);
};
