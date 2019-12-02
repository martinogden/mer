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

	void visit(FunDecl*) override;
	void visit(FunDefn*) override;
	void visit(TypedefStmt*) override;
	void visit(BlockStmt*) override;
	void visit(IfStmt*) override;
	void visit(WhileStmt*) override;
	void visit(ForStmt*) override;
	void visit(ReturnStmt*) override;
	void visit(DeclStmt*) override;
	void visit(AssignStmt*) override;
	void visit(PostOpStmt*) override;
	void visit(ExprStmt*) override;

	void visit(CallExpr*) override;

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
