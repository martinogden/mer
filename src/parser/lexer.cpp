#include <cinttypes>
#include "parser/lexer.hpp"


const int D = static_cast<int>(TokenType::ADD_EQL) - static_cast<int>(TokenType::ADD);


Lexer::Lexer(std::string src) :
	src(std::move(src)),
	start(0),
	curr(0),
	line(1),
	col(1)
{}


Token Lexer::nextToken() {
	while (true) {
		whitespace();
		begin();

		if (isAtEnd())
			return emit(TokenType::END);

		char c = advance();

		switch (c) {
			// punctuation
			case '?':
				return emit(TokenType::QUESTION);
			case ':':
				return emit(TokenType::COLON);
			case ';':
				return emit(TokenType::SEMICOLON);
			case ',':
				return emit(TokenType::COMMA);
			case '(':
				return emit(TokenType::LPAREN);
			case ')':
				return emit(TokenType::RPAREN);
			case '{':
				return emit(TokenType::LBRACE);
			case '}':
				return emit(TokenType::RBRACE);

			// operators + special cases
			case '>':
				if (accept('='))
					return emit(TokenType::GT_EQL);
				return emit(TokenType::GT);
			case '<':
				if (accept('='))
					return emit(TokenType::LT_EQL);
				return emit(TokenType::LT);

			case '!':
				if (accept('='))
					return emit(TokenType::BANG_EQL);
				return emit(TokenType::BANG);
			case '~':
				return emit(TokenType::TILDE);
			case '&':
				if (accept('&'))
					return emit(TokenType::AMP_AMP);
				if (accept('='))
					return emit(TokenType::AMP_EQL);
				return emit(TokenType::AMP);
			case '|':
				if (accept('|'))
					return emit(TokenType::PIPE_PIPE);
				if (accept('='))
					return emit(TokenType::PIPE_EQL);
				return emit(TokenType::PIPE);
			case '^':
				if (accept('='))
					return emit(TokenType::CARET_EQL);
				return emit(TokenType::CARET);
			case '/':
				if (accept('/')) {
					singleLineComment();
					break;
				}
				if (accept('*')) {
					if (multiLineComment() != 0)
						return error("nested comment error");
					break;
				}

				return op(c);
			case '-':
				if (accept('-'))
					return emit(TokenType::SUB_SUB);
				return op(c);
			case '+':
				if (accept('+'))
					return emit(TokenType::ADD_ADD);
			case '*':
			case '%':
				return op(c);
			case '=':
				if (accept('='))
					return emit(TokenType::EQL_EQL);
				return emit(TokenType::EQL);
			default:
				if (isDecDigit(c))
					return number();
				return ident();
		}
	}
}


void Lexer::begin() {
	start = curr;
}


void Lexer::incrCol() {
	col++;
}


void Lexer::incrLine() {
	line++;
	col = 0;
}


char Lexer::prev() {
	assert(curr>0 && "no previous character exists");
	return src[curr-1];
}


char Lexer::get() {
	return src[curr];
}


char Lexer::advance() {
	if (isAtEnd())
		return 0;

	incrCol();
	return src[curr++];
}


bool Lexer::accept(char c) {
	if (get() == c) {
		advance();
		return true;
	}

	return false;
}


bool Lexer::isAtEnd() {
	return curr == src.length();
}


void Lexer::whitespace() {
	while (!isAtEnd()) {
		switch (get()) {
			case '\n':  // line break
				incrLine();
			case ' ':
			case '\t':
			case '\v':
			case '\r':
			case '\f':
				break;
			default:
				return;
		}
		advance();
	}
}


void Lexer::singleLineComment() {
	while (!isAtEnd()) {
		if (accept('\n')) {
			incrLine();
			break;
		}
		advance();
	}
}


int Lexer::multiLineComment() {
	int nesting = 1;

	while (!isAtEnd() && nesting != 0) {

		if (accept('\n')) {
			incrLine();
			continue;
		}

		if (accept('/')) {
			if (isAtEnd())
				break;
			if (accept('*')) {
				nesting++;
				continue;
			}
		}

		if (accept('*')) {
			if (isAtEnd())
				break;
			if (accept('/')) {
				nesting--;
				continue;
			}
		}

		advance();
	}

	return nesting;
}


