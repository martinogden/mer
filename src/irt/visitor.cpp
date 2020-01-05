#include "irt/visitor.hpp"
#include "irt/irt.hpp"


void IRTVisitor::visit(SeqCmd& cmd) {}
void IRTVisitor::visit(NopCmd& cmd) {}
void IRTVisitor::visit(CallCmd& cmd) {}
void IRTVisitor::visit(AssignCmd& cmd) {}
void IRTVisitor::visit(EffAssignCmd& cmd) {}
void IRTVisitor::visit(LoadCmd& cmd) {}
void IRTVisitor::visit(StoreCmd& cmd) {}
void IRTVisitor::visit(LabelCmd& cmd) {}
void IRTVisitor::visit(IfCmd& cmd) {}
void IRTVisitor::visit(GotoCmd& cmd) {}
void IRTVisitor::visit(ReturnCmd& cmd) {}

void IRTVisitor::visit(CmdExpr& e) {}
void IRTVisitor::visit(IRTIntExpr& e) {}
void IRTVisitor::visit(IRTIdExpr& e) {}
void IRTVisitor::visit(IRTBinaryExpr& e) {}
void IRTVisitor::visit(IRTMemExpr& e) {}
