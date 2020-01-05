#pragma once
#include "token.hpp"
#include "cst/cst.hpp"
#include "parser/base.hpp"
#include "parser/spec.hpp"


class PrattParser;


class NullParser {
protected:
	int bp;

public:
	NullParser(int bp);
	virtual ~NullParser() = default;

	virtual int getBP();
	virtual ExprPtr parse(Token& token, PrattParser& parser) = 0;
};


class LeftParser {
protected:
	int lbp;
	int rbp;

public:
	LeftParser(int lbp);
	LeftParser(int lbp, int rbp);
	virtual ~LeftParser() = default;

	virtual int getLBP();
	virtual int getRBP();
	virtual ExprPtr parse(ExprPtr left, Token& token, PrattParser& parser) = 0;
};


class PrattParser : public BaseParser {
protected:
	ParserSpec spec;

public:
	ExprPtr expression(int rbp=0);
	PrattParser(ParserSpec&& spec, Lexer& lexer, uint lookahead);
	~PrattParser() override = default;
};
