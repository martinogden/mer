#include "regalloc/def-use-analyser.hpp"


DefUseAnalyser::DefUseAnalyser(const InstFun& fun) :
		n(fun.insts.size()),
		fun(fun),
		def(n),
		use(n),
		succ(n)
{}


Set<Operand> getNRegs(uint n) {
	assert(n < 7);
	std::unordered_set<Operand> regs;

	for (uint i=1; i<=n; ++i)
		regs.insert( static_cast<Reg>(i) );

	return std::move(regs);
}


void DefUseAnalyser::visit(const Inst& inst, uint l) {
	succ[l] = {{ nextLine(l) }};

	switch (inst.getOpcode()) {
		case Inst::LBL:
			break;

		case Inst::ADD:
		case Inst::SUB:
		case Inst::MUL:
		case Inst::AND:
		case Inst::OR:
		case Inst::XOR:
			setDef(l, {{ inst.getDst() }});
			setUse(l, {{ inst.getSrc1(), inst.getSrc2() }});
			break;
		case Inst::DIV:
		case Inst::MOD:
			setDef(l, {{ Reg::RAX, Reg::RDX, inst.getDst() }});
			setUse(l, {{
				Reg::RAX, Reg::RDX, inst.getSrc1(), inst.getSrc2()
			}});
			break;
		case Inst::MOV: {

			if (inst.getDst().is(Operand::MEM)) {
				setDef(l, {});
				setUse(l, {{inst.getDst(), inst.getSrc1()}});
			}
			else {
				setDef(l, {{inst.getDst()}});
				setUse(l, {{inst.getSrc1()}});
			}
			break;

		}
		case Inst::JMP:
			succ[l] = Set<uint>({ label2Line(inst.getDst()) });
			break;
		case Inst::JEQ:
		case Inst::JNE:
		case Inst::JLT:
		case Inst::JLE:
		case Inst::JGT:
		case Inst::JGE:
			setUse(l, {{ inst.getDst() }});
			succ[l] = succ[l] | Set<uint>({
				label2Line(inst.getSrc1()), label2Line(inst.getSrc2()),
			});
			break;
		case Inst::RET:
			setDef(l, {{ Reg::RAX }});
			setUse(l, {{ inst.getDst() }});
			succ[l] = {};
			break;
		case Inst::ENTER:
			setDef(l, std::unordered_set<Operand>(
			       fun.params.begin(), fun.params.end()));
			break;
		case Inst::CALL:
			// TODO remove for void functions
			setDef(l, {{ inst.getDst(), Reg::RAX }});
			setUse(l, getNRegs( inst.getSrc2().getImm() ));
			break;
		case Inst::ARG:
			setDef(l, {{ static_cast<Reg>(inst.getDst().getImm()) }});
			setUse(l, {{ inst.getSrc1() }});
			break;
	}
}


void DefUseAnalyser::setDef(uint l, Set<Operand>&& ops) {
	std::unordered_set<Operand> valid;

	for (auto& op : ops) {
		if (op.is(Operand::REG) || op.is(Operand::TMP))
			valid.insert(op);
		if (op.is(Operand::MEM))
			valid.insert(op.getMemOperand());
	}

	def[l] = std::move(valid);
}


void DefUseAnalyser::setUse(uint l, Set<Operand>&& ops) {
	std::unordered_set<Operand> valid;

	for (auto& op : ops) {
		if (op.is(Operand::REG) || op.is(Operand::TMP))
			valid.insert(op);
		if (op.is(Operand::MEM))
			valid.insert(op.getMemOperand());
	}

	use[l] = std::move(valid);
}


uint DefUseAnalyser::label2Line(const Operand& op) {
	assert(op.is(Operand::LBL));
	assert(line.find(op.to_string()) != line.end());
	return line[op.to_string()];
}


void DefUseAnalyser::loadLabels() {
	uint l = 0;
	for(auto& inst : fun.insts) {
		if ( inst.is(Inst::LBL) )
			line[inst.getDst().to_string()] = nextLine(l);
		l++;
	}
}


uint DefUseAnalyser::nextLine(uint i) {
	while (++i < n) {
		if ( !fun.insts[i].is(Inst::LBL) )
			break;
	}
	return i;
}


void DefUseAnalyser::run() {
	loadLabels();

	uint l = 0;
	for (const auto& as : fun.insts)
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
