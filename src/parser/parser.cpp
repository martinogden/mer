#include <memory>
#include "parser/parser.hpp"
#include "conversion.hpp"


constexpr uint LHD = 0;
static ParserSpec createParserSpec();


std::set<TokenType> asnOps = {
	TokenType::EQL,
	TokenType::ADD_EQL, TokenType::SUB_EQL,
	TokenType::MUL_EQL, TokenType::DIV_EQL, TokenType::MOD_EQL,
	TokenType::AMP_EQL, TokenType::PIPE_EQL, TokenType::CARET_EQL,
};


std::set<TokenType> postOps = {
	TokenType::ADD_ADD, TokenType::SUB_SUB
};


Parser::Parser(Lexer& lexer) :
	PrattParser(createParserSpec(), lexer, LHD)
{
	seedTypes();
}


ParseTree Parser::run() {
	advance();  // seek lexer
	try {
		// TODO handle w/o block
		std::vector<StmtPtr> decls = program();
		return ParseTree(std::move(decls));
	}
	catch (ParseError& e) {
		errors.add(e.what(), e.token);
	}

	return {{}};
}


std::vector<StmtPtr> Parser::program() {
	std::vector<StmtPtr> decls;
	Token token = get();

	while (!isAtEnd()) {
		try {
			decls.push_back( declaration() );
		}
		catch (ParseError& e) {
			// ignore rest of line on syntax error
			errors.add(e.what(), e.token);
			consume(e.terminator);
		}
	}

	expect(TokenType::END);

	if (errors.exist())
		return {};

	return std::move(decls);
}


TypePtr Parser::typeOpt() {
	return type(true);
}


ExprPtr Parser::expression(int rbp=0) {
	Token token = get();

	// special-case expression that contain types
	if (accept(TokenType::ALLOC)) {
		expect(TokenType::LPAREN);
		TypePtr ty = type();
		expect(TokenType::RPAREN);
		return std::make_unique<AllocExpr>(token, ty);
	}
	else if (accept(TokenType::ALLOC_ARRAY)) {
		expect(TokenType::LPAREN);
		TypePtr ty = type();
		expect(TokenType::COMMA);
		ExprPtr expr = expression();
		expect(TokenType::RPAREN);
		return std::make_unique<AllocExpr>(token, ty, std::move(expr));
	}
	else
		return PrattParser::expression(rbp);
}


TypePtr Parser::type(bool optional) {
	Token token = get();
	TypePtr ty;

	if (token.isTypeAlias && accept(TokenType::IDENT))
		ty = std::make_unique<BaseType>(token.lexeme);

	else if (accept(TokenType::TYPE))
		ty = std::make_unique<BaseType>(token.lexeme);

	else if (accept(TokenType::STRUCT)) {
		Token id = get();
		if (accept(TokenType::TYPE))
			ty = std::make_unique<StructType>(id.lexeme);
		else {
			expect(TokenType::IDENT);
			ty = std::make_unique<StructType>(id.lexeme);
		}
	}
	else {
		if (!optional)
			errors.add("Expected a type, got: `" + token.lexeme + "`.", token);
		return nullptr;
	}

	while (!isAtEnd()) {
		if (accept(TokenType::MUL))
			ty = std::make_unique<PtrType>(std::move(ty));
		else if (accept(TokenType::LBRACK)) {
			expect(TokenType::RBRACK);
			ty = std::make_unique<ArrayType>(std::move(ty));
		}
		else break;
	}

	return std::move(ty);
}


StmtPtr Parser::declaration() {
	Token token = get();

	if (accept(TokenType::TYPEDEF))
		return typedefStmt(token);

	TypePtr ty = type();
	Token t = get();

	if (accept(TokenType::IDENT))
		return finishFun(token, t, ty);

	if ( auto stype = dynamic_cast<StructType*>(ty.get()) )
		return finishStruct(token, stype->name);

	throw ParseError("Unexpected token: `" + t.lexeme + "`.", t, TokenType::RBRACE);
}


StmtPtr Parser::typedefStmt(Token& token) {
	TypePtr ty = type();
	Token idToken = expect(TokenType::IDENT);

	// essential we forward new type alias to lexer here before it
	// reads the semicolon. Otherwise lexer will read in lookahead
	// token, which could be a type token of this new type
	lexer.addTypeAlias(idToken.lexeme);

	expect(TokenType::SEMICOLON);
	return std::make_unique<TypedefStmt>(token, std::move(ty), idToken);
}


