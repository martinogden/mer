#pragma once
#include <string>
#include <vector>
#include "map.hpp"
#include "irt/irt.hpp"


class IRTStruct {
public:
	std::string id;
	std::vector<std::string> fields;
	uint size;
	Map<uint> offsets;

	IRTStruct() {}  // TODO: get rid of this (used to init vector)
	IRTStruct(std::string id, std::vector<std::string> fields, uint size, Map<uint> offsets);
};


class IRTFun {
public:
	std::string id;
	std::vector<std::string> params;
	IRTCmdPtr body;

	IRTFun(std::string id, std::vector<std::string> params, IRTCmdPtr body);
};


typedef std::unique_ptr<IRTStruct> IRTStructPtr;
typedef std::unique_ptr<IRTFun> IRTFunPtr;
