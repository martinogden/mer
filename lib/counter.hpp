#pragma once
#include <unordered_map>
#include <unordered_set>


// loosely based on Python's collections.Counter
template <typename T>
class Counter {
private:
	std::unordered_map<T, unsigned int> counts;

public:
	Counter(const std::unordered_set<T>& keys) {
		for (auto const& key : keys)
			counts[key] = 0;
	}

	void incr(const T& key) {
		assert(counts.find(key) != counts.end());
		counts[key]++;
	}

	void erase(const T& key) {
		counts.erase(key);
	}

	T getMostCommon() {
		assert (!counts.empty());

		auto const& item = *counts.begin();
		unsigned int maxCount = item.second;
		T mostCommon = item.first;

		for (auto const& item : counts) {
			if (item.second > maxCount) {
				maxCount = item.second;
				mostCommon = item.first;
			}
		}
		return mostCommon;
	}
};
