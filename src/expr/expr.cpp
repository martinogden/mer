#include "expr/expr.hpp"


Expr::Expr(Token token, TypePtr type) :
	token(std::move(token)),
	type(std::move(type))
{}


CallExpr::CallExpr(Token token, std::string identifier, std::vector<ExprPtr> args) :
	Expr(std::move(token)),
	identifier(std::move(identifier)),
	args(std::move(args))
{}


void CallExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


TernaryExpr::TernaryExpr(Token token, ExprPtr cond, ExprPtr then, ExprPtr otherwise) :
	Expr(std::move(token)),
	cond(std::move(cond)),
	then(std::move(then)),
	otherwise(std::move(otherwise))
{}


void TernaryExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


BinaryExpr::BinaryExpr(Token token, BinOp op, ExprPtr left, ExprPtr right) :
	Expr(std::move(token)),
	op(op),
	left(std::move(left)),
	right(std::move(right))
{}


void BinaryExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


UnaryExpr::UnaryExpr(Token token, UnOp op, ExprPtr expr) :
	Expr(std::move(token)),
	op(op),
	expr(std::move(expr))
{}


void UnaryExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


LiteralExpr::LiteralExpr(Token token, TypePtr type, union Value as) :
	Expr(std::move(token), std::move(type)),
	as(as)
{}


LiteralExpr::LiteralExpr(Token token, int64_t value) :
	LiteralExpr(std::move(token), Type::INT, {.i = value})
{}


LiteralExpr::LiteralExpr(Token token, bool value) :
	LiteralExpr(std::move(token), Type::BOOL, {.b = value})
{}


void LiteralExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


IdExpr::IdExpr(Token token, std::string lexeme) :
	Expr(std::move(token)),
	identifier(std::move(lexeme))
{}


void IdExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


SubscriptExpr::SubscriptExpr(Token token, ExprPtr left, ExprPtr right) :
	Expr(std::move(token)),
	left(std::move(left)),
	right(std::move(right))
{}


void SubscriptExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


ArrowExpr::ArrowExpr(Token token, ExprPtr expr, std::string identifier) :
	Expr(std::move(token)),
	expr(std::move(expr)),
	identifier(std::move(identifier))
{}


void ArrowExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


DotExpr::DotExpr(Token token, ExprPtr expr, std::string identifier) :
	Expr(std::move(token)),
	expr(std::move(expr)),
	identifier(std::move(identifier))
{}


void DotExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


DerefExpr::DerefExpr(Token token, ExprPtr expr) :
	Expr(std::move(token)),
	expr(std::move(expr))
{}


void DerefExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};


AllocExpr::AllocExpr(Token token, TypePtr typeParam, ExprPtr expr) :
	Expr(std::move(token)),
	typeParam(std::move(typeParam)),
	expr(std::move(expr))
{}


void AllocExpr::accept(ExprVisitor& visitor) {
	visitor.visit(*this);
};
