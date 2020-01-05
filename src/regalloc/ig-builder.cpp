#include "regalloc/regalloc.hpp"
#include "regalloc/ig-builder.hpp"


IGBuilder::IGBuilder(const InstFun& fun) :
		n(fun.insts.size()),
		fun(fun),
		liveness(fun),
		G(std::make_unique<Graph<Operand>>())
{}


std::unique_ptr<Graph<Operand>> IGBuilder::run() {
	liveness.run();

	// add all registers to IG
	for (uint i=0; i<=MAX_REG; ++i)
		G->addVertex(static_cast<Reg>(i));

	// each parameter must interfere with the register
	// associated with each other register
	// (Actually it need only interfere with the regs
	// associated with later params in ltr order)
	uint i = 1;
	for (auto const& param : fun.params) {
		for (uint j=1; j<=fun.params.size(); ++j)
			if (i != j)
				addEdge(param, static_cast<Reg>(j));
		++i;
	}

#ifdef DEBUG
	std::cout << "\nliveness\n========" << std::endl;
	i = 0;
	for (auto& inst : fun.insts) {
		if ( inst.is(Inst::LBL) )
			std::cout << inst << std::endl;
		else
			std::cout << i << ' ' << liveness.get(i) << std::endl;
		i++;
	}
#endif

	uint l = 0;
	for (auto& inst : fun.insts)
		visit(inst, l++);

	return std::move(G);
}


/*
Inference rule for interference relation on operands

isTmpOrReg(d)
isTmpOrReg(u)
succ(l, i)
def(l, d)
live(i, u)
u != d
------------
inter(d, u)
*/
void IGBuilder::visit(const Inst& inst, const uint l) {
	if ( inst.is(Inst::LBL) )
		return;

	addVertices(inst);

	for (uint i : liveness.getSucc(l)) {
		if (i > n-1)
			continue;

		for (const Operand& d : liveness.getDef(l)) {
			for (const Operand& u : liveness.get(i)) {
				// mov optimization
				if ( inst.is(Inst::MOV) && u == inst.getSrc1() )
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


void IGBuilder::addVertex(const Operand& op) {
	switch (op.getType()) {
		case Operand::REG:
		case Operand::TMP:
			G->addVertex(op);
			break;
		case Operand::MEM:
			G->addVertex(op.getMemOperand());
		case Operand::LBL:
		case Operand::IMM:
			break;
	}
}


void IGBuilder::addVertices(const Inst& inst) {
	uint parity = inst.getParity();

	if (parity > 0)
		addVertex(inst.getDst());
	if (parity > 1)
		addVertex(inst.getSrc1());
	if (parity > 2)
		addVertex(inst.getSrc2());
}
