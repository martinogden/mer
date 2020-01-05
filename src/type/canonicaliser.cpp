#include "type/canonicaliser.hpp"
#include "type/comparison.hpp"


TypeCanonicaliser::TypeCanonicaliser(SymTab<TypePtr>& env, Map<StructTypePtr>& structTypes) :
	env(env),
	structTypes(structTypes)
{}


TypePtr TypeCanonicaliser::get(TypePtr& type) {
	if (errors.exist())
		return Type::ERROR;
	else {
		type->accept(*this);
		return std::move(retval);
	}
}


void TypeCanonicaliser::ret(TypePtr type) {
	retval = std::move(type);
}


void TypeCanonicaliser::visit(BaseType& base) {
	TypePtr type;

	if (base.name != Type::Name::UNKNOWN && env.exists(base.alias)) {
		// check alias matches type if type is set
		TypePtr t1 = env.lookup(base.alias);
		auto t = dynamic_cast<BaseType*>(t1.get());
		assert(t && t->alias == base.alias && t->name == base.name);
	}

	if (base.name != Type::Name::UNKNOWN)
		type = Type::get(base.name);
	else if (env.exists(base.alias))
		type = env.lookup(base.alias);
	else {
		errors.add("unknown type alias: " + base.alias);
		type = Type::ERROR;
	}

	ret( std::move(type) );
}


void TypeCanonicaliser::visit(StructType& type) {
	StructTypePtr ty;

	if (structTypes.exists(type.name))
		ty = structTypes.lookup(type.name);
	else
		ty = std::make_shared<StructType>(type.name);

	// assert(ty->isFinal);
	ret(ty);
}


void TypeCanonicaliser::visit(PtrType& ptr) {
	ret( std::make_shared<PtrType>(get(ptr.type)) );
}


void TypeCanonicaliser::visit(ArrayType& arr) {
	ret( std::make_shared<ArrayType>(get(arr.type)) );
}


void TypeCanonicaliser::visit(FunType& fun) {
	std::vector<TypePtr> domain;
	for (auto& type : fun.domain)
		domain.push_back( std::move(get(type)) );

	ret( std::make_shared<FunType>(std::move(domain), get(fun.codomain)) );
}
