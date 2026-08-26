#include "Interpreter.hpp"
#include "Callable.hpp"
#include "Class.hpp"
#include "Environment.hpp"
#include "ErrorHandler.hpp"
#include "Function.hpp"
#include "Instance.hpp"
#include "Return.hpp"
#include "RuntimeError.hpp"
#include "StdLib.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <any>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

Interpreter::Interpreter()
{
	globalEnvironment = std::make_shared<Environment>(); 
	environment = globalEnvironment;
	StdLib::registerAllNativeFunctions(*this);
}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>& stmts)
{
	try
	{
		for (const auto& s : stmts)
		{
			execute(*s);
		}
	}
	catch (RuntimeError error)
	{
		ErrorHandler::runtimeError(error);
	}
}

std::any Interpreter::visitLiteral(const Literal& expr)
{
	return expr.value;
}

std::any Interpreter::visitGrouping(const Grouping& expr)
{
	return evaluate(*expr.expression);
}

std::any Interpreter::visitVariable(const Variable& expr)
{
	return lookUpVariable(expr.name, expr);
}

std::any Interpreter::visitAssign(const Assign& expr)
{
	std::any value = evaluate(*expr.value);

	auto it = locals.find(&expr);

	if (it != locals.end())
	{
		int distance = it->second;
		environment->assignAt(distance, expr.name, value);
	}
	else
	{
		globalEnvironment->assign(expr.name, value);
	}

	return value;
}

std::any Interpreter::visitLogical(const Logical& expr)
{
	std::any left = evaluate(*expr.left);

	if (expr.op.type == TokenType::OR)
	{
		if (isTruthy(left)) return left;
	}
	else
	{
		if (!isTruthy(left)) return left;
	}

	return evaluate(*expr.right);
}

std::any Interpreter::visitCall(const Call& expr)
{
	std::any callee = evaluate(*expr.callee);

	std::vector<std::any> args;
	for (const auto& arg : expr.args)
	{
		args.push_back(evaluate(*arg));
	}

	if (auto function = std::any_cast<std::shared_ptr<Callable>>(&callee))
	{
		if (args.size() != (*function)->arity())
		{
			throw RuntimeError(expr.paren, "Expected " + std::to_string((*function)->arity()) + " arguments but got " + std::to_string(args.size()));
		}

		return (*function)->call(*this, expr.paren, args);
	}

	if (auto newClass = std::any_cast<std::shared_ptr<Class>>(&callee))
	{
		if (args.size() != (*newClass)->arity())
		{
			throw RuntimeError(expr.paren, "Expected " + std::to_string((*newClass)->arity()) + " arguments but got " + std::to_string(args.size()));
		}

		return (*newClass)->call(*this, expr.paren, args);
	}

	throw RuntimeError(expr.paren, "Can only call functions and classes");
}

std::any Interpreter::visitGet(const Get& expr)
{
	std::any object = evaluate(*expr.object);
	if (object.type() == typeid(std::shared_ptr<Instance>))
	{
		std::shared_ptr<Instance> instance = std::any_cast<std::shared_ptr<Instance>>(object);
        return instance->get(expr.name); 
	}

	throw RuntimeError(expr.name, "Only instances have properties.");
}

std::any Interpreter::visitSet(const Set& expr)
{
	std::any object = evaluate(*expr.object);
	if (object.type() != typeid(std::shared_ptr<Instance>))
	{
		throw RuntimeError(expr.name, "Only instances have fields.");
	}

	std::any value = evaluate(*expr.value);
	std::any_cast<std::shared_ptr<Instance>>(object)->set(expr.name, value);
	return value;
}

std::any Interpreter::visitThis(const This& expr)
{
	return lookUpVariable(expr.keyword, expr);
}

std::any Interpreter::visitSuper(const Super& expr)
{
	auto it = locals.find(&expr);
	if (it == locals.end())
	{
		throw RuntimeError(expr.keyword, "Could not resolve 'super'.");
	}

	int distance = it->second;

	std::shared_ptr<Class> superclass = std::any_cast<std::shared_ptr<Class>>(
		environment->getAt(distance, "super")
	);
	std::shared_ptr<Instance> object = std::any_cast<std::shared_ptr<Instance>>(
		environment->getAt(distance - 1, "this")
	); 

	std::shared_ptr<Function> method = superclass->findMethod(expr.method.lexeme);
	
	if (!method)
		throw RuntimeError(expr.method, "Undefined property '" + expr.method.lexeme + "'.");

	return std::shared_ptr<Callable>(method->bind(object));
}

void Interpreter::visitIfStmt(const IfStmt& stmt)
{
	if (isTruthy(evaluate(*stmt.condition)))
	{
		execute(*stmt.thenBranch);
	}
	else if (stmt.elseBranch)
	{
		execute(*stmt.elseBranch);
	}
}

