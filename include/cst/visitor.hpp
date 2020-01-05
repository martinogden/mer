#pragma once


class FunDecl;
class FunDefn;
class StructDecl;
class StructDefn;
class DeclStmt;
class TypedefStmt;
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
class CSTVisitor {
public:
	virtual void visit(FunDecl&);
	virtual void visit(FunDefn&);
	virtual void visit(StructDecl&);
	virtual void visit(StructDefn&);
	virtual void visit(DeclStmt&);
	virtual void visit(TypedefStmt&);
	virtual void visit(IfStmt&);
	virtual void visit(WhileStmt&);
	virtual void visit(ForStmt&);
	virtual void visit(ReturnStmt&);
	virtual void visit(BlockStmt&);
	virtual void visit(ExprStmt&);
	virtual void visit(AssignStmt&);
	virtual void visit(PostOpStmt&);

	virtual ~CSTVisitor() = default;
};
