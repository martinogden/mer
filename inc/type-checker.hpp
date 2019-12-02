#pragma once
#include "parse-tree-visitor.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "type-annotator.hpp"
#include "symtab.hpp"


class TypeChecker : public ASTVisitor {
private:
	FunNode* func;
	SymTab<FunType>& decls;
	SymTab<Type> env;

	void visit(FunNode*);
	void visit(AssignNode*);
	void visit(IfNode*);
	void visit(WhileNode*);
	void visit(ReturnNode*);
	void visit(NopNode*);
	void visit(SeqNode*);
	void visit(DeclNode*);
	void visit(ExprNode*);

	Type annotate(Expr* expr);

public:
	Errors errors;

	TypeChecker(FunNode* node, SymTab<FunType>& decls);
	void run();
};
