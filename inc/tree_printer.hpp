#pragma once
#include <sstream>
#include "visitor.hpp"
#include "ast.hpp"


class TreePrinter : public Visitor {
private:
	std::stringstream buffer;
	Prog* program;

	void append(std::string str);
	void open(std::string atom);
	void open();
	void close(std::string atom);
	void close();

public:
	TreePrinter(Prog* program);
	std::string run();

	void visit(BinaryExpr*);
	void visit(UnaryExpr*);
	void visit(LiteralExpr*);
	void visit(VarExpr*);

	void visit(DeclStmt*);
	void visit(ReturnStmt*);
	void visit(BlockStmt*);
	void visit(AssignStmt*);

	void visit(Prog*);
};
