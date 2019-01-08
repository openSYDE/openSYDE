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
 */

#pragma once

#include <Vector/DBC/platform.h>

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <Vector/DBC/Attribute.h>
#include <Vector/DBC/ByteOrder.h>
#include <Vector/DBC/ExtendedMultiplexor.h>
#include <Vector/DBC/ValueDescriptions.h>
#include <Vector/DBC/ValueType.h>

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/**
 * Signal (SG)
 */
class VECTOR_DBC_EXPORT Signal
{
public:
    Signal();

    /** Name */
    std::string name;

    /** Multiplexed Signal (m) */
    bool multiplexedSignal; // m

    /** Multiplexer Switch Value */
    unsigned int multiplexerSwitchValue;

    /** Multiplexor Switch (M) */
    bool multiplexorSwitch; // M

    /** Start Bit */
    unsigned int startBit;

    /** Bit Size */
    unsigned int bitSize;

    /** Byte Order */
    ByteOrder byteOrder;

    /** Value Type */
    ValueType valueType;

    /** Factor */
    double factor;

    /** Offset */
    double offset;

    /** Minimun Physical Value (or 0 if auto calculated) */
    double minimumPhysicalValue;

    /** Maximum Physical Value (or 0 if auto calculated) */
    double maximumPhysicalValue;

    /** Unit */
    std::string unit;

    /** Receivers */
    std::set<std::string> receivers;

    /** Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    enum class ExtendedValueType : char {
        Undefined = ' ',
        Integer = '0',
        Float = '1',
        Double = '2'
    };

    /** Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    ExtendedValueType extendedValueType;

    /** Value Descriptions (VAL) */
    ValueDescriptions valueDescriptions;

    /** Signal Type Refs (SGTYPE, obsolete) */
    std::string type;

    /** Comment (CM) */
    std::string comment;

    /** Attribute Values (BA) */
    std::map<std::string, Attribute> attributeValues;

    /** Extended Multiplexors (SG_MUL_VAL) */
    std::map<std::string, ExtendedMultiplexor> extendedMultiplexors;

    /**
     * @brief Convert from Raw to Physical Value
     * @param[in] rawValue Raw Value
     * @return Raw Value
     *
     * Converts a value from raw to physical representation.
     */
    double rawToPhysicalValue(double rawValue);

    /**
     * @brief Convert from Physical to Raw Value
     * @param[in] physicalValue Physical Value
     * @return Physical Value
     *
     * Converts a value from physical to raw representation.
     */
    double physicalToRawValue(double physicalValue);

    /**
     * @brief Get minimum Physical Value
     * @return Minimum Physical Value
     *
     * Based on size, valueType and extendedValueType this calculates the minimum raw value.
     */
    double minimumRawValue();

    /**
     * @brief Get maximum Raw Value
     * @return Maximum Raw Value
     *
     * Based on size, valueType and extendedValueType this calculates the maximum raw value.
     */
    double maximumRawValue();

    /**
     * @brief Decodes/Extracts a signal from the message data
     * @param[in] data Data
     * @return Raw signal value
     *
     * Decodes/Extracts a signal from the message data.
     *
     * @note Multiplexors are not taken into account.
     */
    uint64_t decode(std::vector<uint8_t> & data);

    /**
     * @brief Encodes a signal into the message data
     * @param[inout] data Data
     * @param[in] rawValue Raw signal value
     *
     * Encode a signal into the message data.
     *
     * @note Multiplexors are not taken into account.
     */
    void encode(std::vector<uint8_t> & data, uint64_t rawValue);
};

}
}
