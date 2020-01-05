#include <sstream>
#include "parser/base.hpp"


BaseParser::BaseParser(Lexer& lexer, uint lookahead, TokenType terminator) :
	lexer(lexer),
	tokens(lookahead+1, END),
	curr(0),
	lookahead(lookahead),
	terminator(terminator),
	errors("Parse error")
{}


bool BaseParser::isAtEnd() {
	return match(TokenType::END);
}


Token BaseParser::get() {
	return tokens[curr % (lookahead+1)];
}


Token BaseParser::peek(uint lhd) {
	assert(lhd <= lookahead && "invalid lookahead");
	return tokens[(curr+lhd) % (lookahead+1)];
}


void BaseParser::fetch() {
	tokens[curr++ % (lookahead + 1)] = lexer.nextToken();
}


Token BaseParser::advance() {
	Token token = get();
	// skip lexical errors
	while (get().type == TokenType::ERROR) {
		errors.add(get().lexeme, get());
		fetch();
	}

	fetch();
	return token;
}


bool BaseParser::match(TokenType type) {
	Token token = get();
	return token.type == type;
}


bool BaseParser::match(const std::set<TokenType>& types) {
	for (TokenType type : types) {
		if (match(type))
			return true;
	}

	return false;
}


bool BaseParser::accept(TokenType type) {
	if (match(type)) {
		advance();
		return true;
	}

	return false;
}


bool BaseParser::accept(const std::set<TokenType>& types) {
	if (match(types)) {
		advance();
		return true;
	}

	return false;
}


Token BaseParser::expect(TokenType type) {
	Token token = get();

	if (!accept(type)) {
		std::stringstream msg;
		msg << "Unexpected token: ";
		msg << "expected: \"" << type << "\", ";
		msg << "got: \"" << token.type << "\".";
		throw ParseError(msg.str(), token);
	}

	return token;
}


void BaseParser::consume(TokenType until) {
	while (!isAtEnd() && !accept(until))
		advance();
}


ParseError::ParseError(std::string msg, Token token, TokenType terminator) :
	msg(std::move(msg)),
	token(std::move(token)),
	terminator(terminator)
{}


const char* ParseError::what() const noexcept {
	return msg.c_str();
}
