#pragma once
#include <unordered_set>
#include <vector>
#include <stack>
#include "parse-tree.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "parse-tree-visitor.hpp"
#include "symtab.hpp"


// 'elaborates' parse tree into list of function definitions
// where elaborate = lower: ParseTree -> [ASTNode]
class Elaborator : public Visitor {
private:
	ParseTree* tree;
	ASTNode* retval;
	SymTab<Type> types;
	SymTab<FunType> decls;
	std::vector<FunNode*> defns;
	std::unordered_set<std::string> defnNames;

	void visit(FunDecl*);
	void visit(FunDefn*);
	void visit(TypedefStmt*);
	void visit(BlockStmt*);
	void visit(IfStmt*);
	void visit(WhileStmt*);
	void visit(ForStmt*);
	void visit(ReturnStmt*);
	void visit(DeclStmt*);
	void visit(AssignStmt*);
	void visit(PostOpStmt*);
	void visit(ExprStmt*);

	void visit(CallExpr*);

	void ret(ASTNode* stmt);
	ASTNode* get(Stmt* stmt);

	void emit(FunNode* node);

	std::string getIdOrError(Expr* expr);
	ASTNode* seq(Stmt* stmt, ASTNode* node);

	Type resolveType(Token token);
	void seedTypes();
	void seedDecls();

public:
	Errors errors;

	Elaborator(ParseTree* tree);
	std::vector<FunNode*> run();
	SymTab<FunType> getDecls();

};
