#pragma once
#include <sstream>
#include "visitor.hpp"
#include "cst/cst.hpp"
#include "type/printer.hpp"


class CSTPrinter : public CSTVisitor {
private:
	std::string retval;

	void ret(std::string);
	std::string get(StmtPtr&);
	std::string get(ExprPtr&);
	std::string get(TypePtr&);
	std::string signature(FunDecl&);

	void visit(FunDecl&) override;
	void visit(FunDefn&) override;
	void visit(StructDecl&) override;
	void visit(StructDefn&) override;
	void visit(DeclStmt&) override;
	void visit(TypedefStmt&) override;
	void visit(IfStmt&) override;
	void visit(WhileStmt&) override;
	void visit(ForStmt&) override;
	void visit(ReturnStmt&) override;
	void visit(BlockStmt&) override;
	void visit(ExprStmt&) override;
	void visit(AssignStmt&) override;
	void visit(PostOpStmt&) override;

public:
	std::string get(ParseTree&);
};
