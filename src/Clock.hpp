#pragma once
#include "Callable.hpp"
#include <chrono>

class Clock : public Callable
{
	public:

		int arity() override
		{
			return 0;
		}
		std::any call(Interpreter& intepreter, const std::vector<std::any>& args) override
		{

			auto now = std::chrono::system_clock::now();
			auto duration = now.time_since_epoch();
			return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
		}
		std::string toString() override
		{
			return "<native fn clock>";
		}
};