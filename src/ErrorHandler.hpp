#pragma once
#include "RuntimeError.hpp"
#include "Token.hpp"
#include <string>

class ErrorHandler
{
	private:
		static bool hadError_;
		static bool hadRuntimeError_;
		static void report(int line, const std::string where, const std::string message);

	public:
		ErrorHandler() {};
		static bool hadError();
		static bool hadRuntimeError();
		static void error(int line, const std::string message);
		static void error(const Token& token, const std::string message);
		static void runtimeError(RuntimeError error);
};