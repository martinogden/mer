#include "type/comparison.hpp"
#include "type/canonicaliser.hpp"
#include "ast/elaborator/stmt.hpp"
#include "ast/elaborator/expr.hpp"


Elaborator::Elaborator(ParseTree& tree, Generator& gen) :
	tree(tree),
	gen(gen)
{
	seedTypes();
}


std::vector<FunNodePtr> Elaborator::run() {
	for (StmtPtr& decl : tree.decls)
		decl->accept(*this);

	if (!funTypes.exists("main"))
		errors.add("No main function found.");

	for (const auto& pair : structTypes) {
		StructTypePtr type = pair.second;
		if (!type->isFinal)
			errors.add("Struct " + type->name + " declared but not defined");

		// patch mutually recursively defined structs
		for (size_t i=0; i<type->fields.size(); ++i)
			type->fields[i].type = canon(type->fields[i].type);
	}

	for (const auto& pair : funTypes) {
		FunTypePtr type = pair.second;

		// patch non-final struct types etc.
		for (size_t i=0; i<type->domain.size(); ++i)
			type->domain[i] = canon(type->domain[i]);
	}

	return std::move(functions);
};


Map<std::shared_ptr<FunType>> Elaborator::getFunTypes() {
	return std::move(funTypes);
}


Map<std::shared_ptr<StructType>> Elaborator::getStructTypes() {
	return std::move(structTypes);
}


void Elaborator::visit(FunDecl& decl) {
	std::string id = decl.identifier;

	if (funTypes.exists(id)) {
		FunTypePtr type = funTypes.lookup(id);
		if (!checkFunType(decl, *type))
			errors.add("Function declarations mismatch", decl.token);
	}
	else {
		std::unordered_set<std::string> names;
		std::vector<TypePtr> domain;
		for (auto const& param : decl.params) {
			std::string name = param->identifier;
			if (names.find(param->identifier) != names.end())
				errors.add("function params must have distinct names", param->token);
			else
				names.insert(name);
			domain.push_back( canon(param->type) );
		}

		TypePtr codomain = canon(decl.type);
		funTypes.define(id, std::make_shared<FunType>(domain, codomain));
	}
}


void Elaborator::visit(FunDefn& defn) {
	defn.decl->accept(*this);

	std::string id = defn.decl->identifier;
	ASTNodePtr body = get(defn.body);
	assert(funTypes.exists(id));
	FunTypePtr type = funTypes.lookup(id);

	std::vector<FunNode::Param> params;
	for (auto const& param : defn.decl->params)
		params.emplace_back( std::move(param->token), param->identifier );

	FunNodePtr node = std::make_unique<FunNode>(defn.token, id, std::move(type), std::move(params), std::move(body));
	emit( std::move(node) );
}


void Elaborator::visit(StructDecl& decl) {
	std::string id = decl.identifier;
	if (!structTypes.exists(id))
		structTypes.define(id, std::make_shared<StructType>(id));
}


void Elaborator::visit(StructDefn& defn) {
	std::string id = defn.decl->identifier;

	if (structTypes.exists(id) && structTypes.lookup(id)->isFinal)
		errors.add("attempt to redefine struct", defn.token);
	else {
		std::unordered_set<std::string> names;
		std::vector<StructType::Field> fields;

		for (const auto& field : defn.fields) {
			std::string name = field->identifier;
			if (names.find(name) != names.end())
				errors.add("struct fields must have distinct names", field->token);
			else
				names.insert(name);

			TypePtr type = canon(field->type);
			if ( eq(type, Type::ERROR) )
				errors.add("field has incomplete type", field->token);
			if (!is_sml(type))
				errors.add("fields must have small type", field->token);
			fields.emplace_back(field->identifier, std::move(type));
		}

		StructTypePtr stype = std::make_shared<StructType>(id, std::move(fields));
		structTypes.define(id, stype);

		// patch struct types
		for (auto& field : stype->fields) {
			if ( auto t = dynamic_cast<PtrType*>(field.type.get()) ) {
				if ( auto s = dynamic_cast<StructType*>(t->type.get()))
					field.type = std::make_shared<PtrType>( structTypes.lookup(s->name) );
			}
		}

		for (auto& alias : structAliases[id])
			types.define(alias, stype);
	}
}


