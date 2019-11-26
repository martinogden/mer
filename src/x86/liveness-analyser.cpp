#include "x86/liveness-analyser.hpp"


void LivenessAnalyser::visit(X86Asm& as, uint l) {
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
	for (auto& as : code) {
		if (as.opcode == X86Asm::LBL)
			std::cout << as << std::endl;
		else
			std::cout << i << ' ' << dua.getDef(i) << std::endl;
		i++;
	}
	std::cout << "\nuse\n====" << std::endl;

	i = 0;
	for (auto& as : code) {
		if (as.opcode == X86Asm::LBL)
			std::cout << as << std::endl;
		else
			std::cout << i << ' ' << dua.getUse(i) << std::endl;
		i++;
	}
	std::cout << "\nsucc\n====" << std::endl;

	i = 0;
	for (auto& as : code) {
		if (as.opcode == X86Asm::LBL)
			std::cout << as << std::endl;
		else
			std::cout << i << ' ' << dua.getSucc(i) << std::endl;
		i++;
	}
	#endif

	changed = true;
	while(changed) {
		changed = false;
		uint l = 0;
		for (auto& as : code)
			visit(as, l++);
	}
}


Set<Operand>& LivenessAnalyser::get(uint l) {
	assert(l < n);
	return live[l];
}
