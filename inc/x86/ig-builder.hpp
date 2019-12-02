#pragma once

#include <memory>
#include "set.hpp"
#include "graph.hpp"
#include "operand.hpp"
#include "x86/liveness-analyser.hpp"


using IGPtr = std::unique_ptr<Graph<Operand>>;


class IGBuilder {
private:
	const uint n;
	X86Fun& fun;
	LivenessAnalyser liveness;
	IGPtr G;

	void visit(X86Asm& as, uint l);
	void addEdge(const Operand& u, const Operand& v);

public:
	IGBuilder(X86Fun&);
	IGPtr run();
};