void Elaborator::visit(TypedefStmt& stmt) {
	std::string alias = stmt.alias.lexeme;

	if (funTypes.exists(alias))
		errors.add("A function with this name (" + alias + ") has already been declared", stmt.alias);
	else if (types.exists(alias))
		errors.add("Type alias already defined: " + alias + ".", stmt.alias);
	else {
		TypePtr type = canon(stmt.type);
		if ( auto ty = dynamic_cast<StructType*>(type.get()) )
			structAliases[ty->name].insert(alias);
		types.define(alias, type);
	}
}


void Elaborator::visit(BlockStmt& block) {
	ASTNodePtr end = std::make_unique<NopNode>(block.token);
	ret( seq(block.statements, std::move(end)) );
}


void Elaborator::visit(DeclStmt& stmt) {
	std::string id = stmt.identifier;
	ASTNodePtr node;
	TypePtr t = canon(stmt.type);

	if (stmt.expr) {
		ExprPtr lvalue = std::make_unique<IdExpr>(stmt.token, id);
		node = std::make_unique<AssignNode>(stmt.token, std::move(lvalue), get(stmt.expr));
	}
	else
		node = std::make_unique<NopNode>(stmt.token);

	node = std::make_unique<DeclNode>(stmt.token, id, canon(stmt.type), std::move(node));
	ret( std::move(node) );
}


void Elaborator::visit(IfStmt& stmt) {
	ASTNodePtr otherwise =  stmt.otherwise ? get(stmt.otherwise) : std::make_unique<NopNode>(stmt.token);
	ASTNodePtr node = std::make_unique<IfNode>(
		stmt.token,
		get(stmt.cond),
		get(stmt.then),
		std::move(otherwise)
	);
	ret( std::move(node) );
}


void Elaborator::visit(WhileStmt& stmt) {
	ASTNodePtr node = std::make_unique<WhileNode>(stmt.token,get(stmt.cond),get(stmt.body));
	ret( std::move(node) );
}


void Elaborator::visit(ForStmt& stmt) {
	ASTNodePtr body = get(stmt.body);

	if (stmt.step) {
		if ( dynamic_cast<DeclStmt*>(stmt.step.get()) )
			errors.add("Cannot declare a variable here.", stmt.step->token);
		body = std::make_unique<SeqNode>(stmt.token, std::move(body), get(stmt.step));
	}

	ASTNodePtr node = std::make_unique<WhileNode>(stmt.token, get(stmt.cond), std::move(body));

	if (stmt.init)  // lift up to a declNode if stmt->init is a decl
		ret( seq(stmt.init, std::move(node)) );
	else
		ret( std::move(node) );
}


void Elaborator::visit(ReturnStmt& stmt) {
	ExprPtr expr = stmt.expr ? get(stmt.expr) : nullptr;
	ret( std::make_unique<ReturnNode>(stmt.token, std::move(expr)) );
}


void Elaborator::visit(AssignStmt& stmt) {
	auto pair = getLValue(stmt.lvalue);

	if (errors.exist()) {
		ret(nullptr);
		return;
	}

	std::vector<StmtPtr>& stmts = pair.first;
	ExprPtr& lvalue = pair.second;
	ExprPtr rvalue;

	if (stmt.op == BinOp::EQL)
		rvalue = get(stmt.rvalue);
	else
		rvalue = std::make_unique<BinaryExpr>(
			stmt.rvalue->token,
			stmt.op,
			get(lvalue),
			get(stmt.rvalue)
		);

	ASTNodePtr node = std::make_unique<AssignNode>(
		stmt.token,
		get(lvalue),
		std::move(rvalue)
	);

	ret( seq(stmts, std::move(node)) );
}


