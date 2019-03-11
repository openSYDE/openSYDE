/*
 * Copyright (C) 2013 Tobias Lorenz.
 * Contact: tobias.lorenz@gmx.net
 *
 * This file is part of Tobias Lorenz's Toolkit.
 *
 * Commercial License Usage
 * Licensees holding valid commercial licenses may use this file in
 * accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Tobias Lorenz.
 *
 * GNU General Public License 3.0 Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 * This file contains modifications made by Sensor-Technik Wiedemann GmbH (STW)
 * The modifications are also under GPL 3.0 like the original code itself.
 */

#include <Vector/DBC/File.h>

#include <fstream>
#include <sstream>
#include <stack>
#include <string>

/* switch for std/boost regex library */
#ifdef OPTION_USE_CPP11_REGEX
#include <regex>
#define smatch        std::smatch
#define regex         std::regex
#define regex_search  std::regex_search
#define regex_replace std::regex_replace
#else
#include <boost/regex.hpp>
#define smatch        boost::smatch
#define regex         boost::regex
#define regex_search  boost::regex_search
#define regex_replace boost::regex_replace
#endif

/* force Linux to use Windows line endings */
static const char endl[] = "\r\n";

/* Unsigned integer */
#define REGEX_UINT "([[:digit:]]+)"

/* Double (not strict, invalid values may fall through: "..E+-e..1-23e") */
#define REGEX_DOUBLE "([[:digit:]\\.\\+\\-eE]+)"

/* Name */
#ifdef OPTION_USE_STRICT_NAMES
#define REGEX_NAME "([[:alpha:]_][[:alnum:]_]*)" /* C identifier */
#else
#define REGEX_NAME "([^[:space:]\\:]*)"
#endif

/* Attribute name */
#ifdef OPTION_USE_STRICT_NAMES
#define REGEX_ATTRIB_NAME "\"([[:alpha:]_][[:alnum:]_]*)\"" /* C identifier */
#else
#define REGEX_ATTRIB_NAME "\"([^\"[:space:]]*)\""
#endif

/* Optional text between "" */
#define REGEX_STRING "\"([^\"]*)\""

/* Start of line */
#define REGEX_SOL "^[[:space:]]*"

/* End of line */
#define REGEX_EOL "[[:space:]]*$"

/* End of line with ; delimiter */
#define REGEX_EOL_DELIM "[[:space:]]*;" REGEX_EOL

/* Not greedy to exclude trailing whitespace */
#define REGEX_TO_END "(.*?)"

/* One or more spaces */
#define REGEX_SPACE "[[:space:]]+"

/* Delimiter */
#define REGEX_DELIM(x) "[[:space:]]*" x "[[:space:]]*"

namespace Vector {
namespace DBC {

/* Removes trailing spaces incl. windows/unix/mac line endings. */
void File::chomp(std::string & line)
{
    /* don't do anything if line is empty */
    if (line.empty()) {
        return;
    }

    /* remove trailing spaces */
    line = regex_replace(line, regex("[[:space:]]+$"), "");
}

/* stod without C locale */
double File::stod(const std::string & str)
{
    std::istringstream iss(str);

    /* use english decimal points for floating numbers */
    iss.imbue(std::locale("C"));

    double d;
    iss >> d;

    return d;
}

/* Version (VERSION) */
void File::readVersion(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "VERSION" REGEX_SPACE REGEX_STRING REGEX_EOL);
    if (regex_search(line, m, re)) {
        network.version = m[1];
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedVersion);
    }
}

/* New Symbols (NS) */
void File::readNewSymbols(Network & network, std::ifstream & ifs, std::string & line)
{
    while(ifs.good()) {
        std::getline(ifs, line);
        chomp(line);
        if (line.empty())
            return;

        smatch m;
        regex re(REGEX_SOL REGEX_NAME REGEX_EOL);
        if (regex_search(line, m, re)) {
            network.newSymbols.push_back(m[1]);
        }
    }
}

/* Bit Timing (BS) */
void File::readBitTiming(Network & network, std::string & line)
{
    if (line == "BS_:") {
        // all ok
        return;
    } else {
        smatch m;
        regex re(REGEX_SOL "BS_:" REGEX_SPACE REGEX_UINT ":" REGEX_UINT ":" REGEX_UINT REGEX_EOL);
        if (regex_search(line, m, re)) {
            network.bitTiming.baudrate = stoul(m[1]);
            network.bitTiming.btr1 = stoul(m[2]);
            network.bitTiming.btr2 = stoul(m[2]);
            return;
        }
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedBitTiming);
    }
}

/* Nodes (BU) */
void File::readNodes(Network & network, std::string & line)
{
    std::istringstream iss(line);
    std::string node;

    /* skip BU_: */
    iss >> node;

    while (iss.good()) {
        iss >> node;
        network.nodes[node].name = node;
    }
}

/* Value Tables (VAL_TABLE) */
void File::readValueTable(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "VAL_TABLE_" REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string valueTableName = m[1];
        ValueTable & valueTable = network.valueTables[valueTableName];

        /* Name */
        valueTable.name = valueTableName;
        ValueDescriptions & valueDescriptions = valueTable.valueDescriptions;

        /* Value Description Pairs */
        std::istringstream iss(m[2]);
        while (iss.good()) {
            std::string value;
            iss >> value;
            std::string description;
            iss >> description;
            while (description.back() != '"') {
                std::string nextStr;
                iss >> nextStr;
                description += " ";
                description += nextStr;
            }
            description.erase(0, 1);
            if (description.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                description.pop_back();
            }
            valueDescriptions[stoul(value)] = description;
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedValueTable);
    }
}

