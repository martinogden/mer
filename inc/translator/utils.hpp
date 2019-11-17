#pragma once
#include "irt.hpp"


IRTCmd* concat(std::vector<IRTCmd*> cmds);


bool isPureOp(BinOp op);
bool isBoolOp(BinOp op);
bool isBoolOp(UnOp op);
