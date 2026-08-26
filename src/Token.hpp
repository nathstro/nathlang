#pragma once
#include <any>
#include <string>

enum class TokenType {
	NEWLINE,							// \n ;
	LEFT_PAREN, RIGHT_PAREN, 			// ( )
	LEFT_BRACE, RIGHT_BRACE,			// { }
	COMMA, COLON, DOT, ASSIGN,			// , : ; =
	EQ, NE, GREATER, GE, LESSER, LE,	// == != > >= < <=
	PLUS, MINUS, PE, ME,				// + - += -= .
	STAR, SLASH, SE, DE,				// * / *= /=
	
	IDENTIFIER, STRING, NUM, 			// literals

	AND, OR, NOT, NIL, TRUE, FALSE, LET,
	IF, THEN, ELSE, ENDIF,
	FOR, ENDFOR,
	WHILE, DO, ENDWHILE,
	FUNC, RETURN, ENDFUNC,
	METHOD, ENDMETHOD,
	CLASS, ENDCLASS, THIS, EXTENDS, SUPER,
	PRINT, 								// keywords 
	
	END
};

struct Token
{
	TokenType type;
	std::string lexeme;
	int line;
	std::any literal;

	Token(TokenType type, const std::string& lexeme, int line)
		: type(type), lexeme(lexeme), line(line)
		{}
	Token(TokenType type, const std::string& lexeme, int line, const std::any& literal)
		: type(type), lexeme(lexeme), line(line), literal(literal)
		{}
};