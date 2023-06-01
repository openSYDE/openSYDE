// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "Parser.hpp"


// Unqualified %code blocks.
#line 31 "Parser.yy"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
// 2023-03-14 STW: Add interface to allow log entries
#include <sstream>

#include <Vector/DBC/Network.h>
#include <Vector/DBC/Scanner.h>

// 2023-03-14 STW: Add interface to allow log entries
#include "C_OscLoggingHandler.hpp"

#undef yylex
#define yylex scanner->yylex

#define loc scanner->location

#if 0
/* stod without C locale */
static double stod(const std::string & str)
{
    std::istringstream iss(str);

    /* use english decimal points for floating numbers */
    iss.imbue(std::locale("C"));

    double d;
    iss >> d;

    return d;
}
#endif

#line 82 "Parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 4 "Parser.yy"
namespace Vector { namespace DBC {
#line 175 "Parser.cpp"

  /// Build a parser object.
  Parser::Parser (class Scanner * scanner_yyarg, class Network * network_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      network (network_yyarg)
  {}

  Parser::~Parser ()
  {}

  Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  Parser::symbol_kind_type
  Parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  Parser::stack_symbol_type::stack_symbol_type ()
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_object_type: // object_type
        value.YY_MOVE_OR_COPY< AttributeObjectType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_attribute_value_type: // attribute_value_type
        value.YY_MOVE_OR_COPY< AttributeValueType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_byte_order: // byte_order
        value.YY_MOVE_OR_COPY< ByteOrder > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_env_var_type: // env_var_type
        value.YY_MOVE_OR_COPY< EnvironmentVariable::Type > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_multiplexor_value_range_comma: // multiplexor_value_range_comma
      case symbol_kind::S_multiplexor_value_range: // multiplexor_value_range
        value.YY_MOVE_OR_COPY< ExtendedMultiplexor::ValueRange > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_message: // message
        value.YY_MOVE_OR_COPY< Message > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signal: // signal
        value.YY_MOVE_OR_COPY< Signal > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signal_extended_value_type_type: // signal_extended_value_type_type
        value.YY_MOVE_OR_COPY< Signal::ExtendedValueType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_table: // value_table
        value.YY_MOVE_OR_COPY< ValueTable > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_type: // value_type
        value.YY_MOVE_OR_COPY< ValueType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_double: // double
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_offset: // offset
      case symbol_kind::S_minimum: // minimum
      case symbol_kind::S_maximum: // maximum
      case symbol_kind::S_initial_value: // initial_value
      case symbol_kind::S_default_value: // default_value
        value.YY_MOVE_OR_COPY< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signed_64_integer: // signed_64_integer
        value.YY_MOVE_OR_COPY< int64_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_encoding_descriptions: // value_encoding_descriptions
        value.YY_MOVE_OR_COPY< std::map<int64_t, std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signals: // signals
        value.YY_MOVE_OR_COPY< std::map<std::string, Signal> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_encoding_description: // value_encoding_description
        value.YY_MOVE_OR_COPY< std::pair<int64_t, std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_multiplexor_value_ranges: // multiplexor_value_ranges
        value.YY_MOVE_OR_COPY< std::set<ExtendedMultiplexor::ValueRange> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signal_names: // signal_names
      case symbol_kind::S_receivers: // receivers
      case symbol_kind::S_transmitters: // transmitters
      case symbol_kind::S_access_nodes: // access_nodes
        value.YY_MOVE_OR_COPY< std::set<std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_UNSIGNED_INTEGER: // UNSIGNED_INTEGER
      case symbol_kind::S_MULTIPLEXOR_VALUE_RANGE: // MULTIPLEXOR_VALUE_RANGE
      case symbol_kind::S_SIGNED_INTEGER: // SIGNED_INTEGER
      case symbol_kind::S_DOUBLE: // DOUBLE
      case symbol_kind::S_CHAR_STRING: // CHAR_STRING
      case symbol_kind::S_DBC_IDENTIFIER: // DBC_IDENTIFIER
      case symbol_kind::S_NS_VALUE: // NS_VALUE
      case symbol_kind::S_semicolon_with_optional_eol: // semicolon_with_optional_eol
      case symbol_kind::S_char_string: // char_string
      case symbol_kind::S_char_string_eol: // char_string_eol
      case symbol_kind::S_dbc_identifier: // dbc_identifier
      case symbol_kind::S_candb_version_string: // candb_version_string
      case symbol_kind::S_node_name: // node_name
      case symbol_kind::S_value_table_name: // value_table_name
      case symbol_kind::S_message_name: // message_name
      case symbol_kind::S_transmitter: // transmitter
      case symbol_kind::S_signal_name: // signal_name
      case symbol_kind::S_multiplexer_indicator: // multiplexer_indicator
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_receiver: // receiver
      case symbol_kind::S_env_var_name: // env_var_name
      case symbol_kind::S_access_node: // access_node
      case symbol_kind::S_signal_type_name: // signal_type_name
      case symbol_kind::S_signal_group_name: // signal_group_name
      case symbol_kind::S_attribute_name: // attribute_name
      case symbol_kind::S_attribute_value: // attribute_value
      case symbol_kind::S_multiplexed_signal_name: // multiplexed_signal_name
      case symbol_kind::S_multiplexor_switch_name: // multiplexor_switch_name
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_char_strings: // char_strings
      case symbol_kind::S_new_symbol_values: // new_symbol_values
        value.YY_MOVE_OR_COPY< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_access_type: // access_type
        value.YY_MOVE_OR_COPY< uint16_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_unsigned_integer: // unsigned_integer
      case symbol_kind::S_baudrate: // baudrate
      case symbol_kind::S_btr1: // btr1
      case symbol_kind::S_btr2: // btr2
      case symbol_kind::S_message_id: // message_id
      case symbol_kind::S_message_size: // message_size
      case symbol_kind::S_start_bit: // start_bit
      case symbol_kind::S_signal_size: // signal_size
      case symbol_kind::S_ev_id: // ev_id
      case symbol_kind::S_data_size: // data_size
      case symbol_kind::S_repetitions: // repetitions
        value.YY_MOVE_OR_COPY< uint32_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_object_type: // object_type
        value.move< AttributeObjectType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_attribute_value_type: // attribute_value_type
        value.move< AttributeValueType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_byte_order: // byte_order
        value.move< ByteOrder > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_env_var_type: // env_var_type
        value.move< EnvironmentVariable::Type > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_multiplexor_value_range_comma: // multiplexor_value_range_comma
      case symbol_kind::S_multiplexor_value_range: // multiplexor_value_range
        value.move< ExtendedMultiplexor::ValueRange > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_message: // message
        value.move< Message > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signal: // signal
        value.move< Signal > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signal_extended_value_type_type: // signal_extended_value_type_type
        value.move< Signal::ExtendedValueType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_table: // value_table
        value.move< ValueTable > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_type: // value_type
        value.move< ValueType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_double: // double
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_offset: // offset
      case symbol_kind::S_minimum: // minimum
      case symbol_kind::S_maximum: // maximum
      case symbol_kind::S_initial_value: // initial_value
      case symbol_kind::S_default_value: // default_value
        value.move< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signed_64_integer: // signed_64_integer
        value.move< int64_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_encoding_descriptions: // value_encoding_descriptions
        value.move< std::map<int64_t, std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signals: // signals
        value.move< std::map<std::string, Signal> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_encoding_description: // value_encoding_description
        value.move< std::pair<int64_t, std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_multiplexor_value_ranges: // multiplexor_value_ranges
        value.move< std::set<ExtendedMultiplexor::ValueRange> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signal_names: // signal_names
      case symbol_kind::S_receivers: // receivers
      case symbol_kind::S_transmitters: // transmitters
      case symbol_kind::S_access_nodes: // access_nodes
        value.move< std::set<std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_UNSIGNED_INTEGER: // UNSIGNED_INTEGER
      case symbol_kind::S_MULTIPLEXOR_VALUE_RANGE: // MULTIPLEXOR_VALUE_RANGE
      case symbol_kind::S_SIGNED_INTEGER: // SIGNED_INTEGER
      case symbol_kind::S_DOUBLE: // DOUBLE
      case symbol_kind::S_CHAR_STRING: // CHAR_STRING
      case symbol_kind::S_DBC_IDENTIFIER: // DBC_IDENTIFIER
      case symbol_kind::S_NS_VALUE: // NS_VALUE
      case symbol_kind::S_semicolon_with_optional_eol: // semicolon_with_optional_eol
      case symbol_kind::S_char_string: // char_string
      case symbol_kind::S_char_string_eol: // char_string_eol
      case symbol_kind::S_dbc_identifier: // dbc_identifier
      case symbol_kind::S_candb_version_string: // candb_version_string
      case symbol_kind::S_node_name: // node_name
      case symbol_kind::S_value_table_name: // value_table_name
      case symbol_kind::S_message_name: // message_name
      case symbol_kind::S_transmitter: // transmitter
      case symbol_kind::S_signal_name: // signal_name
      case symbol_kind::S_multiplexer_indicator: // multiplexer_indicator
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_receiver: // receiver
      case symbol_kind::S_env_var_name: // env_var_name
      case symbol_kind::S_access_node: // access_node
      case symbol_kind::S_signal_type_name: // signal_type_name
      case symbol_kind::S_signal_group_name: // signal_group_name
      case symbol_kind::S_attribute_name: // attribute_name
      case symbol_kind::S_attribute_value: // attribute_value
      case symbol_kind::S_multiplexed_signal_name: // multiplexed_signal_name
      case symbol_kind::S_multiplexor_switch_name: // multiplexor_switch_name
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_char_strings: // char_strings
      case symbol_kind::S_new_symbol_values: // new_symbol_values
        value.move< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_access_type: // access_type
        value.move< uint16_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_unsigned_integer: // unsigned_integer
      case symbol_kind::S_baudrate: // baudrate
      case symbol_kind::S_btr1: // btr1
      case symbol_kind::S_btr2: // btr2
      case symbol_kind::S_message_id: // message_id
      case symbol_kind::S_message_size: // message_size
      case symbol_kind::S_start_bit: // start_bit
      case symbol_kind::S_signal_size: // signal_size
      case symbol_kind::S_ev_id: // ev_id
      case symbol_kind::S_data_size: // data_size
      case symbol_kind::S_repetitions: // repetitions
        value.move< uint32_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_object_type: // object_type
        value.copy< AttributeObjectType > (that.value);
        break;

      case symbol_kind::S_attribute_value_type: // attribute_value_type
        value.copy< AttributeValueType > (that.value);
        break;

      case symbol_kind::S_byte_order: // byte_order
        value.copy< ByteOrder > (that.value);
        break;

      case symbol_kind::S_env_var_type: // env_var_type
        value.copy< EnvironmentVariable::Type > (that.value);
        break;

      case symbol_kind::S_multiplexor_value_range_comma: // multiplexor_value_range_comma
      case symbol_kind::S_multiplexor_value_range: // multiplexor_value_range
        value.copy< ExtendedMultiplexor::ValueRange > (that.value);
        break;

      case symbol_kind::S_message: // message
        value.copy< Message > (that.value);
        break;

      case symbol_kind::S_signal: // signal
        value.copy< Signal > (that.value);
        break;

      case symbol_kind::S_signal_extended_value_type_type: // signal_extended_value_type_type
        value.copy< Signal::ExtendedValueType > (that.value);
        break;

      case symbol_kind::S_value_table: // value_table
        value.copy< ValueTable > (that.value);
        break;

      case symbol_kind::S_value_type: // value_type
        value.copy< ValueType > (that.value);
        break;

      case symbol_kind::S_double: // double
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_offset: // offset
      case symbol_kind::S_minimum: // minimum
      case symbol_kind::S_maximum: // maximum
      case symbol_kind::S_initial_value: // initial_value
      case symbol_kind::S_default_value: // default_value
        value.copy< double > (that.value);
        break;

      case symbol_kind::S_signed_64_integer: // signed_64_integer
        value.copy< int64_t > (that.value);
        break;

      case symbol_kind::S_value_encoding_descriptions: // value_encoding_descriptions
        value.copy< std::map<int64_t, std::string> > (that.value);
        break;

      case symbol_kind::S_signals: // signals
        value.copy< std::map<std::string, Signal> > (that.value);
        break;

      case symbol_kind::S_value_encoding_description: // value_encoding_description
        value.copy< std::pair<int64_t, std::string> > (that.value);
        break;

      case symbol_kind::S_multiplexor_value_ranges: // multiplexor_value_ranges
        value.copy< std::set<ExtendedMultiplexor::ValueRange> > (that.value);
        break;

      case symbol_kind::S_signal_names: // signal_names
      case symbol_kind::S_receivers: // receivers
      case symbol_kind::S_transmitters: // transmitters
      case symbol_kind::S_access_nodes: // access_nodes
        value.copy< std::set<std::string> > (that.value);
        break;

      case symbol_kind::S_UNSIGNED_INTEGER: // UNSIGNED_INTEGER
      case symbol_kind::S_MULTIPLEXOR_VALUE_RANGE: // MULTIPLEXOR_VALUE_RANGE
      case symbol_kind::S_SIGNED_INTEGER: // SIGNED_INTEGER
      case symbol_kind::S_DOUBLE: // DOUBLE
      case symbol_kind::S_CHAR_STRING: // CHAR_STRING
      case symbol_kind::S_DBC_IDENTIFIER: // DBC_IDENTIFIER
      case symbol_kind::S_NS_VALUE: // NS_VALUE
      case symbol_kind::S_semicolon_with_optional_eol: // semicolon_with_optional_eol
      case symbol_kind::S_char_string: // char_string
      case symbol_kind::S_char_string_eol: // char_string_eol
      case symbol_kind::S_dbc_identifier: // dbc_identifier
      case symbol_kind::S_candb_version_string: // candb_version_string
      case symbol_kind::S_node_name: // node_name
      case symbol_kind::S_value_table_name: // value_table_name
      case symbol_kind::S_message_name: // message_name
      case symbol_kind::S_transmitter: // transmitter
      case symbol_kind::S_signal_name: // signal_name
      case symbol_kind::S_multiplexer_indicator: // multiplexer_indicator
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_receiver: // receiver
      case symbol_kind::S_env_var_name: // env_var_name
      case symbol_kind::S_access_node: // access_node
      case symbol_kind::S_signal_type_name: // signal_type_name
      case symbol_kind::S_signal_group_name: // signal_group_name
      case symbol_kind::S_attribute_name: // attribute_name
      case symbol_kind::S_attribute_value: // attribute_value
      case symbol_kind::S_multiplexed_signal_name: // multiplexed_signal_name
      case symbol_kind::S_multiplexor_switch_name: // multiplexor_switch_name
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_char_strings: // char_strings
      case symbol_kind::S_new_symbol_values: // new_symbol_values
        value.copy< std::vector<std::string> > (that.value);
        break;

      case symbol_kind::S_access_type: // access_type
        value.copy< uint16_t > (that.value);
        break;

      case symbol_kind::S_unsigned_integer: // unsigned_integer
      case symbol_kind::S_baudrate: // baudrate
      case symbol_kind::S_btr1: // btr1
      case symbol_kind::S_btr2: // btr2
      case symbol_kind::S_message_id: // message_id
      case symbol_kind::S_message_size: // message_size
      case symbol_kind::S_start_bit: // start_bit
      case symbol_kind::S_signal_size: // signal_size
      case symbol_kind::S_ev_id: // ev_id
      case symbol_kind::S_data_size: // data_size
      case symbol_kind::S_repetitions: // repetitions
        value.copy< uint32_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_object_type: // object_type
        value.move< AttributeObjectType > (that.value);
        break;

      case symbol_kind::S_attribute_value_type: // attribute_value_type
        value.move< AttributeValueType > (that.value);
        break;

      case symbol_kind::S_byte_order: // byte_order
        value.move< ByteOrder > (that.value);
        break;

      case symbol_kind::S_env_var_type: // env_var_type
        value.move< EnvironmentVariable::Type > (that.value);
        break;

      case symbol_kind::S_multiplexor_value_range_comma: // multiplexor_value_range_comma
      case symbol_kind::S_multiplexor_value_range: // multiplexor_value_range
        value.move< ExtendedMultiplexor::ValueRange > (that.value);
        break;

      case symbol_kind::S_message: // message
        value.move< Message > (that.value);
        break;

      case symbol_kind::S_signal: // signal
        value.move< Signal > (that.value);
        break;

      case symbol_kind::S_signal_extended_value_type_type: // signal_extended_value_type_type
        value.move< Signal::ExtendedValueType > (that.value);
        break;

      case symbol_kind::S_value_table: // value_table
        value.move< ValueTable > (that.value);
        break;

      case symbol_kind::S_value_type: // value_type
        value.move< ValueType > (that.value);
        break;

      case symbol_kind::S_double: // double
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_offset: // offset
      case symbol_kind::S_minimum: // minimum
      case symbol_kind::S_maximum: // maximum
      case symbol_kind::S_initial_value: // initial_value
      case symbol_kind::S_default_value: // default_value
        value.move< double > (that.value);
        break;

      case symbol_kind::S_signed_64_integer: // signed_64_integer
        value.move< int64_t > (that.value);
        break;

      case symbol_kind::S_value_encoding_descriptions: // value_encoding_descriptions
        value.move< std::map<int64_t, std::string> > (that.value);
        break;

      case symbol_kind::S_signals: // signals
        value.move< std::map<std::string, Signal> > (that.value);
        break;

      case symbol_kind::S_value_encoding_description: // value_encoding_description
        value.move< std::pair<int64_t, std::string> > (that.value);
        break;

      case symbol_kind::S_multiplexor_value_ranges: // multiplexor_value_ranges
        value.move< std::set<ExtendedMultiplexor::ValueRange> > (that.value);
        break;

      case symbol_kind::S_signal_names: // signal_names
      case symbol_kind::S_receivers: // receivers
      case symbol_kind::S_transmitters: // transmitters
      case symbol_kind::S_access_nodes: // access_nodes
        value.move< std::set<std::string> > (that.value);
        break;

      case symbol_kind::S_UNSIGNED_INTEGER: // UNSIGNED_INTEGER
      case symbol_kind::S_MULTIPLEXOR_VALUE_RANGE: // MULTIPLEXOR_VALUE_RANGE
      case symbol_kind::S_SIGNED_INTEGER: // SIGNED_INTEGER
      case symbol_kind::S_DOUBLE: // DOUBLE
      case symbol_kind::S_CHAR_STRING: // CHAR_STRING
      case symbol_kind::S_DBC_IDENTIFIER: // DBC_IDENTIFIER
      case symbol_kind::S_NS_VALUE: // NS_VALUE
      case symbol_kind::S_semicolon_with_optional_eol: // semicolon_with_optional_eol
      case symbol_kind::S_char_string: // char_string
      case symbol_kind::S_char_string_eol: // char_string_eol
      case symbol_kind::S_dbc_identifier: // dbc_identifier
      case symbol_kind::S_candb_version_string: // candb_version_string
      case symbol_kind::S_node_name: // node_name
      case symbol_kind::S_value_table_name: // value_table_name
      case symbol_kind::S_message_name: // message_name
      case symbol_kind::S_transmitter: // transmitter
      case symbol_kind::S_signal_name: // signal_name
      case symbol_kind::S_multiplexer_indicator: // multiplexer_indicator
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_receiver: // receiver
      case symbol_kind::S_env_var_name: // env_var_name
      case symbol_kind::S_access_node: // access_node
      case symbol_kind::S_signal_type_name: // signal_type_name
      case symbol_kind::S_signal_group_name: // signal_group_name
      case symbol_kind::S_attribute_name: // attribute_name
      case symbol_kind::S_attribute_value: // attribute_value
      case symbol_kind::S_multiplexed_signal_name: // multiplexed_signal_name
      case symbol_kind::S_multiplexor_switch_name: // multiplexor_switch_name
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_char_strings: // char_strings
      case symbol_kind::S_new_symbol_values: // new_symbol_values
        value.move< std::vector<std::string> > (that.value);
        break;

      case symbol_kind::S_access_type: // access_type
        value.move< uint16_t > (that.value);
        break;

      case symbol_kind::S_unsigned_integer: // unsigned_integer
      case symbol_kind::S_baudrate: // baudrate
      case symbol_kind::S_btr1: // btr1
      case symbol_kind::S_btr2: // btr2
      case symbol_kind::S_message_id: // message_id
      case symbol_kind::S_message_size: // message_size
      case symbol_kind::S_start_bit: // start_bit
      case symbol_kind::S_signal_size: // signal_size
      case symbol_kind::S_ev_id: // ev_id
      case symbol_kind::S_data_size: // data_size
      case symbol_kind::S_repetitions: // repetitions
        value.move< uint32_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (loc));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_object_type: // object_type
        yylhs.value.emplace< AttributeObjectType > ();
        break;

