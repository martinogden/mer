#pragma once

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


// see https://stackoverflow.com/a/612176/107987
inline std::vector<std::string> ls(std::string name) {
	std::vector<std::string> filenames;
	DIR* dir = opendir(name.c_str());
	struct dirent* ent;

	if (dir == NULL) {
		perror("cannot open directory");
		return {};
	}

	while (true) {
		ent = readdir(dir);
		if (ent == NULL)
			break;

		std::string fn(ent->d_name);
		if (fn != "." && fn != "..")
			filenames.push_back(fn);
	}

	closedir(dir);
	return filenames;
}


inline std::string read(std::string fn) {
	std::stringstream buffer;
	std::ifstream stream(fn);
	if (!stream.is_open())
		throw std::invalid_argument("file not found: " + fn);

	buffer << stream.rdbuf();
	return buffer.str();
}