inline TokenType advType(TokenType type) {  // yuk
	return static_cast<TokenType>( static_cast<int>(type) + D );
}


Token Lexer::op(char c) {
	TokenType type;
	switch (c) {
		case '+':
			type = TokenType::ADD;
			break;
		case '-':
			type = TokenType::SUB;
			break;
		case '*':
			type = TokenType::MUL;
			break;
		case '/':
			type = TokenType::DIV;
			break;
		case '%':
			type = TokenType::MOD;
			break;
		default:
			throw 1;  // TODO: we should never get here
	}

	if (accept('='))
		type = advType(type);

	return emit(type);
}


Token Lexer::number() {
	if (accept('x') || accept('X'))
		return hex();
	return dec();
}


// c must be a hexadecimal digit [A-Fa-f0-9]
inline uint hex2int(char c) {
	if (('0' <= c) && (c <= '9'))
		return c - '0';
	if (('a' <= c) && (c <= 'f'))
		return 10 + c - 'a';
	if (('A' <= c) && (c <= 'F'))
		return 10 + c - 'A';

	throw 1;  // we should never get here
}

// [0-9a-fA-F]+
Token Lexer::hex() {
	if (!isHexDigit(get()))  // at least one
		return error("invalid hex number");

	int64_t value = hex2int(advance());

	while (! isAtEnd()) {
		char c = get();

		if (!isHexDigit(c))
			break;

		value = 16*value + hex2int(c);
		if (value > 0xFFFFFFFF)
			return error("integer value too big");
		c = advance();
	}

	// we allow 32-bit input, but operate on 64-bit regs for now
	// TODO: decide if we allow 64-bit ints or operate on 32-bit regs
	return emit(TokenType::NUM, value);
}


// c must be a decimal digit [0-9]
inline uint dec2int(char c) {
	if (('0' <= c) && (c <= '9'))
		return c - '0';

	throw 1;  // we should never get here
}


// 0 | [1-9][0-9]*
Token Lexer::dec() {
	int64_t value = dec2int(prev());

	while (! isAtEnd()) {
		char c = get();

		if (!isDecDigit(c))
			break;

		value = 10*value + dec2int(c);
		if (value > 1U<<31U)
			return error("integer value too big");
		c = advance();
	}

	return emit(TokenType::NUM, value);
}


Token Lexer::ident() {
	while (!isAtEnd()) {
		char c = get();
		if (isAlphaUnder(c) || isDecDigit(c))
			advance();
		else
			break;
	}

	std::string lexeme = getLexeme();

	if (keywords.find(lexeme) != keywords.end())
		return emit(keywords[lexeme], lexeme);

	if (types.find(lexeme) != types.end())
		return emit(TokenType::TYPE, lexeme);

	if (reserved.find(lexeme) != reserved.end())
		return emit(TokenType::RESERVED, lexeme);

	return emit(TokenType::IDENT, lexeme);
}


bool Lexer::isDecDigit(char c) {
	return ('0' <= c) && (c <= '9');
}


bool Lexer::isHexDigit(char c) {
	return (
		isDecDigit(c) ||
		(('a' <= c) && (c <= 'f')) ||
		(('A' <= c) && (c <= 'F'))
	);
}


bool Lexer::isAlpha(char c) {
	return (
		(('a' <= c) && (c <= 'z')) ||
		(('A' <= c) && (c <= 'Z'))
	);
};


bool Lexer::isAlNum(char c) {
	return isDecDigit(c) || isAlpha(c);
};


bool Lexer::isAlphaUnder(char c) {
	return isAlpha(c) || (c == '_');
}


Token Lexer::error(std::string msg) {
	return emit(TokenType::ERROR, msg, 0);
}


Token Lexer::emit(TokenType type, std::string lexeme, uint value) {
	uint length = curr - start;
	return Token(type, std::move(lexeme), value, line, col-length);
}


Token Lexer::emit(TokenType type, std::string lexeme) {
	return emit(type, std::move(lexeme), 0);
}


Token Lexer::emit(TokenType type, uint value) {
	return emit(type, getLexeme(), value);
}


Token Lexer::emit(TokenType type) {
	return emit(type, getLexeme(), 0);
}

std::string Lexer::getLexeme() {
	return src.substr(start, curr-start);
}


void Lexer::addType(std::string type) {
	types.insert(type);
}
