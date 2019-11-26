#include "x86/ig-builder.hpp"


Graph<Operand>* IGBuilder::run() {
	liveness.run();

	for (uint i=0; i<MAX_REG; ++i)
		G->addVertex(static_cast<Reg>(i));

	#ifdef DEBUG
	std::cout << "liveness\n========" << std::endl;
	uint i = 0;
	for (auto& as : code) {
		if (as.opcode == X86Asm::LBL)
			std::cout << as << std::endl;
		else
			std::cout << i << ' ' << liveness.get(i) << std::endl;
		i++;
	}
	#endif

	uint l = 0;
	for (auto& as : code)
		visit(as, l++);

	return G;
}

/*
Inference rule for interference relation on operands

isTmpOrReg(d)
isTmpOrReg(u)
succ(l, l')
def(l, d)
live(l+1, u)
------------
inter(d, u)
*/
void IGBuilder::visit(X86Asm& as, uint l) {
	if (as.opcode == X86Asm::LBL)
		return;

	for (const uint i : liveness.getSucc(l)) {
		if (i >= n-1)
			continue;

		for (const Operand& d : liveness.getDef(i)) {
			for (const Operand& u : liveness.get(i)) {
				// mov optimization
				if (as.opcode == X86Asm::MOV && u == as.src)
					continue;

				addEdge(d, u);
			}
		}
	}
}


void IGBuilder::addEdge(const Operand& u, const Operand& v) {
	if (!u.is(Operand::REG) && !u.is(Operand::TMP))
		return;

	if (!v.is(Operand::REG) && !v.is(Operand::TMP))
		return;

	if (u == v)
		return;

	G->addVertex(u);
	G->addVertex(v);
	G->addEdge(u, v);
}
