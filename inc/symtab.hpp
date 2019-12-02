#pragma once
#include <memory>
#include <string>
#include <unordered_map>


template <typename T>
class Scope {
private:
	std::unordered_map<std::string, T> values;

	T* find(std::string key) {
		if (values.find(key) != values.end())
			return &values[key];

		if (enclosing)
			return enclosing->find(key);

		return nullptr;
	}

public:
	std::unique_ptr<Scope<T>> enclosing;

	Scope(std::unique_ptr<Scope<T>> enclosing=nullptr) :
		enclosing(std::move(enclosing))
	{}

	bool exists(const std::string& key) {
		return find(key) != nullptr;
	}

	void define(std::string key, T value) {
		T* v = find(key);
		assert (!v && "cannot redefine key");
		values[key] = value;
	}

	T& lookup(const std::string& key) {
		T* value = find(key);
		assert (value && "key does not exist");
		return *value;
	}
};


template <typename T>
class SymTab {
private:
	std::unique_ptr<Scope<T>> scope;

public:
	SymTab() :
		scope(std::make_unique<Scope<T>>())
	{}

	// the next four procedures are proxies
	// for those in the scope object
	bool exists(const std::string& key) {
		return scope->exists(key);
	}

	void define(const std::string& key, T value) {
		scope->define(key, value);
	}

	T& lookup(const std::string& key) {
		return scope->lookup(key);
	}

	void enter() {
		scope = std::make_unique<Scope<T>>( std::move(scope) );
	}

	void exit() {
		assert(scope->enclosing != nullptr && "stack underflow");
		scope = std::move(scope->enclosing);
	}
};
