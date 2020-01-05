#include <string>
#include "cst/printer.hpp"
#include "expr/printer.hpp"
#include "print-utils.hpp"


std::string CSTPrinter::get(ParseTree& tree) {
	std::vector<std::string> decls;

	for (auto& decl : tree.decls)
		decls.push_back( get(decl) );

	return join(decls, "\n");
}


void CSTPrinter::ret(std::string s) {
	retval = std::move(s);
}


std::string CSTPrinter::get(StmtPtr& stmt) {
	stmt->accept(*this);
	return retval;
}


std::string CSTPrinter::get(ExprPtr& expr) {
	ExprPrinter printer;
	return printer.get(expr);
}


std::string CSTPrinter::get(TypePtr& type) {
	TypePrinter printer;
	return printer.get(type);
}


void CSTPrinter::visit(FunDecl& decl) {
	ret("declare " + decl.identifier + " " + signature(decl));
}


void CSTPrinter::visit(FunDefn& defn) {
	ret("fun " + defn.decl->identifier + " " + signature(*defn.decl) + " " + get(defn.body) + ";");
}


void CSTPrinter::visit(StructDecl& decl) {
	ret("struct " + decl.identifier + ";");
}


void CSTPrinter::visit(StructDefn& defn) {
	std::string id = defn.decl->identifier;
	std::vector<std::string> fields;

	for (const auto& field : defn.fields)
		fields.push_back(field->identifier + ":" + get(field->type));

	ret("struct " + id + " {" + join(fields, ", ") + "};");
}


void CSTPrinter::visit(TypedefStmt& stmt) {
	ret("typedef " + get(stmt.type) + " " + stmt.alias.lexeme + ";");
}


void CSTPrinter::visit(DeclStmt& stmt) {
	std::string expr = stmt.expr ? " = " + get(stmt.expr) : "";
	ret("declare " + get(stmt.type) + " " + stmt.identifier + expr + ";");
}


void CSTPrinter::visit(IfStmt& stmt) {
	std::string ow = stmt.otherwise ? get(stmt.otherwise) : "()";
	ret("if " + get(stmt.cond) + " " + get(stmt.then) + " " + ow);
}


void CSTPrinter::visit(WhileStmt& stmt) {
	ret("while " + get(stmt.cond) + " " + get(stmt.body));
}


void CSTPrinter::visit(ForStmt& stmt) {
	std::string init = stmt.init ? get(stmt.init) : "()";
	std::string step = stmt.step ? get(stmt.step) : "()";
	ret("for " + init + " " + get(stmt.cond) + " " + step + " " + get(stmt.body));
}


void CSTPrinter::visit(ReturnStmt& stmt) {
	std::string expr = stmt.expr ? " " + get(stmt.expr) : "";
	ret("return" + expr + ";");
}


void CSTPrinter::visit(BlockStmt& block) {
	if (block.statements.empty())
		ret("[]");
	else {
		std::vector<std::string> stmts;
		for (auto& stmt : block.statements)
			stmts.push_back( get(stmt) );

		ret("[\n" + join(stmts, "\n") + "\n]");
	}
}


void CSTPrinter::visit(ExprStmt& stmt) {
	ret( get(stmt.expr) + ";" );
}


void CSTPrinter::visit(AssignStmt& stmt) {
	std::string op = stmt.op == BinOp::EQL ? "" : to_string(stmt.op) + " ";
	ret("assign " + op + get(stmt.lvalue) + " " + get(stmt.rvalue) + ";");
}


void CSTPrinter::visit(PostOpStmt& stmt) {
	std::string tag;

	switch (stmt.op) {
		case BinOp::ADD:
			tag = "incr";
			break;
		case BinOp::SUB:
			tag = "decr";
			break;
		default:
			throw 1; // should never get here
	}

	ret(tag + " " + get(stmt.expr) + ";");
}


std::string CSTPrinter::signature(FunDecl& decl) {
	std::vector<std::string> params;

	for (const auto& param : decl.params)
		params.push_back(param->identifier + ":" + get(param->type));

	return "<(" + join(params, ", ") + ") -> " + get(decl.type) + ">";
}
