#include <iostream>
#include "translator/utils.hpp"
#include "translator/translator.hpp"
#include "translator/bool-expr.hpp"


Translator::Translator(Generator& gen) :
	retval(nullptr),
	returnLabel(""),
	returnTmp(""),
	tr(gen)
{}


void Translator::ret(IRTCmdPtr val) {
	retval = std::move(val);
}


void Translator::ret(std::vector<IRTCmdPtr> val) {
	retval = concat( std::move(val) );
}


IRTFunPtr Translator::get(FunNodePtr& node) {
	std::vector<std::string> params;
	for (auto const& param : node->params)
		params.push_back(param.name);

	node->accept(*this);
	return std::make_unique<IRTFun>(node->id, params, std::move(retval));
}


IRTCmdPtr Translator::get(ASTNodePtr& node) {
	node->accept(*this);
	return std::move(retval);
}


CmdExprPtr Translator::get(ExprPtr expr) {
	return tr.get( std::move(expr) );
}


void Translator::visit(FunNode& node) {
	returnLabel = tr.freshLabel();
	returnTmp = tr.freshTmp();

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( get(node.body) );
	cmds.push_back( std::make_unique<LabelCmd>(returnLabel) );
	cmds.push_back( std::make_unique<ReturnCmd>(std::make_unique<VarExpr>(returnTmp)) );

	ret( std::move(cmds) );
}


void Translator::visit(AssignNode& node) {
	CmdExprPtr e = get( std::move(node.expr) );

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( std::move(e->cmd) );
	cmds.push_back( std::make_unique<AssignCmd>(node.id, std::move(e->expr)) );

	ret( std::move(cmds) );
}


void Translator::visit(ReturnNode& node) {
	if (node.expr) {
		CmdExprPtr e = get( std::move(node.expr) );

		std::vector<IRTCmdPtr> cmds;
		cmds.push_back( std::move(e->cmd) );
		cmds.push_back( std::make_unique<AssignCmd>(returnTmp, std::move(e->expr)) );
		cmds.push_back( std::make_unique<GotoCmd>(returnLabel) );

		ret( std::move(cmds) );
	}
	else
		ret(std::make_unique<GotoCmd>(returnLabel));
}


void Translator::visit(NopNode& node) {
	ret( std::make_unique<NopCmd>() );
}


void Translator::visit(DeclNode& node) {
	ret( get(node.scope) );
}


void Translator::visit(SeqNode& node) {
	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( get(node.head) );
	cmds.push_back( get(node.rest) );
	ret( std::move(cmds) );
}


void Translator::visit(IfNode& node) {
	BoolExprTranslator btr(tr);

	std::string l1 = tr.freshLabel();
	std::string l2 = tr.freshLabel();
	std::string l3 = tr.freshLabel();

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( btr.get(std::move(node.cond), l1, l2) );

	cmds.push_back( std::make_unique<LabelCmd>(l1) );
	cmds.push_back( get(node.then) );
	cmds.push_back( std::make_unique<GotoCmd>(l3) );

	cmds.push_back( std::make_unique<LabelCmd>(l2) );
	cmds.push_back( get(node.otherwise) );
	cmds.push_back( std::make_unique<GotoCmd>(l3) );

	cmds.push_back( std::make_unique<LabelCmd>(l3) );

	ret( std::move(cmds) );
}


void Translator::visit(WhileNode& node) {
	BoolExprTranslator btr(tr);

	std::string l1 = tr.freshLabel();
	std::string l2 = tr.freshLabel();
	std::string l3 = tr.freshLabel();

	std::vector<IRTCmdPtr> cmds;
	cmds.push_back( std::make_unique<LabelCmd>(l1) );
	cmds.push_back( btr.get(std::move(node.cond), l2, l3) );

	cmds.push_back( std::make_unique<LabelCmd>(l2) );
	cmds.push_back( get(node.body) );
	cmds.push_back( std::make_unique<GotoCmd>(l1) );

	cmds.push_back( std::make_unique<LabelCmd>(l3) );

	ret( std::move(cmds) );
}


void Translator::visit(ExprNode& node) {
	// Lift out side-effects and throw away expression
	CmdExprPtr e = get( std::move(node.expr) );
	ret( std::move(e->cmd) );
}
