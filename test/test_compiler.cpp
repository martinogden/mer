#include "catch.hpp"
#include <string>
#include <vector>
#include "files.hpp"
#include "compiler.hpp"


const std::string dir = "fixtures/";

// TODO: check file compiles <==> file valid
TEST_CASE("Test fixtures", "[Compiler]") {
	std::vector<std::string> filenames = ls(dir);

	for (std::string& fn : filenames) {
		std::string src = read(dir + fn);
		compile(src);
	}
}
