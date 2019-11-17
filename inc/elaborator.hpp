#pragma once
#include <vector>
#include <stack>
#include "parse-tree.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "parse-tree-visitor.hpp"


// 'elaborates' prog in place
// where elaborate = lower ParseTree -> AST
class Elaborator : public Visitor {
private:
	ParseTree* tree;
	ASTNode* retval;

	void visit(BlockStmt*);
	void visit(IfStmt*);
	void visit(WhileStmt*);
	void visit(ForStmt*);
	void visit(ReturnStmt*);
	void visit(DeclStmt*);
	void visit(AssignStmt*);
	void visit(PostOpStmt*);
	void visit(ExprStmt*);

	void ret(ASTNode* stmt);
	ASTNode* get(Stmt* stmt);

	std::string getIdOrError(Expr* expr);
	ASTNode* seq(Stmt* stmt, ASTNode* node);

public:
	Errors errors;

	Elaborator(ParseTree* tree);
	ASTNode* run();
};
