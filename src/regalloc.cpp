#include <iostream>
#include <bitset>
#include <set>
#include <vector>
#include "ir.hpp"
#include "regalloc.hpp"


// TODO: make this unbounded
constexpr uint N = 64;  // max # of tmps
typedef std::bitset<N> bitset;
typedef std::vector<std::set<uint>> Table;
typedef std::unordered_map<Vertex, uint> Colors;


inline bitset use(Inst& inst) {
	bitset bits(0);

	if (inst.arity >= 3 && inst.s2.type != OpType::IMM)
		bits.set(inst.s2.value);

	if (inst.arity >= 2 && inst.s1.type != OpType::IMM)
		bits.set(inst.s1.value);

	switch (inst.opcode) {
		case OpCode::RET:
			bits.set(0);
			break;
		case OpCode::DIV:
			bits.set(0);  // %eax
			bits.set(3);  // %edx
			break;
		default:
			break;
	}

	return bits;
}


inline bitset def(Inst& inst) {
	bitset bits(0);

	if (inst.arity >= 1 && inst.dst.type != OpType::IMM)
		bits.set(inst.dst.value);

	switch (inst.opcode) {
		case OpCode::DIV:
			bits.set(0);  // %eax
			bits.set(3);  // %edx
			break;
		default:
			break;
	}

	return bits;
}


inline std::set<uint> toSet(bitset bits) {
	std::set<uint> set;

	for (uint i=0; i<N; ++i) {
		if (bits.test(i))
			set.insert(i);
	}

	return set;
}


inline Table toTable(std::vector<bitset> bitsets) {
	uint n = bitsets.size();
	Table table(n);

	for (uint i=0; i<n; ++i)
		table[i] = toSet(bitsets[i]);

	return table;
}


void printSet(std::set<uint> set) {
	for (uint n : set)
		std::cout << n << " ";
	std::cout << std::endl;
}


Table getLiveness(BasicBlock& bb) {
	uint n = bb.size();

	if (n < 1)
		return {};

	std::vector<bitset> liveness(n);

	liveness[n-1] = use(bb[n-1]);

	for (uint i=n-1; i-- > 0; ) {
		Inst& inst = bb[i];
		liveness[i] = use(inst) | (liveness[i+1] & ~def(inst));
	}

	return toTable(liveness);
}


// build interference graph (naive)
// Graph* getIG(Table& liveness) {
// 	Graph* G = new Graph();

// 	for (auto& row : liveness) {
// 		if (row.empty())
// 			continue;

// 		for (auto v : row)
// 			G->addVertex(v);

// 		for (auto it1=row.begin(); it1!=std::prev(row.end()); ++it1) {
// 			for (auto it2=std::next(it1); it2!=row.end(); ++it2)
// 				G->addEdge(*it1, *it2);
// 		}
// 	}

// 	return G;
// }


Graph* getIG(BasicBlock& bb, Table& liveness) {
	Graph* G = new Graph();

	for (auto& row : liveness) {
		for (auto u : row)
			G->addVertex(u);
	}

	for (int i=0; i<bb.insts.size()-1; ++i) {
		Inst& inst = bb.insts[i];
		uint x = inst.dst.value;

		if (inst.arity == 0)
			continue;

		for (auto u : liveness[i+1]) {
			if (x != u) {
				// special-case move instructions
				if (inst.opcode == OpCode::MOV) {
					uint y = inst.s1.value;
					if (u == y)
						continue;
				}

				G->addVertex(x);
				G->addEdge(x, u);
			}
		}
	}

	return G;
}


// ir operands already assigned a register are precolored
Colors getPrecoloring(BasicBlock& bb) {
	Colors colors;

	for (Inst& inst : bb.insts) {
		if (inst.arity >= 1 && inst.dst.type == OpType::REG)
			colors[inst.dst.value] = inst.dst.value;
		switch (inst.opcode) {
			case OpCode::DIV:
				colors[0] = 0;  // eax
				colors[3] = 3;  // edx
				break;
			default:
				break;
		}
	}

	return colors;
}


// loosely based on Python's collections.Counter
class Counter {
private:
	std::unordered_map<uint, uint> counts;

public:
	Counter(std::set<uint> keys) {
		for (uint key : keys)
			counts[key] = 0;
	}

	void incr(uint key) {
		assert(counts.find(key) != counts.end());
		counts[key]++;
	}

	void erase(uint key) {
		counts.erase(key);
	}

	uint getMostCommon() {
		assert (counts.size() > 0);

		auto& item = *counts.begin();
		uint maxCount = item.second;
		uint mostCommon = item.first;

		for (auto& item : counts) {
			if (item.second > maxCount) {
				maxCount = item.second;
				mostCommon = item.first;
			}
		}
		return mostCommon;
	}
};


std::vector<Vertex> mcs(Graph* G, Colors& precoloring) {
	uint n = G->numVertices();
	assert(n > 1);

	std::set<Vertex> V = G->getVertices();

	std::vector<Vertex> order(n);
	Counter weights(V);

	for (auto& c : precoloring) {
		Vertex u = c.first;
		for (Vertex v : G->getNeighbors(u))
			weights.incr(v);
	}

	for (uint i=0; i<n; ++i) {
		Vertex u = weights.getMostCommon();

		for (Vertex v : G->getNeighbors(u)) {
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
inline uint mex(std::set<uint>& set) {
	uint m = 0;
	while (set.find(m) != set.end())
		m++;
	return m;
}


inline uint leastUnusedColor(std::set<Vertex>& vertices, Colors& colors) {
	std::set<uint> usedColors;
	for (uint v : vertices) {
		if (colors.find(v) != colors.end())
			usedColors.insert(colors[v]);
	}

	return mex(usedColors);
}


Colors greedyColor(Graph* G, std::vector<uint>& order, Colors& precoloring) {
	Colors colors;

	// precolor vertices
	for (auto& item : precoloring)
		colors[item.first] = item.second;

	for (uint u : order) {
		// ignore precolored vertices
		if (colors.find(u) != colors.end())
			continue;

		// color vertex using least unused color among neighbors
		std::set<Vertex> neighbors = G->getNeighbors(u);
		colors[u] = leastUnusedColor(neighbors, colors);
	}

	return colors;
}


inline Operand alloc(Operand& operand, Colors& coloring) {
	if (operand.type == OpType::TMP)
		return { OpType::REG, static_cast<int>(coloring[operand.value]) };
	else
		return operand;
}


inline void alloc(std::vector<Inst>& insts, Colors& coloring) {
	for (Inst& inst : insts) {
		if (inst.arity >= 1)
			inst.dst = alloc(inst.dst, coloring);

		if (inst.arity >= 2)
			inst.s1 = alloc(inst.s1, coloring);

		if (inst.arity >= 3)
			inst.s2 = alloc(inst.s2, coloring);
	}
}


std::set<Reg> alloc(BasicBlock& bb) {
	Table liveness = getLiveness(bb);
	Graph* IG = getIG(bb, liveness);
	Colors precoloring = getPrecoloring(bb);
	std::vector<Vertex> order = mcs(IG, precoloring);
	Colors coloring = greedyColor(IG, order, precoloring);
	alloc(bb.insts, coloring);

	std::set<Reg> regs;
	for (auto& item : coloring)
		regs.insert( static_cast<Reg>(item.second) );

	return regs;
}
