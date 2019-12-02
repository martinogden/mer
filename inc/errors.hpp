#pragma once
#include <string>
#include <vector>
#include "token.hpp"


class Errors {
private:
	std::vector<std::string> errors;
	std::string label;

public:
	Errors(std::string label="Error");

	void append(Errors&);
	void add(const std::string&);
	void add(const std::string&, Token&);
	std::vector<std::string>& get();
	bool exist();
};
