#include "parser/spec.hpp"


void ParserSpec::add(TokenType type, NullParser* parser) {
	assert(null.find(type) == null.end() && "Duplicate parser error");
	null[type] = parser;
}


void ParserSpec::add(TokenType type, LeftParser* parser) {
	assert(left.find(type) == left.end() && "Duplicate parser error");
	left[type] = parser;
}


void ParserSpec::add(std::set<TokenType> types, NullParser* parser) {
	for (TokenType type : types)
		add(type, parser);
}


void ParserSpec::add(std::set<TokenType> types, LeftParser* parser) {
	for (TokenType type : types)
		add(type, parser);
}


NullParser* ParserSpec::getNullParser(TokenType type) {
	if (null.find(type) == null.end())
		return nullptr;

	return null[type];
}


LeftParser* ParserSpec::getLeftParser(TokenType type) {
	if (left.find(type) == left.end())
		return nullptr;

	return left[type];
}
