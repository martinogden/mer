#pragma once
#include <sstream>
#include "parse-tree-visitor.hpp"
#include "parse-tree.hpp"


class TreePrinter : public Visitor {
private:
	std::stringstream buffer;
	ParseTree* tree;

	void append(std::string str);
	void open(std::string atom);
	void open();
	void close(std::string atom);
	void close();

public:
	TreePrinter(ParseTree* program);
	std::string run();

	void visit(TernaryExpr*);
	void visit(BinaryExpr*);
	void visit(UnaryExpr*);
	void visit(LiteralExpr*);
	void visit(IdExpr*);

	void visit(DeclStmt*);
	void visit(IfStmt*);
	void visit(WhileStmt*);
	void visit(ForStmt*);
	void visit(ReturnStmt*);
	void visit(BlockStmt*);
	void visit(ExprStmt*);
	void visit(AssignStmt*);
	void visit(PostOpStmt*);
};
