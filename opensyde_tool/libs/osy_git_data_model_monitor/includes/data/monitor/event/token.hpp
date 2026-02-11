#pragma once

#include <variant>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace data::monitor
{
    struct Token
    {
        using Value = std::variant<uint32_t, int32_t, float, std::string, std::vector<std::string>>;
        enum class Type
        {
            ConstantValue,
            Channel,
            GreaterThan,
            GreaterEqual,
            LessThan,
            LessEqual,
            Equal,
            NotEqual,
            LogicalAnd,
            LogicalOr,
            LogicalNot,
            OpenParenthesis,
            ClosedParenthesis,
            CastType,
            Unset
        };
        Type type = Type::Unset;
        std::optional<Value> value;
        Token(const Type type) : type{type}, value{std::nullopt} {}
        Token(const Type type, const Value &value) : type{type}, value{value} {}
    };
}