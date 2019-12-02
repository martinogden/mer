#include "set.hpp"
#include "graph.hpp"
#include "operand.hpp"
#include "x86/liveness-analyser.hpp"


class IGBuilder {
private:
	const uint n;
	X86Fun& fun;
	LivenessAnalyser liveness;
	Graph<Operand>* G;

	void visit(X86Asm& as, uint l);
	void addEdge(const Operand& u, const Operand& v);

public:
	IGBuilder(X86Fun&);
	Graph<Operand>* run();
};
