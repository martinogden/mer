#pragma once
#include <stack>
#include "ast-visitor.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "set.hpp"


typedef Set<std::string> VarSet;


// Ensure all vars initialized before use
class UseChecker : public ASTVisitor {
private:
	ASTNode* node;
	VarSet init;
	VarSet decl;

	void visit(FunNode*) override;
	void visit(AssignNode*) override;
	void visit(IfNode*) override;
	void visit(WhileNode*) override;
	void visit(ReturnNode*) override;
	void visit(NopNode*) override;
	void visit(SeqNode*) override;
	void visit(DeclNode*) override;
	void visit(ExprNode*) override;

	void check(Expr* expr);

	void setInit(VarSet set);
	void setDecl(VarSet set);
	VarSet& getInit();
	VarSet& getDecl();

public:
	Errors errors;

	UseChecker(ASTNode*);
	void run();
};
