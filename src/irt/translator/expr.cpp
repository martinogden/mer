#include "type/comparison.hpp"
#include "irt/size-analyser.hpp"
#include "irt/translator/utils.hpp"
#include "irt/translator/expr.hpp"
#include "irt/translator/bool-expr.hpp"
#include "irt/translator/lvalue.hpp"


ExprTranslator::ExprTranslator(Generator& gen, Map<IRTStruct>& structs) :
	gen(gen),
	structs(structs),
	retval(nullptr)
{}


std::string ExprTranslator::freshLabel() {
	return gen.label();
}


std::string ExprTranslator::freshTmp() {
	return gen.tmp();
}


IRTStruct& ExprTranslator::getIRTStruct(const std::string& name) {
	return structs.lookup(name);
}


void ExprTranslator::ret(CmdExprPtr val) {
	retval = std::move(val);
}


void ExprTranslator::ret(IRTCmdPtr cmd, IRTExprPtr e) {
	retval = std::make_unique<CmdExpr>(std::move(cmd), std::move(e));
}


CmdExprPtr ExprTranslator::get(Expr* expr) {
	expr->accept(*this);
	return std::move(retval);
}


CmdExprPtr ExprTranslator::get(ExprPtr expr) {
	if (eq(expr->type, Type::BOOL))
		return transBool( std::move(expr) );

	expr->accept(*this);
	return std::move(retval);
}


CmdExprPtr ExprTranslator::getAsLValue(ExprPtr expr) {
	LValueTranslator lvt(*this);
	return lvt.get(expr.get());
}


void ExprTranslator::visit(CallExpr& expr) {
	std::vector<IRTCmdPtr> cmds;
	std::vector<std::string> args;

	for (auto& arg : expr.args) {
		CmdExprPtr e = get( std::move(arg) );
		std::string tmp = freshTmp();

		cmds.push_back( std::move(e->cmd) );
		cmds.push_back( std::make_unique<AssignCmd>(tmp, std::move(e->expr)) );
		args.push_back(tmp);
	}

	std::string t = freshTmp();
	cmds.push_back( std::make_unique<CallCmd>(t, expr.identifier, std::move(args)) );

	ret( concat( std::move(cmds) ), std::make_unique<IRTIdExpr>(t) );
}


void ExprTranslator::visit(AllocExpr& expr) {
	std::string tmp = freshTmp();
	std::string param1 = freshTmp();

	uint sz;
	if ( auto t = dynamic_cast<StructType*>(expr.typeParam.get()) )
		sz = structs.lookup(t->name).size;
	else {
		TypeSizeAnalyser tsa;
		sz = tsa.get(expr.type);
	}

	IRTExprPtr size = std::make_unique<IRTIntExpr>(sz);
	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( std::make_unique<AssignCmd>(param1, std::move(size)) );
	std::vector<std::string> args;
	std::string label;

	if (expr.expr) {
		std::string param2 = freshTmp();
		CmdExprPtr e = get( std::move(expr.expr) );

		cmds.push_back( std::move(e->cmd) );
		cmds.push_back( std::make_unique<AssignCmd>(param2, std::move(e->expr)) );

		args = {param1, param2};
		label = "alloc_array";
	}
	else {
		args = {param1};
		label = "alloc";
	}

	cmds.push_back( std::make_unique<CallCmd>(tmp, "_c0_" + label, std::move(args)) );
	ret( concat(std::move(cmds)), std::make_unique<IRTIdExpr>(tmp) );
}


void ExprTranslator::visit(TernaryExpr& expr) {
	assert(eq(expr.type, Type::INT));

	BoolExprTranslator btr(*this);

	std::string l1 = freshLabel();
	std::string l2 = freshLabel();
	std::string l3 = freshLabel();

	CmdExprPtr then = get( std::move(expr.then) );
	CmdExprPtr otherwise = get( std::move(expr.otherwise) );

	std::string t = freshTmp();

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

	ret( std::move(cmd), std::make_unique<IRTIdExpr>(t) );
}


