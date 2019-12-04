#include "type/annotator.hpp"


const std::string VOID_ERROR = "operand(s) may not be void";


TypeAnnotator::TypeAnnotator(SymTab<Type>& env, SymTab<FunType>& decls) :
	env(env),
	decls(decls)
{}


Type TypeAnnotator::get(ExprPtr& expr) {
	expr->accept(*this);
	return expr->type;
}


void TypeAnnotator::annotate(Expr& expr, Type type) {
	assert(type != Type::UNKNOWN);
	expr.type = type;
}


void TypeAnnotator::visit(CallExpr& expr) {
	std::string id = expr.identifier;

	assert(decls.exists(id));

	FunType& decl = decls.lookup(id);
	Type type = decl.second;
	uint m = expr.args.size();
	uint n = decl.first.size();

	if (env.exists(id)) {
		errors.add("Function name cannot shadow local var name", expr.token);
		type = Type::ERROR;
	}
	if (m != n) {
		errors.add(
			"Incorrect number of function args. Expected: "
			+ std::to_string(n) + ", got: " + std::to_string(m)
			+ ".", expr.token
		);
		type = Type::ERROR;
	}
	else {
		for (uint i=0; i<m; ++i) {
			if (get(expr.args[i]) != decl.first[i]) {
				errors.add("Invalid argument type.", expr.token);
				type = Type::ERROR;
			}
		}
	}

	annotate(expr, type);
}


void TypeAnnotator::visit(TernaryExpr& expr) {
	Type type = Type::UNKNOWN;
	Type ctype = get(expr.cond);
	Type ttype = get(expr.then);
	Type ftype = get(expr.otherwise);

	// propagate errors
	if (ctype == Type::ERROR || ttype == Type::ERROR || ftype == Type::ERROR) {
		type = Type::ERROR;
	}
	else if (ctype != Type::BOOL) {
		errors.add("Condition must be boolean", expr.token);
		type = Type::ERROR;
	}
	else if (ttype != ftype) {
		errors.add("Types of both branches must match", expr.token);
		type = Type::ERROR;
	}
	else if (ttype == Type::VOID) {
		errors.add(VOID_ERROR, expr.then->token);
		type = Type::ERROR;
	}
	else
		type = ttype;

	annotate(expr, type);
}


void TypeAnnotator::visit(BinaryExpr& expr) {
	Type type = Type::UNKNOWN;
	Type ltype = get(expr.left);
	Type rtype = get(expr.right);

	// propagate errors
	if (ltype == Type::ERROR || rtype == Type::ERROR) {
		type = Type::ERROR;
	}

	else if (ltype != rtype) {
		errors.add("Types of both operands must match", expr.token);
		type = Type::ERROR;
	}

	else if (ltype == Type::VOID) {
		errors.add(VOID_ERROR, expr.token);
		type = Type::ERROR;
	}

	else {
		switch (getOpType(expr.op)) {
			case OpType::LOGICAL:
				if (ltype == Type::BOOL)
					type = Type::BOOL;
				else {
					errors.add("Operands must be boolean", expr.token);
					type = Type::ERROR;
				}
				break;

			case OpType::REL:
				if (ltype == Type::INT)
					type = Type::BOOL;
				else {
					errors.add("Operands must be int", expr.token);
					type = Type::ERROR;
				}
				break;

			case OpType::ARITH:
			case OpType::BITWISE:
				if (ltype == Type::INT)
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
	Type type = Type::UNKNOWN;
	Type operandType = get(unary.expr);

	if (operandType == Type::VOID) {
		errors.add(VOID_ERROR, unary.token);
		type = Type::ERROR;
	}
	else {
		switch (unary.op) {
			case UnOp::NEG:
			case UnOp::BIT_NOT:
				if (operandType == Type::INT)
					type = Type::INT;
				else {
					errors.add("operand must be an int", unary.token);
					type = Type::ERROR;
				}
				break;
			case UnOp::LOG_NOT:
				if (operandType == Type::BOOL)
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
	assert(expr.type != Type::UNKNOWN);
}


void TypeAnnotator::visit(IdExpr& expr) {
	Type type = env.lookup(expr.identifier);
	annotate(expr, type);
}