void Interpreter::visitPrintStmt(const PrintStmt& stmt)
{
	std::any value = evaluate(*stmt.expr);
	std::cout << stringify(value) << std::endl;
}

void Interpreter::visitExpressionStmt(const ExpressionStmt& stmt)
{
	evaluate(*stmt.expr);
}

void Interpreter::visitVarStmt(const VarStmt& stmt)
{
	std::any value;
	if (stmt.initialiser)
		value = evaluate(*stmt.initialiser);

	environment->define(stmt.name.lexeme, value);
}

void Interpreter::visitBlockStmt(const BlockStmt& stmt)
{
	auto environment = std::make_shared<Environment>(this->environment);

	executeBlock(stmt.statements, environment);
}

void Interpreter::visitWhileStmt(const WhileStmt& stmt)
{
	while(isTruthy(evaluate(*stmt.condition)))
	{
		execute(*stmt.body);
	}
}

void Interpreter::visitFunctionStmt(const FunctionStmt& stmt)
{
	std::shared_ptr<Callable> function = std::make_shared<Function>(stmt, environment, false);

	environment->define(stmt.name.lexeme, function);
}

void Interpreter::visitReturnStmt(const ReturnStmt& stmt)
{
	std::any value;
	if (stmt.value)
		value = evaluate(*stmt.value);

	throw Return(value);
}

void Interpreter::visitClassStmt(const ClassStmt& stmt)
{
	std::shared_ptr<Class> superclass;

	if (stmt.superclass)
	{
		std::any value = evaluate(*stmt.superclass);
		auto* newSuperclass = std::any_cast<std::shared_ptr<Class>>(&value);
		if (!newSuperclass)
		{
			throw RuntimeError(stmt.superclass->name, "Superclass must be a class.");
		}

		superclass = *newSuperclass;
	}

	environment->define(stmt.name.lexeme, NULL);

	if (stmt.superclass)
	{
		environment = std::make_shared<Environment>(environment);
		environment->define("super", superclass);
	}

	std::unordered_map<std::string, std::shared_ptr<Function>> methods;

	for (const auto& m : stmt.methods)
	{
		std::shared_ptr<Function> function = std::make_shared<Function>(*m, environment, m->name.lexeme == "CONSTRUCTOR");
		methods[m->name.lexeme] = function;
	}

	std::shared_ptr<Class> newClass = std::make_shared<Class>(stmt.name.lexeme, superclass, methods);
	if (superclass) environment = environment->enclosing;
	environment->assign(stmt.name, newClass);
}

std::any Interpreter::visitUnary(const Unary& expr)
{
	std::any right = evaluate(*expr.right);

	switch (expr.op.type)
	{
		case TokenType::NOT:
			return !isTruthy(right);
			
		case TokenType::MINUS:
			checkNumberOperand(expr.op, right);
			return -std::any_cast<double>(right);

		default:
			throw RuntimeError(expr.op, "Unary expressions cannot take this operator");
	}

	return NULL;
}

