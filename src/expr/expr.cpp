#include "expr/expr.hpp"


Expr::Expr(Token token, Type type) :
		token(std::move(token)),
		type(type)
{}


CallExpr::CallExpr(Token token, std::string identifier, std::vector<ExprPtr> args) :
		Expr(std::move(token)),
		identifier(std::move(identifier)),
		args(std::move(args))
{}


void CallExpr::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


TernaryExpr::TernaryExpr(Token token, ExprPtr cond, ExprPtr then, ExprPtr otherwise) :
		Expr(std::move(token)),
		cond(std::move(cond)),
		then(std::move(then)),
		otherwise(std::move(otherwise))
{}


void TernaryExpr::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


BinaryExpr::BinaryExpr(Token token, BinOp op, ExprPtr left, ExprPtr right) :
		Expr(std::move(token)),
		op(op),
		left(std::move(left)),
		right(std::move(right))
{}


void BinaryExpr::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


UnaryExpr::UnaryExpr(Token token, UnOp op, ExprPtr expr) :
		Expr(std::move(token)),
		op(op),
		expr(std::move(expr))
{}


void UnaryExpr::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


LiteralExpr::LiteralExpr(Token token, int value) :
		Expr(std::move(token), Type::INT),
		as({.i = value})
{}


LiteralExpr::LiteralExpr(Token token, bool value) :
		Expr(std::move(token), Type::BOOL),
		as({.b = value})
{}


void LiteralExpr::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


IdExpr::IdExpr(Token token, std::string lexeme) :
		Expr(std::move(token)),
		identifier(std::move(lexeme))
{}


void IdExpr::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};