      case symbol_kind::S_attribute_value_type: // attribute_value_type
        yylhs.value.emplace< AttributeValueType > ();
        break;

      case symbol_kind::S_byte_order: // byte_order
        yylhs.value.emplace< ByteOrder > ();
        break;

      case symbol_kind::S_env_var_type: // env_var_type
        yylhs.value.emplace< EnvironmentVariable::Type > ();
        break;

      case symbol_kind::S_multiplexor_value_range_comma: // multiplexor_value_range_comma
      case symbol_kind::S_multiplexor_value_range: // multiplexor_value_range
        yylhs.value.emplace< ExtendedMultiplexor::ValueRange > ();
        break;

      case symbol_kind::S_message: // message
        yylhs.value.emplace< Message > ();
        break;

      case symbol_kind::S_signal: // signal
        yylhs.value.emplace< Signal > ();
        break;

      case symbol_kind::S_signal_extended_value_type_type: // signal_extended_value_type_type
        yylhs.value.emplace< Signal::ExtendedValueType > ();
        break;

      case symbol_kind::S_value_table: // value_table
        yylhs.value.emplace< ValueTable > ();
        break;

      case symbol_kind::S_value_type: // value_type
        yylhs.value.emplace< ValueType > ();
        break;

      case symbol_kind::S_double: // double
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_offset: // offset
      case symbol_kind::S_minimum: // minimum
      case symbol_kind::S_maximum: // maximum
      case symbol_kind::S_initial_value: // initial_value
      case symbol_kind::S_default_value: // default_value
        yylhs.value.emplace< double > ();
        break;

      case symbol_kind::S_signed_64_integer: // signed_64_integer
        yylhs.value.emplace< int64_t > ();
        break;

      case symbol_kind::S_value_encoding_descriptions: // value_encoding_descriptions
        yylhs.value.emplace< std::map<int64_t, std::string> > ();
        break;

      case symbol_kind::S_signals: // signals
        yylhs.value.emplace< std::map<std::string, Signal> > ();
        break;

