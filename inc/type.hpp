#pragma once
#include <iostream>
#include <unordered_map>
#include <string>


enum class Type {
	UNKNOWN,
	INT,
	BOOL,
	VOID,
	ERROR,
};


typedef std::pair<std::vector<Type>, Type> FunType;


extern std::unordered_map<std::string, Type> concreteTypes;

std::ostream& operator<<(std::ostream& output, const Type& type);
