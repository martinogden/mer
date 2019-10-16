#include "ast.hpp"


Expr::Expr(Token token) :
	token(token)
{}


Stmt::Stmt(Token token) :
	token(token)
{}


DeclStmt::DeclStmt(Token token, std::string identifier, Expr* expr) :
	Stmt(token),
	identifier(identifier),
	expr(expr)
{}


DeclStmt::~DeclStmt() {
	delete expr;
}


void DeclStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


ReturnStmt::ReturnStmt(Token token, Expr* expr) :
	Stmt(token),
	expr(expr)
{}


void ReturnStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


ReturnStmt::~ReturnStmt() {
	delete expr;
}


BlockStmt::BlockStmt(Token token, std::vector<Stmt*> statements) :
	Stmt(token),
	statements(statements)
{}


BlockStmt::~BlockStmt() {
	for (auto stmt : statements)
		delete stmt;
}


void BlockStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


AssignStmt::AssignStmt(Token token, std::string lvalue, AsnOp op, Expr* rvalue) :
	Stmt(token),
	op(op),
	lvalue(lvalue),
	rvalue(rvalue)
{}


AssignStmt::~AssignStmt() {
	delete rvalue;
}


void AssignStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


BinaryExpr::BinaryExpr(Token token, BinOp op, Expr* left, Expr* right) :
	Expr(token),
	op(op),
	left(left),
	right(right)
{}


BinaryExpr::~BinaryExpr() {
	delete left;
	delete right;
}


void BinaryExpr::accept(Visitor& visitor) {
	visitor.visit(this);
};


UnaryExpr::UnaryExpr(Token token, UnOp op, Expr* expr) :
	Expr(token),
	op(op),
	expr(expr)
{}


UnaryExpr::~UnaryExpr() {
	delete expr;
}


void UnaryExpr::accept(Visitor& visitor) {
	visitor.visit(this);
};


LiteralExpr::LiteralExpr(Token token) :
	Expr(token),
	value(std::stoi(token.lexeme))
{}


void LiteralExpr::accept(Visitor& visitor) {
	visitor.visit(this);
};


VarExpr::VarExpr(Token token) :
	Expr(token),
	identifier(token.lexeme)
{}


void VarExpr::accept(Visitor& visitor) {
	visitor.visit(this);
};


Prog::Prog(Token token, std::vector<Stmt*> statements) :
	token(token),
	statements(statements)
{}


Prog::~Prog() {
	for (auto stmt : statements)
		delete stmt;
}


void Prog::accept(Visitor& visitor) {
	visitor.visit(this);
};
