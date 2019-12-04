#pragma once
#include "cst/visitor.hpp"
#include "ast/ast.hpp"
#include "errors.hpp"
#include "type/annotator.hpp"
#include "symtab.hpp"


class TypeChecker : public ASTVisitor {
private:
	FunNodePtr& func;
	SymTab<FunType>& decls;
	SymTab<Type> env;

	void visit(FunNode&) override;
	void visit(AssignNode&) override;
	void visit(IfNode&) override;
	void visit(WhileNode&) override;
	void visit(ReturnNode&) override;
	void visit(NopNode&) override;
	void visit(SeqNode&) override;
	void visit(DeclNode&) override;
	void visit(ExprNode&) override;

	Type annotate(ExprPtr& expr);

public:
	Errors errors;

	TypeChecker(FunNodePtr& node, SymTab<FunType>& decls);
	void run();
};
