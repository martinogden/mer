#include <iostream>
#include "translator/utils.hpp"
#include "translator/translator.hpp"
#include "translator/bool-expr.hpp"


Translator::Translator(Generator& gen) :
	tr(gen)
{}


void Translator::ret(IRTCmd* val) {
	retval = val;
}


void Translator::ret(std::vector<IRTCmd*> val) {
	retval = concat(val);
}


IRTCmd* Translator::get(ASTNode* node) {
	node->accept(*this);
	return retval;
}


CmdExpr* Translator::get(Expr* expr) {
	return tr.get(expr);
}


void Translator::visit(AssignNode* node) {
	CmdExpr* e = get(node->expr);
	ret({ e->cmd, new AssignCmd(node->id, e->expr) });
}


void Translator::visit(ReturnNode* node) {
	CmdExpr* e = get(node->expr);
	ret({ e->cmd, new ReturnCmd(e->expr) });
}


void Translator::visit(NopNode* node) {
	ret( new NopCmd() );
}


void Translator::visit(DeclNode* node) {
	ret( get(node->scope) );
}


void Translator::visit(SeqNode* node) {
	ret({ get(node->head), get(node->rest) });
}


void Translator::visit(IfNode* node) {
	BoolExprTranslator btr(tr);

	std::string l1 = tr.freshLabel();
	std::string l2 = tr.freshLabel();
	std::string l3 = tr.freshLabel();

	ret({
		btr.get(node->cond, l1, l2),
		new LabelCmd(l1), get(node->then), new GotoCmd(l3),
		new LabelCmd(l2), get(node->otherwise), new GotoCmd(l3),
		new LabelCmd(l3)
	});
}


void Translator::visit(WhileNode* node) {
	BoolExprTranslator btr(tr);

	std::string l1 = tr.freshLabel();
	std::string l2 = tr.freshLabel();
	std::string l3 = tr.freshLabel();

	ret({
		new LabelCmd(l1), btr.get(node->cond, l2, l3),
		new LabelCmd(l2), get(node->body), new GotoCmd(l1),
		new LabelCmd(l3),
	});
}


void Translator::visit(ExprNode* node) {
	// Lift out side-effects and throw away expression
	CmdExpr* e = get(node->expr);
	ret(e->cmd);
}
