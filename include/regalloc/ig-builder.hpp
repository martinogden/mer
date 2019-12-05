#pragma once

#include <memory>
#include "set.hpp"
#include "graph.hpp"
#include "inst/inst.hpp"
#include "regalloc/liveness-analyser.hpp"


using IGPtr = std::unique_ptr<Graph<Operand>>;


class IGBuilder {
private:
	const uint n;
	InstFun& fun;
	LivenessAnalyser liveness;
	IGPtr G;

	void visit(Inst& inst, uint l);
	void addEdge(const Operand& u, const Operand& v);

public:
	IGBuilder(InstFun&);
	IGPtr run();
};
