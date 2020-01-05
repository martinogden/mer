#pragma once
#include "map.hpp"
#include <unordered_set>
#include <vector>
#include <string>
#include "generator.hpp"
#include "cst/cst.hpp"
#include "cst/visitor.hpp"
#include "ast/ast.hpp"
#include "errors.hpp"
#include "symtab.hpp"
#include "type/canonicaliser.hpp"
#include "expr/lvalue/checker.hpp"
#include "expr/lvalue/canonicaliser.hpp"


// 'elaborates' parse tree into list of function definitions
// where elaborate = lower: ParseTree -> [ASTNode]
class Elaborator : public CSTVisitor {
private:
	ParseTree& tree;
	Generator& gen;
	ASTNodePtr retval;
	std::vector<FunNodePtr> functions;

	SymTab<TypePtr> types;
	Map<FunTypePtr> funTypes;
	Map<StructTypePtr> structTypes;

	std::unordered_map<std::string, std::unordered_set<std::string>> structAliases;
	std::unordered_set<std::string> funNames;

	void visit(FunDecl&) override;
	void visit(FunDefn&) override;
	void visit(StructDecl&) override;
	void visit(StructDefn&) override;
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

	void ret(ASTNodePtr stmt);
	ASTNodePtr get(StmtPtr& stmt);
	ExprPtr get(ExprPtr& expr);
	std::pair<std::vector<StmtPtr>, ExprPtr> getLValue(ExprPtr& expr);

	void emit(FunNodePtr node);
	ASTNodePtr seq(StmtPtr& stmt, ASTNodePtr node);
	ASTNodePtr seq(std::vector<StmtPtr>& stmts, ASTNodePtr node);

	TypePtr canon(TypePtr type);
	bool checkFunType(FunDecl& decl, FunType& type);
	bool checkStructType(StructDefn& defn, StructType& type);
	void seedTypes();

public:
	Errors errors;

	Elaborator(ParseTree& tree, Generator& gen);
	std::vector<FunNodePtr> run();

	Map<std::shared_ptr<FunType>> getFunTypes();
	Map<std::shared_ptr<StructType>> getStructTypes();
};
