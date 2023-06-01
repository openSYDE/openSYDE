%{
#include <Vector/DBC/Network.h>
#include <Vector/DBC/Scanner.h>

/* match end of file */
// #define yyterminate() return Vector::DBC::Parser::make_END
%}

    // debug options
    // %option backup
%option debug
%option perf-report
    // %option trace
    // %option verbose

    // standard options
%option nodefault
%option nounput
%option noinput
%option batch
%option yyclass="Vector::DBC::Scanner"
%option noyywrap
%option c++

DIGIT                   [0-9]
NONDIGIT                [_a-zA-Z]
HEXADECIMAL_PREFIX      (0x|0X)
NONZERO_DIGIT           [1-9]
HEXADECIMAL_DIGIT       [0-9a-fA-F]
EXPONENT_PART           ([Ee][+-]?{DIGIT}+)

%x NS

%%
    /* 3 Structure of the DBC File */

    /* 4 Version and New Symbol Specification */
"VERSION" {
    return Vector::DBC::Parser::make_VERSION(loc); }
"NS_" {
    BEGIN(NS);
    return Vector::DBC::Parser::make_NS(loc); }
<NS>"BS_" {
    BEGIN(INITIAL);
    return Vector::DBC::Parser::make_BS(loc); }
<NS>{NONDIGIT}+ {
    return Vector::DBC::Parser::make_NS_VALUE(yytext, loc); }
<NS>":" {
    return Vector::DBC::Parser::make_COLON(loc); }
<NS>([ ]*[\r\n]+)+ {
    return Vector::DBC::Parser::make_EOL(loc); }
<NS>[ \t] {
    }

    /* 5 Bit Timing Definition */
"BS_" {
    return Vector::DBC::Parser::make_BS(loc); }

    /* 6 Node Definitions */
"BU_" {
    return Vector::DBC::Parser::make_BU(loc); }

    /* 7 Value Table Definitions */
"VAL_TABLE_" {
    return Vector::DBC::Parser::make_VAL_TABLE(loc); }

    /* 7.1 Value Descriptions (Value Encodings) */

    /* 8 Message Definitions */
"BO_" {
    return Vector::DBC::Parser::make_BO(loc); }
"Vector__XXX" {
    return Vector::DBC::Parser::make_VECTOR_XXX(loc); }

    /* 8.1 Pseudo-message */

    /* 8.2 Signal Definitions */
"SG_" {
    return Vector::DBC::Parser::make_SG(loc); }
"m" {
    return Vector::DBC::Parser::make_LOWER_M(loc); }
"M" {
    return Vector::DBC::Parser::make_UPPER_M(loc); }
"SIG_VALTYPE_" {
    return Vector::DBC::Parser::make_SIG_VALTYPE(loc); }

    /* 8.3 Definition of Message Transmitters */
"BO_TX_BU_" {
    return Vector::DBC::Parser::make_BO_TX_BU(loc); }

    /* 8.4 Signal Value Descriptions (Value Encodings) */
"VAL_" {
    return Vector::DBC::Parser::make_VAL(loc); }

    /* 9 Environment Variable Definitions */
"EV_" {
    return Vector::DBC::Parser::make_EV(loc); }
"DUMMY_NODE_VECTOR0" {
    return Vector::DBC::Parser::make_DUMMY_NODE_VECTOR0(loc); }
"DUMMY_NODE_VECTOR1" {
    return Vector::DBC::Parser::make_DUMMY_NODE_VECTOR1(loc); }
"DUMMY_NODE_VECTOR2" {
    return Vector::DBC::Parser::make_DUMMY_NODE_VECTOR2(loc); }
"DUMMY_NODE_VECTOR3" {
    return Vector::DBC::Parser::make_DUMMY_NODE_VECTOR3(loc); }
"DUMMY_NODE_VECTOR8000" {
    return Vector::DBC::Parser::make_DUMMY_NODE_VECTOR8000(loc); }
"DUMMY_NODE_VECTOR8001" {
    return Vector::DBC::Parser::make_DUMMY_NODE_VECTOR8001(loc); }
"DUMMY_NODE_VECTOR8002" {
    return Vector::DBC::Parser::make_DUMMY_NODE_VECTOR8002(loc); }
"DUMMY_NODE_VECTOR8003" {
    return Vector::DBC::Parser::make_DUMMY_NODE_VECTOR8003(loc); }
"ENVVAR_DATA_" {
    return Vector::DBC::Parser::make_ENVVAR_DATA(loc); }

    /* 9.1 Environment Variable Value Descriptions */

    /* 10 Signal Type and Signal Group Definitions */
"SGTYPE_" {
    return Vector::DBC::Parser::make_SGTYPE(loc); }
