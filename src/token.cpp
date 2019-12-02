#include <sstream>
#include "token.hpp"


Token::Token(TokenType type, std::string lexeme, int value, uint line, uint col) :
	type(type),
	lexeme(std::move(lexeme)),
	value(value),
	line(line),
	col(col)
{}


std::unordered_set<std::string> reserved = {
	"struct",
	"typedef",
	"if",
	"else",
	"while",
	"for",
	"continue",
	"break",
	"return",
	"true",
	"false",
	"NULL",
	"alloc",
	"alloc_array",
	"int",
	"bool",
	"void",
	"char",
	"string",
	"typedef",
};


std::unordered_map<std::string, TokenType> keywords = {
	{"return", TokenType::RETURN},
	{"if", TokenType::IF},
	{"else", TokenType::ELSE},
	{"while", TokenType::WHILE},
	{"for", TokenType::FOR},
	{"true", TokenType::TRUE},
	{"false", TokenType::FALSE},
	{"typedef", TokenType::TYPEDEF},
};


std::string error(const std::string& msg, Token& token) {
	std::stringstream buffer;
	buffer << "(line " << token.line << ", column " << token.col << ')' << msg;
	return buffer.str();
}
