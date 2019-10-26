#include <sstream>
#include "compiler.hpp"
#include "parser.hpp"
#include "semant.hpp"
#include "codegen.hpp"
#include "regalloc.hpp"
#include "instsel.hpp"


std::string join(std::vector<std::string>& lines) {
	std::stringstream buf;
	for (std::string& line : lines)
		buf << line << "\n";
	return buf.str();
}


std::string compile(std::string src) {
	Parser parser(src);
	Prog* program = parser.run();

	if (!program)
		return join( parser.getErrors() );

	Semant semant(program);
	bool is_valid = semant.run();

	if (!is_valid)
		return join( semant.getErrors() );


	CodeGen codegen(program);
	std::vector<Inst>& insts = codegen.run();


	BasicBlock bb = toBasicBlock(insts);
	std::set<Reg> regs = alloc(bb);

	std::vector<std::string> as = instsel(bb.insts, regs);
	return join(as);
}
