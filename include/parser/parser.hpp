#pragma once
#include <string>
#include <vector>
#include "cst/cst.hpp"
#include "parser/expr.hpp"


class LeftParser;
class NullParser;


class Parser : public PrattParser {
private:
	std::vector<StmtPtr> program();

	TypePtr type(bool optional=false);
	TypePtr typeOpt();

	StmtPtr declaration();
	StmtPtr finishFun(Token& token, Token& idToken, TypePtr type);
	StmtPtr finishStruct(Token& token, std::string& name);

	StmtPtr statement();
	StmtPtr ifStmt(Token& token);
	StmtPtr whileStmt(Token& token);
	StmtPtr forStmt(Token& token);
	StmtPtr returnStmt(Token& token);
	StmtPtr finishDeclStmt(Token& token, TypePtr type);
	StmtPtr block(Token& token);
	StmtPtr simpleStmt(Token& token);
	StmtPtr typedefStmt(Token& token);

	std::vector<DeclStmtPtr> fields();
	std::vector<DeclStmtPtr> params(Token& token);

	ExprPtr expression(int rbp) override;
	ExprPtr condition();
	ExprPtr lvalue();

	// helper to parse for stmts
	StmtPtr declOrSimpleStmtOpt(TokenType terminator);

	void seedTypes();

public:
	Parser(Lexer& lexer);
	ParseTree run();
};
