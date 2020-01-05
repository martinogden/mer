#pragma once
#include "visitor.hpp"
#include "cst/printer.hpp"
#include "ast.hpp"
#include "type/printer.hpp"


class ASTPrinter : public ASTVisitor {
private:
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

public:
	std::string get(FunNodePtr& node);
	std::string get(ASTNodePtr& node);
	std::string get(ExprPtr& expr);
	std::string get(TypePtr& type);
};
