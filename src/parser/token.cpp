#include <sstream>
#include "parser/token.hpp"
#include <cstdint>


Token::Token(TokenType type, std::string lexeme, int64_t value, uint line, uint col, bool isTypeAlias=false) :
	type(type),
	lexeme(std::move(lexeme)),
	value(value),
	line(line),
	col(col),
	isTypeAlias(isTypeAlias)
{}


Token END(TokenType::END, "", true, 0, 0);


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
	{"NULL", TokenType::NUL},
	{"typedef", TokenType::TYPEDEF},
	{"struct", TokenType::STRUCT},
	{"alloc", TokenType::ALLOC},
	{"alloc_array", TokenType::ALLOC_ARRAY},
};


std::string error(const std::string& msg, Token& token) {
	std::stringstream buffer;
	buffer << "(line " << token.line << ", column " << token.col << ')' << msg;
	return buffer.str();
}
