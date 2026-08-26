#include "AstPrinter.hpp"


std::string AstPrinter::print(const Expr& expr)
{
    return std::any_cast<std::string>(expr.accept(*this));
}

std::any AstPrinter::visitBinary(const Binary& expr)
{
    return parenthesize(expr.op.lexeme, *expr.left, *expr.right);
}

std::any AstPrinter::visitGrouping(const Grouping& expr)
{
    return parenthesize("group", *expr.expression);
}

std::any AstPrinter::visitLiteral(const Literal& expr)
{
    if (!expr.value.has_value())
        return std::string("nil");

    if (expr.value.type() == typeid(std::string))
        return std::any_cast<std::string>(expr.value);

    if (expr.value.type() == typeid(long))
        return std::to_string(std::any_cast<long>(expr.value));

    if (expr.value.type() == typeid(bool))
        return std::any_cast<bool>(expr.value) ? "true" : "false";

    return std::string("unknown");
}

std::any AstPrinter::visitUnary(const Unary& expr)
{
    return parenthesize(expr.op.lexeme, *expr.right);
}

std::string AstPrinter::parenthesize(
    const std::string& name,
    const Expr& expr)
{
    return "(" + name + " " + print(expr) + ")";
}

std::string AstPrinter::parenthesize(
    const std::string& name,
    const Expr& left,
    const Expr& right)
{
    return "(" + name + " " + print(left) + " " + print(right) + ")";
}
