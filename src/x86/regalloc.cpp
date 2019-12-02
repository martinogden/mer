#include <unordered_map>
#include "graph.hpp"
#include "operand.hpp"
#include "x86/ig-builder.hpp"
#include "x86/regalloc.hpp"


typedef std::unordered_map<Operand, uint> Colors;


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
		assert(colors[u] <= MAX_REG-2 && "TODO: register spilling");
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
