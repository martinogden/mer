#pragma once
#include <unordered_set>
#include <vector>
#include <stack>
#include "cst/cst.hpp"
#include "cst/visitor.hpp"
#include "ast/ast.hpp"
#include "errors.hpp"
#include "symtab.hpp"


// 'elaborates' parse tree into list of function definitions
// where elaborate = lower: ParseTree -> [ASTNode]
class Elaborator : public CSTVisitor {
private:
	ParseTree& tree;
	ASTNodePtr retval;
	SymTab<Type> types;
	SymTab<FunType> decls;
	std::vector<FunNodePtr> defns;
	std::unordered_set<std::string> defnNames;

	void visit(FunDecl&) override;
	void visit(FunDefn&) override;
	void visit(TypedefStmt&) override;
	void visit(BlockStmt&) override;
	void visit(IfStmt&) override;
	void visit(WhileStmt&) override;
	void visit(ForStmt&) override;
	void visit(ReturnStmt&) override;
	void visit(DeclStmt&) override;
	void visit(AssignStmt&) override;
	void visit(PostOpStmt&) override;
	void visit(ExprStmt&) override;

	void visit(CallExpr&) override;

	void ret(ASTNodePtr stmt);
	ASTNodePtr get(StmtPtr& stmt);

	void emit(FunNodePtr node);

	std::string getIdOrError(ExprPtr& expr);
	ASTNodePtr seq(StmtPtr& stmt, ASTNodePtr node);

	Type resolveType(Token token);
	void seedTypes();
	void seedDecls();

public:
	Errors errors;

	Elaborator(ParseTree& tree);
	std::vector<FunNodePtr> run();
	SymTab<FunType> getDecls();
};
