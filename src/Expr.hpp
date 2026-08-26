#pragma once
#include "Token.hpp"
#include <any>
#include <memory>

struct Binary;
struct Unary;
struct Logical;
struct Grouping;
struct Literal;
struct Variable;
struct Assign;
struct Call;
struct Get;
struct Set;
struct This;
struct Super;

struct Expr {
	template<typename R>
	struct ExprVisitor {
		~ExprVisitor() = default;

		virtual R visitBinary(const Binary& expr) = 0;
		virtual R visitUnary(const Unary& expr) = 0;
		virtual R visitLogical(const Logical& expr) = 0;
		virtual R visitGrouping(const Grouping& expr) = 0;
		virtual R visitLiteral(const Literal& expr) = 0;
		virtual R visitVariable(const Variable& expr) = 0;
		virtual R visitAssign(const Assign& expr) = 0;
		virtual R visitCall(const Call& expr) = 0;
		virtual R visitGet(const Get& expr) = 0;
		virtual R visitSet(const Set& expr) = 0;
		virtual R visitThis(const This& expr) = 0;
		virtual R visitSuper(const Super& expr) = 0;
	};
	
	virtual ~Expr() = default;
	virtual std::any accept(ExprVisitor<std::any>& visitor) const = 0;
};

struct Binary : public Expr {
	Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
	: left(std::move(left)), op(std::move(op)), right(std::move(right))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitBinary(*this);
	}

	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;
};

struct Unary : public Expr {
	Unary(Token op, std::unique_ptr<Expr> right)
	: op(std::move(op)), right(std::move(right))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitUnary(*this);
	}

	Token op;
	std::unique_ptr<Expr> right;
};

struct Logical : public Expr {
	Logical(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
	: left(std::move(left)), op(std::move(op)), right(std::move(right))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitLogical(*this);
	}

	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;
};

struct Grouping : public Expr {
	Grouping(std::unique_ptr<Expr> expression)
	: expression(std::move(expression))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitGrouping(*this);
	}

	std::unique_ptr<Expr> expression;
};

struct Literal : public Expr {
	Literal(std::any value)
	: value(std::move(value))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitLiteral(*this);
	}

	std::any value;
};

struct Variable : public Expr {
	Variable(Token name)
	: name(name)
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitVariable(*this);
	}

	Token name;
};

struct Assign : public Expr {
	Assign(Token name, std::unique_ptr<Expr> value)
	: name(name), value(std::move(value))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitAssign(*this);
	}

	Token name;
	std::unique_ptr<Expr> value;
};

struct Call : public Expr {
	Call(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> args)
	: callee(std::move(callee)), paren(paren), args(std::move(args))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitCall(*this);
	}

	std::unique_ptr<Expr> callee;
	Token paren;
	std::vector<std::unique_ptr<Expr>> args;
};

struct Get : public Expr {
	Get(std::unique_ptr<Expr> object, Token name)
	: object(std::move(object)), name(name)
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitGet(*this);
	}

	std::unique_ptr<Expr> object;
	Token name;
};

struct Set : public Expr {
	Set(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value)
	: object(std::move(object)), name(name), value(std::move(value))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitSet(*this);
	}

	std::unique_ptr<Expr> object;
	Token name;
	std::unique_ptr<Expr> value;
};

struct This : public Expr {
	This(Token keyword)
	: keyword(keyword)
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitThis(*this);
	}

	Token keyword;
};

struct Super : public Expr {
	Super(Token keyword, Token method)
	: keyword(keyword), method(method)
	{}

	std::any accept(ExprVisitor<std::any>& visitor) const {
		return visitor.visitSuper(*this);
	}

	Token keyword;
	Token method;
};
