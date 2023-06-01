%skeleton "lalr1.cc"
%require "3.0"
%defines
%define api.namespace {Vector::DBC}
%define api.parser.class {Parser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%define parse.trace
%language "C++"
%locations

    // debug options
%verbose
%debug

%code requires{
#include <Vector/DBC/Network.h>
namespace Vector {
namespace DBC {
class Network;
}
}
}

%lex-param { const Vector::DBC::Parser::location_type & loc }
%parse-param { class Scanner * scanner }
%parse-param { class Network * network }

%code{
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
}

    // %destructor { delete($$); ($$) = nullptr; } <network>

    /* 2 General Definitions */
// 2023-03-15 STW: Add new seperate type for value ranges to avoid mismatch with signed type
%token <std::string> UNSIGNED_INTEGER MULTIPLEXOR_VALUE_RANGE SIGNED_INTEGER DOUBLE CHAR_STRING DBC_IDENTIFIER
%type <uint32_t> unsigned_integer
// 2023-03-15 STW: Remove unused int32 type (replaced by int64 to allow compatibility with int64 DBC files)
// 2023-03-02 STW: Add int64 type to allow compatibility with int32 DBC files
%type <int64_t> signed_64_integer
%type <double> double
%type <std::string> char_string
// 2023-03-14 STW: Add new eol type to include EOL exceptions in some checks
%type <std::string> char_string_eol
%type <std::vector<std::string>> char_strings
%type <std::string> dbc_identifier

    /* 3 Structure of the DBC File */

    /* 4 Version and New Symbol Specification */
%token VERSION NS
%token <std::string> NS_VALUE
%type <std::string> candb_version_string
%type <std::vector<std::string>> new_symbol_values

    /* 5 Bit Timing Definition */
%token BS
%type <uint32_t> baudrate
%type <uint32_t> btr1
%type <uint32_t> btr2

    /* 6 Node Definitions */
%token BU
%type <std::string> node_name

    /* 7 Value Table Definitions */
%token VAL_TABLE
%type <ValueTable> value_table
%type <std::string> value_table_name

    /* 7.1 Value Descriptions (Value Encodings) */
// 2023-03-02 STW: Replace uint32 by int64 to allow compatibility with int32 DBC files
%type <std::map<int64_t, std::string>> value_encoding_descriptions
// 2023-03-02 STW: Replace uint32 by int64 to allow compatibility with int32 DBC files
%type <std::pair<int64_t, std::string>> value_encoding_description

    /* 8 Message Definitions */
%token BO VECTOR_XXX
%type <Message> message
%type <uint32_t> message_id
%type <std::string> message_name
%type <uint32_t> message_size
%type <std::string> transmitter

    /* 8.1 Pseudo-message */

    /* 8.2 Signal Definitions */
%token SG LOWER_M UPPER_M SIG_VALTYPE
%type <std::map<std::string, Signal>> signals
%type <Signal> signal
%type <std::string> signal_name
%type <std::set<std::string>> signal_names
%type <std::string> multiplexer_indicator
//%type <uint32_t> multiplexer_switch_value
%type <uint32_t> start_bit
%type <uint32_t> signal_size
%type <ByteOrder> byte_order
%type <ValueType> value_type
%type <double> factor
%type <double> offset
%type <double> minimum
%type <double> maximum
%type <std::string> unit
%type <std::set<std::string>> receivers
%type <std::string> receiver
%type <Signal::ExtendedValueType> signal_extended_value_type_type

    /* 8.3 Definition of Message Transmitters */
%token BO_TX_BU
%type <std::set<std::string>> transmitters

    /* 8.4 Signal Value Descriptions (Value Encodings) */
%token VAL

    /* 9 Environment Variable Definitions */
%token EV
%token DUMMY_NODE_VECTOR0 DUMMY_NODE_VECTOR1 DUMMY_NODE_VECTOR2 DUMMY_NODE_VECTOR3
%token DUMMY_NODE_VECTOR8000 DUMMY_NODE_VECTOR8001 DUMMY_NODE_VECTOR8002 DUMMY_NODE_VECTOR8003
%token ENVVAR_DATA
%type <std::string> env_var_name
%type <EnvironmentVariable::Type> env_var_type
%type <double> initial_value
%type <uint32_t> ev_id
%type <uint16_t> access_type
%type <std::set<std::string>> access_nodes
%type <std::string> access_node
%type <uint32_t> data_size

    /* 9.1 Environment Variable Value Descriptions */

    /* 10 Signal Type and Signal Group Definitions */
%token SGTYPE SIG_GROUP
%type <std::string> signal_type_name
%type <double> default_value
%type <std::string> signal_group_name
%type <uint32_t> repetitions

    /* 11 Comment Definitions */
%token CM

    /* 12 User Defined Attribute Definitions */

    /* 12.1 Attribute Definitions */
%token BA_DEF INT HEX FLOAT STRING ENUM
%token BA_DEF_REL BU_EV_REL BU_BO_REL BU_SG_REL
%type <AttributeObjectType> object_type
%type <std::string> attribute_name
%type <AttributeValueType> attribute_value_type

    /* Attribute Defaults */
%token BA_DEF_DEF
%token BA_DEF_DEF_REL

    /* 12.2 Attribute Values */
%token BA
%token BA_REL
%type <std::string> attribute_value

    /* 13 Extended Multiplexing */
%token SG_MUL_VAL
%type <std::string> multiplexed_signal_name
%type <std::string> multiplexor_switch_name
%type <std::set<ExtendedMultiplexor::ValueRange>> multiplexor_value_ranges
%type <ExtendedMultiplexor::ValueRange> multiplexor_value_range
// 2023-03-15 STW: Add new comma type to include COMMA exceptions in some checks
%type <ExtendedMultiplexor::ValueRange> multiplexor_value_range_comma

    /* Punctuators */
%token OPEN_BRACKET CLOSE_BRACKET OPEN_PARENTHESIS CLOSE_PARENTHESIS
%token PLUS MINUS VERTICAL_BAR COLON SEMICOLON ASSIGN COMMA AT
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
%type <std::string> semicolon_with_optional_eol

    /* end of line */
%token EOL

    /* match end of file */
%token END 0

%%

    /* 3 Structure of the DBC File */
network
        : version                           // VERSION
          new_symbols                       // NS_(DESC_)
          bit_timing                        // BS_
          nodes                             // BU_
          value_tables                      // VAL_TABLE_
          messages                          // BO_ / SG_
          message_transmitters              // BO_TX_BU_
          environment_variables             // EV_
          environment_variables_data        // ENVVAR_DATA_
          signal_types                      // SGTYPE_ SGTYPE_VAL_ BA_DEF_SGTYPE_ BA_SGTYPE_
          comments                          // CM_ ...
          attribute_definitions             // BA_DEF_(REL_)
          attribute_defaults                // BA_DEF_DEF_(REL_)
          attribute_values                  // BA_(REL_)
          value_descriptions                // VAL_
          /*
          category_definitions              // CAT_DEF_
          categories                        // CAT_
          filter                            // FILTER_
          signal_type_refs                  // SIG_TYPE_REF_
          */
          signal_groups                     // SIG_GROUP_
          signal_extended_value_types       // SIG_VALTYPE_ SIGTYPE_VALTYPE_
          extended_multiplexing             // SG_MUL_VAL_
        ;

    /* 2 General Definitions */
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
semicolon_with_optional_eol
        : SEMICOLON { $$ = ""; }
        | SEMICOLON EOL { $$ = ""; }
        ;
unsigned_integer
        : UNSIGNED_INTEGER { $$ = std::stoul($1); }
        ;
// 2023-03-15 STW: Remove unused int32 type (replaced by int64 to allow compatibility with int64 DBC files)
signed_64_integer
        : UNSIGNED_INTEGER { $$ = std::stoll($1); }
        | SIGNED_INTEGER { $$ = std::stoll($1); }
        ;
double
        : DOUBLE { $$ = std::stod($1); }
        | SIGNED_INTEGER { $$ = std::stod($1); }
        | UNSIGNED_INTEGER { $$ = std::stod($1); }
        ;
char_string
        : CHAR_STRING { $$ = $1; }
        ;
// 2023-03-14 STW: Add new eol type to include EOL exceptions in some checks
char_string_eol
        : CHAR_STRING EOL { $$ = $1; }
        | CHAR_STRING { $$ = $1; }
        ;
char_strings
        : CHAR_STRING { $$ = std::vector<std::string>(); $$.push_back($1); }
        | char_strings COMMA CHAR_STRING { $$ = $1; $$.push_back($3); }
        ;
dbc_identifier
        : DBC_IDENTIFIER { $$ = $1; }
        ;

    /* 4 Version and New Symbol Specification */
version
        : VERSION candb_version_string EOL { network->version = $2; }
        ;
candb_version_string
        : char_string { $$ = $1; }
        ;
new_symbols
        : %empty
        | NS COLON EOL
          new_symbol_values { network->newSymbols = $4; }
        ;
new_symbol_values
        : %empty { $$ = std::vector<std::string>(); }
        | new_symbol_values NS_VALUE EOL { $$ = $1; $$.push_back($2); }
        ;

    /* 5 Bit Timing Definition */
bit_timing
        : BS COLON EOL
        | BS COLON baudrate COLON btr1 COMMA btr2 EOL {
              network->bitTiming.baudrate = $baudrate;
              network->bitTiming.btr1 = $btr1;
              network->bitTiming.btr2 = $btr2;
          }
        ;
baudrate
        : unsigned_integer { $$ = $1; }
        ;
btr1
        : unsigned_integer { $$ = $1; }
        ;
btr2
        : unsigned_integer { $$ = $1; }
        ;

    /* 6 Node Definitions */
nodes
        : BU COLON node_names EOL
        ;
node_names
        : %empty
        | VECTOR_XXX
        | node_names node_name { network->nodes[$node_name].name = $node_name; }
        ;
node_name
        : dbc_identifier { $$ = $1; }
        ;

    /* 7 Value Table Definitions */
value_tables
        : %empty
        | value_tables value_table { network->valueTables[$value_table.name] = $value_table; }
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
value_table
        : VAL_TABLE value_table_name value_encoding_descriptions semicolon_with_optional_eol {
              $$ = ValueTable();
              $$.name = $value_table_name;
              $$.valueDescriptions = $value_encoding_descriptions;
          }
        ;
value_table_name
        : dbc_identifier { $$ = $1; }
        ;

    /* 7.1 Value Descriptions (Value Encodings) */
// 2023-03-02 STW: Replace uint32 by int64 to allow compatibility with int32 DBC files
value_encoding_descriptions
        : %empty { $$ = std::map<int64_t, std::string>(); }
        | value_encoding_descriptions value_encoding_description { $$ = $1; $$.insert($2); }
        ;
// 2023-03-02 STW: Replace uint32 by int64 to allow compatibility with int32 DBC files
// 2023-03-14 STW: Add new eol type to include EOL exceptions in some checks
value_encoding_description
        : signed_64_integer char_string_eol { $$ = std::make_pair($1, $2); }
        | signed_64_integer EOL char_string_eol { $$ = std::make_pair($1, $3); }
        ;

    /* 8 Message Definitions */
messages
        : %empty
        | messages message { network->messages[$message.id] = $message; }
        ;
message
        : BO message_id message_name COLON message_size transmitter EOL signals {
              $$ = Message();
              $$.id = $message_id;
              $$.name = $message_name;
              $$.size = $message_size;
              $$.transmitter = $transmitter;
              $$.signals = $signals;
          }
        ;
message_id
        : unsigned_integer { $$ = $1; }
        ;
message_name
        : dbc_identifier { $$ = $1; }
        ;
message_size
        : unsigned_integer { $$ = $1; }
        ;
transmitter
        : node_name { $$ = $1; }
        | VECTOR_XXX { $$ = ""; }
        ;

    /* 8.1 Pseudo-message */

    /* 8.2 Signal Definitions */
signals
        : %empty { $$ = std::map<std::string, Signal>(); }
        | signals signal { $$ = $1; $$[$2.name] = $2; }
        ;
signal
        : SG signal_name multiplexer_indicator COLON start_bit VERTICAL_BAR signal_size AT byte_order value_type OPEN_PARENTHESIS factor COMMA offset CLOSE_PARENTHESIS OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit receivers EOL {
              $$ = Signal();
              $$.name = $signal_name;
              if ($multiplexer_indicator == "*") {
                  $$.multiplexor = Signal::Multiplexor::MultiplexorSwitch;
              } else
              if (!$multiplexer_indicator.empty()) {
                  $$.multiplexor = Signal::Multiplexor::MultiplexedSignal;
                  $$.multiplexerSwitchValue = std::stoul($multiplexer_indicator);
              }
              $$.startBit = $start_bit;
              $$.bitSize = $signal_size;
              $$.byteOrder = $byte_order;
              $$.valueType = $value_type;
              $$.factor = $factor;
              $$.offset = $offset;
              $$.minimum = $minimum;
              $$.maximum = $maximum;
              $$.unit = $unit;
              $$.receivers = $receivers;
          }
        ;
signal_name
        : dbc_identifier { $$ = $1; }
        ;
signal_names
        : %empty { $$ = std::set<std::string>(); }
        | signal_names signal_name { $$ = $1; $$.insert($2); }
        ;
multiplexer_indicator
        : %empty { $$ = ""; }
        | DBC_IDENTIFIER { $$ = $1; $$.erase(0, 1); }
        | UPPER_M { $$ = "*"; }
        ;
//multiplexer_switch_value
//        : unsigned_integer { $$ = $1; }
//        ;
start_bit
        : unsigned_integer { $$ = $1; }
        ;
signal_size
        : unsigned_integer { $$ = $1; }
        ;
byte_order
        : UNSIGNED_INTEGER {
              if ($1 == "0") { $$ = ByteOrder::BigEndian; }
              if ($1 == "1") { $$ = ByteOrder::LittleEndian; }
          }
        ;
value_type
        : PLUS { $$ = ValueType::Unsigned; }
        | MINUS { $$ = ValueType::Signed; }
        ;
factor
        : double { $$ = $1; }
        ;
offset
        : double { $$ = $1; }
        ;
minimum
        : double { $$ = $1; }
        ;
maximum
        : double { $$ = $1; }
        ;
unit
        : char_string { $$ = $1; }
        ;
receivers
        : receiver {
              $$ = std::set<std::string>();
              if (!$receiver.empty()) {
                  $$.insert($receiver);
              }
          }
        | receivers COMMA receiver {
              $$ = $1;
              if (!$receiver.empty()) {
                  $$.insert($receiver);
              }
          }
        ;
receiver
        : node_name { $$ = $1; }
        | VECTOR_XXX { $$ = ""; }
        ;
signal_extended_value_types
        : %empty
        | signal_extended_value_types signal_extended_value_type
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
signal_extended_value_type
        : SIG_VALTYPE message_id signal_name COLON signal_extended_value_type_type semicolon_with_optional_eol {
              network->messages[$message_id].signals[$signal_name].extendedValueType = $signal_extended_value_type_type;
          }
        ;
signal_extended_value_type_type
        : unsigned_integer {
              switch ($unsigned_integer) {
              case 0:
                  $$ = Signal::ExtendedValueType::Integer;
                  break;
              case 1:
                  $$ = Signal::ExtendedValueType::Float;
                  break;
              case 2:
                  $$ = Signal::ExtendedValueType::Double;
                  break;
              }
          }
        ;

    /* 8.3 Definition of Message Transmitters */
message_transmitters
        : %empty
        | message_transmitters message_transmitter
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
message_transmitter
        : BO_TX_BU message_id COLON transmitters semicolon_with_optional_eol { network->messages[$message_id].transmitters = $transmitters; }
        ;
transmitters
        : transmitter { $$ = std::set<std::string>(); $$.insert($1); }
        | transmitters COMMA transmitter { $$ = $1; $$.insert($3); }
        ;

    /* 8.4 Signal Value Descriptions (Value Encodings) */
value_descriptions
        : %empty
        | value_descriptions value_descriptions_for_signal
        | value_descriptions value_descriptions_for_env_var
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
value_descriptions_for_signal
        : VAL message_id signal_name value_encoding_descriptions semicolon_with_optional_eol {
              network->messages[$message_id].signals[$signal_name].valueDescriptions = $value_encoding_descriptions;
          }
        ;

    /* 9 Environment Variable Definitions */
environment_variables
        : %empty
        | environment_variables environment_variable
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
environment_variable
        : EV env_var_name COLON env_var_type OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit initial_value ev_id access_type access_nodes semicolon_with_optional_eol {
              EnvironmentVariable & environmentVariable = network->environmentVariables[$env_var_name];
              environmentVariable.name = $env_var_name;
              environmentVariable.type = $env_var_type;
              environmentVariable.minimum = $minimum;
              environmentVariable.maximum = $maximum;
              environmentVariable.unit = $unit;
              environmentVariable.initialValue = $initial_value;
              environmentVariable.id = $ev_id;
              if ($access_type >> 15) {
                  environmentVariable.type = EnvironmentVariable::Type::String;
              }
              switch($access_type & 0xff) {
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
              environmentVariable.accessNodes = $access_nodes;
          }
        ;
env_var_name
        : dbc_identifier { $$ = $1; }
        ;
env_var_type
        : UNSIGNED_INTEGER {
              if ($1 == "0") { $$ = EnvironmentVariable::Type::Integer; }
              if ($1 == "1") { $$ = EnvironmentVariable::Type::Float; }
              if ($1 == "2") { $$ = EnvironmentVariable::Type::String; }
          }
        ;
initial_value
        : double { $$ = $1; }
        ;
ev_id
        : unsigned_integer { $$ = $1; }
        ;
access_type
        : DUMMY_NODE_VECTOR0 { $$ = 0x0000; }
        | DUMMY_NODE_VECTOR1 { $$ = 0x0001; }
        | DUMMY_NODE_VECTOR2 { $$ = 0x0002; }
        | DUMMY_NODE_VECTOR3 { $$ = 0x0003; }
        | DUMMY_NODE_VECTOR8000  { $$ = 0x8000; }
        | DUMMY_NODE_VECTOR8001  { $$ = 0x8001; }
        | DUMMY_NODE_VECTOR8002  { $$ = 0x8002; }
        | DUMMY_NODE_VECTOR8003  { $$ = 0x8003; }
        ;
access_nodes
        : access_node {
              $$ = std::set<std::string>();
              if (!$access_node.empty()) {
                  $$.insert($access_node);
              }
          }
        | access_nodes COMMA access_node {
              $$ = $1;
              if (!$access_node.empty()) {
                  $$.insert($access_node);
              }
          }
        ;
access_node
        : node_name { $$ = $1; }
        | VECTOR_XXX { $$ = ""; }
        ;
environment_variables_data
        : %empty
        | environment_variables_data environment_variable_data
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
environment_variable_data
        : ENVVAR_DATA env_var_name COLON data_size semicolon_with_optional_eol {
              network->environmentVariables[$env_var_name].type = EnvironmentVariable::Type::Data;
              network->environmentVariables[$env_var_name].dataSize = $data_size;
          }
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
data_size
        : unsigned_integer { $$ = $1; }
        ;

    /* 9.1 Environment Variable Value Descriptions */
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
value_descriptions_for_env_var
        : VAL env_var_name value_encoding_descriptions semicolon_with_optional_eol {
              network->environmentVariables[$env_var_name].valueDescriptions = $value_encoding_descriptions;
          }
        ;

    /* 10 Signal Type and Signal Group Definitions */
signal_types
        : %empty
        | signal_types signal_type
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
signal_type
        : SGTYPE signal_type_name COLON signal_size VERTICAL_BAR byte_order value_type
          OPEN_PARENTHESIS factor COMMA offset CLOSE_PARENTHESIS
          OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET
          unit default_value COMMA value_table_name semicolon_with_optional_eol {
              SignalType & signalType = network->signalTypes[$signal_type_name];
              signalType.name = $signal_type_name;
              signalType.size = $signal_size;
              signalType.byteOrder = $byte_order;
              signalType.valueType = $value_type;
              signalType.factor = $factor;
              signalType.offset = $offset;
              signalType.minimum = $minimum;
              signalType.maximum = $maximum;
              signalType.unit = $unit;
              signalType.defaultValue = $default_value;
              signalType.valueTable = $value_table_name;
          }
        ;
signal_type_name
        : dbc_identifier { $$ = $1; }
        ;
default_value
        : double { $$ = $1; }
        ;
//signal_type_refs
//        : %empty
//        | signal_type_refs signal_type_ref
//        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
//signal_type_ref
//        : SGTYPE message_id signal_name COLON signal_type_name semicolon_with_optional_eol {
//              network->messages[$message_id].signals[$signal_name].type = $signal_type_name;
//          }
//        ;
signal_groups
        : %empty
        | signal_groups signal_group
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
signal_group
        : SIG_GROUP message_id signal_group_name repetitions COLON signal_names semicolon_with_optional_eol {
              SignalGroup & signalGroup = network->messages[$message_id].signalGroups[$signal_group_name];
              signalGroup.messageId = $message_id;
              signalGroup.name = $signal_group_name;
              signalGroup.repetitions = $repetitions;
              signalGroup.signals = $signal_names;
          }
        ;
signal_group_name
        : dbc_identifier { $$ = $1; }
        ;
repetitions
        : unsigned_integer { $$ = $1; }
        ;

    /* 11 Comment Definitions */
comments
        : %empty
        | comments comment
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
comment
        : CM char_string semicolon_with_optional_eol { network->comment = $char_string; }
        | CM BU node_name char_string semicolon_with_optional_eol { network->nodes[$node_name].comment = $char_string; }
        | CM BO message_id char_string semicolon_with_optional_eol { network->messages[$message_id].comment = $char_string; }
        | CM SG message_id signal_name char_string semicolon_with_optional_eol { network->messages[$message_id].signals[$signal_name].comment = $char_string; }
        | CM EV env_var_name char_string semicolon_with_optional_eol { network->environmentVariables[$env_var_name].comment = $char_string; }
        ;

    /* 12 User Defined Attribute Definitions */

    /* 12.1 Attribute Definitions */
attribute_definitions
        : %empty
        | attribute_definitions attribute_definition
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
attribute_definition
        : BA_DEF object_type attribute_name attribute_value_type semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              attributeDefinition.name = $attribute_name;
              attributeDefinition.objectType = $object_type;
              attributeDefinition.valueType = $attribute_value_type;
          }
        | BA_DEF_REL object_type attribute_name attribute_value_type semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              attributeDefinition.name = $attribute_name;
              attributeDefinition.objectType = $object_type;
              attributeDefinition.valueType = $attribute_value_type;
          }
        ;
