#include <string>
#include "cst/printer.hpp"


const std::string SPACE = " ";
const std::string LPAREN = "(";
const std::string RPAREN = ")";


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


std::string to_string(bool b) {
	return b ? "true" : "false";
}


CSTPrinter::CSTPrinter(ParseTree& tree) :
	tree(tree)
{}


std::string CSTPrinter::run() {
	for (auto const& decl : tree.decls)
		decl->accept(*this);
	return buffer.str();
}


void CSTPrinter::visit(FunDecl& decl) {
	open("declare");
	printFunSignature(decl);
	close();
}


void CSTPrinter::visit(FunDefn& defn) {
	open("define");
	// TODO: get rid of this
	FunDecl* decl = defn.decl.get();
	printFunSignature(*decl);
	append(SPACE);
	defn.body->accept(*this);
	close();
}


void CSTPrinter::visit(TypedefStmt& stmt) {
	open("typedef");
	append(stmt.type.lexeme);
	append(SPACE);
	append(stmt.alias.lexeme);
	close();
}


void CSTPrinter::visit(DeclStmt& stmt) {
	open("declare");
	append(stmt.type.lexeme);
	append(SPACE);
	append(stmt.identifier);
	if (stmt.expr) {
		append(SPACE);
		stmt.expr->accept(*this);
	}
	close();
}


void CSTPrinter::visit(IfStmt& stmt) {
	open("if");
	stmt.cond->accept(*this);
	append(SPACE);
	stmt.then->accept(*this);
	append(SPACE);
	if (stmt.otherwise)
		stmt.otherwise->accept(*this);
	else
		append("()");

	close();
}


void CSTPrinter::visit(WhileStmt& stmt) {
	open("while");
	stmt.cond->accept(*this);
	append(SPACE);
	stmt.body->accept(*this);
	close();
}


void CSTPrinter::visit(ForStmt& stmt) {
	open("for");
	if (stmt.init)
		stmt.init->accept(*this);
	else
		append("()");

	append(SPACE);
	stmt.cond->accept(*this);
	append(SPACE);

	if (stmt.step)
		stmt.step->accept(*this);
	else
		append("()");

	append(SPACE);
	stmt.body->accept(*this);
	close();
}


void CSTPrinter::visit(ReturnStmt& stmt) {
	if (stmt.expr) {
		open(stmt.token.lexeme);
		stmt.expr->accept(*this);
		close();
	}
	else
		append("(return)");
}


void CSTPrinter::visit(BlockStmt& block) {
	if (block.statements.empty()) {
		append("[]");
		return;
	}

	append("[");
	newline();
	std::vector<StmtPtr>& stmts = block.statements;

	for (auto it=stmts.begin(); it != --stmts.end(); it++) {
		(*it)->accept(*this);
		newline();
	}

	(*--stmts.end())->accept(*this);
	newline();
	append("]");
}


void CSTPrinter::visit(ExprStmt& stmt) {
	stmt.expr->accept(*this);
}


void CSTPrinter::visit(AssignStmt& stmt) {
	open("assign");
	append(to_string(stmt.op));
	append(SPACE);
	stmt.lvalue->accept(*this);
	append(SPACE);
	stmt.rvalue->accept(*this);
	close();
}


void CSTPrinter::visit(PostOpStmt& stmt) {
	open();
	switch (stmt.op) {
		case BinOp::ADD:
			append("incr");
			break;
		case BinOp::SUB:
			append("decr");
			break;
		default:
			throw 1; // should never get here
	}

	append(SPACE);
	stmt.expr->accept(*this);
	close();
}


void CSTPrinter::visit(CallExpr& expr) {
	open("call");
	append(expr.identifier);
	append(SPACE);

	open();
	std::vector<ExprPtr>& args = expr.args;

	if (!expr.args.empty()) {
		for (auto it=args.begin(); it != --args.end(); ++it) {
			(*it)->accept(*this);
			append(SPACE);
		}
		(*--args.end())->accept(*this);
	}
	close();
	close();
}


void CSTPrinter::visit(TernaryExpr& expr) {
	open("?");
	expr.cond->accept(*this);
	append(SPACE);
	expr.then->accept(*this);
	append(SPACE);
	expr.otherwise->accept(*this);
	close();
}


void CSTPrinter::visit(BinaryExpr& expr) {
	open(to_string(expr.op));
	expr.left->accept(*this);
	append(SPACE);
	expr.right->accept(*this);
	close();
}


void CSTPrinter::visit(UnaryExpr& unary) {
	open("neg");
	unary.expr->accept(*this);
	close();
}


void CSTPrinter::visit(LiteralExpr& expr) {
	switch (expr.type) {
		case Type::INT:
			append(std::to_string(expr.as.i));
			break;
		case Type::BOOL:
			append(std::to_string(expr.as.b));
			break;
		default:
			throw 1;  // should never get here
	}
}


void CSTPrinter::visit(IdExpr& expr) {
	append(expr.identifier);
}


void CSTPrinter::append(const std::string& str) {
	buffer << str;
}


void CSTPrinter::open(const std::string& atom) {
	append(LPAREN + atom + SPACE);
}


void CSTPrinter::open() {
	append(LPAREN);
}


void CSTPrinter::close() {
	append(RPAREN);
}


void CSTPrinter::newline() {
	append("\n");
}


void CSTPrinter::printFunSignature(FunDecl& decl) {
	append(decl.identifier);
	append(" <");
	open();

	std::vector<DeclStmtPtr>& params = decl.params;
	if (!params.empty()) {
		for (auto it=params.begin(); it != --params.end(); ++it)
			append((*it)->identifier + ":" + (*it)->type.lexeme + ", ");
		append((*--params.end())->identifier + ":" + (*--params.end())->type.lexeme);
	}
	close();

	append(" -> ");
	append(decl.type.lexeme);
	append(">");
}