/* Signals (SG) */
void File::readSignal(Network & network, Message & message, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "SG_" REGEX_SPACE REGEX_NAME "[[:space:]]*((m[[:digit:]]+)|M)?[[:space:]]*:[[:space:]]*" REGEX_UINT "\\|" REGEX_UINT "@([01])([+-])" REGEX_SPACE "\\(" REGEX_DOUBLE "," REGEX_DOUBLE "\\)" REGEX_SPACE "\\[" REGEX_DOUBLE "\\|" REGEX_DOUBLE "\\]" REGEX_SPACE REGEX_STRING REGEX_SPACE REGEX_TO_END REGEX_EOL);
    if (regex_search(line, m, re)) {
        std::string signalName = m[1];
        Signal & signal = message.signals[signalName];

        /* Name */
        signal.name = signalName;

        /* Multiplexed Signal */
        std::string temp = m[2];
        signal.multiplexedSignal = (!temp.empty()) && (temp[0] == 'm');

        /* Multiplexer Switch Value */
        std::string multiplexerSwitchValue = (temp.size() > 1) ? temp.substr(1) : "";
        if (multiplexerSwitchValue.empty()) {
            signal.multiplexerSwitchValue = 0;
        } else {
            signal.multiplexerSwitchValue = stoul(multiplexerSwitchValue);
        }

        /* Multiplexor Switch */
        signal.multiplexorSwitch = (!temp.empty()) && (temp[0] == 'M');

        /* Start Bit */
        signal.startBit = stoul(m[4]);

        /* Size */
        signal.bitSize = stoul(m[5]);

        /* Byte Order */
        std::string byteOrder = m[6];
        switch(byteOrder.front()) {
        case '0':
            signal.byteOrder = ByteOrder::BigEndian;
            break;
        case '1':
            signal.byteOrder = ByteOrder::LittleEndian;
            break;
        }

        /* Value Type */
        std::string valueType = m[7];
        switch(valueType.front()) {
        case '+':
            signal.valueType = ValueType::Unsigned;
            break;
        case '-':
            signal.valueType = ValueType::Signed;
            break;
        }

        /* Factor, Offset */
        signal.factor = stod(m[8]);
        signal.offset = stod(m[9]);

        /* Minimum and Maximum Physical Value */
        signal.minimumPhysicalValue = stod(m[10]);
        signal.maximumPhysicalValue = stod(m[11]);

        /* Unit */
        signal.unit = m[12];

        /* Receivers */
        std::istringstream iss(m[13]);
        while (iss.good()) {
            std::string node;
            iss >> node;
            if (node != "Vector__XXX") {
                signal.receivers.insert(node);
            }
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedSignal);
    }
}

/* Messages (BO) */
Message * File::readMessage(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BO_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_DELIM(":") REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_EOL);
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        Message & message = network.messages[messageId];

        /* Identifier */
        message.id = messageId;

        /* Name */
        message.name = m[2];

        /* Size */
        message.size = stoul(m[3]);

        /* Transmitter */
        std::string transmitter = m[4];
        if (transmitter != "Vector__XXX") {
            message.transmitter = transmitter;
        }

        return &message;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedMessage);
    }

    return nullptr;
}

/* Message Transmitters (BO_TX_BU) */
void File::readMessageTransmitter(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BO_TX_BU_" REGEX_SPACE REGEX_UINT REGEX_DELIM(":") REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        Message & message = network.messages[messageId];

        /* Message Identifier */
        message.id = messageId;

        /* Transmitters */
        std::istringstream iss(m[2]);
        while(iss.good()) {
            std::string transmitter;
            iss >> transmitter;
            message.transmitters.insert(transmitter);
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedMessageTransmitter);
    }
}

/* Environment Variables (EV) */
void File::readEnvironmentVariable(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "EV_" REGEX_SPACE REGEX_NAME REGEX_DELIM(":") "([01])" REGEX_SPACE "\\[" REGEX_DOUBLE "\\|" REGEX_DOUBLE "\\]" REGEX_SPACE REGEX_STRING REGEX_SPACE REGEX_DOUBLE REGEX_SPACE REGEX_UINT REGEX_SPACE "DUMMY_NODE_VECTOR([[:xdigit:]]+)" REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string envVarName = m[1];
        EnvironmentVariable & environmentVariable = network.environmentVariables[envVarName];

        /* Name */
        environmentVariable.name = envVarName;

        /* Type */
        std::string envVarType = m[2];
        switch(envVarType.front()) {
        case '0':
            environmentVariable.type = EnvironmentVariable::Type::Integer;
            break;
        case '1':
            environmentVariable.type = EnvironmentVariable::Type::Float;
            break;
        }

        /* Minimum, Maximum */
        environmentVariable.minimum = stod(m[3]);
        environmentVariable.maximum = stod(m[4]);

        /* Unit */
        environmentVariable.unit = m[5];

        /* Initial Value */
        environmentVariable.initialValue = stod(m[6]);

        /* Identifier */
        environmentVariable.id = stoul(m[7]);

        /* Access Type */
        unsigned int accessType = stoul(m[8], nullptr, 16);
        if (accessType & 0x8000) {
            accessType &= ~0x8000;
            environmentVariable.type = EnvironmentVariable::Type::String;
        }
        switch(accessType) {
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
        default:
            if (statusCallback != nullptr) {
                statusCallback(network, Status::MalformedEnvironmentVariable);
            }
        }

        /* Access Nodes */
        std::istringstream iss(m[9]);
        while(iss.good()) {
            std::string accessNode;
            std::getline(iss, accessNode, ',');
            if (accessNode != "Vector__XXX") {
                environmentVariable.accessNodes.insert(accessNode);
            }
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedEnvironmentVariable);
    }
}

/* Environment Variable Data (ENVVAR_DATA) */
void File::readEnvironmentVariableData(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "ENVVAR_DATA_" REGEX_SPACE REGEX_NAME REGEX_DELIM(":") REGEX_UINT REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string envVarName = m[1];
        EnvironmentVariable & environmentVariable = network.environmentVariables[envVarName];

        /* Name */
        environmentVariable.name = envVarName;

        /* Type */
        environmentVariable.type = EnvironmentVariable::Type::Data;

        /* Data Size */
        environmentVariable.dataSize = stoul(m[2]);
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedEnvironmentVariableData);
    }
}

