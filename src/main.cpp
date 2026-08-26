#include "Nathlang.hpp"
#include <iostream>

int main(int argc, char const *argv[])
{
	Nathlang nathlang;
	bool hadError = false;

	if (argc > 2)
	{
		std::cout << "Usage: nathlang [script]" << std::endl;
		return 64;
	}
	else if (argc == 2)
	{
		return nathlang.runFile(argv[1]);
	}
	else
	{
		return nathlang.runPrompt();
	}

	return 0;
}