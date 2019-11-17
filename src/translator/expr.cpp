#include "translator/utils.hpp"
#include "translator/expr.hpp"
#include "translator/bool-expr.hpp"


ExprTranslator::ExprTranslator(Generator& gen) :
	gen(gen)
{}


std::string ExprTranslator::freshLabel() {
	return gen.label();
}


std::string ExprTranslator::freshTmp() {
	return gen.tmp();
}


void ExprTranslator::ret(CmdExpr* val) {
	retval = val;
}


void ExprTranslator::ret(IRTCmd* cmd, IRTExpr* e) {
	retval = new CmdExpr(cmd, e);
}


CmdExpr* ExprTranslator::get(Expr* expr) {
	expr->accept(*this);
	return retval;
}


void ExprTranslator::visit(TernaryExpr* expr) {
	if (expr->type == Type::BOOL) {
		ret( transBool(expr) );
		return;
	}

	BoolExprTranslator btr(*this);

	std::string l1 = freshLabel();
	std::string l2 = freshLabel();
	std::string l3 = freshLabel();

	std::string t = freshTmp();
	CmdExpr* then = get(expr->then);
	CmdExpr* otherwise = get(expr->otherwise);

	IRTCmd* cmd = concat({
		btr.get(expr, l1, l2),
		new LabelCmd(l1), then->cmd, new AssignCmd(t, then->expr), new GotoCmd(l3),
		new LabelCmd(l2), otherwise->cmd, new AssignCmd(t, otherwise->expr), new GotoCmd(l3),
		new LabelCmd(l3)
	});

	ret(cmd, new VarExpr(t));
}


void ExprTranslator::visit(BinaryExpr* expr) {
	if (expr->type == Type::BOOL) {
		ret( transBool(expr) );
		return;
	}

	assert(expr->type == Type::INT);

	CmdExpr* lhs = get(expr->left);
	CmdExpr* rhs = get(expr->right);
	IRTCmd* cmd = concat({ lhs->cmd, rhs->cmd });
	IRTExpr* e;

	if ( isPureOp(expr->op) )
		e = new PairExpr(expr->op, lhs->expr, rhs->expr);
	else {
		std::string var = freshTmp();
		cmd = concat({ cmd, new EffAssignCmd(var, expr->op, lhs->expr, rhs->expr) });
		e = new VarExpr(var);
	}

	ret(cmd, e);
}


void ExprTranslator::visit(UnaryExpr* unary) {
	if (unary->type == Type::BOOL) {
		ret( transBool(unary) );
		return;
	}

	assert(unary->type == Type::INT);
	CmdExpr* e = get(unary->expr);
	IRTExpr* expr;

	if (unary->op == UnOp::NEG)
		expr = new PairExpr(BinOp::SUB, new IntExpr(0), e->expr);
	else if (unary->op == UnOp::BIT_NOT)
		expr = new PairExpr(BinOp::XOR, new IntExpr(INT32_MAX), e->expr);
	else
		throw 1;  // we should never get here

	ret(e->cmd, expr);
}


void ExprTranslator::visit(LiteralExpr* expr) {
	if (expr->type == Type::BOOL) {
		ret( transBool(expr) );
		return;
	}

	assert(expr->type == Type::INT);
	ret(new NopCmd(), new IntExpr(expr->as.i));
}


void ExprTranslator::visit(IdExpr* expr) {
	if (expr->type == Type::BOOL) {
		ret( transBool(expr) );
		return;
	}

	assert(expr->type == Type::INT);
	ret(new NopCmd(), new VarExpr(expr->identifier));
}


CmdExpr* ExprTranslator::transBool(Expr* expr) {
	BoolExprTranslator btr(*this);

	std::string l1 = freshLabel();
	std::string l2 = freshLabel();
	std::string l3 = freshLabel();
	std::string t = freshTmp();

	IRTCmd* cmd = concat({
		btr.get(expr, l1, l2),
		new LabelCmd(l1), new AssignCmd(t, new IntExpr(1)), new GotoCmd(l3),
		new LabelCmd(l2), new AssignCmd(t, new IntExpr(0)), new GotoCmd(l3),
		new LabelCmd(l3)
	});

	return new CmdExpr(cmd, new VarExpr(t));
}
