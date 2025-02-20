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

  case 6: // signed_64_integer: DOUBLE
#line 257 "Parser.yy"
                 { yylhs.value.as < int64_t > () = std::stod(yystack_[0].value.as < std::string > ());
                   osc_write_log_warning("DBC parser", "Converted \"" + yystack_[0].value.as < std::string > () + "\" to int due to data type restrictions"); }
#line 1249 "Parser.cpp"
    break;

  case 7: // signed_64_integer: UNSIGNED_INTEGER
#line 259 "Parser.yy"
                           { yylhs.value.as < int64_t > () = std::stoll(yystack_[0].value.as < std::string > ()); }
#line 1255 "Parser.cpp"
    break;

  case 8: // signed_64_integer: SIGNED_INTEGER
#line 260 "Parser.yy"
                         { yylhs.value.as < int64_t > () = std::stoll(yystack_[0].value.as < std::string > ()); }
#line 1261 "Parser.cpp"
    break;

  case 9: // double: DOUBLE
#line 263 "Parser.yy"
                 { yylhs.value.as < double > () = std::stod(yystack_[0].value.as < std::string > ()); }
#line 1267 "Parser.cpp"
    break;

  case 10: // double: SIGNED_INTEGER
#line 264 "Parser.yy"
                         { yylhs.value.as < double > () = std::stod(yystack_[0].value.as < std::string > ()); }
#line 1273 "Parser.cpp"
    break;

  case 11: // double: UNSIGNED_INTEGER
#line 265 "Parser.yy"
                           { yylhs.value.as < double > () = std::stod(yystack_[0].value.as < std::string > ()); }
#line 1279 "Parser.cpp"
    break;

  case 12: // char_string: CHAR_STRING
#line 268 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1285 "Parser.cpp"
    break;

  case 13: // char_string_eol: CHAR_STRING EOL
#line 272 "Parser.yy"
                          { yylhs.value.as < std::string > () = yystack_[1].value.as < std::string > (); }
#line 1291 "Parser.cpp"
    break;

  case 14: // char_string_eol: CHAR_STRING
#line 273 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1297 "Parser.cpp"
    break;

  case 15: // char_strings: CHAR_STRING
#line 276 "Parser.yy"
                      { yylhs.value.as < std::vector<std::string> > () = std::vector<std::string>(); yylhs.value.as < std::vector<std::string> > ().push_back(yystack_[0].value.as < std::string > ()); }
#line 1303 "Parser.cpp"
    break;

  case 16: // char_strings: char_strings COMMA CHAR_STRING
#line 277 "Parser.yy"
                                         { yylhs.value.as < std::vector<std::string> > () = yystack_[2].value.as < std::vector<std::string> > (); yylhs.value.as < std::vector<std::string> > ().push_back(yystack_[0].value.as < std::string > ()); }
#line 1309 "Parser.cpp"
    break;

  case 17: // dbc_identifier: DBC_IDENTIFIER
#line 280 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1315 "Parser.cpp"
    break;

  case 18: // version: VERSION candb_version_string EOL
#line 285 "Parser.yy"
                                           { network->version = yystack_[1].value.as < std::string > (); }
#line 1321 "Parser.cpp"
    break;

  case 19: // candb_version_string: char_string
#line 288 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1327 "Parser.cpp"
    break;

  case 21: // new_symbols: NS COLON EOL new_symbol_values
#line 293 "Parser.yy"
                            { network->newSymbols = yystack_[0].value.as < std::vector<std::string> > (); }
#line 1333 "Parser.cpp"
    break;

  case 22: // new_symbol_values: %empty
#line 296 "Parser.yy"
                 { yylhs.value.as < std::vector<std::string> > () = std::vector<std::string>(); }
#line 1339 "Parser.cpp"
    break;

  case 23: // new_symbol_values: new_symbol_values NS_VALUE EOL
#line 297 "Parser.yy"
                                         { yylhs.value.as < std::vector<std::string> > () = yystack_[2].value.as < std::vector<std::string> > (); yylhs.value.as < std::vector<std::string> > ().push_back(yystack_[1].value.as < std::string > ()); }
#line 1345 "Parser.cpp"
    break;

  case 25: // bit_timing: BS COLON baudrate COLON btr1 COMMA btr2 EOL
#line 303 "Parser.yy"
                                                      {
              network->bitTiming.baudrate = yystack_[5].value.as < uint32_t > ();
              network->bitTiming.btr1 = yystack_[3].value.as < uint32_t > ();
              network->bitTiming.btr2 = yystack_[1].value.as < uint32_t > ();
          }
#line 1355 "Parser.cpp"
    break;

  case 26: // baudrate: unsigned_integer
#line 310 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1361 "Parser.cpp"
    break;

  case 27: // btr1: unsigned_integer
#line 313 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1367 "Parser.cpp"
    break;

  case 28: // btr2: unsigned_integer
#line 316 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1373 "Parser.cpp"
    break;

  case 32: // node_names: node_names node_name
#line 326 "Parser.yy"
                               { network->nodes[yystack_[0].value.as < std::string > ()].name = yystack_[0].value.as < std::string > (); }
#line 1379 "Parser.cpp"
    break;

  case 33: // node_name: dbc_identifier
#line 329 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1385 "Parser.cpp"
    break;

  case 35: // value_tables: value_tables value_table
#line 335 "Parser.yy"
                                   { network->valueTables[yystack_[0].value.as < ValueTable > ().name] = yystack_[0].value.as < ValueTable > (); }
#line 1391 "Parser.cpp"
    break;

  case 36: // value_table: VAL_TABLE value_table_name value_encoding_descriptions semicolon_with_optional_eol
#line 339 "Parser.yy"
                                                                                             {
              yylhs.value.as < ValueTable > () = ValueTable();
              yylhs.value.as < ValueTable > ().name = yystack_[2].value.as < std::string > ();
              yylhs.value.as < ValueTable > ().valueDescriptions = yystack_[1].value.as < std::map<int64_t, std::string> > ();
          }
#line 1401 "Parser.cpp"
    break;

  case 37: // value_table_name: dbc_identifier
#line 346 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1407 "Parser.cpp"
    break;

  case 38: // value_encoding_descriptions: %empty
#line 352 "Parser.yy"
                 { yylhs.value.as < std::map<int64_t, std::string> > () = std::map<int64_t, std::string>(); }
#line 1413 "Parser.cpp"
    break;

  case 39: // value_encoding_descriptions: value_encoding_descriptions value_encoding_description
#line 353 "Parser.yy"
                                                                 { yylhs.value.as < std::map<int64_t, std::string> > () = yystack_[1].value.as < std::map<int64_t, std::string> > (); yylhs.value.as < std::map<int64_t, std::string> > ().insert(yystack_[0].value.as < std::pair<int64_t, std::string> > ()); }
