#pragma once
#include "ast-visitor.hpp"
#include "parse-tree-printer.hpp"
#include "ast.hpp"


class ASTPrinter : public ASTVisitor {
private:
	std::vector<FunNode*>& nodes;
	std::string retval;

	void visit(FunNode*);
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
	ASTPrinter(std::vector<FunNode*>& nodes);
	std::string run();
};
