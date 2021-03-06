#include "regalloc/liveness-analyser.hpp"


LivenessAnalyser::LivenessAnalyser(const InstFun& fun) :
		n(fun.insts.size()),
		fun(fun),
		dua(fun),
		live(n),
		changed(true)
{}


/*
 * Inference rule for liveness relation
 *
 * use(l, u)
 * ----------
 * live(l, u)
 *
 * live(i, u)
 * succ(l, i)
 * !def(l, u)
 * ----------
 * live(l, u)
 */
void LivenessAnalyser::visit(const Inst& inst, uint l) {
	update(l, dua.getUse(l));

	for (uint i : dua.getSucc(l)) {
		if (i < n)
			update(l, live[i] - dua.getDef(l));
	}
}


void LivenessAnalyser::update(uint i, Set<Operand> opnds) {
	if ( !(opnds - live[i]).empty() ) {
		live[i] = live[i] | opnds;
		changed = true;
	}
}


void LivenessAnalyser::run() {
	dua.run();

#ifdef DEBUG
	std::cout << "def\n====" << std::endl;
	uint i = 0;
	for (auto& inst : fun.insts) {
		if ( inst.is(Inst::LBL) )
			std::cout << inst << std::endl;
		else
			std::cout << i << ' ' << dua.getDef(i) << std::endl;
		i++;
	}
	std::cout << "\nuse\n====" << std::endl;

	i = 0;
	for (auto& inst : fun.insts) {
		if ( inst.is(Inst::LBL) )
			std::cout << inst << std::endl;
		else
			std::cout << i << ' ' << dua.getUse(i) << std::endl;
		i++;
	}
	std::cout << "\nsucc\n====" << std::endl;

	i = 0;
	for (auto& inst : fun.insts) {
		if ( inst.is(Inst::LBL) )
			std::cout << inst << std::endl;
		else
			std::cout << i << ' ' << dua.getSucc(i) << std::endl;
		i++;
	}
#endif

	changed = true;
	while(changed) {
		changed = false;
		uint l = 0;
		for (const auto& inst : fun.insts)
			visit(inst, l++);
	}
}


Set<Operand>& LivenessAnalyser::get(uint l) {
	assert(l < n);
	return live[l];
}


Set<Operand>& LivenessAnalyser::getDef(uint l) {
	assert(l < n);
	return dua.getDef(l);
}


Set<Operand>& LivenessAnalyser::getUse(uint l) {
	assert(l < n);
	return dua.getUse(l);
}


Set<uint>& LivenessAnalyser::getSucc(uint l) {
	assert(l < n);
	return dua.getSucc(l);
}
