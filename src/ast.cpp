#include "ast.hpp"


Param::Param(std::string name, Type type) :
	name(std::move(name)),
	type(type)
{}


ASTNode::ASTNode(Token token) :
	token(std::move(token))
{}


FunNode::FunNode(Token token, std::string id, Type type,
	             std::vector<Param> params, ASTNode* body) :
	ASTNode(std::move(token)),
	id(std::move(id)),
	type(type),
	params(std::move(params)),
	body(body)
{}

FunNode::~FunNode() {
	delete body;
}


void FunNode::accept(ASTVisitor& visitor) {
	visitor.visit(this);
};


AssignNode::AssignNode(Token token, std::string id, Expr* expr) :
	ASTNode(std::move(token)),
	id(std::move(id)),
	expr(expr)
{}


AssignNode::~AssignNode() {
	delete expr;
}


void AssignNode::accept(ASTVisitor& visitor) {
	visitor.visit(this);
};


IfNode::IfNode(Token token, Expr* cond, ASTNode* then, ASTNode* otherwise) :
	ASTNode(std::move(token)),
	cond(cond),
	then(then),
	otherwise(otherwise)
{}


IfNode::~IfNode() {
	delete cond;
	delete then;
	delete otherwise;
}


void IfNode::accept(ASTVisitor& visitor) {
	visitor.visit(this);
};


WhileNode::WhileNode(Token token, Expr* cond, ASTNode* body) :
	ASTNode(std::move(token)),
	cond(cond),
	body(body)
{}


WhileNode::~WhileNode() {
	delete cond;
	delete body;
}


void WhileNode::accept(ASTVisitor& visitor) {
	visitor.visit(this);
};


ReturnNode::ReturnNode(Token token, Expr* expr) :
	ASTNode(std::move(token)),
	expr(expr)
{}


ReturnNode::~ReturnNode() {
	delete expr;
}


void ReturnNode::accept(ASTVisitor& visitor) {
	visitor.visit(this);
};


NopNode::NopNode(Token token) :
	ASTNode(std::move(token))
{}


void NopNode::accept(ASTVisitor& visitor) {
	visitor.visit(this);
};


SeqNode::SeqNode(Token token, ASTNode* head, ASTNode* rest) :
	ASTNode(std::move(token)),
	head(head),
	rest(rest)
{}


SeqNode::~SeqNode() {
	delete head;
	delete rest;
}


void SeqNode::accept(ASTVisitor& visitor) {
	visitor.visit(this);
};


DeclNode::DeclNode(Token token, std::string id, Type type, ASTNode* scope) :
	ASTNode(std::move(token)),
	id(std::move(id)),
	type(type),
	scope(scope)
{}


DeclNode::~DeclNode() {
	delete scope;
}


void DeclNode::accept(ASTVisitor& visitor) {
	visitor.visit(this);
};


ExprNode::ExprNode(Token token, Expr* expr) :
	ASTNode(std::move(token)),
	expr(expr)
{}


ExprNode::~ExprNode() {
	delete expr;
}


void ExprNode::accept(ASTVisitor& visitor) {
	visitor.visit(this);
};
