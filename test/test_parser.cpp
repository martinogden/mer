#include "catch.hpp"
#include <string>
#include "parser.hpp"
#include "tree_printer.hpp"


void testParse(std::string src, std::string expected) {
	Parser parser(src);
	Prog* program = parser.run();
	TreePrinter printer(program);

	REQUIRE( parser.getErrors().size() == 0 );
	REQUIRE( printer.run() == expected );
}


TEST_CASE("Assignment is parsed correctly", "[Parser]") {
	testParse(
		"int main() {int x; x=2*(3+4);}",
		"(int x)\n(= x (* 2 (+ 3 4)))"
	);
}


TEST_CASE("Return statement is parsed correctly", "[Parser]") {
	testParse(
		"int main() {return 22/7;}",
		"(return (/ 22 7))"
	);
}
