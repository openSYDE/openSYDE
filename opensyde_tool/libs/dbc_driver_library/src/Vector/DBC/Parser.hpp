// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file Parser.hpp
 ** Define the Vector::DBC::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 18 "Parser.yy"

#include <Vector/DBC/Network.h>
namespace Vector {
namespace DBC {
class Network;
}
}

#line 58 "Parser.hpp"

# include <cassert>
# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "location.hh"
#include <typeinfo>
#ifndef YY_ASSERT
# include <cassert>
# define YY_ASSERT assert
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

#line 4 "Parser.yy"
namespace Vector { namespace DBC {
#line 199 "Parser.hpp"




  /// A Bison parser.
  class Parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
      , yytypeid_ (YY_NULLPTR)
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
      : yytypeid_ (&typeid (T))
    {
      YY_ASSERT (sizeof (T) <= size);
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {
      YY_ASSERT (!yytypeid_);
    }

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == *that.yytypeid_);
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
      yytypeid_ = YY_NULLPTR;
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // object_type
      char dummy1[sizeof (AttributeObjectType)];

      // attribute_value_type
      char dummy2[sizeof (AttributeValueType)];

      // byte_order
      char dummy3[sizeof (ByteOrder)];

      // env_var_type
      char dummy4[sizeof (EnvironmentVariable::Type)];

      // multiplexor_value_range_comma
      // multiplexor_value_range
      char dummy5[sizeof (ExtendedMultiplexor::ValueRange)];

      // message
      char dummy6[sizeof (Message)];

      // signal
      char dummy7[sizeof (Signal)];

      // signal_extended_value_type_type
      char dummy8[sizeof (Signal::ExtendedValueType)];

      // value_table
      char dummy9[sizeof (ValueTable)];

      // value_type
      char dummy10[sizeof (ValueType)];

      // double
      // factor
      // offset
      // minimum
      // maximum
      // initial_value
      // default_value
      char dummy11[sizeof (double)];

      // signed_64_integer
      char dummy12[sizeof (int64_t)];

      // value_encoding_descriptions
      char dummy13[sizeof (std::map<int64_t, std::string>)];

      // signals
      char dummy14[sizeof (std::map<std::string, Signal>)];

      // value_encoding_description
      char dummy15[sizeof (std::pair<int64_t, std::string>)];

      // multiplexor_value_ranges
      char dummy16[sizeof (std::set<ExtendedMultiplexor::ValueRange>)];

      // signal_names
      // receivers
      // transmitters
      // access_nodes
      char dummy17[sizeof (std::set<std::string>)];

      // UNSIGNED_INTEGER
      // MULTIPLEXOR_VALUE_RANGE
      // SIGNED_INTEGER
      // DOUBLE
      // CHAR_STRING
      // DBC_IDENTIFIER
      // NS_VALUE
      // semicolon_with_optional_eol
      // char_string
      // char_string_eol
      // dbc_identifier
      // candb_version_string
      // node_name
      // value_table_name
      // message_name
      // transmitter
      // signal_name
      // multiplexer_indicator
      // unit
      // receiver
      // env_var_name
      // access_node
      // signal_type_name
      // signal_group_name
      // attribute_name
      // attribute_value
      // multiplexed_signal_name
      // multiplexor_switch_name
      char dummy18[sizeof (std::string)];

      // char_strings
      // new_symbol_values
      char dummy19[sizeof (std::vector<std::string>)];

      // access_type
      char dummy20[sizeof (uint16_t)];

      // unsigned_integer
      // baudrate
      // btr1
      // btr2
      // message_id
      // message_size
      // start_bit
      // signal_size
      // ev_id
      // data_size
      // repetitions
      char dummy21[sizeof (uint32_t)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };

