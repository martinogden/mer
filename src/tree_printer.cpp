#include <string>
#include "tree_printer.hpp"


const std::string BLANK = "";
const std::string SPACE = " ";
const std::string NEWLINE = "\n";
const std::string LPAREN = "(";
const std::string RPAREN = ")";
const std::string LET = "let";


std::string to_string(UnOp op) {
	std::stringstream buffer;
	buffer << op;
	return buffer.str();
}


std::string to_string(BinOp op) {
	std::stringstream buffer;
	buffer << op;
	return buffer.str();
}


std::string to_string(AsnOp op) {
	std::stringstream buffer;
	buffer << op;
	return buffer.str();
}


TreePrinter::TreePrinter(Prog* program) :
	program(program)
{}


std::string TreePrinter::run() {
	program->accept(*this);
	return buffer.str();
}


void TreePrinter::visit(Prog* program) {
	std::string sep = BLANK;
	for (auto stmt : program->statements) {
		append(sep);
		stmt->accept(*this);
		sep = NEWLINE;
	}
}


void TreePrinter::visit(DeclStmt* stmt) {
	open(stmt->token.lexeme);
	append(stmt->identifier);
	if (stmt->expr) {
		append(SPACE);
		stmt->expr->accept(*this);
	}
	close();
}


void TreePrinter::visit(ReturnStmt* stmt) {
	open(stmt->token.lexeme);
	stmt->expr->accept(*this);
	close();
}


void TreePrinter::visit(BlockStmt* block) {
	if(block->statements.size() == 1)
		block->statements[0]->accept(*this);
	else {
		open();
		for (auto stmt : block->statements)
			stmt->accept(*this);
		close();
	}
}


void TreePrinter::visit(AssignStmt* stmt) {
	open(to_string(stmt->op));
	append(stmt->lvalue + SPACE);
	stmt->rvalue->accept(*this);
	close();
}


void TreePrinter::visit(BinaryExpr* expr) {
	open(to_string(expr->op));
	expr->left->accept(*this);
	append(SPACE);
	expr->right->accept(*this);
	close();
}


void TreePrinter::visit(UnaryExpr* unary) {
	open("neg");
	unary->expr->accept(*this);
	close();
}


void TreePrinter::visit(LiteralExpr* expr) {
	append(std::to_string(expr->value));
}


void TreePrinter::visit(VarExpr* expr) {
	append(expr->identifier);
}


void TreePrinter::append(std::string str) {
	buffer << str;
}


void TreePrinter::open(std::string atom) {
	append(LPAREN + atom + SPACE);
}


void TreePrinter::open() {
	append(LPAREN);
}


void TreePrinter::close() {
	append(RPAREN);
}

