#include "translator/utils.hpp"
#include "translator/expr.hpp"
#include "translator/bool-expr.hpp"


ExprTranslator::ExprTranslator(Generator& gen) :
	gen(gen),
	retval(nullptr)
{}


std::string ExprTranslator::freshLabel() {
	return gen.label();
}


std::string ExprTranslator::freshTmp() {
	return gen.tmp();
}


void ExprTranslator::ret(CmdExprPtr val) {
	retval = std::move(val);
}


void ExprTranslator::ret(IRTCmdPtr cmd, IRTExprPtr e) {
	retval = std::make_unique<CmdExpr>(std::move(cmd), std::move(e));
}


CmdExprPtr ExprTranslator::get(ExprPtr expr) {
	if (expr->type == Type::BOOL)
		return transBool( std::move(expr) );

	expr->accept(*this);
	return std::move(retval);
}


void ExprTranslator::visit(CallExpr& expr) {
	std::vector<IRTCmdPtr> cmds;
	std::vector<std::string> tmps;

	for (auto& arg : expr.args) {
		CmdExprPtr e = get( std::move(arg) );
		std::string tmp = freshTmp();

		cmds.push_back( std::move(e->cmd) );
		cmds.push_back( std::make_unique<AssignCmd>(tmp, std::move(e->expr)) );
		tmps.push_back(tmp);
	}

	std::string t = freshTmp();
	cmds.push_back( std::make_unique<CallCmd>(t, expr.identifier, std::move(tmps)) );

	ret( concat( std::move(cmds) ), std::make_unique<VarExpr>(t) );
}


void ExprTranslator::visit(TernaryExpr& expr) {
	assert(expr.type == Type::INT);

	BoolExprTranslator btr(*this);

	std::string l1 = freshLabel();
	std::string l2 = freshLabel();
	std::string l3 = freshLabel();

	std::string t = freshTmp();
	CmdExprPtr then = get( std::move(expr.then) );
	CmdExprPtr otherwise = get( std::move(expr.otherwise) );

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( btr.get( std::move(expr.cond), l1, l2 ) );

	cmds.push_back( std::make_unique<LabelCmd>(l1) );
	cmds.push_back( std::move(then->cmd) );
	cmds.push_back( std::make_unique<AssignCmd>(t, std::move(then->expr)) );
	cmds.push_back( std::make_unique<GotoCmd>(l3) );

	cmds.push_back( std::make_unique<LabelCmd>(l2) );
	cmds.push_back( std::move(otherwise->cmd) );
	cmds.push_back( std::make_unique<AssignCmd>(t, std::move(otherwise->expr)) );
	cmds.push_back( std::make_unique<GotoCmd>(l3) );

	cmds.push_back( std::make_unique<LabelCmd>(l3) );

	IRTCmdPtr cmd = concat( std::move(cmds) );

	ret( std::move(cmd), std::make_unique<VarExpr>(t) );
}


void ExprTranslator::visit(BinaryExpr& expr) {
	assert(expr.type == Type::INT);

	CmdExprPtr lhs = get( std::move(expr.left) );
	CmdExprPtr rhs = get( std::move(expr.right) );
	IRTCmdPtr cmd = std::make_unique<SeqCmd>( std::move(lhs->cmd), std::move(rhs->cmd) );
	IRTExprPtr e;

	if ( isPureOp(expr.op) )
		e = std::make_unique<PairExpr>(expr.op, std::move(lhs->expr), std::move(rhs->expr));
	else {
		std::string var = freshTmp();

		std::vector<IRTCmdPtr> cmds;
		cmds.push_back( std::move(cmd) );
		cmds.push_back( std::make_unique<EffAssignCmd>(var, expr.op, std::move(lhs->expr), std::move(rhs->expr)) );

		cmd = concat( std::move(cmds) );
		e = std::make_unique<VarExpr>(var);
	}

	ret( std::move(cmd), std::move(e) );
}


void ExprTranslator::visit(UnaryExpr& unary) {
	assert(unary.type == Type::INT);
	CmdExprPtr e = get( std::move(unary.expr) );
	IRTExprPtr expr;
	IRTExprPtr zero = std::make_unique<IntExpr>(0);
	IRTExprPtr neg = std::make_unique<PairExpr>(BinOp::SUB, std::move(zero), std::move(e->expr));

	if (unary.op == UnOp::NEG)
		expr = std::move(neg);
	else if (unary.op == UnOp::BIT_NOT) {
		// ~x = -x - 1
		IRTExprPtr one = std::make_unique<IntExpr>(1);
		expr = std::make_unique<PairExpr>(BinOp::SUB, std::move(neg), std::move(one));
	}
	else
		throw 1;  // we should never get here

	ret( std::move(e->cmd), std::move(expr) );
}


void ExprTranslator::visit(LiteralExpr& expr) {
	assert(expr.type == Type::INT);
	ret( std::make_unique<NopCmd>(), std::make_unique<IntExpr>(expr.as.i) );
}


void ExprTranslator::visit(IdExpr& expr) {
	assert(expr.type == Type::INT);
	ret( std::make_unique<NopCmd>(), std::make_unique<VarExpr>(expr.identifier) );
}


CmdExprPtr ExprTranslator::transBool(ExprPtr expr) {
	BoolExprTranslator btr(*this);

	std::string l1 = freshLabel();
	std::string l2 = freshLabel();
	std::string l3 = freshLabel();
	std::string t = freshTmp();

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( btr.get(std::move(expr), l1, l2) );

	cmds.push_back( std::make_unique<LabelCmd>(l1) );
	cmds.push_back( std::make_unique<AssignCmd>(t, std::make_unique<IntExpr>(1)) );
	cmds.push_back( std::make_unique<GotoCmd>(l3) );

	cmds.push_back( std::make_unique<LabelCmd>(l2) );
	cmds.push_back( std::make_unique<AssignCmd>(t, std::make_unique<IntExpr>(0)) );
	cmds.push_back( std::make_unique<GotoCmd>(l3) );

	cmds.push_back( std::make_unique<LabelCmd>(l3) );

	IRTCmdPtr cmd = concat( std::move(cmds) );

	return std::make_unique<CmdExpr>(std::move(cmd), std::make_unique<VarExpr>(t));
}
