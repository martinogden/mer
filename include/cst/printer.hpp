#pragma once
#include <sstream>
#include "visitor.hpp"
#include "cst/cst.hpp"


class CSTPrinter : public CSTVisitor {
private:
	std::stringstream buffer;
	ParseTree& tree;

	void append(const std::string& str);
	void open(const std::string& atom);
	void open();
	void close();
	void newline();
	void printFunSignature(FunDecl& decl);

public:
	CSTPrinter(ParseTree&);
	std::string run();

	void visit(CallExpr&) override;
	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;

	void visit(FunDecl&) override;
	void visit(FunDefn&) override;
	void visit(TypedefStmt&) override;
	void visit(DeclStmt&) override;
	void visit(IfStmt&) override;
	void visit(WhileStmt&) override;
	void visit(ForStmt&) override;
	void visit(ReturnStmt&) override;
	void visit(BlockStmt&) override;
	void visit(ExprStmt&) override;
	void visit(AssignStmt&) override;
	void visit(PostOpStmt&) override;
};