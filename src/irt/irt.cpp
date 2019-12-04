#include "irt/irt.hpp"


IRTFun::IRTFun(std::string id, std::vector<std::string> params, IRTCmdPtr body) :
	id(std::move(id)),
	params(std::move(params)),
	body(std::move(body))
{}


void IRTFun::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


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


IntExpr::IntExpr(int value) :
	value(value)
{}


void IntExpr::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


VarExpr::VarExpr(std::string name) :
	name(std::move(name))
{}


void VarExpr::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}


PairExpr::PairExpr(BinOp op, IRTExprPtr left, IRTExprPtr right) :
	op(op),
	left(std::move(left)),
	right(std::move(right))
{}


void PairExpr::accept(IRTVisitor& visitor) {
	visitor.visit(*this);
}
