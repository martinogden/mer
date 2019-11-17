#pragma once
#include <string>
#include "irt-visitor.hpp"
#include "irt.hpp"


class IRTPrinter : public IRTVisitor {
private:
	std::string retval;
	void ret(std::string);

public:
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

	std::string get(IRTCmd*);
	std::string get(IRTExpr*);
};
