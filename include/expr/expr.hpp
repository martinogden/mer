#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "parser/token.hpp"
#include "expr/visitor.hpp"
#include "cst/operator.hpp"
#include "type/type.hpp"


class Expr;
typedef std::unique_ptr<Expr> ExprPtr;


class Expr {
public:
	Token token;
	TypePtr type;

	Expr(Token token, TypePtr type=Type::UNKNOWN);
	virtual ~Expr() = default;
	virtual void accept(ExprVisitor& visitor) = 0;
};


class CallExpr : public Expr {
public:
	std::string identifier;
	std::vector<ExprPtr> args;

	CallExpr(Token token, std::string identifier, std::vector<ExprPtr> args);
	void accept(ExprVisitor& visitor) override;
};


class TernaryExpr : public Expr {
public:
	ExprPtr cond;
	ExprPtr then;
	ExprPtr otherwise;

	TernaryExpr(Token token, ExprPtr cond, ExprPtr then, ExprPtr otherwise);
	void accept(ExprVisitor& visitor) override;
};


class BinaryExpr : public Expr {
public:
	BinOp op;
	ExprPtr left;
	ExprPtr right;

	BinaryExpr(Token token, BinOp op, ExprPtr left, ExprPtr right);
	void accept(ExprVisitor& visitor) override;
};


class UnaryExpr : public Expr {
public:
	UnOp op;
	ExprPtr expr;

	UnaryExpr(Token token, UnOp op, ExprPtr expr);
	void accept(ExprVisitor& visitor) override;
};


class LiteralExpr : public Expr {
public:
	union Value {
		int64_t i;
		bool b;
		Value() = default;
	} as;

	LiteralExpr(Token token, TypePtr type, union Value value);
	LiteralExpr(Token token, int64_t value);
	LiteralExpr(Token token, bool value);
	void accept(ExprVisitor& visitor) override;
};


class AllocExpr : public Expr {
public:
	TypePtr typeParam;
	ExprPtr expr;
	AllocExpr(Token token, TypePtr typeParam, ExprPtr expr=nullptr);
	void accept(ExprVisitor& visitor) override;
};


/* LValues */


class IdExpr : public Expr {
public:
	std::string identifier;

	IdExpr(Token token, std::string lexeme);
	void accept(ExprVisitor& visitor) override;
};


class SubscriptExpr : public Expr {
public:
	ExprPtr left;
	ExprPtr right;

	SubscriptExpr(Token token, ExprPtr left, ExprPtr right);
	void accept(ExprVisitor& visitor) override;
};


class ArrowExpr : public Expr {
public:
	ExprPtr expr;
	std::string identifier;

	ArrowExpr(Token token, ExprPtr expr, std::string identifier);
	void accept(ExprVisitor& visitor) override;
};


class DotExpr : public Expr {
public:
	ExprPtr expr;
	std::string identifier;

	DotExpr(Token token, ExprPtr expr, std::string identifier);
	void accept(ExprVisitor& visitor) override;
};


class DerefExpr : public Expr {
public:
	ExprPtr expr;

	DerefExpr(Token token, ExprPtr expr);
	void accept(ExprVisitor& visitor) override;
};
