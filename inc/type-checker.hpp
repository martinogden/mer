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

	void visit(FunNode*) override;
	void visit(AssignNode*) override;
	void visit(IfNode*) override;
	void visit(WhileNode*) override;
	void visit(ReturnNode*) override;
	void visit(NopNode*) override;
	void visit(SeqNode*) override;
	void visit(DeclNode*) override;
	void visit(ExprNode*) override;

	Type annotate(Expr* expr);

public:
	Errors errors;

	TypeChecker(FunNode* node, SymTab<FunType>& decls);
	void run();
};