std::any Interpreter::visitBinary(const Binary& expr)
{
	std::any left = evaluate(*expr.left);
	std::any right = evaluate(*expr.right);

	switch (expr.op.type)
	{
		case TokenType::MINUS:
			checkNumberOperands(expr.op, left, right);
			return std::any_cast<double>(left) - std::any_cast<double>(right);

		case TokenType::SLASH:
			checkNumberOperands(expr.op, left, right);

			if (std::any_cast<double>(right) == 0.0)
				throw RuntimeError(expr.op, "Can't divide by zero");

			return std::any_cast<double>(left) / std::any_cast<double>(right);

		case TokenType::PLUS:
			if (left.type() == typeid(double) && right.type() == typeid(double))
				return std::any_cast<double>(left) + std::any_cast<double>(right);

			if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
				return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);

			if (left.type() == typeid(std::string) && right.type() == typeid(double))
				return std::any_cast<std::string>(left) + std::to_string(std::any_cast<double>(right));

			if (left.type() == typeid(double) && right.type() == typeid(std::string))
				return std::to_string(std::any_cast<double>(left)) + std::any_cast<std::string>(right);

			throw RuntimeError(expr.op, "Operands must be two numbers, two strings, or one number and one string.");

		case TokenType::STAR:
			if (left.type() == typeid(double) && right.type() == typeid(double))
				return std::any_cast<double>(left) * std::any_cast<double>(right);

			if (left.type() == typeid(std::string) && right.type() == typeid(double))
			{
				const std::string& str = std::any_cast<const std::string&>(left);
				double number = std::any_cast<double>(right);

				if (std::floor(number) != number)
				{
					throw RuntimeError(expr.op, "Can't multiply a string by a double.");
				}
				if (number < 0)
				{
					throw RuntimeError(expr.op, "Can't multiply a string by a negative.");
				}

				std::string result = "";
				for (int i = 0; i < number; i++)
				{
					result += str;
				}

				return result;
			}

			if (left.type() == typeid(double) && right.type() == typeid(std::string))
			{
				const std::string& str = std::any_cast<const std::string&>(right);
				double number = std::any_cast<double>(left);

				if (std::floor(number) != number)
				{
					throw RuntimeError(expr.op, "Can't multiply a string by a double.");
				}
				if (number < 0)
				{
					throw RuntimeError(expr.op, "Can't multiply a string by a negative.");
				}

				std::string result = "";
				for (int i = 0; i < number; i++)
				{
					result += str;
				}

				return result;
			}

			throw RuntimeError(expr.op, "Operands must be two numbers, two strings, or one number and one string.");

		case TokenType::GREATER:
			checkNumberOperands(expr.op, left, right);
			return std::any_cast<double>(left) > std::any_cast<double>(right);

		case TokenType::LESSER:
			checkNumberOperands(expr.op, left, right);
			return std::any_cast<double>(left) < std::any_cast<double>(right);

		case TokenType::GE:
			checkNumberOperands(expr.op, left, right);
			return std::any_cast<double>(left) >= std::any_cast<double>(right);

		case TokenType::LE:
			checkNumberOperands(expr.op, left, right);
			return std::any_cast<double>(left) <= std::any_cast<double>(right);

		case TokenType::EQ:
			return isEqual(left, right);

		case TokenType::NE:
			return !isEqual(left, right);

		default:
			throw RuntimeError(expr.op, "Binary expressions cannot take this operator.");
	}

	return NULL;
}

std::any Interpreter::evaluate(const Expr& expr)
{
	return expr.accept(*this);
}

void Interpreter::execute(const Stmt& stmt)
{
	stmt.accept(*this);
}

void Interpreter::executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment)
{
	std::shared_ptr<Environment> previous = this->environment;

	try
	{
		this->environment = std::move(environment);
		for (const auto& stmt : statements)
		{ 
			execute(*stmt);
		}
	}
	catch (...)
	{
		this->environment = previous;
		throw;	
	}

	this->environment = previous;
}

void Interpreter::resolve(const Expr& expr, int depth)
{
	locals[&expr] = depth;
}

std::any Interpreter::lookUpVariable(const Token& name, const Expr& expr)
{
	auto it = locals.find(&expr);

	if (it != locals.end())
	{
		int distance = it->second;
		return environment->getAt(distance, name.lexeme);
	}
	else
	{
		return globalEnvironment->get(name);
	}
}

bool Interpreter::isTruthy(const std::any& object)
{
	if (!object.has_value())
		return false;

	if (object.type() == typeid(bool))
		return std::any_cast<bool>(object);

	return false;
}

bool Interpreter::isEqual(const std::any& left, const std::any& right)
{
	if (!left.has_value() && !right.has_value())
		return true;
	if (!left.has_value())
		return false;
	if (left.type() != right.type())
		return false;

	if (left.type() == typeid(bool))
 		return std::any_cast<bool>(left) == std::any_cast<bool>(right);
 	if (left.type() == typeid(double))
 		return std::any_cast<double>(left) == std::any_cast<double>(right);
 	if (left.type() == typeid(std::string))
 		return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);

 	return false;
}

std::string Interpreter::stringify(const std::any& object)
{
	if (!object.has_value()) return "nil";

	if (object.type() == typeid(bool))
		return std::any_cast<bool>(object) ? "true" : "false";

	if (object.type() == typeid(double))
	{
		std::string text = std::to_string(std::any_cast<double>(object));
		text.erase(text.find_last_not_of('0') + 1, std::string::npos);

		if (!text.empty() && text.back() == '.') {
	        text.pop_back();
	    }
		return text;
	}

	if (object.type() == typeid(std::shared_ptr<Instance>))
        return std::any_cast<std::shared_ptr<Instance>>(object)->toString();

    if (object.type() == typeid(std::shared_ptr<Callable>))
        return std::any_cast<std::shared_ptr<Callable>>(object)->toString();
	
	return std::any_cast<std::string>(object);
}

void Interpreter::checkNumberOperand(const Token& op, const std::any& operand)
{
	if (operand.type() == typeid(double)) return;
	throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token& op, const std::any& left, const std::any& right)
{
	if (left.type() == typeid(double) && right.type() == typeid(double)) return;
	throw RuntimeError(op, "Operand must be numbers.");
}
