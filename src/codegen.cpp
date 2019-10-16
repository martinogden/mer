#include "codegen.hpp"


CodeGen::CodeGen(Prog* program) :
	program(program),
	op(Reg::RAX),
	tmp(0)
{}


std::vector<Inst>& CodeGen::run() {
	program->accept(*this);
	return insts;
}


void CodeGen::visit(BinaryExpr* expr) {
	Operand dst = getDst();

	Operand s1 = newTmp();
	setDst(s1);
	expr->left->accept(*this);

	Operand s2 = newTmp();
	setDst(s2);
	expr->right->accept(*this);

	// TODO use correct opcode
	emit({ OpCode::ADD, dst, s1, s2 });
}


void CodeGen::visit(UnaryExpr* unary) {
	Operand dst = getDst();

	Operand src = newTmp();
	setDst(src);
	unary->expr->accept(*this);

	emit({ OpCode::SUB, dst, {OpType::IMM, 0}, src });
}


void CodeGen::visit(LiteralExpr* expr) {
	int value = expr->token.value;
	emit({ OpCode::MOV, getDst(), {OpType::IMM, value} });
}


void CodeGen::visit(VarExpr* expr) {
	emit({ OpCode::MOV, getDst(), lookup(expr->identifier) });
}


void CodeGen::visit(DeclStmt* stmt) {
	Operand op = reserve(stmt->identifier);

	if (stmt->expr) {
		setDst(op);
		stmt->expr->accept(*this);
	}
}


void CodeGen::visit(ReturnStmt* stmt) {
	setDst(Reg::RAX);
	stmt->expr->accept(*this);
	emit(OpCode::RET);
}


void CodeGen::visit(AssignStmt* stmt) {
	setDst( lookup(stmt->lvalue) );
	stmt->rvalue->accept(*this);
}


void CodeGen::visit(BlockStmt* block) {
	for (auto stmt : block->statements)
		stmt->accept(*this);
}


void CodeGen::visit(Prog* prog) {
	for (auto stmt : prog->statements)
		stmt->accept(*this);
}


void CodeGen::emit(Inst inst) {
	insts.push_back(std::move(inst));
}


Operand CodeGen::newTmp() {
	return tmp++;
}


Operand CodeGen::getDst() {
	return op;
}


void CodeGen::setDst(Operand dst) {
	op = dst;
}


Operand CodeGen::reserve(std::string id) {
	env.define(id, tmp++);
	return lookup(id);
}


Operand CodeGen::lookup(std::string id) {
	return env.lookup(id);
}