object_type
        : %empty { $$ = AttributeObjectType::Network; }
        | BU { $$ = AttributeObjectType::Node; }
        | BO { $$ = AttributeObjectType::Message; }
        | SG { $$ = AttributeObjectType::Signal; }
        | EV { $$ = AttributeObjectType::EnvironmentVariable; }
        | BU_EV_REL { $$ = AttributeObjectType::ControlUnitEnvironmentVariable; }
        | BU_BO_REL { $$ = AttributeObjectType::NodeTxMessage; }
        | BU_SG_REL { $$ = AttributeObjectType::NodeMappedRxSignal; }
        ;
attribute_name
        : char_string { $$ = $1; }
        ;
// 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
attribute_value_type
        : INT signed_64_integer signed_64_integer {
              $$ = AttributeValueType();
              $$.type = AttributeValueType::Type::Int;
              $$.integerValue.minimum = $2;
              $$.integerValue.maximum = $3;
          }
        | HEX signed_64_integer signed_64_integer {
              $$ = AttributeValueType();
              $$.type = AttributeValueType::Type::Hex;
              $$.hexValue.minimum = $2;
              $$.hexValue.maximum = $3;
          }
        | FLOAT double double {
              $$ = AttributeValueType();
              $$.type = AttributeValueType::Type::Float;
              $$.floatValue.minimum = $2;
              $$.floatValue.maximum = $3;
          }
        | STRING {
              $$ = AttributeValueType();
              $$.type = AttributeValueType::Type::String;
          }
        | ENUM char_strings {
              $$ = AttributeValueType();
              $$.type = AttributeValueType::Type::Enum;
              $$.enumValues = $char_strings;
          }
        ;

    /* Attribute Defaults */