    /// Whether the content is built: if defined, the name of the stored type.
    const std::type_info *yytypeid_;
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        YYEMPTY = -2,
    END = 0,                       // END
    YYerror = 256,                 // error
    YYUNDEF = 257,                 // "invalid token"
    UNSIGNED_INTEGER = 258,        // UNSIGNED_INTEGER
    MULTIPLEXOR_VALUE_RANGE = 259, // MULTIPLEXOR_VALUE_RANGE
    SIGNED_INTEGER = 260,          // SIGNED_INTEGER
    DOUBLE = 261,                  // DOUBLE
    CHAR_STRING = 262,             // CHAR_STRING
    DBC_IDENTIFIER = 263,          // DBC_IDENTIFIER
    VERSION = 264,                 // VERSION
    NS = 265,                      // NS
    NS_VALUE = 266,                // NS_VALUE
    BS = 267,                      // BS
    BU = 268,                      // BU
    VAL_TABLE = 269,               // VAL_TABLE
    BO = 270,                      // BO
    VECTOR_XXX = 271,              // VECTOR_XXX
    SG = 272,                      // SG
    LOWER_M = 273,                 // LOWER_M
    UPPER_M = 274,                 // UPPER_M
    SIG_VALTYPE = 275,             // SIG_VALTYPE
    BO_TX_BU = 276,                // BO_TX_BU
    VAL = 277,                     // VAL
    EV = 278,                      // EV
    DUMMY_NODE_VECTOR0 = 279,      // DUMMY_NODE_VECTOR0
    DUMMY_NODE_VECTOR1 = 280,      // DUMMY_NODE_VECTOR1
    DUMMY_NODE_VECTOR2 = 281,      // DUMMY_NODE_VECTOR2
    DUMMY_NODE_VECTOR3 = 282,      // DUMMY_NODE_VECTOR3
    DUMMY_NODE_VECTOR8000 = 283,   // DUMMY_NODE_VECTOR8000
    DUMMY_NODE_VECTOR8001 = 284,   // DUMMY_NODE_VECTOR8001
    DUMMY_NODE_VECTOR8002 = 285,   // DUMMY_NODE_VECTOR8002
    DUMMY_NODE_VECTOR8003 = 286,   // DUMMY_NODE_VECTOR8003
    ENVVAR_DATA = 287,             // ENVVAR_DATA
    SGTYPE = 288,                  // SGTYPE
    SIG_GROUP = 289,               // SIG_GROUP
    CM = 290,                      // CM
    BA_DEF = 291,                  // BA_DEF
    INT = 292,                     // INT
    HEX = 293,                     // HEX
    FLOAT = 294,                   // FLOAT
    STRING = 295,                  // STRING
    ENUM = 296,                    // ENUM
    BA_DEF_REL = 297,              // BA_DEF_REL
    BU_EV_REL = 298,               // BU_EV_REL
    BU_BO_REL = 299,               // BU_BO_REL
    BU_SG_REL = 300,               // BU_SG_REL
    BA_DEF_DEF = 301,              // BA_DEF_DEF
    BA_DEF_DEF_REL = 302,          // BA_DEF_DEF_REL
    BA = 303,                      // BA
    BA_REL = 304,                  // BA_REL
    SG_MUL_VAL = 305,              // SG_MUL_VAL
    OPEN_BRACKET = 306,            // OPEN_BRACKET
    CLOSE_BRACKET = 307,           // CLOSE_BRACKET
    OPEN_PARENTHESIS = 308,        // OPEN_PARENTHESIS
    CLOSE_PARENTHESIS = 309,       // CLOSE_PARENTHESIS
    PLUS = 310,                    // PLUS
    MINUS = 311,                   // MINUS
    VERTICAL_BAR = 312,            // VERTICAL_BAR
    COLON = 313,                   // COLON
    SEMICOLON = 314,               // SEMICOLON
    ASSIGN = 315,                  // ASSIGN
    COMMA = 316,                   // COMMA
    AT = 317,                      // AT
    EOL = 318                      // EOL
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 64, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // END
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_UNSIGNED_INTEGER = 3,                  // UNSIGNED_INTEGER
        S_MULTIPLEXOR_VALUE_RANGE = 4,           // MULTIPLEXOR_VALUE_RANGE
        S_SIGNED_INTEGER = 5,                    // SIGNED_INTEGER
        S_DOUBLE = 6,                            // DOUBLE
        S_CHAR_STRING = 7,                       // CHAR_STRING
        S_DBC_IDENTIFIER = 8,                    // DBC_IDENTIFIER
        S_VERSION = 9,                           // VERSION
        S_NS = 10,                               // NS
        S_NS_VALUE = 11,                         // NS_VALUE
        S_BS = 12,                               // BS
        S_BU = 13,                               // BU
        S_VAL_TABLE = 14,                        // VAL_TABLE
        S_BO = 15,                               // BO
        S_VECTOR_XXX = 16,                       // VECTOR_XXX
        S_SG = 17,                               // SG
        S_LOWER_M = 18,                          // LOWER_M
        S_UPPER_M = 19,                          // UPPER_M
        S_SIG_VALTYPE = 20,                      // SIG_VALTYPE
        S_BO_TX_BU = 21,                         // BO_TX_BU
        S_VAL = 22,                              // VAL
        S_EV = 23,                               // EV
        S_DUMMY_NODE_VECTOR0 = 24,               // DUMMY_NODE_VECTOR0
        S_DUMMY_NODE_VECTOR1 = 25,               // DUMMY_NODE_VECTOR1
        S_DUMMY_NODE_VECTOR2 = 26,               // DUMMY_NODE_VECTOR2
        S_DUMMY_NODE_VECTOR3 = 27,               // DUMMY_NODE_VECTOR3
        S_DUMMY_NODE_VECTOR8000 = 28,            // DUMMY_NODE_VECTOR8000
        S_DUMMY_NODE_VECTOR8001 = 29,            // DUMMY_NODE_VECTOR8001
        S_DUMMY_NODE_VECTOR8002 = 30,            // DUMMY_NODE_VECTOR8002
        S_DUMMY_NODE_VECTOR8003 = 31,            // DUMMY_NODE_VECTOR8003
        S_ENVVAR_DATA = 32,                      // ENVVAR_DATA
        S_SGTYPE = 33,                           // SGTYPE
        S_SIG_GROUP = 34,                        // SIG_GROUP
        S_CM = 35,                               // CM
        S_BA_DEF = 36,                           // BA_DEF
        S_INT = 37,                              // INT
        S_HEX = 38,                              // HEX
        S_FLOAT = 39,                            // FLOAT
        S_STRING = 40,                           // STRING
        S_ENUM = 41,                             // ENUM
        S_BA_DEF_REL = 42,                       // BA_DEF_REL
        S_BU_EV_REL = 43,                        // BU_EV_REL
        S_BU_BO_REL = 44,                        // BU_BO_REL
        S_BU_SG_REL = 45,                        // BU_SG_REL
        S_BA_DEF_DEF = 46,                       // BA_DEF_DEF
        S_BA_DEF_DEF_REL = 47,                   // BA_DEF_DEF_REL
        S_BA = 48,                               // BA
        S_BA_REL = 49,                           // BA_REL
        S_SG_MUL_VAL = 50,                       // SG_MUL_VAL
        S_OPEN_BRACKET = 51,                     // OPEN_BRACKET
        S_CLOSE_BRACKET = 52,                    // CLOSE_BRACKET
        S_OPEN_PARENTHESIS = 53,                 // OPEN_PARENTHESIS
        S_CLOSE_PARENTHESIS = 54,                // CLOSE_PARENTHESIS
        S_PLUS = 55,                             // PLUS
        S_MINUS = 56,                            // MINUS
        S_VERTICAL_BAR = 57,                     // VERTICAL_BAR
        S_COLON = 58,                            // COLON
        S_SEMICOLON = 59,                        // SEMICOLON
        S_ASSIGN = 60,                           // ASSIGN
        S_COMMA = 61,                            // COMMA
        S_AT = 62,                               // AT
        S_EOL = 63,                              // EOL
        S_YYACCEPT = 64,                         // $accept
        S_network = 65,                          // network
        S_semicolon_with_optional_eol = 66,      // semicolon_with_optional_eol
        S_unsigned_integer = 67,                 // unsigned_integer
        S_signed_64_integer = 68,                // signed_64_integer
        S_double = 69,                           // double
        S_char_string = 70,                      // char_string
        S_char_string_eol = 71,                  // char_string_eol
        S_char_strings = 72,                     // char_strings
        S_dbc_identifier = 73,                   // dbc_identifier
        S_version = 74,                          // version
        S_candb_version_string = 75,             // candb_version_string
        S_new_symbols = 76,                      // new_symbols
        S_new_symbol_values = 77,                // new_symbol_values
        S_bit_timing = 78,                       // bit_timing
        S_baudrate = 79,                         // baudrate
        S_btr1 = 80,                             // btr1
        S_btr2 = 81,                             // btr2
        S_nodes = 82,                            // nodes
        S_node_names = 83,                       // node_names
        S_node_name = 84,                        // node_name
        S_value_tables = 85,                     // value_tables
        S_value_table = 86,                      // value_table
        S_value_table_name = 87,                 // value_table_name
        S_value_encoding_descriptions = 88,      // value_encoding_descriptions
        S_value_encoding_description = 89,       // value_encoding_description
        S_messages = 90,                         // messages
        S_message = 91,                          // message
        S_message_id = 92,                       // message_id
        S_message_name = 93,                     // message_name
        S_message_size = 94,                     // message_size
        S_transmitter = 95,                      // transmitter
        S_signals = 96,                          // signals
        S_signal = 97,                           // signal
        S_signal_name = 98,                      // signal_name
        S_signal_names = 99,                     // signal_names
        S_multiplexer_indicator = 100,           // multiplexer_indicator
        S_start_bit = 101,                       // start_bit
        S_signal_size = 102,                     // signal_size
        S_byte_order = 103,                      // byte_order
        S_value_type = 104,                      // value_type
        S_factor = 105,                          // factor
        S_offset = 106,                          // offset
        S_minimum = 107,                         // minimum
        S_maximum = 108,                         // maximum
        S_unit = 109,                            // unit
        S_receivers = 110,                       // receivers
        S_receiver = 111,                        // receiver
        S_signal_extended_value_types = 112,     // signal_extended_value_types
        S_signal_extended_value_type = 113,      // signal_extended_value_type
        S_signal_extended_value_type_type = 114, // signal_extended_value_type_type
        S_message_transmitters = 115,            // message_transmitters
        S_message_transmitter = 116,             // message_transmitter
        S_transmitters = 117,                    // transmitters
        S_value_descriptions = 118,              // value_descriptions
        S_value_descriptions_for_signal = 119,   // value_descriptions_for_signal
        S_environment_variables = 120,           // environment_variables
        S_environment_variable = 121,            // environment_variable
        S_env_var_name = 122,                    // env_var_name
        S_env_var_type = 123,                    // env_var_type
        S_initial_value = 124,                   // initial_value
        S_ev_id = 125,                           // ev_id
        S_access_type = 126,                     // access_type
        S_access_nodes = 127,                    // access_nodes
        S_access_node = 128,                     // access_node
        S_environment_variables_data = 129,      // environment_variables_data
        S_environment_variable_data = 130,       // environment_variable_data
        S_data_size = 131,                       // data_size
        S_value_descriptions_for_env_var = 132,  // value_descriptions_for_env_var
        S_signal_types = 133,                    // signal_types
        S_signal_type = 134,                     // signal_type
        S_signal_type_name = 135,                // signal_type_name
        S_default_value = 136,                   // default_value
        S_signal_groups = 137,                   // signal_groups
        S_signal_group = 138,                    // signal_group
        S_signal_group_name = 139,               // signal_group_name
        S_repetitions = 140,                     // repetitions
        S_comments = 141,                        // comments
        S_comment = 142,                         // comment
        S_attribute_definitions = 143,           // attribute_definitions
        S_attribute_definition = 144,            // attribute_definition
        S_object_type = 145,                     // object_type
        S_attribute_name = 146,                  // attribute_name
        S_attribute_value_type = 147,            // attribute_value_type
        S_attribute_defaults = 148,              // attribute_defaults
        S_attribute_default = 149,               // attribute_default
        S_attribute_value = 150,                 // attribute_value
        S_attribute_values = 151,                // attribute_values
        S_attribute_value_for_object = 152,      // attribute_value_for_object
        S_extended_multiplexing = 153,           // extended_multiplexing
        S_multiplexed_signal = 154,              // multiplexed_signal
        S_multiplexed_signal_name = 155,         // multiplexed_signal_name
        S_multiplexor_switch_name = 156,         // multiplexor_switch_name
        S_multiplexor_value_ranges = 157,        // multiplexor_value_ranges
        S_multiplexor_value_range_comma = 158,   // multiplexor_value_range_comma
        S_multiplexor_value_range = 159          // multiplexor_value_range
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_object_type: // object_type
        value.move< AttributeObjectType > (std::move (that.value));
        break;

