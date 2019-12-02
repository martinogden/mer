#pragma once
#include <vector>
#include "ast-visitor.hpp"
#include "ast.hpp"
#include "irt.hpp"
#include "translator/expr.hpp"
#include "generator.hpp"


// translate: AST -> IRT
class Translator : public ASTVisitor {
private:
	IRTCmd* retval;
	std::string returnLabel;
	std::string returnTmp;

	ExprTranslator tr;

	void ret(IRTCmd*);
	void ret(std::vector<IRTCmd*>);
	CmdExpr* get(Expr*);
	IRTCmd* get(ASTNode*);

public:
	Translator(Generator& gen);

	void visit(FunNode*) override;
	void visit(AssignNode*) override;
	void visit(IfNode*) override;
	void visit(WhileNode*) override;
	void visit(ReturnNode*) override;
	void visit(NopNode*) override;
	void visit(SeqNode*) override;
	void visit(DeclNode*) override;
	void visit(ExprNode*) override;

	IRTFun* get(FunNode*);
};