StmtPtr Parser::finishStruct(Token& token, std::string& name) {
	StructDeclPtr decl = std::make_unique<StructDecl>(token, name);
	StmtPtr stmt;

	if (accept(TokenType::LBRACE))
		stmt = std::make_unique<StructDefn>(token, std::move(decl), fields());
	else
		stmt = std::move(decl);

	expect(TokenType::SEMICOLON);
	return std::move(stmt);
}


std::vector<DeclStmtPtr> Parser::fields() {
	std::vector<DeclStmtPtr> decls;

	while (!isAtEnd() && !match(TokenType::RBRACE)) {
		Token t = get();
		TypePtr ty = type();
		Token id = expect(TokenType::IDENT);
		expect(TokenType::SEMICOLON);
		DeclStmtPtr decl = std::make_unique<DeclStmt>(t, id.lexeme, std::move(ty), nullptr);
		decls.push_back( std::move(decl) );
	}

	expect(TokenType::RBRACE);
	return std::move(decls);
}


StmtPtr Parser::finishFun(Token& token, Token& idToken, TypePtr ty) {
	FunDeclPtr decl = std::make_unique<FunDecl>(token, idToken.lexeme, std::move(ty), params(idToken));

	if (accept(TokenType::SEMICOLON))
		return std::move(decl);

	Token t = expect(TokenType::LBRACE);
	return std::make_unique<FunDefn>(token, std::move(decl), block(t));
}


std::vector<DeclStmtPtr> Parser::params(Token& token) {
	std::vector<DeclStmtPtr> decls;

	expect(TokenType::LPAREN);
	while (!isAtEnd() && !match(TokenType::RPAREN)) {
		Token t = get();
		TypePtr ty = type();
		Token id = expect(TokenType::IDENT);
		DeclStmtPtr decl = std::make_unique<DeclStmt>(t, id.lexeme, std::move(ty), nullptr);
		decls.push_back( std::move(decl) );

		if (!accept(TokenType::COMMA))
			break;
	}
	expect(TokenType::RPAREN);

	// TODO: remove this restriction once backend (x86) can handle it
	if (decls.size() > 6)
		errors.add("function has too many parameters.", token);

	return std::move(decls);
}


StmtPtr Parser::statement() {
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
	StmtPtr stmt;
	if (accept(TokenType::RETURN))
		stmt = returnStmt(token);
	else if (TypePtr type = typeOpt())
		stmt = finishDeclStmt(token, std::move(type));
	else
		stmt = simpleStmt(token);

	expect(TokenType::SEMICOLON);
	return std::move(stmt);
}


StmtPtr Parser::ifStmt(Token& token) {
	ExprPtr cond = condition();
	StmtPtr then = statement();
	StmtPtr otherwise;

	if (accept(TokenType::ELSE))
		otherwise = statement();

	return std::make_unique<IfStmt>(token, std::move(cond), std::move(then), std::move(otherwise));
}


StmtPtr Parser::whileStmt(Token& token) {
	ExprPtr cond = condition();
	StmtPtr body = statement();
	return std::make_unique<WhileStmt>(token, std::move(cond), std::move(body));
}


StmtPtr Parser::forStmt(Token& token) {
	expect(TokenType::LPAREN);

	StmtPtr init = declOrSimpleStmtOpt(TokenType::SEMICOLON);

	ExprPtr cond = expression();
	expect(TokenType::SEMICOLON);

	StmtPtr step = declOrSimpleStmtOpt(TokenType::RPAREN);

	StmtPtr body = statement();

	return std::make_unique<ForStmt>(token, std::move(init), std::move(cond), std::move(step), std::move(body));
}


StmtPtr Parser::declOrSimpleStmtOpt(TokenType terminator) {
	if (accept(terminator))
		return nullptr;

	Token token = get();
	StmtPtr stmt;
	if (TypePtr type = typeOpt())
		stmt = finishDeclStmt(token, std::move(type));
	else
		stmt = simpleStmt(token);

	expect(terminator);
	return std::move(stmt);
}


