#pragma once


class CallExpr;
class TernaryExpr;
class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class IdExpr;

class FunDecl;
class FunDefn;
class TypedefStmt;
class DeclStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class ReturnStmt;
class BlockStmt;
class ExprStmt;
class AssignStmt;
class PostOpStmt;


// Implements the 'identity' visitor
// visits every node and does nothing
// implemented in visitor.cpp
class Visitor {
public:
	virtual void visit(CallExpr*);
	virtual void visit(TernaryExpr*);
	virtual void visit(BinaryExpr*);
	virtual void visit(UnaryExpr*);
	virtual void visit(LiteralExpr*);
	virtual void visit(IdExpr*);

	virtual void visit(FunDecl*);
	virtual void visit(FunDefn*);
	virtual void visit(TypedefStmt*);
	virtual void visit(DeclStmt*);
	virtual void visit(IfStmt*);
	virtual void visit(WhileStmt*);
	virtual void visit(ForStmt*);
	virtual void visit(ReturnStmt*);
	virtual void visit(BlockStmt*);
	virtual void visit(ExprStmt*);
	virtual void visit(AssignStmt*);
	virtual void visit(PostOpStmt*);

	virtual ~Visitor() = default;
};
