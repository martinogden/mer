#pragma once
#include "cst/visitor.hpp"
#include "cst/cst.hpp"
#include "errors.hpp"
#include "symtab.hpp"


class TypeAnnotator : public CSTVisitor {
private:
	SymTab<Type>& env;
	SymTab<FunType>& decls;

	void visit(CallExpr&) override;
	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;

	void annotate(Expr& expr, Type type);
public:
	Errors errors;

	TypeAnnotator(SymTab<Type>& env, SymTab<FunType>& decls);
	Type get(ExprPtr& expr);
};
