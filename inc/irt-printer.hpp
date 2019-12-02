#pragma once
#include <string>
#include "irt-visitor.hpp"
#include "irt.hpp"


class IRTPrinter : public IRTVisitor {
private:
	std::string retval;
	void ret(std::string);

public:
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

	std::string get(IRTFunPtr&);
	std::string get(IRTCmdPtr&);
	std::string get(IRTExprPtr&);
};