attribute_defaults
        : %empty
        | attribute_defaults attribute_default
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
attribute_default
        : BA_DEF_DEF attribute_name attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              Attribute & attributeDefault = network->attributeDefaults[$attribute_name];
              attributeDefault.name = $attribute_name;
              attributeDefault.objectType = attributeDefinition.objectType;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeDefault.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeDefault.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attributeDefault.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attributeDefault.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  attributeDefault.stringValue = $attribute_value;
                  break;
              }
          }
        | BA_DEF_DEF_REL attribute_name attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              Attribute & attributeDefault = network->attributeDefaults[$attribute_name];
              attributeDefault.name = $attribute_name;
              attributeDefault.objectType = attributeDefinition.objectType;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeDefault.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeDefault.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attributeDefault.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attributeDefault.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  attributeDefault.stringValue = $attribute_value;
                  break;
              }
          }
        ;
attribute_value
        : UNSIGNED_INTEGER { $$ = $1; }
        | SIGNED_INTEGER { $$ = $1; }
        | DOUBLE { $$ = $1; }
        | CHAR_STRING { $$ = $1; }
        ;

    /* 12.2 Attribute Values */
attribute_values
        : %empty
        | attribute_values attribute_value_for_object
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
attribute_value_for_object
        : BA attribute_name attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              Attribute & attribute = network->attributeValues[$attribute_name];
              attribute.name = $attribute_name;
              attribute.objectType = AttributeObjectType::Network;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.enumValue = std::stoll($attribute_value);
                  break;
              }
          }
        | BA attribute_name BU node_name attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              Attribute & attribute = network->nodes[$node_name].attributeValues[$attribute_name];
              attribute.name = $attribute_name;
              attribute.objectType = AttributeObjectType::Node;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.enumValue = std::stoll($attribute_value);
                  break;
              }
          }
        | BA attribute_name BO message_id attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              Attribute & attribute = network->messages[$message_id].attributeValues[$attribute_name];
              attribute.name = $attribute_name;
              attribute.objectType = AttributeObjectType::Message;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.enumValue = std::stoll($attribute_value);
                  break;
              }
          }
        | BA attribute_name SG message_id signal_name attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              Attribute & attribute = network->messages[$message_id].signals[$signal_name].attributeValues[$attribute_name];
              attribute.name = $attribute_name;
              attribute.objectType = AttributeObjectType::Signal;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-17 STW: Improve enum handling
                  attribute.stringValue = attributeDefinition.valueType.GetEnumValue($attribute_value);
                  break;
              }
          }
        | BA attribute_name EV env_var_name attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              Attribute & attribute = network->environmentVariables[$env_var_name].attributeValues[$attribute_name];
              attribute.name = $attribute_name;
              attribute.objectType = AttributeObjectType::EnvironmentVariable;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attribute.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attribute.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attribute.enumValue = std::stoll($attribute_value);
                  break;
              }
          }
        | BA_REL attribute_name BU_EV_REL node_name env_var_name attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              AttributeRelation & attributeRelation = network->attributeRelationValues[$attribute_name];
              attributeRelation.name = $attribute_name;
              attributeRelation.objectType = AttributeObjectType::ControlUnitEnvironmentVariable;
              attributeRelation.nodeName = $node_name;
              attributeRelation.environmentVariableName = $env_var_name;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attributeRelation.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attributeRelation.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.enumValue = std::stoll($attribute_value);
                  break;
              }
          }
        | BA_REL attribute_name BU_BO_REL node_name message_id attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              AttributeRelation & attributeRelation = network->attributeRelationValues[$attribute_name];
              attributeRelation.name = $attribute_name;
              attributeRelation.objectType = AttributeObjectType::NodeTxMessage;
              attributeRelation.nodeName = $node_name;
              attributeRelation.messageId = $message_id;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attributeRelation.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attributeRelation.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.enumValue = std::stoll($attribute_value);
                  break;
              }
          }
        | BA_REL attribute_name BU_SG_REL node_name SG message_id signal_name attribute_value semicolon_with_optional_eol {
              AttributeDefinition & attributeDefinition = network->attributeDefinitions[$attribute_name];
              AttributeRelation & attributeRelation = network->attributeRelationValues[$attribute_name];
              attributeRelation.name = $attribute_name;
              attributeRelation.objectType = AttributeObjectType::NodeMappedRxSignal;
              attributeRelation.nodeName = $node_name;
              attributeRelation.messageId = $message_id;
              attributeRelation.signalName = $signal_name;
              switch(attributeDefinition.valueType.type) {
              case AttributeValueType::Type::Int:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.integerValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Hex:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.hexValue = std::stoll($attribute_value);
                  break;
              case AttributeValueType::Type::Float:
                  attributeRelation.floatValue = std::stod($attribute_value);
                  break;
              case AttributeValueType::Type::String:
                  attributeRelation.stringValue = $attribute_value;
                  break;
              case AttributeValueType::Type::Enum:
                  // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
                  attributeRelation.enumValue = std::stoll($attribute_value);
                  break;
              }
          }
        ;

    /* 13 Extended Multiplexing */
