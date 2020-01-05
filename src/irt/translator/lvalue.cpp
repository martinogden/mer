#include "irt/translator/lvalue.hpp"
#include "irt/translator/utils.hpp"
#include "irt/size-analyser.hpp"


LValueTranslator::LValueTranslator(ExprTranslator& tr) :
	tr(tr)
{}


void LValueTranslator::ret(IRTCmdPtr cmd, IRTExprPtr e) {
	retval = std::make_unique<CmdExpr>(std::move(cmd), std::move(e));
}


CmdExprPtr LValueTranslator::get(Expr* expr) {
	expr->accept(*this);
	return std::move(retval);
}


void LValueTranslator::visit(SubscriptExpr& expr) {
	TypeSizeAnalyser tsa;

	auto type = dynamic_cast<ArrayType*>(expr.left->type.get());
	assert(type);

	CmdExprPtr left = tr.getAsLValue( std::move(expr.left) );
	CmdExprPtr right = tr.get( std::move(expr.right) );

	std::string base = tr.freshTmp();
	std::string index = tr.freshTmp();
	std::vector<std::string> args = {base, index};

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( std::move(left->cmd) );
	cmds.push_back( std::move(right->cmd) );
	cmds.push_back( std::make_unique<AssignCmd>(base, std::move(left->expr)) );
	cmds.push_back( std::make_unique<AssignCmd>(index, std::move(right->expr)) );
	cmds.push_back( std::make_unique<CallCmd>(index, "_c0_bounds_check", std::move(args)) );

	IRTExprPtr width = std::make_unique<IRTIntExpr>( tsa.get(expr.type) );
	IRTExprPtr offset = std::make_unique<IRTBinaryExpr>(BinOp::MUL, std::make_unique<IRTIdExpr>(index), std::move(width));
	IRTExprPtr mem = std::make_unique<IRTBinaryExpr>(BinOp::ADD, std::make_unique<IRTIdExpr>(base), std::move(offset));

	ret( concat(std::move(cmds)), std::make_unique<IRTMemExpr>(std::move(mem)) );
}


void LValueTranslator::visit(ArrowExpr& expr) {
	throw 1;  // we should never get here
}


void LValueTranslator::visit(DotExpr& expr) {
	// TODO: tidy up
	if ( auto left = dynamic_cast<DerefExpr*>(expr.expr.get()) ) {
		auto stype = dynamic_cast<StructType*>(left->type.get());
		assert(stype);

		// expr has form (*p).f
		IRTStruct& s = tr.getIRTStruct(stype->name);
		int offset = s.offsets.lookup(expr.identifier);
		CmdExprPtr e = tr.get(std::move(left->expr));

		IRTExprPtr m;
		if ( auto mem = dynamic_cast<IRTMemExpr*>(e->expr.get()) ) {
			m = std::move(mem->address);
			offset += mem->offset;
		}
		else m = std::move(e->expr);

		ret(std::move(e->cmd), std::make_unique<IRTMemExpr>(std::move(m), offset));
	}
	else if ( auto left = dynamic_cast<SubscriptExpr*>(expr.expr.get()) ) {
		auto stype = dynamic_cast<StructType*>(left->type.get());
		assert(stype);

		// expr has form A[i].f
		IRTStruct& s = tr.getIRTStruct(stype->name);
		int offset = s.offsets.lookup(expr.identifier);
		CmdExprPtr e = tr.getAsLValue(std::move(expr.expr));

		IRTExprPtr m;
		if ( auto mem = dynamic_cast<IRTMemExpr*>(e->expr.get()) ) {
			m = std::move(mem->address);
			offset += mem->offset;
		}
		else m = std::move(e->expr);

		ret(std::move(e->cmd), std::make_unique<IRTMemExpr>(std::move(m), offset));
	}
	else throw 1;  // we should never get here
}


void LValueTranslator::visit(DerefExpr& ptr) {
	auto type = dynamic_cast<PtrType*>(ptr.expr->type.get());
	assert(type);

	CmdExprPtr e = tr.get( std::move(ptr.expr) );
	ret( std::move(e->cmd), std::make_unique<IRTMemExpr>(std::move(e->expr)) );
}


void LValueTranslator::visit(IdExpr& expr) {
	ret( std::make_unique<NopCmd>(), std::make_unique<IRTIdExpr>(expr.identifier) );
}


void LValueTranslator::visit(CallExpr& expr) {
	throw 1;  // we should never get here
}


void LValueTranslator::visit(AllocExpr& alloc) {
	throw 1;  // we should never get here
}


void LValueTranslator::visit(TernaryExpr& expr) {
	throw 1;  // we should never get here
}


void LValueTranslator::visit(BinaryExpr& expr) {
	throw 1;  // we should never get here
}


void LValueTranslator::visit(UnaryExpr& unary) {
	throw 1;  // we should never get here
}


void LValueTranslator::visit(LiteralExpr& expr) {
	throw 1;  // we should never get here
}
