#pragma once


class Type;
class BaseType;
class StructType;
class PtrType;
class ArrayType;
class FunType;


class TypeVisitor {
public:
	virtual void visit(BaseType& type);
	virtual void visit(StructType& type);
	virtual void visit(PtrType& type);
	virtual void visit(ArrayType& type);
	virtual void visit(FunType& type);

	virtual ~TypeVisitor() = default;
};
