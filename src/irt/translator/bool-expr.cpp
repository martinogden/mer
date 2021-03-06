#include "irt/translator/utils.hpp"
#include "irt/translator/bool-expr.hpp"


BoolExprTranslator::BoolExprTranslator(ExprTranslator& tr) :
	tr(tr)
{}


std::string BoolExprTranslator::getTrueLabel() {
	return l1;
}


std::string BoolExprTranslator::getFalseLabel() {
	return l2;
}


void BoolExprTranslator::ret(IRTCmdPtr val) {
	retval = std::move(val);
}


void BoolExprTranslator::ret(std::vector<IRTCmdPtr> val) {
	retval = concat( std::move(val) );
}


IRTCmdPtr BoolExprTranslator::get(ExprPtr expr, std::string t, std::string f) {
	l1 = std::move(t);
	l2 = std::move(f);
	expr->accept(*this);
	assert(retval);
	return std::move(retval);
}


void BoolExprTranslator::visit(CallExpr& expr) {
//	throw 1;  // we should never get here
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();
	IRTExprPtr zero = std::make_unique<IRTIntExpr>(0);
	CmdExprPtr e = tr.get(&expr);  // HACK

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( std::move(e->cmd) );
	cmds.push_back( std::make_unique<IfCmd>(Comparison(BinOp::NOT_EQL, std::move(e->expr), std::move(zero)), t, f) );

	ret( std::move(cmds) );
}


void BoolExprTranslator::visit(AllocExpr& expr) {
	throw 1;  // we should never get here
}


void BoolExprTranslator::visit(TernaryExpr& expr) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();
	std::string lbl1 = tr.freshLabel();
	std::string lbl2 = tr.freshLabel();

	IRTCmdPtr cond = get(std::move(expr.cond), lbl1, lbl2);
	IRTCmdPtr left = get(std::move(expr.then), t, f);
	IRTCmdPtr right = get(std::move(expr.otherwise), t, f);

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( std::move(cond) );

	cmds.push_back( std::make_unique<LabelCmd>(lbl1) );
	cmds.push_back( std::move(left) );

	cmds.push_back( std::make_unique<LabelCmd>(lbl2) );
	cmds.push_back( std::move(right) );

	ret( std::move(cmds) );
}


void BoolExprTranslator::visit(BinaryExpr& expr) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();

	switch (expr.op) {
		case BinOp::LOG_AND: {
			std::string l = tr.freshLabel();
			IRTCmdPtr left = get(std::move(expr.left), l, f);
			IRTCmdPtr right = get(std::move(expr.right), t, f);

			std::vector<IRTCmdPtr> cmds;
			cmds.push_back( std::move(left) );
			cmds.push_back( std::make_unique<LabelCmd>(l) );
			cmds.push_back( std::move(right) );
			ret( std::move(cmds) );
		} break;
		case BinOp::LOG_OR: {
			std::string l = tr.freshLabel();
			IRTCmdPtr left = get(std::move(expr.left), t, l);
			IRTCmdPtr right = get(std::move(expr.right), t, f);

			std::vector<IRTCmdPtr> cmds;
			cmds.push_back( std::move(left) );
			cmds.push_back( std::make_unique<LabelCmd>(l) );
			cmds.push_back( std::move(right) );
			ret( std::move(cmds) );
		} break;
		default: {
			assert(getOpType(expr.op) == OpType::REL ||
				   getOpType(expr.op) == OpType::EQL);

			CmdExprPtr lhs = tr.get( std::move(expr.left) );
			CmdExprPtr rhs = tr.get( std::move(expr.right) );

			std::vector<IRTCmdPtr> cmds;
			cmds.push_back( std::move(lhs->cmd) );
			cmds.push_back( std::move(rhs->cmd) );
			cmds.push_back( std::make_unique<IfCmd>(Comparison(expr.op, std::move(lhs->expr), std::move(rhs->expr)), t, f) );
			ret( std::move(cmds) );
		} break;
	}
}


void BoolExprTranslator::visit(UnaryExpr& unary) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();

	assert(unary.op == UnOp::LOG_NOT);
	ret( get(std::move(unary.expr), f, t) );
}


void BoolExprTranslator::visit(LiteralExpr& expr) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();
	bool value = expr.as.b;

	ret( std::make_unique<GotoCmd>(value ? t : f) );
}


void BoolExprTranslator::visit(IdExpr& expr) {
	std::string t = getTrueLabel();
	std::string f = getFalseLabel();
	IRTExprPtr e = std::make_unique<IRTIdExpr>(expr.identifier);
	IRTExprPtr zero = std::make_unique<IRTIntExpr>(0);

	ret( std::make_unique<IfCmd>(Comparison(BinOp::NOT_EQL, std::move(e), std::move(zero)), t, f) );
}


void BoolExprTranslator::visit(SubscriptExpr& expr) {
	ret( toRValue(expr, getTrueLabel(), getFalseLabel()) );
}


void BoolExprTranslator::visit(ArrowExpr& expr) {
	throw 1;  // we should never get here
}


void BoolExprTranslator::visit(DotExpr& expr) {
	ret( toRValue(expr, getTrueLabel(), getFalseLabel()) );
}


void BoolExprTranslator::visit(DerefExpr& expr) {
	ret( toRValue(expr, getTrueLabel(), getFalseLabel()) );
}


IRTCmdPtr BoolExprTranslator::toRValue(Expr& expr, const std::string& t, const std::string& f) {
	CmdExprPtr e = tr.toRValue(expr);

	auto v = dynamic_cast<IRTIdExpr*>(e->expr.get());
	assert(v);

	IRTExprPtr zero = std::make_unique<IRTIntExpr>(0);

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( std::move(e->cmd) );
	cmds.push_back( std::make_unique<IfCmd>(Comparison(BinOp::NOT_EQL, std::move(e->expr), std::move(zero)), t, f) );
	return concat(std::move(cmds));
}
