#pragma once
#include <iostream>
#include <unordered_set>


template <typename T>
std::unordered_set<T>
set_intersect(std::unordered_set<T> s, std::unordered_set<T> t) {
	std::unordered_set<T> dst;

	for (const T& elt : s) {
		if (t.find(elt) != t.end())
			dst.insert(elt);
	}

	return dst;
}


template <typename T>
std::unordered_set<T>
set_union(std::unordered_set<T> s, std::unordered_set<T> t) {
	std::unordered_set<T> dst;

	dst.insert(s.begin(), s.end());
	dst.insert(t.begin(), t.end());

	return dst;
}


template <typename T>
std::unordered_set<T>
set_difference(std::unordered_set<T> s, std::unordered_set<T> t) {
	std::unordered_set<T> dst;

	for (const T& elt : s) {
		if (t.find(elt) == t.end())
			dst.insert(elt);
	}

	return dst;
}


// an immutable set
template <typename T>
class Set {
private:
	std::unordered_set<T> elts;

public:
	Set() {}

	Set(std::unordered_set<T>&& elts) :
		elts(elts)
	{}

	Set<T> operator&(Set<T> other) {
		return set_intersect(elts, other.elts);
	}

	Set<T> operator|(Set<T> other) {
		return set_union(elts, other.elts);
	}

	Set<T> operator-(Set<T> other) {
		return set_difference(elts, other.elts);
	}

	bool operator==(Set<T> other) {
		return elts == other.elts;
	}

	bool contains(T& elt) {
		return elts.find(elt) != elts.end();
	}

	bool empty() {
		return elts.size() == 0;
	}

	friend std::ostream& operator<<(std::ostream& output, const Set<T>& set) {
		output << "{ ";
		for (const T& elt : set.elts)
			output << elt << " ";
		output << "}";
		return output;
	}

	// expose set iterator
	using iterator= typename std::unordered_set<T>::iterator;

	iterator begin() {
		return elts.begin();
	};
	iterator end() {
		return elts.end();
	};
};