"SIG_GROUP_" {
    return Vector::DBC::Parser::make_SIG_GROUP(loc); }

    /* 11 Comment Definitions */
"CM_" {
    return Vector::DBC::Parser::make_CM(loc); }

    /* 12 User Defined Attribute Definitions */

    /* 12.1 Attribute Definitions */
"BA_DEF_" {
    return Vector::DBC::Parser::make_BA_DEF(loc); }
"INT" {
    return Vector::DBC::Parser::make_INT(loc); }
"HEX" {
    return Vector::DBC::Parser::make_HEX(loc); }
"FLOAT" {
    return Vector::DBC::Parser::make_FLOAT(loc); }
"STRING" {
    return Vector::DBC::Parser::make_STRING(loc); }
"ENUM" {
    return Vector::DBC::Parser::make_ENUM(loc); }
"BA_DEF_REL_" {
    return Vector::DBC::Parser::make_BA_DEF_REL(loc); }
"BU_EV_REL_" {
    return Vector::DBC::Parser::make_BU_EV_REL(loc); }
"BU_BO_REL_" {
    return Vector::DBC::Parser::make_BU_BO_REL(loc); }
"BU_SG_REL_" {
    return Vector::DBC::Parser::make_BU_SG_REL(loc); }

    /* Attribute Defaults */
"BA_DEF_DEF_" {
    return Vector::DBC::Parser::make_BA_DEF_DEF(loc); }
"BA_DEF_DEF_REL_" {
    return Vector::DBC::Parser::make_BA_DEF_DEF_REL(loc); }

    /* 12.2 Attribute Values */
"BA_" {
    return Vector::DBC::Parser::make_BA(loc); }
"BA_REL_" {
    return Vector::DBC::Parser::make_BA_REL(loc); }

    /* 13 Extended Multiplexing */
"SG_MUL_VAL_" {
    return Vector::DBC::Parser::make_SG_MUL_VAL(loc); }

    /* Punctuators */
"[" { // left square bracket
    return Vector::DBC::Parser::make_OPEN_BRACKET(loc); }
"]" { // right square bracket
    return Vector::DBC::Parser::make_CLOSE_BRACKET(loc); }
"(" { // left parenthesis
    return Vector::DBC::Parser::make_OPEN_PARENTHESIS(loc); }
")" { // right parenthesis
    return Vector::DBC::Parser::make_CLOSE_PARENTHESIS(loc); }
"+" { // plus sign
    return Vector::DBC::Parser::make_PLUS(loc); }
"-" { // hyphen-minus
    return Vector::DBC::Parser::make_MINUS(loc); }
"|" { // vertical bar
    return Vector::DBC::Parser::make_VERTICAL_BAR(loc); }
":" { // colon
    return Vector::DBC::Parser::make_COLON(loc); }
";" { // semicolon
    return Vector::DBC::Parser::make_SEMICOLON(loc); }
"=" { // equal sign
    return Vector::DBC::Parser::make_ASSIGN(loc); }
"," { // comma
    return Vector::DBC::Parser::make_COMMA(loc); }
"@" { // at sign
    return Vector::DBC::Parser::make_AT(loc); }

    /* 2 General Definitions */
{DIGIT}+ {
    return Vector::DBC::Parser::make_UNSIGNED_INTEGER(yytext, loc); }
    /* 2023-03-15 STW: Add new seperate type for value ranges to avoid mismatch with signed type */
{DIGIT}+[-]{DIGIT}+ {
    return Vector::DBC::Parser::make_MULTIPLEXOR_VALUE_RANGE(yytext, loc); }
[-+]?{DIGIT}+ {
    return Vector::DBC::Parser::make_SIGNED_INTEGER(yytext, loc); }
[-+]?{DIGIT}*"."?{DIGIT}+{EXPONENT_PART}? {
    return Vector::DBC::Parser::make_DOUBLE(yytext, loc); }
\"(\\.|[^\\"])*\" {
    std::string str(yytext);
    str.erase(0, 1);
    str.erase(str.size() - 1);
    return Vector::DBC::Parser::make_CHAR_STRING(str, loc); }
    /* 2023-03-23 STW: Alow digit start to properly handle variable names starting with a digit */
({NONDIGIT}|{DIGIT})+ {
    return Vector::DBC::Parser::make_DBC_IDENTIFIER(yytext, loc); }

    /* comments */
"//".*[\r\n]+ {
    }

    /* end of line */
([ ]*[\r\n]+)+ {
    return Vector::DBC::Parser::make_EOL(loc); }

    /* whitespace */
[ \t] {
    }

    /* yet unmatched characters */
. {
    std::cerr << "unmatched character: " << *yytext; }

    /* match end of file */
<<EOF>> {
    return Vector::DBC::Parser::make_END(loc); }

%%

