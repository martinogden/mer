#pragma once
#include <stack>
#include "ast/visitor.hpp"
#include "ast/ast.hpp"
#include "errors.hpp"
#include "set.hpp"


// Ensure all vars declared before initialization
class InitChecker : public ASTVisitor {
private:
	FunNodePtr& root;
	Set<std::string> scope;

	void visit(FunNode&) override;
	void visit(AssignNode&) override;
	void visit(IfNode&) override;
	void visit(WhileNode&) override;
	void visit(ReturnNode&) override;
	void visit(NopNode&) override;
	void visit(SeqNode&) override;
	void visit(DeclNode&) override;
	void visit(ExprNode&) override;

public:
	Errors errors;

	InitChecker(FunNodePtr& node);
	void run();
};