      case symbol_kind::S_attribute_value_type: // attribute_value_type
        value.move< AttributeValueType > (std::move (that.value));
        break;

      case symbol_kind::S_byte_order: // byte_order
        value.move< ByteOrder > (std::move (that.value));
        break;

      case symbol_kind::S_env_var_type: // env_var_type
        value.move< EnvironmentVariable::Type > (std::move (that.value));
        break;

      case symbol_kind::S_multiplexor_value_range_comma: // multiplexor_value_range_comma
      case symbol_kind::S_multiplexor_value_range: // multiplexor_value_range
        value.move< ExtendedMultiplexor::ValueRange > (std::move (that.value));
        break;

      case symbol_kind::S_message: // message
        value.move< Message > (std::move (that.value));
        break;

      case symbol_kind::S_signal: // signal
        value.move< Signal > (std::move (that.value));
        break;

      case symbol_kind::S_signal_extended_value_type_type: // signal_extended_value_type_type
        value.move< Signal::ExtendedValueType > (std::move (that.value));
        break;

      case symbol_kind::S_value_table: // value_table
        value.move< ValueTable > (std::move (that.value));
        break;

      case symbol_kind::S_value_type: // value_type
        value.move< ValueType > (std::move (that.value));
        break;

      case symbol_kind::S_double: // double
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_offset: // offset
      case symbol_kind::S_minimum: // minimum
      case symbol_kind::S_maximum: // maximum
      case symbol_kind::S_initial_value: // initial_value
      case symbol_kind::S_default_value: // default_value
        value.move< double > (std::move (that.value));
        break;

