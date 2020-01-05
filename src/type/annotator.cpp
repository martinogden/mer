#include "type/annotator.hpp"
#include "type/canonicaliser.hpp"
#include "type/comparison.hpp"


const std::string VOID_ERROR = "operand(s) may not be void";


TypeAnnotator::TypeAnnotator(SymTab<TypePtr>& env, Map<FunTypePtr>& funTypes, Map<StructTypePtr>& structTypes) :
	env(env),
	funTypes(funTypes),
	structTypes(structTypes)
{}


TypePtr TypeAnnotator::get(ExprPtr& expr) {
	expr->accept(*this);
	return canon(expr->type);
}


void TypeAnnotator::annotate(Expr& expr, TypePtr& type) {
	assert(!eq(type, Type::UNKNOWN));
	expr.type = type;
}


TypePtr TypeAnnotator::canon(TypePtr& type) {
	TypeCanonicaliser typeCanon(env, structTypes);
	return typeCanon.get(type);
}


// TODO: refactor
void TypeAnnotator::visit(CallExpr& expr) {
	TypePtr type = Type::UNKNOWN;
	std::string id = expr.identifier;

	if (!funTypes.exists(id)) {
		errors.add("Use of undeclared function: " + id + ".", expr.token);
		type = Type::ERROR;
	}
	else {
		FunTypePtr& decl = funTypes.lookup(id);
		type = decl->codomain;
		uint m = expr.args.size();
		uint n = decl->domain.size();

		if (env.exists(id)) {
			errors.add("Function name cannot shadow local var name", expr.token);
			type = Type::ERROR;
		}

		if (m != n) {
			errors.add("Incorrect number of function args. Expected: " +
			           std::to_string(n) + ", got: " +
			           std::to_string(m) + ".", expr.token);
			type = Type::ERROR;
		} else {
			for (uint i = 0; i < m; ++i) {
				if ( !eq(get(expr.args[i]), decl->domain[i]) ) {
					errors.add("Invalid argument type.", expr.token);
					type = Type::ERROR;
				}
			}
		}
	}

	annotate(expr, type);
}


void TypeAnnotator::visit(TernaryExpr& expr) {
	TypePtr type = Type::UNKNOWN;
	TypePtr ctype = get(expr.cond);
	TypePtr ttype = get(expr.then);
	TypePtr ftype = get(expr.otherwise);

	// propagate errors
	if (eq(ctype, Type::ERROR) || eq(ttype, Type::ERROR) || eq(ftype, Type::ERROR))
		type = Type::ERROR;

	else if (!eq(ctype, Type::BOOL)) {
		errors.add("Condition must be boolean", expr.token);
		type = Type::ERROR;
	}
	else if (!eq(ttype, ftype)) {
		errors.add("Types of both branches must match", expr.token);
		type = Type::ERROR;
	}
	else if (eq(ttype, Type::VOID)) {
		errors.add(VOID_ERROR, expr.then->token);
		type = Type::ERROR;
	}
	else
		type = ttype;

	annotate(expr, type);
}


// TODO: refactor
void TypeAnnotator::visit(BinaryExpr& expr) {
	TypePtr type = Type::UNKNOWN;
	TypePtr ltype = get(expr.left);
	TypePtr rtype = get(expr.right);

	// propagate errors
	if (eq(ltype, Type::ERROR) || eq(rtype, Type::ERROR))
		type = Type::ERROR;

	else if (!cmp(ltype, rtype)) {
		errors.add("Operands not of comparable types", expr.token);
		type = Type::ERROR;
	}

	else if (eq(ltype, Type::VOID) || eq(rtype, Type::VOID)) {
		errors.add(VOID_ERROR, expr.token);
		type = Type::ERROR;
	}

	else if (!is_sml(ltype) || !is_sml(rtype)) {
		errors.add("operands must have small type", expr.token);
		type = Type::ERROR;
	}

	else {
		switch (getOpType(expr.op)) {
			case OpType::LOGICAL:
				if (is_sub(ltype, Type::BOOL))
					type = Type::BOOL;
				else {
					errors.add("Operands must be boolean", expr.token);
					type = Type::ERROR;
				}
				break;

			case OpType::REL:
				if (is_sub(ltype, Type::INT))
					type = Type::BOOL;
				else {
					errors.add("Operands must be int", expr.token);
					type = Type::ERROR;
				}
				break;

			case OpType::ARITH:
			case OpType::BITWISE:
				if (is_sub(ltype, Type::INT))
					type = Type::INT;
				else {
					errors.add("Operands must be int", expr.token);
					type = Type::ERROR;
				}
				break;

			case OpType::EQL:
				type = Type::BOOL;
				break;
		}
	}

	annotate(expr, type);
}