void Elaborator::visit(PostOpStmt& stmt) {
	auto pair = getLValue(stmt.expr);

	if (errors.exist()) {
		ret(nullptr);
		return;
	}

	std::vector<StmtPtr>& stmts = pair.first;
	ExprPtr& lvalue = pair.second;

	ExprPtr rvalue = std::make_unique<BinaryExpr>(
		stmt.token,
		stmt.op,
		get(lvalue),
		std::make_unique<LiteralExpr>(stmt.token, 0x1LL)
	);

	ASTNodePtr node = std::make_unique<AssignNode>(
		stmt.token,
		get(lvalue),
		std::move(rvalue)
	);

	ret( seq(stmts, std::move(node)) );
}


void Elaborator::visit(ExprStmt& stmt) {
	ret( std::make_unique<ExprNode>(stmt.token, get(stmt.expr)) );
}


ASTNodePtr Elaborator::seq(std::vector<StmtPtr>& stmts, ASTNodePtr node) {
	for (auto it=stmts.rbegin(); it!=stmts.rend(); ++it)
		node = seq(*it, std::move(node));
	return node;
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


ExprPtr Elaborator::get(ExprPtr& expr) {
	ExprElaborator elab(types, funTypes, structTypes);
	ExprPtr e = elab.get(expr);
	if (elab.errors.exist())
		errors.append(elab.errors);

	return std::move(e);
}


std::pair<std::vector<StmtPtr>, ExprPtr> Elaborator::getLValue(ExprPtr& expr) {
	LValueChecker checker;
	if (!checker.get(expr)) {
		errors.add("Invalid lvalue", expr->token);
		return {};
	}

	LValueCanon lvalCanon(expr, funTypes, gen);
	return lvalCanon.run();
}


void Elaborator::emit(FunNodePtr node) {
	std::string id = node->id;
	if (funNames.find(id) != funNames.end())
		errors.add("Cannot redefine function", node->token);
	else {
		funNames.insert(id);
		functions.push_back(std::move(node) );
	}
}


TypePtr Elaborator::canon(TypePtr type) {
	TypeCanonicaliser typeCanon(types, structTypes);
	TypePtr ty = typeCanon.get(type);

	if (typeCanon.errors.exist())
		errors.append(typeCanon.errors);

	if ( auto t = dynamic_cast<StructType*>(ty.get()) ) {
		if (structTypes.exists(t->name))
			return structTypes.lookup(t->name);
	}
	return std::move(ty);
}


// Check fun signature matches recorded fun decl type
bool Elaborator::checkFunType(FunDecl& decl, FunType& type) {
	if (decl.params.size() != type.domain.size())
		return false;

	TypePtr codomain = canon(decl.type);
	if (!eq(codomain, type.codomain))
		return false;

	for (uint i=0; i<decl.params.size(); ++i) {
		if (!eq(canon(decl.params[i]->type), canon(type.domain[i])))
			return false;
	}

	return true;
}


bool Elaborator::checkStructType(StructDefn& defn, StructType& type) {
	if (defn.fields.size() != type.fields.size())
		return false;

	for (uint i=0; i<defn.fields.size(); ++i) {
		DeclStmtPtr& field = defn.fields[i];
		TypePtr ty = canon(field->type);
		assert(ty && type.fields[i].type);
		if (!eq(ty, type.fields[i].type) || field->identifier != type.fields[i].name)
			return false;
	}

	return true;
}


void Elaborator::seedTypes() {
	for (const auto& pair : Type::baseTypes)
		types.define(pair.first, pair.second);

	std::vector<TypePtr> main_dom;
	funTypes.define("main", std::make_shared<FunType>(std::move(main_dom), Type::INT));

	std::vector<TypePtr> assert_dom;
	assert_dom.push_back(Type::BOOL);
	funTypes.define("assert",std::make_shared<FunType>(std::move(assert_dom), Type::VOID));

	std::vector<TypePtr> alloc_dom;
	alloc_dom.push_back(Type::INT);
	funTypes.define("alloc",std::make_shared<FunType>(std::move(alloc_dom), std::make_shared<PtrType>(Type::INT)));

	std::vector<TypePtr> alloc_array_dom;
	alloc_array_dom.push_back(Type::INT);
	alloc_array_dom.push_back(Type::INT);
	funTypes.define("alloc_array",std::make_shared<FunType>(std::move(alloc_array_dom), std::make_shared<PtrType>(Type::INT)));
}
