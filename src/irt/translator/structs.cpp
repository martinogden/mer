#include "map.hpp"
#include <vector>
#include "irt/containers.hpp"
#include "irt/size-analyser.hpp"


static IRTStruct toIRTStruct(StructType& type) {
	uint size = 0;
	uint largest = 0;
	Map<uint> offsets;
	std::vector<std::string> fields;
	TypeSizeAnalyser tsa;

	for (auto& field : type.fields) {
		uint width = tsa.get(field.type);
		uint padding = TypeSizeAnalyser::pad(width, size);

		largest = std::max<uint>(width, largest);
		offsets.define(field.name, size + padding);
		size += padding + width;
		fields.push_back(field.name);
	}

	if (largest > 0)
		size += TypeSizeAnalyser::pad(largest, size);
	return {type.name, std::move(fields), size, std::move(offsets)};
}


Map<IRTStruct> toIRTStructs(Map<StructTypePtr>& types) {
	Map<IRTStruct> structs;

	for (const auto& pair : types)
		structs.define(pair.first, toIRTStruct(*pair.second));

	return std::move(structs);
}
