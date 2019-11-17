#pragma once
#include "parser/pratt.hpp"


enum class Assoc {
	LEFT,
	RIGHT,
};


enum Precedence {
	LOW     =   0,
	RPAREN  =  10,
	ASSIGN  =  20,
	TERN    =  30,
	BWOR    =  40,
	BWAND   =  50,
	LOR     =  60,
	XOR     =  70,
	LAND    =  80,
	EQUAL   =  90,
	COMP    = 100,
	SHIFT   = 110,
	ADD     = 120,
	MUL     = 130,
	CALL    = 140,
	UNARY   = 150,
	LPAREN  = 160,
};


enum Level {
	EXPR   = 10,
	DECL   = 20,
	CTRL   = 30,
	BLOCK  = 40,
};


class TernaryParser : public LeftParser {
public:
	TernaryParser(int lbp=TERN);
	Expr* parse(Expr* cond, Token& token, PrattParser& parser);
};


class BinaryParser : public LeftParser {
private:
	Assoc assoc;

public:
	BinaryParser(int lbp, Assoc assoc=Assoc::LEFT);
	BinaryParser(int lbp, int rbp, Assoc assoc);
	Expr* parse(Expr* left, Token& token, PrattParser& parser);
	int getRBP();
};


class UnaryParser : public NullParser {
public:
	UnaryParser(int bp=UNARY);
	Expr* parse(Token& token, PrattParser& parser);
};


class LiteralParser : public NullParser {
public:
	LiteralParser(int bp=LOW);
	Expr* parse(Token& token, PrattParser& parser);
};


class IdentParser : public NullParser {
public:
	IdentParser(int bp=LOW);
	Expr* parse(Token& token, PrattParser& parser);
};


class ParensParser : public NullParser {
public:
	ParensParser(int bp=RPAREN);
	Expr* parse(Token& token, PrattParser& parser);
};
