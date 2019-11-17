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

	void visit(AssignNode*);
	void visit(IfNode*);
	void visit(WhileNode*);
	void visit(ReturnNode*);
	void visit(NopNode*);
	void visit(SeqNode*);
	void visit(DeclNode*);
	void visit(ExprNode*);

	void check(Expr* expr);

	void setInit(VarSet set);
	void setDecl(VarSet set);
	VarSet& getInit();
	VarSet& getDecl();

	void enter();
	void exit();

public:
	Errors errors;

	UseChecker(ASTNode* node);
	void run();
};
