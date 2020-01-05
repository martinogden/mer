#pragma once


class CallExpr;
class TernaryExpr;
class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class IdExpr;
class SubscriptExpr;
class ArrowExpr;
class DotExpr;
class DerefExpr;
class AllocExpr;


class ExprVisitor {
public:
	virtual void visit(CallExpr&);
	virtual void visit(AllocExpr&);
	virtual void visit(TernaryExpr&);
	virtual void visit(BinaryExpr&);
	virtual void visit(UnaryExpr&);
	virtual void visit(LiteralExpr&);
	virtual void visit(IdExpr&);
	virtual void visit(SubscriptExpr&);
	virtual void visit(ArrowExpr&);
	virtual void visit(DotExpr&);
	virtual void visit(DerefExpr&);

	virtual ~ExprVisitor() = default;
};
