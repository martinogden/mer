#pragma once
#include "type/visitor.hpp"
#include "type/type.hpp"


class TypePrinter : public TypeVisitor {
private:
	std::string retval;
	void ret(const std::string& s);

public:
	void visit(BaseType& type) override;
	void visit(StructType& type) override;
	void visit(PtrType& type) override;
	void visit(ArrayType& type) override;
	void visit(FunType& type) override;

	std::string get(const TypePtr& type);
};
