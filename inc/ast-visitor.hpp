#pragma once


class FunNode;
class AssignNode;
class IfNode;
class WhileNode;
class ReturnNode;
class NopNode;
class SeqNode;
class DeclNode;
class ExprNode;


class ASTVisitor {
public:
	virtual void visit(FunNode*);
	virtual void visit(AssignNode*);
	virtual void visit(IfNode*);
	virtual void visit(WhileNode*);
	virtual void visit(ReturnNode*);
	virtual void visit(NopNode*);
	virtual void visit(SeqNode*);
	virtual void visit(DeclNode*);
	virtual void visit(ExprNode*);

	virtual ~ASTVisitor() {};
};
