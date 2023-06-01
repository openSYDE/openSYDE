#pragma once

#include <Vector/DBC/platform.h>

#include <iostream>

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#define YY_DECL Vector::DBC::Parser::symbol_type Vector::DBC::Scanner::yylex(const Vector::DBC::Parser::location_type & loc)

#define YY_USER_ACTION \
    location.begin.line = location.end.line; \
    location.begin.column = location.end.column; \
    for (int i = 0; yytext[i] != '\0'; i++) { \
        if(yytext[i] == '\n') { \
            location.end.line++; \
            location.end.column = 0; \
        } \
        else { \
            location.end.column++; \
        } \
    }

#include <Vector/DBC/Parser.hpp>

namespace Vector {
namespace DBC {

/**
 * @brief Lex scanner class
 *
 * This contains the converter-specific Flex Lexer class.
 */
class Scanner : public yyFlexLexer {
  public:
    Scanner(std::istream & istream) :
        yyFlexLexer(&istream),
        location()
    { }

    /**
     * lexer function
     *
     * @param loc location
     * @return symbol type
     */
    Parser::symbol_type yylex(const Parser::location_type & loc);

    /** location */
    Parser::location_type location;
};

}
}