/* Signal Types (SGTYPE, obsolete) */
void File::readSignalType(Network & network, std::string & line)
{
    // Signal Type
    smatch m;
    regex re(REGEX_SOL "SGTYPE_" REGEX_SPACE REGEX_NAME REGEX_DELIM(":") REGEX_UINT "@([01])([+-])" REGEX_SPACE "\\(" REGEX_DOUBLE "," REGEX_DOUBLE "\\)" REGEX_SPACE "\\[" REGEX_DOUBLE "\\|" REGEX_DOUBLE "\\]" REGEX_SPACE REGEX_STRING REGEX_SPACE REGEX_DOUBLE REGEX_DELIM(",") REGEX_NAME REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string signalTypeName = m[1];
        SignalType & signalType = network.signalTypes[signalTypeName];

        /* Name */
        signalType.name = signalTypeName;

        /* Size */
        signalType.size = stoul(m[2]);

        /* Byte Order */
        std::string byteOrder = m[3];
        switch(byteOrder.front()) {
        case '0':
            signalType.byteOrder = ByteOrder::BigEndian;
            break;
        case '1':
            signalType.byteOrder = ByteOrder::LittleEndian;
            break;
        }

        /* Value Type */
        std::string valueType = m[4];
        switch(valueType.front()) {
        case '+':
            signalType.valueType = ValueType::Unsigned;
            break;
        case '-':
            signalType.valueType = ValueType::Signed;
            break;
        }

        /* Factor, Offset */
        signalType.factor = stod(m[5]);
        signalType.offset = stod(m[6]);

        /* Minimum, Maximum */
        signalType.minimum = stod(m[7]);
        signalType.maximum = stod(m[8]);

        /* Unit */
        signalType.unit = m[9];

        /* Default Value */
        signalType.defaultValue = stod(m[10]);

        /* Value Table */
        signalType.valueTable = m[11];
        return;
    }

    // Signal Type Ref
    smatch mSTR;
    regex reSTR(REGEX_SOL "SGTYPE_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_DELIM(":") REGEX_NAME REGEX_EOL_DELIM);
    if (regex_search(line, mSTR, reSTR)) {
        unsigned int messageId = stoul(mSTR[1]);
        std::string signalName = mSTR[2];
        std::string signalTypeName = mSTR[3];
        network.messages[messageId].signals[signalName].type = signalTypeName;
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedSignalType);
    }
}

/* Comments (CM) for Networks */
bool File::readCommentNetwork(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "CM_" REGEX_SPACE REGEX_STRING REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string comment2 = m[1];
        while(!lineBreaks.empty()) {
            comment2.insert(lineBreaks.top(), endl);
            lineBreaks.pop();
        }

        /* replace \' with \" */
        for (std::size_t p = comment2.find('\\'); (p != std::string::npos) &&
             (p < comment2.size() - 1) &&
             (comment2[p+1] == '\''); p = comment2.find('\\', p+1)) {
            comment2[p+1] = '"';
        }

        network.comment = comment2;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) for Nodes (BU) */
bool File::readCommentNode(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "CM_" REGEX_SPACE "BU_" REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_STRING REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string nodeName = m[1];
        std::string comment = m[2];
        while(!lineBreaks.empty()) {
            comment.insert(lineBreaks.top(), endl);
            lineBreaks.pop();
        }

        /* replace \' with \" */
        for (std::size_t p = comment.find('\\'); (p != std::string::npos) &&
             (p < comment.size() - 1) &&
             (comment[p+1] == '\''); p = comment.find('\\', p+1)) {
            comment[p+1] = '"';
        }

        network.nodes[nodeName].comment = comment;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) for Messages (BO) */
bool File::readCommentMessage(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "CM_" REGEX_SPACE "BO_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_STRING REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string comment = m[2];
        while(!lineBreaks.empty()) {
            comment.insert(lineBreaks.top(), endl);
            lineBreaks.pop();
        }

        /* replace \' with \" */
        for (std::size_t p = comment.find('\\'); (p != std::string::npos) &&
             (p < comment.size() - 1) &&
             (comment[p+1] == '\''); p = comment.find('\\', p+1)) {
            comment[p+1] = '"';
        }

        network.messages[messageId].comment = comment;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) for Signals (SG) */
bool File::readCommentSignal(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "CM_" REGEX_SPACE "SG_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_STRING REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string signalName = m[2];
        std::string comment = m[3];
        while(!lineBreaks.empty()) {
            comment.insert(lineBreaks.top(), endl);
            lineBreaks.pop();
        }

        /* replace \' with \" */
        for (std::size_t p = comment.find('\\'); (p != std::string::npos) &&
             (p < comment.size() - 1) &&
             (comment[p+1] == '\''); p = comment.find('\\', p+1)) {
            comment[p+1] = '"';
        }

        network.messages[messageId].signals[signalName].comment = comment;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) for Environment Variables (EV) */
bool File::readCommentEnvironmentVariable(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "CM_" REGEX_SPACE "EV_" REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_STRING REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string envVarName = m[1];
        std::string comment = m[2];
        while(!lineBreaks.empty()) {
            comment.insert(lineBreaks.top(), endl);
            lineBreaks.pop();
        }

        /* replace \' with \" */
        for (std::size_t p = comment.find('\\'); (p != std::string::npos) &&
             (p < comment.size() - 1) &&
             (comment[p+1] == '\''); p = comment.find('\\', p+1)) {
            comment[p+1] = '"';
        }

        network.environmentVariables[envVarName].comment = comment;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) */
