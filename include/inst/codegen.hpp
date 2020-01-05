#pragma once
#include <vector>
#include "irt/visitor.hpp"
#include "irt/irt.hpp"
#include "irt/containers.hpp"
#include "inst/operand.hpp"
#include "inst/inst.hpp"
#include "generator.hpp"


class CodeGen : public IRTVisitor {
private:
	IRTFun& fun;
	Generator& gen;

	std::vector<Inst> insts;
	Operand retval;

	void emit(const Inst&);
	void ret(Operand);

public:
	CodeGen(IRTFun& fun, Generator& gen);

	void visit(SeqCmd&) override;
	void visit(NopCmd&) override;
	void visit(CallCmd&) override;
	void visit(AssignCmd&) override;
	void visit(EffAssignCmd&) override;
	void visit(LoadCmd&) override;
	void visit(StoreCmd&) override;
	void visit(LabelCmd&) override;
	void visit(IfCmd&) override;
	void visit(GotoCmd&) override;
	void visit(ReturnCmd&) override;

	void visit(CmdExpr&) override;
	void visit(IRTIntExpr&) override;
	void visit(IRTIdExpr&) override;
	void visit(IRTBinaryExpr&) override;
	void visit(IRTMemExpr&) override;

	Operand& get(IRTExprPtr&);
	InstFun run();
};
