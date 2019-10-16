#pragma once
#include <vector>
#include "visitor.hpp"
#include "ast.hpp"
#include "symtab.hpp"
#include "token.hpp"


class Semant : public Visitor {
private:
	Prog* program;
	SymTab<bool> env;
	bool hasReturned;
	std::vector<std::string> errors;

public:
	Semant(Prog* program);
	bool run();
	void addError(std::string, Token& token);
	std::vector<std::string>& getErrors();

	void visit(BinaryExpr*);
	void visit(UnaryExpr*);
	void visit(LiteralExpr*);
	void visit(VarExpr*);

	void visit(DeclStmt*);
	void visit(ReturnStmt*);
	void visit(BlockStmt*);
	void visit(AssignStmt*);

	void visit(Prog*);
};
