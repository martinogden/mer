#pragma once


class IRTFun;

class SeqCmd;
class NopCmd;
class CallCmd;
class AssignCmd;
class EffAssignCmd;
class LoadCmd;
class StoreCmd;
class LabelCmd;
class IfCmd;
class GotoCmd;
class ReturnCmd;

class CmdExpr;
class IRTIntExpr;
class IRTIdExpr;
class IRTBinaryExpr;
class IRTMemExpr;


class IRTVisitor {
public:
	virtual void visit(SeqCmd&);
	virtual void visit(NopCmd&);
	virtual void visit(CallCmd&);
	virtual void visit(AssignCmd&);
	virtual void visit(EffAssignCmd&);
	virtual void visit(LoadCmd&);
	virtual void visit(StoreCmd&);
	virtual void visit(LabelCmd&);
	virtual void visit(IfCmd&);
	virtual void visit(GotoCmd&);
	virtual void visit(ReturnCmd&);

	virtual void visit(CmdExpr&);
	virtual void visit(IRTIntExpr&);
	virtual void visit(IRTIdExpr&);
	virtual void visit(IRTBinaryExpr&);
	virtual void visit(IRTMemExpr&);

	virtual ~IRTVisitor() = default;
};