#line 1419 "Parser.cpp"
    break;

  case 40: // value_encoding_description: signed_64_integer char_string_eol
#line 358 "Parser.yy"
                                            { yylhs.value.as < std::pair<int64_t, std::string> > () = std::make_pair(yystack_[1].value.as < int64_t > (), yystack_[0].value.as < std::string > ()); }
#line 1425 "Parser.cpp"
    break;

  case 41: // value_encoding_description: signed_64_integer EOL char_string_eol
#line 359 "Parser.yy"
                                                { yylhs.value.as < std::pair<int64_t, std::string> > () = std::make_pair(yystack_[2].value.as < int64_t > (), yystack_[0].value.as < std::string > ()); }
#line 1431 "Parser.cpp"
    break;

  case 43: // messages: messages message
#line 365 "Parser.yy"
                           { network->messages[yystack_[0].value.as < Message > ().id] = yystack_[0].value.as < Message > (); }
#line 1437 "Parser.cpp"
    break;

  case 44: // message: BO message_id message_name COLON message_size transmitter EOL signals
#line 368 "Parser.yy"
                                                                                {
              yylhs.value.as < Message > () = Message();
              yylhs.value.as < Message > ().id = yystack_[6].value.as < uint32_t > ();
              yylhs.value.as < Message > ().name = yystack_[5].value.as < std::string > ();
              yylhs.value.as < Message > ().size = yystack_[3].value.as < uint32_t > ();
              yylhs.value.as < Message > ().transmitter = yystack_[2].value.as < std::string > ();
              yylhs.value.as < Message > ().signals = yystack_[0].value.as < std::map<std::string, Signal> > ();
          }
#line 1450 "Parser.cpp"
    break;

  case 45: // message_id: unsigned_integer
#line 378 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1456 "Parser.cpp"
    break;

  case 46: // message_name: dbc_identifier
#line 381 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1462 "Parser.cpp"
    break;

  case 47: // message_size: unsigned_integer
#line 384 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1468 "Parser.cpp"
    break;

  case 48: // transmitter: node_name
#line 387 "Parser.yy"
                    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1474 "Parser.cpp"
    break;

  case 49: // transmitter: VECTOR_XXX
#line 388 "Parser.yy"
                     { yylhs.value.as < std::string > () = ""; }
#line 1480 "Parser.cpp"
    break;

  case 50: // signals: %empty
#line 395 "Parser.yy"
                 { yylhs.value.as < std::map<std::string, Signal> > () = std::map<std::string, Signal>(); }
#line 1486 "Parser.cpp"
    break;

  case 51: // signals: signals signal
#line 396 "Parser.yy"
                         { yylhs.value.as < std::map<std::string, Signal> > () = yystack_[1].value.as < std::map<std::string, Signal> > (); yylhs.value.as < std::map<std::string, Signal> > ()[yystack_[0].value.as < Signal > ().name] = yystack_[0].value.as < Signal > (); }
#line 1492 "Parser.cpp"
    break;

  case 52: // signal: SG signal_name multiplexer_indicator COLON start_bit VERTICAL_BAR signal_size AT byte_order value_type OPEN_PARENTHESIS factor COMMA offset CLOSE_PARENTHESIS OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit receivers EOL
#line 399 "Parser.yy"
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
#line 1518 "Parser.cpp"
    break;

  case 53: // signal_name: dbc_identifier
#line 422 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1524 "Parser.cpp"
    break;

  case 54: // signal_names: %empty
#line 425 "Parser.yy"
                 { yylhs.value.as < std::set<std::string> > () = std::set<std::string>(); }
#line 1530 "Parser.cpp"
    break;

  case 55: // signal_names: signal_names signal_name
#line 426 "Parser.yy"
                                   { yylhs.value.as < std::set<std::string> > () = yystack_[1].value.as < std::set<std::string> > (); yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ()); }
#line 1536 "Parser.cpp"
    break;

  case 56: // multiplexer_indicator: %empty
#line 429 "Parser.yy"
                 { yylhs.value.as < std::string > () = ""; }
#line 1542 "Parser.cpp"
    break;

  case 57: // multiplexer_indicator: DBC_IDENTIFIER
#line 430 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); yylhs.value.as < std::string > ().erase(0, 1); }
#line 1548 "Parser.cpp"
    break;

  case 58: // multiplexer_indicator: UPPER_M
#line 431 "Parser.yy"
                  { yylhs.value.as < std::string > () = "*"; }
#line 1554 "Parser.cpp"
    break;

  case 59: // start_bit: unsigned_integer
#line 437 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1560 "Parser.cpp"
    break;

  case 60: // signal_size: unsigned_integer
#line 440 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1566 "Parser.cpp"
    break;

  case 61: // byte_order: UNSIGNED_INTEGER
#line 443 "Parser.yy"
                           {
              if (yystack_[0].value.as < std::string > () == "0") { yylhs.value.as < ByteOrder > () = ByteOrder::BigEndian; }
              if (yystack_[0].value.as < std::string > () == "1") { yylhs.value.as < ByteOrder > () = ByteOrder::LittleEndian; }
          }
#line 1575 "Parser.cpp"
    break;

  case 62: // value_type: PLUS
#line 449 "Parser.yy"
               { yylhs.value.as < ValueType > () = ValueType::Unsigned; }
#line 1581 "Parser.cpp"
    break;

  case 63: // value_type: MINUS
#line 450 "Parser.yy"
                { yylhs.value.as < ValueType > () = ValueType::Signed; }
#line 1587 "Parser.cpp"
    break;

  case 64: // factor: double
#line 453 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1593 "Parser.cpp"
    break;

  case 65: // offset: double
#line 456 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1599 "Parser.cpp"
    break;

  case 66: // minimum: double
#line 459 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1605 "Parser.cpp"
    break;

  case 67: // maximum: double
#line 462 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1611 "Parser.cpp"
    break;

  case 68: // unit: char_string
#line 465 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1617 "Parser.cpp"
    break;

  case 69: // receivers: receiver
#line 468 "Parser.yy"
                   {
              yylhs.value.as < std::set<std::string> > () = std::set<std::string>();
              if (!yystack_[0].value.as < std::string > ().empty()) {
                  yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ());
              }
          }
#line 1628 "Parser.cpp"
    break;

  case 70: // receivers: receivers COMMA receiver
#line 474 "Parser.yy"
                                   {
              yylhs.value.as < std::set<std::string> > () = yystack_[2].value.as < std::set<std::string> > ();
              if (!yystack_[0].value.as < std::string > ().empty()) {
                  yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ());
              }
          }
#line 1639 "Parser.cpp"
    break;

  case 71: // receiver: node_name
