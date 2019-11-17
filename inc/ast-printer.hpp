#pragma once
#include "ast-visitor.hpp"
#include "tree-printer.hpp"
#include "ast.hpp"


class ASTPrinter : public ASTVisitor {
private:
	ASTNode* node;
	std::string retval;

	void visit(AssignNode*);
	void visit(IfNode*);
	void visit(WhileNode*);
	void visit(ReturnNode*);
	void visit(NopNode*);
	void visit(SeqNode*);
	void visit(DeclNode*);
	void visit(ExprNode*);

	void ret(std::string s);
	std::string get(ASTNode* node);
	std::string get(Expr* expr);

public:
	ASTPrinter(ASTNode* node);
	std::string run();
};
