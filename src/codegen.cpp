#include "codegen.hpp"
#include "conversion.hpp"


CodeGen::CodeGen(IRTFun* fun, Generator& gen) :
	fun(fun),
	gen(gen)
{}


InstFun CodeGen::run() {
	fun->accept(*this);
	return InstFun(fun->id, std::move(fun->params), std::move(insts));
}


void CodeGen::emit(const Inst& inst) {
	insts.push_back(inst);
}


void CodeGen::ret(Operand op) {
	retval = std::move(op);
}


Operand& CodeGen::get(IRTExpr* expr) {
	expr->accept(*this);
	return retval;
}


void CodeGen::visit(IRTFun* fun) {
	fun->body->accept(*this);
}


void CodeGen::visit(SeqCmd* cmd) {
	cmd->head->accept(*this);
	cmd->rest->accept(*this);
}


void CodeGen::visit(NopCmd* cmd) {}


void CodeGen::visit(CallCmd* cmd) {
	int i = 1;
	for (auto const& arg : cmd->args)
		emit({ Inst::ARG, i++, arg });
	emit({ Inst::CALL, Operand::label(cmd->label), i-1 });
	emit({ Inst::MOV, cmd->var, Reg::EAX });
}


void CodeGen::visit(AssignCmd* cmd) {
	Operand op = get(cmd->value);
	emit({ Inst::MOV, cmd->var, op });
}


void CodeGen::visit(EffAssignCmd* cmd) {
	Operand left = get(cmd->left);
	Operand right = get(cmd->right);
	Inst::OpCode opcode = toOpCode(cmd->op);

	emit({ opcode, cmd->var, left, right });
}


void CodeGen::visit(LabelCmd* cmd) {
	emit({ Inst::LBL, Operand::label(cmd->name) });
}


void CodeGen::visit(IfCmd* cmd) {
	Operand t = gen.tmp();
	Operand left = get(cmd->cmp.left);
	Operand right = get(cmd->cmp.right);
	Inst::OpCode opcode = toOpCode(cmd->cmp.op);

	emit({ Inst::SUB, t, left, right });
	emit({ opcode, t, Operand::label(cmd->then), Operand::label(cmd->otherwise) });
}


void CodeGen::visit(GotoCmd* cmd) {
	emit({ Inst::JMP, Operand::label(cmd->label) });
}


void CodeGen::visit(ReturnCmd* cmd) {
	Operand op = get(cmd->expr);
	emit({ Inst::RET, op });
}


void CodeGen::visit(CmdExpr* e) {
	e->cmd->accept(*this);
	ret( get(e->expr) );
}


void CodeGen::visit(IntExpr* e) {
	ret(e->value);
}


void CodeGen::visit(VarExpr* e) {
	ret(e->name);
}


void CodeGen::visit(PairExpr* e) {
	Operand left = get(e->left);
	Operand right = get(e->right);
	Operand t = gen.tmp();
	Inst::OpCode opcode = toOpCode(e->op);

	emit({ opcode, t, left, right });
	ret(t);
}
