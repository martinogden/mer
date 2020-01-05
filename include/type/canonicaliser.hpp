#pragma once
#include <unordered_map>
#include <string>
#include "symtab.hpp"
#include "visitor.hpp"
#include "type.hpp"
#include "map.hpp"
#include "errors.hpp"


class TypeCanonicaliser : public TypeVisitor {
private:
	SymTab<TypePtr>& env;
	Map<StructTypePtr>& structTypes;

	TypePtr retval;
	std::unordered_map<Type::Name, std::string> alias;

	void visit(BaseType& type) override;
	void visit(StructType& type) override;
	void visit(PtrType& type) override;
	void visit(ArrayType& type) override;
	void visit(FunType& type) override;

	void ret(TypePtr type);

public:
	Errors errors;

	TypeCanonicaliser(SymTab<TypePtr>& env, Map<StructTypePtr>& structTypes);
	TypePtr get(TypePtr& type);
};
