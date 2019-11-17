#include "translator/utils.hpp"
#include "translator/bool-expr.hpp"


BoolExprTranslator::BoolExprTranslator(ExprTranslator& tr) :
	tr(tr)
{}


std::string BoolExprTranslator::getTrueLabel() {
	return l1;
}


std::string BoolExprTranslator::getFalseLabel() {
	return l2;
}


void BoolExprTranslator::ret(IRTCmd* val) {
	retval = val;
}


void BoolExprTranslator::ret(std::vector<IRTCmd*> val) {
	retval = concat(val);
}


IRTCmd* BoolExprTranslator::get(Expr* expr, std::string t, std::string f) {
	l1 = t;
	l2 = f;
	expr->accept(*this);
	return retval;
}


void BoolExprTranslator::visit(TernaryExpr* expr) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();
	ret( get(expr->cond, t, f) );
}


void BoolExprTranslator::visit(BinaryExpr* expr) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();

	switch (expr->op) {
		case BinOp::LOG_AND: {
			std::string l = tr.freshLabel();
			IRTCmd* left = get(expr->left, l, f);
			IRTCmd* right = get(expr->right, t, f);

			ret({ left, new LabelCmd(l), right });
		} break;
		case BinOp::LOG_OR: {
			std::string l = tr.freshLabel();
			IRTCmd* left = get(expr->left, t, l);
			IRTCmd* right = get(expr->right, t, f);

			ret({ left, new LabelCmd(l), right });
		} break;
		default: {
			assert(getOpType(expr->op) == OpType::REL ||
				   getOpType(expr->op) == OpType::EQL);

			CmdExpr* lhs = tr.get(expr->left);
			CmdExpr* rhs = tr.get(expr->right);

			ret({
				lhs->cmd, rhs->cmd,
				new IfCmd({expr->op, lhs->expr, rhs->expr}, t, f)
			});
		} break;
	}
}


void BoolExprTranslator::visit(UnaryExpr* unary) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();

	assert(unary->op == UnOp::NEG);
	ret( get(unary->expr, f, t) );
}


void BoolExprTranslator::visit(LiteralExpr* expr) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();
	bool value = expr->as.b;

	ret( new GotoCmd(value ? t : f) );
}


void BoolExprTranslator::visit(IdExpr* expr) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();
	CmdExpr* e = tr.get(expr);

	ret({
		e->cmd,
		new IfCmd({BinOp::NOT_EQL, e->expr, new IntExpr(0)}, t, f)
	});
}