#line 482 "Parser.yy"
                    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1645 "Parser.cpp"
    break;

  case 72: // receiver: VECTOR_XXX
#line 483 "Parser.yy"
                     { yylhs.value.as < std::string > () = ""; }
#line 1651 "Parser.cpp"
    break;

  case 75: // signal_extended_value_type: SIG_VALTYPE message_id signal_name COLON signal_extended_value_type_type semicolon_with_optional_eol
#line 491 "Parser.yy"
                                                                                                               {
              network->messages[yystack_[4].value.as < uint32_t > ()].signals[yystack_[3].value.as < std::string > ()].extendedValueType = yystack_[1].value.as < Signal::ExtendedValueType > ();
          }
#line 1659 "Parser.cpp"
    break;

  case 76: // signal_extended_value_type_type: unsigned_integer
#line 496 "Parser.yy"
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
#line 1677 "Parser.cpp"
    break;

  case 79: // message_transmitter: BO_TX_BU message_id COLON transmitters semicolon_with_optional_eol
#line 518 "Parser.yy"
                                                                             { network->messages[yystack_[3].value.as < uint32_t > ()].transmitters = yystack_[1].value.as < std::set<std::string> > (); }
#line 1683 "Parser.cpp"
    break;

  case 80: // transmitters: transmitter
#line 521 "Parser.yy"
                      { yylhs.value.as < std::set<std::string> > () = std::set<std::string>(); yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ()); }
#line 1689 "Parser.cpp"
    break;

  case 81: // transmitters: transmitters COMMA transmitter
#line 522 "Parser.yy"
                                         { yylhs.value.as < std::set<std::string> > () = yystack_[2].value.as < std::set<std::string> > (); yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ()); }
#line 1695 "Parser.cpp"
    break;

  case 85: // value_descriptions_for_signal: VAL message_id signal_name value_encoding_descriptions semicolon_with_optional_eol
#line 533 "Parser.yy"
                                                                                             {
              network->messages[yystack_[3].value.as < uint32_t > ()].signals[yystack_[2].value.as < std::string > ()].valueDescriptions = yystack_[1].value.as < std::map<int64_t, std::string> > ();
          }
#line 1703 "Parser.cpp"
    break;

  case 88: // environment_variable: EV env_var_name COLON env_var_type OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit initial_value ev_id access_type access_nodes semicolon_with_optional_eol
#line 545 "Parser.yy"
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
#line 1736 "Parser.cpp"
    break;

  case 89: // env_var_name: dbc_identifier
#line 575 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1742 "Parser.cpp"
    break;

  case 90: // env_var_type: UNSIGNED_INTEGER
#line 578 "Parser.yy"
                           {
              if (yystack_[0].value.as < std::string > () == "0") { yylhs.value.as < EnvironmentVariable::Type > () = EnvironmentVariable::Type::Integer; }
              if (yystack_[0].value.as < std::string > () == "1") { yylhs.value.as < EnvironmentVariable::Type > () = EnvironmentVariable::Type::Float; }
              if (yystack_[0].value.as < std::string > () == "2") { yylhs.value.as < EnvironmentVariable::Type > () = EnvironmentVariable::Type::String; }
          }
#line 1752 "Parser.cpp"
    break;

  case 91: // initial_value: double
#line 585 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1758 "Parser.cpp"
    break;

  case 92: // ev_id: unsigned_integer
#line 588 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1764 "Parser.cpp"
    break;

  case 93: // access_type: DUMMY_NODE_VECTOR0
#line 591 "Parser.yy"
                             { yylhs.value.as < uint16_t > () = 0x0000; }
#line 1770 "Parser.cpp"
    break;

  case 94: // access_type: DUMMY_NODE_VECTOR1
#line 592 "Parser.yy"
                             { yylhs.value.as < uint16_t > () = 0x0001; }
#line 1776 "Parser.cpp"
    break;

  case 95: // access_type: DUMMY_NODE_VECTOR2
#line 593 "Parser.yy"
                             { yylhs.value.as < uint16_t > () = 0x0002; }
#line 1782 "Parser.cpp"
    break;

  case 96: // access_type: DUMMY_NODE_VECTOR3
#line 594 "Parser.yy"
                             { yylhs.value.as < uint16_t > () = 0x0003; }
#line 1788 "Parser.cpp"
    break;

  case 97: // access_type: DUMMY_NODE_VECTOR8000
#line 595 "Parser.yy"
                                 { yylhs.value.as < uint16_t > () = 0x8000; }
#line 1794 "Parser.cpp"
    break;

  case 98: // access_type: DUMMY_NODE_VECTOR8001
#line 596 "Parser.yy"
                                 { yylhs.value.as < uint16_t > () = 0x8001; }
#line 1800 "Parser.cpp"
    break;

  case 99: // access_type: DUMMY_NODE_VECTOR8002
#line 597 "Parser.yy"
                                 { yylhs.value.as < uint16_t > () = 0x8002; }
#line 1806 "Parser.cpp"
    break;

  case 100: // access_type: DUMMY_NODE_VECTOR8003
#line 598 "Parser.yy"
                                 { yylhs.value.as < uint16_t > () = 0x8003; }
#line 1812 "Parser.cpp"
    break;

  case 101: // access_nodes: access_node
#line 601 "Parser.yy"
                      {
              yylhs.value.as < std::set<std::string> > () = std::set<std::string>();
              if (!yystack_[0].value.as < std::string > ().empty()) {
                  yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ());
              }
          }
#line 1823 "Parser.cpp"
    break;

  case 102: // access_nodes: access_nodes COMMA access_node
#line 607 "Parser.yy"
                                         {
              yylhs.value.as < std::set<std::string> > () = yystack_[2].value.as < std::set<std::string> > ();
              if (!yystack_[0].value.as < std::string > ().empty()) {
                  yylhs.value.as < std::set<std::string> > ().insert(yystack_[0].value.as < std::string > ());
              }
          }
#line 1834 "Parser.cpp"
    break;

  case 103: // access_node: node_name
