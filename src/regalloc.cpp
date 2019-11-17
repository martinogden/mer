#include "regalloc.hpp"
#include "set.hpp"
#include "graph.hpp"
#include "operand.hpp"
#include "irt-visitor.hpp"
#include "irt.hpp"
#include "x86asm.hpp"


typedef std::unordered_map<Operand, uint> Colors;
constexpr uint MAX_REG = 15;


// class DefUseGenerator {
// private:
// 	uint l;
// 	std::vector<Inst>& insts;
// 	std::unordered_map<std::string, uint> labelIndices;

// 	void setLabelIndices() {
// 		uint j = 0;

// 		for (Inst& inst : insts) {
// 			if ( inst.is(Inst::LBL) )
// 				labelIndices[inst.getDst().getLabel()] = j+1;
// 			++j;
// 		}
// 	}

// public:
// 	std::vector<Set<Operand>> def;
// 	std::vector<Set<Operand>> use;
// 	std::vector<Set<uint>> succ;

// 	DefUseGenerator(std::vector<Inst>& insts) :
// 		l(0),
// 		insts(insts),
// 		def(insts.size()),
// 		use(insts.size()),
// 		succ(insts.size())
// 	{}

// 	void run() {
// 		setLabelIndices();

// 		for (Inst& inst : insts) {
// 			visit(inst);
// 			++l;
// 		}
// 	}

// 	void visit(Inst& inst) {
// 		if (inst.is(Inst::DIV) || inst.is(Inst::MOD))
// 			return visitDiv(inst.getDst(), inst.getSrc1(), inst.getSrc2());

// 		switch (inst.getType()) {
// 			case Inst::Type::BINARY:
// 				return visitBinary(inst.getDst(), inst.getSrc1(), inst.getSrc2());
// 			case Inst::Type::UNARY:
// 				return visitUnary(inst.getDst(), inst.getSrc1());
// 			case Inst::Type::RET:
// 				return visitRet(inst.getDst());
// 			case Inst::Type::JMP:
// 				return visitJmp(inst.getDst());
// 			case Inst::Type::CJMP:
// 				return visitCJmp(inst.getDst(), inst.getSrc1(), inst.getSrc2());
// 			case Inst::Type::LABEL:
// 				return visitLabel(inst.getDst());
// 		}
// 	}

// 	void visitDiv(Operand&& dst, Operand&& src1, Operand&& src2) {
// 		def[l] = {{ Reg::EAX, Reg::EDX }};
// 		use[l] = {{ Reg::EAX, Reg::EDX, src2 }};
// 		succ[l] = {{ l+1 }};
// 	}

// 	void visitBinary(Operand&& dst, Operand&& src1, Operand&& src2) {
// 		def[l] = {{ dst }};

// 		if (src1.is(Operand::TMP))
// 			use[l] = {{ src1 }};

// 		if (src2.is(Operand::TMP))
// 			use[l] = use[l] | Set<Operand>({ src2 });

// 		succ[l] = {{ l+1 }};
// 	}

// 	void visitUnary(Operand&& dst, Operand&& src) {
// 		def[l] = {{ dst }};

// 		if (src.is(Operand::TMP))
// 			use[l] = {{ src }};

// 		succ[l] = {{ nextInstIdx(l) }};
// 	}

// 	void visitRet(Operand&& opnd) {
// 		if (opnd.is(Operand::TMP))
// 			use[l] = {{ opnd }};
// 	}

// 	void visitJmp(Operand&& opnd) {
// 		succ[l] = {{ labelIndices[opnd.getLabel()] }};
// 	}

// 	void visitCJmp(Operand&& opnd, Operand&& t, Operand&& f) {
// 		if (opnd.is(Operand::TMP))
// 			use[l] = {{ opnd }};

// 		succ[l] = {{
// 			labelIndices[t.getLabel()],
// 			labelIndices[f.getLabel()],
// 		}};
// 	}

// 	void visitLabel(Operand&& lbl) {}

// 	uint nextInstIdx(uint i) {
// 		while (++i < insts.size()) {
// 			if (!insts[i].is(Inst::LBL))
// 				break;
// 		}
// 		return i;
// 	}
// };


// class LivenessGenerator {
// private:
// 	std::vector<Set<Operand>> live;

// 	uint l;
// 	bool changed = true;
// 	std::vector<Inst>& insts;
// 	DefUseGenerator gen;

// 	void visit(Inst& inst) {
// 		update(l, gen.use[l]);

// 		for (uint i : gen.succ[l]) {
// 			if (i < insts.size())
// 				update(l, live[i] - gen.def[l]);
// 		}
// 	}

// 	void update(uint i, Set<Operand> opnds) {
// 		if ( !(opnds - live[i]).empty() ) {
// 			live[i] = live[i] | opnds;
// 			changed = true;
// 		}
// 	}

// public:
// 	LivenessGenerator(std::vector<Inst>& insts) :
// 		live(insts.size()),
// 		l(0),
// 		insts(insts),
// 		gen(insts)
// 	{}

// 	void run() {
// 		gen.run();

// 		// drive live towards fixed point
// 		while (changed) {
// 			changed = false;
// 			l = 0;

// 			for (Inst& inst : insts) {
// 				visit(inst);
// 				++l;
// 			}
// 		}
// 	}

// 	Set<Operand>& get(uint i) {
// 		assert(i < insts.size());
// 		return live[i];
// 	}
// };


class IGBuilder {
private:
	std::vector<X86Asm>& code;
	LivenessAnalyser liveness;
	Graph<Operand>* G;

public:
	IGBuilder(std::vector<X86Asm>& code) :
		code(code),
		liveness(code),
		G(new Graph<Operand>())
	{}

