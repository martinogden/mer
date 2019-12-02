#include "elaborator.hpp"


Elaborator::Elaborator(ParseTree* tree) : tree(tree) {
	seedTypes();
	seedDecls();
}


std::vector<FunNode*> Elaborator::run() {
	// TODO store list of stmts rather than block
	BlockStmt* block = static_cast<BlockStmt*>(tree->block);

	for (Stmt* stmt : block->statements)
		stmt->accept(*this);

	if (!decls.exists("main"))
		errors.add("No main function found.");

	if (errors.exist())
		return {};
	else
		return defns;
};


SymTab<FunType> Elaborator::getDecls() {
	return decls;
}


void Elaborator::visit(FunDecl* decl) {
	// TODO: tidy up
	std::string id = decl->identifier;
	if (decls.exists(id)) {
		// check types match
		std::vector<DeclStmt*> params = decl->params;

		FunType type = decls.lookup(id);
		if (params.size() != type.first.size()) {
			errors.add("Function declarations mismatch", decl->token);
			return;
		}

		if (resolveType(decl->type) != type.second) {
			errors.add("Function declarations mismatch", decl->token);
			return;
		}

		for (uint i=0; i<params.size(); ++i) {
			DeclStmt* param = params[i];
			if (resolveType(param->type) != type.first[i]) {
				errors.add("Function declarations mismatch", param->token);
				return;
			}
		}
	}
	else {
		std::vector<Type> domain;
		for (auto const& param : decl->params)
			domain.push_back( resolveType(param->type) );

		Type codomain = resolveType(decl->type);
		decls.define(id, { domain, codomain });
	}
}


void Elaborator::visit(FunDefn* defn) {
	defn->decl->accept(*this);

	std::string id = defn->decl->identifier;
	Type type = resolveType(defn->decl->type);
	ASTNode* body = get(defn->body);

	std::vector<Param> params;
	for (auto const& param : defn->decl->params)
		params.push_back({ param->identifier, resolveType(param->type) });

	emit( new FunNode(defn->token, id, type, params, body) );
}


void Elaborator::visit(TypedefStmt* stmt) {
	std::string alias = stmt->alias.lexeme;
	if (decls.exists(alias))
		errors.add(
				"A function with this name (" + alias
				+ ") has already been declared",stmt->alias
			);
	else if (types.exists(alias))
		errors.add("Type alias already defined: " + alias + ".", stmt->alias);
	else
		types.define(alias, resolveType(stmt->type));
	// TODO what do we return here?
}


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


void Elaborator::visit(DeclStmt* stmt) {
	std::string id = stmt->identifier;
	ASTNode* node;

	if (stmt->expr) {
		stmt->expr->accept(*this);

		Expr* expr = stmt->expr;
		node = new AssignNode(expr->token, id, expr);
	}
	else
		node = new NopNode(stmt->token);

	ret( new DeclNode(stmt->token, id, resolveType(stmt->type), node) );
}


void Elaborator::visit(IfStmt* stmt) {
	stmt->cond->accept(*this);

	ASTNode* otherwise;
	if (stmt->otherwise)
		otherwise = get(stmt->otherwise);
	else
		otherwise = new NopNode(stmt->token);

	ret( new IfNode(stmt->token, stmt->cond, get(stmt->then), otherwise) );
}


void Elaborator::visit(WhileStmt* stmt) {
	stmt->cond->accept(*this);
	ret( new WhileNode(stmt->token, stmt->cond, get(stmt->body)) );
}


void Elaborator::visit(ForStmt* stmt) {
	if (stmt->init) stmt->init->accept(*this);
	stmt->cond->accept(*this);
	if (stmt->step) stmt->step->accept(*this);

	ASTNode* body = get(stmt->body);

	if (stmt->step) {
		if (dynamic_cast<DeclStmt*>(stmt->step))
			errors.add("Cannot declare a variable here.", stmt->step->token);
		body = new SeqNode(stmt->token, body, get(stmt->step));
	}

	ASTNode* node = new WhileNode(stmt->token, stmt->cond, body);

	if (stmt->init) {
		// lift up to a declNode if stmt->init is a decl
		ret( seq(stmt->init, node) );
	}
	else
		ret(node);
}


void Elaborator::visit(ReturnStmt* stmt) {
	if (stmt->expr)
		stmt->expr->accept(*this);

	ret( new ReturnNode(stmt->token, stmt->expr) );
}


void Elaborator::visit(AssignStmt* stmt) {
	std::string id = getIdOrError(stmt->lvalue);

	stmt->lvalue->accept(*this);
	stmt->rvalue->accept(*this);

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
	Expr* one = new LiteralExpr(stmt->token, 0x1);
	Expr* rvalue = new BinaryExpr(stmt->token, stmt->op, stmt->expr, one);

	ret( new AssignNode(stmt->token, id, rvalue) );
}


void Elaborator::visit(ExprStmt* stmt) {
	stmt->expr->accept(*this);
	ret( new ExprNode(stmt->token, stmt->expr) );
}


void Elaborator::visit(CallExpr* expr) {
	// FIXME: temporary hack
	if (expr->identifier == "main")
		expr->identifier = "__c0_main";

	std::string id = expr->identifier;

	for (auto const& arg : expr->args)
		arg->accept(*this);

	if (!decls.exists(id))
		errors.add("Use of undeclared function: " + id + ".", expr->token);
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


void Elaborator::ret(ASTNode* stmt) {
	retval = stmt;
}


ASTNode* Elaborator::get(Stmt* stmt) {
	stmt->accept(*this);
	ASTNode* node = retval;
	retval = nullptr;
	return node;
}


void Elaborator::emit(FunNode* node) {
	std::string id = node->id;
	if (defnNames.find(id) != defnNames.end())
		errors.add("Cannot redefine function", node->token);
	else {
		defnNames.insert(id);
		defns.push_back(node);
	}
}


std::string Elaborator::getIdOrError(Expr* expr) {
	IdExpr* idExpr = dynamic_cast<IdExpr*>(expr);

	if (!idExpr) {
		errors.add("Invalid lvalue", expr->token);
		return "";
	}

	return idExpr->identifier;
}


Type Elaborator::resolveType(Token token) {
	std::string name = token.lexeme;

	if (!types.exists(name)) {
		errors.add("Invalid type alias: " + name + ".", token);
		return Type::ERROR;
	}
	else
		return types.lookup(name);
}


void Elaborator::seedTypes() {
	for (auto const& pair : concreteTypes)
		types.define(pair.first, pair.second);
}


void Elaborator::seedDecls() {
	decls.define("main", {{}, Type::INT});
	decls.define("assert", {{Type::BOOL}, Type::VOID});
}