void TypeAnnotator::visit(UnaryExpr& unary) {
	TypePtr type = Type::UNKNOWN;
	TypePtr operandType = get(unary.expr);

	if (eq(operandType, Type::ERROR))
		type = Type::ERROR;  // propagate errors
	else if (eq(operandType, Type::VOID)) {
		errors.add(VOID_ERROR, unary.token);
		type = Type::ERROR;
	}
	else {
		switch (unary.op) {
			case UnOp::NEG:
			case UnOp::BIT_NOT:
				if (eq(operandType, Type::INT))
					type = Type::INT;
				else {
					errors.add("operand must be an int", unary.token);
					type = Type::ERROR;
				}
				break;
			case UnOp::LOG_NOT:
				if (eq(operandType, Type::BOOL))
					type = Type::BOOL;
				else {
					errors.add("operand must be a bool", unary.token);
					type = Type::ERROR;
				}
				break;
		}
	}

	annotate(unary, type);
}


void TypeAnnotator::visit(LiteralExpr& expr) {
	assert(!eq(expr.type, Type::UNKNOWN));
}


void TypeAnnotator::visit(IdExpr& expr) {
	std::string id = expr.identifier;
	TypePtr type = env.lookup(id);
	annotate(expr, type);
}


void TypeAnnotator::visit(SubscriptExpr& expr) {
	TypePtr type = Type::UNKNOWN;
	TypePtr ltype = get(expr.left);

	if (eq(ltype, Type::ERROR))
		type = Type::ERROR;  // propagate errors
	else if (auto atype = dynamic_cast<ArrayType*>(ltype.get())) {
		TypePtr rtype = get(expr.right);
		if (eq(rtype, Type::ERROR))
			type = Type::ERROR;  // propagate error
		else if (eq(rtype, Type::INT))
			// TODO: bounds check?
			type = atype->type;
		else {
			errors.add("index must be an int", expr.right->token);
			type = Type::ERROR;
		}
	}
	else {
		errors.add("left hand side must be an array type", expr.token);
		type = Type::ERROR;
	}

	annotate(expr, type);
}


void TypeAnnotator::visit(ArrowExpr& expr) {
	throw 1;  // we should never get here
}


// TODO: refactor
void TypeAnnotator::visit(DotExpr& expr) {
	std::string id = expr.identifier;
	TypePtr type = Type::UNKNOWN;
	TypePtr ltype = get(expr.expr);

	if (eq(ltype, Type::ERROR))
		type = Type::ERROR;  // propagate errors
	else if (auto stype = dynamic_cast<StructType*>(ltype.get())) {
		if (structTypes.exists(stype->name)) {
			// check field name exists in struct
			StructTypePtr s = structTypes.lookup(stype->name);
			bool exists = false;

			for (const auto& field : s->fields) {
				if (field.name == id) {
					type = field.type;
					exists = true;
				}
			}

			if (!exists) {
				errors.add("invalid field name: " + id, expr.token);
				type = Type::ERROR;
			}
		}
		else {
			errors.add("struct " + stype->name + " not a known type", expr.token);
			type = Type::ERROR;
		}
	}
	else {
		errors.add("left hand side must be a struct type", expr.token);
		type = Type::ERROR;
	}

	annotate(expr, type);
}


void TypeAnnotator::visit(DerefExpr& ptr) {
	TypePtr type = Type::UNKNOWN;
	TypePtr ptype = get(ptr.expr);

	if (eq(ptype, Type::ERROR))
		type = Type::ERROR;  // propagate errors
	else if ( auto p = dynamic_cast<PtrType*>(ptype.get()) )
		type = p->type;
	else {
		errors.add("can not dereference non-pointer type", ptr.token);
		type = Type::ERROR;
	}

	annotate(ptr, type);
}


void TypeAnnotator::visit(AllocExpr& alloc) {
	TypePtr type = Type::UNKNOWN;

	if (alloc.expr) {
		TypePtr etype = get(alloc.expr);

		if (!eq(etype, Type::INT)) {
			errors.add("operand must be an int", alloc.expr->token);
			type = Type::ERROR;
		}

		type = std::make_shared<ArrayType>(alloc.typeParam);
	}
	else
		type = std::make_shared<PtrType>(alloc.typeParam);

	annotate(alloc, type);
}
