#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "token.hpp"
#include "parse-tree-visitor.hpp"
#include "operator.hpp"
#include "type.hpp"


class Expr;
class Stmt;
class DeclStmt;
class FunDecl;

typedef std::unique_ptr<Expr> ExprPtr;
typedef std::unique_ptr<Stmt> StmtPtr;
typedef std::unique_ptr<DeclStmt> DeclStmtPtr;
typedef std::unique_ptr<FunDecl> FunDeclPtr;


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


class FunDecl : public Stmt {
public:
	Token type;
	std::string identifier;
	std::vector<DeclStmtPtr> params;

	FunDecl(Token token, std::string identifier, Token type, std::vector<DeclStmtPtr> params);
	void accept(Visitor& visitor) override;
};


class FunDefn : public Stmt {
public:
	FunDeclPtr decl;
	StmtPtr body;

	FunDefn(Token token, FunDeclPtr decl, StmtPtr body);
	void accept(Visitor& visitor) override;
};


class TypedefStmt : public Stmt {
public:
	Token type;
	Token alias;

	TypedefStmt(Token token, Token type, Token alias);
	void accept(Visitor& visitor) override;
};


class DeclStmt : public Stmt {
public:
	Token type;
	std::string identifier;
	ExprPtr expr;

	DeclStmt(Token token, std::string identifier, Token type, ExprPtr expr);
	void accept(Visitor& visitor) override;
};


class IfStmt : public Stmt {
public:
	ExprPtr cond;
	StmtPtr then;
	StmtPtr otherwise;

	IfStmt(Token token, ExprPtr cond, StmtPtr then, StmtPtr otherwise);
	void accept(Visitor& visitor) override;
};


class WhileStmt : public Stmt {
public:
	ExprPtr cond;
	StmtPtr body;

	WhileStmt(Token token, ExprPtr cond, StmtPtr body);
	void accept(Visitor& visitor) override;
};


class ForStmt : public Stmt {
public:
	StmtPtr init;
	ExprPtr cond;
	StmtPtr step;
	StmtPtr body;

	ForStmt(Token token, StmtPtr init, ExprPtr cond, StmtPtr step, StmtPtr body);
	void accept(Visitor& visitor) override;
};


class ReturnStmt : public Stmt {
public:
	ExprPtr expr;

	ReturnStmt(Token token, ExprPtr expr);
	void accept(Visitor& visitor) override;
};


class BlockStmt : public Stmt {
public:
	std::vector<StmtPtr> statements;

	BlockStmt(Token token, std::vector<StmtPtr> statements);
	void accept(Visitor& visitor) override;
};


class ExprStmt : public Stmt {
public:
	ExprPtr expr;

	ExprStmt(Token token, ExprPtr expr);
	void accept(Visitor& visitor) override;
};


class AssignStmt : public Stmt {
public:
	BinOp op;
	ExprPtr lvalue;
	ExprPtr rvalue;

	AssignStmt(Token token, BinOp op, ExprPtr lvalue, ExprPtr rvalue);
	void accept(Visitor& visitor) override;
};


class PostOpStmt : public Stmt {
public:
	BinOp op;
	ExprPtr expr;

	PostOpStmt(Token token, BinOp op, ExprPtr expr);
	void accept(Visitor& visitor) override;
};



class CallExpr : public Expr {
public:
	std::string identifier;
	std::vector<ExprPtr> args;

	CallExpr(Token token, std::string identifier, std::vector<ExprPtr> args);
	void accept(Visitor& visitor) override;
};


class TernaryExpr : public Expr {
public:
	ExprPtr cond;
	ExprPtr then;
	ExprPtr otherwise;

	TernaryExpr(Token token, ExprPtr cond, ExprPtr then, ExprPtr otherwise);
	void accept(Visitor& visitor) override;
};


class BinaryExpr : public Expr {
public:
	BinOp op;
	ExprPtr left;
	ExprPtr right;

	BinaryExpr(Token token, BinOp op, ExprPtr left, ExprPtr right);
	void accept(Visitor& visitor) override;
};


class UnaryExpr : public Expr {
public:
	UnOp op;
	ExprPtr expr;

	UnaryExpr(Token token, UnOp op, ExprPtr expr);
	void accept(Visitor& visitor) override;
};


class LiteralExpr : public Expr {
public:
	union Value {
		int i;
		bool b;
	} as;

	LiteralExpr(Token token, int value);
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
	std::vector<StmtPtr> decls;
	ParseTree(std::vector<StmtPtr> decls);
};
