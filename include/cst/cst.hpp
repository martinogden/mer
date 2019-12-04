#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "expr/expr.hpp"
#include "token.hpp"
#include "visitor.hpp"
#include "operator.hpp"
#include "type/type.hpp"


class Stmt;
class DeclStmt;
class FunDecl;

typedef std::unique_ptr<Stmt> StmtPtr;
typedef std::unique_ptr<DeclStmt> DeclStmtPtr;
typedef std::unique_ptr<FunDecl> FunDeclPtr;


class Stmt {
public:
	Token token;

	Stmt(Token token);
	virtual ~Stmt() = default;
	virtual void accept(CSTVisitor& visitor) = 0;
};


class FunDecl : public Stmt {
public:
	Token type;
	std::string identifier;
	std::vector<DeclStmtPtr> params;

	FunDecl(Token token, std::string identifier, Token type, std::vector<DeclStmtPtr> params);
	void accept(CSTVisitor& visitor) override;
};


class FunDefn : public Stmt {
public:
	FunDeclPtr decl;
	StmtPtr body;

	FunDefn(Token token, FunDeclPtr decl, StmtPtr body);
	void accept(CSTVisitor& visitor) override;
};


class TypedefStmt : public Stmt {
public:
	Token type;
	Token alias;

	TypedefStmt(Token token, Token type, Token alias);
	void accept(CSTVisitor& visitor) override;
};


class DeclStmt : public Stmt {
public:
	Token type;
	std::string identifier;
	ExprPtr expr;

	DeclStmt(Token token, std::string identifier, Token type, ExprPtr expr);
	void accept(CSTVisitor& visitor) override;
};


class IfStmt : public Stmt {
public:
	ExprPtr cond;
	StmtPtr then;
	StmtPtr otherwise;

	IfStmt(Token token, ExprPtr cond, StmtPtr then, StmtPtr otherwise);
	void accept(CSTVisitor& visitor) override;
};


class WhileStmt : public Stmt {
public:
	ExprPtr cond;
	StmtPtr body;

	WhileStmt(Token token, ExprPtr cond, StmtPtr body);
	void accept(CSTVisitor& visitor) override;
};


class ForStmt : public Stmt {
public:
	StmtPtr init;
	ExprPtr cond;
	StmtPtr step;
	StmtPtr body;

	ForStmt(Token token, StmtPtr init, ExprPtr cond, StmtPtr step, StmtPtr body);
	void accept(CSTVisitor& visitor) override;
};


class ReturnStmt : public Stmt {
public:
	ExprPtr expr;

	ReturnStmt(Token token, ExprPtr expr);
	void accept(CSTVisitor& visitor) override;
};


class BlockStmt : public Stmt {
public:
	std::vector<StmtPtr> statements;

	BlockStmt(Token token, std::vector<StmtPtr> statements);
	void accept(CSTVisitor& visitor) override;
};


class ExprStmt : public Stmt {
public:
	ExprPtr expr;

	ExprStmt(Token token, ExprPtr expr);
	void accept(CSTVisitor& visitor) override;
};


class AssignStmt : public Stmt {
public:
	BinOp op;
	ExprPtr lvalue;
	ExprPtr rvalue;

	AssignStmt(Token token, BinOp op, ExprPtr lvalue, ExprPtr rvalue);
	void accept(CSTVisitor& visitor) override;
};


class PostOpStmt : public Stmt {
public:
	BinOp op;
	ExprPtr expr;

	PostOpStmt(Token token, BinOp op, ExprPtr expr);
	void accept(CSTVisitor& visitor) override;
};


class ParseTree {
public:
	std::vector<StmtPtr> decls;
	ParseTree(std::vector<StmtPtr> decls);
};
