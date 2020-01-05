#pragma once
#include <unordered_map>


template <typename T>
class Map {
private:
	std::unordered_map<std::string, T> items;

public:
	bool exists(const std::string& key) const {
		return items.find(key) != items.end();
	}

	T& lookup(const std::string& key) {
		assert(exists(key));
		return items[key];
	}

	void define(const std::string& key, const T& value) {
		// assert(!exists(key));
		items[key] = std::move(value);
	}

	// expose underlying iterator
	using iterator = typename std::unordered_map<std::string, T>::iterator;

	iterator begin() {
		return items.begin();
	};
	iterator end() {
		return items.end();
	};
};
