#pragma once
#include <vector>
#include "ast/visitor.hpp"
#include "ast/ast.hpp"
#include "irt/irt.hpp"
#include "irt/containers.hpp"
#include "expr.hpp"
#include "generator.hpp"


// translate: AST -> IRT
class Translator : public ASTVisitor {
private:
	IRTCmdPtr retval;
	std::string returnLabel;
	std::string returnTmp;

	ExprTranslator tr;

	void ret(IRTCmdPtr);
	void ret(std::vector<IRTCmdPtr>);
	CmdExprPtr get(ExprPtr);
	IRTCmdPtr get(ASTNodePtr&);

public:
	Translator(Generator& gen, Map<IRTStruct>& structs);

	void visit(FunNode&) override;
	void visit(AssignNode&) override;
	void visit(IfNode&) override;
	void visit(WhileNode&) override;
	void visit(ReturnNode&) override;
	void visit(NopNode&) override;
	void visit(SeqNode&) override;
	void visit(DeclNode&) override;
	void visit(ExprNode&) override;

	IRTFun get(FunNodePtr&);
};
