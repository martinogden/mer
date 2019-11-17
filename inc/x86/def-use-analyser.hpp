#pragma once
#include <vector>
#include <unordered_map>
#include "x86/asm.hpp"
#include "inst.hpp"
#include "set.hpp"


class DefUseAnalyser {
private:
	const uint n;

	std::vector<X86Asm>& code;
	std::vector<Set<Operand>> def;
	std::vector<Set<Operand>> use;
	std::vector<Set<uint>> succ;
	std::unordered_map<std::string, uint> line;

	void visit(X86Asm& as, uint l);

	void setDef(uint l, Set<Operand>&& ops);
	void setUse(uint l, Set<Operand>&& ops);

	uint label2Line(Operand& op);
	void loadLabels();
	uint nextLine(uint i);

public:
	DefUseAnalyser(std::vector<X86Asm>& code)
		: n(code.size()), code(code), def(n), use(n), succ(n) {}

	void run();

	Set<Operand>& getDef(uint l);
	Set<Operand>& getUse(uint l);
	Set<uint>& getSucc(uint l);
};