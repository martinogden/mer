#include "parser.hpp"
#include <sstream>


Token END(TokenType::END, "", 0, 0, 0);


class ParseError : public std::exception {
public:
	std::string msg;
	Token token;

	ParseError(std::string msg, Token token) :
		msg(msg),
		token(token)
	{}

	virtual const char* what() const noexcept {
		return msg.c_str();
	}
};


Parser::Parser(std::string src) :
	lexer(src),
	curr(END),
	next(END)
{}


Prog* Parser::run() {
	try {
		return program();
	}
	catch (ParseError e) {
		addError(e.what(), e.token);
	}

	return nullptr;
}


Prog* Parser::program() {
	advance();
	advance();
	programPrologue();

	std::vector<Stmt*> stmts;

	while (!isAtEnd()) {
		if (match(TokenType::RBRACE))
			break;

		try {
			stmts.push_back(statement());
		}
		catch(ParseError e) {
			// ignore rest of line on syntax error
			addError(e.what(), e.token);
			consumeLine();
		}
	}

	Token token = programEpilogue();
	expect(TokenType::END);

	if (errors.size() > 0)
		return nullptr;

	return new Prog(token, stmts);
}


void Parser::programPrologue() {
	expect(TokenType::INT);

	Token token = get();
	expect(TokenType::IDENT);
	if (token.lexeme != "main")
		addError("Expected 'main', got '" + token.lexeme + "'", token);

	expect(TokenType::LPAREN);
	expect(TokenType::RPAREN);
	expect(TokenType::LBRACE);
}


Token Parser::programEpilogue() {
	Token token = get();
	expect(TokenType::RBRACE);
	return token;
}


Stmt* Parser::statement() {
	Token token = get();
	if (accept(TokenType::RETURN))
		return returnStmt(token);

	if (accept(TokenType::INT))
		return declStmt(token);

	if (accept(TokenType::LBRACE))
		return block(token);

	return assignStmt(token);
}


Stmt* Parser::returnStmt(Token& token) {
	Expr* expr = expression();
	expect(TokenType::SEMICOLON);
	return new ReturnStmt(token, expr);
}


Stmt* Parser::declStmt(Token& token) {
	Token id = get();
	expect(TokenType::IDENT);

	Expr* expr = nullptr;
	// TODO add definition later
	// if (accept(TokenType::EQL))
	// 	expr = expression();

	expect(TokenType::SEMICOLON);

	return new DeclStmt(token, id.lexeme, expr);
}


Stmt* Parser::block(Token& token) {
	std::vector<Stmt*> stmts;

	while (!accept(TokenType::RBRACE))
		stmts.push_back(statement());

	return new BlockStmt(token, stmts);
}


Stmt* Parser::assignStmt(Token& token) {
	std::string lval = lvalue();
	Token asnop = get();

	bool isasnop = accept(TokenType::ADD_EQL) || accept(TokenType::SUB_EQL) ||
				   accept(TokenType::MUL_EQL) || accept(TokenType::DIV_EQL) ||
				   accept(TokenType::MOD_EQL) || accept(TokenType::EQL);

	if (!isasnop)
		throw ParseError("Expected assignment operator", token);

	Expr* rval = expression();
	expect(TokenType::SEMICOLON);
	return new AssignStmt(token, lval, toAsnOp(asnop.type), rval);
}


// TODO this will return Expr* eventually
std::string Parser::lvalue() {
	if (accept(TokenType::LPAREN)) {
		std::string name = lvalue();
		expect(TokenType::RPAREN);
		return name;
	}

	Token token = get();
	expect(TokenType::IDENT);
	return token.lexeme;
}


Expr* Parser::expression() {
	return addition();
}

Expr* Parser::addition() {
	Expr* left = multiplication();
	Token token = get();

	while ( accept(TokenType::ADD) || accept(TokenType::SUB) ) {
		Expr* right = multiplication();
		left = new BinaryExpr(token, toBinOp(token.type), left, right);
		token = get();
	}

	return left;
}


Expr* Parser::multiplication() {
	Expr* left = unary();
	Token token = get();

	while ( accept(TokenType::MUL) || accept(TokenType::DIV) ) {
		Expr* right = unary();
		left = new BinaryExpr(token, toBinOp(token.type), left, right);
		token = get();
	}

	return left;
}


Expr* Parser::unary() {
	Token token = get();
	if (accept(TokenType::SUB))
		return new UnaryExpr(token, toUnOp(token.type), unary());

	return primary();
}


Expr* Parser::primary() {
	Token token = get();

	if (accept(TokenType::LPAREN)) {
		Expr* expr = expression();
		expect(TokenType::RPAREN);
		return expr;
	}

	if (accept(TokenType::NUM))
		return new LiteralExpr(token);

	if(accept(TokenType::IDENT))
		return new VarExpr(token);

	// we should have seen an expression here
	std::string msg = "Expected an expression, got: \"" + token.lexeme + "\"";
	throw ParseError(msg, token);
	return nullptr;  // keep compiler happy
}


bool Parser::isAtEnd() {
	return match(TokenType::END);
}


bool Parser::match(TokenType type) {
	Token token = get();
	return token.type == type;
}


bool Parser::accept(TokenType type) {
	if (match(type)) {
		advance();
		return true;
	}

	return false;
}


void Parser::expect(TokenType type) {
	Token token = get();

	if (!accept(type)) {
		std::stringstream msg;
		msg << "Unexpected token. ";
		msg << "Expected: \"" << type << "\", ";
		msg << "got: \"" << token.type << "\".";
		throw ParseError(msg.str(), token);
	}
}


void Parser::advance() {
	// skip lexical errors
	while (next.type == TokenType::ERROR) {
		addError(next.lexeme, next);
		next = lexer.nextToken();
	}

	curr = next;
	next = lexer.nextToken();
}


Token Parser::get() {
	return curr;
}


Token Parser::peek() {
	return next;
}


void Parser::consumeLine() {
	while (!isAtEnd()) {
		if (accept(TokenType::SEMICOLON))
			return;
		advance();
	}
}


void Parser::addError(std::string msg, Token& token) {
	errors.push_back(error(msg, token));
}


std::vector<std::string>& Parser::getErrors() {
	return errors;
}


UnOp Parser::toUnOp(TokenType type) {
	switch(type) {
		case TokenType::SUB:
			return UnOp::NEG;
		default:
			throw 1;  // TODO: we should never get here
	}
}


BinOp Parser::toBinOp(TokenType type) {
	switch(type) {
		case TokenType::ADD:
			return BinOp::ADD;
		case TokenType::SUB:
			return BinOp::SUB;
		case TokenType::MUL:
			return BinOp::MUL;
		case TokenType::DIV:
			return BinOp::DIV;
		case TokenType::MOD:
			return BinOp::MOD;
		default:
			throw 1;  // TODO: we should never get here
	}
}


AsnOp Parser::toAsnOp(TokenType type) {
	switch(type) {
		case TokenType::ADD_EQL:
			return AsnOp::ADD_EQL;
		case TokenType::SUB_EQL:
			return AsnOp::SUB_EQL;
		case TokenType::MUL_EQL:
			return AsnOp::MUL_EQL;
		case TokenType::DIV_EQL:
			return AsnOp::DIV_EQL;
		case TokenType::MOD_EQL:
			return AsnOp::MOD_EQL;
		case TokenType::EQL:
			return AsnOp::EQL;
		default:
			throw 1;  // TODO: we should never get here
	}
}