#pragma once
#include <set>
#include "errors.hpp"
#include "lexer.hpp"


class BaseParser {
private:
	void fetch();

protected:
	Lexer& lexer;
	std::vector<Token> tokens;
	uint curr;
	uint lookahead;
	TokenType terminator;

public:
	Errors errors;

	bool isAtEnd();
	Token get();
	Token peek(uint lhd);
	Token advance();
	bool match(TokenType type);
	bool match(const std::set<TokenType> &types);
	bool accept(TokenType type);
	bool accept(const std::set<TokenType>& types);
	Token expect(TokenType type);
	void consume(TokenType until);

	BaseParser(Lexer& lexer, uint lookahead=0, TokenType terminator=TokenType::SEMICOLON);
	virtual ~BaseParser() = default;
};


class ParseError : public std::exception {
public:
	std::string msg;
	Token token;

	ParseError(std::string msg, Token token);
	const char* what() const noexcept override;
};