      case symbol_kind::S_value_encoding_description: // value_encoding_description
        yylhs.value.emplace< std::pair<int64_t, std::string> > ();
        break;

      case symbol_kind::S_multiplexor_value_ranges: // multiplexor_value_ranges
        yylhs.value.emplace< std::set<ExtendedMultiplexor::ValueRange> > ();
        break;

      case symbol_kind::S_signal_names: // signal_names
      case symbol_kind::S_receivers: // receivers
      case symbol_kind::S_transmitters: // transmitters
      case symbol_kind::S_access_nodes: // access_nodes
        yylhs.value.emplace< std::set<std::string> > ();
        break;

      case symbol_kind::S_UNSIGNED_INTEGER: // UNSIGNED_INTEGER
      case symbol_kind::S_MULTIPLEXOR_VALUE_RANGE: // MULTIPLEXOR_VALUE_RANGE
      case symbol_kind::S_SIGNED_INTEGER: // SIGNED_INTEGER
      case symbol_kind::S_DOUBLE: // DOUBLE
      case symbol_kind::S_CHAR_STRING: // CHAR_STRING
      case symbol_kind::S_DBC_IDENTIFIER: // DBC_IDENTIFIER
      case symbol_kind::S_NS_VALUE: // NS_VALUE
      case symbol_kind::S_semicolon_with_optional_eol: // semicolon_with_optional_eol
      case symbol_kind::S_char_string: // char_string
      case symbol_kind::S_char_string_eol: // char_string_eol
      case symbol_kind::S_dbc_identifier: // dbc_identifier
      case symbol_kind::S_candb_version_string: // candb_version_string
      case symbol_kind::S_node_name: // node_name
      case symbol_kind::S_value_table_name: // value_table_name
      case symbol_kind::S_message_name: // message_name
      case symbol_kind::S_transmitter: // transmitter
      case symbol_kind::S_signal_name: // signal_name
      case symbol_kind::S_multiplexer_indicator: // multiplexer_indicator
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_receiver: // receiver
      case symbol_kind::S_env_var_name: // env_var_name
      case symbol_kind::S_access_node: // access_node
      case symbol_kind::S_signal_type_name: // signal_type_name
      case symbol_kind::S_signal_group_name: // signal_group_name
      case symbol_kind::S_attribute_name: // attribute_name
      case symbol_kind::S_attribute_value: // attribute_value
      case symbol_kind::S_multiplexed_signal_name: // multiplexed_signal_name
      case symbol_kind::S_multiplexor_switch_name: // multiplexor_switch_name
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_char_strings: // char_strings
      case symbol_kind::S_new_symbol_values: // new_symbol_values
        yylhs.value.emplace< std::vector<std::string> > ();
        break;

      case symbol_kind::S_access_type: // access_type
        yylhs.value.emplace< uint16_t > ();
        break;

      case symbol_kind::S_unsigned_integer: // unsigned_integer
      case symbol_kind::S_baudrate: // baudrate
      case symbol_kind::S_btr1: // btr1
      case symbol_kind::S_btr2: // btr2
      case symbol_kind::S_message_id: // message_id
      case symbol_kind::S_message_size: // message_size
      case symbol_kind::S_start_bit: // start_bit
      case symbol_kind::S_signal_size: // signal_size
      case symbol_kind::S_ev_id: // ev_id
      case symbol_kind::S_data_size: // data_size
      case symbol_kind::S_repetitions: // repetitions
        yylhs.value.emplace< uint32_t > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 3: // semicolon_with_optional_eol: SEMICOLON
#line 248 "Parser.yy"
                    { yylhs.value.as < std::string > () = ""; }
#line 1230 "Parser.cpp"
    break;

  case 4: // semicolon_with_optional_eol: SEMICOLON EOL
#line 249 "Parser.yy"
                        { yylhs.value.as < std::string > () = ""; }
#line 1236 "Parser.cpp"
    break;

  case 5: // unsigned_integer: UNSIGNED_INTEGER
#line 252 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = std::stoul(yystack_[0].value.as < std::string > ()); }
#line 1242 "Parser.cpp"
    break;

  case 6: // signed_64_integer: UNSIGNED_INTEGER
#line 256 "Parser.yy"
                           { yylhs.value.as < int64_t > () = std::stoll(yystack_[0].value.as < std::string > ()); }
#line 1248 "Parser.cpp"
    break;

  case 7: // signed_64_integer: SIGNED_INTEGER
#line 257 "Parser.yy"
                         { yylhs.value.as < int64_t > () = std::stoll(yystack_[0].value.as < std::string > ()); }
#line 1254 "Parser.cpp"
    break;

  case 8: // double: DOUBLE
#line 260 "Parser.yy"
                 { yylhs.value.as < double > () = std::stod(yystack_[0].value.as < std::string > ()); }
#line 1260 "Parser.cpp"
    break;

  case 9: // double: SIGNED_INTEGER
#line 261 "Parser.yy"
                         { yylhs.value.as < double > () = std::stod(yystack_[0].value.as < std::string > ()); }
#line 1266 "Parser.cpp"
    break;

  case 10: // double: UNSIGNED_INTEGER
#line 262 "Parser.yy"
                           { yylhs.value.as < double > () = std::stod(yystack_[0].value.as < std::string > ()); }
#line 1272 "Parser.cpp"
    break;

  case 11: // char_string: CHAR_STRING
#line 265 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1278 "Parser.cpp"
    break;

  case 12: // char_string_eol: CHAR_STRING EOL
#line 269 "Parser.yy"
                          { yylhs.value.as < std::string > () = yystack_[1].value.as < std::string > (); }
#line 1284 "Parser.cpp"
    break;

  case 13: // char_string_eol: CHAR_STRING
#line 270 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1290 "Parser.cpp"
    break;

  case 14: // char_strings: CHAR_STRING
#line 273 "Parser.yy"
                      { yylhs.value.as < std::vector<std::string> > () = std::vector<std::string>(); yylhs.value.as < std::vector<std::string> > ().push_back(yystack_[0].value.as < std::string > ()); }
#line 1296 "Parser.cpp"
    break;

  case 15: // char_strings: char_strings COMMA CHAR_STRING
#line 274 "Parser.yy"
                                         { yylhs.value.as < std::vector<std::string> > () = yystack_[2].value.as < std::vector<std::string> > (); yylhs.value.as < std::vector<std::string> > ().push_back(yystack_[0].value.as < std::string > ()); }
#line 1302 "Parser.cpp"
    break;

  case 16: // dbc_identifier: DBC_IDENTIFIER
#line 277 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1308 "Parser.cpp"
    break;

  case 17: // version: VERSION candb_version_string EOL
#line 282 "Parser.yy"
                                           { network->version = yystack_[1].value.as < std::string > (); }
#line 1314 "Parser.cpp"
    break;

  case 18: // candb_version_string: char_string
#line 285 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1320 "Parser.cpp"
    break;

  case 20: // new_symbols: NS COLON EOL new_symbol_values
#line 290 "Parser.yy"
                            { network->newSymbols = yystack_[0].value.as < std::vector<std::string> > (); }
#line 1326 "Parser.cpp"
    break;

  case 21: // new_symbol_values: %empty
#line 293 "Parser.yy"
                 { yylhs.value.as < std::vector<std::string> > () = std::vector<std::string>(); }
#line 1332 "Parser.cpp"
    break;

  case 22: // new_symbol_values: new_symbol_values NS_VALUE EOL
#line 294 "Parser.yy"
                                         { yylhs.value.as < std::vector<std::string> > () = yystack_[2].value.as < std::vector<std::string> > (); yylhs.value.as < std::vector<std::string> > ().push_back(yystack_[1].value.as < std::string > ()); }
#line 1338 "Parser.cpp"
    break;

  case 24: // bit_timing: BS COLON baudrate COLON btr1 COMMA btr2 EOL
#line 300 "Parser.yy"
                                                      {
              network->bitTiming.baudrate = yystack_[5].value.as < uint32_t > ();
              network->bitTiming.btr1 = yystack_[3].value.as < uint32_t > ();
              network->bitTiming.btr2 = yystack_[1].value.as < uint32_t > ();
          }
#line 1348 "Parser.cpp"
    break;

  case 25: // baudrate: unsigned_integer
#line 307 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1354 "Parser.cpp"
    break;

  case 26: // btr1: unsigned_integer
#line 310 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1360 "Parser.cpp"
    break;

  case 27: // btr2: unsigned_integer
#line 313 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1366 "Parser.cpp"
    break;

  case 31: // node_names: node_names node_name
#line 323 "Parser.yy"
                               { network->nodes[yystack_[0].value.as < std::string > ()].name = yystack_[0].value.as < std::string > (); }
#line 1372 "Parser.cpp"
    break;

  case 32: // node_name: dbc_identifier
#line 326 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1378 "Parser.cpp"
    break;

  case 34: // value_tables: value_tables value_table
#line 332 "Parser.yy"
                                   { network->valueTables[yystack_[0].value.as < ValueTable > ().name] = yystack_[0].value.as < ValueTable > (); }
#line 1384 "Parser.cpp"
    break;

  case 35: // value_table: VAL_TABLE value_table_name value_encoding_descriptions semicolon_with_optional_eol
#line 336 "Parser.yy"
                                                                                             {
              yylhs.value.as < ValueTable > () = ValueTable();
              yylhs.value.as < ValueTable > ().name = yystack_[2].value.as < std::string > ();
              yylhs.value.as < ValueTable > ().valueDescriptions = yystack_[1].value.as < std::map<int64_t, std::string> > ();
          }
#line 1394 "Parser.cpp"
    break;

  case 36: // value_table_name: dbc_identifier
#line 343 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1400 "Parser.cpp"
    break;

  case 37: // value_encoding_descriptions: %empty
#line 349 "Parser.yy"
                 { yylhs.value.as < std::map<int64_t, std::string> > () = std::map<int64_t, std::string>(); }
#line 1406 "Parser.cpp"
    break;

  case 38: // value_encoding_descriptions: value_encoding_descriptions value_encoding_description
#line 350 "Parser.yy"
                                                                 { yylhs.value.as < std::map<int64_t, std::string> > () = yystack_[1].value.as < std::map<int64_t, std::string> > (); yylhs.value.as < std::map<int64_t, std::string> > ().insert(yystack_[0].value.as < std::pair<int64_t, std::string> > ()); }
#line 1412 "Parser.cpp"
    break;

  case 39: // value_encoding_description: signed_64_integer char_string_eol
#line 355 "Parser.yy"
                                            { yylhs.value.as < std::pair<int64_t, std::string> > () = std::make_pair(yystack_[1].value.as < int64_t > (), yystack_[0].value.as < std::string > ()); }
#line 1418 "Parser.cpp"
    break;

  case 40: // value_encoding_description: signed_64_integer EOL char_string_eol
#line 356 "Parser.yy"
                                                { yylhs.value.as < std::pair<int64_t, std::string> > () = std::make_pair(yystack_[2].value.as < int64_t > (), yystack_[0].value.as < std::string > ()); }
#line 1424 "Parser.cpp"
    break;

  case 42: // messages: messages message
#line 362 "Parser.yy"
                           { network->messages[yystack_[0].value.as < Message > ().id] = yystack_[0].value.as < Message > (); }
#line 1430 "Parser.cpp"
    break;

