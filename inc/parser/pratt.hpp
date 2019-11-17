#pragma once
#include "token.hpp"
#include "parse-tree.hpp"
#include "parser/base.hpp"
#include "parser/spec.hpp"


class PrattParser;


class NullParser {
protected:
	int bp;

public:
	NullParser(int bp);
	virtual ~NullParser() {}

	virtual int getBP();
	virtual Expr* parse(Token& token, PrattParser& parser);
};


class LeftParser {
protected:
	int lbp;
	int rbp;

public:
	LeftParser(int lbp);
	LeftParser(int lbp, int rbp);
	virtual ~LeftParser() {}

	virtual int getLBP();
	virtual int getRBP();
	virtual Expr* parse(Expr* left, Token& token, PrattParser& parser);
};


class PrattParser : public BaseParser {
protected:
	ParserSpec spec;

public:
	Expr* expression(int rbp=0);
	PrattParser(ParserSpec&& spec, Lexer& lexer);
	virtual ~PrattParser() {}
};
