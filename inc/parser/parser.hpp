#pragma once
#include <string>
#include "parse-tree.hpp"
#include "parser/expr.hpp"


class LeftParser;
class NullParser;


class Parser : public PrattParser {
private:
	BlockStmt* program();
	void prologue();
	void epilogue();

	Stmt* statement();
	Stmt* ifStmt(Token& token);
	Stmt* whileStmt(Token& token);
	Stmt* forStmt(Token& token);
	Stmt* returnStmt(Token& token);
	Stmt* declStmt(Token& token);
	Stmt* block(Token& token);
	Stmt* simpleStmt(Token& token);
	Expr* condition();

	// helper to parse for stmts
	Stmt* declOrSimpleStmtOpt(TokenType terminator);

public:
	Parser(Lexer& lexer);
	ParseTree* run();
};
