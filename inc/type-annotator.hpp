#pragma once
#include "parse-tree-visitor.hpp"
#include "parse-tree.hpp"
#include "errors.hpp"
#include "symtab.hpp"


class TypeAnnotator : public Visitor {
private:
	SymTab<Type>& env;
	SymTab<FunType>& decls;

	void visit(CallExpr*);
	void visit(TernaryExpr*);
	void visit(BinaryExpr*);
	void visit(UnaryExpr*);
	void visit(LiteralExpr*);
	void visit(IdExpr*);

	void annotate(Expr* expr, Type type);
public:
	Errors errors;

	TypeAnnotator(SymTab<Type>& env, SymTab<FunType>& decls);
	Type get(Expr* expr);
};
