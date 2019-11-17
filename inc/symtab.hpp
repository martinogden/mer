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

	bool exists(std::string key) {
		return find(key) != nullptr;
	}

	void assign(std::string key, T value) {
		T* v = find(key);
		assert (v && "key does not exist");
		*v = value;
	}

	void assignAll(T value) {
		for (auto& item : values)
			values[item.first] = value;

		if (encl)
			encl->assignAll(value);
	}

	void define(std::string key, T value) {
		T* v = find(key);
		assert (!v && "cannot redefine key");
		values[key] = value;
	}

	T& lookup(std::string key) {
		T* value = find(key);
		assert (value && "key does not exist");
		return *value;
	}

	std::unordered_map<std::string, T>& locals() {
		return values;
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
	bool exists(std::string key) {
		return scope->exists(key);
	}

	void assign(std::string key, T value) {
		scope->assign(key, value);
	}

	void assignAll(T value) {
		scope->assignAll(value);
	}

	void define(std::string key, T value) {
		scope->define(key, value);
	}

	T& lookup(std::string key) {
		return scope->lookup(key);
	}

	std::unordered_map<std::string, T>& locals() {
		return scope->locals();
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
