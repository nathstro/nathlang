#include "Parser.hpp"
#include "ErrorHandler.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <cstddef>
#include <initializer_list>
#include <utility>
#include <memory>
#include <vector>

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
	std::vector<std::unique_ptr<Stmt>> statements;

	while (!atEnd())
	{
		statements.push_back(declaration());
	}

	return statements;
}

std::unique_ptr<Expr> Parser::expression()
{
	return assignment();
}

std::unique_ptr<Expr> Parser::assignment()
{
	std::unique_ptr<Expr> expr = orExpression();

	if (match(TokenType::ASSIGN))
	{
		Token equals = previous();
		std::unique_ptr<Expr> value = assignment();

		if (auto* variable = dynamic_cast<Variable*>(expr.get()))
		{
			return std::make_unique<Assign>(variable->name, std::move(value));
		}
		else if (auto* get = dynamic_cast<Get*>(expr.get()))
		{
			return std::make_unique<Set>(std::move(get->object), get->name, std::move(value));
		}

		error(equals, "Invalid assignment target.");
	}

	return expr;
}

std::unique_ptr<Expr> Parser::orExpression()
{
	std::unique_ptr<Expr> expr = andExpression();

	while(match(TokenType::OR))
	{
		Token op = previous();
		std::unique_ptr<Expr> right = andExpression();
		expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::andExpression()
{
	std::unique_ptr<Expr> expr = equality();

	while(match(TokenType::AND))
	{
		Token op = previous();
		std::unique_ptr<Expr> right = equality();
		expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Stmt> Parser::statement()
{
	if (match(TokenType::IF)) return ifStatement();
	if (match(TokenType::PRINT)) return printStatement();
	if (match(TokenType::RETURN)) return returnStatement();
	if (match(TokenType::WHILE)) return whileStatement();
	if (match(TokenType::FOR)) return forStatement();
	if (match(TokenType::LEFT_BRACE)) return std::make_unique<BlockStmt>(block());

	return expressionStatement();
}

std::unique_ptr<Stmt> Parser::ifStatement()
{
	std::unique_ptr<Expr> condition = expression();
	consume(TokenType::THEN, "Expect 'then' after if condition");

	std::unique_ptr<Stmt> thenBranch = std::make_unique<BlockStmt>(
		blockUntil({TokenType::ELSE, TokenType::ENDIF})
	);

	std::unique_ptr<Stmt> elseBranch;

	if (match(TokenType::ELSE))
	{
		elseBranch = std::make_unique<BlockStmt>(
			blockUntil({TokenType::ELSE, TokenType::ENDIF})
		);
	}

	consume(TokenType::ENDIF, "Expect 'endif' after if statement.");
	return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::printStatement()
{
	std::unique_ptr<Expr> value = expression();
	consume(TokenType::NEWLINE, "Expect newline after print statement.");
	return std::make_unique<PrintStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::whileStatement()
{
	std::unique_ptr<Expr> condition = expression();
	consume(TokenType::DO, "Expect 'do' after while condition.");
	skipNewlines();

	std::unique_ptr<Stmt> body = std::make_unique<BlockStmt>(
		blockUntil({TokenType::ENDWHILE})
	);

	consume(TokenType::ENDWHILE, "Expect 'endwhile' after while body.");
	skipNewlines();
	return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::forStatement()
{
	Token it = consume(TokenType::IDENTIFIER, "Expect loop variable name");
	consume(TokenType::ASSIGN, "Expect '=' after loop variable.");
	std::unique_ptr<Expr> start = expression();

	consume(TokenType::COLON, "Expect ':' after loop start");
	std::unique_ptr<Expr> end = expression();

	std::unique_ptr<Expr> step = std::make_unique<Literal>(1.0);
	if (match(TokenType::COLON))
	{
		step = expression();
	}

	consume(TokenType::NEWLINE, "Expect newline after loop increment");
	std::vector<std::unique_ptr<Stmt>> body = blockUntil({TokenType::ENDFOR});

	consume(TokenType::ENDFOR, "Expect 'endfor' after loop body.");
	skipNewlines();
	std::unique_ptr<VarStmt> initialiser = std::make_unique<VarStmt>(
		it, std::move(start)
	);

	std::unique_ptr<Binary> condition = std::make_unique<Binary>(
		std::make_unique<Variable>(it), Token(TokenType::LE, "<=", it.line), std::move(end)
	);

	std::unique_ptr<ExpressionStmt> increment = std::make_unique<ExpressionStmt>(
		std::make_unique<Assign>(
			it, std::make_unique<Binary>(
				std::make_unique<Variable>(it), Token(TokenType::PLUS, "+", it.line), std::move(step)
			)
		)
	);

	body.push_back(std::move(increment));

	std::unique_ptr<WhileStmt> whileStmt = std::make_unique<WhileStmt>(
		std::move(condition), std::move(std::make_unique<BlockStmt>(std::move(body)))
	);

	std::vector<std::unique_ptr<Stmt>> statements;
	statements.push_back(std::move(initialiser));
	statements.push_back(std::move(whileStmt));

	return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::expressionStatement()
{
	std::unique_ptr<Expr> expr = expression();
	consume(TokenType::NEWLINE, "Expect newline after expression.");
	return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::returnStatement()
{
	Token keyword = previous();
	std::unique_ptr<Expr> value;

	if (!check(TokenType::NEWLINE))
	{
		value = expression();
	}

	consume(TokenType::NEWLINE, "Expect newline after return value.");
	return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::vector<std::unique_ptr<Stmt>> Parser::block()
{
	std::vector<std::unique_ptr<Stmt>> statements;
	skipNewlines();

	while (!check(TokenType::RIGHT_BRACE) && !atEnd())
	{
		statements.push_back(declaration());
	}

	consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");

	skipNewlines();
	return statements;
}

std::vector<std::unique_ptr<Stmt>> Parser::blockUntil(std::initializer_list<TokenType> terminators)
{
	std::vector<std::unique_ptr<Stmt>> statements;
	bool finished = false;

	while (!atEnd())
	{
		for (TokenType t : terminators)
		{
			if (check(t))
			{
				return statements;
			}
		}

		statements.push_back(declaration());
	}

	return statements;
}

std::unique_ptr<Stmt> Parser::declaration()
{
	try
	{
		if (match(TokenType::CLASS)) return classDeclaration();
		if (match(TokenType::FUNC)) return function("function");
		if (match(TokenType::LET)) return varDeclaration();
		return statement();
	}
	catch (ParseError error)
	{
		synchronise();
		return NULL;
	}
}

std::unique_ptr<Stmt> Parser::varDeclaration()
{
	Token name = consume(TokenType::IDENTIFIER, "Expect variable name");
	
	std::unique_ptr<Expr> initialiser;
	if (match(TokenType::ASSIGN))
	{
		initialiser = expression();
	}

	consume(TokenType::NEWLINE, "Expect newline after variable expression.");
	return std::make_unique<VarStmt>(name, std::move(initialiser));
}

std::unique_ptr<Stmt> Parser::classDeclaration()
{
	Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
	std::unique_ptr<Variable> superclass;

	if (match(TokenType::EXTENDS))
	{
		consume(TokenType::IDENTIFIER, "Expect superclass name.");
		superclass = std::make_unique<Variable>(previous());
	}

	skipNewlines();

	std::vector<std::unique_ptr<FunctionStmt>> methods;
	while (!check(TokenType::ENDCLASS) && !atEnd())
	{
		consume(TokenType::METHOD, "Expect 'method' before method body.");
		methods.push_back(function("method"));
		skipNewlines();
	}

	consume(TokenType::ENDCLASS, "Expect 'endclass' after class body.");
	skipNewlines();
	return std::make_unique<ClassStmt>(name, std::move(superclass), std::move(methods));
}

std::unique_ptr<FunctionStmt> Parser::function(const std::string& kind)
{
	Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " .");
	consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

	std::vector<Token> parameters;
	if (!check(TokenType::RIGHT_PAREN))
	{
		do
		{
			if (parameters.size() >= 255)
			{
				error(peek(), "Can't have more than 255 parameters.");
			}

			parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
		} while (match(TokenType::COMMA));
	}

	consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
	skipNewlines();
	std::vector<std::unique_ptr<Stmt>> body;

	if (kind == "method")
	{
		body = blockUntil({TokenType::ENDMETHOD});
		consume(TokenType::ENDMETHOD, "Expect 'endmethod' after method body.");
	}
	else
	{
		body = blockUntil({TokenType::ENDFUNC});
		consume(TokenType::ENDFUNC, "Expect 'endfunc' after function body.");
	}
	
	skipNewlines();
	return std::make_unique<FunctionStmt>(name, std::move(parameters), std::move(body));
}

std::unique_ptr<Expr> Parser::equality()
{
	std::unique_ptr<Expr> expr = comparison();

	while (match(TokenType::NE, TokenType::EQ)) {
		Token op = previous();
		auto right = comparison();

		expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::comparison()
{
	std::unique_ptr<Expr> expr = term();

	while (match(TokenType::GREATER, TokenType::GE, TokenType::LESSER, TokenType::LE)) {
		Token op = previous();
		auto right = term();

		expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::term()
{
	std::unique_ptr<Expr> expr = factor();

	while (match(TokenType::PLUS, TokenType::MINUS)) {
		Token op = previous();
		auto right = factor();

		expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::factor()
{
	std::unique_ptr<Expr> expr = unary();

	while (match(TokenType::STAR, TokenType::SLASH)) {
		Token op = previous();
		auto right = unary();

		expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::unary()
{
	if (match(TokenType::NOT, TokenType::MINUS)) {
		Token op = previous();
		auto right = unary();

		return std::make_unique<Unary>(op, std::move(right));
	}

	return call();
}

std::unique_ptr<Expr> Parser::call()
{
	std::unique_ptr<Expr> expr = primary();

	while (true)
	{
		if (match(TokenType::LEFT_PAREN))
		{
			expr = finishCall(std::move(expr));
		}
		else if (match(TokenType::DOT))
		{
			Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
			expr = std::make_unique<Get>(std::move(expr), name);
		}
		else
		{
			break;
		}
	}

	return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee)
{
	std::vector<std::unique_ptr<Expr>> args;

	if (!check(TokenType::RIGHT_PAREN))
	{
		do
		{
			if (args.size() >= 255)
			{
				error(peek(), "Can't have more than 255 args.");
			}
			args.push_back(expression());
		}
		while (match(TokenType::COMMA));
	}

	Token paren = consume(TokenType::RIGHT_PAREN, "Expect ',' after arguments");
	return std::make_unique<Call>(std::move(callee), paren, std::move(args));
}

std::unique_ptr<Expr> Parser::primary()
{
	if (match(TokenType::FALSE)) return std::make_unique<Literal>(false);
	if (match(TokenType::TRUE)) return std::make_unique<Literal>(true);
	if (match(TokenType::NIL)) return std::make_unique<Literal>(NULL);
	if (match(TokenType::THIS)) return std::make_unique<This>(previous());
	if (match(TokenType::SUPER))
	{
		Token keyword = previous();
		consume(TokenType::DOT, "Expect '.' after super.");
		Token method = consume(TokenType::IDENTIFIER, "Expect superclass method after '.'");
		return std::make_unique<Super>(keyword, method);
	}

	if (match(TokenType::NUM, TokenType::STRING)) return std::make_unique<Literal>(previous().literal);
	if (match(TokenType::IDENTIFIER)) return std::make_unique<Variable>(previous());
	if (match(TokenType::LEFT_PAREN)) {
		std::unique_ptr<Expr> expr = expression();
		consume(TokenType::RIGHT_PAREN, "A ')' was expected in the expression.");
		return std::make_unique<Grouping>(std::move(expr));
	}

	throw error(peek(), "Expect expression");
}

bool Parser::check(TokenType type)
{
	if (atEnd()) return false;
	return peek().type == type;
}

bool Parser::atEnd()
{
	return peek().type == TokenType::END;
}

Token Parser::peek() const
{
	return tokens.at(current);
}

Token Parser::previous() const
{
	return tokens.at(current - 1);
}

Token Parser::advance()
{
	if (!atEnd()) current++;
	return previous();
}

Token Parser::consume(TokenType type, std::string message)
{
	if (check(type)) return advance();

	throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, const std::string message)
{
	ErrorHandler::error(token, message);
	return ParseError();
}

void Parser::skipNewlines()
{
	while (match(TokenType::NEWLINE)) {}
}

void Parser::synchronise()
{
	advance();

	while (!atEnd())
	{
		if (previous().type == TokenType::NEWLINE) return;

		switch (peek().type)
		{
			case TokenType::FUNC:
			case TokenType::LET:
			case TokenType::FOR:
			case TokenType::WHILE:
			case TokenType::IF:
			case TokenType::PRINT:
			case TokenType::RETURN:
				return;
			default:
				break;
		}

		advance();
	}
}





