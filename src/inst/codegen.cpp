#include "inst/codegen.hpp"
#include "conversion.hpp"


CodeGen::CodeGen(IRTFun& fun, Generator& gen) :
	fun(fun),
	gen(gen)
{}


InstFun CodeGen::run() {
	emit({ Inst::ENTER });
	fun.body->accept(*this);
	return InstFun(fun.id, std::move(fun.params), std::move(insts));
}


void CodeGen::emit(const Inst& inst) {
	insts.push_back(inst);
}


void CodeGen::ret(Operand op) {
	retval = std::move(op);
}


Operand& CodeGen::get(IRTExprPtr& expr) {
	expr->accept(*this);
	return retval;
}


void CodeGen::visit(SeqCmd& cmd) {
	cmd.head->accept(*this);
	cmd.rest->accept(*this);
}


void CodeGen::visit(NopCmd& cmd) {}


void CodeGen::visit(CallCmd& cmd) {
	int i = 1;
	for (auto const& arg : cmd.args)
		emit({ Inst::ARG, i++, arg });
	emit({ Inst::CALL, cmd.var, Operand::label(cmd.label), i-1 });
	// TODO: check if we can omit this and save it for x86 codegen
}


void CodeGen::visit(AssignCmd& cmd) {
	Operand op = get(cmd.value);
	emit({ Inst::MOV, cmd.var, op });
}


void CodeGen::visit(EffAssignCmd& cmd) {
	Operand left = get(cmd.left);
	Operand right = get(cmd.right);
	Inst::OpCode opcode = toOpCode(cmd.op);

	emit({ opcode, cmd.var, left, right });
}


void CodeGen::visit(LoadCmd& cmd) {
	Operand& op = get(cmd.src);
	emit({ Inst::OpCode::MOV, cmd.dst, op });
}


void CodeGen::visit(StoreCmd& cmd) {
	Operand& op = get(cmd.dst);
	emit({ Inst::OpCode::MOV, op, cmd.src });
}


void CodeGen::visit(LabelCmd& cmd) {
	emit({ Inst::LBL, Operand::label(cmd.name) });
}


void CodeGen::visit(IfCmd& cmd) {
	Operand left = get(cmd.cmp.left);
	Operand right = get(cmd.cmp.right);
	std::string t = gen.tmp();
	Inst::OpCode opcode = toOpCode(cmd.cmp.op);

	emit({ Inst::SUB, t, left, right });
	emit({ opcode, t, Operand::label(cmd.then), Operand::label(cmd.otherwise) });
}


void CodeGen::visit(GotoCmd& cmd) {
	emit({ Inst::JMP, Operand::label(cmd.label) });
}


void CodeGen::visit(ReturnCmd& cmd) {
	Operand op = get(cmd.expr);
	emit({ Inst::RET, op });
}


void CodeGen::visit(CmdExpr& e) {
	e.cmd->accept(*this);
	ret( get(e.expr) );
}


void CodeGen::visit(IRTIntExpr& e) {
	ret(e.value);
}


void CodeGen::visit(IRTIdExpr& e) {
	ret(e.name);
}


void CodeGen::visit(IRTBinaryExpr& e) {
	Operand left = get(e.left);
	Operand right = get(e.right);
	std::string t = gen.tmp();
	Inst::OpCode opcode = toOpCode(e.op);

	emit({ opcode, t, left, right });
	ret(t);
}


void CodeGen::visit(IRTMemExpr& e) {
	Operand& op = get(e.address);
	switch (op.getType()) {
		case Operand::REG:
			ret({ op.getReg(), e.offset });
			break;
		case Operand::TMP:
			ret({ op.getTmp(), e.offset });
			break;
		default:
			throw 1;  // we should never get here
	}
}
