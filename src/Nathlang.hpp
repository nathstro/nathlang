#pragma once
#include "Resolver.hpp"
#include "Token.hpp"
#include "Interpreter.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>

class Nathlang
{
	private:
		static std::unordered_map<TokenType, std::string> tokenConvert;
		static Interpreter interpreter;
		static Resolver resolver();

	public:
		int run(std::string source);
		int runFile(const std::string& path);
		int runPrompt();
};