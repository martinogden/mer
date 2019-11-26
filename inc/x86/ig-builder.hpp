#include "set.hpp"
#include "graph.hpp"
#include "operand.hpp"
#include "x86/liveness-analyser.hpp"


class IGBuilder {
private:
	const uint n;
	std::vector<X86Asm>& code;
	LivenessAnalyser liveness;
	Graph<Operand>* G;

	void visit(X86Asm& as, uint l);
	void addEdge(const Operand& u, const Operand& v);

public:
	IGBuilder(std::vector<X86Asm>& code)
		: n(code.size()), code(code), liveness(code), G(new Graph<Operand>()) {}

	Graph<Operand>* run();
};
