#include "inst/generator.hpp"


Generator::Generator() :
	nextTmp(1),
	nextLabel(1)
{}

std::string Generator::tmp() {
	return "#" + std::to_string(nextTmp++);
}

std::string Generator::label() {
	return "L" + std::to_string(nextLabel++);
}

