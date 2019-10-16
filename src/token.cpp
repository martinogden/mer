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
	{"int", TokenType::INT},
	{"return", TokenType::RETURN},
};


std::string error(std::string msg, Token& token) {
	std::stringstream buffer;
	buffer << "Error on line ";
	buffer << token.line << ", column " << token.col << "." << std::endl;
	buffer << msg;
	return buffer.str();
}
