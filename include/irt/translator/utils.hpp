#pragma once
#include "irt/irt.hpp"


IRTCmdPtr concat(std::vector<IRTCmdPtr> cmds);


bool isPureOp(BinOp op);
bool isBoolOp(BinOp op);
bool isBoolOp(UnOp op);
