#include "irt/irt.hpp"


SeqCmd::SeqCmd(IRTCmdPtr head, IRTCmdPtr rest) :
	head(std::move(head)),
	rest(std::move(rest))
{}


void SeqCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


void NopCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


CallCmd::CallCmd(std::string var, std::string label, std::vector<std::string> args) :
	var(std::move(var)),
	label(std::move(label)),
	args(std::move(args))
{}


void CallCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


EffAssignCmd::EffAssignCmd(std::string var, BinOp op, IRTExprPtr left, IRTExprPtr right) :
	var(std::move(var)),
	op(op),
	left(std::move(left)),
	right(std::move(right))
{}


void EffAssignCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


AssignCmd::AssignCmd(std::string var, IRTExprPtr value) :
	var(std::move(var)),
	value(std::move(value))
{}


void AssignCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


LoadCmd::LoadCmd(std::string dst, IRTExprPtr src) :
	dst(std::move(dst)),
	src(std::move(src))
{}


void LoadCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


StoreCmd::StoreCmd(IRTExprPtr dst, std::string src) :
	dst(std::move(dst)),
	src(std::move(src))
{}


void StoreCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


LabelCmd::LabelCmd(std::string name) :
	name(std::move(name))
{}


void LabelCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


Comparison::Comparison(BinOp op, IRTExprPtr left, IRTExprPtr right) :
	op(op),
	left(std::move(left)),
	right(std::move(right))
{}


IfCmd::IfCmd(Comparison cmp, std::string then, std::string otherwise) :
	cmp(std::move(cmp)),
	then(std::move(then)),
	otherwise(std::move(otherwise))
{}


void IfCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


GotoCmd::GotoCmd(std::string label) :
	label(std::move(label))
{}


void GotoCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


ReturnCmd::ReturnCmd(IRTExprPtr expr) :
	expr(std::move(expr))
{}


void ReturnCmd::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


CmdExpr::CmdExpr(IRTCmdPtr cmd, IRTExprPtr expr) :
	cmd(std::move(cmd)),
	expr(std::move(expr))
{}


void CmdExpr::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


IRTIntExpr::IRTIntExpr(int64_t value) :
	value(value)
{}


void IRTIntExpr::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


IRTIdExpr::IRTIdExpr(std::string name) :
	name(std::move(name))
{}


void IRTIdExpr::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


IRTBinaryExpr::IRTBinaryExpr(BinOp op, IRTExprPtr left, IRTExprPtr right) :
	op(op),
	left(std::move(left)),
	right(std::move(right))
{}


void IRTBinaryExpr::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


IRTMemExpr::IRTMemExpr(IRTExprPtr address, int offset) :
	address(std::move(address)),
	offset(offset)
{}


void IRTMemExpr::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}
