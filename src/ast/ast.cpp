#include "ast/ast.hpp"


ASTNode::ASTNode(Token token) :
	token(std::move(token))
{}


FunNode::FunNode(Token token, std::string id, FunTypePtr type, std::vector<Param> params, ASTNodePtr body) :
	ASTNode(std::move(token)),
	id(std::move(id)),
	type(std::move(type)),
	params(std::move(params)),
	body(std::move(body))
{}


FunNode::Param::Param(Token token, std::string name) :
	token(std::move(token)),
	name(std::move(name))
{}


void FunNode::accept(ASTVisitor& visitor) {
	visitor.visit(*this);
};


AssignNode::AssignNode(Token token, ExprPtr lvalue, ExprPtr rvalue) :
	ASTNode(std::move(token)),
	lvalue(std::move(lvalue)),
	rvalue(std::move(rvalue))
{}


void AssignNode::accept(ASTVisitor& visitor) {
	visitor.visit(*this);
};


IfNode::IfNode(Token token, ExprPtr cond, ASTNodePtr then, ASTNodePtr otherwise) :
	ASTNode(std::move(token)),
	cond(std::move(cond)),
	then(std::move(then)),
	otherwise(std::move(otherwise))
{}


void IfNode::accept(ASTVisitor& visitor) {
	visitor.visit(*this);
};


WhileNode::WhileNode(Token token, ExprPtr cond, ASTNodePtr body) :
	ASTNode(std::move(token)),
	cond(std::move(cond)),
	body(std::move(body))
{}


void WhileNode::accept(ASTVisitor& visitor) {
	visitor.visit(*this);
};


ReturnNode::ReturnNode(Token token, ExprPtr expr) :
	ASTNode(std::move(token)),
	expr(std::move(expr))
{}


void ReturnNode::accept(ASTVisitor& visitor) {
	visitor.visit(*this);
};


NopNode::NopNode(Token token) :
	ASTNode(std::move(token))
{}


void NopNode::accept(ASTVisitor& visitor) {
	visitor.visit(*this);
};


SeqNode::SeqNode(Token token, ASTNodePtr head, ASTNodePtr rest) :
	ASTNode(std::move(token)),
	head(std::move(head)),
	rest(std::move(rest))
{}


void SeqNode::accept(ASTVisitor& visitor) {
	visitor.visit(*this);
};


DeclNode::DeclNode(Token token, std::string id, TypePtr type, ASTNodePtr scope) :
	ASTNode(std::move(token)),
	id(std::move(id)),
	type(std::move(type)),
	scope(std::move(scope))
{}


void DeclNode::accept(ASTVisitor& visitor) {
	visitor.visit(*this);
};


ExprNode::ExprNode(Token token, ExprPtr expr) :
	ASTNode(std::move(token)),
	expr(std::move(expr))
{}


void ExprNode::accept(ASTVisitor& visitor) {
	visitor.visit(*this);
};
