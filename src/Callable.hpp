#pragma once
#include "Interpreter.hpp"
#include <string>

class Callable
{
	public:
		virtual ~Callable() = default;
		virtual int arity() = 0;
		virtual std::any call(Interpreter& intepreter, const Token& token, const std::vector<std::any>& args) = 0;
		virtual std::string toString() = 0;
};