  case 43: // message: BO message_id message_name COLON message_size transmitter EOL signals
#line 365 "Parser.yy"
                                                                                {
              yylhs.value.as < Message > () = Message();
              yylhs.value.as < Message > ().id = yystack_[6].value.as < uint32_t > ();
              yylhs.value.as < Message > ().name = yystack_[5].value.as < std::string > ();
              yylhs.value.as < Message > ().size = yystack_[3].value.as < uint32_t > ();
              yylhs.value.as < Message > ().transmitter = yystack_[2].value.as < std::string > ();
              yylhs.value.as < Message > ().signals = yystack_[0].value.as < std::map<std::string, Signal> > ();
          }
#line 1443 "Parser.cpp"
    break;

  case 44: // message_id: unsigned_integer
#line 375 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1449 "Parser.cpp"
    break;

  case 45: // message_name: dbc_identifier
#line 378 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1455 "Parser.cpp"
    break;

  case 46: // message_size: unsigned_integer
#line 381 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1461 "Parser.cpp"
    break;

  case 47: // transmitter: node_name
#line 384 "Parser.yy"
                    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1467 "Parser.cpp"
    break;

  case 48: // transmitter: VECTOR_XXX
#line 385 "Parser.yy"
                     { yylhs.value.as < std::string > () = ""; }
#line 1473 "Parser.cpp"
    break;

  case 49: // signals: %empty
#line 392 "Parser.yy"
                 { yylhs.value.as < std::map<std::string, Signal> > () = std::map<std::string, Signal>(); }
#line 1479 "Parser.cpp"
    break;

  case 50: // signals: signals signal
#line 393 "Parser.yy"
                         { yylhs.value.as < std::map<std::string, Signal> > () = yystack_[1].value.as < std::map<std::string, Signal> > (); yylhs.value.as < std::map<std::string, Signal> > ()[yystack_[0].value.as < Signal > ().name] = yystack_[0].value.as < Signal > (); }
#line 1485 "Parser.cpp"
    break;

  case 51: // signal: SG signal_name multiplexer_indicator COLON start_bit VERTICAL_BAR signal_size AT byte_order value_type OPEN_PARENTHESIS factor COMMA offset CLOSE_PARENTHESIS OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit receivers EOL
#line 396 "Parser.yy"
                                                                                                                                                                                                                                                   {
              yylhs.value.as < Signal > () = Signal();
              yylhs.value.as < Signal > ().name = yystack_[21].value.as < std::string > ();
              if (yystack_[20].value.as < std::string > () == "*") {
                  yylhs.value.as < Signal > ().multiplexor = Signal::Multiplexor::MultiplexorSwitch;
              } else
              if (!yystack_[20].value.as < std::string > ().empty()) {
                  yylhs.value.as < Signal > ().multiplexor = Signal::Multiplexor::MultiplexedSignal;
                  yylhs.value.as < Signal > ().multiplexerSwitchValue = std::stoul(yystack_[20].value.as < std::string > ());
              }
              yylhs.value.as < Signal > ().startBit = yystack_[18].value.as < uint32_t > ();
              yylhs.value.as < Signal > ().bitSize = yystack_[16].value.as < uint32_t > ();
              yylhs.value.as < Signal > ().byteOrder = yystack_[14].value.as < ByteOrder > ();
              yylhs.value.as < Signal > ().valueType = yystack_[13].value.as < ValueType > ();
              yylhs.value.as < Signal > ().factor = yystack_[11].value.as < double > ();
              yylhs.value.as < Signal > ().offset = yystack_[9].value.as < double > ();
              yylhs.value.as < Signal > ().minimum = yystack_[6].value.as < double > ();
              yylhs.value.as < Signal > ().maximum = yystack_[4].value.as < double > ();
              yylhs.value.as < Signal > ().unit = yystack_[2].value.as < std::string > ();
              yylhs.value.as < Signal > ().receivers = yystack_[1].value.as < std::set<std::string> > ();
          }
#line 1511 "Parser.cpp"
    break;

  case 52: // signal_name: dbc_identifier
#line 419 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1517 "Parser.cpp"
    break;

  case 53: // signal_names: %empty
#line 422 "Parser.yy"
                 { yylhs.value.as < std::set<std::string> > () = std::set<std::string>(); }
#line 1523 "Parser.cpp"
    break;

  case 54: // signal_names: signal_names signal_name
#line 423 "Parser.yy"
                                   { yylhs.value.as < std::set<std::string> > () = yystack_[1].value.as < std::set<std::string> > (); yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ()); }
#line 1529 "Parser.cpp"
    break;

  case 55: // multiplexer_indicator: %empty
#line 426 "Parser.yy"
                 { yylhs.value.as < std::string > () = ""; }
#line 1535 "Parser.cpp"
    break;

  case 56: // multiplexer_indicator: DBC_IDENTIFIER
#line 427 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); yylhs.value.as < std::string > ().erase(0, 1); }
#line 1541 "Parser.cpp"
    break;

  case 57: // multiplexer_indicator: UPPER_M
#line 428 "Parser.yy"
                  { yylhs.value.as < std::string > () = "*"; }
#line 1547 "Parser.cpp"
    break;

  case 58: // start_bit: unsigned_integer
#line 434 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1553 "Parser.cpp"
    break;

  case 59: // signal_size: unsigned_integer
#line 437 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1559 "Parser.cpp"
    break;

  case 60: // byte_order: UNSIGNED_INTEGER
#line 440 "Parser.yy"
                           {
              if (yystack_[0].value.as < std::string > () == "0") { yylhs.value.as < ByteOrder > () = ByteOrder::BigEndian; }
              if (yystack_[0].value.as < std::string > () == "1") { yylhs.value.as < ByteOrder > () = ByteOrder::LittleEndian; }
          }
#line 1568 "Parser.cpp"
    break;

  case 61: // value_type: PLUS
#line 446 "Parser.yy"
               { yylhs.value.as < ValueType > () = ValueType::Unsigned; }
#line 1574 "Parser.cpp"
    break;

  case 62: // value_type: MINUS
#line 447 "Parser.yy"
                { yylhs.value.as < ValueType > () = ValueType::Signed; }
#line 1580 "Parser.cpp"
    break;

  case 63: // factor: double
#line 450 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1586 "Parser.cpp"
    break;

  case 64: // offset: double
#line 453 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1592 "Parser.cpp"
    break;

  case 65: // minimum: double
#line 456 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1598 "Parser.cpp"
    break;

  case 66: // maximum: double
#line 459 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1604 "Parser.cpp"
    break;

  case 67: // unit: char_string
#line 462 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1610 "Parser.cpp"
    break;

  case 68: // receivers: receiver
#line 465 "Parser.yy"
                   {
              yylhs.value.as < std::set<std::string> > () = std::set<std::string>();
              if (!yystack_[0].value.as < std::string > ().empty()) {
                  yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ());
              }
          }
#line 1621 "Parser.cpp"
    break;

  case 69: // receivers: receivers COMMA receiver
#line 471 "Parser.yy"
                                   {
              yylhs.value.as < std::set<std::string> > () = yystack_[2].value.as < std::set<std::string> > ();
              if (!yystack_[0].value.as < std::string > ().empty()) {
                  yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ());
              }
          }
#line 1632 "Parser.cpp"
    break;

  case 70: // receiver: node_name
#line 479 "Parser.yy"
                    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1638 "Parser.cpp"
    break;

  case 71: // receiver: VECTOR_XXX
#line 480 "Parser.yy"
                     { yylhs.value.as < std::string > () = ""; }
#line 1644 "Parser.cpp"
    break;

  case 74: // signal_extended_value_type: SIG_VALTYPE message_id signal_name COLON signal_extended_value_type_type semicolon_with_optional_eol
#line 488 "Parser.yy"
                                                                                                               {
              network->messages[yystack_[4].value.as < uint32_t > ()].signals[yystack_[3].value.as < std::string > ()].extendedValueType = yystack_[1].value.as < Signal::ExtendedValueType > ();
          }
#line 1652 "Parser.cpp"
    break;

  case 75: // signal_extended_value_type_type: unsigned_integer
#line 493 "Parser.yy"
                           {
              switch (yystack_[0].value.as < uint32_t > ()) {
              case 0:
                  yylhs.value.as < Signal::ExtendedValueType > () = Signal::ExtendedValueType::Integer;
                  break;
              case 1:
                  yylhs.value.as < Signal::ExtendedValueType > () = Signal::ExtendedValueType::Float;
                  break;
              case 2:
                  yylhs.value.as < Signal::ExtendedValueType > () = Signal::ExtendedValueType::Double;
                  break;
              }
          }
#line 1670 "Parser.cpp"
    break;

  case 78: // message_transmitter: BO_TX_BU message_id COLON transmitters semicolon_with_optional_eol
#line 515 "Parser.yy"
                                                                             { network->messages[yystack_[3].value.as < uint32_t > ()].transmitters = yystack_[1].value.as < std::set<std::string> > (); }
#line 1676 "Parser.cpp"
    break;

  case 79: // transmitters: transmitter
#line 518 "Parser.yy"
                      { yylhs.value.as < std::set<std::string> > () = std::set<std::string>(); yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ()); }
#line 1682 "Parser.cpp"
    break;

  case 80: // transmitters: transmitters COMMA transmitter
#line 519 "Parser.yy"
                                         { yylhs.value.as < std::set<std::string> > () = yystack_[2].value.as < std::set<std::string> > (); yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ()); }
#line 1688 "Parser.cpp"
    break;

  case 84: // value_descriptions_for_signal: VAL message_id signal_name value_encoding_descriptions semicolon_with_optional_eol
#line 530 "Parser.yy"
                                                                                             {
              network->messages[yystack_[3].value.as < uint32_t > ()].signals[yystack_[2].value.as < std::string > ()].valueDescriptions = yystack_[1].value.as < std::map<int64_t, std::string> > ();
          }
#line 1696 "Parser.cpp"
    break;

  case 87: // environment_variable: EV env_var_name COLON env_var_type OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit initial_value ev_id access_type access_nodes semicolon_with_optional_eol
#line 542 "Parser.yy"
                                                                                                                                                                                   {
              EnvironmentVariable & environmentVariable = network->environmentVariables[yystack_[13].value.as < std::string > ()];
              environmentVariable.name = yystack_[13].value.as < std::string > ();
              environmentVariable.type = yystack_[11].value.as < EnvironmentVariable::Type > ();
              environmentVariable.minimum = yystack_[9].value.as < double > ();
              environmentVariable.maximum = yystack_[7].value.as < double > ();
              environmentVariable.unit = yystack_[5].value.as < std::string > ();
              environmentVariable.initialValue = yystack_[4].value.as < double > ();
              environmentVariable.id = yystack_[3].value.as < uint32_t > ();
              if (yystack_[2].value.as < uint16_t > () >> 15) {
                  environmentVariable.type = EnvironmentVariable::Type::String;
              }
              switch(yystack_[2].value.as < uint16_t > () & 0xff) {
                  case 0:
                      environmentVariable.accessType = EnvironmentVariable::AccessType::Unrestricted;
                      break;
                  case 1:
                      environmentVariable.accessType = EnvironmentVariable::AccessType::Read;
                      break;
                  case 2:
                      environmentVariable.accessType = EnvironmentVariable::AccessType::Write;
                      break;
                  case 3:
                      environmentVariable.accessType = EnvironmentVariable::AccessType::ReadWrite;
                      break;
              }
              environmentVariable.accessNodes = yystack_[1].value.as < std::set<std::string> > ();
          }
#line 1729 "Parser.cpp"
    break;

  case 88: // env_var_name: dbc_identifier
#line 572 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1735 "Parser.cpp"
    break;

  case 89: // env_var_type: UNSIGNED_INTEGER
