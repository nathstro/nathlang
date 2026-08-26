#include "Instance.hpp"
#include "Class.hpp"
#include "RuntimeError.hpp"
#include <memory>

std::string Instance::toString()
{
	return "<" + originClass->name + " instance>";
}

std::any Instance::get(const Token& name)
{
	auto it = fields.find(name.lexeme);

	if (it != fields.end())
		return it->second; 

	auto method = originClass->findMethod(name.lexeme);
	if (method)
		return std::shared_ptr<Callable>(method->bind(shared_from_this()));

	throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void Instance::set(const Token& name, const std::any& value)
{
	fields[name.lexeme] = value;
}