#line 615 "Parser.yy"
                    { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1840 "Parser.cpp"
    break;

  case 104: // access_node: VECTOR_XXX
#line 616 "Parser.yy"
                     { yylhs.value.as < std::string > () = ""; }
#line 1846 "Parser.cpp"
    break;

  case 107: // environment_variable_data: ENVVAR_DATA env_var_name COLON data_size semicolon_with_optional_eol
#line 624 "Parser.yy"
                                                                               {
              network->environmentVariables[yystack_[3].value.as < std::string > ()].type = EnvironmentVariable::Type::Data;
              network->environmentVariables[yystack_[3].value.as < std::string > ()].dataSize = yystack_[1].value.as < uint32_t > ();
          }
#line 1855 "Parser.cpp"
    break;

  case 108: // data_size: unsigned_integer
#line 631 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1861 "Parser.cpp"
    break;

  case 109: // value_descriptions_for_env_var: VAL env_var_name value_encoding_descriptions semicolon_with_optional_eol
#line 637 "Parser.yy"
                                                                                   {
              network->environmentVariables[yystack_[2].value.as < std::string > ()].valueDescriptions = yystack_[1].value.as < std::map<int64_t, std::string> > ();
          }
#line 1869 "Parser.cpp"
    break;

  case 112: // signal_type: SGTYPE signal_type_name COLON signal_size VERTICAL_BAR byte_order value_type OPEN_PARENTHESIS factor COMMA offset CLOSE_PARENTHESIS OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit default_value COMMA value_table_name semicolon_with_optional_eol
#line 652 "Parser.yy"
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
#line 1888 "Parser.cpp"
    break;

  case 113: // signal_type_name: dbc_identifier
#line 668 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1894 "Parser.cpp"
    break;

  case 114: // default_value: double
#line 671 "Parser.yy"
                 { yylhs.value.as < double > () = yystack_[0].value.as < double > (); }
#line 1900 "Parser.cpp"
    break;

  case 117: // signal_group: SIG_GROUP message_id signal_group_name repetitions COLON signal_names semicolon_with_optional_eol
#line 689 "Parser.yy"
                                                                                                            {
              SignalGroup & signalGroup = network->messages[yystack_[5].value.as < uint32_t > ()].signalGroups[yystack_[4].value.as < std::string > ()];
              signalGroup.messageId = yystack_[5].value.as < uint32_t > ();
              signalGroup.name = yystack_[4].value.as < std::string > ();
              signalGroup.repetitions = yystack_[3].value.as < uint32_t > ();
              signalGroup.signals = yystack_[1].value.as < std::set<std::string> > ();
          }
#line 1912 "Parser.cpp"
    break;

  case 118: // signal_group_name: dbc_identifier
#line 698 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1918 "Parser.cpp"
    break;

  case 119: // repetitions: unsigned_integer
#line 701 "Parser.yy"
                           { yylhs.value.as < uint32_t > () = yystack_[0].value.as < uint32_t > (); }
#line 1924 "Parser.cpp"
    break;

  case 122: // comment: CM char_string semicolon_with_optional_eol
#line 711 "Parser.yy"
                                                     { network->comment = yystack_[1].value.as < std::string > (); }
#line 1930 "Parser.cpp"
    break;

  case 123: // comment: CM BU node_name char_string semicolon_with_optional_eol
#line 712 "Parser.yy"
                                                                  { network->nodes[yystack_[2].value.as < std::string > ()].comment = yystack_[1].value.as < std::string > (); }
#line 1936 "Parser.cpp"
    break;

  case 124: // comment: CM BO message_id char_string semicolon_with_optional_eol
#line 713 "Parser.yy"
                                                                   { network->messages[yystack_[2].value.as < uint32_t > ()].comment = yystack_[1].value.as < std::string > (); }
#line 1942 "Parser.cpp"
    break;

  case 125: // comment: CM SG message_id signal_name char_string semicolon_with_optional_eol
#line 714 "Parser.yy"
                                                                               { network->messages[yystack_[3].value.as < uint32_t > ()].signals[yystack_[2].value.as < std::string > ()].comment = yystack_[1].value.as < std::string > (); }
#line 1948 "Parser.cpp"
    break;

  case 126: // comment: CM EV env_var_name char_string semicolon_with_optional_eol
#line 715 "Parser.yy"
                                                                     { network->environmentVariables[yystack_[2].value.as < std::string > ()].comment = yystack_[1].value.as < std::string > (); }
#line 1954 "Parser.cpp"
    break;

  case 129: // attribute_definition: BA_DEF object_type attribute_name attribute_value_type semicolon_with_optional_eol
#line 727 "Parser.yy"
                                                                                             {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[2].value.as < std::string > ()];
              attributeDefinition.name = yystack_[2].value.as < std::string > ();
              attributeDefinition.objectType = yystack_[3].value.as < AttributeObjectType > ();
              attributeDefinition.valueType = yystack_[1].value.as < AttributeValueType > ();
          }
#line 1965 "Parser.cpp"
    break;

  case 130: // attribute_definition: BA_DEF_REL object_type attribute_name attribute_value_type semicolon_with_optional_eol
#line 733 "Parser.yy"
                                                                                                 {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[yystack_[2].value.as < std::string > ()];
              attributeDefinition.name = yystack_[2].value.as < std::string > ();
              attributeDefinition.objectType = yystack_[3].value.as < AttributeObjectType > ();
              attributeDefinition.valueType = yystack_[1].value.as < AttributeValueType > ();
          }
#line 1976 "Parser.cpp"
    break;

  case 131: // object_type: %empty
#line 741 "Parser.yy"
                 { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::Network; }
#line 1982 "Parser.cpp"
    break;

  case 132: // object_type: BU
#line 742 "Parser.yy"
             { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::Node; }
#line 1988 "Parser.cpp"
    break;

  case 133: // object_type: BO
#line 743 "Parser.yy"
             { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::Message; }
#line 1994 "Parser.cpp"
    break;

  case 134: // object_type: SG
#line 744 "Parser.yy"
             { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::Signal; }
#line 2000 "Parser.cpp"
    break;

  case 135: // object_type: EV
#line 745 "Parser.yy"
             { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::EnvironmentVariable; }
#line 2006 "Parser.cpp"
    break;

  case 136: // object_type: BU_EV_REL
#line 746 "Parser.yy"
                    { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::ControlUnitEnvironmentVariable; }
#line 2012 "Parser.cpp"
    break;

  case 137: // object_type: BU_BO_REL
#line 747 "Parser.yy"
                    { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::NodeTxMessage; }
#line 2018 "Parser.cpp"
    break;

  case 138: // object_type: BU_SG_REL
#line 748 "Parser.yy"
                    { yylhs.value.as < AttributeObjectType > () = AttributeObjectType::NodeMappedRxSignal; }
#line 2024 "Parser.cpp"
    break;

  case 139: // attribute_name: char_string
#line 751 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2030 "Parser.cpp"
    break;

  case 140: // attribute_value_type: INT signed_64_integer signed_64_integer
#line 755 "Parser.yy"
                                                  {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::Int;
              yylhs.value.as < AttributeValueType > ().integerValue.minimum = yystack_[1].value.as < int64_t > ();
              yylhs.value.as < AttributeValueType > ().integerValue.maximum = yystack_[0].value.as < int64_t > ();
          }
#line 2041 "Parser.cpp"
    break;

  case 141: // attribute_value_type: HEX signed_64_integer signed_64_integer
