#pragma once


class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class VarExpr;

class DeclStmt;
class ReturnStmt;
class BlockStmt;
class AssignStmt;

class Prog;


// Implements the 'identity' visitor
// visits every node and does nothing
// implemented in visitor.cpp
class Visitor {
public:
	virtual void visit(BinaryExpr*);
	virtual void visit(UnaryExpr*);
	virtual void visit(LiteralExpr*);
	virtual void visit(VarExpr*);

	virtual void visit(DeclStmt*);
	virtual void visit(ReturnStmt*);
	virtual void visit(BlockStmt*);
	virtual void visit(AssignStmt*);

	virtual void visit(Prog*);

	virtual ~Visitor() {}
};

