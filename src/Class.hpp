#pragma once
#include "Callable.hpp"
#include "Function.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Class : public Callable, public std::enable_shared_from_this<Class>
{
	public:
		std::string name;
		std::shared_ptr<Class> superclass;
		std::unordered_map<std::string, std::shared_ptr<Function>> methods;
		Class(std::string name, std::shared_ptr<Class> superclass, std::unordered_map<std::string, std::shared_ptr<Function>> methods)
			: name(name), superclass(std::move(superclass)), methods(std::move(methods)) {}
		std::shared_ptr<Function> findMethod(const std::string& name);

		int arity() override;
		std::string toString() override
		{
			return "<class " + name + ">";
		}
		std::any call(Interpreter& interpreter, const Token& token, const std::vector<std::any>& args) override;
};