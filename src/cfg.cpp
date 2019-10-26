#include "cfg.hpp"


BasicBlock toBasicBlock(std::vector<Inst> insts) {
	BasicBlock bb;

	for (Inst& inst : insts) {
		bb.insts.push_back(inst);
		if (inst.opcode == OpCode::RET)
			break;
	}

	return bb;
}
