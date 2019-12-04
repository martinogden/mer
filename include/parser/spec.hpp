#pragma once
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "cst/token.hpp"


class NullParser;
class LeftParser;

typedef std::shared_ptr<NullParser> NullParserPtr;
typedef std::shared_ptr<LeftParser> LeftParserPtr;


class ParserSpec {
private:
	// TODO write hash function for TokenType so we
	// can use the (faster lookup) unordered_(map|set)
	std::map<TokenType, NullParserPtr> null;
	std::map<TokenType, LeftParserPtr> left;

public:
	void add(TokenType type, NullParserPtr parser);
	void add(const std::set<TokenType>& types, NullParserPtr parser);
	void add(TokenType type, LeftParserPtr parser);
	void add(const std::set<TokenType>& types, LeftParserPtr parser);

	NullParserPtr getNullParser(TokenType type);
	LeftParserPtr getLeftParser(TokenType type);
};
