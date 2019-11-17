#pragma once
#include <iostream>
#include <vector>
#include "token.hpp"
#include "parse-tree-visitor.hpp"
#include "operator.hpp"
#include "type.hpp"


class Expr {
public:
	Token token;
	Type type;

	Expr(Token token, Type type=Type::UNKNOWN);
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
	Type type;
	std::string identifier;
	Expr* expr;

	DeclStmt(Token token, std::string identifier, Type type, Expr* expr);
	~DeclStmt();
	void accept(Visitor& visitor) override;
};


class IfStmt : public Stmt {
public:
	Expr* cond;
	Stmt* then;
	Stmt* otherwise;

	IfStmt(Token token, Expr* cond, Stmt* then, Stmt* otherwise);
	~IfStmt();
	void accept(Visitor& visitor) override;
};


class WhileStmt : public Stmt {
public:
	Expr* cond;
	Stmt* body;

	WhileStmt(Token token, Expr* cond, Stmt* body);
	~WhileStmt();
	void accept(Visitor& visitor) override;
};


class ForStmt : public Stmt {
public:
	Stmt* init;
	Expr* cond;
	Stmt* step;
	Stmt* body;

	ForStmt(Token token, Stmt* init, Expr* cond, Stmt* step, Stmt* body);
	~ForStmt();
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


class ExprStmt : public Stmt {
public:
	Expr* expr;

	ExprStmt(Token token, Expr* expr);
	~ExprStmt();
	void accept(Visitor& visitor) override;
};


class AssignStmt : public Stmt {
public:
	BinOp op;
	Expr* lvalue;
	Expr* rvalue;

	AssignStmt(Token token, BinOp op, Expr* lvalue, Expr* rvalue);
	~AssignStmt();
	void accept(Visitor& visitor) override;
};


class PostOpStmt : public Stmt {
public:
	BinOp op;
	Expr* expr;

	PostOpStmt(Token token, BinOp op, Expr* expr);
	~PostOpStmt();
	void accept(Visitor& visitor) override;
};



class TernaryExpr : public Expr {
public:
	Expr* cond;
	Expr* then;
	Expr* otherwise;

	TernaryExpr(Token token, Expr* cond, Expr* then, Expr* otherwise);
	~TernaryExpr();
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
	union Value {
		uint i;
		bool b;
	} as;

	LiteralExpr(Token token, uint value);
	LiteralExpr(Token token, bool value);
	void accept(Visitor& visitor) override;
};


class IdExpr : public Expr {
public:
	std::string identifier;

	IdExpr(Token token, std::string lexeme);
	void accept(Visitor& visitor) override;
};


class ParseTree {
public:
	BlockStmt* block;

	ParseTree(BlockStmt* block);
	~ParseTree();
};
