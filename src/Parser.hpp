#pragma once
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <algorithm>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class Parser
{
	public:
		Parser(std::vector<Token> tokens) : tokens(tokens) {};
		std::vector<std::unique_ptr<Stmt>> parse();

	private:
		class ParseError : public std::runtime_error
		{
			public:
				ParseError() : std::runtime_error("Parse error") {}
		};
		ParseError error(const Token& token, const std::string message);

		const std::vector<Token> tokens;
		int current = 0;
		
		bool atEnd();
		Token peek() const;
		Token previous() const;
		Token advance();
		Token consume(TokenType type, std::string message);

		template<typename... Types>
		bool match(Types... types);

		bool check(TokenType type);

		std::unique_ptr<Stmt> statement();
		std::unique_ptr<Stmt> ifStatement();
		std::unique_ptr<Stmt> printStatement();
		std::unique_ptr<Stmt> whileStatement();
		std::unique_ptr<Stmt> forStatement();
		std::unique_ptr<Stmt> expressionStatement();
		std::unique_ptr<Stmt> returnStatement();
		std::vector<std::unique_ptr<Stmt>> block();
		std::vector<std::unique_ptr<Stmt>> blockUntil(std::initializer_list<TokenType> terminators);
		std::unique_ptr<Stmt> declaration();
		std::unique_ptr<Stmt> varDeclaration();
		std::unique_ptr<Stmt> classDeclaration();
		std::unique_ptr<FunctionStmt> function(const std::string& kind);

		std::unique_ptr<Expr> expression();
		std::unique_ptr<Expr> assignment();
		std::unique_ptr<Expr> orExpression();
		std::unique_ptr<Expr> andExpression();
		std::unique_ptr<Expr> equality();
		std::unique_ptr<Expr> comparison();
		std::unique_ptr<Expr> term();
		std::unique_ptr<Expr> factor();
		std::unique_ptr<Expr> unary();
		std::unique_ptr<Expr> call();
		std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee);
		std::unique_ptr<Expr> primary();

		void skipNewlines();
		void synchronise();
};

template<typename... Types>
bool Parser::match(Types... types)
{
	std::initializer_list<bool> results{check(types)...};

	for (bool r : results) {
		if (r) {
			advance();
			return true;
		}
	}
	return false;
}

/*
expression     → equality ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")" ;
*/