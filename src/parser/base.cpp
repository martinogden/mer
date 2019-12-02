#include <sstream>
#include "parser/base.hpp"


Token END(TokenType::END, "", true, 0, 0);


BaseParser::BaseParser(Lexer& lexer, TokenType terminator) :
	lexer(lexer),
	curr(END),
	terminator(terminator),
	errors("Parse error")
{}


bool BaseParser::isAtEnd() {
	return match(TokenType::END);
}


Token BaseParser::get() {
	return curr;
}


Token BaseParser::advance() {
	Token token = get();
	// skip lexical errors
	while (curr.type == TokenType::ERROR) {
		errors.add(curr.lexeme, curr);
		curr = lexer.nextToken();
	}

	curr = lexer.nextToken();
	return token;
}


bool BaseParser::match(TokenType type) {
	Token token = get();
	return token.type == type;
}


bool BaseParser::match(std::set<TokenType> types) {
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


bool BaseParser::accept(std::set<TokenType> types) {
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


void BaseParser::consumeLine() {
	while (!isAtEnd()) {
		if (accept(terminator))
			return;
		advance();
	}
}


ParseError::ParseError(std::string msg, Token token) :
	msg(msg),
	token(token)
{}


const char* ParseError::what() const noexcept {
	return msg.c_str();
}
