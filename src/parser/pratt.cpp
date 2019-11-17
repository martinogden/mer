#include "parser/pratt.hpp"


PrattParser::PrattParser(ParserSpec&& spec, Lexer& lexer) :
	BaseParser(lexer),
	spec(spec)
{}


Expr* PrattParser::expression(int rbp) {
	Token token = advance();
	NullParser* nparser = spec.getNullParser(token.type);

	if (!nparser)
		throw ParseError("Unexpected token: " + token.lexeme, token);

	Expr* left = nparser->parse(token, *this);

	while (true) {
		token = get();
		LeftParser* lparser = spec.getLeftParser(token.type);

		if ( !lparser || rbp >= lparser->getLBP() )
			break;

		advance();
		left = lparser->parse(left, token, *this);
	}

	return left;
}


NullParser::NullParser(int bp) :
	bp(bp)
{}


Expr* NullParser::parse(Token& token, PrattParser& parser) {
	return nullptr;
}


int NullParser::getBP() {
	return bp;
}


LeftParser::LeftParser(int lbp) :
	lbp(lbp),
	rbp(lbp)
{}


LeftParser::LeftParser(int lbp, int rbp) :
	lbp(lbp),
	rbp(rbp)
{}


Expr* LeftParser::parse(Expr* expr, Token& token, PrattParser& parser) {
	return nullptr;
}


int LeftParser::getLBP() {
	return lbp;
}


int LeftParser::getRBP() {
	return rbp;
}
