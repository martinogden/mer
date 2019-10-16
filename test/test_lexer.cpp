#include "catch.hpp"
#include <vector>
#include "lexer.hpp"


const std::string MODULE = "[Lexer]";

void expect(std::string src, std::vector<Token> expected) {
	Lexer lexer(src);

	for (auto const& exp : expected) {
		Token actual = lexer.nextToken();
		REQUIRE( actual == exp );
	}
}


TEST_CASE("Numbers have correct values", MODULE) {
	expect("0xff 1111", {
		{TokenType::NUM, "0xff", 255, 1, 1},
		{TokenType::NUM, "1111", 1111, 1, 6},
	});
}


TEST_CASE("Numbers too big", MODULE) {
	expect("0xFFFFFFFF1", {
		{TokenType::ERROR, "integer value too big", 0, 1, 1},
	});
}


TEST_CASE("Spacing", MODULE) {
	expect("id\n{ \t}", {
		{TokenType::IDENT, "id", 0, 1, 1},
		{TokenType::LBRACE, "{", 0, 2, 1},
		{TokenType::RBRACE, "}", 0, 2, 4},
	});
}


TEST_CASE("Valid program", MODULE) {
	expect("int main() {\n\tint x = 3;\n\treturn x;\n}\n", {
		{TokenType::INT, "int", 0, 1, 1},
		{TokenType::IDENT, "main", 0, 1, 5},
		{TokenType::LPAREN, "(", 0, 1, 9},
		{TokenType::RPAREN, ")", 0, 1, 10},
		{TokenType::LBRACE, "{", 0, 1, 12},
		{TokenType::INT, "int", 0, 2, 2},
		{TokenType::IDENT, "x", 0, 2, 6},
		{TokenType::EQL, "=", 0, 2, 8},
		{TokenType::NUM, "3", 3, 2, 10},
		{TokenType::SEMICOLON, ";", 0, 2, 11},
		{TokenType::RETURN, "return", 0, 3, 2},
		{TokenType::IDENT, "x", 0, 3, 9},
		{TokenType::SEMICOLON, ";", 0, 3, 10},
		{TokenType::RBRACE, "}", 0, 4, 1},
	});
}