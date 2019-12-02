#pragma once
#include "ast-visitor.hpp"
#include "ast.hpp"
#include "errors.hpp"


class ReturnChecker : public ASTVisitor {
private:
	FunNode* node;
	bool retval;

	void visit(AssignNode*) override;
	void visit(IfNode*) override;
	void visit(WhileNode*) override;
	void visit(ReturnNode*) override;
	void visit(NopNode*) override;
	void visit(SeqNode*) override;
	void visit(DeclNode*) override;
	void visit(ExprNode*) override;

	void ret(bool b);

public:
	Errors errors;

	ReturnChecker(FunNode*);
	bool get(ASTNode*);
	void run();
};
