#include "parser/pratt.hpp"


PrattParser::PrattParser(ParserSpec&& spec, Lexer& lexer, uint lookahead) :
	BaseParser(lexer, lookahead),
	spec(spec)
{}


ExprPtr PrattParser::expression(int rbp) {
	Token token = advance();
	NullParserPtr nparser = spec.getNullParser(token.type);

	if (!nparser)
		throw ParseError("Unexpected token: " + token.lexeme, token);

	ExprPtr left = nparser->parse(token, *this);

	while (true) {
		token = get();
		LeftParserPtr lparser = spec.getLeftParser(token.type);

		if ( !lparser || rbp >= lparser->getLBP() )
			break;

		advance();
		left = lparser->parse(std::move(left), token, *this);
	}

	return std::move(left);
}


NullParser::NullParser(int bp) :
	bp(bp)
{}


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


int LeftParser::getLBP() {
	return lbp;
}


int LeftParser::getRBP() {
	return rbp;
}
