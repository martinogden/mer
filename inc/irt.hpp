#pragma once
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
	virtual ~IRTExpr() = default;
	virtual void accept(IRTVisitor& visitor) = 0;
};


class IRTCmd : public IRTNode {
public:
	virtual ~IRTCmd() = default;
	virtual void accept(IRTVisitor& visitor) = 0;
};


class IRTFun {
public:
	std::string id;
	std::vector<std::string> params;
	IRTCmd* body;

	IRTFun(std::string id, std::vector<std::string> params, IRTCmd* body);
	void accept(IRTVisitor& visitor);
};


class SeqCmd : public IRTCmd {
public:
	IRTCmd* head;
	IRTCmd* rest;

	SeqCmd(IRTCmd* head, IRTCmd* rest);
	void accept(IRTVisitor& visitor);
};


class NopCmd : public IRTCmd {
	void accept(IRTVisitor& visitor);
};


class CallCmd : public IRTCmd {
public:
	std::string var;
	std::string label;
	std::vector<std::string> args;

	CallCmd(std::string var, std::string label, std::vector<std::string> args);
	void accept(IRTVisitor& visitor);
};


// An effectful assign cmd may have side-effects,
// e.g. div by zero exceptions
class EffAssignCmd : public IRTCmd {
public:
	std::string var;
	BinOp op;
	IRTExpr* left;
	IRTExpr* right;

	EffAssignCmd(std::string var, BinOp op, IRTExpr* left, IRTExpr* right);
	void accept(IRTVisitor& visitor);
};


// A pure assign is guaranteed to have no side-effects
class AssignCmd : public IRTCmd {
public:
	std::string var;
	IRTExpr* value;

	AssignCmd(std::string var, IRTExpr* value);
	void accept(IRTVisitor& visitor);
};


class LabelCmd : public IRTCmd {
public:
	std::string name;

	LabelCmd(std::string name);
	void accept(IRTVisitor& visitor);
};


struct Comparison {
	BinOp op;
	IRTExpr* left;
	IRTExpr* right;

	Comparison(BinOp op, IRTExpr* left, IRTExpr* right);
};


class IfCmd : public IRTCmd {
public:
	Comparison cmp;

	std::string then;
	std::string otherwise;

	IfCmd(Comparison cmp, std::string then, std::string otherwise);
	void accept(IRTVisitor& visitor);
};


class GotoCmd : public IRTCmd {
public:
	std::string label;

	GotoCmd(std::string label);
	void accept(IRTVisitor& visitor);
};


class ReturnCmd : public IRTCmd {
public:
	IRTExpr* expr;

	ReturnCmd(IRTExpr* expr);
	void accept(IRTVisitor& visitor);
};


// TODO: rename
class CmdExpr : public IRTCmd {
public:
	IRTCmd* cmd;
	IRTExpr* expr;

	CmdExpr(IRTCmd* cmd, IRTExpr* expr);
	void accept(IRTVisitor& visitor);
};


class IntExpr : public IRTExpr {
public:
	int value;

	IntExpr(int value);
	void accept(IRTVisitor& visitor);
};


class VarExpr : public IRTExpr {
public:
	std::string name;

	VarExpr(std::string name);
	void accept(IRTVisitor& visitor);
};


// TODO: better name or namespace it
class PairExpr : public IRTExpr {
public:
	BinOp op;
	IRTExpr* left;
	IRTExpr* right;

	PairExpr(BinOp op, IRTExpr* left, IRTExpr* right);
	void accept(IRTVisitor& visitor);
};
