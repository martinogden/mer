#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "cst/visitor.hpp"
#include "parser/token.hpp"
#include "cst/operator.hpp"
#include "type/type.hpp"


class Expr;
typedef std::unique_ptr<Expr> ExprPtr;


class Expr {
public:
	Token token;
	Type type;

	Expr(Token token, Type type=Type::UNKNOWN);
	virtual ~Expr() = default;
	virtual void accept(CSTVisitor& visitor) = 0;
};


class CallExpr : public Expr {
public:
	std::string identifier;
	std::vector<ExprPtr> args;

	CallExpr(Token token, std::string identifier, std::vector<ExprPtr> args);
	void accept(CSTVisitor& visitor) override;
};


class TernaryExpr : public Expr {
public:
	ExprPtr cond;
	ExprPtr then;
	ExprPtr otherwise;

	TernaryExpr(Token token, ExprPtr cond, ExprPtr then, ExprPtr otherwise);
	void accept(CSTVisitor& visitor) override;
};


class BinaryExpr : public Expr {
public:
	BinOp op;
	ExprPtr left;
	ExprPtr right;

	BinaryExpr(Token token, BinOp op, ExprPtr left, ExprPtr right);
	void accept(CSTVisitor& visitor) override;
};


class UnaryExpr : public Expr {
public:
	UnOp op;
	ExprPtr expr;

	UnaryExpr(Token token, UnOp op, ExprPtr expr);
	void accept(CSTVisitor& visitor) override;
};


class LiteralExpr : public Expr {
public:
	union Value {
		int i;
		bool b;
	} as;

	LiteralExpr(Token token, int value);
	LiteralExpr(Token token, bool value);
	void accept(CSTVisitor& visitor) override;
};


class IdExpr : public Expr {
public:
	std::string identifier;

	IdExpr(Token token, std::string lexeme);
	void accept(CSTVisitor& visitor) override;
};
