#pragma once

#include "Expr.hpp"
#include <string>

class AstPrinter : public Expr::ExprVisitor<std::any> {
public:
    std::string print(const Expr& expr);

    std::any visitBinary(const Binary& expr) override;
    std::any visitGrouping(const Grouping& expr) override;
    std::any visitLiteral(const Literal& expr) override;
    std::any visitUnary(const Unary& expr) override;

private:
    std::string parenthesize(const std::string& name, const Expr& expr);

    std::string parenthesize(const std::string& name, const Expr& left, const Expr& right);
};