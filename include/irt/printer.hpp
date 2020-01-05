#pragma once
#include <string>
#include "visitor.hpp"
#include "irt/irt.hpp"
#include "irt/containers.hpp"


class IRTPrinter : public IRTVisitor {
private:
	std::string retval;

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

	void ret(std::string);
	std::string get(IRTCmdPtr&);
	std::string get(IRTExprPtr&);

public:
	std::string get(IRTFun&);
};
