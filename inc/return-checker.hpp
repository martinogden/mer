#pragma once
#include "ast-visitor.hpp"
#include "ast.hpp"
#include "errors.hpp"


class ReturnChecker : public ASTVisitor {
private:
	FunNode* node;
	bool retval;

	virtual void visit(AssignNode*);
	virtual void visit(IfNode*);
	virtual void visit(WhileNode*);
	virtual void visit(ReturnNode*);
	virtual void visit(NopNode*);
	virtual void visit(SeqNode*);
	virtual void visit(DeclNode*);
	virtual void visit(ExprNode*);

	void ret(bool b);

public:
	Errors errors;

	ReturnChecker(FunNode* node);
	bool get(ASTNode* node);
	void run();
};