void File::readComment(Network & network, std::ifstream & ifs, std::string & line)
{
    /* support multi-line comments and escape sequences */
    std::size_t firstCommentCharPos = line.find('"');
    if (firstCommentCharPos == std::string::npos) {
        return;
    }
    std::size_t lastCommentCharPos = line.rfind('"');
    std::size_t eolPos = line.rfind(';');
    bool eol = (lastCommentCharPos > firstCommentCharPos) &&
            (line[lastCommentCharPos-1] != '\\') &&
            (eolPos != std::string::npos) &&
            (eolPos > lastCommentCharPos);
    firstCommentCharPos++;
    std::stack<std::size_t> lineBreaks;
    while (!eol) {
        std::string nextLine;
        std::getline(ifs, nextLine);
        chomp(nextLine);
        lineBreaks.push(line.size() - firstCommentCharPos);
        lastCommentCharPos = nextLine.rfind('"');
        eolPos = nextLine.rfind(';');
        eol = (lastCommentCharPos != std::string::npos) &&
                (nextLine[lastCommentCharPos-1] != '\\') &&
                (eolPos != std::string::npos) &&
                (eolPos > lastCommentCharPos);
        line += nextLine;
    }

    /* replace \" with \' */
    for (std::size_t p = line.find('\\', firstCommentCharPos);
         (p != std::string::npos) &&
         (p < line.size() - 1) &&
         (line[p+1] == '"'); p = line.find('\\', p+1)) {
        line[p+1] = '\'';
    }

    // for nodes (BU)
    if (readCommentNode(network, lineBreaks, line)) {
        return;
    }

    // for messages (BO)
    if (readCommentMessage(network, lineBreaks, line)) {
        return;
    }

    // for signals (SG)
    if (readCommentSignal(network, lineBreaks, line)) {
        return;
    }

    // for environment variables (EV)
    if (readCommentEnvironmentVariable(network, lineBreaks, line)) {
        return;
    }

    // for network
    if (readCommentNetwork(network, lineBreaks, line)) {
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedComment);
    }
}

/* Attribute Definitions (BA_DEF) */
void File::readAttributeDefinition(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BA_DEF_" REGEX_SPACE "(BU_|BO_|SG_|EV_)?[[:space:]]*" REGEX_ATTRIB_NAME REGEX_SPACE "(INT|HEX|FLOAT|STRING|ENUM)[[:space:]]*" REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string objectType = m[1];
        std::string attributeName = m[2];
        std::string attributeValueType = m[3];
        std::istringstream iss(m[4]);
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeName];
        attributeDefinition.name = attributeName;

        // for network
        if (objectType == "") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Network;
        } else
        // for node
        if (objectType == "BU_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Node;
        } else
        // for message
        if (objectType == "BO_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Message;
        } else
        // for signal
        if (objectType == "SG_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Signal;
        } else
        // for environment variable
        if (objectType == "EV_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::EnvironmentVariable;
        } else
        // format doesn't match
        if (statusCallback != nullptr) {
            statusCallback(network, Status::MalformedAttributeDefinition);
        }

        // for integer
        if (attributeValueType == "INT") {
            attributeDefinition.valueType = AttributeValueType::Int;
            std::string value;
            iss >> value;
            attributeDefinition.minimumIntegerValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumIntegerValue = stoul(value);
        } else
        // for hexadecimal
        if (attributeValueType == "HEX") {
            attributeDefinition.valueType = AttributeValueType::Hex;
            std::string value;
            iss >> value;
            attributeDefinition.minimumHexValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumHexValue = stoul(value);
        } else
        // for float
        if (attributeValueType == "FLOAT") {
            attributeDefinition.valueType = AttributeValueType::Float;
            std::string value;
            iss >> value;
            attributeDefinition.minimumFloatValue = stod(value);
            iss >> value;
            attributeDefinition.maximumFloatValue = stod(value);
        } else
        // for string
        if (attributeValueType == "STRING") {
            attributeDefinition.valueType = AttributeValueType::String;
        } else
        // for enumeration
        if (attributeValueType == "ENUM") {
            attributeDefinition.valueType = AttributeValueType::Enum;
            while(iss.good()) {
                std::string value;
                std::getline(iss, value, ',');
                value.erase(0, 1);
                if (value.empty() == false)
                {
                    // 2018-07-12 STW: added check for empty string before pop_back(),
                    //                 because of undefined behavior causing a crash
                    value.pop_back();
                }
                attributeDefinition.enumValues.push_back(value);
            }
        } else
        // format doesn't match
        if (statusCallback != nullptr) {
            statusCallback(network, Status::MalformedAttributeDefinition);
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedAttributeDefinition);
    }
}

/* Attribute Definitions at Relations (BA_DEF_REL) */
void File::readAttributeDefinitionRelation(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BA_DEF_REL_" REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string objectType = m[1];
        std::string attributeName = m[2];
        std::string attributeValueType = m[3];
        std::istringstream iss(m[4]);
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeName];
        attributeDefinition.name = attributeName;

        // Control Unit - Env. Variable
        if (objectType == "BU_EV_REL_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::ControlUnitEnvironmentVariable;
        } else

        // Node - Tx Message
        if (objectType == "BU_BO_REL_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::NodeTxMessage;
        } else

        // Node - Mapped Rx Signal
        if (objectType == "BU_SG_REL_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::NodeMappedRxSignal;
        } else

        // format doesn't match
        if (statusCallback != nullptr) {
            statusCallback(network, Status::MalformedAttributeDefinitionRelation);
        }

        // Integer
        if (attributeValueType == "INT") {
            attributeDefinition.valueType = AttributeValueType::Int;
            std::string value;
            iss >> value;
            attributeDefinition.minimumIntegerValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumIntegerValue = stoul(value);
        } else

        // Hexadecimal
        if (attributeValueType == "HEX") {
            attributeDefinition.valueType = AttributeValueType::Hex;
            std::string value;
            iss >> value;
            attributeDefinition.minimumHexValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumHexValue = stoul(value);
        } else

        // Float
        if (attributeValueType == "FLOAT") {
            attributeDefinition.valueType = AttributeValueType::Float;
            std::string value;
            iss >> value;
            attributeDefinition.minimumFloatValue = stod(value);
            iss >> value;
            attributeDefinition.maximumFloatValue = stod(value);
        } else

        // String
        if (attributeValueType == "STRING") {
            attributeDefinition.valueType = AttributeValueType::String;
        } else

        // Enumeration
        if (attributeValueType == "ENUM") {
            attributeDefinition.valueType = AttributeValueType::Enum;
            while(iss.good()) {
                std::string value;
                std::getline(iss, value, ',');
                value.erase(0, 1);
                if (value.empty() == false)
                {
                    // 2018-07-12 STW: added check for empty string before pop_back(),
                    //                 because of undefined behavior causing a crash
                    value.pop_back();
                }
                attributeDefinition.enumValues.push_back(value);
            }
        } else

        // format doesn't match
        if (statusCallback != nullptr) {
            statusCallback(network, Status::MalformedAttributeDefinitionRelation);
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedAttributeDefinitionRelation);
    }
}

