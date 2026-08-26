#pragma once
#include "Token.hpp"
#include <stdexcept>

class RuntimeError : public std::runtime_error
{
	public:
		RuntimeError(const Token& token, std::string message) : std::runtime_error(message), token(token) {}
	private:
		Token token;
};