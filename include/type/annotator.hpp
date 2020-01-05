#pragma once
#include "symtab.hpp"
#include "map.hpp"
#include "errors.hpp"
#include "expr/visitor.hpp"
#include "expr/expr.hpp"


class TypeAnnotator : public ExprVisitor {
private:
	SymTab<TypePtr>& env;
	Map<FunTypePtr>& funTypes;
	Map<StructTypePtr>& structTypes;

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

	TypePtr canon(TypePtr& type);
	void annotate(Expr& expr, TypePtr& type);
public:
	Errors errors;

	TypeAnnotator(SymTab<TypePtr>& env, Map<FunTypePtr>& funTypes, Map<StructTypePtr>& structTypes);
	TypePtr get(ExprPtr& expr);
};
