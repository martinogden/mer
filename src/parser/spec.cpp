#include "parser/spec.hpp"


void ParserSpec::add(TokenType type, NullParserPtr parser) {
	assert(null.find(type) == null.end() && "Duplicate parser error");
	null.emplace(type, parser);
}


void ParserSpec::add(TokenType type, LeftParserPtr parser) {
	assert(left.find(type) == left.end() && "Duplicate parser error");
	left.emplace(type, parser);
}


void ParserSpec::add(const std::set<TokenType>& types, NullParserPtr parser) {
	for (TokenType type : types)
		add(type, parser);
}


void ParserSpec::add(const std::set<TokenType>& types, LeftParserPtr parser) {
	for (TokenType type : types)
		add(type, parser);
}


NullParserPtr ParserSpec::getNullParser(TokenType type) {
	if (null.find(type) == null.end())
		return nullptr;
	else
		return null[type];
}


LeftParserPtr ParserSpec::getLeftParser(TokenType type) {
	if (left.find(type) == left.end())
		return nullptr;
	else
		return left[type];
}
