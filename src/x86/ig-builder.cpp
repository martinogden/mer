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


void IGBuilder::visit(X86Asm& as, uint l) {
	switch (as.opcode) {
		case X86Asm::LBL:
			return;
		case X86Asm::MOV:
			return visitMov(as, l);
		default:
			break;
	}

	switch (as.parity) {
		case 0:
			return visitNullary(as, l);
		case 1:
			return visitUnary(as, l);
		case 2:
			return visitBinary(as, l);
		default:
			throw 1;  // TODO: we should never get here
	}
}


void IGBuilder::visitNullary(X86Asm& as, uint l) {
	Set<Operand>& live = liveness.get(l);
	addVertices(live);
	addEdges(live);
}


void IGBuilder::visitUnary(X86Asm& as, uint l) {
	Set<Operand>& live = liveness.get(l);
	addVertices(live | Set<Operand>({ as.dst }));
	addEdges(live);
}


void IGBuilder::visitBinary(X86Asm& as, uint l) {
	Set<Operand>& live = liveness.get(l);
	addVertices(live | Set<Operand>({ as.dst, as.src }));
	addEdges(live);
}


void IGBuilder::visitMov(X86Asm& as, uint l) {
	bool isJoined = false;
	bool bothTmps = as.dst.is(Operand::TMP) && as.src.is(Operand::TMP);
	if (bothTmps)
		isJoined = G->hasEdge(as.dst, as.src);

	Set<Operand>& live = liveness.get(l);
	addVertices(live | Set<Operand>({ as.dst, as.src }));
	addEdges(live);

	if (bothTmps && !isJoined)
		G->removeEdge(as.dst, as.src);
}


void IGBuilder::addVertices(Set<Operand> ops) {
	for (const auto& op : ops) {
		if (op.is(Operand::REG) || op.is(Operand::TMP))
			G->addVertex(op);
	}
}


void IGBuilder::addEdges(Set<Operand>& live) {
	// TODO reduce unnecessary loops
	for (const auto& u : live) {
		if (!u.is(Operand::REG) && !u.is(Operand::TMP))
			continue;

		for (const auto& v : live) {
			if (!v.is(Operand::REG) && !v.is(Operand::TMP))
				continue;

			if (u != v)
				G->addEdge(u, v);
		}
	}
}
