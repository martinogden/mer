#pragma once
#include <string>
#include <vector>
#include "parse-tree.hpp"
#include "parser/expr.hpp"


class LeftParser;
class NullParser;


class Parser : public PrattParser {
private:
	std::vector<StmtPtr> program();

	StmtPtr declaration();
	StmtPtr typedefStmt(Token& token);
	StmtPtr function(Token& token);
	std::vector<DeclStmtPtr> params(Token& token);

	StmtPtr statement();
	StmtPtr ifStmt(Token& token);
	StmtPtr whileStmt(Token& token);
	StmtPtr forStmt(Token& token);
	StmtPtr returnStmt(Token& token);
	StmtPtr declStmt(Token& token);
	StmtPtr block(Token& token);
	StmtPtr simpleStmt(Token& token);
	ExprPtr condition();

	// helper to parse for stmts
	StmtPtr declOrSimpleStmtOpt(TokenType terminator);

	void seedTypes();

public:
	Parser(Lexer& lexer);
	ParseTree run();
};
