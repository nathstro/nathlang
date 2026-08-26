#pragma once
#include "Interpreter.hpp"
#include "Stmt.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Resolver : public Expr::ExprVisitor<std::any>, public Stmt::StmtVisitor<void>
{
	public:
		Resolver(Interpreter& interpreter)
		: interpreter(interpreter), currentFunction(FunctionType::NONE), currentClass(ClassType::NONE) {}
		void resolve(const std::vector<std::unique_ptr<Stmt>>& stmts);

		std::any visitLiteral(const Literal& expr) override;
		std::any visitGrouping(const Grouping& expr) override;
		std::any visitUnary(const Unary& expr) override;
		std::any visitBinary(const Binary& expr) override;
		std::any visitVariable(const Variable& expr) override;
		std::any visitAssign(const Assign& expr) override;
		std::any visitLogical(const Logical& expr) override;
		std::any visitCall(const Call& expr) override;
		std::any visitGet(const Get& expr) override;
		std::any visitSet(const Set& expr) override;
		std::any visitThis(const This& expr) override;
		std::any visitSuper(const Super& expr) override;

		void visitIfStmt(const IfStmt& stmt) override;
		void visitPrintStmt(const PrintStmt& stmt) override;
		void visitExpressionStmt(const ExpressionStmt& stmt) override;
		void visitVarStmt(const VarStmt& stmt) override;
		void visitBlockStmt(const BlockStmt& stmt) override;
		void visitWhileStmt(const WhileStmt& stmt) override;
		void visitFunctionStmt(const FunctionStmt& stmt) override;
		void visitReturnStmt(const ReturnStmt& stmt) override;
		void visitClassStmt(const ClassStmt& stmt) override;

	private:
		enum class FunctionType
		{
			NONE, FUNCTION, METHOD, INITIALISER
		};
		enum class ClassType
		{
			NONE, CLASS, SUBCLASS
		};

		void resolve(const Stmt& stmt);
		void resolve(const Expr& expr);
		void resolveLocal(const Expr& expr, const Token& name);
		void resolveFunction(const FunctionStmt& function, FunctionType type);
		void beginScope();
		void endScope();
		void declare(const Token& name);
		void define(const Token& name);

		Interpreter& interpreter;
		std::vector<std::unordered_map<std::string, bool>> scopes;
		FunctionType currentFunction;
		ClassType currentClass;
};