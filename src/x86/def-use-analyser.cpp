#include "x86/def-use-analyser.hpp"


void DefUseAnalyser::visit(X86Asm& as, uint l) {
	succ[l] = {{ nextLine(l) }};

	switch (as.opcode) {
		case X86Asm::LBL:
			break;

		case X86Asm::NEG:
			setDef(l, {{ as.dst }});
			setUse(l, {{ as.dst }});
			break;
		case X86Asm::ADD:
		case X86Asm::SUB:
		case X86Asm::IMUL:
		case X86Asm::AND:
		case X86Asm::OR:
		case X86Asm::XOR:
			setDef(l, {{ as.dst }});
			setUse(l, {{ as.dst, as.src }});
			break;
		case X86Asm::IDIV:
		case X86Asm::MOD:
			setDef(l, {{ Reg::EAX, Reg::EDX }});
			setUse(l, {{ Reg::EAX, Reg::EDX, as.dst }});
			break;
		case X86Asm::CDQ:
			setUse(l, {{ Reg::EAX }});
			setDef(l, {{ Reg::EDX }});
			break;
		case X86Asm::MOV:
			setDef(l, {{ as.dst }});
			setUse(l, {{ as.src }});
			break;
		case X86Asm::JMP:
			succ[l] = Set<uint>({ label2Line(as.dst) });
			break;
		case X86Asm::JE:
		case X86Asm::JNE:
		case X86Asm::JL:
		case X86Asm::JLE:
		case X86Asm::JG:
		case X86Asm::JGE:
			setUse(l, {{ as.dst }});
			succ[l] = succ[l] | Set<uint>({ label2Line(as.dst) });
			break;
		case X86Asm::RET:
			setUse(l, {{ Reg::EAX }});
			succ[l] = {};
			break;
		case X86Asm::PUSH:
			setUse(l, {{ as.dst }});
			break;
		case X86Asm::POP:
			setDef(l, {{ as.dst }});
			break;
		case X86Asm::CMP:
			setUse(l, {{ as.dst, as.src }});
			break;
	}
}


void DefUseAnalyser::setDef(uint l, Set<Operand>&& ops) {
	std::unordered_set<Operand> valid;

	for (auto& op : ops) {
		if (op.is(Operand::REG) || op.is(Operand::TMP))
			valid.insert(op);
	}

	def[l] = std::move(valid);
}


void DefUseAnalyser::setUse(uint l, Set<Operand>&& ops) {
	std::unordered_set<Operand> valid;

	for (auto& op : ops) {
		if (op.is(Operand::REG) || op.is(Operand::TMP))
			valid.insert(op);
	}

	use[l] = std::move(valid);
}


uint DefUseAnalyser::label2Line(Operand& op) {
	assert(op.is(Operand::LBL));
	assert(line.find(op.to_string()) != line.end());
	return line[op.to_string()];
}


void DefUseAnalyser::loadLabels() {
	uint l = 0;
	for(auto& as : code) {
		if (as.opcode == X86Asm::LBL)
			line[as.dst.to_string()] = nextLine(l);
		l++;
	}
}


uint DefUseAnalyser::nextLine(uint i) {
	while (++i < n) {
		if (code[i].opcode != X86Asm::LBL)
			break;
	}
	return i;
}


void DefUseAnalyser::run() {
	loadLabels();

	uint l = 0;
	for (auto& as : code)
		visit(as, l++);
}


Set<Operand>& DefUseAnalyser::getDef(uint l) {
	assert(l < n);
	return def[l];
}


Set<Operand>& DefUseAnalyser::getUse(uint l) {
	assert(l < n);
	return use[l];
}


Set<uint>& DefUseAnalyser::getSucc(uint l) {
	assert(l < n);
	return succ[l];
}
