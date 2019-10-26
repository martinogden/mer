#pragma once
#include <set>
#include <string>
#include <vector>
#include "ir.hpp"


std::vector<std::string> instsel(std::vector<Inst>& insts, std::set<Reg>& regs);
