#pragma once
#include <sstream>
#include <vector>


template <typename T>
std::string join(std::vector<T>& items, std::string sep=" ") {
	if (items.size() == 0)
		return "";

	std::stringstream buf;
	for (auto it=items.begin(); it != --items.end(); ++it)
		buf << *it << sep;
	buf << *(--items.end());

	return buf.str();
}
