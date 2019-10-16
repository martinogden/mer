#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>


typedef unsigned int uint;


enum struct TokenType {
	END,
	ERROR,

	// l0 tokens
	IDENT,
	NUM,
	RESERVED,

	// ops
	// maybe sure these...
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	EQL,
	// ...are in the same order as these
	ADD_EQL,
	SUB_EQL,
	MUL_EQL,
	DIV_EQL,
	MOD_EQL,
	// EQL_EQL // not yet,

	// punctuation
	SEMICOLON,
	LPAREN,
	RPAREN,
	LBRACE,
	RBRACE,

	// keywords
	RETURN,
	INT,
};



struct Token {
	TokenType type;
	std::string lexeme;
	uint value;
	uint line;
	uint col;

	Token(TokenType type, std::string lexeme, uint value, uint line, uint col) :
		type(type),
		lexeme(lexeme),
		value(value),
		line(line),
		col(col)
	{}

	bool operator==(const Token& other) const;
	friend std::ostream& operator<<(std::ostream& output, const Token& token);
};


std::string error(std::string msg, Token& token);


std::ostream& operator<<(std::ostream& output, const TokenType& type);

extern std::unordered_map<std::string, TokenType> keywords;
extern std::unordered_set<std::string> reserved;