extended_multiplexing
        : %empty
        | extended_multiplexing multiplexed_signal
        ;
// 2023-03-21 STW: Combine semicolon and eol check to make eol optional
multiplexed_signal
        : SG_MUL_VAL message_id multiplexed_signal_name multiplexor_switch_name multiplexor_value_ranges semicolon_with_optional_eol {
              ExtendedMultiplexor & extendedMultiplexor = network->messages[$message_id].signals[$multiplexed_signal_name].extendedMultiplexors[$multiplexor_switch_name];
              extendedMultiplexor.switchName = $multiplexor_switch_name;
              extendedMultiplexor.valueRanges = $multiplexor_value_ranges;
          }
        ;
multiplexed_signal_name
        : dbc_identifier { $$ = $1; }
        ;
multiplexor_switch_name
        : dbc_identifier { $$ = $1; }
        ;
// 2023-03-15 STW: Add new comma type to include COMMA exceptions in some checks
multiplexor_value_ranges
        : %empty { $$ = std::set<ExtendedMultiplexor::ValueRange>(); }
        | multiplexor_value_ranges multiplexor_value_range_comma { $$ = $1; $$.insert($2); }
        ;
// 2023-03-15 STW: Add new comma type to include COMMA exceptions in some checks
multiplexor_value_range_comma
        : multiplexor_value_range COMMA { $$ = $1; }
        | multiplexor_value_range { $$ = $1; }
        ;
// 2023-03-15 STW: Add new seperate type for value ranges to avoid mismatch with signed type
multiplexor_value_range
        : MULTIPLEXOR_VALUE_RANGE { 
        const std::string complete = $1; 
        const std::string token1_str = complete.substr(0, complete.find("-")); 
        const std::string token2_str = complete.substr(token1_str.size() + 1, complete.size() - (token1_str.size() + 1)); 
        const uint32_t token1_int = std::stoul(token1_str);
        const uint32_t token2_int = std::stoul(token2_str);
        $$ = std::make_pair(token1_int, token2_int); 
        }
        ;

%%

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