/* Sigtype Attr Lists (?, obsolete) */

/* Attribute Defaults (BA_DEF_DEF) */
void File::readAttributeDefault(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BA_DEF_DEF_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        std::string attributeValue = m[2];
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeName];
        Attribute & attributeDefault = network.attributeDefaults[attributeName];

        /* Name */
        attributeDefault.name = attributeName;

        /* Value Type */
        attributeDefault.valueType = attributeDefinition.valueType;

        /* Value */
        attributeDefault.stringValue = attributeValue;
        switch(attributeDefinition.valueType) {

        // Integer
        case AttributeValueType::Int:
            attributeDefault.integerValue = stoul(attributeValue);
            break;

        // Hexadecimal
        case AttributeValueType::Hex:
            attributeDefault.hexValue = stoul(attributeValue);
            break;

        // Float
        case AttributeValueType::Float:
            attributeDefault.floatValue = stod(attributeValue);
            break;

        // String
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attributeDefault.stringValue = attributeValue;
            break;

        // Enumeration
        case AttributeValueType::Enum:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attributeDefault.stringValue = attributeValue;
            break;
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedAttributeDefault);
    }
}

/* Attribute Defaults at Relations (BA_DEF_DEF_REL) */
void File::readAttributeDefaultRelation(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BA_DEF_DEF_REL_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        std::string attributeValue = m[2];
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeName];
        Attribute & attributeDefault = network.attributeDefaults[attributeName];

        /* Name */
        attributeDefault.name = attributeName;

        /* Value Type */
        attributeDefault.valueType = attributeDefinition.valueType;

        /* Value */
        attributeDefault.stringValue = attributeValue;
        switch(attributeDefinition.valueType) {

        // Integer
        case AttributeValueType::Int:
            attributeDefault.integerValue = stoul(attributeValue);
            break;

        // Hexadecimal
        case AttributeValueType::Hex:
            attributeDefault.hexValue = stoul(attributeValue);
            break;

        // Float
        case AttributeValueType::Float:
            attributeDefault.floatValue = stod(attributeValue);
            break;

        // String
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attributeDefault.stringValue = attributeValue;
            break;

        // Enumeration
        case AttributeValueType::Enum:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attributeDefault.stringValue = attributeValue;
            break;
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedAttributeDefaultRelation);
    }
}

