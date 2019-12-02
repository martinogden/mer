#pragma once
#include "parse-tree-visitor.hpp"
#include "ast.hpp"
#include "irt.hpp"
#include "generator.hpp"


class ExprTranslator : public Visitor {
private:
	Generator& gen;

	CmdExprPtr retval;
	void ret(CmdExprPtr e);
	void ret(IRTCmdPtr cmd, IRTExprPtr e);

	CmdExprPtr transBool(ExprPtr);

public:
	ExprTranslator(Generator& gen);

	void visit(CallExpr&) override;
	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;

	std::string freshLabel();
	std::string freshTmp();

	CmdExprPtr get(ExprPtr expr);
};
