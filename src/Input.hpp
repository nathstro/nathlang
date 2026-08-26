#pragma once
#include "Callable.hpp"
#include <iostream>
#include <string>

class Input : public Callable
{
	public:

		int arity() override
		{
			return 0;
		}
		std::any call(Interpreter& intepreter, const std::vector<std::any>& args) override
		{
			std::string input;
			std::getline(std::cin, input);
			return input;
		}
		std::string toString() override
		{
			return "<native fn input>";
		}
};