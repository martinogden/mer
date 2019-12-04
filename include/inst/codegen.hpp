#pragma once
#include <vector>
#include "irt/visitor.hpp"
#include "irt/irt.hpp"
#include "inst/operand.hpp"
#include "inst/inst.hpp"
#include "inst/generator.hpp"


class CodeGen : public IRTVisitor {
private:
	IRTFunPtr& fun;
	Generator& gen;

	std::vector<Inst> insts;
	Operand retval;

	void emit(const Inst&);
	void ret(Operand);

public:
	CodeGen(IRTFunPtr& fun, Generator& gen);

	void visit(IRTFun&) override;
	void visit(SeqCmd&) override;
	void visit(NopCmd&) override;
	void visit(CallCmd&) override;
	void visit(AssignCmd&) override;
	void visit(EffAssignCmd&) override;
	void visit(LabelCmd&) override;
	void visit(IfCmd&) override;
	void visit(GotoCmd&) override;
	void visit(ReturnCmd&) override;

	void visit(CmdExpr&) override;
	void visit(IntExpr&) override;
	void visit(VarExpr&) override;
	void visit(PairExpr&) override;

	Operand& get(IRTExprPtr&);
	InstFun run();
};
