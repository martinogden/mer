#pragma once
#include <memory>
#include <string>
#include <utility>
#include "parser/token.hpp"
#include "visitor.hpp"
#include "type/type.hpp"
#include "cst/cst.hpp"


struct Decl;
typedef std::unique_ptr<Decl> DeclPtr;


class ASTNode {
public:
	Token token;
	ASTNode(Token token);
	virtual ~ASTNode() = default;
	virtual void accept(ASTVisitor& visitor) = 0;
};


typedef std::unique_ptr<ASTNode> ASTNodePtr;


class FunNode : public ASTNode {
public:
	struct Param {
		Token token;
		std::string name;
		Param(Token token, std::string name);
	};

	std::string id;
	FunTypePtr type;
	std::vector<Param> params;
	ASTNodePtr body;

	FunNode(Token token, std::string id, FunTypePtr type, std::vector<Param> params, ASTNodePtr body);
	void accept(ASTVisitor& visitor) override;
};


typedef std::unique_ptr<FunNode> FunNodePtr;


class AssignNode : public ASTNode {
public:
	ExprPtr lvalue;
	ExprPtr rvalue;

	AssignNode(Token token, ExprPtr lvalue, ExprPtr rvalue);
	void accept(ASTVisitor& visitor) override;
};


class IfNode : public ASTNode {
public:
	ExprPtr cond;
	ASTNodePtr then;
	ASTNodePtr otherwise;

	IfNode(Token token, ExprPtr cond, ASTNodePtr then, ASTNodePtr otherwise);
	void accept(ASTVisitor& visitor) override;
};


class WhileNode : public ASTNode {
public:
	ExprPtr cond;
	ASTNodePtr body;

	WhileNode(Token token, ExprPtr cond, ASTNodePtr body);
	void accept(ASTVisitor& visitor) override;
};


class ReturnNode : public ASTNode {
public:
	ExprPtr expr;

	ReturnNode(Token token, ExprPtr expr);
	void accept(ASTVisitor& visitor) override;
};



class NopNode : public ASTNode {
public:
	NopNode(Token token);
	void accept(ASTVisitor& visitor) override;
};


class SeqNode : public ASTNode {
public:
	ASTNodePtr head;
	ASTNodePtr rest;

	SeqNode(Token token, ASTNodePtr head, ASTNodePtr rest);
	void accept(ASTVisitor& visitor) override;
};


class DeclNode : public ASTNode {
public:
	std::string id;
	TypePtr type;
	ASTNodePtr scope;

	DeclNode(Token token, std::string id, TypePtr type, ASTNodePtr scope);
	void accept(ASTVisitor& visitor) override;
};


class ExprNode : public ASTNode {
public:
	ExprPtr expr;

	ExprNode(Token token, ExprPtr expr);
	void accept(ASTVisitor& visitor) override;
};
