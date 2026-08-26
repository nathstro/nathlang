#include "ErrorHandler.hpp"
#include "Token.hpp"
#include <iostream>
#include <string>

bool ErrorHandler::hadError_ = false;
bool ErrorHandler::hadRuntimeError_ = false;

bool ErrorHandler::hadError()
{
	return hadError_;
}

bool ErrorHandler::hadRuntimeError()
{
	return hadRuntimeError_;
}

void ErrorHandler::report(int line, const std::string where, const std::string message)
{
	std::cout << "[line " << line << "] Error" << where << ": " << message << std::endl;
	hadError_ = true;
}

void ErrorHandler::error(const int line, const std::string message)
{
	report(line, "", message);
}

void ErrorHandler::error(const Token& token, const std::string message)
{
	if (token.type == TokenType::END)
		report(token.line, " at end", message);
	else if (token.type == TokenType::NEWLINE)
		report(token.line, " at newline", message);
	else
		report(token.line, " at '" + token.lexeme + "'", message);
}

void ErrorHandler::runtimeError(RuntimeError error)
{
	std::cout << error.what() << std::endl;
	hadRuntimeError_ = true;
}