StmtPtr Parser::returnStmt(Token& token) {
	ExprPtr expr = nullptr;
	if (!match(TokenType::SEMICOLON))
		expr = expression();
	return std::make_unique<ReturnStmt>(token, std::move(expr));
}


StmtPtr Parser::finishDeclStmt(Token& token, TypePtr ty) {
	assert(ty != nullptr);

	Token id = expect(TokenType::IDENT);
	Token t = get();

	if (accept(TokenType::EQL))
		return std::make_unique<DeclStmt>(t, id.lexeme, std::move(ty), expression());
	else
		return std::make_unique<DeclStmt>(token, id.lexeme, std::move(ty), nullptr);
}


StmtPtr Parser::block(Token& token) {
	std::vector<StmtPtr> stmts;

	while (!isAtEnd() && !match(TokenType::RBRACE))
		stmts.push_back( statement() );

	expect(TokenType::RBRACE);
	return std::make_unique<BlockStmt>(token, std::move(stmts));
}


StmtPtr Parser::simpleStmt(Token& token) {
	ExprPtr lval = lvalue();
	Token op = get();

	bool isasnop = match(asnOps);
	bool ispostop = match(postOps);

	if (!isasnop && !ispostop)
		return std::make_unique<ExprStmt>(token, std::move(lval));

	Token t = advance();  // is asnop or postop, so read token

	if (isasnop) {
		ExprPtr rval = expression();
		return std::make_unique<AssignStmt>(t, asnOpToBinOp(op.type), std::move(lval), std::move(rval));
	}

	assert(ispostop);
	return std::make_unique<PostOpStmt>(token, postOpToBinOp(op.type), std::move(lval));
}


ExprPtr Parser::condition() {
	expect(TokenType::LPAREN);
	ExprPtr expr = expression();
	expect(TokenType::RPAREN);
	return std::move(expr);
}


ExprPtr Parser::lvalue() {
	// TODO: is this the best way to handle lvalues?
	// Doing it this way, we need to check for valid lvalues in elab
	return expression();
}


void Parser::seedTypes() {
	for (const auto& pair : Type::baseTypes)
		lexer.addType(pair.first);
}


ParserSpec createParserSpec() {
	ParserSpec spec;

	// null parsers
	spec.add( TokenType::LPAREN, std::make_shared<ParensParser>());
	spec.add({TokenType::BANG, TokenType::TILDE, TokenType::SUB}, std::make_shared<UnaryParser>());
	spec.add({TokenType::NUM, TokenType::TRUE, TokenType::FALSE, TokenType::NUL}, std::make_shared<LiteralParser>());
	spec.add( TokenType::IDENT, std::make_shared<IdentParser>());
	spec.add( TokenType::MUL, std::make_shared<DerefParser>());

	// left parsers
	spec.add( TokenType::LPAREN, std::make_shared<CallParser>());
	spec.add( TokenType::LBRACK, std::make_shared<SubscriptParser>());
	spec.add( TokenType::SUB_GT, std::make_shared<ArrowParser>());
	spec.add( TokenType::FULL_STOP, std::make_shared<DotParser>());
	spec.add({TokenType::MUL, TokenType::DIV, TokenType::MOD}, std::make_shared<BinaryParser>(MUL));
	spec.add({TokenType::ADD, TokenType::SUB}, std::make_shared<BinaryParser>(ADD));
	spec.add({TokenType::LT, TokenType::LT_EQL, TokenType::GT, TokenType::GT_EQL}, std::make_shared<BinaryParser>(COMP));
	spec.add({TokenType::EQL_EQL, TokenType::BANG_EQL}, std::make_shared<BinaryParser>(EQUAL));
	spec.add( TokenType::AMP, std::make_shared<BinaryParser>(BWAND));
	spec.add( TokenType::CARET, std::make_shared<BinaryParser>(XOR));
	spec.add( TokenType::PIPE, std::make_shared<BinaryParser>(BWOR));
	spec.add( TokenType::AMP_AMP, std::make_shared<BinaryParser>(LAND));
	spec.add( TokenType::PIPE_PIPE, std::make_shared<BinaryParser>(LOR));
	spec.add( TokenType::QUESTION, std::make_shared<TernaryParser>());

	return std::move(spec);
}
