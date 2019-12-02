#pragma once
#include <stack>
#include "ast-visitor.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "set.hpp"


// Ensure all vars declared before initialization
class InitChecker : public ASTVisitor {
private:
	ASTNode* node;
	Set<std::string> scope;

	void visit(FunNode*);
	void visit(AssignNode*);
	void visit(IfNode*);
	void visit(WhileNode*);
	void visit(ReturnNode*);
	void visit(NopNode*);
	void visit(SeqNode*);
	void visit(DeclNode*);
	void visit(ExprNode*);

public:
	Errors errors;

	InitChecker(ASTNode* node);
	void run();
};
