#pragma once

#include "data/monitor/event.hpp"
#include <data/monitor/event/token.hpp>

#include <vector>
#include <string>
#include <regex>

namespace data::monitor
{
    namespace dc = data::container;

    using Value = std::variant<
        uint8_t, int8_t,
        uint16_t, int16_t,
        uint32_t, int32_t,
        float>;

    enum TokenType
    {
        integer,
        greater,
        greater_equal,
        lesser,
        lesser_equal,
        equal,
        not_equal,
        logicalAnd,
        logicalOr,
        logicalNot,
        topic,
        channel,
        openParenthesis,
        closedParenthesis,
        castType
    };

    class Lexer
    {
    public:
        Lexer();
        std::vector<Token> lex(const std::string &expr) const;

    private:
        void create_and_add_token(const std::string_view &strToken, std::vector<Token> &token) const;
        std::regex pattern;
    };
};