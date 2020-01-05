#pragma once
#include "map.hpp"
#include "symtab.hpp"
#include "cst/visitor.hpp"
#include "ast/ast.hpp"
#include "errors.hpp"
#include "type/annotator.hpp"


class TypeChecker : public ASTVisitor {
private:
	FunNodePtr& func;
	Map<FunTypePtr>& funTypes;
	Map<StructTypePtr>& structTypes;
	SymTab<TypePtr> env;

	void visit(FunNode&) override;
	void visit(AssignNode&) override;
	void visit(IfNode&) override;
	void visit(WhileNode&) override;
	void visit(ReturnNode&) override;
	void visit(SeqNode&) override;
	void visit(DeclNode&) override;
	void visit(ExprNode&) override;

	TypePtr& annotate(ExprPtr& expr);

public:
	Errors errors;

	TypeChecker(FunNodePtr& node, Map<FunTypePtr>& funTypes, Map<StructTypePtr>& structType);
	void run();
};
