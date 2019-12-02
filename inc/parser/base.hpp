#pragma once
#include <set>
#include "errors.hpp"
#include "lexer.hpp"


class BaseParser {
protected:
	Lexer& lexer;
	Token curr;
	TokenType terminator;

public:
	Errors errors;

	bool isAtEnd();
	Token get();
	Token advance();
	bool match(TokenType type);
	bool match(std::set<TokenType> types);
	bool accept(TokenType type);
	bool accept(std::set<TokenType> types);
	Token expect(TokenType type);
	void consumeLine();

	BaseParser(Lexer& lexer, TokenType terminator=TokenType::SEMICOLON);
	virtual ~BaseParser() = default;
};


class ParseError : public std::exception {
public:
	std::string msg;
	Token token;

	ParseError(std::string msg, Token token);
	const char* what() const noexcept override;
};