#line 575 "Parser.yy"
                           {
              if (yystack_[0].value.as < std::string > () == "0") { yylhs.value.as < EnvironmentVariable::Type > () = EnvironmentVariable::Type::Integer; }
              if (yystack_[0].value.as < std::string > () == "1") { yylhs.value.as < EnvironmentVariable::Type > () = EnvironmentVariable::Type::Float; }
              if (yystack_[0].value.as < std::string > () == "2") { yylhs.value.as < EnvironmentVariable::Type > () = EnvironmentVariable::Type::String; }
          }
#line 1745 "Parser.cpp"
    break;

  case 90: // initial_value: double
#line 582 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1751 "Parser.cpp"
    break;

  case 91: // ev_id: unsigned_integer
#line 585 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1757 "Parser.cpp"
    break;

  case 92: // access_type: DUMMY_NODE_VECTOR0
#line 588 "Parser.yy"
                             { yylhs.value.as < uint16_t > () = 0x0000; }
#line 1763 "Parser.cpp"
    break;

  case 93: // access_type: DUMMY_NODE_VECTOR1
#line 589 "Parser.yy"
                             { yylhs.value.as < uint16_t > () = 0x0001; }
#line 1769 "Parser.cpp"
    break;

  case 94: // access_type: DUMMY_NODE_VECTOR2
#line 590 "Parser.yy"
                             { yylhs.value.as < uint16_t > () = 0x0002; }
#line 1775 "Parser.cpp"
    break;

  case 95: // access_type: DUMMY_NODE_VECTOR3
#line 591 "Parser.yy"
                             { yylhs.value.as < uint16_t > () = 0x0003; }
#line 1781 "Parser.cpp"
    break;

  case 96: // access_type: DUMMY_NODE_VECTOR8000
#line 592 "Parser.yy"
                                 { yylhs.value.as < uint16_t > () = 0x8000; }
#line 1787 "Parser.cpp"
    break;

  case 97: // access_type: DUMMY_NODE_VECTOR8001
#line 593 "Parser.yy"
                                 { yylhs.value.as < uint16_t > () = 0x8001; }
#line 1793 "Parser.cpp"
    break;

  case 98: // access_type: DUMMY_NODE_VECTOR8002
#line 594 "Parser.yy"
                                 { yylhs.value.as < uint16_t > () = 0x8002; }
#line 1799 "Parser.cpp"
    break;

  case 99: // access_type: DUMMY_NODE_VECTOR8003
#line 595 "Parser.yy"
                                 { yylhs.value.as < uint16_t > () = 0x8003; }
#line 1805 "Parser.cpp"
    break;

  case 100: // access_nodes: access_node
#line 598 "Parser.yy"
                      {
              yylhs.value.as < std::set<std::string> > () = std::set<std::string>();
              if (!yystack_[0].value.as < std::string > ().empty()) {
                  yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ());
              }
          }
#line 1816 "Parser.cpp"
    break;

  case 101: // access_nodes: access_nodes COMMA access_node
#line 604 "Parser.yy"
                                         {
              yylhs.value.as < std::set<std::string> > () = yystack_[2].value.as < std::set<std::string> > ();
              if (!yystack_[0].value.as < std::string > ().empty()) {
                  yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ());
              }
          }
#line 1827 "Parser.cpp"
    break;

  case 102: // access_node: node_name
#line 612 "Parser.yy"
                    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1833 "Parser.cpp"
    break;

  case 103: // access_node: VECTOR_XXX
#line 613 "Parser.yy"
                     { yylhs.value.as < std::string > () = ""; }
#line 1839 "Parser.cpp"
    break;

  case 106: // environment_variable_data: ENVVAR_DATA env_var_name COLON data_size semicolon_with_optional_eol
#line 621 "Parser.yy"
                                                                               {
              network->environmentVariables[yystack_[3].value.as < std::string > ()].type = EnvironmentVariable::Type::Data;
              network->environmentVariables[yystack_[3].value.as < std::string > ()].dataSize = yystack_[1].value.as < uint32_t > ();
          }
#line 1848 "Parser.cpp"
    break;

  case 107: // data_size: unsigned_integer
#line 628 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1854 "Parser.cpp"
    break;

  case 108: // value_descriptions_for_env_var: VAL env_var_name value_encoding_descriptions semicolon_with_optional_eol
#line 634 "Parser.yy"
                                                                                   {
              network->environmentVariables[yystack_[2].value.as < std::string > ()].valueDescriptions = yystack_[1].value.as < std::map<int64_t, std::string> > ();
          }
#line 1862 "Parser.cpp"
    break;

  case 111: // signal_type: SGTYPE signal_type_name COLON signal_size VERTICAL_BAR byte_order value_type OPEN_PARENTHESIS factor COMMA offset CLOSE_PARENTHESIS OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit default_value COMMA value_table_name semicolon_with_optional_eol
#line 649 "Parser.yy"
                                                                                {
              SignalType & signalType = network->signalTypes[yystack_[20].value.as < std::string > ()];
              signalType.name = yystack_[20].value.as < std::string > ();
              signalType.size = yystack_[18].value.as < uint32_t > ();
              signalType.byteOrder = yystack_[16].value.as < ByteOrder > ();
              signalType.valueType = yystack_[15].value.as < ValueType > ();
              signalType.factor = yystack_[13].value.as < double > ();
              signalType.offset = yystack_[11].value.as < double > ();
              signalType.minimum = yystack_[8].value.as < double > ();
              signalType.maximum = yystack_[6].value.as < double > ();
              signalType.unit = yystack_[4].value.as < std::string > ();
              signalType.defaultValue = yystack_[3].value.as < double > ();
              signalType.valueTable = yystack_[1].value.as < std::string > ();
          }
#line 1881 "Parser.cpp"
    break;

  case 112: // signal_type_name: dbc_identifier
#line 665 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1887 "Parser.cpp"
    break;

  case 113: // default_value: double
#line 668 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1893 "Parser.cpp"
    break;

  case 116: // signal_group: SIG_GROUP message_id signal_group_name repetitions COLON signal_names semicolon_with_optional_eol
#line 686 "Parser.yy"
                                                                                                            {
              SignalGroup & signalGroup = network->messages[yystack_[5].value.as < uint32_t > ()].signalGroups[yystack_[4].value.as < std::string > ()];
              signalGroup.messageId = yystack_[5].value.as < uint32_t > ();
              signalGroup.name = yystack_[4].value.as < std::string > ();
              signalGroup.repetitions = yystack_[3].value.as < uint32_t > ();
              signalGroup.signals = yystack_[1].value.as < std::set<std::string> > ();
          }
#line 1905 "Parser.cpp"
    break;

  case 117: // signal_group_name: dbc_identifier
#line 695 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1911 "Parser.cpp"
    break;

  case 118: // repetitions: unsigned_integer
#line 698 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1917 "Parser.cpp"
    break;

  case 121: // comment: CM char_string semicolon_with_optional_eol
#line 708 "Parser.yy"
                                                     { network->comment = yystack_[1].value.as < std::string > (); }
#line 1923 "Parser.cpp"
    break;

  case 122: // comment: CM BU node_name char_string semicolon_with_optional_eol
#line 709 "Parser.yy"
                                                                  { network->nodes[yystack_[2].value.as < std::string > ()].comment = yystack_[1].value.as < std::string > (); }
#line 1929 "Parser.cpp"
    break;

  case 123: // comment: CM BO message_id char_string semicolon_with_optional_eol
#line 710 "Parser.yy"
                                                                   { network->messages[yystack_[2].value.as < uint32_t > ()].comment = yystack_[1].value.as < std::string > (); }
#line 1935 "Parser.cpp"
    break;

  case 124: // comment: CM SG message_id signal_name char_string semicolon_with_optional_eol
#line 711 "Parser.yy"
                                                                               { network->messages[yystack_[3].value.as < uint32_t > ()].signals[yystack_[2].value.as < std::string > ()].comment = yystack_[1].value.as < std::string > (); }
#line 1941 "Parser.cpp"
    break;

  case 125: // comment: CM EV env_var_name char_string semicolon_with_optional_eol
#line 712 "Parser.yy"
                                                                     { network->environmentVariables[yystack_[2].value.as < std::string > ()].comment = yystack_[1].value.as < std::string > (); }
#line 1947 "Parser.cpp"
    break;

  case 128: // attribute_definition: BA_DEF object_type attribute_name attribute_value_type semicolon_with_optional_eol
#line 724 "Parser.yy"
                                                                                             {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[2].value.as < std::string > ()];
              attributeDefinition.name = yystack_[2].value.as < std::string > ();
              attributeDefinition.objectType = yystack_[3].value.as < AttributeObjectType > ();
              attributeDefinition.valueType = yystack_[1].value.as < AttributeValueType > ();
          }
#line 1958 "Parser.cpp"
    break;

  case 129: // attribute_definition: BA_DEF_REL object_type attribute_name attribute_value_type semicolon_with_optional_eol
#line 730 "Parser.yy"
                                                                                                 {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[2].value.as < std::string > ()];
              attributeDefinition.name = yystack_[2].value.as < std::string > ();
              attributeDefinition.objectType = yystack_[3].value.as < AttributeObjectType > ();
              attributeDefinition.valueType = yystack_[1].value.as < AttributeValueType > ();
          }
#line 1969 "Parser.cpp"
    break;

  case 130: // object_type: %empty
#line 738 "Parser.yy"
                 { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::Network; }
#line 1975 "Parser.cpp"
    break;

  case 131: // object_type: BU
#line 739 "Parser.yy"
             { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::Node; }
#line 1981 "Parser.cpp"
    break;

  case 132: // object_type: BO
#line 740 "Parser.yy"
             { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::Message; }
#line 1987 "Parser.cpp"
    break;

  case 133: // object_type: SG
#line 741 "Parser.yy"
             { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::Signal; }
#line 1993 "Parser.cpp"
    break;

  case 134: // object_type: EV
#line 742 "Parser.yy"
             { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::EnvironmentVariable; }
#line 1999 "Parser.cpp"
    break;

  case 135: // object_type: BU_EV_REL
#line 743 "Parser.yy"
                    { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::ControlUnitEnvironmentVariable; }
#line 2005 "Parser.cpp"
    break;

  case 136: // object_type: BU_BO_REL
#line 744 "Parser.yy"
                    { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::NodeTxMessage; }
#line 2011 "Parser.cpp"
    break;

  case 137: // object_type: BU_SG_REL
#line 745 "Parser.yy"
                    { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::NodeMappedRxSignal; }
#line 2017 "Parser.cpp"
    break;

  case 138: // attribute_name: char_string
#line 748 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2023 "Parser.cpp"
    break;

  case 139: // attribute_value_type: INT signed_64_integer signed_64_integer
#line 752 "Parser.yy"
                                                  {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::Int;
              yylhs.value.as < AttributeValueType > ().integerValue.minimum = yystack_[1].value.as < int64_t > ();
              yylhs.value.as < AttributeValueType > ().integerValue.maximum = yystack_[0].value.as < int64_t > ();
          }
#line 2034 "Parser.cpp"
    break;

  case 140: // attribute_value_type: HEX signed_64_integer signed_64_integer
#line 758 "Parser.yy"
                                                  {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::Hex;
              yylhs.value.as < AttributeValueType > ().hexValue.minimum = yystack_[1].value.as < int64_t > ();
              yylhs.value.as < AttributeValueType > ().hexValue.maximum = yystack_[0].value.as < int64_t > ();
          }
#line 2045 "Parser.cpp"
    break;

  case 141: // attribute_value_type: FLOAT double double
