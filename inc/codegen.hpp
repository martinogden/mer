#pragma once
#include <vector>
#include "irt-visitor.hpp"
#include "irt.hpp"
#include "operand.hpp"
#include "generator.hpp"


class CodeGen : public IRTVisitor {
private:
	IRTCmd* cmd;
	Generator& gen;

	std::vector<Inst> insts;
	Operand retval;

	void emit(Inst);
	void ret(Operand);

public:
	CodeGen(IRTCmd* cmd, Generator& gen);

	void visit(SeqCmd*);
	void visit(NopCmd*);
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
	std::vector<Inst>& run();
};