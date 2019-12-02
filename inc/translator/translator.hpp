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

	void visit(FunNode*);
	void visit(AssignNode*);
	void visit(IfNode*);
	void visit(WhileNode*);
	void visit(ReturnNode*);
	void visit(NopNode*);
	void visit(SeqNode*);
	void visit(DeclNode*);
	void visit(ExprNode*);

	IRTFun* get(FunNode*);
};
