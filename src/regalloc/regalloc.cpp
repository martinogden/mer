#include <unordered_map>
#include "counter.hpp"
#include "inst/operand.hpp"
#include "regalloc/regalloc.hpp"
#include "regalloc/ig-builder.hpp"


typedef std::unordered_map<Operand, uint> Colors;


Alloc::Alloc() :
	num_used_regs(0),
	num_spilled(0)
{}


Operand Alloc::lookup(const Operand& operand) const {
	switch (operand.getType()) {
		case Operand::REG:
		case Operand::TMP:
			assert(map.find(operand) != map.end());
			return map.at(operand);
		case Operand::IMM:
		case Operand::MEM:
		case Operand::LBL:
			return operand;
	}
}


void Alloc::assign(const Operand& src, const Operand& dst) {
	assert(map.find(src) == map.end());
	map[src] = dst;
}


// ir operands already assigned a register are precolored
Colors getPrecoloring(const InstFun& fun) {
	Colors colors;

	for (uint i=0; i<=MAX_REG; ++i)
		colors[static_cast<Reg>(i)] = i;

	return colors;
}


std::vector<Operand> mcs(IGPtr& G, Colors& precoloring) {
	uint n = G->numVertices();
	assert(n > 0);

	std::unordered_set<Operand> V = G->getVertices();

	std::vector<Operand> order(n);
	Counter<Operand> weights(V);

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
	}

	return colors;
}


Alloc toColoring(const Colors& colors) {
	Alloc alloc;
	uint offset = callerSaved.size() + 1;
	std::unordered_set<Reg> used_regs;

	for (auto const& pair : colors) {
		if (pair.second <= MAX_REG) {
			Reg reg = static_cast<Reg>(pair.second);
			alloc.assign(pair.first, reg);
			if (!pair.first.is(Operand::REG))
				used_regs.insert(reg);
		}
		else {
			alloc.assign(pair.first, Operand(Reg::RBP, -8 * (pair.second - offset)));
			alloc.num_spilled++;
		}
	}

	alloc.num_used_regs = used_regs.size();
	return alloc;
}


Alloc regAlloc(const InstFun& fun) {
	IGBuilder builder(fun);
	IGPtr IG = builder.run();

	Colors precoloring = getPrecoloring(fun);
	std::vector<Operand> order = mcs(IG, precoloring);
	Colors colors = greedyColor(IG, order, precoloring);

	return toColoring(colors);
}


InstFun regAssign(const InstFun& fun, const Alloc& alloc) {
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
					alloc.lookup(inst.getDst())
				);
				break;
			case 2:
				insts.emplace_back(
					opcode,
					alloc.lookup(inst.getDst()),
					alloc.lookup(inst.getSrc1())
				);
				break;
			case 3:
				insts.emplace_back(
					opcode,
					alloc.lookup(inst.getDst()),
					alloc.lookup(inst.getSrc1()),
					alloc.lookup(inst.getSrc2())
				);
				break;
			default:
				throw 1;  // TODO: we should never get here
		}
	}

	return {fun.id, fun.params, insts};
}
