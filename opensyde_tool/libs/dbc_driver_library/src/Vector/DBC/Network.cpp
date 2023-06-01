/*
 * Copyright (C) 2013-2019 Tobias Lorenz.
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
 */

#include <Vector/DBC/Network.h>

#include <Vector/DBC/Parser.hpp>
#include <Vector/DBC/Scanner.h>

namespace Vector {
namespace DBC {

std::ostream & operator<<(std::ostream & os, const Network & network) {
    /* use english decimal points for floating numbers */
    os.imbue(std::locale("C"));

    os.precision(16);

    /* Version (VERSION) */
    os << "VERSION \"" << network.version << "\"" << endl;
    os << endl;

    /* New Symbols (NS) */
    os << endl;
    os << "NS_ : " << endl;
    for (const auto & newSymbol : network.newSymbols)
        os << "\t" << newSymbol << endl;
    os << endl;

    /* Bit Timing (BS) */
    os << network.bitTiming;
    os << endl;

    /* Nodes (BU) */
    os << "BU_:";
    if (network.nodes.empty())
        os << " Vector__XXX";
    else
        for (const auto & node : network.nodes)
            os << " " << node.second.name;
    os << endl;

    /* Value Tables (VAL_TABLE) */
    for (const auto & valueTable : network.valueTables)
        os << valueTable.second;
    os << endl;
    os << endl;

    /* Messages (BO) */
    for (const auto & message : network.messages)
        os << message.second;

    /* Message Transmitters (BO_TX_BU) */
    for (const auto & message : network.messages) {
        if (!message.second.transmitters.empty()) {
            os << "BO_TX_BU_ " << message.second.id << " :";
            bool first = true;
            for (const auto & transmitter : message.second.transmitters) {
                if (first)
                    first = false;
                else
                    os << ',';
                os << transmitter;
            }
            os << ';' << endl;
        }
    }
    os << endl;

    /* Environment Variables (EV) */
    for (const auto & environmentVariable : network.environmentVariables) {
        os << endl;
        os << environmentVariable.second;
    }

    /* Environment Variable Data (ENVVAR_DATA) */
    for (const auto & environmentVariable : network.environmentVariables) {
        if (environmentVariable.second.type == EnvironmentVariable::Type::Data) {
            os << "ENVVAR_DATA_ " << environmentVariable.second.name;
            os << ": " << environmentVariable.second.dataSize;
            os << ";" << endl;
        }
    }
    os << endl; // this might go below SGTYPE

    /* Signal Types (SGTYPE, obsolete) */
    // @todo writeSignalTypes(ofs, network);

    /* Comments (CM) */
    if (!network.comment.empty())
        os << "CM_ \"" << network.comment << "\";" << endl;
    for (const auto & node : network.nodes) {
        if (!node.second.comment.empty())
            os << "CM_ BU_ " << node.second.name << " \"" << node.second.comment << "\";" << endl;
    }
    for (const auto & message : network.messages) {
        if (!message.second.comment.empty())
            os << "CM_ BO_ " << message.second.id << " \"" << message.second.comment << "\";" << endl;
    }
    for (const auto & message : network.messages) {
        for (const auto & signal : message.second.signals) {
            if (!signal.second.comment.empty())
                os << "CM_ SG_ " << message.second.id << ' ' << signal.second.name << " \"" << signal.second.comment << "\";" << endl;
        }
    }
    for (const auto & environmentVariable : network.environmentVariables) {
        if (!environmentVariable.second.comment.empty())
            os << "CM_ EV_ " << environmentVariable.second.name << " \"" << environmentVariable.second.comment << "\";" << endl;
    }

    /* Attribute Definitions (BA_DEF) and Attribute Definitions at Relations (BA_DEF_REL) */
    for (const auto & attributeDefinition : network.attributeDefinitions)
        os << attributeDefinition.second;

    /* Sigtype Attr Lists (?, obsolete) */

    /* Attribute Defaults (BA_DEF_DEF) and Attribute Defaults at Relations (BA_DEF_DEF_REL) */
    for (const auto & attributeDefault : network.attributeDefaults) {
        const AttributeDefinition & attributeDefinition = network.attributeDefinitions.at(attributeDefault.second.name);

        /* Object Type */
        switch (attributeDefinition.objectType) {
        case AttributeObjectType::Network:
        case AttributeObjectType::Node:
        case AttributeObjectType::Message:
        case AttributeObjectType::Signal:
        case AttributeObjectType::EnvironmentVariable:
            os << "BA_DEF_DEF_ ";
            break;
        case AttributeObjectType::ControlUnitEnvironmentVariable:
        case AttributeObjectType::NodeTxMessage:
        case AttributeObjectType::NodeMappedRxSignal:
            os << "BA_DEF_DEF_REL_";
            break;
        }

        /* Name */
        os << " \"" << attributeDefault.second.name << "\" ";

        /* Value */
        switch (attributeDefinition.valueType.type) {
        case AttributeValueType::Type::Int:
            os << attributeDefault.second.integerValue;
            break;
        case AttributeValueType::Type::Hex:
            os << attributeDefault.second.hexValue;
            break;
        case AttributeValueType::Type::Float:
            os << attributeDefault.second.floatValue;
            break;
        case AttributeValueType::Type::String:
            os << '"' << attributeDefault.second.stringValue << '"';
            break;
        case AttributeValueType::Type::Enum:
            os << '"' << attributeDefault.second.stringValue << '"';
            break;
        }
        os << ';' << endl;
    }

    /* Attribute Values (BA) */
    for (const auto & attributeValue : network.attributeValues) {
        const AttributeDefinition & attributeDefinition = network.attributeDefinitions.at(attributeValue.second.name);

        /* Name */
        os << "BA_ \"" << attributeValue.second.name << "\" ";

        /* Value */
        switch (attributeDefinition.valueType.type) {
        case AttributeValueType::Type::Int:
            os << attributeValue.second.integerValue;
            break;
        case AttributeValueType::Type::Hex:
            os << attributeValue.second.hexValue;
            break;
        case AttributeValueType::Type::Float:
            os << attributeValue.second.floatValue;
            break;
        case AttributeValueType::Type::String:
            os << '"' << attributeValue.second.stringValue << '"';
            break;
        case AttributeValueType::Type::Enum:
            os << attributeValue.second.enumValue;
            break;
        }
        os << ';' << endl;
    }
    for (const auto & node : network.nodes) {
        for (const auto & attributeValue : node.second.attributeValues) {
            const AttributeDefinition & attributeDefinition = network.attributeDefinitions.at(attributeValue.second.name);

            /* Name */
            os << "BA_ \"" << attributeValue.second.name << "\" ";

            /* Node Name */
            os << "BU_ " << node.second.name << ' ';

            /* Value */
            switch (attributeDefinition.valueType.type) {
            case AttributeValueType::Type::Int:
                os << attributeValue.second.integerValue;
                break;
            case AttributeValueType::Type::Hex:
                os << attributeValue.second.hexValue;
                break;
            case AttributeValueType::Type::Float:
                os << attributeValue.second.floatValue;
                break;
            case AttributeValueType::Type::String:
                os << '"' << attributeValue.second.stringValue << '"';
                break;
            case AttributeValueType::Type::Enum:
                os << attributeValue.second.enumValue;
                break;
            }
            os << ';' << endl;
        }
    }
    for (const auto & message : network.messages) {
        for (const auto & attributeValue : message.second.attributeValues) {
            const AttributeDefinition & attributeDefinition = network.attributeDefinitions.at(attributeValue.second.name);

            /* Name */
            os << "BA_ \"" << attributeValue.second.name << "\" ";

            /* Message Name */
            os << "BO_ " << message.second.id << ' ';

            /* Value */
            switch (attributeDefinition.valueType.type) {
            case AttributeValueType::Type::Int:
                os << attributeValue.second.integerValue;
                break;
            case AttributeValueType::Type::Hex:
                os << attributeValue.second.hexValue;
                break;
            case AttributeValueType::Type::Float:
                os << attributeValue.second.floatValue;
                break;
            case AttributeValueType::Type::String:
                os << '"' << attributeValue.second.stringValue << '"';
                break;
            case AttributeValueType::Type::Enum:
                os << attributeValue.second.enumValue;
                break;
            }
            os << ';' << endl;
        }
    }
    for (const auto & message : network.messages) {
        for (const auto & signal : message.second.signals) {
            for (const auto & attributeValue : signal.second.attributeValues) {
                const AttributeDefinition & attributeDefinition = network.attributeDefinitions.at(attributeValue.second.name);

                /* Name */
                os << "BA_ \"" << attributeValue.second.name << "\" ";

                /* Message Identifier, Signal Name */
                os << "SG_ " << message.second.id << ' ' << signal.second.name << ' ';

                /* Value */
                switch (attributeDefinition.valueType.type) {
                case AttributeValueType::Type::Int:
                    os << attributeValue.second.integerValue;
                    break;
                case AttributeValueType::Type::Hex:
                    os << attributeValue.second.hexValue;
                    break;
                case AttributeValueType::Type::Float:
                    os << attributeValue.second.floatValue;
                    break;
                case AttributeValueType::Type::String:
                    os << '"' << attributeValue.second.stringValue << '"';
                    break;
                case AttributeValueType::Type::Enum:
                    os << attributeValue.second.enumValue;
                    break;
                }
                os << ';' << endl;
            }
        }
    }
    for (const auto & environmentVariable : network.environmentVariables) {
        for (const auto & attributeValue : environmentVariable.second.attributeValues) {
            const AttributeDefinition & attributeDefinition = network.attributeDefinitions.at(attributeValue.second.name);

            /* Name */
            os << "BA_ \"" << attributeValue.second.name << "\" ";

            /* Environment Variable Name */
            os << "EV_ " << environmentVariable.second.name << ' ';

            /* Value */
            switch (attributeDefinition.valueType.type) {
            case AttributeValueType::Type::Int:
                os << attributeValue.second.integerValue;
                break;
            case AttributeValueType::Type::Hex:
                os << attributeValue.second.hexValue;
                break;
            case AttributeValueType::Type::Float:
                os << attributeValue.second.floatValue;
                break;
            case AttributeValueType::Type::String:
                os << '"' << attributeValue.second.stringValue << '"';
                break;
            case AttributeValueType::Type::Enum:
                os << attributeValue.second.enumValue;
                break;
            }
            os << ';' << endl;
        }
    }

    /* Attribute Values at Relations (BA_REL) */
    for (const auto & attributeRelationValue : network.attributeRelationValues) {
        const AttributeDefinition & attributeDefinition = network.attributeDefinitions.at(attributeRelationValue.second.name);

        /* Name */
        os << "BA_REL_ \"" << attributeRelationValue.second.name << "\" ";

        /* Relation Type */
        switch (attributeRelationValue.second.objectType) {
        case AttributeObjectType::Network:
        case AttributeObjectType::Node:
        case AttributeObjectType::Message:
        case AttributeObjectType::Signal:
        case AttributeObjectType::EnvironmentVariable:
            /* not handled here */
            break;
        case AttributeObjectType::ControlUnitEnvironmentVariable:
            os << "BU_EV_REL_ ";
            os << attributeRelationValue.second.nodeName;
            os << ' ';
            os << attributeRelationValue.second.environmentVariableName;
            break;
        case AttributeObjectType::NodeTxMessage:
            os << "BU_BO_REL_ ";
            os << attributeRelationValue.second.nodeName;
            os << ' ';
            os << attributeRelationValue.second.messageId;
            break;
        case AttributeObjectType::NodeMappedRxSignal:
            os << "BU_SG_REL_ ";
            os << attributeRelationValue.second.nodeName;
            os << " SG_ ";
            os << attributeRelationValue.second.messageId;
            os << ' ';
            os << attributeRelationValue.second.signalName;
            break;
        }
        os << ' ';

        /* Value Type */
        switch (attributeDefinition.valueType.type) {
        case AttributeValueType::Type::Int:
            os << attributeRelationValue.second.integerValue;
            break;
        case AttributeValueType::Type::Hex:
            os << attributeRelationValue.second.hexValue;
            break;
        case AttributeValueType::Type::Float:
            os << attributeRelationValue.second.floatValue;
            break;
        case AttributeValueType::Type::String:
            os << '"' << attributeRelationValue.second.stringValue << '"';
            break;
        case AttributeValueType::Type::Enum:
            os << attributeRelationValue.second.enumValue;
            break;
        }
        os << ';' << endl;
    }

    /* Value Descriptions (VAL) */
    for (const auto & message : network.messages) {
        for (const auto & signal : message.second.signals) {
            if (!signal.second.valueDescriptions.empty()) {
                os << "VAL_ " << message.second.id << ' ' << signal.second.name;
                for (const auto & valueDescription : signal.second.valueDescriptions) {
                    os << " " << valueDescription.first;
                    os << " \"" << valueDescription.second << "\"";
                }
                os << " ;" << endl;
            }
        }
    }
    for (const auto & environmentVariable : network.environmentVariables) {
        if (!environmentVariable.second.valueDescriptions.empty()) {
            os << "VAL_ " << environmentVariable.second.name;
            for (const auto & valueDescription : environmentVariable.second.valueDescriptions) {
                os << " " << valueDescription.first;
                os << " \"" << valueDescription.second << "\"";
            }
            os << " ;" << endl;
        }
    }

    /* Category Definitions (CAT_DEF, obsolete) */

    /* Categories (CAT, obsolete) */

    /* Filters (FILTER, obsolete) */

    /* Signal Type Refs (SGTYPE, obsolete) */
    for (const auto & signalType : network.signalTypes)
        os << signalType.second;
    for (const auto & message : network.messages) {
        for (const auto & signal : message.second.signals) {
            if (!signal.second.type.empty())
                os << "SGTYPE_ " << message.second.id << ' ' << signal.second.name << " : " << signal.second.type << ";" << endl;
        }
    }

    /* Signal Groups (SIG_GROUP) */
    for (const auto & message : network.messages) {
        for (const auto & signalGroup : message.second.signalGroups)
            os << signalGroup.second;
    }

    /* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
    for (const auto & message : network.messages) {
        for (const auto & signal : message.second.signals) {
            if (signal.second.extendedValueType  !=  Signal::ExtendedValueType::Undefined) {
                os << "SIG_VALTYPE_ " << message.second.id << ' ' << signal.second.name;
                os << " : " << char(signal.second.extendedValueType);
                os << ';' << endl;
            }
        }
    }

    /* Extended Multiplexors (SG_MUL_VAL) */
    for (const auto & message : network.messages) {
        for (const auto & signal : message.second.signals) {
            for (const auto & extendedMultiplexor : signal.second.extendedMultiplexors) {
                /* Identifier, Name */
                os << "SG_MUL_VAL_ " << message.second.id << ' ' << signal.second.name;

                /* Switch Name */
                os << ' ' << extendedMultiplexor.second.switchName;

                /* Value Ranges */
                bool first = true;
                for (const auto & valueRange : extendedMultiplexor.second.valueRanges) {
                    if (first)
                        first = false;
                    else
                        os << ", ";
                    os << valueRange.first << '-' << valueRange.second;
                }
                os << ';' << endl;
            }
        }
    }

    os << endl;

    return os;
}

std::istream & operator>>(std::istream & is, Network & network) {
    /* Flex scanner */
    Scanner scanner(is);

    /* Bison parser */
    Parser parser(&scanner, &network);

    /* parse */
    network.successfullyParsed = (parser.parse() == 0);

    return is;
}

}
}
