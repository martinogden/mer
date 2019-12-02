#pragma once
#include <vector>
#include "irt-visitor.hpp"
#include "irt.hpp"
#include "operand.hpp"
#include "inst.hpp"
#include "generator.hpp"


class CodeGen : public IRTVisitor {
private:
	IRTFun* fun;
	Generator& gen;

	std::vector<Inst> insts;
	Operand retval;

	void emit(Inst);
	void ret(Operand);

public:
	CodeGen(IRTFun* fun, Generator& gen);

	void visit(IRTFun*);
	void visit(SeqCmd*);
	void visit(NopCmd*);
	void visit(CallCmd*);
	void visit(AssignCmd*);
	void visit(EffAssignCmd*);
	void visit(LabelCmd*);
	void visit(IfCmd*);
	void visit(GotoCmd*);
	void visit(ReturnCmd*);

	void visit(CmdExpr*);
	void visit(IntExpr*);
	void visit(VarExpr*);
	void visit(PairExpr*);

	Operand& get(IRTExpr*);
	InstFun run();
};