#include "cst/cst.hpp"


void CSTVisitor::visit(FunDecl& decl) {
	for (auto const& param : decl.params)
		param->accept(*this);
}


void CSTVisitor::visit(FunDefn& defn) {
	defn.decl->accept(*this);
	defn.body->accept(*this);
}


void CSTVisitor::visit(StructDecl& decl) {}


void CSTVisitor::visit(StructDefn& defn) {
	for (auto const& field : defn.fields)
		field->accept(*this);
}


void CSTVisitor::visit(DeclStmt& stmt) {}


void CSTVisitor::visit(TypedefStmt& stmt) {}


void CSTVisitor::visit(IfStmt& stmt) {
	stmt.then->accept(*this);

	if (stmt.otherwise)
		stmt.otherwise->accept(*this);
}


void CSTVisitor::visit(WhileStmt& stmt) {
	stmt.body->accept(*this);
}


void CSTVisitor::visit(ForStmt& stmt) {
	if (stmt.init)
		stmt.init->accept(*this);

	if (stmt.step)
		stmt.step->accept(*this);
	stmt.body->accept(*this);
}


void CSTVisitor::visit(ReturnStmt& stmt) {}


void CSTVisitor::visit(BlockStmt& block) {
	for (auto const& stmt : block.statements)
		stmt->accept(*this);
}


void CSTVisitor::visit(ExprStmt& stmt) {}


void CSTVisitor::visit(AssignStmt& stmt) {}


void CSTVisitor::visit(PostOpStmt& stmt) {}
