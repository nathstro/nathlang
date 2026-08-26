#include "StdLib.hpp"
#include "RuntimeError.hpp"
#include "Token.hpp"
#include <cmath>
#include <iostream>
#include <string>
#include <chrono>

void StdLib::registerAllNativeFunctions(Interpreter& interpreter)
{
	registerStringLib(interpreter);
	registerNumberLib(interpreter);
	registerUtilityLib(interpreter);
}

void StdLib::registerStringLib(Interpreter& interpreter)
{
	defineNative(interpreter, "len", 1,
		[](Interpreter&, const Token& token, const auto& args) -> std::any {
			if (!checkStringOperand(args[0]))
				throw RuntimeError(token, "Function len() expected string.");

			return static_cast<double>(std::any_cast<std::string>(args[0]).length());
		});

	defineNative(interpreter, "at", 2,
		[](Interpreter&, const Token& token, const auto& args) -> std::any {
			if (!checkStringOperand(args[0]) || !checkNumberOperand(args[1]))
				throw RuntimeError(token, "Function at() expected string and int.");

			double number = std::any_cast<double>(args[1]);

			if (std::floor(number) != number)
				throw RuntimeError(token, "Function at() expected string and int.");
			
			const std::string& str = std::any_cast<const std::string&>(args[0]);
			int index = int(number);

			if (index < 0 || index >= str.length())
				throw RuntimeError(token, "Function at() received out of bounds index.");

			std::string c(1, str.at(index));
			return c;
		});

	defineNative(interpreter, "slice", 3,
		[](Interpreter&, const Token& token, const auto& args) -> std::any {
			if (!checkStringOperand(args[0]) || !checkNumberOperand(args[1]) || !checkNumberOperand(args[2]))
				throw RuntimeError(token, "Function slice() expected string and int.");

			double lower = std::any_cast<double>(args[1]);
			double upper = std::any_cast<double>(args[2]);

			if (upper < lower)
				throw RuntimeError(token, "Function slice() received an upper index less than lower index.");

			if (std::floor(lower) != lower)
				throw RuntimeError(token, "Function slice() expected string and int.");

			if (std::floor(upper) != upper)
				throw RuntimeError(token, "Function slice() expected string and int.");
			
			const std::string& str = std::any_cast<const std::string&>(args[0]);
			int lowerIndex = int(lower);
			int upperIndex = int(upper);

			if (lowerIndex < 0 || upperIndex < 0 || upperIndex >= str.length())
				throw RuntimeError(token, "Function slice() received out of bounds index.");

			return str.substr(lower, upper - lower + 1);
		});
}

void StdLib::registerNumberLib(Interpreter& interpreter)
{
	defineNative(interpreter, "sqrt", 1,
		[](Interpreter&, const Token& token, const auto& args) -> std::any {
			if (!checkNumberOperand(args[0]))
				throw RuntimeError(token, "Function sqrt() expected number.");

			return std::sqrt(std::any_cast<double>(args[0]));
		});

	defineNative(interpreter, "pow", 2,
		[](Interpreter&, const Token& token, const auto& args) -> std::any {
			if (!checkNumberOperand(args[0]) || !checkNumberOperand(args[1]))
				throw RuntimeError(token, "Function power() expected numbers.");

			return std::pow(std::any_cast<double>(args[0]), std::any_cast<double>(args[1]));
		});
}

void StdLib::registerUtilityLib(Interpreter& interpreter)
{
	defineNative(interpreter, "input", 0,
		[](Interpreter&, const Token& token, const auto& args) -> std::any {
			std::string input;
			std::getline(std::cin, input);
			return input;
		});

	defineNative(interpreter, "time", 0,
		[](Interpreter&, const Token& token, const auto& args) -> std::any {
			auto now = std::chrono::system_clock::now();
			auto duration = now.time_since_epoch();
			return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
		});
}

void StdLib::defineNative(Interpreter& interpreter, const std::string& name, int arity, NativeFunction::FunctionDef function)
{
	std::shared_ptr<Callable> native = std::make_shared<NativeFunction>(arity, std::move(function));
	interpreter.globals()->define(name, native);
}

bool StdLib::checkStringOperand(std::any operand)
{
	return operand.type() == typeid(std::string);
}

bool StdLib::checkNumberOperand(std::any operand)
{
	return operand.type() == typeid(double);
}