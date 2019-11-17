#include <sstream>
#include "token.hpp"


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
	"assert",
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
};


std::unordered_map<std::string, TokenType> keywords = {
	{"return", TokenType::RETURN},
	{"if", TokenType::IF},
	{"else", TokenType::ELSE},
	{"while", TokenType::WHILE},
	{"for", TokenType::FOR},
	{"true", TokenType::TRUE},
	{"false", TokenType::FALSE},
};


std::string error(std::string msg, Token& token) {
	std::stringstream buffer;
	buffer << "(line " << token.line << ", column " << token.col << ')' << msg;
	return buffer.str();
}
