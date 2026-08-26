#include "Class.hpp"
#include "Instance.hpp"
#include <memory>

std::any Class::call(Interpreter& interpreter, const std::vector<std::any>& args)
{
	std::shared_ptr<Instance> instance = std::make_shared<Instance>(shared_from_this());
	std::shared_ptr<Function> initialiser = findMethod("CONSTRUCTOR");
	if (initialiser)
	{
		initialiser->bind(instance)->call(interpreter, args);
	}

	return instance;
}

int Class::arity()
{
	std::shared_ptr<Function> initialiser = findMethod("CONSTRUCTOR");
	if (!initialiser)
		return 0;

	return initialiser->arity();
}

std::shared_ptr<Function> Class::findMethod(const std::string& name)
{
	auto it = methods.find(name);

	if (it != methods.end())
		return it->second;

	if (superclass)
	{
		return superclass->findMethod(name);
	}

	return nullptr;
}
