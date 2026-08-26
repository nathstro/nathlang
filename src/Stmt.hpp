#pragma once
#include "Expr.hpp"
#include <memory>

struct PrintStmt;
struct ExpressionStmt;
struct VarStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct FunctionStmt;
struct ReturnStmt;
struct ClassStmt;

struct Stmt {
	template<typename R>
	struct StmtVisitor {
		~StmtVisitor() = default;

		virtual R visitPrintStmt(const PrintStmt& stmt) = 0;
		virtual R visitExpressionStmt(const ExpressionStmt& stmt) = 0;
		virtual R visitVarStmt(const VarStmt& stmt) = 0;
		virtual R visitBlockStmt(const BlockStmt& stmt) = 0;
		virtual R visitIfStmt(const IfStmt& stmt) = 0;
		virtual R visitWhileStmt(const WhileStmt& stmt) = 0;
		virtual R visitFunctionStmt(const FunctionStmt& stmt) = 0;
		virtual R visitReturnStmt(const ReturnStmt& stmt) = 0;
		virtual R visitClassStmt(const ClassStmt& stmt) = 0;
	};

	virtual ~Stmt() = default;
	virtual void accept(StmtVisitor<void>& visitor) const = 0;
};

struct PrintStmt : public Stmt {
	PrintStmt(std::unique_ptr<Expr> expr)
	: expr(std::move(expr))
	{}

	void accept(StmtVisitor<void>& visitor) const {
		visitor.visitPrintStmt(*this);
	}

	std::unique_ptr<Expr> expr;
};

struct ExpressionStmt : public Stmt {
	ExpressionStmt(std::unique_ptr<Expr> expr)
	: expr(std::move(expr))
	{}

	void accept(StmtVisitor<void>& visitor) const {
		visitor.visitExpressionStmt(*this);
	}

	std::unique_ptr<Expr> expr;
};

struct VarStmt : public Stmt {
	VarStmt(Token name, std::unique_ptr<Expr> initialiser)
	: name(name), initialiser(std::move(initialiser))
	{}

	void accept(StmtVisitor<void>& visitor) const {
		visitor.visitVarStmt(*this);
	}

	Token name;
	std::unique_ptr<Expr> initialiser;
};

struct BlockStmt : public Stmt {
	BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
	: statements(std::move(statements))
	{}

	void accept(StmtVisitor<void>& visitor) const {
		visitor.visitBlockStmt(*this);
	}

	std::vector<std::unique_ptr<Stmt>> statements;
};

struct IfStmt : public Stmt {
	IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
	: condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch))
	{}

	void accept(StmtVisitor<void>& visitor) const {
		visitor.visitIfStmt(*this);
	}

	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> thenBranch;
	std::unique_ptr<Stmt> elseBranch;
};

struct WhileStmt : public Stmt {
	WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
	: condition(std::move(condition)), body(std::move(body))
	{}

	void accept(StmtVisitor<void>& visitor) const {
		visitor.visitWhileStmt(*this);
	}

	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> body;
};

struct FunctionStmt : public Stmt {
	FunctionStmt(Token name, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body)
	: name(name), params(std::move(params)), body(std::move(body))
	{}

	void accept(StmtVisitor<void>& visitor) const {
		visitor.visitFunctionStmt(*this);
	}

	Token name;
	std::vector<Token> params;
	std::vector<std::unique_ptr<Stmt>> body;
};

struct ReturnStmt : public Stmt {
	ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
	: keyword(keyword), value(std::move(value))
	{}

	void accept(StmtVisitor<void>& visitor) const {
		visitor.visitReturnStmt(*this);
	}

	Token keyword;
	std::unique_ptr<Expr> value;
};

struct ClassStmt : public Stmt {
	ClassStmt(Token name, std::unique_ptr<Variable> superclass, std::vector<std::unique_ptr<FunctionStmt>> methods)
	: name(name), superclass(std::move(superclass)), methods(std::move(methods))
	{}

	void accept(StmtVisitor<void>& visitor) const {
		visitor.visitClassStmt(*this);
	}

	Token name;
	std::unique_ptr<Variable> superclass;
	std::vector<std::unique_ptr<FunctionStmt>> methods;
};