/* Attribute Values (BA) for Network */
bool File::readAttributeValueNetwork(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BA_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        std::string attributeValue = m[2];
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeName];
        Attribute & attribute = network.attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {

        // Integer
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;

        // Hexadecimal
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;

        // Float
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;

        // String
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attribute.stringValue = attributeValue;
            break;

        // Enumeration
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Attribute Values (BA) for Node (BU) */
bool File::readAttributeValueNode(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BA_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE "BU_" REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        std::string nodeName = m[2];
        std::string attributeValue = m[3];
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeName];
        attributeDefinition.name = attributeName;
        Node & node = network.nodes[nodeName];
        node.name = nodeName;
        Attribute & attribute = node.attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {
        // Integer
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;

        // Hexadecimal
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;

        // Float
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;

        // String
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attribute.stringValue = attributeValue;
            break;

        // Enumeration
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Attribute Values (BA) for Message (BO) */
bool File::readAttributeValueMessage(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BA_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE "BO_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        unsigned int messageId = stoul(m[2]);
        std::string attributeValue = m[3];
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeName];
        Attribute & attribute = network.messages[messageId].attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {

        // Integer
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;

        // Hexadecimal
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;

        // Float
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;

        // String
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attribute.stringValue = attributeValue;
            break;

        // Enumeration
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Attribute Values (BA) for Signal (SG) */
bool File::readAttributeValueSignal(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BA_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE "SG_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        unsigned int messageId = stoul(m[2]);
        std::string signalName = m[3];
        std::string attributeValue = m[4];
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeName];
        Attribute & attribute = network.messages[messageId].signals[signalName].attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {

        // Integer
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;

        // Hexadecimal
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;

        // Float
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;

        // String
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attribute.stringValue = attributeValue;
            break;

        // Enumeration
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Attribute Values (BA) for Environment Variable (EV) */
bool File::readAttributeValueEnvironmentVariable(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "BA_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE "EV_" REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        std::string envVarName = m[2];
        std::string attributeValue = m[3];
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeName];
        Attribute & attribute = network.environmentVariables[envVarName].attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {

        // Integer
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;

        // Hexadecimal
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;

        // Float
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;

        // String
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attribute.stringValue = attributeValue;
            break;

        // Enumeration
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Attribute Values (BA) */
void File::readAttributeValue(Network & network, std::string & line)
{
    // for nodes (BU)
    if (readAttributeValueNode(network, line)) {
        return;
    }

    // for messages (BO)
    if (readAttributeValueMessage(network, line)) {
        return;
    }

    // for signals (SG)
    if (readAttributeValueSignal(network, line)) {
        return;
    }

    // for environment variables (EV)
    if (readAttributeValueEnvironmentVariable(network, line)) {
        return;
    }

    // for network
    if (readAttributeValueNetwork(network, line)) {
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedAttributeValue);
    }
}

/* Attribute Values at Relations (BA_REL) */
void File::readAttributeRelationValue(Network & network, std::string & line)
{
    bool found = false;
    AttributeRelation attributeRelation;
    std::string attributeValue;

    // for relation "Control Unit - Env. Variable" (BU_EV_REL)
    smatch mBU_EV_REL;
    regex reBU_EV_REL(REGEX_SOL "BA_REL_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE "BU_EV_REL_" REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (!found && regex_search(line, mBU_EV_REL, reBU_EV_REL)) {
        attributeRelation.relationType = AttributeRelation::RelationType::ControlUnitEnvironmentVariable;
        attributeRelation.name = mBU_EV_REL[1];
        attributeRelation.nodeName = mBU_EV_REL[2];
        attributeRelation.environmentVariableName = mBU_EV_REL[3];
        attributeValue = mBU_EV_REL[4];
        found = true;
    }

    // for relation "Node - Tx Message" (BU_BO_REL)
    smatch mBU_BO_REL;
    regex reBU_BO_REL(REGEX_SOL "BA_REL_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE "BU_BO_REL_" REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (!found && regex_search(line, mBU_BO_REL, reBU_BO_REL)) {
        attributeRelation.relationType = AttributeRelation::RelationType::NodeTxMessage;
        attributeRelation.name = mBU_BO_REL[1];
        attributeRelation.nodeName = mBU_BO_REL[2];
        attributeRelation.messageId = stoul(mBU_BO_REL[3]);
        attributeValue = mBU_BO_REL[4];
        found = true;
    }

    // for relation "Node - Mapped Rx Signal" (BU_SG_REL)
    smatch mBU_SG_REL;
    regex reBU_SG_REL(REGEX_SOL "BA_REL_" REGEX_SPACE REGEX_ATTRIB_NAME REGEX_SPACE "BU_SG_REL_" REGEX_SPACE REGEX_NAME REGEX_SPACE "SG_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (!found && regex_search(line, mBU_SG_REL, reBU_SG_REL)) {
        attributeRelation.relationType = AttributeRelation::RelationType::NodeMappedRxSignal;
        attributeRelation.name = mBU_SG_REL[1];
        attributeRelation.nodeName = mBU_SG_REL[2];
        attributeRelation.messageId = stoul(mBU_SG_REL[3]);
        attributeRelation.signalName = mBU_SG_REL[4];
        attributeValue = mBU_SG_REL[5];
        found = true;
    }

    if (found) {
        AttributeDefinition & attributeDefinition = network.attributeDefinitions[attributeRelation.name];
        attributeRelation.valueType = attributeDefinition.valueType;
        switch(attributeRelation.valueType) {
        // Integer
        case AttributeValueType::Int:
            attributeRelation.integerValue = stoul(attributeValue);
            break;

        // Hexadecimal
        case AttributeValueType::Hex:
            attributeRelation.hexValue = stoul(attributeValue);
            break;

        // Float
        case AttributeValueType::Float:
            attributeRelation.floatValue = stod(attributeValue);
            break;

        // String
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            if (attributeValue.empty() == false)
            {
                // 2018-07-12 STW: added check for empty string before pop_back(),
                //                 because of undefined behavior causing a crash
                attributeValue.pop_back();
            }
            attributeRelation.stringValue = attributeValue;
            break;

        // Enumeration
        case AttributeValueType::Enum:
            attributeRelation.enumValue = stoul(attributeValue);
            break;
        }
        network.attributeRelationValues.insert(attributeRelation);
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedAttributeRelationValue);
    }
}

/* Value Descriptions (VAL) for Signals (SG) */
bool File::readValueDescriptionSignal(Network & network, std::string & line)
{
   // 2018-09-20 STW: after discussion with colleagues we replace the following code lines.
   //                 Reason is a known stack overflow problem with the std::regex function
   //                 https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70411
   //                 https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61582
   //                 The regex of this library for 'VAL_' is rather greedy and taking each word of
   //                 the signal value description. At long lines we get a segmentation fault message
   //                 in debug mode and a tool crash in release mode.
   //                 We can go around this problem if we increase the stack size to e.g. 16 MB
   //                 (default 2 MB, see http://sourceware.org/binutils/docs-2.16/ld/Options.html).
   //                 To increase the stack size put the following command into .pro file:
   //                 QMAKE_LFLAGS_WINDOWS += -Wl,--stack,16777216
   //                 Instead of increasing the stack size we decided to replace the code for signal values,
   //                 because we get a better performance and we do not suffer from side effects.
   //                 A side effect is if you increase the stack size to 134217728 Bytes, the
   //                 QFileDialog can only be opened once since program start. This issue is not studied in detail.
   //                 Another way is to use QRegularExpressions. But this is not an easy task because the DBC library
   //                 uses also partial matches.
   //                 At the moment the most appropriate way seems to replace the implementation by using simple
   //                 string operations.
   //                 Hint: the statusCallback is not used. If it is used you must take care of it.
   // 2019-02-22 STW: Adapted our replacement and added some defensive checks for std::string::erase in combination
   //                 with finding a specific character. No ";" in a VAL_ line led to crashes. This issue occured on
   //                 files with line breaks between different values. Parsing such files will still not work correctly,
   //                 because at the moment only the first line is considered.

   const char * pcn_Trim = " \t\r\n\v\f";
   std::string c_SearchLine = line;
   bool q_Return = false;
   size_t un_Pos;

   un_Pos = c_SearchLine.find_first_not_of(pcn_Trim);

   if (un_Pos < c_SearchLine.size())
   {
      c_SearchLine.erase(un_Pos);
   }

   if (c_SearchLine.substr(0, 4) == "VAL_")
   {
      unsigned int un_MessageId;
      std::string c_SignalName;

      q_Return = true;
      // Trim right and remove line ending of value description
      un_Pos = c_SearchLine.find_first_of(';');
      if (un_Pos < c_SearchLine.size())
      {
         c_SearchLine.erase(un_Pos);
      }
      c_SearchLine.erase(c_SearchLine.find_last_not_of(pcn_Trim) + 1);

      // Remove the VAL_
      c_SearchLine.erase(0, 4);

      // Get the id
      c_SearchLine.erase(0, c_SearchLine.find_first_not_of(pcn_Trim));
      un_Pos = c_SearchLine.find_first_of(pcn_Trim);

      try
      {
         un_MessageId = stoul(c_SearchLine.substr(0, un_Pos));
      }
      catch (...)
      {
         q_Return = false;
      }

      if (q_Return == true)
      {
         c_SearchLine.erase(0, un_Pos);

         // Get the name
         c_SearchLine.erase(0, c_SearchLine.find_first_not_of(pcn_Trim));
         un_Pos = c_SearchLine.find_first_of(pcn_Trim);
         c_SignalName = c_SearchLine.substr(0, un_Pos);
         c_SearchLine.erase(0, un_Pos);

         c_SearchLine.erase(0, c_SearchLine.find_first_not_of(pcn_Trim));

         if (c_SearchLine.empty() == false)
         {
            ValueDescriptions & c_ValueDescriptions =
               network.messages[un_MessageId].signals[c_SignalName].valueDescriptions;
            unsigned int un_Value;

            do
            {
               // Get the value
               un_Pos = c_SearchLine.find_first_of(pcn_Trim);
               un_Value = stoul(c_SearchLine.substr(0, un_Pos));

               // Get the description
               un_Pos = c_SearchLine.find_first_of('"');

               if (un_Pos != std::string::npos)
               {
                  const size_t un_RightPos = c_SearchLine.find_first_of('"', un_Pos + 1);
                  if (un_RightPos != std::string::npos)
                  {
                     std::string c_Description = c_SearchLine.substr(un_Pos + 1, (un_RightPos - un_Pos) - 1);

                     // Assign value description
                     c_ValueDescriptions[un_Value] = c_Description;

                     // Erase processed part
                     c_SearchLine.erase(0, un_RightPos + 1);
                     c_SearchLine.erase(0, c_SearchLine.find_first_not_of(pcn_Trim));
                  }
                  else
                  {
                     c_SearchLine = "";
                  }
               }
               else
               {
                  c_SearchLine = "";
               }
            } while (c_SearchLine.empty() == false);
         }
      }
   }

   return q_Return;

    // Original code
    //
    // smatch m;
    // regex re(REGEX_SOL "VAL_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    //
    // if (regex_search(line, m, re)) {
    //     unsigned int messageId = stoul(m[1]);
    //     std::string signalName = m[2];
    //     ValueDescriptions & valueDescriptions = network.messages[messageId].signals[signalName].valueDescriptions;
    //
    //     /* Value Description Pairs */
    //     std::string s = m[3];
    //     while(!s.empty()) {
    //         /* value */
    //         unsigned int value = stoul(s);
    //
    //         /* description */
    //         std::size_t leftPos = s.find_first_of('"');
    //         if (leftPos == std::string::npos)
    //             break;
    //         std::size_t rightPos = s.find_first_of('"', leftPos + 1);
    //         if (rightPos == std::string::npos)
    //             break;
    //         std::string description = s.substr(leftPos + 1, rightPos - leftPos - 1);
    //
    //         /* erase processed part */
    //         s.erase(0, rightPos + 2);
    //
    //         /* assign value description */
    //         valueDescriptions[value] = description;
    //     }
    //     return true;
    // }
    //
    // /* format doesn't match */
    // return false;
}

/* Value Descriptions (VAL) for Environment Variables (EV) */
bool File::readValueDescriptionEnvironmentVariable(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "VAL_" REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        std::string envVarName = m[1];
        ValueDescriptions & valueDescriptions = network.environmentVariables[envVarName].valueDescriptions;

        /* Value Description Pairs */
        std::string s = m[2];
        while(!s.empty()) {
            /* value */
            unsigned int value = stoul(s);

            /* description */
            std::size_t leftPos = s.find_first_of('"');
            if (leftPos == std::string::npos)
                break;
            std::size_t rightPos = s.find_first_of('"', leftPos + 1);
            if (rightPos == std::string::npos)
                break;
            std::string description = s.substr(leftPos + 1, rightPos - leftPos - 1);

            /* erase processed part */
            s.erase(0, rightPos + 2);

            /* assign value description */
            valueDescriptions[value] = description;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Value Descriptions (VAL) */
void File::readValueDescription(Network & network, std::string & line)
{
    // for signal
    if (readValueDescriptionSignal(network, line)) {
        return;
    }

    // for environment variable
    if (readValueDescriptionEnvironmentVariable(network, line)) {
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedValueDescription);
    }
}

/* Category Definitions (?, obsolete) */

/* Categories (?, obsolete) */

/* Filters (?, obsolete) */

/* Signal Type Refs (SGTYPE, obsolete) */
// see above readSignalType

/* Signal Groups (SIG_GROUP) */
void File::readSignalGroup(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "SIG_GROUP_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_UINT REGEX_DELIM(":") REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string signalGroupName = m[2];
        SignalGroup & signalGroup = network.messages[messageId].signalGroups[signalGroupName];

        /* Message Identifier */
        signalGroup.messageId = messageId;

        /* Name */
        signalGroup.name = signalGroupName;

        /* Repetitions */
        signalGroup.repetitions = stoul(m[3]);

        /* Signals */
        std::istringstream iss(m[4]);
        while (iss.good()) {
            std::string signalName;
            iss >> signalName;
            signalGroup.signals.insert(signalName);
        }
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedSignalGroup);
    }
}

/* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
void File::readSignalExtendedValueType(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "SIG_VALTYPE_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_DELIM(":") "([012])" REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string signalName = m[2];

        /* Extended Value Type */
        unsigned int signalExtendedValueType = stoul(m[3]);
        switch(signalExtendedValueType) {
        // Integer
        case 0:
            network.messages[messageId].signals[signalName].extendedValueType = Signal::ExtendedValueType::Integer;
            break;
        // Float
        case 1:
            network.messages[messageId].signals[signalName].extendedValueType = Signal::ExtendedValueType::Float;
            break;
        // Double
        case 2:
            network.messages[messageId].signals[signalName].extendedValueType = Signal::ExtendedValueType::Double;
            break;
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedSignalExtendedValueType);
    }
}

