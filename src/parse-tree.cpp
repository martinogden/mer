#include "parse-tree.hpp"


Expr::Expr(Token token, Type type) :
	token(std::move(token)),
	type(type)
{}


Stmt::Stmt(Token token) :
	token(std::move(token))
{}


FunDecl::FunDecl(Token token, std::string identifier,
                 Token type, std::vector<DeclStmt*> params) :
	Stmt(std::move(token)),
	type(std::move(type)),
	identifier(std::move(identifier)),
	params(std::move(params))
{}


FunDecl::~FunDecl() {
	for (auto const& param : params)
		delete param;
}


void FunDecl::accept(Visitor& visitor) {
	visitor.visit(this);
};


FunDefn::FunDefn(Token token, FunDecl* decl, Stmt* body) :
	Stmt(std::move(token)),
	decl(decl),
	body(body)
{}


FunDefn::~FunDefn() {
	delete decl;
	delete body;
}


void FunDefn::accept(Visitor& visitor) {
	visitor.visit(this);
};


TypedefStmt::TypedefStmt(Token token, Token type, Token alias) :
	Stmt(std::move(token)),
	type(std::move(type)),
	alias(std::move(alias))
{}


TypedefStmt::~TypedefStmt() = default;


void TypedefStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};



DeclStmt::DeclStmt(Token token, std::string identifier, Token type, Expr* expr) :
	Stmt(std::move(token)),
	type(std::move(type)),
	identifier(std::move(identifier)),
	expr(expr)
{}


DeclStmt::~DeclStmt() {
	delete expr;
}


void DeclStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


IfStmt::IfStmt(Token token, Expr* cond, Stmt* then, Stmt* otherwise) :
	Stmt(std::move(token)),
	cond(cond),
	then(then),
	otherwise(otherwise)
{}


IfStmt::~IfStmt() {
	delete cond;
	delete then;
	delete otherwise;
}


void IfStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


WhileStmt::WhileStmt(Token token, Expr* cond, Stmt* body) :
	Stmt(std::move(token)),
	cond(cond),
	body(body)
{}


WhileStmt::~WhileStmt() {
	delete cond;
	delete body;
}


void WhileStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


ForStmt::ForStmt(Token token, Stmt* init, Expr* cond, Stmt* step, Stmt* body) :
	Stmt(std::move(token)),
	init(init),
	cond(cond),
	step(step),
	body(body)
{}


ForStmt::~ForStmt() {
	delete init;
	delete cond;
	delete step;
	delete body;
}


void ForStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


ReturnStmt::ReturnStmt(Token token, Expr* expr) :
	Stmt(std::move(token)),
	expr(expr)
{}


void ReturnStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


ReturnStmt::~ReturnStmt() {
	delete expr;
}


BlockStmt::BlockStmt(Token token, std::vector<Stmt*> statements) :
	Stmt(std::move(token)),
	statements(std::move(statements))
{}


BlockStmt::~BlockStmt() {
	for (auto stmt : statements)
		delete stmt;
}


void BlockStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


ExprStmt::ExprStmt(Token token, Expr* expr) :
	Stmt(std::move(token)),
	expr(expr)
{}


ExprStmt::~ExprStmt() {
	delete expr;
}


AssignStmt::AssignStmt(Token token, BinOp op, Expr* lvalue, Expr* rvalue) :
	Stmt(std::move(token)),
	op(op),
	lvalue(lvalue),
	rvalue(rvalue)
{}


AssignStmt::~AssignStmt() {
	delete lvalue;
	delete rvalue;
}


void AssignStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


PostOpStmt::PostOpStmt(Token token, BinOp op, Expr* expr) :
	Stmt(std::move(token)),
	op(op),
	expr(expr)
{}


PostOpStmt::~PostOpStmt() {
	delete expr;
}


void PostOpStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


void ExprStmt::accept(Visitor& visitor) {
	visitor.visit(this);
};


CallExpr::CallExpr(Token token, std::string identifier, std::vector<Expr*> args) :
	Expr(std::move(token)),
	identifier(std::move(identifier)),
	args(std::move(args)),
	decl(nullptr)
{}


CallExpr::~CallExpr() {
	for (auto const& arg : args)
		delete arg;
}


void CallExpr::accept(Visitor& visitor) {
	visitor.visit(this);
};


TernaryExpr::TernaryExpr(Token token, Expr* cond, Expr* then, Expr* otherwise) :
	Expr(std::move(token)),
	cond(cond),
	then(then),
	otherwise(otherwise)
{}


TernaryExpr::~TernaryExpr() {
	delete cond;
	delete then;
	delete otherwise;
}


void TernaryExpr::accept(Visitor& visitor) {
	visitor.visit(this);
};


BinaryExpr::BinaryExpr(Token token, BinOp op, Expr* left, Expr* right) :
	Expr(std::move(token)),
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
	Expr(std::move(token)),
	op(op),
	expr(expr)
{}


UnaryExpr::~UnaryExpr() {
	delete expr;
}


void UnaryExpr::accept(Visitor& visitor) {
	visitor.visit(this);
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
	visitor.visit(this);
};


IdExpr::IdExpr(Token token, std::string lexeme) :
	Expr(std::move(token)),
	identifier(std::move(lexeme))
{}


void IdExpr::accept(Visitor& visitor) {
	visitor.visit(this);
};


ParseTree::ParseTree(BlockStmt* block) :
	block(block)
{}


ParseTree::~ParseTree() {
	delete block;
}
