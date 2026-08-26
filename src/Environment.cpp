#include "Environment.hpp"
#include "RuntimeError.hpp"
#include <memory>

Environment::Environment() : enclosing(nullptr)
{}

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(std::move(enclosing))
{}

void Environment::define(const std::string& name, const std::any& value)
{
	values.emplace(name, value);
}

void Environment::assign(const Token& name, const std::any& value)
{
	auto var = values.find(name.lexeme);

	if (var != values.end())
	{
		var->second = value;
		return;
	}

	if (enclosing)
	{
		enclosing->assign(name, value);
		return;
	}

	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assignAt(int distance, const Token& name, const std::any& value)
{
	ancestor(distance)->values[name.lexeme] = value;
}

std::any Environment::get(const Token& name)
{
	auto it = values.find(name.lexeme);

	if (it != values.end())
	{
		return it->second;
	}

	if (enclosing)
	{
		return enclosing->get(name);
	}

	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

std::any Environment::getAt(int distance, const std::string& name)
{
	return ancestor(distance)->values[name];
}

std::shared_ptr<Environment> Environment::ancestor(int distance)
{
	std::shared_ptr<Environment> environment = shared_from_this();
	for (int i = 0; i < distance; i++)
	{
		environment = environment->enclosing;
	}

	return environment;
}
