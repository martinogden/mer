#include "irt/size-analyser.hpp"


void TypeSizeAnalyser::ret(uint n) {
	retval = n;
}


uint TypeSizeAnalyser::get(const TypePtr& type) {
	type->accept(*this);
	return retval;
}


uint TypeSizeAnalyser::pad(uint width, uint size) {
	if (width == 0) return 0;
	return (width - size % width) % width;
}


void TypeSizeAnalyser::visit(BaseType& type) {
	if (type.name == Type::Name::INT || type.name == Type::Name::BOOL)
		ret(8);
	else
		throw 1;  // we should never get here
}


void TypeSizeAnalyser::visit(StructType& type) {
	uint size = 0;
	uint largest = 0;

	for (auto& field : type.fields) {
		uint width = get(field.type);
		uint padding = pad(width, size);

		largest = std::max<uint>(width, largest);
		size += padding + width;
	}

	size += pad(largest, size);
	ret(size);
}


void TypeSizeAnalyser::visit(PtrType& type) {
	ret(8);
}


void TypeSizeAnalyser::visit(ArrayType& type) {
	ret(8);
}


void TypeSizeAnalyser::visit(FunType& type) {
	throw 1;  // we should never get here
}
