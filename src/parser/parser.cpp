#include <memory>
#include "parser/parser.hpp"
#include "conversion.hpp"


ParserSpec createParserSpec();
constexpr uint LHD = 0;


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
	advance();

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


StmtPtr Parser::declaration() {
	Token token = get();

	if (accept(TokenType::TYPEDEF))
		return typedefStmt(token);

	expect(TokenType::TYPE);
	return function(token);
}


StmtPtr Parser::typedefStmt(Token& token) {
	Token tyToken = expect(TokenType::TYPE);
	Token idToken = expect(TokenType::IDENT);
	// essential we forward new type to lexer here before it
	// reads the semicolon. Otherwise lexer will read in lookahead
	// token, which could be a type token of this new type
	lexer.addType(idToken.lexeme);
	expect(TokenType::SEMICOLON);
	return std::make_unique<TypedefStmt>(token, tyToken, idToken);
}


StmtPtr Parser::function(Token& token) {
	Token id = expect(TokenType::IDENT);
	FunDeclPtr decl = std::make_unique<FunDecl>(token, id.lexeme, token, params(token));

	if (accept(TokenType::SEMICOLON))
		return std::move(decl);

	Token t = expect(TokenType::LBRACE);
	return std::make_unique<FunDefn>(token, std::move(decl), block(t));
}


std::vector<DeclStmtPtr> Parser::params(Token& token) {
	std::vector<DeclStmtPtr> decls;

	expect(TokenType::LPAREN);
	while (!isAtEnd() && !match(TokenType::RPAREN)) {
		Token type = expect(TokenType::TYPE);
		Token id = expect(TokenType::IDENT);
		DeclStmtPtr decl = std::make_unique<DeclStmt>(type, id.lexeme, type, nullptr);
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
	else if (accept(TokenType::TYPE))
		stmt = declStmt(token);
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
	if (accept(TokenType::TYPE))
		stmt = declStmt(token);
	else
		stmt = simpleStmt(token);

	expect(terminator);
	return std::move(stmt);
}


StmtPtr Parser::returnStmt(Token& token) {
	ExprPtr expr = nullptr;
	if (!match(terminator))
		expr = expression();
	return std::make_unique<ReturnStmt>(token, std::move(expr));
}


StmtPtr Parser::declStmt(Token& token) {
	Token id = expect(TokenType::IDENT);

	ExprPtr expr = nullptr;
	if (accept(TokenType::EQL))
		expr = expression();

	return std::make_unique<DeclStmt>(token, id.lexeme, token, std::move(expr));
}


StmtPtr Parser::block(Token& token) {
	std::vector<StmtPtr> stmts;

	while (!isAtEnd() && !match(TokenType::RBRACE))
		stmts.push_back( statement() );

	expect(TokenType::RBRACE);
	return std::make_unique<BlockStmt>(token, std::move(stmts));
}


StmtPtr Parser::simpleStmt(Token& token) {
	ExprPtr lval = expression();
	Token op = get();

	bool isasnop = match(asnOps);
	bool ispostop = match(postOps);

	if (!isasnop && !ispostop)
		return std::make_unique<ExprStmt>(token, std::move(lval));

	advance();  // is asnop or postop, so read token

	// make sure lval is an ident
//	if ( dynamic_cast<IdExpr*>(lval.get()) == nullptr )
//		throw ParseError("Expected an identifier", token);

	if (isasnop) {
		ExprPtr rval = expression();
		return std::make_unique<AssignStmt>(token, asnOpToBinOp(op.type), std::move(lval), std::move(rval));
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


void Parser::seedTypes() {
	for (auto const& pair : concreteTypes)
		lexer.addType(pair.first);
}


ParserSpec createParserSpec() {
	ParserSpec spec;

	// null parsers
	spec.add( TokenType::LPAREN, std::make_shared<ParensParser>());
	spec.add({TokenType::BANG, TokenType::TILDE, TokenType::SUB}, std::make_shared<UnaryParser>());
	spec.add({TokenType::TRUE, TokenType::FALSE}, std::make_shared<LiteralParser>());
	spec.add( TokenType::IDENT, std::make_shared<IdentParser>());
	spec.add( TokenType::NUM, std::make_shared<LiteralParser>());

	// left parsers
	spec.add( TokenType::LPAREN, std::make_shared<CallParser>());
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

	return spec;
}
