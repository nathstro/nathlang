#include "Resolver.hpp"
#include "ErrorHandler.hpp"
#include "Stmt.hpp"

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& stmts)
{
	for (const auto& s : stmts)
	{
		resolve(*s);
	}
}

void Resolver::resolve(const Stmt& stmt)
{
	stmt.accept(*this);
}

void Resolver::resolve(const Expr& expr)
{
	expr.accept(*this);
}

void Resolver::resolveLocal(const Expr& expr, const Token& name)
{
	for (int i = static_cast<int>(scopes.size()) - 1; i >= 0; i--)
    {
        if (scopes[i].find(name.lexeme) != scopes[i].end())
        {
            interpreter.resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }
}

void Resolver::resolveFunction(const FunctionStmt& function, FunctionType type)
{
	FunctionType enclosingFunction = currentFunction;
	currentFunction = type;

	beginScope();
	for (const auto& p : function.params)
	{
		declare(p);
		define(p);
	}
	resolve(function.body);
	endScope();

	currentFunction = enclosingFunction;
}

std::any Resolver::visitLiteral(const Literal& expr)
{
	return {};
}

std::any Resolver::visitGrouping(const Grouping& expr)
{
	resolve(*expr.expression);
	return {};
}

std::any Resolver::visitUnary(const Unary& expr)
{
	resolve(*expr.right);
	return {};
}

std::any Resolver::visitBinary(const Binary& expr)
{
	resolve(*expr.left);
	resolve(*expr.right);
	return {};
}

std::any Resolver::visitVariable(const Variable& expr)
{
	if (!scopes.empty())
    {
        auto it = scopes.back().find(expr.name.lexeme);
        if (it != scopes.back().end() && !it->second)
        {
            ErrorHandler::error(expr.name, "Can't read local variable in its own initializer.");
   	    }
    }

	resolveLocal(expr, expr.name);
	return {};
}

std::any Resolver::visitAssign(const Assign& expr)
{
	resolve(*expr.value);
	resolveLocal(expr, expr.name);
	return {};
}

std::any Resolver::visitLogical(const Logical& expr)
{
	resolve(*expr.left);
	resolve(*expr.right);
	return {};
}

std::any Resolver::visitCall(const Call& expr)
{
	resolve(*expr.callee);

	for (const auto& arg : expr.args)
	{
		resolve(*arg);
	}

	return {};
}

std::any Resolver::visitGet(const Get& expr)
{
	resolve(*expr.object);
	return {};
}

std::any Resolver::visitSet(const Set& expr)
{
	resolve(*expr.value);
	resolve(*expr.object);
	return {};
}

std::any Resolver::visitThis(const This& expr)
{
	if (currentClass == ClassType::NONE)
	{
		ErrorHandler::error(expr.keyword, "Can't use 'this' outside of a class.");
		return {};
	}
	
	resolveLocal(expr, expr.keyword);
	return {};
}

std::any Resolver::visitSuper(const Super& expr)
{
	if (currentClass == ClassType::NONE)
	{
		ErrorHandler::error(expr.keyword, "Can't use 'super' outside of a class.");
	}
	else if (currentClass != ClassType::SUBCLASS)
	{
		ErrorHandler::error(expr.keyword, "Can't use 'super' in a class without a superclass.");
	}
	resolveLocal(expr, expr.keyword);
	return {};
}

void Resolver::visitIfStmt(const IfStmt& stmt)
{
	resolve(*stmt.condition);
	resolve(*stmt.thenBranch);

	if (stmt.elseBranch)
		resolve(*stmt.elseBranch);
}

void Resolver::visitPrintStmt(const PrintStmt& stmt)
{
	resolve(*stmt.expr);
}

void Resolver::visitExpressionStmt(const ExpressionStmt& stmt)
{
	resolve(*stmt.expr);
}

void Resolver::visitVarStmt(const VarStmt& stmt)
{
	declare(stmt.name);
	if (stmt.initialiser)
	{
		resolve(*stmt.initialiser);
	}
	define(stmt.name);
}

void Resolver::visitBlockStmt(const BlockStmt& stmt)
{
	beginScope();
	resolve(stmt.statements);
	endScope();
}

void Resolver::visitWhileStmt(const WhileStmt& stmt)
{
	resolve(*stmt.condition);
	resolve(*stmt.body);

}

void Resolver::visitFunctionStmt(const FunctionStmt& stmt)
{
	declare(stmt.name);
	define(stmt.name);

	resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::visitReturnStmt(const ReturnStmt& stmt)
{
	if (currentFunction == FunctionType::NONE)
		ErrorHandler::error(stmt.keyword, "Can't return from top level code.");
	if (stmt.value)
	{
		if (currentFunction == FunctionType::INITIALISER)
			ErrorHandler::error(stmt.keyword, "Can't return from a constructor.");
		resolve(*stmt.value);
	}
}

void Resolver::visitClassStmt(const ClassStmt& stmt)
{
	ClassType enclosingClass = currentClass;
	currentClass = ClassType::CLASS;

	declare(stmt.name);
	define(stmt.name);

	if (stmt.superclass)
	{
		if (stmt.name.lexeme == stmt.superclass->name.lexeme)
		{
			ErrorHandler::error(stmt.superclass->name, "A class can't inherit from itself.");
		}
		else
		{
			currentClass = ClassType::SUBCLASS;
			resolve(*stmt.superclass);
			beginScope();
			scopes.back()["super"] = true;
		}
	}

	beginScope();
	scopes.back()["this"] = true;

	for (const auto& m : stmt.methods)
	{
		FunctionType declaration = FunctionType::METHOD;
		if (m->name.lexeme == "CONSTRUCTOR")
			declaration = FunctionType::INITIALISER;

		resolveFunction(*m, declaration);
	}

	endScope();
	if (stmt.superclass) endScope();
	currentClass = enclosingClass;
}

void Resolver::beginScope()
{
	scopes.emplace_back();
}

void Resolver::endScope()
{
	scopes.pop_back();
}

void Resolver::declare(const Token& name)
{
	if (!scopes.empty())
		scopes.back()[name.lexeme] = false;
}

void Resolver::define(const Token& name)
{
	if (!scopes.empty())
		scopes.back()[name.lexeme] = true;
}

