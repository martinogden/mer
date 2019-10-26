#pragma once
#include <set>
#include "ir.hpp"
#include "cfg.hpp"
#include "graph.hpp"


// allocated registers (in-place)
std::set<Reg> alloc(BasicBlock& bb);
