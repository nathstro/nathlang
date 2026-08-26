#include "Nathlang.hpp"
//#include "AstPrinter.hpp"
#include "AstPrinter.hpp"
#include "ErrorHandler.hpp"
#include "Parser.hpp"
#include "Token.hpp"
#include "Scanner.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <unordered_map>

std::unordered_map<TokenType, std::string> Nathlang::tokenConvert;
Interpreter Nathlang::interpreter;

int Nathlang::run(std::string source)
{
	Scanner scanner(source);

	std::vector<Token> tokens = scanner.scanTokens();
	Parser parser(tokens);
	std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

	if (ErrorHandler::hadError()) return 65;

	Resolver resolver(interpreter);
	resolver.resolve(statements);
	
	if (ErrorHandler::hadError()) return 65;
	if (ErrorHandler::hadRuntimeError()) return 70;

	interpreter.interpret(statements);
	return 0;
}

int Nathlang::runFile(const std::string& path)
{
	try
	{
		std::ifstream file(path);

		if (!file)
			throw std::runtime_error("Could not open script");

		std::string fileContents{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
		
		return run(fileContents);
	}
	catch (const std::exception& ex)
	{
		std::cout << "ERROR: " << ex.what() << std::endl;
		return 1;
	}
}

int Nathlang::runPrompt()
{
	std::string line;
	int status;

	while (true)
	{
		std::cout << "> ";

		if (!std::getline(std::cin, line)) break;
		if (line == "") break;

		status = run(line);
		if (status != 0) return status;
	}

	return 0;
}
