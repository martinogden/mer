#pragma once
#include <string>


class Generator {
private:
	int nextTmp;
	int nextLabel;

public:
	Generator()
		: nextTmp(1), nextLabel(1) {}

	std::string tmp() {
		return "#" + std::to_string(nextTmp++);
	}

	std::string label() {
		return "L" + std::to_string(nextLabel++);
	}
};
