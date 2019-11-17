#include "set.hpp"
#include "graph.hpp"
#include "operand.hpp"
#include "x86/liveness-analyser.hpp"


class IGBuilder {
private:
	std::vector<X86Asm>& code;
	LivenessAnalyser liveness;
	Graph<Operand>* G;

	void visit(X86Asm& as, uint l);
	void visitNullary(X86Asm& as, uint l);
	void visitUnary(X86Asm& as, uint l);
	void visitBinary(X86Asm& as, uint l);
	void visitMov(X86Asm& as, uint l);

	void addVertices(Set<Operand> ops);
	void addEdges(Set<Operand>& live);

public:
	IGBuilder(std::vector<X86Asm>& code)
		: code(code), liveness(code), G(new Graph<Operand>()) {}

	Graph<Operand>* run();
};
