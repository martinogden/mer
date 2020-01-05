#include "cst/cst.hpp"


Stmt::Stmt(Token token) :
	token(std::move(token))
{}


FunDecl::FunDecl(Token token, std::string identifier, TypePtr type, std::vector<DeclStmtPtr> params) :
	Stmt(std::move(token)),
	type(std::move(type)),
	identifier(std::move(identifier)),
	params(std::move(params))
{}


void FunDecl::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


FunDefn::FunDefn(Token token, FunDeclPtr decl, StmtPtr body) :
	Stmt(std::move(token)),
	decl(std::move(decl)),
	body(std::move(body))
{}


void FunDefn::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


StructDecl::StructDecl(Token token, std::string identifier) :
	Stmt(std::move(token)),
	identifier(std::move(identifier))
{}


void StructDecl::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


StructDefn::StructDefn(Token token, StructDeclPtr decl, std::vector<DeclStmtPtr> fields) :
	Stmt(std::move(token)),
	decl(std::move(decl)),
	fields(std::move(fields))
{}


void StructDefn::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


DeclStmt::DeclStmt(Token token, std::string identifier, TypePtr type, ExprPtr expr) :
	Stmt(std::move(token)),
	type(std::move(type)),
	identifier(std::move(identifier)),
	expr(std::move(expr))
{}


void DeclStmt::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


TypedefStmt::TypedefStmt(Token token, TypePtr type, Token alias) :
	Stmt(std::move(token)),
	type(std::move(type)),
	alias(std::move(alias))
{}


void TypedefStmt::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


IfStmt::IfStmt(Token token, ExprPtr cond, StmtPtr then, StmtPtr otherwise) :
	Stmt(std::move(token)),
	cond(std::move(cond)),
	then(std::move(then)),
	otherwise(std::move(otherwise))
{}


void IfStmt::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


WhileStmt::WhileStmt(Token token, ExprPtr cond, StmtPtr body) :
	Stmt(std::move(token)),
	cond(std::move(cond)),
	body(std::move(body))
{}


void WhileStmt::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


ForStmt::ForStmt(Token token, StmtPtr init, ExprPtr cond, StmtPtr step, StmtPtr body) :
	Stmt(std::move(token)),
	init(std::move(init)),
	cond(std::move(cond)),
	step(std::move(step)),
	body(std::move(body))
{}


void ForStmt::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


ReturnStmt::ReturnStmt(Token token, ExprPtr expr) :
	Stmt(std::move(token)),
	expr(std::move(expr))
{}


void ReturnStmt::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


BlockStmt::BlockStmt(Token token, std::vector<StmtPtr> statements) :
	Stmt(std::move(token)),
	statements(std::move(statements))
{}


void BlockStmt::accept(CSTVisitor& visitor) {
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


void AssignStmt::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


PostOpStmt::PostOpStmt(Token token, BinOp op, ExprPtr expr) :
	Stmt(std::move(token)),
	op(op),
	expr(std::move(expr))
{}


void PostOpStmt::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


void ExprStmt::accept(CSTVisitor& visitor) {
	visitor.visit(*this);
};


ParseTree::ParseTree(std::vector<StmtPtr> decls) :
	decls(std::move(decls))
{}
