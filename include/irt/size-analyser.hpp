#pragma once
#include "type/visitor.hpp"
#include "type/type.hpp"


// calculate size of an object of each type (in bytes)
class TypeSizeAnalyser : public TypeVisitor {
private:
	uint retval;

	void visit(BaseType& type) override;
	void visit(StructType& type) override;
	void visit(PtrType& type) override;
	void visit(ArrayType& type) override;
	void visit(FunType& type) override;
	void ret(uint n);

public:
	static uint pad(uint width, uint size);
	uint get(const TypePtr& type);
};