void ExprTranslator::visit(BinaryExpr& expr) {
	assert(eq(expr.type, Type::INT));

	CmdExprPtr lhs = get( std::move(expr.left) );
	CmdExprPtr rhs = get( std::move(expr.right) );
	IRTCmdPtr left = std::move(lhs->cmd);
	IRTCmdPtr right = std::move(rhs->cmd);
	assert(left && right);

	IRTCmdPtr cmd = std::make_unique<SeqCmd>( std::move(left), std::move(right) );
	IRTExprPtr e;

	if ( isPureOp(expr.op) )
		e = std::make_unique<IRTBinaryExpr>(expr.op, std::move(lhs->expr), std::move(rhs->expr));
	else {
		std::string t = freshTmp();

		std::vector<IRTCmdPtr> cmds;
		cmds.push_back( std::move(cmd) );
		cmds.push_back( std::make_unique<EffAssignCmd>(t, expr.op, std::move(lhs->expr), std::move(rhs->expr)) );

		cmd = concat( std::move(cmds) );
		e = std::make_unique<IRTIdExpr>(t);
	}

	ret( std::move(cmd), std::move(e) );
}


void ExprTranslator::visit(UnaryExpr& unary) {
	assert(eq(unary.type, Type::INT));
	CmdExprPtr e = get( std::move(unary.expr) );
	IRTExprPtr expr;
	IRTExprPtr zero = std::make_unique<IRTIntExpr>(0);
	IRTExprPtr neg = std::make_unique<IRTBinaryExpr>(BinOp::SUB, std::move(zero), std::move(e->expr));

	if (unary.op == UnOp::NEG)
		expr = std::move(neg);
	else if (unary.op == UnOp::BIT_NOT) {
		// ~x = -x - 1
		IRTExprPtr one = std::make_unique<IRTIntExpr>(1);
		expr = std::make_unique<IRTBinaryExpr>(BinOp::SUB, std::move(neg), std::move(one));
	}
	else
		throw 1;  // we should never get here

	ret( std::move(e->cmd), std::move(expr) );
}


void ExprTranslator::visit(LiteralExpr& expr) {
	assert(eq(expr.type, Type::INT) || eq(expr.type, Type::INDEF));
	int64_t val = expr.as.i;
	if (val >= INT32_MAX) {
		std::string t = freshTmp();
		ret( std::make_unique<AssignCmd>(t, std::make_unique<IRTIntExpr>(val)), std::make_unique<IRTIdExpr>(t) );
	}
	else
		ret( std::make_unique<NopCmd>(), std::make_unique<IRTIntExpr>(val) );
}


void ExprTranslator::visit(IdExpr& expr) {
	ret( std::make_unique<NopCmd>(), std::make_unique<IRTIdExpr>(expr.identifier) );
}


void ExprTranslator::visit(SubscriptExpr& expr) {
	ret( toRValue(expr) );
}


void ExprTranslator::visit(ArrowExpr& expr) {
	throw 1;  // we should never get here
}


void ExprTranslator::visit(DotExpr& expr) {
	ret( toRValue(expr) );
}


void ExprTranslator::visit(DerefExpr& expr) {
	ret( toRValue(expr) );
}


CmdExprPtr ExprTranslator::toRValue(Expr& expr) {
	LValueTranslator lvt(*this);
	CmdExprPtr e = lvt.get(&expr);

	if ( dynamic_cast<IRTMemExpr*>(e->expr.get()) ) {
		std::string addr = freshTmp();
		std::string tmp = freshTmp();

		std::vector<IRTCmdPtr> cmds;
		cmds.push_back( std::move(e->cmd) );
		cmds.push_back( std::make_unique<LoadCmd>(tmp, std::move(e->expr)) );

		return std::make_unique<CmdExpr>( concat(std::move(cmds)), std::make_unique<IRTIdExpr>(tmp) );
	}
	else throw 1;  // we should never get here
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
	cmds.push_back( std::make_unique<AssignCmd>(t, std::make_unique<IRTIntExpr>(1)) );
	cmds.push_back( std::make_unique<GotoCmd>(l3) );

	cmds.push_back( std::make_unique<LabelCmd>(l2) );
	cmds.push_back( std::make_unique<AssignCmd>(t, std::make_unique<IRTIntExpr>(0)) );
	cmds.push_back( std::make_unique<GotoCmd>(l3) );

	cmds.push_back( std::make_unique<LabelCmd>(l3) );

	IRTCmdPtr cmd = concat( std::move(cmds) );

	return std::make_unique<CmdExpr>(std::move(cmd), std::make_unique<IRTIdExpr>(t));
}
