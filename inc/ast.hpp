#pragma once
#include <string>
#include <utility>
#include "token.hpp"
#include "ast-visitor.hpp"
#include "type.hpp"
#include "parse-tree.hpp"


struct Param {
	std::string name;
	Type type;

	Param(std::string name, Type type);
};


class ASTNode {
public:
	Token token;
	ASTNode(Token token);
	virtual ~ASTNode() = default;
	virtual void accept(ASTVisitor& visitor) = 0;
};


class FunNode : public ASTNode {
public:
	std::string id;
	Type type;
	std::vector<Param> params;
	ASTNode* body;

	FunNode(Token token, std::string id, Type type,
	        std::vector<Param> params, ASTNode* body);
	~FunNode() override;
	void accept(ASTVisitor& visitor) override;
};


class AssignNode : public ASTNode {
public:
	std::string id;
	Expr* expr;

	AssignNode(Token token, std::string id, Expr* expr);
	~AssignNode() override;
	void accept(ASTVisitor& visitor) override;
};


class IfNode : public ASTNode {
public:
	Expr* cond;
	ASTNode* then;
	ASTNode* otherwise;

	IfNode(Token token, Expr* cond, ASTNode* then, ASTNode* otherwise);
	~IfNode() override;
	void accept(ASTVisitor& visitor) override;
};


class WhileNode : public ASTNode {
public:
	Expr* cond;
	ASTNode* body;

	WhileNode(Token token, Expr* cond, ASTNode* body);
	~WhileNode() override;
	void accept(ASTVisitor& visitor) override;
};


class ReturnNode : public ASTNode {
public:
	Expr* expr;

	ReturnNode(Token token, Expr* expr);
	~ReturnNode() override;
	void accept(ASTVisitor& visitor) override;
};



class NopNode : public ASTNode {
public:
	NopNode(Token token);
	void accept(ASTVisitor& visitor) override;
};


class SeqNode : public ASTNode {
public:
	ASTNode* head;
	ASTNode* rest;

	SeqNode(Token token, ASTNode* head, ASTNode* rest);
	~SeqNode() override;
	void accept(ASTVisitor& visitor) override;
};


class DeclNode : public ASTNode {
public:
	std::string id;
	Type type;
	ASTNode* scope;

	DeclNode(Token token, std::string id, Type type, ASTNode* scope);
	~DeclNode() override;
	void accept(ASTVisitor& visitor) override;
};


class ExprNode : public ASTNode {
public:
	Expr* expr;

	ExprNode(Token token, Expr* expr);
	~ExprNode() override;
	void accept(ASTVisitor& visitor) override;
};
