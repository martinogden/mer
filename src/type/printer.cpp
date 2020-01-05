#include <sstream>
#include "type/printer.hpp"
#include "print-utils.hpp"


std::string TypePrinter::get(const TypePtr& type) {
	type->accept(*this);
	return retval;
}


void TypePrinter::ret(const std::string& s) {
	retval = s;
}


void TypePrinter::visit(BaseType& type) {
	ret( type.alias );
}


void TypePrinter::visit(StructType& type) {
	ret( "struct " + type.name );
}


void TypePrinter::visit(PtrType& ptr) {
	ret( get(ptr.type) + "*" );
}


void TypePrinter::visit(ArrayType& arr) {
	ret( get(arr.type) + "[]" );
}


void TypePrinter::visit(FunType& fun) {
	std::stringstream buf;

	buf << "(";
	std::vector<std::string> types;

	for (const auto& type : fun.domain)
		types.push_back( get(type) );

	buf << join(types, ", ")  << ") -> " << get(fun.codomain);

	ret( buf.str() );
}
