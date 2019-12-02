#include "translator/utils.hpp"


IRTCmdPtr concat(std::vector<IRTCmdPtr> cmds) {
	IRTCmdPtr cmd = std::make_unique<NopCmd>();

	for (auto it=cmds.rbegin(); it!=cmds.rend(); ++it)
		cmd = std::make_unique<SeqCmd>(std::move(*it), std::move(cmd));

	return cmd;
}


bool isPureOp(BinOp op) {
	switch (op) {
		case BinOp::DIV:
		case BinOp::MOD:
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
