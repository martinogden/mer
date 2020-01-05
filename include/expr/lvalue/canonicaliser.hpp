#pragma once
#include "map.hpp"
#include "generator.hpp"
#include "expr/visitor.hpp"
#include "expr/expr.hpp"
#include "cst/cst.hpp"


class LValueCanon : public ExprVisitor {
private:
	ExprPtr& root;
	Map<FunTypePtr>& funTypes;
	Generator& gen;

	std::vector<StmtPtr> stmts;
	ExprPtr retval;

	void visit(CallExpr&) override;
	void visit(TernaryExpr&) override;
	void visit(BinaryExpr&) override;
	void visit(UnaryExpr&) override;
	void visit(LiteralExpr&) override;
	void visit(IdExpr&) override;
	void visit(SubscriptExpr&) override;
	void visit(ArrowExpr&) override;
	void visit(DotExpr&) override;
	void visit(DerefExpr&) override;
	void visit(AllocExpr&) override;

	void emit(StmtPtr);
	void ret(ExprPtr);
	ExprPtr get(ExprPtr&);

public:
	LValueCanon(ExprPtr&, Map<FunTypePtr>& funTypes, Generator& gen);

	std::pair<std::vector<StmtPtr>, ExprPtr> run();
};
