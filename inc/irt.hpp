#pragma once
#include <memory>
#include <string>
#include "operator.hpp"
#include "parse-tree.hpp"
#include "irt-visitor.hpp"


class IRTNode {
public:
	virtual ~IRTNode() = default;
};


class IRTExpr : public IRTNode {
public:
	virtual void accept(IRTVisitor& visitor) = 0;
};


class IRTCmd : public IRTNode {
public:
	virtual void accept(IRTVisitor& visitor) = 0;
};


typedef std::unique_ptr<IRTExpr> IRTExprPtr;
typedef std::unique_ptr<IRTCmd> IRTCmdPtr;


class IRTFun {
public:
	std::string id;
	std::vector<std::string> params;
	IRTCmdPtr body;

	IRTFun(std::string id, std::vector<std::string> params, IRTCmdPtr body);
	void accept(IRTVisitor& visitor);
};


typedef std::unique_ptr<IRTFun> IRTFunPtr;


class SeqCmd : public IRTCmd {
public:
	IRTCmdPtr head;
	IRTCmdPtr rest;

	SeqCmd(IRTCmdPtr head, IRTCmdPtr rest);
	void accept(IRTVisitor& visitor) override;
};


class NopCmd : public IRTCmd {
	void accept(IRTVisitor& visitor) override;
};


class CallCmd : public IRTCmd {
public:
	std::string var;
	std::string label;
	std::vector<std::string> args;

	CallCmd(std::string var, std::string label, std::vector<std::string> args);
	void accept(IRTVisitor& visitor) override;
};


// An effectful assign cmd may have side-effects,
// e.g. div by zero exceptions
class EffAssignCmd : public IRTCmd {
public:
	std::string var;
	BinOp op;
	IRTExprPtr left;
	IRTExprPtr right;

	EffAssignCmd(std::string var, BinOp op, IRTExprPtr left, IRTExprPtr right);
	void accept(IRTVisitor& visitor) override;
};


// A pure assign is guaranteed to have no side-effects
class AssignCmd : public IRTCmd {
public:
	std::string var;
	IRTExprPtr value;

	AssignCmd(std::string var, IRTExprPtr value);
	void accept(IRTVisitor& visitor) override;
};


class LabelCmd : public IRTCmd {
public:
	std::string name;

	LabelCmd(std::string name);
	void accept(IRTVisitor& visitor) override;
};


struct Comparison {
	BinOp op;
	IRTExprPtr left;
	IRTExprPtr right;

	Comparison(BinOp op, IRTExprPtr left, IRTExprPtr right);
};


class IfCmd : public IRTCmd {
public:
	Comparison cmp;

	std::string then;
	std::string otherwise;

	IfCmd(Comparison cmp, std::string then, std::string otherwise);
	void accept(IRTVisitor& visitor) override;
};


class GotoCmd : public IRTCmd {
public:
	std::string label;

	GotoCmd(std::string label);
	void accept(IRTVisitor& visitor) override;
};


class ReturnCmd : public IRTCmd {
public:
	IRTExprPtr expr;

	ReturnCmd(IRTExprPtr expr);
	void accept(IRTVisitor& visitor) override;
};


// TODO: rename
class CmdExpr : public IRTCmd {
public:
	IRTCmdPtr cmd;
	IRTExprPtr expr;

	CmdExpr(IRTCmdPtr cmd, IRTExprPtr expr);
	void accept(IRTVisitor& visitor) override;
};


typedef std::unique_ptr<CmdExpr> CmdExprPtr;


class IntExpr : public IRTExpr {
public:
	int value;

	IntExpr(int value);
	void accept(IRTVisitor& visitor) override;
};


class VarExpr : public IRTExpr {
public:
	std::string name;

	VarExpr(std::string name);
	void accept(IRTVisitor& visitor) override;
};


// TODO: better name or namespace it
class PairExpr : public IRTExpr {
public:
	BinOp op;
	IRTExprPtr left;
	IRTExprPtr right;

	PairExpr(BinOp op, IRTExprPtr left, IRTExprPtr right);
	void accept(IRTVisitor& visitor) override;
};