      case symbol_kind::S_signed_64_integer: // signed_64_integer
        value.move< int64_t > (std::move (that.value));
        break;

      case symbol_kind::S_value_encoding_descriptions: // value_encoding_descriptions
        value.move< std::map<int64_t, std::string> > (std::move (that.value));
        break;

      case symbol_kind::S_signals: // signals
        value.move< std::map<std::string, Signal> > (std::move (that.value));
        break;

      case symbol_kind::S_value_encoding_description: // value_encoding_description
        value.move< std::pair<int64_t, std::string> > (std::move (that.value));
        break;

      case symbol_kind::S_multiplexor_value_ranges: // multiplexor_value_ranges
        value.move< std::set<ExtendedMultiplexor::ValueRange> > (std::move (that.value));
        break;

      case symbol_kind::S_signal_names: // signal_names
      case symbol_kind::S_receivers: // receivers
      case symbol_kind::S_transmitters: // transmitters
      case symbol_kind::S_access_nodes: // access_nodes
        value.move< std::set<std::string> > (std::move (that.value));
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
        value.move< std::string > (std::move (that.value));
        break;

      case symbol_kind::S_char_strings: // char_strings
      case symbol_kind::S_new_symbol_values: // new_symbol_values
        value.move< std::vector<std::string> > (std::move (that.value));
        break;

      case symbol_kind::S_access_type: // access_type
        value.move< uint16_t > (std::move (that.value));
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
        value.move< uint32_t > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AttributeObjectType&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AttributeObjectType& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AttributeValueType&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AttributeValueType& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ByteOrder&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ByteOrder& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, EnvironmentVariable::Type&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const EnvironmentVariable::Type& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ExtendedMultiplexor::ValueRange&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ExtendedMultiplexor::ValueRange& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Message&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Message& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Signal&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Signal& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Signal::ExtendedValueType&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Signal::ExtendedValueType& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ValueTable&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ValueTable& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ValueType&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ValueType& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, double&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const double& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, int64_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const int64_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::map<int64_t, std::string>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::map<int64_t, std::string>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::map<std::string, Signal>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::map<std::string, Signal>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::pair<int64_t, std::string>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::pair<int64_t, std::string>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::set<ExtendedMultiplexor::ValueRange>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::set<ExtendedMultiplexor::ValueRange>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::set<std::string>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::set<std::string>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<std::string>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<std::string>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, uint16_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const uint16_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, uint32_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const uint32_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_object_type: // object_type
        value.template destroy< AttributeObjectType > ();
        break;

      case symbol_kind::S_attribute_value_type: // attribute_value_type
        value.template destroy< AttributeValueType > ();
        break;

      case symbol_kind::S_byte_order: // byte_order
        value.template destroy< ByteOrder > ();
        break;

      case symbol_kind::S_env_var_type: // env_var_type
        value.template destroy< EnvironmentVariable::Type > ();
        break;

      case symbol_kind::S_multiplexor_value_range_comma: // multiplexor_value_range_comma
      case symbol_kind::S_multiplexor_value_range: // multiplexor_value_range
        value.template destroy< ExtendedMultiplexor::ValueRange > ();
        break;

      case symbol_kind::S_message: // message
        value.template destroy< Message > ();
        break;

      case symbol_kind::S_signal: // signal
        value.template destroy< Signal > ();
        break;

      case symbol_kind::S_signal_extended_value_type_type: // signal_extended_value_type_type
        value.template destroy< Signal::ExtendedValueType > ();
        break;

      case symbol_kind::S_value_table: // value_table
        value.template destroy< ValueTable > ();
        break;

