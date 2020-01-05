#pragma once
#include <unordered_set>
#include "type.hpp"


// strict (structural) type equality
bool eq(const TypePtr& t1, const TypePtr& t2);


// returns true iff t1 is a subtype of t2 (t1 <: t2)
// for now, this means, either t1 == t2, or
// t1 has indefinite type and t2 a pointer type
bool is_sub(const TypePtr& t1, const TypePtr& t2);


// type subsumption: returns true iff t1 and t2 are comparable
// i.e. if either t1 <: t2, t1 == t2, or t1 :> t2
bool cmp(const TypePtr& t1, const TypePtr& t2);


// small types T -> int | bool | T*, T[] only
bool is_sml(const TypePtr& type);


class TypeEq : public TypeVisitor {
private:
	bool retval;
	TypePtr other;
	std::unordered_set<std::string> structs;

	void visit(BaseType& type) override;
	void visit(StructType& type) override;
	void visit(PtrType& type) override;
	void visit(ArrayType& type) override;
	void visit(FunType& type) override;

	void ret(bool b);
public:

	bool get(const TypePtr& t1, const TypePtr& t2);
};
