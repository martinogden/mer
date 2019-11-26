#include "elaborator.hpp"


Elaborator::Elaborator(ParseTree* tree) :
	tree(tree)
{}


ASTNode* Elaborator::run() {
	return get(tree->block);
};


void Elaborator::visit(BlockStmt* block) {
	std::vector<Stmt*> stmts = block->statements;
	uint n = stmts.size();
	ASTNode* node;

	if (n == 0)
		node = new NopNode(block->token);
	else {
		node = get(stmts[n-1]);
		for (auto it=++stmts.rbegin(); it!=stmts.rend(); ++it)
			node = seq(*it, node);
	}

	ret(node);
}


ASTNode* Elaborator::seq(Stmt* stmt, ASTNode* node) {
	if (dynamic_cast<DeclStmt*>(stmt)) {
		// patch decl node
		DeclNode* decl = static_cast<DeclNode*>(get(stmt));
		ASTNode* scope = decl->scope;

		if (scope)
			decl->scope = new SeqNode(scope->token, scope, node);
		else
			decl->scope = node;

		return decl;
	}

	return new SeqNode(stmt->token, get(stmt), node);
}


void Elaborator::visit(DeclStmt* stmt) {
	std::string id = stmt->identifier;
	ASTNode* node;

	if (stmt->expr) {
		Expr* expr = stmt->expr;
		node = new AssignNode(expr->token, id, expr);
	}
	else
		node = new NopNode(stmt->token);

	ret( new DeclNode(stmt->token, id, stmt->type, node) );
}


void Elaborator::visit(IfStmt* stmt) {
	ASTNode* otherwise;
	if (stmt->otherwise)
		otherwise = get(stmt->otherwise);
	else
		otherwise = new NopNode(stmt->token);

	ret( new IfNode(stmt->token, stmt->cond, get(stmt->then), otherwise) );
}


void Elaborator::visit(WhileStmt* stmt) {
	ret( new WhileNode(stmt->token, stmt->cond, get(stmt->body)) );
}


void Elaborator::visit(ForStmt* stmt) {
	ASTNode* body = get(stmt->body);

	if (stmt->step) {
		if (dynamic_cast<DeclStmt*>(stmt->step))
			errors.add("Cannot declare a variable here.", stmt->step->token);
		body = new SeqNode(stmt->token, body, get(stmt->step));
	}

	ASTNode* node = new WhileNode(stmt->token, stmt->cond, body);

	if (stmt->init)
		// lift up to a declNode if stmt->init is a decl
		ret( seq(stmt->init, node) );
	else
		ret(node);
}


void Elaborator::visit(ReturnStmt* stmt) {
	ret( new ReturnNode(stmt->token, stmt->expr) );
}


void Elaborator::visit(AssignStmt* stmt) {
	std::string id = getIdOrError(stmt->lvalue);

	Expr* expr;
	if (stmt->op == BinOp::EQL)
		expr = stmt->rvalue;
	else {
		expr = new BinaryExpr(stmt->rvalue->token, stmt->op, stmt->lvalue, stmt->rvalue);
	}

	ret( new AssignNode(stmt->token, id, expr) );
}


void Elaborator::visit(PostOpStmt* stmt) {
	std::string id = getIdOrError(stmt->expr);
	Expr* one = new LiteralExpr(stmt->token, 1U);
	Expr* rvalue = new BinaryExpr(stmt->token, stmt->op, stmt->expr, one);

	ret( new AssignNode(stmt->token, id, rvalue) );
}


void Elaborator::visit(ExprStmt* stmt) {
	ret( new ExprNode(stmt->token, stmt->expr) );
}


void Elaborator::ret(ASTNode* stmt) {
	retval = stmt;
}


ASTNode* Elaborator::get(Stmt* stmt) {
	stmt->accept(*this);
	ASTNode* node = retval;
	retval = nullptr;
	return node;
}


std::string Elaborator::getIdOrError(Expr* expr) {
	IdExpr* idExpr = dynamic_cast<IdExpr*>(expr);

	if (!idExpr) {
		errors.add("Invalid lvalue", expr->token);
		return "";
	}

	return idExpr->identifier;
}
