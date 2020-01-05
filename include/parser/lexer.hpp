#pragma once
#include <unordered_set>
#include <string>
#include "token.hpp"


class Lexer {
private:
	std::string src;
	uint start;
	uint curr;
	uint line;
	uint col;
	std::unordered_set<std::string> types;
	std::unordered_set<std::string> typeAliases;

	// src navigation
	void incrCol();
	void incrLine();
	void begin();
	char prev();
	char get();
	char peek();
	char advance();
	bool accept(char c);

	// lexers
	void whitespace();
	void singleLineComment();
	int multiLineComment();
	Token op(char c);
	Token number();
	Token hex();
	Token dec();
	Token ident();

	// helpers
	bool isDecDigit(char c);
	bool isHexDigit(char c);
	bool isAlpha(char c);
	bool isAlNum(char c);
	bool isAlphaUnder(char c);
	std::string getLexeme();

	Token emit(TokenType type, std::string lexeme, int64_t value, bool isTypeAlias=false);
	Token emit(TokenType type, std::string lexeme);
	Token emit(TokenType type, int64_t value);
	Token emit(TokenType type);
	Token emitTypeAlias(std::string lexeme);
	Token error(std::string msg);
public:
	Lexer(std::string src);
	Token nextToken();
	bool isAtEnd();
	void addType(std::string type);
	void addTypeAlias(std::string type);
};
