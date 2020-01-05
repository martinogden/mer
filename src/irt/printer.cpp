#include <sstream>
#include "irt/printer.hpp"
#include "irt/containers.hpp"
#include "print-utils.hpp"


inline std::string to_string(BinOp op) {
	std::stringstream buffer;
	buffer << op;
	return buffer.str();
}


void IRTPrinter::ret(std::string str) {
	retval = std::move(str);
}


std::string IRTPrinter::get(IRTFun& fun) {
	std::string label = fun.id + "(" + join(fun.params, ", ") + "):\n";
	return label + get(fun.body);
}


std::string IRTPrinter::get(IRTCmdPtr& cmd) {
	cmd->accept(*this);
	return retval;
}


std::string IRTPrinter::get(IRTExprPtr& e) {
	e->accept(*this);
	return retval;
}


void IRTPrinter::visit(SeqCmd& cmd) {
	ret( get(cmd.head) + get(cmd.rest) );
}


void IRTPrinter::visit(NopCmd& cmd) {
	ret("");
}


void IRTPrinter::visit(CallCmd& cmd) {
	ret( "\t" + cmd.var + " <- call " + cmd.label + "(" + join(cmd.args, ", ") + ")\n" );
}


void IRTPrinter::visit(AssignCmd& cmd) {
	ret( "\t" + cmd.var + " <- " + get(cmd.value) + "\n" );
}


void IRTPrinter::visit(LoadCmd& cmd) {
	ret( "\t" + cmd.dst + " <- MEM[" + get(cmd.src) + "]\n" );
}


void IRTPrinter::visit(StoreCmd& cmd) {
	ret( "\tMEM[" + get(cmd.dst) + "] <- " + cmd.src + "\n" );
}


void IRTPrinter::visit(EffAssignCmd& cmd) {
	ret("\t" + cmd.var + " <- " + get(cmd.left)\
		+ " " + to_string(cmd.op)\
		+ " " + get(cmd.right) + "\n"
	);
}


void IRTPrinter::visit(LabelCmd& cmd) {
	ret ( cmd.name + ":\n" );
}


void IRTPrinter::visit(IfCmd& cmd) {
	ret("\tif ("  + get(cmd.cmp.left) + " "\
		+ to_string(cmd.cmp.op)\
		+ " " + get(cmd.cmp.right) +  ") "\
		+ cmd.then + " else "\
		+ cmd.otherwise + "\n"
	);
}


void IRTPrinter::visit(GotoCmd& cmd) {
	ret( "\tgoto " + cmd.label + "\n" );
}


void IRTPrinter::visit(ReturnCmd& cmd) {
	ret( "\treturn " + get(cmd.expr) + "\n" );
}


void IRTPrinter::visit(CmdExpr& e) {
	ret( get(e.cmd) + get(e.expr) );
}


void IRTPrinter::visit(IRTIntExpr& e) {
	ret( std::to_string(e.value) );
}


void IRTPrinter::visit(IRTIdExpr& e) {
	ret( e.name );
}


void IRTPrinter::visit(IRTBinaryExpr& e) {
	ret( "(" + get(e.left) + " " + to_string(e.op) + " " + get(e.right) + ")");
}


void IRTPrinter::visit(IRTMemExpr& e) {
	ret(get(e.address) );
}
