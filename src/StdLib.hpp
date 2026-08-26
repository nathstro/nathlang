#pragma once
#include "Callable.hpp"
#include "Interpreter.hpp"
#include "Token.hpp"
#include <functional>
#include <string>

class NativeFunction : public Callable
{
	public:
		using FunctionDef = std::function<std::any(Interpreter&, const Token&, const std::vector<std::any>&)>;

		NativeFunction(int arity, FunctionDef function) : arity_(arity), function_(function)
		{}

		int arity() override
		{
			return arity_;
		}
		std::any call(Interpreter& intepreter, const Token& token,  const std::vector<std::any>& args) override
		{
			return function_(intepreter, token, args);
		}
		std::string toString() override
		{
			return "<native fn>";
		}
	private:
		int arity_;
		FunctionDef function_;
};

namespace StdLib
{
	void registerAllNativeFunctions(Interpreter& interpreter);
	void registerStringLib(Interpreter& interpreter);
	void registerNumberLib(Interpreter& interpreter);
	void registerUtilityLib(Interpreter& interpreter);
	void defineNative(Interpreter& interpreter, const std::string& name, int arity, NativeFunction::FunctionDef function);

	bool checkStringOperand(std::any operand);
	bool checkNumberOperand(std::any operand);
}