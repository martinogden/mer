#pragma once
#include <vector>
#include <unordered_map>
#include "inst/inst.hpp"
#include "set.hpp"


class DefUseAnalyser {
private:
	const uint n;

	const InstFun& fun;
	std::vector<Set<Operand>> def;
	std::vector<Set<Operand>> use;
	std::vector<Set<uint>> succ;
	std::unordered_map<std::string, uint> line;

	void visit(const Inst& as, uint l);

	void setDef(uint l, Set<Operand>&& ops);
	void setUse(uint l, Set<Operand>&& ops);

	uint label2Line(const Operand& op);
	void loadLabels();
	uint nextLine(uint i);

public:
	DefUseAnalyser(const InstFun& fun);
	void run();

	Set<Operand>& getDef(uint l);
	Set<Operand>& getUse(uint l);
	Set<uint>& getSucc(uint l);
};