#line 761 "Parser.yy"
                                                  {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::Hex;
              yylhs.value.as < AttributeValueType > ().hexValue.minimum = yystack_[1].value.as < int64_t > ();
              yylhs.value.as < AttributeValueType > ().hexValue.maximum = yystack_[0].value.as < int64_t > ();
          }
#line 2052 "Parser.cpp"
    break;

  case 142: // attribute_value_type: FLOAT double double
#line 767 "Parser.yy"
                              {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::Float;
              yylhs.value.as < AttributeValueType > ().floatValue.minimum = yystack_[1].value.as < double > ();
              yylhs.value.as < AttributeValueType > ().floatValue.maximum = yystack_[0].value.as < double > ();
          }
#line 2063 "Parser.cpp"
    break;

  case 143: // attribute_value_type: STRING
#line 773 "Parser.yy"
                 {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::String;
          }
#line 2072 "Parser.cpp"
    break;

  case 144: // attribute_value_type: ENUM char_strings
#line 777 "Parser.yy"
                            {
              yylhs.value.as < AttributeValueType > () = AttributeValueType();
              yylhs.value.as < AttributeValueType > ().type = AttributeValueType::Type::Enum;
              yylhs.value.as < AttributeValueType > ().enumValues = yystack_[0].value.as < std::vector<std::string> > ();
          }
#line 2082 "Parser.cpp"
    break;

  case 147: // attribute_default: BA_DEF_DEF attribute_name attribute_value semicolon_with_optional_eol
#line 791 "Parser.yy"
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
#line 2112 "Parser.cpp"
    break;

  case 148: // attribute_default: BA_DEF_DEF_REL attribute_name attribute_value semicolon_with_optional_eol
#line 816 "Parser.yy"
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
#line 2142 "Parser.cpp"
    break;

  case 149: // attribute_value: UNSIGNED_INTEGER
