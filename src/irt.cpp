#include "irt.hpp"


IRTFun::IRTFun(std::string id, std::vector<std::string> params, IRTCmd* body) :
	id(id),
	params(params),
	body(body)
{}


void IRTFun::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


SeqCmd::SeqCmd(IRTCmd* head, IRTCmd* rest) :
	head(head),
	rest(rest)
{}

void SeqCmd::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


void NopCmd::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


CallCmd::CallCmd(std::string var, std::string label, std::vector<std::string> args) :
	var(var),
	label(label),
	args(args)
{}


void CallCmd::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


EffAssignCmd::EffAssignCmd(std::string var, BinOp op, IRTExpr* left, IRTExpr* right) :
	var(var),
	op(op),
	left(left),
	right(right)
{}


void EffAssignCmd::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


AssignCmd::AssignCmd(std::string var, IRTExpr* value) :
	var(var),
	value(value)
{}


void AssignCmd::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


LabelCmd::LabelCmd(std::string name) :
	name(name)
{}


void LabelCmd::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


Comparison::Comparison(BinOp op, IRTExpr* left, IRTExpr* right) :
	op(op),
	left(left),
	right(right)
{}


IfCmd::IfCmd(Comparison cmp, std::string then, std::string otherwise) :
	cmp(cmp),
	then(then),
	otherwise(otherwise)
{}


void IfCmd::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


GotoCmd::GotoCmd(std::string label) :
	label(label)
{}


void GotoCmd::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


ReturnCmd::ReturnCmd(IRTExpr* expr) :
	expr(expr)
{}


void ReturnCmd::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


CmdExpr::CmdExpr(IRTCmd* cmd, IRTExpr* expr) :
	cmd(cmd),
	expr(expr)
{}


void CmdExpr::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


IntExpr::IntExpr(int value) :
	value(value)
{}


void IntExpr::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


VarExpr::VarExpr(std::string name) :
	name(name)
{}


void VarExpr::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}


PairExpr::PairExpr(BinOp op, IRTExpr* left, IRTExpr* right) :
	op(op),
	left(left),
	right(right)
{}


void PairExpr::accept(IRTVisitor& visitor) {
	visitor.visit(this);
}