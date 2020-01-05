#include "irt/containers.hpp"


IRTStruct::IRTStruct(std::string id, std::vector<std::string> fields, uint size, Map<uint> offsets) :
	id(std::move(id)),
	fields(std::move(fields)),
	size(size),
	offsets(std::move(offsets))
{}


IRTFun::IRTFun(std::string id, std::vector<std::string> params, IRTCmdPtr body) :
	id(std::move(id)),
	params(std::move(params)),
	body(std::move(body))
{}
