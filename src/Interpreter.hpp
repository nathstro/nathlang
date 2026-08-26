#pragma once
#include "Environment.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <memory>
#include <unordered_map>

class Interpreter : public Expr::ExprVisitor<std::any>, public Stmt::StmtVisitor<void>
{
	public:
		Interpreter();
		std::shared_ptr<Environment> globals() {return globalEnvironment;}
		void interpret(std::vector<std::unique_ptr<Stmt>>& stmts);

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

		std::any evaluate(const Expr& expr);
		void execute(const Stmt& stmt);
		void executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);
		void resolve(const Expr& expr, int depth);

	private:
		std::shared_ptr<Environment> globalEnvironment, environment;
		std::unordered_map<const Expr*, int> locals;
		std::any lookUpVariable(const Token& name, const Expr& expr);

		bool isTruthy(const std::any& object);
		bool isEqual(const std::any& objectA, const std::any& objectB);
		
		std::string stringify(const std::any& object);

		void checkNumberOperand(const Token& op, const std::any& operand);
		void checkNumberOperands(const Token& op, const std::any& left, const std::any& right);
};
