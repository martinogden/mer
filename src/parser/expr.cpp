#include "parser/base.hpp"
#include "parser/expr.hpp"
#include "conversion.hpp"


TernaryParser::TernaryParser(int lbp) :
	LeftParser(lbp)
{}


Expr* TernaryParser::parse(Expr* cond, Token& token, PrattParser& parser) {
	Expr* then = parser.expression( getRBP() );
	parser.expect(TokenType::COLON);
	Expr* otherwise = parser.expression( getRBP() );
	return new TernaryExpr(token, cond, then, otherwise);
}


int TernaryParser::getRBP() {
	return rbp - 1;  // ? is right assoc
}


BinaryParser::BinaryParser(int lbp, Assoc assoc) :
	LeftParser(lbp),
	assoc(assoc)
{}


BinaryParser::BinaryParser(int lbp, int rbp, Assoc assoc) :
	LeftParser(lbp, rbp),
	assoc(assoc)
{}


Expr* BinaryParser::parse(Expr* left, Token& token, PrattParser& parser) {
	BinOp op = toBinOp(token.type);
	Expr* right = parser.expression( getRBP() );
	return new BinaryExpr(token, op, left, right);
}


int BinaryParser::getRBP() {
	switch(assoc) {
		case Assoc::LEFT:
			return rbp;
		case Assoc::RIGHT:
			return rbp - 1;
	}
}


UnaryParser::UnaryParser(int bp) :
	NullParser(bp)
{}

Expr* UnaryParser::parse(Token& token, PrattParser& parser) {
	UnOp op = toUnOp(token.type);
	Expr* expr = parser.expression( getBP() );
	return new UnaryExpr(token, op, expr);
}


LiteralParser::LiteralParser(int bp) :
	NullParser(bp)
{}


Expr* LiteralParser::parse(Token& token, PrattParser& parser) {
	switch(token.type) {
		case TokenType::NUM:
			return new LiteralExpr(token, token.value);
		case TokenType::TRUE:
			return new LiteralExpr(token, true);
		case TokenType::FALSE:
			return new LiteralExpr(token, false);
		default:
			// should never get here
			throw ParseError("Invalid literal", token);
	}
}


IdentParser::IdentParser(int bp) :
	NullParser(bp)
{}


Expr* IdentParser::parse(Token& token, PrattParser& parser) {
	return new IdExpr(token, token.lexeme);
}


ParensParser::ParensParser(int bp) :
	NullParser(bp)
{}


Expr* ParensParser::parse(Token& token, PrattParser& parser) {
	Expr* expr = parser.expression( getBP() );
	parser.expect(TokenType::RPAREN);
	return expr;
}
