#pragma once
#include "Callable.hpp"
#include "Environment.hpp"
#include "Instance.hpp"
#include "Stmt.hpp"
#include <memory>

class Function : public Callable
{
	public:
		Function(const FunctionStmt& declaration, std::shared_ptr<Environment> closure, bool isInitialiser)
		: declaration(declaration), closure(std::move(closure)), isInitialiser(isInitialiser)
		{}
		std::shared_ptr<Function> bind(std::shared_ptr<Instance> instance);
		std::any call(Interpreter& intepreter, const Token& token, const std::vector<std::any>& args) override;
		int arity() override;
		std::string toString() override;

	private:
		const FunctionStmt& declaration;
		std::shared_ptr<Environment> closure;
		bool isInitialiser;
};