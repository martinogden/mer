#include "errors.hpp"


Errors::Errors(std::string label) :
	label(label)
{}


void Errors::append(Errors& other) {
	for (auto& msg : other.get())
		add(msg);
}


void Errors::add(std::string msg) {
	errors.push_back(msg);
}


void Errors::add(std::string msg, Token& token) {
	std::string str = "[" + label + "] " + msg;
	add( error(str, token) );
}


std::vector<std::string>& Errors::get() {
	return errors;
}


bool Errors::exist() {
	return errors.size() > 0;
}
