#include "type/visitor.hpp"
#include "type/type.hpp"


void TypeVisitor::visit(BaseType& type) {}
void TypeVisitor::visit(StructType& type) {}


void TypeVisitor::visit(PtrType& ptr) {
	ptr.type->accept(*this);
}


void TypeVisitor::visit(ArrayType& arr) {
	arr.type->accept(*this);
}


void TypeVisitor::visit(FunType& fun) {
	for (const auto& type : fun.domain)
		type->accept(*this);
	fun.codomain->accept(*this);
}
