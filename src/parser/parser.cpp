#include "parser/parser.hpp"
#include "conversion.hpp"


ParserSpec createParserSpec();


std::set<TokenType> asnOps = {
	TokenType::EQL,
	TokenType::ADD_EQL, TokenType::SUB_EQL,
	TokenType::MUL_EQL, TokenType::DIV_EQL, TokenType::MOD_EQL,
	TokenType::AMP_EQL, TokenType::PIPE_EQL, TokenType::CARET_EQL,
	TokenType::LT_LT_EQL, TokenType::GT_GT_EQL,
};


std::set<TokenType> postOps = {
	TokenType::ADD_ADD, TokenType::SUB_SUB
};


Parser::Parser(Lexer& lexer) :
	PrattParser(createParserSpec(), lexer)
{}


ParseTree* Parser::run() {
	try {
		return new ParseTree( program() );
	}
	catch (ParseError e) {
		errors.add(e.what(), e.token);
	}

	return nullptr;
}


BlockStmt* Parser::program() {
	advance();
	advance();
	prologue();

	std::vector<Stmt*> stmts;
	Token token = get();

	while (!isAtEnd() && !match(TokenType::RBRACE)) {
		try {
			stmts.push_back(statement());
		}
		catch(ParseError e) {
			// ignore rest of line on syntax error
			errors.add(e.what(), e.token);
			consumeLine();
		}
	}

	epilogue();
	expect(TokenType::END);

	if (errors.exist())
		return nullptr;

	return new BlockStmt(token, stmts);
}


void Parser::prologue() {
	Token token = get();
	expect(TokenType::TYPE);
	if (token.lexeme != "int")
		errors.add("Expected 'int', got '" + token.lexeme + "'", token);

	token = get();
	expect(TokenType::IDENT);
	if (token.lexeme != "main")
		errors.add("Expected 'main', got '" + token.lexeme + "'", token);

	expect(TokenType::LPAREN);
	expect(TokenType::RPAREN);
	expect(TokenType::LBRACE);
}


void Parser::epilogue() {
	expect(TokenType::RBRACE);
}


Stmt* Parser::statement() {
	Token token = get();

	if (accept(TokenType::IF))
		return ifStmt(token);
	if (accept(TokenType::WHILE))
		return whileStmt(token);
	if (accept(TokenType::FOR))
		return forStmt(token);
	if (accept(TokenType::LBRACE))
		return block(token);

	// semicolon terminated stmts
	Stmt* stmt;
	if (accept(TokenType::RETURN))
		stmt = returnStmt(token);
	else if (accept(TokenType::TYPE))
		stmt = declStmt(token);
	else
		stmt = simpleStmt(token);

	expect(TokenType::SEMICOLON);
	return stmt;
}


Stmt* Parser::ifStmt(Token& token) {
	Expr* cond = condition();
	Stmt* then = statement();
	Stmt* otherwise = nullptr;

	if (accept(TokenType::ELSE))
		otherwise = statement();

	return new IfStmt(token, cond, then, otherwise);
}


Stmt* Parser::whileStmt(Token& token) {
	Expr* cond = condition();
	Stmt* body = statement();
	return new WhileStmt(token, cond, body);
}


Stmt* Parser::forStmt(Token& token) {
	expect(TokenType::LPAREN);

	Stmt* init = declOrSimpleStmtOpt(TokenType::SEMICOLON);

	Expr* cond = expression();
	expect(TokenType::SEMICOLON);

	Stmt* step = declOrSimpleStmtOpt(TokenType::RPAREN);

	Stmt* body = statement();

	return new ForStmt(token, init, cond, step, body);
}


Stmt* Parser::declOrSimpleStmtOpt(TokenType terminator) {
	if (accept(terminator))
		return nullptr;

	Token token = get();
	Stmt* stmt;
	if (accept(TokenType::TYPE))
		stmt = declStmt(token);
	else
		stmt = simpleStmt(token);

	expect(terminator);
	return stmt;
}


Stmt* Parser::returnStmt(Token& token) {
	Expr* expr = expression();
	return new ReturnStmt(token, expr);
}


Stmt* Parser::declStmt(Token& token) {
	Type type = toType(token);
	Token id = expect(TokenType::IDENT);

	Expr* expr = nullptr;
	if (accept(TokenType::EQL))
		expr = expression();

	return new DeclStmt(token, id.lexeme, type, expr);
}


Stmt* Parser::block(Token& token) {
	std::vector<Stmt*> stmts;

	while (!isAtEnd() && !match(TokenType::RBRACE))
		stmts.push_back(statement());

	expect(TokenType::RBRACE);
	return new BlockStmt(token, stmts);
}


Stmt* Parser::simpleStmt(Token& token) {
	Expr* lval = expression();
	Token op = get();

	bool isasnop = match(asnOps);
	bool ispostop = match(postOps);

	if (!isasnop && !ispostop)
		return new ExprStmt(token, lval);

	advance();  // is asnop or postop, so read token

	// make sure lval is an ident
	IdExpr* var = dynamic_cast<IdExpr*>(lval);
	if (var == nullptr)
		throw ParseError("Expected an identifier", token);

	if (isasnop) {
		Expr* rval = expression();
		return new AssignStmt(token, asnOpToBinOp(op.type), lval, rval);
	}

	assert(ispostop);
	return new PostOpStmt(token, postOpToBinOp(op.type), lval);
}


Expr* Parser::condition() {
	expect(TokenType::LPAREN);
	Expr* expr = expression();
	expect(TokenType::RPAREN);
	return expr;
}


ParserSpec createParserSpec() {
	ParserSpec spec;

	// null parsers
	spec.add( TokenType::LPAREN, new ParensParser());
	spec.add({TokenType::BANG, TokenType::TILDE, TokenType::SUB}, new UnaryParser());
	spec.add({TokenType::TRUE, TokenType::FALSE}, new LiteralParser());
	spec.add( TokenType::IDENT, new IdentParser());
	spec.add( TokenType::NUM, new LiteralParser());

	// left parsers
	spec.add({TokenType::MUL, TokenType::DIV, TokenType::MOD}, new BinaryParser(MUL));
	spec.add({TokenType::ADD, TokenType::SUB}, new BinaryParser(ADD));
	spec.add({TokenType::LT_LT, TokenType::GT_GT}, new BinaryParser(SHIFT));
	spec.add({TokenType::LT, TokenType::LT_EQL, TokenType::GT, TokenType::GT_EQL}, new BinaryParser(COMP));
	spec.add({TokenType::EQL_EQL, TokenType::BANG_EQL}, new BinaryParser(EQUAL));
	spec.add( TokenType::AMP, new BinaryParser(BWAND));
	spec.add( TokenType::CARET, new BinaryParser(XOR));
	spec.add( TokenType::PIPE, new BinaryParser(BWOR));
	spec.add( TokenType::AMP_AMP, new BinaryParser(LAND));
	spec.add( TokenType::PIPE_PIPE, new BinaryParser(LOR));
	spec.add( TokenType::QUESTION, new TernaryParser());

	return spec;
}
