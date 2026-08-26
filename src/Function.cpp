#include "Function.hpp"
#include "Environment.hpp"
#include "Return.hpp"
#include <memory>

std::shared_ptr<Function> Function::bind(std::shared_ptr<Instance> instance)
{
	std::shared_ptr<Environment> environment = std::make_shared<Environment>(closure);
	environment->define("this", instance);
	return std::make_shared<Function>(declaration, environment, isInitialiser);
}

std::any Function::call(Interpreter& interpreter, const Token& token, const std::vector<std::any>& args)
{
	std::shared_ptr<Environment> environment = std::make_shared<Environment>(closure);

	for (int i = 0; i < declaration.params.size(); i++)
	{
		environment->define(declaration.params[i].lexeme, args[i]);
	}
	try
	{
		interpreter.executeBlock(declaration.body, environment);
	}
	catch (const Return& returnValue)
	{
		if (!isInitialiser) return returnValue.value;
	}

	if (isInitialiser) return closure->getAt(0, "this");
	return {};
}

int Function::arity()
{
	return declaration.params.size();
}

std::string Function::toString()
{
	return "<fn " + declaration.name.lexeme + ">";
}
