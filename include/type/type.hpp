#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "type/visitor.hpp"


class Type;
class StructType;
class FunType;
typedef std::shared_ptr<Type> TypePtr;
typedef std::shared_ptr<StructType> StructTypePtr;
typedef std::shared_ptr<FunType> FunTypePtr;


class Type {
public:
	enum class Name {
		INDEF,
		UNKNOWN,
		ERROR,
		VOID,
		INT,
		BOOL,
	};

	static const TypePtr INDEF;
	static const TypePtr UNKNOWN;
	static const TypePtr ERROR;
	static const TypePtr VOID;
	static const TypePtr INT;
	static const TypePtr BOOL;
	static TypePtr get(Type::Name name);
	static std::unordered_map<std::string, TypePtr> baseTypes;

	virtual ~Type() = default;
	virtual void accept(TypeVisitor& visitor) = 0;
};


class BaseType : public Type {
public:
	Name name;
	std::string alias;

	BaseType(Type::Name type, std::string name="");
	BaseType(std::string name);
	void accept(TypeVisitor& visitor) override;
};


class StructType : public Type {
public:
	struct Field {
		std::string name;
		TypePtr type;
		Field(std::string name, TypePtr type);
	};

	std::string name;
	std::vector<Field> fields;
	bool isFinal;

	StructType(std::string name);
	StructType(std::string name, std::vector<Field> fields);
	void accept(TypeVisitor& visitor) override;
};


class PtrType : public Type {
public:
	TypePtr type;

	PtrType(TypePtr type);
	void accept(TypeVisitor& visitor) override;
};


class ArrayType : public Type {
public:
	TypePtr type;

	ArrayType(TypePtr type);
	void accept(TypeVisitor& visitor) override;
};


class FunType : public Type {
public:
	std::vector<TypePtr> domain;
	TypePtr codomain;

	FunType(std::vector<TypePtr> domain, TypePtr codomain);
	void accept(TypeVisitor& visitor) override;
};


std::ostream& operator<<(std::ostream& output, const Type::Name& type);
