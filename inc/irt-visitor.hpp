#pragma once


class IRTFun;

class SeqCmd;
class NopCmd;
class CallCmd;
class AssignCmd;
class EffAssignCmd;
class LabelCmd;
class IfCmd;
class GotoCmd;
class ReturnCmd;

class CmdExpr;
class IntExpr;
class VarExpr;
class PairExpr;


class IRTVisitor {
public:
	virtual void visit(IRTFun*);

	virtual void visit(SeqCmd*);
	virtual void visit(NopCmd*);
	virtual void visit(CallCmd*);
	virtual void visit(AssignCmd*);
	virtual void visit(EffAssignCmd*);
	virtual void visit(LabelCmd*);
	virtual void visit(IfCmd*);
	virtual void visit(GotoCmd*);
	virtual void visit(ReturnCmd*);

	virtual void visit(CmdExpr*);
	virtual void visit(IntExpr*);
	virtual void visit(VarExpr*);
	virtual void visit(PairExpr*);

	virtual ~IRTVisitor() = default;
};
