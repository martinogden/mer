#include "elaborator.hpp"


Elaborator::Elaborator(ParseTree& tree) : tree(tree) {
	seedTypes();
	seedDecls();
}


std::vector<FunNodePtr> Elaborator::run() {
	for (StmtPtr& decl : tree.decls)
		decl->accept(*this);

	if (!decls.exists("main"))
		errors.add("No main function found.");

	if (errors.exist())
		defns.clear();

	return std::move(defns);
};


SymTab<FunType> Elaborator::getDecls() {
	return std::move(decls);
}


void Elaborator::visit(FunDecl& decl) {
	// TODO: tidy up
	std::string id = decl.identifier;
	if (decls.exists(id)) {
		// check types match
		std::vector<DeclStmtPtr>& params = decl.params;

		FunType type = decls.lookup(id);
		if (params.size() != type.first.size()) {
			errors.add("Function declarations mismatch", decl.token);
			return;
		}

		if (resolveType(decl.type) != type.second) {
			errors.add("Function declarations mismatch", decl.token);
			return;
		}

		for (uint i=0; i<params.size(); ++i) {
			DeclStmtPtr& param = params[i];
			if (resolveType(param->type) != type.first[i]) {
				errors.add("Function declarations mismatch", param->token);
				return;
			}
		}
	}
	else {
		std::vector<Type> domain;
		for (auto const& param : decl.params)
			domain.push_back( resolveType(param->type) );

		Type codomain = resolveType(decl.type);
		decls.define(id, { domain, codomain });
	}
}


void Elaborator::visit(FunDefn& defn) {
	defn.decl->accept(*this);

	std::string id = defn.decl->identifier;
	Type type = resolveType(defn.decl->type);
	ASTNodePtr body = get(defn.body);

	std::vector<Param> params;
	for (auto const& param : defn.decl->params)
		params.emplace_back( param->identifier, resolveType(param->type) );

	emit( std::make_unique<FunNode>(defn.token, id, type, params, std::move(body)) );
}


void Elaborator::visit(TypedefStmt& stmt) {
	std::string alias = stmt.alias.lexeme;
	if (decls.exists(alias))
		errors.add(
			"A function with this name (" + alias
			+ ") has already been declared",stmt.alias
		);
	else if (types.exists(alias))
		errors.add("Type alias already defined: " + alias + ".", stmt.alias);
	else
		types.define(alias, resolveType(stmt.type));
	// TODO: what do we return here?
}


void Elaborator::visit(BlockStmt& block) {
	std::vector<StmtPtr>& stmts = block.statements;
	uint n = stmts.size();
	ASTNodePtr node;

	if (n == 0)
		node = std::make_unique<NopNode>(block.token);
	else {
		node = get(stmts[n-1]);
		for (auto it=++stmts.rbegin(); it!=stmts.rend(); ++it)
			node = seq(*it, std::move(node));
	}

	ret( std::move(node) );
}


void Elaborator::visit(DeclStmt& stmt) {
	std::string id = stmt.identifier;
	ASTNodePtr node;

	if (stmt.expr) {
		stmt.expr->accept(*this);
		node = std::make_unique<AssignNode>(stmt.token, id, std::move(stmt.expr));
	}
	else
		node = std::make_unique<NopNode>(stmt.token);

	ret( std::make_unique<DeclNode>(stmt.token, id, resolveType(stmt.type), std::move(node)) );
}


void Elaborator::visit(IfStmt& stmt) {
	stmt.cond->accept(*this);

	ASTNodePtr otherwise;
	if (stmt.otherwise)
		otherwise = get(stmt.otherwise);
	else
		otherwise = std::make_unique<NopNode>(stmt.token);

	ret( std::make_unique<IfNode>(stmt.token, std::move(stmt.cond), get(stmt.then), std::move(otherwise)) );
}


void Elaborator::visit(WhileStmt& stmt) {
	stmt.cond->accept(*this);
	ret( std::make_unique<WhileNode>(stmt.token, std::move(stmt.cond), get(stmt.body)) );
}


