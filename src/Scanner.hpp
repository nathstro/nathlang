#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include "Token.hpp"

class Scanner 
{
	private:
		std::string source;
		std::vector<Token> tokens;
		std::unordered_map<std::string, TokenType> keywords;
		bool ignoreNewlines;

		int start = 0;
		int current = 0;
		int line = 1;

		bool atEnd() { return current >= source.length(); };
		char advance();
		bool match(char expected);
		char peek();
		char peekNext();
		bool isDigit(char c);
		bool isAlpha(char c);
		bool isAlnum(char c);

		void string();
		void number();
		void identifier();
		void comment();

		void scanToken();
		void addToken(TokenType type);
		void addToken(TokenType type, double numconst);
		void addToken(TokenType type, std::string strconst);

	public:
		Scanner(std::string source) : source(source), ignoreNewlines(false)
		{
			keywords.emplace("and", TokenType::AND);
			keywords.emplace("or", TokenType::OR);
			keywords.emplace("not", TokenType::NOT);
			keywords.emplace("null", TokenType::NIL);
			keywords.emplace("true", TokenType::TRUE);
			keywords.emplace("false", TokenType::FALSE);
			keywords.emplace("let", TokenType::LET);
			keywords.emplace("if", TokenType::IF);
			keywords.emplace("then", TokenType::THEN);
			keywords.emplace("else", TokenType::ELSE);
			keywords.emplace("endif", TokenType::ENDIF);
			keywords.emplace("for", TokenType::FOR);
			keywords.emplace("endfor", TokenType::ENDFOR);
			keywords.emplace("while", TokenType::WHILE);
			keywords.emplace("do", TokenType::DO);
			keywords.emplace("endwhile", TokenType::ENDWHILE);
			keywords.emplace("func", TokenType::FUNC);
			keywords.emplace("return", TokenType::RETURN);
			keywords.emplace("endfunc", TokenType::ENDFUNC);
			keywords.emplace("method", TokenType::METHOD);
			keywords.emplace("endmethod", TokenType::ENDMETHOD);
			keywords.emplace("class", TokenType::CLASS);
			keywords.emplace("endclass", TokenType::ENDCLASS);
			keywords.emplace("this", TokenType::THIS);
			keywords.emplace("super", TokenType::SUPER);
			keywords.emplace("extends", TokenType::EXTENDS);
			keywords.emplace("print", TokenType::PRINT);
		};
		std::vector<Token> scanTokens();
};