#line 764 "Parser.yy"
                              {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::Float;
              yylhs.value.as < AttributeValueType > ().floatValue.minimum = yystack_[1].value.as < double > ();
              yylhs.value.as < AttributeValueType > ().floatValue.maximum = yystack_[0].value.as < double > ();
          }
#line 2056 "Parser.cpp"
    break;

  case 142: // attribute_value_type: STRING
#line 770 "Parser.yy"
                 {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::String;
          }
#line 2065 "Parser.cpp"
    break;

  case 143: // attribute_value_type: ENUM char_strings
#line 774 "Parser.yy"
                            {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::Enum;
              yylhs.value.as < AttributeValueType > ().enumValues = yystack_[0].value.as < std::vector<std::string> > ();
          }
#line 2075 "Parser.cpp"
    break;

  case 146: // attribute_default: BA_DEF_DEF attribute_name attribute_value semicolon_with_optional_eol
#line 788 "Parser.yy"
                                                                                {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[2].value.as < std::string > ()];
              Attribute & attributeDefault = network->attributeDefaults[yystack_[2].value.as < std::string > ()];
              attributeDefault.name = yystack_[2].value.as < std::string > ();
              attributeDefault.objectType = attributeDefinition.objectType;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeDefault.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeDefault.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attributeDefault.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attributeDefault.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  attributeDefault.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              }
          }
#line 2105 "Parser.cpp"
    break;

  case 147: // attribute_default: BA_DEF_DEF_REL attribute_name attribute_value semicolon_with_optional_eol
#line 813 "Parser.yy"
                                                                                    {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[2].value.as < std::string > ()];
              Attribute & attributeDefault = network->attributeDefaults[yystack_[2].value.as < std::string > ()];
              attributeDefault.name = yystack_[2].value.as < std::string > ();
              attributeDefault.objectType = attributeDefinition.objectType;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeDefault.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeDefault.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attributeDefault.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attributeDefault.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  attributeDefault.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              }
          }
#line 2135 "Parser.cpp"
    break;

  case 148: // attribute_value: UNSIGNED_INTEGER
#line 840 "Parser.yy"
                           { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2141 "Parser.cpp"
    break;

  case 149: // attribute_value: SIGNED_INTEGER
#line 841 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2147 "Parser.cpp"
    break;

  case 150: // attribute_value: DOUBLE
#line 842 "Parser.yy"
                 { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2153 "Parser.cpp"
    break;

  case 151: // attribute_value: CHAR_STRING
#line 843 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2159 "Parser.cpp"
    break;

  case 154: // attribute_value_for_object: BA attribute_name attribute_value semicolon_with_optional_eol
#line 853 "Parser.yy"
                                                                        {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[2].value.as < std::string > ()];
              Attribute & attribute = network->attributeValues[yystack_[2].value.as < std::string > ()];
              attribute.name = yystack_[2].value.as < std::string > ();
              attribute.objectType = AttributeObjectType::Network;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.enumValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              }
          }
#line 2190 "Parser.cpp"
    break;

  case 155: // attribute_value_for_object: BA attribute_name BU node_name attribute_value semicolon_with_optional_eol
#line 879 "Parser.yy"
                                                                                     {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[4].value.as < std::string > ()];
              Attribute & attribute = network->nodes[yystack_[2].value.as < std::string > ()].attributeValues[yystack_[4].value.as < std::string > ()];
              attribute.name = yystack_[4].value.as < std::string > ();
              attribute.objectType = AttributeObjectType::Node;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.enumValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              }
          }
#line 2221 "Parser.cpp"
    break;

  case 156: // attribute_value_for_object: BA attribute_name BO message_id attribute_value semicolon_with_optional_eol
#line 905 "Parser.yy"
                                                                                      {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[4].value.as < std::string > ()];
              Attribute & attribute = network->messages[yystack_[2].value.as < uint32_t > ()].attributeValues[yystack_[4].value.as < std::string > ()];
              attribute.name = yystack_[4].value.as < std::string > ();
              attribute.objectType = AttributeObjectType::Message;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.enumValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              }
          }
#line 2252 "Parser.cpp"
    break;

  case 157: // attribute_value_for_object: BA attribute_name SG message_id signal_name attribute_value semicolon_with_optional_eol
#line 931 "Parser.yy"
                                                                                                  {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[5].value.as < std::string > ()];
              Attribute & attribute = network->messages[yystack_[3].value.as < uint32_t > ()].signals[yystack_[2].value.as < std::string > ()].attributeValues[yystack_[5].value.as < std::string > ()];
              attribute.name = yystack_[5].value.as < std::string > ();
              attribute.objectType = AttributeObjectType::Signal;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-17 STW: Improve enum handling
                  attribute.stringValue = attributeDefinition.valueType.GetEnumValue(yystack_[1].value.as < std::string > ());
                  break;
              }
          }
#line 2283 "Parser.cpp"
    break;

  case 158: // attribute_value_for_object: BA attribute_name EV env_var_name attribute_value semicolon_with_optional_eol
#line 957 "Parser.yy"
                                                                                        {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[4].value.as < std::string > ()];
              Attribute & attribute = network->environmentVariables[yystack_[2].value.as < std::string > ()].attributeValues[yystack_[4].value.as < std::string > ()];
              attribute.name = yystack_[4].value.as < std::string > ();
              attribute.objectType = AttributeObjectType::EnvironmentVariable;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.enumValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              }
          }
#line 2314 "Parser.cpp"
    break;

  case 159: // attribute_value_for_object: BA_REL attribute_name BU_EV_REL node_name env_var_name attribute_value semicolon_with_optional_eol
#line 983 "Parser.yy"
                                                                                                             {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[5].value.as < std::string > ()];
              AttributeRelation & attributeRelation = network->attributeRelationValues[yystack_[5].value.as < std::string > ()];
              attributeRelation.name = yystack_[5].value.as < std::string > ();
              attributeRelation.objectType = AttributeObjectType::ControlUnitEnvironmentVariable;
              attributeRelation.nodeName = yystack_[3].value.as < std::string > ();
              attributeRelation.environmentVariableName = yystack_[2].value.as < std::string > ();
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attributeRelation.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attributeRelation.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.enumValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              }
          }
#line 2347 "Parser.cpp"
    break;

  case 160: // attribute_value_for_object: BA_REL attribute_name BU_BO_REL node_name message_id attribute_value semicolon_with_optional_eol
#line 1011 "Parser.yy"
                                                                                                           {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[5].value.as < std::string > ()];
              AttributeRelation & attributeRelation = network->attributeRelationValues[yystack_[5].value.as < std::string > ()];
              attributeRelation.name = yystack_[5].value.as < std::string > ();
              attributeRelation.objectType = AttributeObjectType::NodeTxMessage;
              attributeRelation.nodeName = yystack_[3].value.as < std::string > ();
              attributeRelation.messageId = yystack_[2].value.as < uint32_t > ();
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attributeRelation.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attributeRelation.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.enumValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              }
          }
#line 2380 "Parser.cpp"
    break;

  case 161: // attribute_value_for_object: BA_REL attribute_name BU_SG_REL node_name SG message_id signal_name attribute_value semicolon_with_optional_eol
#line 1039 "Parser.yy"
                                                                                                                          {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[7].value.as < std::string > ()];
              AttributeRelation & attributeRelation = network->attributeRelationValues[yystack_[7].value.as < std::string > ()];
              attributeRelation.name = yystack_[7].value.as < std::string > ();
              attributeRelation.objectType = AttributeObjectType::NodeMappedRxSignal;
              attributeRelation.nodeName = yystack_[5].value.as < std::string > ();
              attributeRelation.messageId = yystack_[3].value.as < uint32_t > ();
              attributeRelation.signalName = yystack_[2].value.as < std::string > ();
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.integerValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.hexValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::Float:
                  attributeRelation.floatValue = std::stod(yystack_[1].value.as < std::string > ());
                  break;
              case AttributeValueType::Type::String:
                  attributeRelation.stringValue = yystack_[1].value.as < std::string > ();
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.enumValue = std::stoll(yystack_[1].value.as < std::string > ());
                  break;
              }
          }
#line 2414 "Parser.cpp"
    break;

  case 164: // multiplexed_signal: SG_MUL_VAL message_id multiplexed_signal_name multiplexor_switch_name multiplexor_value_ranges semicolon_with_optional_eol
#line 1077 "Parser.yy"
                                                                                                                                     {
              ExtendedMultiplexor & extendedMultiplexor = network->messages[yystack_[4].value.as < uint32_t > ()].signals[yystack_[3].value.as < std::string > ()].extendedMultiplexors[yystack_[2].value.as < std::string > ()];
              extendedMultiplexor.switchName = yystack_[2].value.as < std::string > ();
              extendedMultiplexor.valueRanges = yystack_[1].value.as < std::set<ExtendedMultiplexor::ValueRange> > ();
          }
#line 2424 "Parser.cpp"
    break;

  case 165: // multiplexed_signal_name: dbc_identifier
#line 1084 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2430 "Parser.cpp"
    break;

  case 166: // multiplexor_switch_name: dbc_identifier
#line 1087 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2436 "Parser.cpp"
    break;

  case 167: // multiplexor_value_ranges: %empty
#line 1091 "Parser.yy"
                 { yylhs.value.as < std::set<ExtendedMultiplexor::ValueRange> > () = std::set<ExtendedMultiplexor::ValueRange>(); }
#line 2442 "Parser.cpp"
    break;

  case 168: // multiplexor_value_ranges: multiplexor_value_ranges multiplexor_value_range_comma
#line 1092 "Parser.yy"
                                                                 { yylhs.value.as < std::set<ExtendedMultiplexor::ValueRange> > () = yystack_[1].value.as < std::set<ExtendedMultiplexor::ValueRange> > (); yylhs.value.as < std::set<ExtendedMultiplexor::ValueRange> > ().insert(yystack_[0].value.as < ExtendedMultiplexor::ValueRange > ()); }
#line 2448 "Parser.cpp"
    break;

  case 169: // multiplexor_value_range_comma: multiplexor_value_range COMMA
#line 1096 "Parser.yy"
                                        { yylhs.value.as < ExtendedMultiplexor::ValueRange > () = yystack_[1].value.as < ExtendedMultiplexor::ValueRange > (); }
#line 2454 "Parser.cpp"
    break;

  case 170: // multiplexor_value_range_comma: multiplexor_value_range
#line 1097 "Parser.yy"
                                  { yylhs.value.as < ExtendedMultiplexor::ValueRange > () = yystack_[0].value.as < ExtendedMultiplexor::ValueRange > (); }
#line 2460 "Parser.cpp"
    break;

  case 171: // multiplexor_value_range: MULTIPLEXOR_VALUE_RANGE
#line 1101 "Parser.yy"
                                  { 
        const std::string complete = yystack_[0].value.as < std::string > (); 
        const std::string token1_str = complete.substr(0, complete.find("-")); 
        const std::string token2_str = complete.substr(token1_str.size() + 1, complete.size() - (token1_str.size() + 1)); 
        const uint32_t token1_int = std::stoul(token1_str);
        const uint32_t token2_int = std::stoul(token2_str);
        yylhs.value.as < ExtendedMultiplexor::ValueRange > () = std::make_pair(token1_int, token2_int); 
        }
#line 2473 "Parser.cpp"
    break;


