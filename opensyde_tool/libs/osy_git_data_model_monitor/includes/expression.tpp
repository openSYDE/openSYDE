#include "expression.hpp"
namespace data::monitor
{
    // ========================================================================== //
    // OperandExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    OperandExpression<T>::OperandExpression(OperandOwner<T> lo, OperandOwner<T> ro) : lOperand(std::move(lo)), rOperand(std::move(ro)) {}

    // ========================================================================== //
    // EqualExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    bool EqualExpression<T>::evaluate(const ChannelDataContainer &ctx)
    {
        return this->lOperand->get(ctx) == this->rOperand->get(ctx);
    }

    // ========================================================================== //
    // GreaterExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    bool GreaterExpression<T>::evaluate(const ChannelDataContainer &ctx)
    {
        return this->lOperand->get(ctx) > this->rOperand->get(ctx);
    }

    // ========================================================================== //
    // GreaterEqualExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    bool GreaterEqualExpression<T>::evaluate(const ChannelDataContainer &ctx)
    {
        return this->lOperand->get(ctx) >= this->rOperand->get(ctx);
    }

    // ========================================================================== //
    // LessExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    bool LessExpression<T>::evaluate(const ChannelDataContainer &ctx)
    {
        return this->lOperand->get(ctx) < this->rOperand->get(ctx);
    }

    // ========================================================================== //
    // LessEqualExpression
    // -------------------------------------------------------------------------- //
    template <typename T>
    bool LessEqualExpression<T>::evaluate(const ChannelDataContainer &ctx)
    {
        return this->lOperand->get(ctx) <= this->rOperand->get(ctx);
    }

    // ========================================================================== //
    // ExpressionBuilder
    // -------------------------------------------------------------------------- //
    template <typename T>
    OperandOwner<T> ExpressionBuilder::create_operand(const uint32_t posOperand) const
    {
        if (token[posOperand].type == Token::Type::ConstantValue)
        {
            if (this->is_operand_of_type<uint32_t>(posOperand))
            {
                return std::unique_ptr<Operand<T>>(new Constant<T>(std::get<uint32_t>(this->token[posOperand].value.value())));
            }
            if (this->is_operand_of_type<int32_t>(posOperand))
            {
                return std::unique_ptr<Operand<T>>(new Constant<T>(std::get<int32_t>(this->token[posOperand].value.value())));
            }
            return std::unique_ptr<Operand<T>>(new Constant<T>(std::get<float>(this->token[posOperand].value.value())));
        }
        if (token[posOperand].type == Token::Type::Channel)
        {
            return std::unique_ptr<Operand<T>>(new Channel<T>(std::get<uint32_t>(this->token[posOperand].value.value())));
        }
        throw std::runtime_error("Expression Parsing Error, expect a Constant or Channel Type, but got something else");
    }

    template <template <typename> class E>
    BooleanExpressionOwner ExpressionBuilder::create_operand_expression(const uint32_t idx) const
    {
        const uint32_t posOpLeft = idx - 1;
        const uint32_t posOpRight = idx + 1;
        if (this->is_operand_of_type<uint32_t>(posOpLeft) and this->is_operand_of_type<uint32_t>(posOpRight))
        {
            return std::unique_ptr<BooleanExpression>(new E<uint32_t>(
                this->create_operand<uint32_t>(posOpLeft),
                this->create_operand<uint32_t>(posOpRight)));
        }

        if (this->is_operand_of_type<int32_t>(posOpLeft) and this->is_operand_of_type<int32_t>(posOpRight))
        {
            return std::unique_ptr<BooleanExpression>(new E<int32_t>(
                this->create_operand<int32_t>(posOpLeft),
                this->create_operand<int32_t>(posOpRight)));
        }
        return std::unique_ptr<BooleanExpression>(new E<float>(
            this->create_operand<float>(posOpLeft),
            this->create_operand<float>(posOpRight)));
    }

    template <typename T>
    bool ExpressionBuilder::is_operand_of_type(const uint32_t pos) const
    {
        if (this->token[pos].type == Token::Type::ConstantValue)
        {
            if (this->token[pos].value.has_value())
            {
                return std::holds_alternative<T>(*this->token[pos].value);
            }
            return false;
        }
        if (this->token[pos].type == Token::Type::Channel)
        {
            const data::container::StdDataVariant &v = this->ctx.get_uninitialized(std::get<uint32_t>(this->token[pos].value.value()));
            return std::holds_alternative<data::container::Scalar<T>>(v);
        }
        throw std::runtime_error("Expect ether a Channel or a ConstantValue, but got something different!");
    }
}