void Elaborator::visit(ForStmt& stmt) {
	stmt.cond->accept(*this);

	ASTNodePtr body = get(stmt.body);

	if (stmt.step) {
		if ( dynamic_cast<DeclStmt*>(stmt.step.get()) )
			errors.add("Cannot declare a variable here.", stmt.step->token);
		body = std::make_unique<SeqNode>(stmt.token, std::move(body), get(stmt.step));
	}

	ASTNodePtr node = std::make_unique<WhileNode>(stmt.token, std::move(stmt.cond), std::move(body));

	if (stmt.init) {
		// lift up to a declNode if stmt->init is a decl
		ret( seq(stmt.init, std::move(node)) );
	}
	else
		ret( std::move(node) );
}


void Elaborator::visit(ReturnStmt& stmt) {
	if (stmt.expr)
		stmt.expr->accept(*this);

	ret( std::make_unique<ReturnNode>(stmt.token, std::move(stmt.expr)) );
}


void Elaborator::visit(AssignStmt& stmt) {
	std::string id = getIdOrError(stmt.lvalue);

	stmt.lvalue->accept(*this);
	stmt.rvalue->accept(*this);

	ExprPtr expr;
	if (stmt.op == BinOp::EQL)
		expr = std::move(stmt.rvalue);
	else
		expr = std::make_unique<BinaryExpr>(stmt.rvalue->token, stmt.op, std::move(stmt.lvalue), std::move(stmt.rvalue));

	ret( std::make_unique<AssignNode>(stmt.token, id, std::move(expr)) );
}


void Elaborator::visit(PostOpStmt& stmt) {
	std::string id = getIdOrError(stmt.expr);
	ExprPtr one = std::make_unique<LiteralExpr>(stmt.token, 0x1);
	ExprPtr rvalue = std::make_unique<BinaryExpr>(stmt.token, stmt.op, std::move(stmt.expr), std::move(one));

	ret( std::make_unique<AssignNode>(stmt.token, id, std::move(rvalue)) );
}


void Elaborator::visit(ExprStmt& stmt) {
	stmt.expr->accept(*this);
	ret( std::make_unique<ExprNode>(stmt.token, std::move(stmt.expr)) );
}


void Elaborator::visit(CallExpr& expr) {
	// FIXME: temporary hack
	if (expr.identifier == "main")
		expr.identifier = "__c0_main";

	std::string id = expr.identifier;

	for (auto const& arg : expr.args)
		arg->accept(*this);

	if (!decls.exists(id))
		errors.add("Use of undeclared function: " + id + ".", expr.token);
}


ASTNodePtr Elaborator::seq(StmtPtr& stmt, ASTNodePtr node) {
	if ( auto declStmt = dynamic_cast<DeclStmt*>(stmt.get()) ) {
		// patch decl node
		visit(*declStmt);
		DeclNode* decl = dynamic_cast<DeclNode*>(retval.release());

		if (decl->scope)
			decl->scope = std::make_unique<SeqNode>(decl->token, std::move(decl->scope), std::move(node));
		else
			decl->scope = std::move(node);

		return std::unique_ptr<DeclNode>(decl);
	}

	return std::make_unique<SeqNode>(stmt->token, get(stmt), std::move(node));
}


void Elaborator::ret(ASTNodePtr stmt) {
	retval = std::move(stmt);
}


ASTNodePtr Elaborator::get(StmtPtr& stmt) {
	stmt->accept(*this);
	return std::move(retval);
}


void Elaborator::emit(FunNodePtr node) {
	std::string id = node->id;
	if (defnNames.find(id) != defnNames.end())
		errors.add("Cannot redefine function", node->token);
	else {
		defnNames.insert(id);
		defns.emplace_back( std::move(node) );
	}
}


std::string Elaborator::getIdOrError(ExprPtr& expr) {
	IdExpr* idExpr = dynamic_cast<IdExpr*>(expr.get());

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
