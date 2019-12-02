#pragma once
#include <string>
#include <unordered_map>


template <typename T>
class Scope {
private:
	std::unordered_map<std::string, T> values;
	Scope<T>* encl;

	T* find(std::string key) {
		if (values.find(key) != values.end())
			return &values[key];

		if (encl)
			return encl->find(key);

		return nullptr;
	}

public:
	Scope(Scope<T>* encl=nullptr) :
		encl(encl)
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

	Scope<T>* enclosing() {
		return encl;
	}
};


template <typename T>
class SymTab {
private:
	Scope<T>* scope;

public:
	SymTab() :
		scope(new Scope<T>())
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
		scope = new Scope<T>(scope);
	}

	void exit() {
		Scope<T>* enclosing = scope->enclosing();
		assert(enclosing && "stack underflow");
		delete scope;
		scope = enclosing;
	}
};