	Graph<Operand>* run() {
		liveness.run();

		for (uint i=0; i<MAX_REG; ++i)
			G->addVertex(static_cast<Reg>(i));

		// DEBUG
		// uint i = 0;
		// for (auto& as : code) {
		// 	if (as.opcode == X86Asm::LBL)
		// 		std::cout << as << std::endl;
		// 	else
		// 		std::cout << i << ' ' << liveness.get(i) << std::endl;
		// 	i++;
		// }

		uint l = 0;
		for (auto& as : code)
			visit(as, l++);

		// std::cout << *G << std::endl;
		return G;
	}

	void visit(X86Asm& as, uint l) {
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

	void visitNullary(X86Asm& as, uint l) {
		Set<Operand>& live = liveness.get(l);
		addVertices(live);
		addEdges(live);
	}

	void visitUnary(X86Asm& as, uint l) {
		Set<Operand>& live = liveness.get(l);
		addVertices(live | Set<Operand>({ as.dst }));
		addEdges(live);
	}

	void visitBinary(X86Asm& as, uint l) {
		Set<Operand>& live = liveness.get(l);
		addVertices(live | Set<Operand>({ as.dst, as.src }));
		addEdges(live);
	}

	void visitMov(X86Asm& as, uint l) {
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

	void addVertices(Set<Operand> ops) {
		for (const auto& op : ops) {
			if (op.is(Operand::REG) || op.is(Operand::TMP))
				G->addVertex(op);
		}
	}

	void addEdges(Set<Operand>& live) {
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
};


// ir operands already assigned a register are precolored
Colors getPrecoloring(std::vector<X86Asm>& code) {
	Colors colors;

	for (uint i=0; i<=MAX_REG; ++i)
		colors[static_cast<Reg>(i)] = i;

	return colors;
}


// loosely based on Python's collections.Counter
class Counter {
private:
	std::unordered_map<Operand, uint> counts;

public:
	Counter(std::unordered_set<Operand> keys) {
		for (auto key : keys)
			counts[key] = 0;
	}

	void incr(Operand key) {
		assert(counts.find(key) != counts.end());
		counts[key]++;
	}

	void erase(Operand key) {
		counts.erase(key);
	}

	Operand getMostCommon() {
		assert (counts.size() > 0);

		auto& item = *counts.begin();
		uint maxCount = item.second;
		Operand mostCommon = item.first;

		for (auto& item : counts) {
			if (item.second > maxCount) {
				maxCount = item.second;
				mostCommon = item.first;
			}
		}
		return mostCommon;
	}
};


std::vector<Operand> mcs(Graph<Operand>* G, Colors& precoloring) {
	uint n = G->numVertices();
	assert(n > 0);

	std::unordered_set<Operand> V = G->getVertices();

	std::vector<Operand> order(n);
	Counter weights(V);

	for (auto& c : precoloring) {
		Operand u = c.first;
		for (auto v : G->getAdj(u))
			weights.incr(v);
	}

	for (uint i=0; i<n; ++i) {
		Operand u = weights.getMostCommon();

		for (auto v : G->getAdj(u)) {
			if (V.find(v) != V.end())
				weights.incr(v);
		}

		V.erase(u);
		weights.erase(u);
		order[i] = u;
	}

	return order;
}


// find minimum excluded elmt
inline uint mex(std::unordered_set<uint>& set) {
	uint m = 0;
	while (set.find(m) != set.end())
		m++;
	return m;
}


inline uint leastUnusedColor(std::unordered_set<Operand>& vertices, Colors& colors) {
	std::unordered_set<uint> usedColors;
	for (auto v : vertices) {
		if (colors.find(v) != colors.end())
			usedColors.insert(colors[v]);
	}

	return mex(usedColors);
}


Colors greedyColor(Graph<Operand>* G, std::vector<Operand>& order, Colors& precoloring) {
	Colors colors;

	// precolor vertices
	for (auto& item : precoloring)
		colors[item.first] = item.second;

	for (auto u : order) {
		// ignore precolored vertices
		if (colors.find(u) != colors.end())
			continue;

		// color vertex using least unused color among neighbors
		std::unordered_set<Operand> adj = G->getAdj(u);
		colors[u] = leastUnusedColor(adj, colors);
	}

	return colors;
}


Alloc toColoring(Colors colors) {
	Alloc regs;

	for (const auto& pair : colors) {
		assert( pair.second <= MAX_REG && "invalid allocation" );
		regs[pair.first] = static_cast<Reg>(pair.second);
	}

	return regs;
}


std::string printGraph(Graph<Operand>* G, std::unordered_map<Operand, uint>& coloring);


Alloc regAlloc(std::vector<X86Asm>& code) {
	IGBuilder builder(code);
	Graph<Operand>* IG = builder.run();

	Colors precoloring = getPrecoloring(code);
	std::vector<Operand> order = mcs(IG, precoloring);
	Colors colors = greedyColor(IG, order, precoloring);

	// std::cout << printGraph(IG, colors);
	return toColoring(colors);
}


inline Operand assign(Operand& op, Alloc& regs) {
	switch (op.getType()) {
		case Operand::REG:
		case Operand::TMP:
			return regs[op];
		default:
			return op;
	}
};


std::vector<X86Asm> regAssign(std::vector<X86Asm>& code, Alloc& regs) {
	std::vector<X86Asm> out;

	for (auto& as : code) {
		if (as.opcode == X86Asm::LBL) {
			out.push_back(as);
			continue;
		}

		switch (as.parity) {
			case 0:
				out.push_back(as);
				break;
			case 1:
				out.push_back({ as.opcode, assign(as.dst, regs) });
				break;
			case 2:
				out.push_back({ as.opcode, assign(as.dst, regs), assign(as.src, regs) });
				break;
			default:
				throw 1;  // TODO: we should never get here
		}
	}

	return out;
}
