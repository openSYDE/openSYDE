#pragma once
#include <concepts/ddd.hpp>

#include <data/monitor/event.hpp>
#include "operand.hpp"

#include <memory>

namespace data::monitor
{
    // ========================================================================== //
    // OperandExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    class OperandExpression : public BooleanExpression
    {
    public:
        OperandExpression(OperandOwner<T> left, OperandOwner<T> right);

    protected:
        OperandOwner<T> lOperand;
        OperandOwner<T> rOperand;
    };

    // ========================================================================== //
    // EqualExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    class EqualExpression : public OperandExpression<T>
    {
    public:
        using OperandExpression<T>::OperandExpression;
        bool evaluate(const ChannelDataContainer &ctx) override;
    };

    // ========================================================================== //
    // GreaterExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    class GreaterExpression : public OperandExpression<T>
    {
    public:
        using OperandExpression<T>::OperandExpression;
        bool evaluate(const ChannelDataContainer &ctx) override;
    };

    // ========================================================================== //
    // GreaterEqualExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    class GreaterEqualExpression : public OperandExpression<T>
    {
    public:
        using OperandExpression<T>::OperandExpression;
        bool evaluate(const ChannelDataContainer &ctx) override;
    };

    // ========================================================================== //
    // LessExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    class LessExpression : public OperandExpression<T>
    {
    public:
        using OperandExpression<T>::OperandExpression;
        bool evaluate(const ChannelDataContainer &ctx) override;
    };

    // ========================================================================== //
    // LessEqualExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    class LessEqualExpression : public OperandExpression<T>
    {
    public:
        using OperandExpression<T>::OperandExpression;
        bool evaluate(const ChannelDataContainer &ctx) override;
    };

    // ========================================================================== //
    // ConcenateExpression
    // -------------------------------------------------------------------------- //
    class ConcenateExpression : public BooleanExpression
    {
    public:
        ConcenateExpression(BooleanExpressionOwner left, BooleanExpressionOwner right);

    protected:
        BooleanExpressionOwner lExpr;
        BooleanExpressionOwner rExpr;
    };

    // ========================================================================== //
    // AndExpression
    // -------------------------------------------------------------------------- //
    class AndExpression : public ConcenateExpression
    {
    public:
        using ConcenateExpression::ConcenateExpression;
        bool evaluate(const ChannelDataContainer &ctx) override;
    };

    // ========================================================================== //
    // OrExpression
    // -------------------------------------------------------------------------- //
    class OrExpression : public ConcenateExpression
    {
    public:
        using ConcenateExpression::ConcenateExpression;
        bool evaluate(const ChannelDataContainer &ctx) override;
    };

    // ========================================================================== //
    // NotExpression
    // -------------------------------------------------------------------------- //
    class NotExpression : public BooleanExpression
    {
    public:
        NotExpression(BooleanExpressionOwner expr);
        bool evaluate(const ChannelDataContainer &ctx) override;

    private:
        BooleanExpressionOwner expr;
    };

    // ========================================================================== //
    // ExpressionBuilder
    // -------------------------------------------------------------------------- //
    class ExpressionBuilder
    {
    public:
        ExpressionBuilder(const std::vector<Token> &token, const ChannelDataContainer &ctx);
        BooleanExpressionOwner create();

    private:
        BooleanExpressionOwner build(uint32_t &idx, const bool onlyOneExpression = false, const bool expectClosingParenthesis = false) const;
        BooleanExpressionOwner combine_expression_stack(std::vector<BooleanExpressionOwner> &exprStack, std::vector<Token::Type> &opStack) const;
        template <template <typename> class E>
        BooleanExpressionOwner create_operand_expression(const uint32_t idx) const;

        template <typename T>
        OperandOwner<T> create_operand(const uint32_t posOperand) const;

        template <typename T>
        bool is_operand_of_type(const uint32_t posOperand) const;

        const ChannelDataContainer &ctx;
        const std::vector<Token> &token;
    };

} // namespace data::monitor
#include "expression.tpp"