#line 843 "Parser.yy"
                           { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2148 "Parser.cpp"
    break;

  case 150: // attribute_value: SIGNED_INTEGER
#line 844 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2154 "Parser.cpp"
    break;

  case 151: // attribute_value: DOUBLE
#line 845 "Parser.yy"
                 { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2160 "Parser.cpp"
    break;

  case 152: // attribute_value: CHAR_STRING
#line 846 "Parser.yy"
                      { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2166 "Parser.cpp"
    break;

  case 155: // attribute_value_for_object: BA attribute_name attribute_value semicolon_with_optional_eol
#line 856 "Parser.yy"
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
#line 2197 "Parser.cpp"
    break;

  case 156: // attribute_value_for_object: BA attribute_name BU node_name attribute_value semicolon_with_optional_eol
#line 882 "Parser.yy"
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
#line 2228 "Parser.cpp"
    break;

  case 157: // attribute_value_for_object: BA attribute_name BO message_id attribute_value semicolon_with_optional_eol
#line 908 "Parser.yy"
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
#line 2259 "Parser.cpp"
    break;

  case 158: // attribute_value_for_object: BA attribute_name SG message_id signal_name attribute_value semicolon_with_optional_eol
#line 934 "Parser.yy"
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
#line 2290 "Parser.cpp"
    break;

  case 159: // attribute_value_for_object: BA attribute_name EV env_var_name attribute_value semicolon_with_optional_eol
#line 960 "Parser.yy"
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
#line 2321 "Parser.cpp"
    break;

  case 160: // attribute_value_for_object: BA_REL attribute_name BU_EV_REL node_name env_var_name attribute_value semicolon_with_optional_eol
#line 986 "Parser.yy"
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
#line 2354 "Parser.cpp"
    break;

  case 161: // attribute_value_for_object: BA_REL attribute_name BU_BO_REL node_name message_id attribute_value semicolon_with_optional_eol
#line 1014 "Parser.yy"
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
#line 2387 "Parser.cpp"
    break;

  case 162: // attribute_value_for_object: BA_REL attribute_name BU_SG_REL node_name SG message_id signal_name attribute_value semicolon_with_optional_eol
#line 1042 "Parser.yy"
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
#line 2421 "Parser.cpp"
    break;

  case 165: // multiplexed_signal: SG_MUL_VAL message_id multiplexed_signal_name multiplexor_switch_name multiplexor_value_ranges semicolon_with_optional_eol
#line 1080 "Parser.yy"
                                                                                                                                     {
              ExtendedMultiplexor & extendedMultiplexor = network->messages[yystack_[4].value.as < uint32_t > ()].signals[yystack_[3].value.as < std::string > ()].extendedMultiplexors[yystack_[2].value.as < std::string > ()];
              extendedMultiplexor.switchName = yystack_[2].value.as < std::string > ();
              extendedMultiplexor.valueRanges = yystack_[1].value.as < std::set<ExtendedMultiplexor::ValueRange> > ();
          }
#line 2431 "Parser.cpp"
    break;

  case 166: // multiplexed_signal_name: dbc_identifier
#line 1087 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2437 "Parser.cpp"
    break;

  case 167: // multiplexor_switch_name: dbc_identifier
#line 1090 "Parser.yy"
                         { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2443 "Parser.cpp"
    break;

  case 168: // multiplexor_value_ranges: %empty
#line 1094 "Parser.yy"
                 { yylhs.value.as < std::set<ExtendedMultiplexor::ValueRange> > () = std::set<ExtendedMultiplexor::ValueRange>(); }
#line 2449 "Parser.cpp"
    break;

  case 169: // multiplexor_value_ranges: multiplexor_value_ranges multiplexor_value_range_comma
#line 1095 "Parser.yy"
                                                                 { yylhs.value.as < std::set<ExtendedMultiplexor::ValueRange> > () = yystack_[1].value.as < std::set<ExtendedMultiplexor::ValueRange> > (); yylhs.value.as < std::set<ExtendedMultiplexor::ValueRange> > ().insert(yystack_[0].value.as < ExtendedMultiplexor::ValueRange > ()); }
#line 2455 "Parser.cpp"
    break;

  case 170: // multiplexor_value_range_comma: multiplexor_value_range COMMA
#line 1099 "Parser.yy"
                                        { yylhs.value.as < ExtendedMultiplexor::ValueRange > () = yystack_[1].value.as < ExtendedMultiplexor::ValueRange > (); }
#line 2461 "Parser.cpp"
    break;

  case 171: // multiplexor_value_range_comma: multiplexor_value_range
#line 1100 "Parser.yy"
                                  { yylhs.value.as < ExtendedMultiplexor::ValueRange > () = yystack_[0].value.as < ExtendedMultiplexor::ValueRange > (); }
#line 2467 "Parser.cpp"
    break;

  case 172: // multiplexor_value_range: MULTIPLEXOR_VALUE_RANGE
#line 1104 "Parser.yy"
                                  { 
        const std::string complete = yystack_[0].value.as < std::string > (); 
        const std::string token1_str = complete.substr(0, complete.find("-")); 
        const std::string token2_str = complete.substr(token1_str.size() + 1, complete.size() - (token1_str.size() + 1)); 
        const uint32_t token1_int = std::stoul(token1_str);
        const uint32_t token2_int = std::stoul(token2_str);
        yylhs.value.as < ExtendedMultiplexor::ValueRange > () = std::make_pair(token1_int, token2_int); 
        }
#line 2480 "Parser.cpp"
    break;


#line 2484 "Parser.cpp"

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


  const short Parser::yypact_ninf_ = -316;

  const signed char Parser::yytable_ninf_ = -1;

  const short
  Parser::yypact_[] =
  {
      18,    24,    41,    66,  -316,  -316,    29,  -316,    37,   109,
    -316,    61,    73,   122,  -316,     4,    91,  -316,   130,  -316,
    -316,  -316,   110,   158,   161,   113,   174,  -316,     0,   170,
    -316,   164,  -316,  -316,   120,  -316,  -316,  -316,  -316,  -316,
    -316,   174,  -316,   162,   174,     9,  -316,   170,   174,  -316,
     165,  -316,   131,  -316,  -316,  -316,   134,  -316,    -5,  -316,
    -316,   135,   138,   170,  -316,   168,  -316,  -316,   139,   194,
    -316,   174,    71,  -316,   147,   170,  -316,   176,  -316,  -316,
    -316,    71,  -316,  -316,  -316,    -6,   210,   157,   170,  -316,
     181,   155,    71,  -316,  -316,   171,   174,  -316,   166,    25,
    -316,    33,  -316,  -316,   111,  -316,   160,   174,   170,   174,
     174,   170,   160,     7,     7,  -316,    97,   208,  -316,  -316,
    -316,  -316,   169,  -316,  -316,   172,    24,    24,   170,    24,
    -316,  -316,  -316,  -316,  -316,  -316,  -316,  -316,    24,    24,
      24,    24,  -316,   118,   170,  -316,   111,   225,   160,   160,
    -316,    24,   160,  -316,    72,    72,   156,   156,    24,    24,
     209,  -316,    17,  -316,   178,  -316,   116,  -316,  -316,   160,
    -316,   133,   133,   111,  -316,   226,   160,   160,  -316,  -316,
    -316,  -316,   160,   160,    67,   112,    51,  -316,  -316,   198,
    -316,  -316,   179,    24,  -316,  -316,   183,  -316,   133,   133,
     111,  -316,   177,  -316,  -316,  -316,  -316,   170,   174,   174,
     170,   160,   170,   170,   170,   170,  -316,   174,   219,  -316,
     174,  -316,   111,   111,  -316,  -316,  -316,   234,   156,   156,
     170,   156,  -316,   170,   174,   227,  -316,     9,   170,   174,
    -316,   195,  -316,   189,  -316,   174,  -316,   187,  -316,   160,
     160,   156,   160,   156,   156,   174,     9,  -316,  -316,   174,
     170,   174,  -316,   174,  -316,   238,   111,  -316,  -316,   160,
    -316,   160,   160,   170,  -316,  -316,   192,   193,   170,   196,
    -316,  -316,  -316,  -316,  -316,  -316,  -316,  -316,    80,  -316,
     200,  -316,  -316,  -316,   156,  -316,   174,  -316,   170,   225,
    -316,  -316,    42,  -316,   204,   160,    -2,  -316,   160,  -316,
    -316,   116,    80,  -316,   111,  -316,  -316,  -316,  -316,     5,
     203,  -316,   202,  -316,  -316,  -316,   199,   111,   111,  -316,
     213,   205,   111,    24,   216,   111,   220,  -316,   214,   111,
     170,   221,   160,   111,  -316,   229,    24,    83,  -316,  -316,
      45,  -316,    83,  -316,  -316
  };

  const unsigned char
  Parser::yydefact_[] =
  {
       0,     0,     0,    20,    12,    19,     0,     1,     0,     0,
      18,     0,     0,     0,    22,     0,     0,    34,    21,     5,
      24,    26,     0,    30,    42,     0,     0,    31,     0,     0,
      35,    77,    23,    27,     0,    17,    29,    33,    32,    37,
      38,     0,    43,    86,     0,     0,    45,     0,     0,    78,
     105,    28,     0,     7,     8,     6,     3,    36,     0,    39,
      46,     0,     0,     0,    87,   110,    25,     4,    14,     0,
      40,     0,     0,    89,     0,     0,   106,   120,    13,    41,
      47,     0,    49,    48,    80,     0,     0,     0,     0,   111,
     127,     0,     0,    79,    90,     0,     0,   113,     0,     0,
     121,   145,    50,    81,     0,   108,     0,     0,     0,     0,
       0,     0,     0,   131,   131,   128,   153,    44,    11,    10,
       9,    66,     0,   107,    60,     0,     0,     0,     0,     0,
     122,   132,   133,   134,   135,   136,   137,   138,     0,     0,
       0,     0,   146,    82,     0,    51,     0,     0,     0,     0,
      53,     0,     0,   139,     0,     0,     0,     0,     0,     0,
     115,   154,    56,    67,     0,    61,     0,   123,   124,     0,
     126,     0,     0,     0,   143,     0,     0,     0,   149,   150,
     151,   152,     0,     0,     0,     0,     0,    83,    84,    73,
      57,    58,     0,     0,    62,    63,     0,   125,     0,     0,
       0,    15,   144,   129,   130,   147,   148,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    38,     0,   163,   116,
       0,    68,     0,     0,   140,   141,   142,     0,     0,     0,
       0,     0,   155,     0,     0,     0,    38,     0,     0,     0,
      74,     2,    59,     0,    91,     0,    64,     0,    16,     0,
       0,     0,     0,     0,     0,     0,     0,   109,   118,     0,
       0,     0,   164,     0,    92,     0,     0,   156,   157,     0,
     159,     0,     0,     0,    85,   119,     0,     0,     0,     0,
      93,    94,    95,    96,    97,    98,    99,   100,     0,    65,
       0,   158,   160,   161,     0,    54,     0,   166,     0,     0,
     104,   103,     0,   101,     0,     0,     0,    76,     0,   167,
     168,     0,     0,    88,     0,   162,   117,    55,    75,     0,
       0,   102,     0,   172,   165,   169,   171,     0,     0,   170,
       0,     0,     0,     0,     0,     0,     0,   114,     0,     0,
       0,     0,     0,     0,   112,     0,     0,     0,    72,    71,
       0,    69,     0,    52,    70
  };

  const short
  Parser::yypgoto_[] =
  {
    -316,  -316,   -29,   -10,  -153,  -129,    -1,   215,  -316,   -26,
    -316,  -316,  -316,  -316,  -316,  -316,  -316,  -316,  -316,  -316,
     -27,  -316,  -316,   -58,  -193,  -316,  -316,  -316,   -44,  -316,
    -316,    -3,  -316,  -316,  -115,  -316,  -316,  -316,    20,   -12,
     -23,   -38,   -41,  -304,  -315,  -307,  -316,   -60,  -316,  -316,
    -316,  -316,  -316,  -316,  -316,  -316,  -316,  -316,   -64,  -316,
    -316,  -316,  -316,  -316,   -19,  -316,  -316,  -316,  -316,  -316,
    -316,  -316,  -316,  -316,  -316,  -316,  -316,  -316,  -316,  -316,
    -316,   180,    -7,   140,  -316,  -316,  -124,  -316,  -316,  -316,
    -316,  -316,  -316,  -316,  -316,  -316
  };

  const short
  Parser::yydefgoto_[] =
  {
       0,     2,    57,    46,    58,   121,   153,    70,   202,    37,
       3,     6,     9,    18,    13,    22,    34,    52,    17,    28,
      83,    24,    30,    40,    45,    59,    31,    42,    47,    61,
      81,    84,   117,   145,   151,   306,   192,   243,   125,   166,
     196,   247,   290,   122,   164,   222,   350,   351,   218,   240,
     308,    43,    49,    85,   160,   187,    50,    64,    74,    95,
     245,   265,   288,   302,   303,    65,    76,   106,   188,    77,
      89,    98,   338,   189,   219,   259,   276,    90,   100,   101,
     115,   138,   154,   176,   116,   142,   182,   143,   161,   241,
     262,   298,   310,   319,   325,   326
  };

  const short
  Parser::yytable_[] =
  {
       5,    38,    68,    39,    62,    21,    35,    19,    35,   323,
     322,    87,    53,   331,    54,    55,    33,   163,   198,   199,
     131,    60,   132,   237,   133,   190,   335,     1,   345,   162,
     134,     4,     4,   183,    51,   341,   191,    73,   108,   347,
     109,     7,   110,   256,   200,   224,   225,   129,   111,    73,
     135,   136,   137,    56,    19,    92,    93,    56,    69,    35,
     211,    80,    97,    36,    56,   127,   128,    20,    56,   113,
     178,   226,   179,   180,   181,   114,     8,   123,    91,    35,
     207,   126,   208,   130,   209,    73,   105,    82,    35,   103,
     210,    35,    10,   244,   246,    11,   300,   124,   112,   348,
     236,    56,   150,   312,   249,   250,   352,   252,   353,   171,
     172,   173,   174,   175,   118,   251,   119,   120,   150,   167,
     168,    12,   216,   170,    14,   148,   149,   269,   152,   271,
     272,    15,   155,   156,   157,    16,    53,   289,    54,    55,
     197,    25,   215,   140,   141,   277,   231,   203,   204,    23,
     169,   184,   185,   205,   206,   212,   213,   214,   294,   178,
      73,   179,   180,   181,   229,   230,   158,   159,    26,   253,
     305,   194,   195,   238,    27,    29,    32,    19,    35,    41,
     228,    44,   232,    48,    73,   233,   234,   235,    63,   150,
     254,   317,   221,    71,    66,   260,    72,    67,   246,   163,
      75,    68,    78,   289,   150,    86,   337,    73,   257,    88,
     242,   273,   258,    94,   163,    96,    99,   278,   102,    56,
     267,   268,   104,   270,   107,   144,   146,   274,   165,   147,
     193,   186,   217,   201,   150,   264,   223,   220,   227,   239,
     291,   248,   292,   293,   255,   261,   263,   150,   266,   275,
     295,   296,   297,   124,   304,   314,   327,   333,   299,   328,
     329,   301,   280,   281,   282,   283,   284,   285,   286,   287,
     336,   339,   309,   313,   332,   340,   315,   316,   343,   318,
     150,   346,   342,   279,    79,   301,   307,   311,   320,   330,
     324,   334,   354,   321,   139,   177,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   344,    39,     0,     0,     0,     0,     0,
     349,     0,     0,     0,     0,   349,     0,     0,     0,     0,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   221
  };

  const short
  Parser::yycheck_[] =
  {
       1,    28,     7,    29,    48,    15,     8,     3,     8,     4,
     314,    75,     3,   328,     5,     6,    26,   146,   171,   172,
      13,    47,    15,   216,    17,     8,   333,     9,   343,   144,
      23,     7,     7,   157,    44,   339,    19,    63,    13,   346,
      15,     0,    17,   236,   173,   198,   199,   111,    23,    75,
      43,    44,    45,    59,     3,    61,    85,    59,    63,     8,
     184,    71,    88,    63,    59,   109,   110,    63,    59,    36,
       3,   200,     5,     6,     7,    42,    10,   106,    81,     8,
      13,   108,    15,   112,    17,   111,    96,    16,     8,    92,
      23,     8,    63,   222,   223,    58,    16,   107,    99,    16,
     215,    59,   128,    61,   228,   229,    61,   231,    63,    37,
      38,    39,    40,    41,     3,   230,     5,     6,   144,   148,
     149,    12,   186,   152,    63,   126,   127,   251,   129,   253,
     254,    58,   139,   140,   141,    13,     3,   266,     5,     6,
     169,    11,   186,    46,    47,   260,   210,   176,   177,    58,
     151,   158,   159,   182,   183,    43,    44,    45,   273,     3,
     186,     5,     6,     7,   208,   209,    48,    49,    58,   233,
     294,    55,    56,   217,    16,    14,    63,     3,     8,    15,
     207,    61,   211,    21,   210,   212,   213,   214,    23,   215,
     234,   306,   193,    58,    63,   239,    58,    63,   327,   328,
      32,     7,    63,   332,   230,    58,   335,   233,   237,    33,
     220,   255,   238,     3,   343,    58,    35,   261,    63,    59,
     249,   250,    51,   252,    58,    17,    57,   256,     3,    57,
      52,    22,    34,     7,   260,   245,    53,    58,    61,    20,
     269,     7,   271,   272,    17,    50,    57,   273,    61,   259,
      58,    58,   278,   263,    54,    51,    53,    52,    62,    57,
      61,   288,    24,    25,    26,    27,    28,    29,    30,    31,
      54,    51,   298,   302,    61,    61,   305,   306,    57,   308,
     306,    52,   340,   263,    69,   312,   296,   299,   311,   327,
     319,   332,   352,   312,   114,   155,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   342,   340,    -1,    -1,    -1,    -1,    -1,
     347,    -1,    -1,    -1,    -1,   352,    -1,    -1,    -1,    -1,
      -1,    -1,   333,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   346
  };

  const unsigned char
  Parser::yystos_[] =
  {
       0,     9,    65,    74,     7,    70,    75,     0,    10,    76,
      63,    58,    12,    78,    63,    58,    13,    82,    77,     3,
      63,    67,    79,    58,    85,    11,    58,    16,    83,    14,
      86,    90,    63,    67,    80,     8,    63,    73,    84,    73,
      87,    15,    91,   115,    61,    88,    67,    92,    21,   116,
     120,    67,    81,     3,     5,     6,    59,    66,    68,    89,
      73,    93,    92,    23,   121,   129,    63,    63,     7,    63,
      71,    58,    58,    73,   122,    32,   130,   133,    63,    71,
      67,    94,    16,    84,    95,   117,    58,   122,    33,   134,
     141,    95,    61,    66,     3,   123,    58,    73,   135,    35,
     142,   143,    63,    95,    51,    67,   131,    58,    13,    15,
      17,    23,    70,    36,    42,   144,   148,    96,     3,     5,
       6,    69,   107,    66,    67,   102,    84,    92,    92,   122,
      66,    13,    15,    17,    23,    43,    44,    45,   145,   145,
      46,    47,   149,   151,    17,    97,    57,    57,    70,    70,
      73,    98,    70,    70,   146,   146,   146,   146,    48,    49,
     118,   152,    98,    69,   108,     3,   103,    66,    66,    70,
      66,    37,    38,    39,    40,    41,   147,   147,     3,     5,
       6,     7,   150,   150,   146,   146,    22,   119,   132,   137,
       8,    19,   100,    52,    55,    56,   104,    66,    68,    68,
      69,     7,    72,    66,    66,    66,    66,    13,    15,    17,
      23,   150,    43,    44,    45,    92,   122,    34,   112,   138,
      58,    70,   109,    53,    68,    68,    69,    61,    84,    92,
      92,   122,    66,    84,    84,    84,    98,    88,    92,    20,
     113,   153,    67,   101,    69,   124,    69,   105,     7,   150,
     150,    98,   150,   122,    92,    17,    88,    66,    73,   139,
      92,    50,   154,    57,    67,   125,    61,    66,    66,   150,
      66,   150,   150,    92,    66,    67,   140,    98,    92,   102,
      24,    25,    26,    27,    28,    29,    30,    31,   126,    69,
     106,    66,    66,    66,    98,    58,    58,    73,   155,    62,
      16,    84,   127,   128,    54,   150,    99,    67,   114,    73,
     156,   103,    61,    66,    51,    66,    66,    98,    66,   157,
     104,   128,   107,     4,    66,   158,   159,    53,    57,    61,
     105,   108,    61,    52,   106,   109,    54,    69,   136,    51,
      61,   107,    87,    57,    66,   108,    52,   109,    16,    84,
     110,   111,    61,    63,   111
  };

  const unsigned char
  Parser::yyr1_[] =
  {
       0,    64,    65,    66,    66,    67,    68,    68,    68,    69,
      69,    69,    70,    71,    71,    72,    72,    73,    74,    75,
      76,    76,    77,    77,    78,    78,    79,    80,    81,    82,
      83,    83,    83,    84,    85,    85,    86,    87,    88,    88,
      89,    89,    90,    90,    91,    92,    93,    94,    95,    95,
      96,    96,    97,    98,    99,    99,   100,   100,   100,   101,
     102,   103,   104,   104,   105,   106,   107,   108,   109,   110,
     110,   111,   111,   112,   112,   113,   114,   115,   115,   116,
     117,   117,   118,   118,   118,   119,   120,   120,   121,   122,
     123,   124,   125,   126,   126,   126,   126,   126,   126,   126,
     126,   127,   127,   128,   128,   129,   129,   130,   131,   132,
     133,   133,   134,   135,   136,   137,   137,   138,   139,   140,
     141,   141,   142,   142,   142,   142,   142,   143,   143,   144,
     144,   145,   145,   145,   145,   145,   145,   145,   145,   146,
     147,   147,   147,   147,   147,   148,   148,   149,   149,   150,
     150,   150,   150,   151,   151,   152,   152,   152,   152,   152,
     152,   152,   152,   153,   153,   154,   155,   156,   157,   157,
     158,   158,   159
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,    18,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     3,     1,     3,     1,
       0,     4,     0,     3,     3,     8,     1,     1,     1,     4,
       0,     1,     2,     1,     0,     2,     4,     1,     0,     2,
       2,     3,     0,     2,     8,     1,     1,     1,     1,     1,
       0,     2,    23,     1,     0,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     0,     2,     6,     1,     0,     2,     5,
       1,     3,     0,     2,     2,     5,     0,     2,    15,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     0,     2,     5,     1,     4,
       0,     2,    22,     1,     1,     0,     2,     7,     1,     1,
       0,     2,     3,     5,     5,     6,     5,     0,     2,     5,
       5,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     1,     2,     0,     2,     4,     4,     1,
       1,     1,     1,     0,     2,     4,     6,     6,     7,     6,
       7,     7,     9,     0,     2,     6,     1,     1,     0,     2,
       2,     1,     1
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
       0,   219,   219,   248,   249,   252,   257,   259,   260,   263,
     264,   265,   268,   272,   273,   276,   277,   280,   285,   288,
     291,   292,   296,   297,   302,   303,   310,   313,   316,   321,
     324,   325,   326,   329,   334,   335,   339,   346,   352,   353,
     358,   359,   364,   365,   368,   378,   381,   384,   387,   388,
     395,   396,   399,   422,   425,   426,   429,   430,   431,   437,
     440,   443,   449,   450,   453,   456,   459,   462,   465,   468,
     474,   482,   483,   486,   487,   491,   496,   513,   514,   518,
     521,   522,   527,   528,   529,   533,   540,   541,   545,   575,
     578,   585,   588,   591,   592,   593,   594,   595,   596,   597,
     598,   601,   607,   615,   616,   619,   620,   624,   631,   637,
     644,   645,   649,   668,   671,   684,   685,   689,   698,   701,
     706,   707,   711,   712,   713,   714,   715,   722,   723,   727,
     733,   741,   742,   743,   744,   745,   746,   747,   748,   751,
     755,   761,   767,   773,   777,   786,   787,   791,   816,   843,
     844,   845,   846,   851,   852,   856,   882,   908,   934,   960,
     986,  1014,  1042,  1075,  1076,  1080,  1087,  1090,  1094,  1095,
    1099,  1100,  1104
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
#line 3225 "Parser.cpp"

#line 1114 "Parser.yy"


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
