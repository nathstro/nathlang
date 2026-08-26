#pragma once
#include "Token.hpp"
#include <any>
#include <memory>


class Class;

class Instance : public std::enable_shared_from_this<Instance>
{
	public:
		Instance(std::shared_ptr<Class> originClass) : originClass(std::move(originClass)) {}
		std::string toString();
		std::any get(const Token& name);
		void set(const Token& name, const std::any& value);

	private:
		std::shared_ptr<Class> originClass;
		std::unordered_map<std::string, std::any> fields;
};