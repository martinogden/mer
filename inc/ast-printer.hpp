#pragma once
#include "ast-visitor.hpp"
#include "parse-tree-printer.hpp"
#include "ast.hpp"


class ASTPrinter : public ASTVisitor {
private:
	std::vector<FunNode*>& nodes;
	std::string retval;

	void visit(FunNode*) override;
	void visit(AssignNode*) override;
	void visit(IfNode*) override;
	void visit(WhileNode*) override;
	void visit(ReturnNode*) override;
	void visit(NopNode*) override;
	void visit(SeqNode*) override;
	void visit(DeclNode*) override;
	void visit(ExprNode*) override;

	void ret(std::string s);
	std::string get(ASTNode* node);
	std::string get(Expr* expr);

public:
	ASTPrinter(std::vector<FunNode*>& nodes);
	std::string run();
};
