#pragma once
#include <iostream>
#include <vector>
#include "token.hpp"
#include "visitor.hpp"
#include "operator.hpp"


class Expr {
public:
	Token token;

	Expr(Token token);
	virtual ~Expr() = default;
	virtual void accept(Visitor& visitor) = 0;
};


class Stmt {
public:
	Token token;

	Stmt(Token token);
	virtual ~Stmt() = default;
	virtual void accept(Visitor& visitor) = 0;
};


class DeclStmt : public Stmt {
public:
	std::string identifier;
	Expr* expr;

	DeclStmt(Token token, std::string identifier, Expr* expr);
	~DeclStmt();
	void accept(Visitor& visitor) override;
};


class ReturnStmt : public Stmt {
public:
	Expr* expr;

	ReturnStmt(Token token, Expr* expr);
	~ReturnStmt();
	void accept(Visitor& visitor) override;
};


class BlockStmt : public Stmt {
public:
	std::vector<Stmt*> statements;

	BlockStmt(Token token, std::vector<Stmt*> statements);
	~BlockStmt();
	void accept(Visitor& visitor) override;
};


class AssignStmt : public Stmt {
public:
	AsnOp op;
	std::string lvalue;
	Expr* rvalue;

	AssignStmt(Token token, std::string lvalue, AsnOp op, Expr* rvalue);
	~AssignStmt();
	void accept(Visitor& visitor) override;
};


class BinaryExpr : public Expr {
public:
	BinOp op;
	Expr* left;
	Expr* right;

	BinaryExpr(Token token, BinOp op, Expr* left, Expr* right);
	~BinaryExpr();
	void accept(Visitor& visitor) override;
};


class UnaryExpr : public Expr {
public:
	UnOp op;
	Expr* expr;

	UnaryExpr(Token token, UnOp op, Expr* expr);
	~UnaryExpr();
	void accept(Visitor& visitor) override;
};


class LiteralExpr : public Expr {
public:
	int value;

	LiteralExpr(Token token);
	void accept(Visitor& visitor) override;
};


class VarExpr : public Expr {
public:
	std::string identifier;

	VarExpr(Token token);
	void accept(Visitor& visitor) override;
};


class Prog {
public:
	Token token;
	std::vector<Stmt*> statements;

	Prog(Token token, std::vector<Stmt*> statements);
	~Prog();
	void accept(Visitor& visitor);
};
