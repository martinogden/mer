#include "files.hpp"
#include "compiler.hpp"


int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Error: please supply a filename" << std::endl;
		return 1;
	}

	std::string fn(argv[1]);
	std::string src;

	try {
		src = read(fn);
	} catch (std::invalid_argument e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}

	compile(src);
}
