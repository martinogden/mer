#pragma once

#include <irt/containers.hpp>
#include "expr/visitor.hpp"
#include "ast/ast.hpp"
#include "irt/irt.hpp"
#include "generator.hpp"


class ExprTranslator : public ExprVisitor {
private:
	Generator& gen;
	Map<IRTStruct>& structs;

	CmdExprPtr retval;
	void ret(CmdExprPtr e);
	void ret(IRTCmdPtr cmd, IRTExprPtr e);

	CmdExprPtr transBool(ExprPtr);

public:
	ExprTranslator(Generator& gen, Map<IRTStruct>& structs);

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

	std::string freshLabel();
	std::string freshTmp();
	IRTStruct& getIRTStruct(const std::string&);

	CmdExprPtr get(Expr* expr);  // hack used only in BoolExprTrans...
	CmdExprPtr get(ExprPtr expr);
	CmdExprPtr getAsLValue(ExprPtr expr);
	CmdExprPtr toRValue(Expr& expr);
};
