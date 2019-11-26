#pragma once
#include <vector>
#include "x86/def-use-analyser.hpp"


class LivenessAnalyser {
private:
	const uint n;
	std::vector<X86Asm>& code;
	DefUseAnalyser dua;
	std::vector<Set<Operand>> live;
	bool changed;

	void visit(X86Asm& as, uint l);
	void update(uint i, Set<Operand> opnds);

public:
	LivenessAnalyser(std::vector<X86Asm>& code)
		: n(code.size()), code(code), dua(code), live(n) {}

	void run();
	Set<Operand>& get(uint l);
	Set<Operand>& getDef(uint l);
	Set<Operand>& getUse(uint l);
	Set<uint>& getSucc(uint l);
};
