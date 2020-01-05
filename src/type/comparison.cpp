#include "type/comparison.hpp"


void TypeEq::visit(BaseType& type) {
	if ( auto t = dynamic_cast<BaseType*>(other.get()) )
		ret(type.name == t->name && type.alias == t->alias );
	else
		ret(false);
}


void TypeEq::visit(StructType& type) {
	if ( auto ty = dynamic_cast<StructType*>(other.get()) ) {

		if(ty->name != type.name)
			return ret(false);

		if (structs.find(type.name) != structs.end())
			return ret(true);
		else
			structs.insert(type.name);

		if (ty->fields.size() != type.fields.size())
			return ret(false);

		for (uint i=0; i<type.fields.size(); ++i) {
			if (ty->fields[i].name != type.fields[i].name)
				return ret(false);
			if (!get(ty->fields[i].type, type.fields[i].type))
				return ret(false);
		}

		structs.erase(type.name);

		ret(true);
	}
	else
		ret(false);
}


void TypeEq::visit(PtrType& ptr) {
	if ( auto ty = dynamic_cast<PtrType*>(other.get()) )
		ret( get(ty->type, ptr.type) );
	else
		ret(false);
}


void TypeEq::visit(ArrayType& arr) {
	if ( auto ty = dynamic_cast<ArrayType*>(other.get()) )
		ret( get(ty->type, arr.type) );
	else
		ret(false);
}


void TypeEq::visit(FunType& type) {
	if ( auto ty = dynamic_cast<FunType*>(other.get()) ){

		if (!get(ty->codomain, type.codomain))
			return ret(false);

		if (ty->domain.size() != type.domain.size())
			return ret(false);

		for (uint i=0; i<type.domain.size(); ++i) {
			if (!get(ty->domain[i], type.domain[i]))
				return ret(false);
		}

		return ret(true);
	}

	ret(false);
}


bool TypeEq::get(const TypePtr& t1, const TypePtr& t2) {
	assert(t1 && t2);
	other = t2;
	t1->accept(*this);
	return retval;
}


void TypeEq::ret(bool b) {
	retval = b;
}


bool eq(const TypePtr& t1, const TypePtr& t2) {
	assert(t1 && t2);
	TypeEq typeEq;
	return typeEq.get(t1, t2);
}


bool is_sub(const TypePtr& t1, const TypePtr& t2) {
	return eq(t1, t2) || (dynamic_cast<PtrType*>(t2.get()) && eq(t1, Type::INDEF));
}


bool cmp(const TypePtr& t1, const TypePtr& t2) {
	return is_sub(t1, t2) || is_sub(t2, t1);
}


bool is_sml(const TypePtr& type) {
	return (
		!dynamic_cast<StructType*>(type.get()) &&
		!dynamic_cast<FunType*>(type.get())
	);
}
