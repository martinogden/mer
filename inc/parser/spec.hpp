#pragma once
#include <map>
#include <set>
#include "token.hpp"


class NullParser;
class LeftParser;


class ParserSpec {
private:
	// TODO write hash function for TokenType so we
	// can use the (faster lookup) unordered_(map|set)
	std::map<TokenType, NullParser*> null;
	std::map<TokenType, LeftParser*> left;

public:
	void add(TokenType type, NullParser* parser);
	void add(std::set<TokenType> types, NullParser* parser);
	void add(TokenType type, LeftParser* parser);
	void add(std::set<TokenType> types, LeftParser* parser);

	NullParser* getNullParser(TokenType type);
	LeftParser* getLeftParser(TokenType type);
};
