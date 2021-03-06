#pragma once
#include <vector>
#include "inst/inst.hpp"
#include "regalloc/def-use-analyser.hpp"


class LivenessAnalyser {
private:
	const uint n;
	const InstFun& fun;
	DefUseAnalyser dua;
	std::vector<Set<Operand>> live;
	bool changed;

	void visit(const Inst& inst, uint l);
	void update(uint i, Set<Operand> opnds);

public:
	LivenessAnalyser(const InstFun& fun);
	void run();
	Set<Operand>& get(uint l);
	Set<Operand>& getDef(uint l);
	Set<Operand>& getUse(uint l);
	Set<uint>& getSucc(uint l);
};
