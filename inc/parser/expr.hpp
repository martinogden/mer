#pragma once
#include "parser/pratt.hpp"


enum class Assoc {
	LEFT,
	RIGHT,
};


enum Precedence {
	LOW     =   0,
	RPAREN  =  10,
	TERN    =  20,
	LOR     =  30,
	LAND    =  40,
	BWOR    =  50,
	XOR     =  60,
	BWAND   =  70,
	EQUAL   =  80,
	COMP    =  90,
	ADD     = 100,
	MUL     = 110,
	UNARY   = 120,
	CALL    = 130,
	LPAREN  = 140,
};


class CallParser : public LeftParser {
public:
	CallParser(int lbp=CALL);
	Expr* parse(Expr* cond, Token& token, PrattParser& parser) override;
};


class TernaryParser : public LeftParser {
public:
	TernaryParser(int lbp=TERN);
	Expr* parse(Expr* cond, Token& token, PrattParser& parser) override;
	int getRBP() override;
};


class BinaryParser : public LeftParser {
private:
	Assoc assoc;

public:
	BinaryParser(int lbp, Assoc assoc=Assoc::LEFT);
	BinaryParser(int lbp, int rbp, Assoc assoc);
	Expr* parse(Expr* left, Token& token, PrattParser& parser) override;
	int getRBP() override;
};


class UnaryParser : public NullParser {
public:
	UnaryParser(int bp=UNARY);
	Expr* parse(Token& token, PrattParser& parser) override;
};


class LiteralParser : public NullParser {
public:
	LiteralParser(int bp=LOW);
	Expr* parse(Token& token, PrattParser& parser) override;
};


class IdentParser : public NullParser {
public:
	IdentParser(int bp=LOW);
	Expr* parse(Token& token, PrattParser& parser) override;
};


class ParensParser : public NullParser {
public:
	ParensParser(int bp=RPAREN);
	Expr* parse(Token& token, PrattParser& parser) override;
};
