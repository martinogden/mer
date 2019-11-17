#include "irt-visitor.hpp"
#include "irt.hpp"


void IRTVisitor::visit(SeqCmd* cmd) {}
void IRTVisitor::visit(NopCmd* cmd) {}
void IRTVisitor::visit(AssignCmd* cmd) {}
void IRTVisitor::visit(EffAssignCmd* cmd) {}
void IRTVisitor::visit(LabelCmd* cmd) {}
void IRTVisitor::visit(IfCmd* cmd) {}
void IRTVisitor::visit(GotoCmd* cmd) {}
void IRTVisitor::visit(ReturnCmd* cmd) {}
void IRTVisitor::visit(CmdExpr* e) {}
void IRTVisitor::visit(IntExpr* e) {}
void IRTVisitor::visit(VarExpr* e) {}
void IRTVisitor::visit(PairExpr* e) {}
