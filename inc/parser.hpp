#pragma once
#include <string>
#include <vector>
#include "lexer.hpp"
#include "ast.hpp"


class Parser {
private:
	Lexer lexer;
	Token curr;
	Token next;
	std::vector<std::string> errors;

	// mutually recursive parsing functions
	void programPrologue();
	Token programEpilogue();
	Prog* program();
	Stmt* statement();
	Expr* expression();

	Stmt* block(Token& token);
	Stmt* declStmt(Token& token);
	Stmt* returnStmt(Token& token);
	Stmt* assignStmt(Token& token);

	Expr* addition();
	Expr* multiplication();
	Expr* unary();
	Expr* primary();
	std::string lvalue();

	// navigation
	bool isAtEnd();
	bool match(TokenType type);
	bool accept(TokenType type);
	void expect(TokenType type);

	void advance();
	Token get();
	Token peek();
	void consumeLine();
	void addError(std::string msg, Token& token);

	// token -> operator conversion
	UnOp toUnOp(TokenType type);
	BinOp toBinOp(TokenType type);
	AsnOp toAsnOp(TokenType type);
public:
	Parser(std::string src);
	std::vector<std::string>& getErrors();
	Prog* run();
};