/* Extended Multiplexors (SG_MUL_VAL) */
void File::readExtendedMultiplexor(Network & network, std::string & line)
{
    smatch m;
    regex re(REGEX_SOL "SG_MUL_VAL_" REGEX_SPACE REGEX_UINT REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_NAME REGEX_SPACE REGEX_TO_END REGEX_EOL_DELIM);
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string multiplexedSignalName = m[2];
        std::string switchName = m[3];
        ExtendedMultiplexor & extendedMultiplexor = network.messages[messageId].signals[multiplexedSignalName].extendedMultiplexors[switchName];

        /* Switch Name */
        extendedMultiplexor.switchName = switchName;

        /* Multiplexor Value Range */
        std::istringstream iss(m[4]);
        while(iss.good()) {
            std::string multiplexorValueRange;
            std::getline(iss, multiplexorValueRange, ',');

            std::istringstream iss2(multiplexorValueRange);
            std::string minValue;
            std::getline(iss2, minValue, '-');
            std::string maxValue;
            std::getline(iss2, maxValue, '-');
            ExtendedMultiplexor::ValueRange valueRange;
            valueRange.first = stoul(minValue);
            valueRange.second = stoul(maxValue);
            extendedMultiplexor.valueRanges.insert(valueRange);
        }
        return;
    }

    /* format doesn't match */
    if (statusCallback != nullptr) {
        statusCallback(network, Status::MalformedExtendedMultiplexor);
    }
}

