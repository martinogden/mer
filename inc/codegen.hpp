#pragma once
#include <vector>
#include "visitor.hpp"
#include "ast.hpp"
#include "ir.hpp"
#include "symtab.hpp"


class CodeGen : public Visitor {
private:
	Prog* program;
	std::vector<Inst> insts;
	SymTab<int> env;
	Operand op;
	uint tmp;

	Operand newTmp();
	void setDst(Operand dst);
	Operand getDst();
	Operand reserve(std::string name);
	Operand lookup(std::string name);

	void emit(Inst inst);

public:
	CodeGen(Prog* program);
	std::vector<Inst>& run();

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
