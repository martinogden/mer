#pragma once
#include <string>


class Generator {
private:
	int nextTmp;
	int nextLabel;

public:
	Generator();
	std::string tmp();
	std::string label();
};
