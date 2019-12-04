#pragma once
#include "visitor.hpp"
#include "cst/printer.hpp"
#include "ast.hpp"


class ASTPrinter : public ASTVisitor {
private:
	FunNodePtr& node;
	std::string retval;

	void visit(FunNode&) override;
	void visit(AssignNode&) override;
	void visit(IfNode&) override;
	void visit(WhileNode&) override;
	void visit(ReturnNode&) override;
	void visit(NopNode&) override;
	void visit(SeqNode&) override;
	void visit(DeclNode&) override;
	void visit(ExprNode&) override;

	void ret(std::string s);
	std::string get(ASTNodePtr& node);
	std::string get(ExprPtr& expr);

public:
	ASTPrinter(FunNodePtr& node);
	std::string run();
};