Status File::load(Network & network, const char * filename)
{
    std::ifstream ifs;
    std::streampos fileSize;

    /* in case the line is previous line was a message or signal, messageContext is set to the message */
    Message * messageContext = nullptr;

    /* open stream */
    ifs.open(filename, std::ifstream::in);
    if (!ifs.is_open()) {
        return Status::FileOpenError;
    }

    /* use english decimal points for floating numbers */
    ifs.imbue(std::locale("C"));

    /* get file size */
    ifs.seekg(0, std::ifstream::end);
    fileSize = ifs.tellg();
    ifs.seekg(0);

    /* parse stream */
    while(ifs.good()) {
        /* call progress function */
        if (progressCallback != nullptr) {
            progressCallback(network, ifs.tellg(), fileSize);
        }

        /* get one line */
        std::string line;
        std::getline(ifs, line);
        chomp(line);

        /* parse line */
        smatch m;
        regex re(REGEX_SOL REGEX_NAME);
        if ((!line.empty()) && (regex_search(line, m, re))) {
            std::string name = m[1];
            Message * newMessageContext = nullptr;

            /* Version (VERSION) */
            if (name == "VERSION") {
                readVersion(network, line);
            } else

            /* New Symbols (NS) */
            if (name == "NS_") {
                readNewSymbols(network, ifs, line);
            } else

            /* Bit Timing (BS) */
            if (name == "BS_") {
                readBitTiming(network, line);
            } else

            /* Nodes (BU) */
            if (name == "BU_") {
                readNodes(network, line);
            } else

            /* Value Tables (VAL_TABLE) */
            if (name == "VAL_TABLE_") {
                readValueTable(network, line);
            } else

            /* Messages (BO) */
            if (name == "BO_") {
                newMessageContext = readMessage(network, line);
            } else

            /* Signal (SG) */
            if (name == "SG_") {
                if (messageContext) {
                    readSignal(network, *messageContext, line);

                    /* remember that we are still in context of the same message */
                    newMessageContext = messageContext;
                } else {
                    statusCallback(network, Status::SignalWithoutMessage);
                }
            } else

            /* Message Transmitters (BO_TX_BU) */
            if (name == "BO_TX_BU_") {
                readMessageTransmitter(network, line);
            } else

            /* Environment Variables (EV) */
            if (name == "EV_") {
                readEnvironmentVariable(network, line);
            } else

            /* Environment Variable Data (ENVVAR_DATA) */
            if (name == "ENVVAR_DATA_") {
                readEnvironmentVariableData(network, line);
            } else

            /* Signal Types (SGTYPE, obsolete) */
            if (name == "SGTYPE_") {
                readSignalType(network, line);
            } else

            /* Comments (CM) */
            if (name == "CM_") {
                readComment(network, ifs, line);
            } else

            /* Attribute Definitions (BA_DEF) */
            if (name == "BA_DEF_") {
                readAttributeDefinition(network, line);
            } else

            /* Attribute Definitions at Relations (BA_DEF_REL) */
            if (name == "BA_DEF_REL_") {
                readAttributeDefinitionRelation(network, line);
            } else

            /* Sigtype Attr Lists (?, obsolete) */

            /* Attribute Defaults (BA_DEF_DEF) */
            if (name == "BA_DEF_DEF_") {
                readAttributeDefault(network, line);
            } else

            /* Attribute Defaults at Relations (BA_DEF_DEF_REL) */
            if (name == "BA_DEF_DEF_REL_") {
                readAttributeDefaultRelation(network, line);
            } else

            /* Attribute Values (BA) */
            if (name == "BA_") {
                readAttributeValue(network, line);
            } else

            /* Attribute Values at Relations (BA_REL) */
            if (name == "BA_REL_") {
                readAttributeRelationValue(network, line);
            } else

            /* Value Descriptions (VAL) */
            if (name == "VAL_") {
                readValueDescription(network, line);
            } else

            /* Category Definitions (CAT_DEF, obsolete) */

            /* Categories (CAT, obsolete) */

            /* Filters (FILTER, obsolete) */

            /* Signal Type Refs (SGTYPE, obsolete) */
            // see above readSignalType

            /* Signal Groups (SIG_GROUP) */
            if (name == "SIG_GROUP_") {
                readSignalGroup(network, line);
            } else

            /* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
            if (name == "SIG_VALTYPE_") {
                readSignalExtendedValueType(network, line);
            } else

            /* Extended Multiplexors (SG_MUL_VAL) */
            if (name == "SG_MUL_VAL_") {
                readExtendedMultiplexor(network, line);
            } else

            /* not supported */
            if (statusCallback != nullptr) {
                statusCallback(network, Status::Unknown);
            }

            /* remember the message context (set if the current line was a message or signal) */
            messageContext = newMessageContext;
        }
    }

    /* close stream */
    ifs.close();
    return Status::Ok;
}

Status File::load(Network & network, std::string & filename)
{
    return load(network, filename.c_str());
}

}
}
