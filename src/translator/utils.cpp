#include "translator/utils.hpp"


IRTCmd* concat(std::vector<IRTCmd*> cmds) {
	unsigned n = cmds.size();

	if (n == 0)
		return new NopCmd();

	IRTCmd* head = cmds[0];
	std::vector<IRTCmd*> rest(std::next(cmds.begin()), cmds.end());
	return new SeqCmd(head, concat(rest));
}


bool isPureOp(BinOp op) {
	switch (op) {
		case BinOp::DIV:
		case BinOp::MOD:
		case BinOp::LS:
		case BinOp::RS:
			return false;
		default:
			return true;
	}
}


bool isBoolOp(BinOp op) {
	switch (getOpType(op)) {
		case OpType::REL:
		case OpType::EQL:
		case OpType::LOGICAL:
			return true;
		default:
			return false;
	}
}


bool isBoolOp(UnOp op) {
	return op == UnOp::LOG_NOT;
}
