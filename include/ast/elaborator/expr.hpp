#pragma once
#include "errors.hpp"
#include "map.hpp"
#include "symtab.hpp"
#include "expr/visitor.hpp"
#include "expr/expr.hpp"


class ExprElaborator : public ExprVisitor {
private:
	SymTab<TypePtr>& env;
	Map<FunTypePtr>& funTypes;
	Map<StructTypePtr>& structTypes;

	ExprPtr retval;
	void ret(ExprPtr expr);
	TypePtr get(TypePtr& type);

	void visit(CallExpr&) override;
	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;
	void visit(SubscriptExpr&) override;
	void visit(ArrowExpr&) override;
	void visit(DotExpr&) override;
	void visit(DerefExpr&) override;
	void visit(AllocExpr&) override;

public:
	Errors errors;

	ExprElaborator(SymTab<TypePtr>& env, Map<FunTypePtr>& funTypes, Map<StructTypePtr>& structTypes);
	ExprPtr get(ExprPtr& expr);
};