      case symbol_kind::S_value_type: // value_type
        value.template destroy< ValueType > ();
        break;

      case symbol_kind::S_double: // double
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_offset: // offset
      case symbol_kind::S_minimum: // minimum
      case symbol_kind::S_maximum: // maximum
      case symbol_kind::S_initial_value: // initial_value
      case symbol_kind::S_default_value: // default_value
        value.template destroy< double > ();
        break;

      case symbol_kind::S_signed_64_integer: // signed_64_integer
        value.template destroy< int64_t > ();
        break;

      case symbol_kind::S_value_encoding_descriptions: // value_encoding_descriptions
        value.template destroy< std::map<int64_t, std::string> > ();
        break;

      case symbol_kind::S_signals: // signals
        value.template destroy< std::map<std::string, Signal> > ();
        break;

      case symbol_kind::S_value_encoding_description: // value_encoding_description
        value.template destroy< std::pair<int64_t, std::string> > ();
        break;

      case symbol_kind::S_multiplexor_value_ranges: // multiplexor_value_ranges
        value.template destroy< std::set<ExtendedMultiplexor::ValueRange> > ();
        break;

      case symbol_kind::S_signal_names: // signal_names
      case symbol_kind::S_receivers: // receivers
      case symbol_kind::S_transmitters: // transmitters
      case symbol_kind::S_access_nodes: // access_nodes
        value.template destroy< std::set<std::string> > ();
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
        value.template destroy< std::string > ();
        break;

      case symbol_kind::S_char_strings: // char_strings
      case symbol_kind::S_new_symbol_values: // new_symbol_values
        value.template destroy< std::vector<std::string> > ();
        break;

