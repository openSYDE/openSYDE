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

#pragma once

#include <Vector/DBC/platform.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <Vector/DBC/Attribute.h>
#include <Vector/DBC/AttributeDefinition.h>
#include <Vector/DBC/AttributeRelation.h>
#include <Vector/DBC/BitTiming.h>
#include <Vector/DBC/EnvironmentVariable.h>
#include <Vector/DBC/Message.h>
#include <Vector/DBC/Node.h>
#include <Vector/DBC/SignalType.h>
#include <Vector/DBC/ValueDescriptions.h>
#include <Vector/DBC/ValueTable.h>

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/**
 * Network
 */
struct VECTOR_DBC_EXPORT Network {
    /** successfully parsed */
    bool successfullyParsed { false };

    /** Version (VERSION) */
    std::string version {};

    /** New Symbols (NS) */
    std::vector<std::string> newSymbols {};

    /** Bit Timing (BS) */
    BitTiming bitTiming {};

    /** Nodes (BU) */
    std::map<std::string, Node> nodes {};

    /** Value Tables (VAL_TABLE) */
    std::map<std::string, ValueTable> valueTables {};

    /** Messages (BO) and Signals (SG) */
    std::map<uint32_t, Message> messages {};

    /* Message Transmitters (BO_TX_BU) */
    // moved to Message (BO)

    /** Environment Variables (EV) */
    std::map<std::string, EnvironmentVariable> environmentVariables {};

    /* Environment Variables Data (ENVVAR_DATA) */
    // moved to Environment Variables (EV)

    /** Signal Types (SGTYPE, obsolete) */
    std::map<std::string, SignalType> signalTypes {};

    /** Comments (CM) */
    std::string comment {}; // for network
    // moved to Node (BU) for nodes
    // moved to Message (BO) for messages
    // moved to Signal (SG) for signals
    // moved to Environment Variable (EV) for environment variables

    /**
     * Attribute Definitions (BA_DEF) and
     * Attribute Definitions for Relations (BA_DEF_REL)
     */
    std::map<std::string, AttributeDefinition> attributeDefinitions {};

    /* Sigtype Attr List (?, obsolete) */

    /**
     * Attribute Defaults (BA_DEF_DEF) and
     * Attribute Defaults for Relations (BA_DEF_DEF_REL)
     */
    std::map<std::string, Attribute> attributeDefaults {};

    /** Attribute Values (BA) */
    std::map<std::string, Attribute> attributeValues {}; // for network
    // moved to Node (BU) for nodes
    // moved to Message (BO) for messages
    // moved to Signal (SG) for signals
    // moved to Environment Variable (EV) for environment variables

    /** Attribute Values on Relations (BA_REF) */
    std::map<std::string, AttributeRelation> attributeRelationValues {};

    /* Value Descriptions (VAL) */
    // moved to Signals (BO) for signals
    // moved to EnvironmentVariable (EV) for environment variables

    /* Category Definitions (CAT_DEF, obsolete) */

    /* Categories (CAT, obsolete) */

    /* Filters (FILTER, obsolete) */

    /* Signal Type Refs (SGTYPE, obsolete) */
    // moved to Signal (SG)

    /* Signal Groups (SIG_GROUP) */
    // moved to Message (BO)

    /* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
    // moved to Signal (SG)

    /* Extended Multiplexors (SG_MUL_VAL) */
    // moved to Signal (SG)
};

VECTOR_DBC_EXPORT std::ostream & operator<<(std::ostream & os, const Network & network);
VECTOR_DBC_EXPORT std::istream & operator>>(std::istream & is, Network & network);

}
}
