#pragma once
#include <string>
#include <unordered_map>


template <typename T>
class SymTab {
private:
	std::unordered_map<std::string, T> values;

public:
	SymTab() {}

	bool exists(std::string key) {
		return values.find(key) != values.end();
	}

	void assign(std::string key, T value) {
		assert (exists(key) && "key does not exist");
		values[key] = value;
	}

	void define(std::string key, T value) {
		assert (!exists(key) && "cannot redefine key");
		values[key] = value;
	}

	T& lookup(std::string key) {
		assert (exists(key) && "key does not exist");
		return values[key];
	}
};
