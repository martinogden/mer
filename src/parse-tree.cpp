#include "parse-tree.hpp"


Expr::Expr(Token token, Type type) :
	token(std::move(token)),
	type(type)
{}


Stmt::Stmt(Token token) :
	token(std::move(token))
{}


FunDecl::FunDecl(Token token, std::string identifier, Token type, std::vector<DeclStmtPtr> params) :
	Stmt(std::move(token)),
	type(std::move(type)),
	identifier(std::move(identifier)),
	params(std::move(params))
{}


void FunDecl::accept(Visitor& visitor) {
	visitor.visit(*this);
};


FunDefn::FunDefn(Token token, FunDeclPtr decl, StmtPtr body) :
	Stmt(std::move(token)),
	decl(std::move(decl)),
	body(std::move(body))
{}


void FunDefn::accept(Visitor& visitor) {
	visitor.visit(*this);
};


TypedefStmt::TypedefStmt(Token token, Token type, Token alias) :
	Stmt(std::move(token)),
	type(std::move(type)),
	alias(std::move(alias))
{}


void TypedefStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};



DeclStmt::DeclStmt(Token token, std::string identifier, Token type, ExprPtr expr) :
	Stmt(std::move(token)),
	type(std::move(type)),
	identifier(std::move(identifier)),
	expr(std::move(expr))
{}


void DeclStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};


IfStmt::IfStmt(Token token, ExprPtr cond, StmtPtr then, StmtPtr otherwise) :
	Stmt(std::move(token)),
	cond(std::move(cond)),
	then(std::move(then)),
	otherwise(std::move(otherwise))
{}


void IfStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};


WhileStmt::WhileStmt(Token token, ExprPtr cond, StmtPtr body) :
	Stmt(std::move(token)),
	cond(std::move(cond)),
	body(std::move(body))
{}


void WhileStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};


ForStmt::ForStmt(Token token, StmtPtr init, ExprPtr cond, StmtPtr step, StmtPtr body) :
	Stmt(std::move(token)),
	init(std::move(init)),
	cond(std::move(cond)),
	step(std::move(step)),
	body(std::move(body))
{}


void ForStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};


ReturnStmt::ReturnStmt(Token token, ExprPtr expr) :
	Stmt(std::move(token)),
	expr(std::move(expr))
{}


void ReturnStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};


BlockStmt::BlockStmt(Token token, std::vector<StmtPtr> statements) :
	Stmt(std::move(token)),
	statements(std::move(statements))
{}


void BlockStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};


ExprStmt::ExprStmt(Token token, ExprPtr expr) :
	Stmt(std::move(token)),
	expr(std::move(expr))
{}


AssignStmt::AssignStmt(Token token, BinOp op, ExprPtr lvalue, ExprPtr rvalue) :
	Stmt(std::move(token)),
	op(op),
	lvalue(std::move(lvalue)),
	rvalue(std::move(rvalue))
{}


void AssignStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};


PostOpStmt::PostOpStmt(Token token, BinOp op, ExprPtr expr) :
	Stmt(std::move(token)),
	op(op),
	expr(std::move(expr))
{}


void PostOpStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};


void ExprStmt::accept(Visitor& visitor) {
	visitor.visit(*this);
};


CallExpr::CallExpr(Token token, std::string identifier, std::vector<ExprPtr> args) :
	Expr(std::move(token)),
	identifier(std::move(identifier)),
	args(std::move(args))
{}


void CallExpr::accept(Visitor& visitor) {
	visitor.visit(*this);
};


TernaryExpr::TernaryExpr(Token token, ExprPtr cond, ExprPtr then, ExprPtr otherwise) :
	Expr(std::move(token)),
	cond(std::move(cond)),
	then(std::move(then)),
	otherwise(std::move(otherwise))
{}


void TernaryExpr::accept(Visitor& visitor) {
	visitor.visit(*this);
};


BinaryExpr::BinaryExpr(Token token, BinOp op, ExprPtr left, ExprPtr right) :
	Expr(std::move(token)),
	op(op),
	left(std::move(left)),
	right(std::move(right))
{}


void BinaryExpr::accept(Visitor& visitor) {
	visitor.visit(*this);
};


UnaryExpr::UnaryExpr(Token token, UnOp op, ExprPtr expr) :
	Expr(std::move(token)),
	op(op),
	expr(std::move(expr))
{}


void UnaryExpr::accept(Visitor& visitor) {
	visitor.visit(*this);
};


LiteralExpr::LiteralExpr(Token token, int value) :
	Expr(std::move(token), Type::INT),
	as({.i = value})
{}


LiteralExpr::LiteralExpr(Token token, bool value) :
	Expr(std::move(token), Type::BOOL),
	as({.b = value})
{}


void LiteralExpr::accept(Visitor& visitor) {
	visitor.visit(*this);
};


IdExpr::IdExpr(Token token, std::string lexeme) :
	Expr(std::move(token)),
	identifier(std::move(lexeme))
{}


void IdExpr::accept(Visitor& visitor) {
	visitor.visit(*this);
};


ParseTree::ParseTree(std::vector<StmtPtr> decls) :
	decls(std::move(decls))
{}