#line 2477 "Parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  Parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // Parser::context.
  Parser::context::context (const Parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  Parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  Parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short Parser::yypact_ninf_ = -304;

  const signed char Parser::yytable_ninf_ = -1;

  const short
  Parser::yypact_[] =
  {
       9,    16,    28,    27,  -304,  -304,   -23,  -304,    -2,    62,
    -304,    14,    32,    95,  -304,     3,    42,  -304,   133,  -304,
    -304,  -304,    76,   150,   156,   112,   171,  -304,     0,   168,
    -304,   162,  -304,  -304,   117,  -304,  -304,  -304,  -304,  -304,
    -304,   171,  -304,   159,   171,     8,  -304,   168,   171,  -304,
     164,  -304,   119,  -304,  -304,   127,  -304,     5,  -304,  -304,
     134,   135,   168,  -304,   163,  -304,  -304,   136,   191,  -304,
     171,    86,  -304,   142,   168,  -304,   169,  -304,  -304,  -304,
      86,  -304,  -304,  -304,    50,   202,   155,   168,  -304,   179,
     152,    86,  -304,  -304,   166,   171,  -304,   160,    66,  -304,
      68,  -304,  -304,   145,  -304,   165,   171,   168,   171,   171,
     168,   165,     7,     7,  -304,    74,   204,  -304,  -304,  -304,
    -304,   170,  -304,  -304,   172,    16,    16,   168,    16,  -304,
    -304,  -304,  -304,  -304,  -304,  -304,  -304,    16,    16,    16,
      16,  -304,   113,   168,  -304,   145,   220,   165,   165,  -304,
      16,   165,  -304,    92,    92,   109,   109,    16,    16,   203,
    -304,    38,  -304,   176,  -304,   102,  -304,  -304,   165,  -304,
     123,   123,   145,  -304,   223,   165,   165,  -304,  -304,  -304,
    -304,   165,   165,    26,   111,     6,  -304,  -304,   197,  -304,
    -304,   174,    16,  -304,  -304,   182,  -304,   123,   123,   145,
    -304,   175,  -304,  -304,  -304,  -304,   168,   171,   171,   168,
     165,   168,   168,   168,   168,  -304,   171,   217,  -304,   171,
    -304,   145,   145,  -304,  -304,  -304,   231,   109,   109,   168,
     109,  -304,   168,   171,   226,  -304,     8,   168,   171,  -304,
     190,  -304,   187,  -304,   171,  -304,   184,  -304,   165,   165,
     109,   165,   109,   109,   171,     8,  -304,  -304,   171,   168,
     171,  -304,   171,  -304,   237,   145,  -304,  -304,   165,  -304,
     165,   165,   168,  -304,  -304,   189,   192,   168,   193,  -304,
    -304,  -304,  -304,  -304,  -304,  -304,  -304,    87,  -304,   195,
    -304,  -304,  -304,   109,  -304,   171,  -304,   168,   220,  -304,
    -304,    79,  -304,   205,   165,    11,  -304,   165,  -304,  -304,
     102,    87,  -304,   145,  -304,  -304,  -304,  -304,    13,   200,
    -304,   201,  -304,  -304,  -304,   196,   145,   145,  -304,   198,
     218,   145,    16,   215,   145,   222,  -304,   213,   145,   168,
     224,   165,   145,  -304,   225,    16,    91,  -304,  -304,   110,
    -304,    91,  -304,  -304
  };

  const unsigned char
  Parser::yydefact_[] =
  {
       0,     0,     0,    19,    11,    18,     0,     1,     0,     0,
      17,     0,     0,     0,    21,     0,     0,    33,    20,     5,
      23,    25,     0,    29,    41,     0,     0,    30,     0,     0,
      34,    76,    22,    26,     0,    16,    28,    32,    31,    36,
      37,     0,    42,    85,     0,     0,    44,     0,     0,    77,
     104,    27,     0,     6,     7,     3,    35,     0,    38,    45,
       0,     0,     0,    86,   109,    24,     4,    13,     0,    39,
       0,     0,    88,     0,     0,   105,   119,    12,    40,    46,
       0,    48,    47,    79,     0,     0,     0,     0,   110,   126,
       0,     0,    78,    89,     0,     0,   112,     0,     0,   120,
     144,    49,    80,     0,   107,     0,     0,     0,     0,     0,
       0,     0,   130,   130,   127,   152,    43,    10,     9,     8,
      65,     0,   106,    59,     0,     0,     0,     0,     0,   121,
     131,   132,   133,   134,   135,   136,   137,     0,     0,     0,
       0,   145,    81,     0,    50,     0,     0,     0,     0,    52,
       0,     0,   138,     0,     0,     0,     0,     0,     0,   114,
     153,    55,    66,     0,    60,     0,   122,   123,     0,   125,
       0,     0,     0,   142,     0,     0,     0,   148,   149,   150,
     151,     0,     0,     0,     0,     0,    82,    83,    72,    56,
      57,     0,     0,    61,    62,     0,   124,     0,     0,     0,
      14,   143,   128,   129,   146,   147,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    37,     0,   162,   115,     0,
      67,     0,     0,   139,   140,   141,     0,     0,     0,     0,
       0,   154,     0,     0,     0,    37,     0,     0,     0,    73,
       2,    58,     0,    90,     0,    63,     0,    15,     0,     0,
       0,     0,     0,     0,     0,     0,   108,   117,     0,     0,
       0,   163,     0,    91,     0,     0,   155,   156,     0,   158,
       0,     0,     0,    84,   118,     0,     0,     0,     0,    92,
      93,    94,    95,    96,    97,    98,    99,     0,    64,     0,
     157,   159,   160,     0,    53,     0,   165,     0,     0,   103,
     102,     0,   100,     0,     0,     0,    75,     0,   166,   167,
       0,     0,    87,     0,   161,   116,    54,    74,     0,     0,
     101,     0,   171,   164,   168,   170,     0,     0,   169,     0,
       0,     0,     0,     0,     0,     0,   113,     0,     0,     0,
       0,     0,     0,   111,     0,     0,     0,    71,    70,     0,
      68,     0,    51,    69
  };

  const short
  Parser::yypgoto_[] =
  {
    -304,  -304,   -29,   -10,  -144,  -130,    -1,   186,  -304,   -26,
    -304,  -304,  -304,  -304,  -304,  -304,  -304,  -304,  -304,  -304,
     -27,  -304,  -304,   -59,  -190,  -304,  -304,  -304,   -44,  -304,
    -304,   -33,  -304,  -304,  -136,  -304,  -304,  -304,    20,   -15,
     -24,   -39,   -43,  -303,  -283,  -270,  -304,   -61,  -304,  -304,
    -304,  -304,  -304,  -304,  -304,  -304,  -304,  -304,   -72,  -304,
    -304,  -304,  -304,  -304,   -20,  -304,  -304,  -304,  -304,  -304,
    -304,  -304,  -304,  -304,  -304,  -304,  -304,  -304,  -304,  -304,
    -304,   180,   -52,   138,  -304,  -304,   -85,  -304,  -304,  -304,
    -304,  -304,  -304,  -304,  -304,  -304
  };

  const short
  Parser::yydefgoto_[] =
  {
       0,     2,    56,    46,    57,   120,   152,    69,   201,    37,
       3,     6,     9,    18,    13,    22,    34,    52,    17,    28,
      82,    24,    30,    40,    45,    58,    31,    42,    47,    60,
      80,    83,   116,   144,   150,   305,   191,   242,   124,   165,
     195,   246,   289,   121,   163,   221,   349,   350,   217,   239,
     307,    43,    49,    84,   159,   186,    50,    63,    73,    94,
     244,   264,   287,   301,   302,    64,    75,   105,   187,    76,
      88,    97,   337,   188,   218,   258,   275,    89,    99,   100,
     114,   137,   153,   175,   115,   141,   181,   142,   160,   240,
     261,   297,   309,   318,   324,   325
  };

  const short
  Parser::yytable_[] =
  {
       5,    38,    86,    39,    61,    21,    19,   161,    35,    19,
     321,    53,    67,    54,    35,   162,    33,   322,     1,    35,
     130,    59,   131,     4,   132,   236,   197,   198,     7,   177,
     133,   178,   179,   180,    51,   340,    72,     8,   128,   206,
      10,   207,   199,   208,   330,   255,   189,    90,    72,   209,
     134,   135,   136,   223,   224,    92,    11,   190,   102,   344,
      79,    96,   334,    36,   126,   127,    20,    55,    68,   225,
      55,   182,    55,     4,    12,   346,   122,    14,   235,   107,
     125,   108,   129,   109,    72,   104,   154,   155,   156,   110,
      15,   243,   245,   250,    35,    35,   123,   111,   210,    35,
      23,   149,    81,   299,   112,   183,   184,   347,    16,    55,
     113,    91,   177,   215,   178,   179,   180,   149,   166,   167,
     139,   140,   169,   276,   147,   148,    53,   151,    54,   170,
     171,   172,   173,   174,    26,   288,   293,   230,    55,   196,
     311,   214,   248,   249,    25,   251,   202,   203,   117,   168,
     118,   119,   204,   205,   211,   212,   213,   193,   194,    72,
     252,   157,   158,   228,   229,   268,    27,   270,   271,   316,
      29,   351,   237,   352,    19,    32,    35,    41,    44,   227,
      48,   231,    65,    72,   232,   233,   234,    62,   149,   253,
      66,   220,    70,    71,   259,    74,   245,   162,    67,    77,
      85,   288,    87,   149,   336,    93,    72,   256,   304,   241,
     272,   257,   162,    95,    98,   101,   277,   103,   106,   266,
     267,   143,   269,   164,    55,   185,   273,   145,   192,   146,
     200,   216,   219,   149,   263,   222,   226,   238,   247,   290,
     260,   291,   292,   254,   262,   265,   149,   294,   274,   303,
     295,   296,   123,   326,    78,   298,   313,   328,   327,   331,
     300,   279,   280,   281,   282,   283,   284,   285,   286,   335,
     332,   308,   312,   338,   339,   314,   315,   345,   317,   149,
     341,   342,   278,   310,   300,   306,   319,   329,   333,   323,
     353,   320,   176,   138,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   343,    39,     0,     0,     0,     0,     0,   348,
       0,     0,     0,     0,   348,     0,     0,     0,     0,     0,
       0,   220,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   220
  };

  const short
  Parser::yycheck_[] =
  {
       1,    28,    74,    29,    48,    15,     3,   143,     8,     3,
     313,     3,     7,     5,     8,   145,    26,     4,     9,     8,
      13,    47,    15,     7,    17,   215,   170,   171,     0,     3,
      23,     5,     6,     7,    44,   338,    62,    10,   110,    13,
      63,    15,   172,    17,   327,   235,     8,    80,    74,    23,
      43,    44,    45,   197,   198,    84,    58,    19,    91,   342,
      70,    87,   332,    63,   108,   109,    63,    59,    63,   199,
      59,   156,    59,     7,    12,   345,   105,    63,   214,    13,
     107,    15,   111,    17,   110,    95,   138,   139,   140,    23,
      58,   221,   222,   229,     8,     8,   106,    98,   183,     8,
      58,   127,    16,    16,    36,   157,   158,    16,    13,    59,
      42,    61,     3,   185,     5,     6,     7,   143,   147,   148,
      46,    47,   151,   259,   125,   126,     3,   128,     5,    37,
      38,    39,    40,    41,    58,   265,   272,   209,    59,   168,
      61,   185,   227,   228,    11,   230,   175,   176,     3,   150,
       5,     6,   181,   182,    43,    44,    45,    55,    56,   185,
     232,    48,    49,   207,   208,   250,    16,   252,   253,   305,
      14,    61,   216,    63,     3,    63,     8,    15,    61,   206,
      21,   210,    63,   209,   211,   212,   213,    23,   214,   233,
      63,   192,    58,    58,   238,    32,   326,   327,     7,    63,
      58,   331,    33,   229,   334,     3,   232,   236,   293,   219,
     254,   237,   342,    58,    35,    63,   260,    51,    58,   248,
     249,    17,   251,     3,    59,    22,   255,    57,    52,    57,
       7,    34,    58,   259,   244,    53,    61,    20,     7,   268,
      50,   270,   271,    17,    57,    61,   272,    58,   258,    54,
      58,   277,   262,    53,    68,    62,    51,    61,    57,    61,
     287,    24,    25,    26,    27,    28,    29,    30,    31,    54,
      52,   297,   301,    51,    61,   304,   305,    52,   307,   305,
     339,    57,   262,   298,   311,   295,   310,   326,   331,   318,
     351,   311,   154,   113,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   341,   339,    -1,    -1,    -1,    -1,    -1,   346,
      -1,    -1,    -1,    -1,   351,    -1,    -1,    -1,    -1,    -1,
      -1,   332,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   345
  };

  const unsigned char
  Parser::yystos_[] =
  {
       0,     9,    65,    74,     7,    70,    75,     0,    10,    76,
      63,    58,    12,    78,    63,    58,    13,    82,    77,     3,
      63,    67,    79,    58,    85,    11,    58,    16,    83,    14,
      86,    90,    63,    67,    80,     8,    63,    73,    84,    73,
      87,    15,    91,   115,    61,    88,    67,    92,    21,   116,
     120,    67,    81,     3,     5,    59,    66,    68,    89,    73,
      93,    92,    23,   121,   129,    63,    63,     7,    63,    71,
      58,    58,    73,   122,    32,   130,   133,    63,    71,    67,
      94,    16,    84,    95,   117,    58,   122,    33,   134,   141,
      95,    61,    66,     3,   123,    58,    73,   135,    35,   142,
     143,    63,    95,    51,    67,   131,    58,    13,    15,    17,
      23,    70,    36,    42,   144,   148,    96,     3,     5,     6,
      69,   107,    66,    67,   102,    84,    92,    92,   122,    66,
      13,    15,    17,    23,    43,    44,    45,   145,   145,    46,
      47,   149,   151,    17,    97,    57,    57,    70,    70,    73,
      98,    70,    70,   146,   146,   146,   146,    48,    49,   118,
     152,    98,    69,   108,     3,   103,    66,    66,    70,    66,
      37,    38,    39,    40,    41,   147,   147,     3,     5,     6,
       7,   150,   150,   146,   146,    22,   119,   132,   137,     8,
      19,   100,    52,    55,    56,   104,    66,    68,    68,    69,
       7,    72,    66,    66,    66,    66,    13,    15,    17,    23,
     150,    43,    44,    45,    92,   122,    34,   112,   138,    58,
      70,   109,    53,    68,    68,    69,    61,    84,    92,    92,
     122,    66,    84,    84,    84,    98,    88,    92,    20,   113,
     153,    67,   101,    69,   124,    69,   105,     7,   150,   150,
      98,   150,   122,    92,    17,    88,    66,    73,   139,    92,
      50,   154,    57,    67,   125,    61,    66,    66,   150,    66,
     150,   150,    92,    66,    67,   140,    98,    92,   102,    24,
      25,    26,    27,    28,    29,    30,    31,   126,    69,   106,
      66,    66,    66,    98,    58,    58,    73,   155,    62,    16,
      84,   127,   128,    54,   150,    99,    67,   114,    73,   156,
     103,    61,    66,    51,    66,    66,    98,    66,   157,   104,
     128,   107,     4,    66,   158,   159,    53,    57,    61,   105,
     108,    61,    52,   106,   109,    54,    69,   136,    51,    61,
     107,    87,    57,    66,   108,    52,   109,    16,    84,   110,
     111,    61,    63,   111
  };

  const unsigned char
  Parser::yyr1_[] =
  {
       0,    64,    65,    66,    66,    67,    68,    68,    69,    69,
      69,    70,    71,    71,    72,    72,    73,    74,    75,    76,
      76,    77,    77,    78,    78,    79,    80,    81,    82,    83,
      83,    83,    84,    85,    85,    86,    87,    88,    88,    89,
      89,    90,    90,    91,    92,    93,    94,    95,    95,    96,
      96,    97,    98,    99,    99,   100,   100,   100,   101,   102,
     103,   104,   104,   105,   106,   107,   108,   109,   110,   110,
     111,   111,   112,   112,   113,   114,   115,   115,   116,   117,
     117,   118,   118,   118,   119,   120,   120,   121,   122,   123,
     124,   125,   126,   126,   126,   126,   126,   126,   126,   126,
     127,   127,   128,   128,   129,   129,   130,   131,   132,   133,
     133,   134,   135,   136,   137,   137,   138,   139,   140,   141,
     141,   142,   142,   142,   142,   142,   143,   143,   144,   144,
     145,   145,   145,   145,   145,   145,   145,   145,   146,   147,
     147,   147,   147,   147,   148,   148,   149,   149,   150,   150,
     150,   150,   151,   151,   152,   152,   152,   152,   152,   152,
     152,   152,   153,   153,   154,   155,   156,   157,   157,   158,
     158,   159
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,    18,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     3,     1,     3,     1,     0,
       4,     0,     3,     3,     8,     1,     1,     1,     4,     0,
       1,     2,     1,     0,     2,     4,     1,     0,     2,     2,
       3,     0,     2,     8,     1,     1,     1,     1,     1,     0,
       2,    23,     1,     0,     2,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     0,     2,     6,     1,     0,     2,     5,     1,
       3,     0,     2,     2,     5,     0,     2,    15,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     1,     0,     2,     5,     1,     4,     0,
       2,    22,     1,     1,     0,     2,     7,     1,     1,     0,
       2,     3,     5,     5,     6,     5,     0,     2,     5,     5,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     1,     2,     0,     2,     4,     4,     1,     1,
       1,     1,     0,     2,     4,     6,     6,     7,     6,     7,
       7,     9,     0,     2,     6,     1,     1,     0,     2,     2,
       1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "END", "error", "\"invalid token\"", "UNSIGNED_INTEGER",
  "MULTIPLEXOR_VALUE_RANGE", "SIGNED_INTEGER", "DOUBLE", "CHAR_STRING",
  "DBC_IDENTIFIER", "VERSION", "NS", "NS_VALUE", "BS", "BU", "VAL_TABLE",
  "BO", "VECTOR_XXX", "SG", "LOWER_M", "UPPER_M", "SIG_VALTYPE",
  "BO_TX_BU", "VAL", "EV", "DUMMY_NODE_VECTOR0", "DUMMY_NODE_VECTOR1",
  "DUMMY_NODE_VECTOR2", "DUMMY_NODE_VECTOR3", "DUMMY_NODE_VECTOR8000",
  "DUMMY_NODE_VECTOR8001", "DUMMY_NODE_VECTOR8002",
  "DUMMY_NODE_VECTOR8003", "ENVVAR_DATA", "SGTYPE", "SIG_GROUP", "CM",
  "BA_DEF", "INT", "HEX", "FLOAT", "STRING", "ENUM", "BA_DEF_REL",
  "BU_EV_REL", "BU_BO_REL", "BU_SG_REL", "BA_DEF_DEF", "BA_DEF_DEF_REL",
  "BA", "BA_REL", "SG_MUL_VAL", "OPEN_BRACKET", "CLOSE_BRACKET",
  "OPEN_PARENTHESIS", "CLOSE_PARENTHESIS", "PLUS", "MINUS", "VERTICAL_BAR",
  "COLON", "SEMICOLON", "ASSIGN", "COMMA", "AT", "EOL", "$accept",
  "network", "semicolon_with_optional_eol", "unsigned_integer",
  "signed_64_integer", "double", "char_string", "char_string_eol",
  "char_strings", "dbc_identifier", "version", "candb_version_string",
  "new_symbols", "new_symbol_values", "bit_timing", "baudrate", "btr1",
  "btr2", "nodes", "node_names", "node_name", "value_tables",
  "value_table", "value_table_name", "value_encoding_descriptions",
  "value_encoding_description", "messages", "message", "message_id",
  "message_name", "message_size", "transmitter", "signals", "signal",
  "signal_name", "signal_names", "multiplexer_indicator", "start_bit",
  "signal_size", "byte_order", "value_type", "factor", "offset", "minimum",
  "maximum", "unit", "receivers", "receiver",
  "signal_extended_value_types", "signal_extended_value_type",
  "signal_extended_value_type_type", "message_transmitters",
  "message_transmitter", "transmitters", "value_descriptions",
  "value_descriptions_for_signal", "environment_variables",
  "environment_variable", "env_var_name", "env_var_type", "initial_value",
  "ev_id", "access_type", "access_nodes", "access_node",
  "environment_variables_data", "environment_variable_data", "data_size",
  "value_descriptions_for_env_var", "signal_types", "signal_type",
  "signal_type_name", "default_value", "signal_groups", "signal_group",
  "signal_group_name", "repetitions", "comments", "comment",
  "attribute_definitions", "attribute_definition", "object_type",
  "attribute_name", "attribute_value_type", "attribute_defaults",
  "attribute_default", "attribute_value", "attribute_values",
  "attribute_value_for_object", "extended_multiplexing",
  "multiplexed_signal", "multiplexed_signal_name",
  "multiplexor_switch_name", "multiplexor_value_ranges",
  "multiplexor_value_range_comma", "multiplexor_value_range", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  Parser::yyrline_[] =
  {
       0,   219,   219,   248,   249,   252,   256,   257,   260,   261,
     262,   265,   269,   270,   273,   274,   277,   282,   285,   288,
     289,   293,   294,   299,   300,   307,   310,   313,   318,   321,
     322,   323,   326,   331,   332,   336,   343,   349,   350,   355,
     356,   361,   362,   365,   375,   378,   381,   384,   385,   392,
     393,   396,   419,   422,   423,   426,   427,   428,   434,   437,
     440,   446,   447,   450,   453,   456,   459,   462,   465,   471,
     479,   480,   483,   484,   488,   493,   510,   511,   515,   518,
     519,   524,   525,   526,   530,   537,   538,   542,   572,   575,
     582,   585,   588,   589,   590,   591,   592,   593,   594,   595,
     598,   604,   612,   613,   616,   617,   621,   628,   634,   641,
     642,   646,   665,   668,   681,   682,   686,   695,   698,   703,
     704,   708,   709,   710,   711,   712,   719,   720,   724,   730,
     738,   739,   740,   741,   742,   743,   744,   745,   748,   752,
     758,   764,   770,   774,   783,   784,   788,   813,   840,   841,
     842,   843,   848,   849,   853,   879,   905,   931,   957,   983,
    1011,  1039,  1072,  1073,  1077,  1084,  1087,  1091,  1092,  1096,
    1097,  1101
  };

  void
  Parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  Parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 4 "Parser.yy"
} } // Vector::DBC
#line 3218 "Parser.cpp"

#line 1111 "Parser.yy"


void Vector::DBC::Parser::error(const location_type & location, const std::string & message)
{
    // 2023-03-14 STW: Add interface to allow log entries
    std::stringstream stream;
    stream << location;
    std::string log_entry = "Parse error at " + stream.str() + ": " + message;
    osc_write_log_warning("DBC parser", log_entry.c_str());
    //DEBUG entry
    std::cout<<log_entry<<std::endl;
}
