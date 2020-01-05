#include "type/type.hpp"


TypePtr Type::get(Type::Name name) {
	switch (name) {
		case Type::Name::INDEF:
			return Type::INDEF;
		case Type::Name::UNKNOWN:
			return Type::UNKNOWN;
		case Type::Name::ERROR:
			return Type::ERROR;
		case Type::Name::VOID:
			return Type::VOID;
		case Type::Name::INT:
			return Type::INT;
		case Type::Name::BOOL:
			return Type::BOOL;
	}
}


BaseType::BaseType(Type::Name type, std::string alias) :
	name(type),
	alias(std::move(alias))
{}


BaseType::BaseType(std::string alias) :
	BaseType(Type::Name::UNKNOWN, std::move(alias))
{}


void BaseType::accept(TypeVisitor& visitor) {
	visitor.visit(*this);
};


StructType::StructType(std::string name) :
	name(std::move(name)),
	isFinal(false)
{}


StructType::StructType(std::string name, std::vector<Field> fields) :
	name(std::move(name)),
	fields(std::move(fields)),
	isFinal(true)
{}


StructType::Field::Field(std::string name, TypePtr type) :
	name(std::move(name)),
	type(std::move(type))
{}


void StructType::accept(TypeVisitor& visitor) {
	visitor.visit(*this);
};


PtrType::PtrType(TypePtr type) :
	type(std::move(type))
{}


void PtrType::accept(TypeVisitor& visitor) {
	visitor.visit(*this);
};


ArrayType::ArrayType(TypePtr type) :
		type(std::move(type))
{}


void ArrayType::accept(TypeVisitor& visitor) {
	visitor.visit(*this);
};


FunType::FunType(std::vector<TypePtr> domain, TypePtr codomain) :
	domain(std::move(domain)),
	codomain(std::move(codomain))
{}


void FunType::accept(TypeVisitor& visitor) {
	visitor.visit(*this);
};


const TypePtr Type::INDEF   = std::make_shared<BaseType>(Type::Name::INDEF, "any*");
const TypePtr Type::UNKNOWN = std::make_shared<BaseType>(Type::Name::UNKNOWN, "unknown");
const TypePtr Type::ERROR   = std::make_shared<BaseType>(Type::Name::ERROR, "error");
const TypePtr Type::VOID    = std::make_shared<BaseType>(Type::Name::VOID, "void");
const TypePtr Type::INT     = std::make_shared<BaseType>(Type::Name::INT, "int");
const TypePtr Type::BOOL    = std::make_shared<BaseType>(Type::Name::BOOL, "bool");


std::unordered_map<std::string, TypePtr> Type::baseTypes = {
	{"void", Type::VOID},
	{"int", Type::INT},
	{"bool", Type::BOOL},
};
