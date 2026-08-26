#include "Scanner.hpp"
#include "Token.hpp"
#include "ErrorHandler.hpp"

std::vector<Token> Scanner::scanTokens()
{
	while (!atEnd())
	{
		start = current;
		scanToken();
	}

	if (tokens.back().type != TokenType::NEWLINE)
		tokens.emplace_back(TokenType::NEWLINE, "\n", line);

	tokens.emplace_back(TokenType::END, "", line);
	return tokens;
}

char Scanner::advance()
{
	return source.at(current++);
}

char Scanner::peek()
{
	if (atEnd()) return '\0';
	return source.at(current);
}

char Scanner::peekNext()
{
	if (current + 1 >= source.length()) return '\0';
	return source.at(current + 1);
}

bool Scanner::isDigit(char c)
{
	return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

bool Scanner::isAlnum(char c)
{
	return isDigit(c) || isAlpha(c);
}

void Scanner::scanToken()
{
	char c = advance();
	switch (c)
	{
		case '#': comment(); break;
		case '(': 
			addToken(TokenType::LEFT_PAREN);
			ignoreNewlines = true;
			break;
		case ')':
			addToken(TokenType::RIGHT_PAREN);
			ignoreNewlines = false;
			break;
		case '{':
			addToken(TokenType::LEFT_BRACE);
			ignoreNewlines = true;
			break;
		case '}':
			addToken(TokenType::RIGHT_BRACE);
			ignoreNewlines = false;
			break;
		case ',': addToken(TokenType::COMMA); break;
		case ':': addToken(TokenType::COLON); break;
		case '.': addToken(TokenType::DOT); break;
		case '=': addToken(match('=') ? TokenType::EQ : TokenType::ASSIGN); break;
		case '!': addToken(match('=') ? TokenType::NE : TokenType::NOT); break;
		case '+': addToken(match('=') ? TokenType::PE : TokenType::PLUS); break;
		case '-': addToken(match('=') ? TokenType::ME : TokenType::MINUS); break;
		case '*': addToken(match('=') ? TokenType::SE : TokenType::STAR); break;
		case '/': addToken(match('=') ? TokenType::DE : TokenType::SLASH); break;
		case '<': addToken(match('=') ? TokenType::LE : TokenType::LESSER); break;
		case '>': addToken(match('=') ? TokenType::GE : TokenType::GREATER); break;

		case ' ': 
		case '\r':
		case '\t':
			break;

		case '\n':
			if (!tokens.empty() && tokens.back().type != TokenType::NEWLINE && !ignoreNewlines)
				addToken(TokenType::NEWLINE);
			line++;
			break;

		case '"': string(); break;

		default:
			if (isDigit(c))
				number();
			else if (isAlpha(c))
				identifier();
			else
				ErrorHandler::error(line, "Unexpected character. ");
			break; 
	}
}

void Scanner::string()
{
	while (peek() != '"' && !atEnd())
	{
		if (peek() == '\n') line++;
		advance();
	}

	if (atEnd())
	{
		ErrorHandler::error(line, "Unclosed string. ");
		return;
	}

	advance();

	std::string value = source.substr(start + 1, current - start - 2);
	addToken(TokenType::STRING, value);
}

void Scanner::number()
{
	while (isDigit(peek())) advance();

	if (peek() == '.' && isDigit(peekNext()))
	{
		advance();
		while (isDigit(peek())) advance();
	}
	
	double value = std::stod(source.substr(start, current - start));
	addToken(TokenType::NUM, value);
}

void Scanner::identifier()
{
	while (isAlnum(peek()))
		advance();

	std::string value = source.substr(start, current - start);
	if (keywords.count(value) > 0)
		addToken(keywords.at(value));
	else
		addToken(TokenType::IDENTIFIER, value);
}

void Scanner::comment()
{
	while (peek() != '\n' && !atEnd())
	{
		advance();
	}
}

bool Scanner::match(char expected)
{
	if (atEnd()) return false;
	if (source.at(current) != expected) return false;

	current++;
	return true;
}

void Scanner::addToken(TokenType type)
{
	std::string text = source.substr(start, current - start);
	tokens.emplace_back(type, text, line);
}

void Scanner::addToken(TokenType type, double numconst)
{
	std::string text = source.substr(start, current - start);
	tokens.emplace_back(type, text, line, numconst);
}

void Scanner::addToken(TokenType type, std::string strconst)
{
	std::string text = source.substr(start, current - start);
	tokens.emplace_back(type, text, line, strconst);
}


