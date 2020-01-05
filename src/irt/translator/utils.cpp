#include "irt/translator/utils.hpp"


IRTCmdPtr concat(std::vector<IRTCmdPtr> cmds) {
	IRTCmdPtr cmd = std::make_unique<NopCmd>();

	for (auto it=cmds.rbegin(); it!=cmds.rend(); ++it) {
		assert(*it);
		assert(cmd);
		cmd = std::make_unique<SeqCmd>(std::move(*it), std::move(cmd));
	}

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