      case symbol_kind::S_access_type: // access_type
        value.template destroy< uint16_t > ();
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
        value.template destroy< uint32_t > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// The user-facing name of this symbol.
      std::string name () const YY_NOEXCEPT
      {
        return Parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::END
                   || (token::YYerror <= tok && tok <= token::YYUNDEF)
                   || (token::VERSION <= tok && tok <= token::NS)
                   || (token::BS <= tok && tok <= token::EOL));
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::string v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const std::string& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT ((token::UNSIGNED_INTEGER <= tok && tok <= token::DBC_IDENTIFIER)
                   || tok == token::NS_VALUE);
#endif
      }
    };

    /// Build a parser object.
    Parser (class Scanner * scanner_yyarg, class Network * network_yyarg);
    virtual ~Parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    Parser (const Parser&) = delete;
    /// Non copyable.
    Parser& operator= (const Parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static std::string symbol_name (symbol_kind_type yysymbol);

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_END (location_type l)
      {
        return symbol_type (token::END, std::move (l));
      }
#else
      static
      symbol_type
      make_END (const location_type& l)
      {
        return symbol_type (token::END, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UNSIGNED_INTEGER (std::string v, location_type l)
      {
        return symbol_type (token::UNSIGNED_INTEGER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_UNSIGNED_INTEGER (const std::string& v, const location_type& l)
      {
        return symbol_type (token::UNSIGNED_INTEGER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MULTIPLEXOR_VALUE_RANGE (std::string v, location_type l)
      {
        return symbol_type (token::MULTIPLEXOR_VALUE_RANGE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_MULTIPLEXOR_VALUE_RANGE (const std::string& v, const location_type& l)
      {
        return symbol_type (token::MULTIPLEXOR_VALUE_RANGE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SIGNED_INTEGER (std::string v, location_type l)
      {
        return symbol_type (token::SIGNED_INTEGER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_SIGNED_INTEGER (const std::string& v, const location_type& l)
      {
        return symbol_type (token::SIGNED_INTEGER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOUBLE (std::string v, location_type l)
      {
        return symbol_type (token::DOUBLE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_DOUBLE (const std::string& v, const location_type& l)
      {
        return symbol_type (token::DOUBLE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CHAR_STRING (std::string v, location_type l)
      {
        return symbol_type (token::CHAR_STRING, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CHAR_STRING (const std::string& v, const location_type& l)
      {
        return symbol_type (token::CHAR_STRING, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DBC_IDENTIFIER (std::string v, location_type l)
      {
        return symbol_type (token::DBC_IDENTIFIER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_DBC_IDENTIFIER (const std::string& v, const location_type& l)
      {
        return symbol_type (token::DBC_IDENTIFIER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VERSION (location_type l)
      {
        return symbol_type (token::VERSION, std::move (l));
      }
#else
      static
      symbol_type
      make_VERSION (const location_type& l)
      {
        return symbol_type (token::VERSION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NS (location_type l)
      {
        return symbol_type (token::NS, std::move (l));
      }
#else
      static
      symbol_type
      make_NS (const location_type& l)
      {
        return symbol_type (token::NS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NS_VALUE (std::string v, location_type l)
      {
        return symbol_type (token::NS_VALUE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_NS_VALUE (const std::string& v, const location_type& l)
      {
        return symbol_type (token::NS_VALUE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BS (location_type l)
      {
        return symbol_type (token::BS, std::move (l));
      }
#else
      static
      symbol_type
      make_BS (const location_type& l)
      {
        return symbol_type (token::BS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BU (location_type l)
      {
        return symbol_type (token::BU, std::move (l));
      }
#else
      static
      symbol_type
      make_BU (const location_type& l)
      {
        return symbol_type (token::BU, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VAL_TABLE (location_type l)
      {
        return symbol_type (token::VAL_TABLE, std::move (l));
      }
#else
      static
      symbol_type
      make_VAL_TABLE (const location_type& l)
      {
        return symbol_type (token::VAL_TABLE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BO (location_type l)
      {
        return symbol_type (token::BO, std::move (l));
      }
#else
      static
      symbol_type
      make_BO (const location_type& l)
      {
        return symbol_type (token::BO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VECTOR_XXX (location_type l)
      {
        return symbol_type (token::VECTOR_XXX, std::move (l));
      }
#else
      static
      symbol_type
      make_VECTOR_XXX (const location_type& l)
      {
        return symbol_type (token::VECTOR_XXX, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SG (location_type l)
      {
        return symbol_type (token::SG, std::move (l));
      }
#else
      static
      symbol_type
      make_SG (const location_type& l)
      {
        return symbol_type (token::SG, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOWER_M (location_type l)
      {
        return symbol_type (token::LOWER_M, std::move (l));
      }
#else
      static
      symbol_type
      make_LOWER_M (const location_type& l)
      {
        return symbol_type (token::LOWER_M, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UPPER_M (location_type l)
      {
        return symbol_type (token::UPPER_M, std::move (l));
      }
#else
      static
      symbol_type
      make_UPPER_M (const location_type& l)
      {
        return symbol_type (token::UPPER_M, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SIG_VALTYPE (location_type l)
      {
        return symbol_type (token::SIG_VALTYPE, std::move (l));
      }
#else
      static
      symbol_type
      make_SIG_VALTYPE (const location_type& l)
      {
        return symbol_type (token::SIG_VALTYPE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BO_TX_BU (location_type l)
      {
        return symbol_type (token::BO_TX_BU, std::move (l));
      }
#else
      static
      symbol_type
      make_BO_TX_BU (const location_type& l)
      {
        return symbol_type (token::BO_TX_BU, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VAL (location_type l)
      {
        return symbol_type (token::VAL, std::move (l));
      }
#else
      static
      symbol_type
      make_VAL (const location_type& l)
      {
        return symbol_type (token::VAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EV (location_type l)
      {
        return symbol_type (token::EV, std::move (l));
      }
#else
      static
      symbol_type
      make_EV (const location_type& l)
      {
        return symbol_type (token::EV, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUMMY_NODE_VECTOR0 (location_type l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR0, std::move (l));
      }
#else
      static
      symbol_type
      make_DUMMY_NODE_VECTOR0 (const location_type& l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR0, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUMMY_NODE_VECTOR1 (location_type l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR1, std::move (l));
      }
#else
      static
      symbol_type
      make_DUMMY_NODE_VECTOR1 (const location_type& l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR1, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUMMY_NODE_VECTOR2 (location_type l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR2, std::move (l));
      }
#else
      static
      symbol_type
      make_DUMMY_NODE_VECTOR2 (const location_type& l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR2, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUMMY_NODE_VECTOR3 (location_type l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR3, std::move (l));
      }
#else
      static
      symbol_type
      make_DUMMY_NODE_VECTOR3 (const location_type& l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR3, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUMMY_NODE_VECTOR8000 (location_type l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR8000, std::move (l));
      }
#else
      static
      symbol_type
      make_DUMMY_NODE_VECTOR8000 (const location_type& l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR8000, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUMMY_NODE_VECTOR8001 (location_type l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR8001, std::move (l));
      }
#else
      static
      symbol_type
      make_DUMMY_NODE_VECTOR8001 (const location_type& l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR8001, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUMMY_NODE_VECTOR8002 (location_type l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR8002, std::move (l));
      }
#else
      static
      symbol_type
      make_DUMMY_NODE_VECTOR8002 (const location_type& l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR8002, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUMMY_NODE_VECTOR8003 (location_type l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR8003, std::move (l));
      }
#else
      static
      symbol_type
      make_DUMMY_NODE_VECTOR8003 (const location_type& l)
      {
        return symbol_type (token::DUMMY_NODE_VECTOR8003, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ENVVAR_DATA (location_type l)
      {
        return symbol_type (token::ENVVAR_DATA, std::move (l));
      }
#else
      static
      symbol_type
      make_ENVVAR_DATA (const location_type& l)
      {
        return symbol_type (token::ENVVAR_DATA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SGTYPE (location_type l)
      {
        return symbol_type (token::SGTYPE, std::move (l));
      }
#else
      static
      symbol_type
      make_SGTYPE (const location_type& l)
      {
        return symbol_type (token::SGTYPE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SIG_GROUP (location_type l)
      {
        return symbol_type (token::SIG_GROUP, std::move (l));
      }
#else
      static
      symbol_type
      make_SIG_GROUP (const location_type& l)
      {
        return symbol_type (token::SIG_GROUP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CM (location_type l)
      {
        return symbol_type (token::CM, std::move (l));
      }
#else
      static
      symbol_type
      make_CM (const location_type& l)
      {
        return symbol_type (token::CM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BA_DEF (location_type l)
      {
        return symbol_type (token::BA_DEF, std::move (l));
      }
#else
      static
      symbol_type
      make_BA_DEF (const location_type& l)
      {
        return symbol_type (token::BA_DEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INT (location_type l)
      {
        return symbol_type (token::INT, std::move (l));
      }
#else
      static
      symbol_type
      make_INT (const location_type& l)
      {
        return symbol_type (token::INT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HEX (location_type l)
      {
        return symbol_type (token::HEX, std::move (l));
      }
#else
      static
      symbol_type
      make_HEX (const location_type& l)
      {
        return symbol_type (token::HEX, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FLOAT (location_type l)
      {
        return symbol_type (token::FLOAT, std::move (l));
      }
#else
      static
      symbol_type
      make_FLOAT (const location_type& l)
      {
        return symbol_type (token::FLOAT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING (location_type l)
      {
        return symbol_type (token::STRING, std::move (l));
      }
#else
      static
      symbol_type
      make_STRING (const location_type& l)
      {
        return symbol_type (token::STRING, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ENUM (location_type l)
      {
        return symbol_type (token::ENUM, std::move (l));
      }
#else
      static
      symbol_type
      make_ENUM (const location_type& l)
      {
        return symbol_type (token::ENUM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BA_DEF_REL (location_type l)
      {
        return symbol_type (token::BA_DEF_REL, std::move (l));
      }
#else
      static
      symbol_type
      make_BA_DEF_REL (const location_type& l)
      {
        return symbol_type (token::BA_DEF_REL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BU_EV_REL (location_type l)
      {
        return symbol_type (token::BU_EV_REL, std::move (l));
      }
#else
      static
      symbol_type
      make_BU_EV_REL (const location_type& l)
      {
        return symbol_type (token::BU_EV_REL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BU_BO_REL (location_type l)
      {
        return symbol_type (token::BU_BO_REL, std::move (l));
      }
#else
      static
      symbol_type
      make_BU_BO_REL (const location_type& l)
      {
        return symbol_type (token::BU_BO_REL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BU_SG_REL (location_type l)
      {
        return symbol_type (token::BU_SG_REL, std::move (l));
      }
#else
      static
      symbol_type
      make_BU_SG_REL (const location_type& l)
      {
        return symbol_type (token::BU_SG_REL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BA_DEF_DEF (location_type l)
      {
        return symbol_type (token::BA_DEF_DEF, std::move (l));
      }
#else
      static
      symbol_type
      make_BA_DEF_DEF (const location_type& l)
      {
        return symbol_type (token::BA_DEF_DEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BA_DEF_DEF_REL (location_type l)
      {
        return symbol_type (token::BA_DEF_DEF_REL, std::move (l));
      }
#else
      static
      symbol_type
      make_BA_DEF_DEF_REL (const location_type& l)
      {
        return symbol_type (token::BA_DEF_DEF_REL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BA (location_type l)
      {
        return symbol_type (token::BA, std::move (l));
      }
#else
      static
      symbol_type
      make_BA (const location_type& l)
      {
        return symbol_type (token::BA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BA_REL (location_type l)
      {
        return symbol_type (token::BA_REL, std::move (l));
      }
#else
      static
      symbol_type
      make_BA_REL (const location_type& l)
      {
        return symbol_type (token::BA_REL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SG_MUL_VAL (location_type l)
      {
        return symbol_type (token::SG_MUL_VAL, std::move (l));
      }
#else
      static
      symbol_type
      make_SG_MUL_VAL (const location_type& l)
      {
        return symbol_type (token::SG_MUL_VAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OPEN_BRACKET (location_type l)
      {
        return symbol_type (token::OPEN_BRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_OPEN_BRACKET (const location_type& l)
      {
        return symbol_type (token::OPEN_BRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLOSE_BRACKET (location_type l)
      {
        return symbol_type (token::CLOSE_BRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_CLOSE_BRACKET (const location_type& l)
      {
        return symbol_type (token::CLOSE_BRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OPEN_PARENTHESIS (location_type l)
      {
        return symbol_type (token::OPEN_PARENTHESIS, std::move (l));
      }
#else
      static
      symbol_type
      make_OPEN_PARENTHESIS (const location_type& l)
      {
        return symbol_type (token::OPEN_PARENTHESIS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLOSE_PARENTHESIS (location_type l)
      {
        return symbol_type (token::CLOSE_PARENTHESIS, std::move (l));
      }
#else
      static
      symbol_type
      make_CLOSE_PARENTHESIS (const location_type& l)
      {
        return symbol_type (token::CLOSE_PARENTHESIS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PLUS (location_type l)
      {
        return symbol_type (token::PLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_PLUS (const location_type& l)
      {
        return symbol_type (token::PLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MINUS (location_type l)
      {
        return symbol_type (token::MINUS, std::move (l));
      }
#else
      static
      symbol_type
      make_MINUS (const location_type& l)
      {
        return symbol_type (token::MINUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VERTICAL_BAR (location_type l)
      {
        return symbol_type (token::VERTICAL_BAR, std::move (l));
      }
#else
      static
      symbol_type
      make_VERTICAL_BAR (const location_type& l)
      {
        return symbol_type (token::VERTICAL_BAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COLON (location_type l)
      {
        return symbol_type (token::COLON, std::move (l));
      }
#else
      static
      symbol_type
      make_COLON (const location_type& l)
      {
        return symbol_type (token::COLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SEMICOLON (location_type l)
      {
        return symbol_type (token::SEMICOLON, std::move (l));
      }
#else
      static
      symbol_type
      make_SEMICOLON (const location_type& l)
      {
        return symbol_type (token::SEMICOLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ASSIGN (location_type l)
      {
        return symbol_type (token::ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_ASSIGN (const location_type& l)
      {
        return symbol_type (token::ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COMMA (location_type l)
      {
        return symbol_type (token::COMMA, std::move (l));
      }
#else
      static
      symbol_type
      make_COMMA (const location_type& l)
      {
        return symbol_type (token::COMMA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AT (location_type l)
      {
        return symbol_type (token::AT, std::move (l));
      }
#else
      static
      symbol_type
      make_AT (const location_type& l)
      {
        return symbol_type (token::AT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EOL (location_type l)
      {
        return symbol_type (token::EOL, std::move (l));
      }
#else
      static
      symbol_type
      make_EOL (const location_type& l)
      {
        return symbol_type (token::EOL, l);
      }
#endif


    class context
    {
    public:
      context (const Parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const Parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    Parser (const Parser&);
    /// Non copyable.
    Parser& operator= (const Parser&);
#endif


    /// Stored state numbers (used for stacks).
    typedef short state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;

    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *yystr);

    /// For a symbol, its name in clear.
    static const char* const yytname_[];


    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const unsigned char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const unsigned char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const unsigned char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 345,     ///< Last index in yytable_.
      yynnts_ = 96,  ///< Number of nonterminal symbols.
      yyfinal_ = 7 ///< Termination state number.
    };


    // User arguments.
    class Scanner * scanner;
    class Network * network;

  };

  inline
  Parser::symbol_kind_type
  Parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63
    };
    // Last valid token kind.
    const int code_max = 318;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

  // basic_symbol.
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_object_type: // object_type
        value.copy< AttributeObjectType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_attribute_value_type: // attribute_value_type
        value.copy< AttributeValueType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_byte_order: // byte_order
        value.copy< ByteOrder > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_env_var_type: // env_var_type
        value.copy< EnvironmentVariable::Type > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_multiplexor_value_range_comma: // multiplexor_value_range_comma
      case symbol_kind::S_multiplexor_value_range: // multiplexor_value_range
        value.copy< ExtendedMultiplexor::ValueRange > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_message: // message
        value.copy< Message > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signal: // signal
        value.copy< Signal > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signal_extended_value_type_type: // signal_extended_value_type_type
        value.copy< Signal::ExtendedValueType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_table: // value_table
        value.copy< ValueTable > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_type: // value_type
        value.copy< ValueType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_double: // double
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_offset: // offset
      case symbol_kind::S_minimum: // minimum
      case symbol_kind::S_maximum: // maximum
      case symbol_kind::S_initial_value: // initial_value
      case symbol_kind::S_default_value: // default_value
        value.copy< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signed_64_integer: // signed_64_integer
        value.copy< int64_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_encoding_descriptions: // value_encoding_descriptions
        value.copy< std::map<int64_t, std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signals: // signals
        value.copy< std::map<std::string, Signal> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value_encoding_description: // value_encoding_description
        value.copy< std::pair<int64_t, std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_multiplexor_value_ranges: // multiplexor_value_ranges
        value.copy< std::set<ExtendedMultiplexor::ValueRange> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_signal_names: // signal_names
      case symbol_kind::S_receivers: // receivers
      case symbol_kind::S_transmitters: // transmitters
      case symbol_kind::S_access_nodes: // access_nodes
        value.copy< std::set<std::string> > (YY_MOVE (that.value));
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
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_char_strings: // char_strings
      case symbol_kind::S_new_symbol_values: // new_symbol_values
        value.copy< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_access_type: // access_type
        value.copy< uint16_t > (YY_MOVE (that.value));
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
        value.copy< uint32_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  Parser::symbol_kind_type
  Parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  Parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_object_type: // object_type
        value.move< AttributeObjectType > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_attribute_value_type: // attribute_value_type
        value.move< AttributeValueType > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_byte_order: // byte_order
        value.move< ByteOrder > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_env_var_type: // env_var_type
        value.move< EnvironmentVariable::Type > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_multiplexor_value_range_comma: // multiplexor_value_range_comma
      case symbol_kind::S_multiplexor_value_range: // multiplexor_value_range
        value.move< ExtendedMultiplexor::ValueRange > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_message: // message
        value.move< Message > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_signal: // signal
        value.move< Signal > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_signal_extended_value_type_type: // signal_extended_value_type_type
        value.move< Signal::ExtendedValueType > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_value_table: // value_table
        value.move< ValueTable > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_value_type: // value_type
        value.move< ValueType > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_double: // double
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_offset: // offset
      case symbol_kind::S_minimum: // minimum
      case symbol_kind::S_maximum: // maximum
      case symbol_kind::S_initial_value: // initial_value
      case symbol_kind::S_default_value: // default_value
        value.move< double > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_signed_64_integer: // signed_64_integer
        value.move< int64_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_value_encoding_descriptions: // value_encoding_descriptions
        value.move< std::map<int64_t, std::string> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_signals: // signals
        value.move< std::map<std::string, Signal> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_value_encoding_description: // value_encoding_description
        value.move< std::pair<int64_t, std::string> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_multiplexor_value_ranges: // multiplexor_value_ranges
        value.move< std::set<ExtendedMultiplexor::ValueRange> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_signal_names: // signal_names
      case symbol_kind::S_receivers: // receivers
      case symbol_kind::S_transmitters: // transmitters
      case symbol_kind::S_access_nodes: // access_nodes
        value.move< std::set<std::string> > (YY_MOVE (s.value));
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
        value.move< std::string > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_char_strings: // char_strings
      case symbol_kind::S_new_symbol_values: // new_symbol_values
        value.move< std::vector<std::string> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_access_type: // access_type
        value.move< uint16_t > (YY_MOVE (s.value));
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
        value.move< uint32_t > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  inline
  Parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  Parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  inline
  Parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  inline
  Parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  inline
  void
  Parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  inline
  void
  Parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  inline
  Parser::symbol_kind_type
  Parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  inline
  Parser::symbol_kind_type
  Parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


#line 4 "Parser.yy"
} } // Vector::DBC
#line 3351 "Parser.hpp"




#endif // !YY_YY_PARSER_HPP_INCLUDED
