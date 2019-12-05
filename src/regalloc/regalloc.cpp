#include <unordered_map>
#include "graph.hpp"
#include "inst/operand.hpp"
#include "regalloc/regalloc.hpp"
#include "regalloc/ig-builder.hpp"


typedef std::unordered_map<Operand, uint> Colors;


// ir operands already assigned a register are precolored
Colors getPrecoloring(InstFun& fun) {
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
	Counter(const std::unordered_set<Operand>& keys) {
		for (auto const& key : keys)
			counts[key] = 0;
	}

	void incr(const Operand& key) {
		assert(counts.find(key) != counts.end());
		counts[key]++;
	}

	void erase(const Operand& key) {
		counts.erase(key);
	}

	Operand getMostCommon() {
		assert (!counts.empty());

		auto const& item = *counts.begin();
		uint maxCount = item.second;
		Operand mostCommon = item.first;

		for (auto const& item : counts) {
			if (item.second > maxCount) {
				maxCount = item.second;
				mostCommon = item.first;
			}
		}
		return mostCommon;
	}
};


std::vector<Operand> mcs(IGPtr& G, Colors& precoloring) {
	uint n = G->numVertices();
	assert(n > 0);

	std::unordered_set<Operand> V = G->getVertices();

	std::vector<Operand> order(n);
	Counter weights(V);

	for (auto& c : precoloring) {
		Operand u = c.first;
		for (auto const& v : G->getAdj(u))
			weights.incr(v);
	}

	for (uint i=0; i<n; ++i) {
		Operand u = weights.getMostCommon();

		for (auto const& v : G->getAdj(u)) {
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
	for (auto const& v : vertices) {
		if (colors.find(v) != colors.end())
			usedColors.insert(colors[v]);
	}

	return mex(usedColors);
}


Colors greedyColor(IGPtr& G, std::vector<Operand>& order, Colors& precoloring) {
	Colors colors;

	// precolor vertices
	for (auto& item : precoloring)
		colors[item.first] = item.second;

	for (auto const& u : order) {
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


Alloc toColoring(const Colors& colors) {
	Alloc regs;

	for (auto const& pair : colors) {
		assert( pair.second <= MAX_REG && "invalid allocation" );
		regs[pair.first] = static_cast<Reg>(pair.second);
	}

	return regs;
}


Alloc regAlloc(InstFun& fun) {
	IGBuilder builder(fun);
	IGPtr IG = builder.run();

	Colors precoloring = getPrecoloring(fun);
	std::vector<Operand> order = mcs(IG, precoloring);
	Colors colors = greedyColor(IG, order, precoloring);

	return toColoring(colors);
}


inline Operand assign(const Operand& op, Alloc& regs) {
	switch (op.getType()) {
		case Operand::REG:
		case Operand::TMP:
			return regs[op];
		default:
			return op;
	}
};


InstFun regAssign(InstFun& fun, Alloc& regs) {
	std::vector<Inst> insts;

	for (auto &inst : fun.insts) {
		Inst::OpCode opcode = inst.getOpcode();

		if (opcode == Inst::LBL) {
			insts.push_back(inst);
			continue;
		}

		switch (inst.getParity()) {
			case 0:
				insts.push_back(inst);
				break;
			case 1:
				insts.emplace_back(
					opcode,
					assign(inst.getDst(), regs)
				);
				break;
			case 2:
				insts.emplace_back(
					opcode,
					assign(inst.getDst(), regs),
					assign(inst.getSrc1(), regs)
				);
				break;
			case 3:
				insts.emplace_back(
					opcode,
					assign(inst.getDst(), regs),
					assign(inst.getSrc1(), regs),
					assign(inst.getSrc2(), regs)
				);
				break;
			default:
				throw 1;  // TODO: we should never get here
		}
	}

	return {fun.id, fun.params, insts};
}
