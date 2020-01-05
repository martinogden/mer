#include "parser/base.hpp"
#include "parser/expr.hpp"
#include "conversion.hpp"


CallParser::CallParser(int lbp) :
	LeftParser(lbp)
{}


ExprPtr CallParser::parse(ExprPtr left, Token& token, PrattParser& parser) {
	std::vector<ExprPtr> args;

	while (!parser.isAtEnd() && !parser.match(TokenType::RPAREN)) {
		ExprPtr arg = parser.expression(LOW);
		args.push_back( std::move(arg) );

		if (!parser.accept(TokenType::COMMA))
			break;
	}
	parser.expect(TokenType::RPAREN);

	return std::make_unique<CallExpr>(token, left->token.lexeme, std::move(args));
}


SubscriptParser::SubscriptParser(int lbp) :
		LeftParser(lbp)
{}


ExprPtr SubscriptParser::parse(ExprPtr left, Token& token, PrattParser& parser) {
	ExprPtr right = parser.expression( 0 );
	parser.expect(TokenType::RBRACK);
	return std::make_unique<SubscriptExpr>(token, std::move(left), std::move(right));
}


ArrowParser::ArrowParser(int lbp) :
		LeftParser(lbp)
{}


ExprPtr ArrowParser::parse(ExprPtr left, Token& token, PrattParser& parser) {
	Token t = parser.expect(TokenType::IDENT);
	return std::make_unique<ArrowExpr>(token, std::move(left), std::move(t.lexeme));
}


DotParser::DotParser(int lbp) :
		LeftParser(lbp)
{}


ExprPtr DotParser::parse(ExprPtr left, Token& token, PrattParser& parser) {
	Token t = parser.expect(TokenType::IDENT);
	return std::make_unique<DotExpr>(token, std::move(left), std::move(t.lexeme));
}


TernaryParser::TernaryParser(int lbp) :
	LeftParser(lbp)
{}


ExprPtr TernaryParser::parse(ExprPtr cond, Token& token, PrattParser& parser) {
	ExprPtr then = parser.expression( getRBP() );
	parser.expect(TokenType::COLON);
	ExprPtr otherwise = parser.expression( getRBP() );
	return std::make_unique<TernaryExpr>(token, std::move(cond), std::move(then), std::move(otherwise));
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


ExprPtr BinaryParser::parse(ExprPtr left, Token& token, PrattParser& parser) {
	BinOp op = toBinOp(token.type);
	ExprPtr right = parser.expression( getRBP() );
	return std::make_unique<BinaryExpr>(token, op, std::move(left), std::move(right));
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

ExprPtr UnaryParser::parse(Token& token, PrattParser& parser) {
	UnOp op = toUnOp(token.type);
	ExprPtr expr = parser.expression( getBP() );
	return std::make_unique<UnaryExpr>(token, op, std::move(expr));
}


LiteralParser::LiteralParser(int bp) :
	NullParser(bp)
{}


ExprPtr LiteralParser::parse(Token& token, PrattParser& parser) {
	switch(token.type) {
		case TokenType::NUM:
			return std::make_unique<LiteralExpr>(token, token.value);
		case TokenType::TRUE:
			return std::make_unique<LiteralExpr>(token, true);
		case TokenType::FALSE:
			return std::make_unique<LiteralExpr>(token, false);
		case TokenType::NUL: {
			LiteralExpr::Value value {.i=0};
			return  std::make_unique<LiteralExpr>(token, Type::INDEF, value);
		}
		default:
			// should never get here
			throw ParseError("Invalid literal", token);
	}
}


IdentParser::IdentParser(int bp) :
	NullParser(bp)
{}


ExprPtr IdentParser::parse(Token& token, PrattParser& parser) {
	return std::make_unique<IdExpr>(token, token.lexeme);
}


ParensParser::ParensParser(int bp) :
	NullParser(bp)
{}


ExprPtr ParensParser::parse(Token& token, PrattParser& parser) {
	ExprPtr expr = parser.expression( getBP() );
	parser.expect(TokenType::RPAREN);
	return std::move(expr);
}


DerefParser::DerefParser(int bp) :
		NullParser(bp)
{}


ExprPtr DerefParser::parse(Token& token, PrattParser& parser) {
	ExprPtr expr = parser.expression( getBP() );
	return std::make_unique<DerefExpr>(token, std::move(expr));
}
