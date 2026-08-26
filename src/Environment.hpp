#pragma once
#include "Token.hpp"
#include <any>
#include <memory>
#include <unordered_map>

class Environment : public std::enable_shared_from_this<Environment>
{
	public:
		Environment();
		Environment(std::shared_ptr<Environment> closing);
		
		void define(const std::string& name, const std::any& value);
		void assign(const Token& name, const std::any& value);
		void assignAt(int distance, const Token& name, const std::any& value);

		std::any get(const Token& name);
		std::any getAt(int distance, const std::string& name);
		std::shared_ptr<Environment> ancestor(int distance);
		std::shared_ptr<Environment> enclosing;

	private:
		std::unordered_map<std::string, std::any> values;
};