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

#include <algorithm>
#include <cmath>
#include <limits>

#include <Vector/DBC/Signal.h>

namespace Vector {
namespace DBC {

double Signal::rawToPhysicalValue(double rawValue) const {
    /* physicalValue = rawValue * factor + offset */
    return rawValue * factor + offset;
}

double Signal::physicalToRawValue(double physicalValue) const {
    /* rawValue = (physicalValue - offset) / factor */
    return (physicalValue - offset) / factor;
}

double Signal::minimumRawValue() const {
    /* calculate minimum raw value */
    double minimumRawValue = 0.0;
    switch (extendedValueType) {
    case ExtendedValueType::Undefined:
    case ExtendedValueType::Integer:
        if (valueType == ValueType::Signed) {
            minimumRawValue = -(2 << (bitSize - 2)); // bitSize-- because shift instead of pow
        } else
            minimumRawValue = 0.0;
        break;

    case ExtendedValueType::Float:
        minimumRawValue = std::numeric_limits<float>::min();
        break;

    case ExtendedValueType::Double:
        minimumRawValue = std::numeric_limits<double>::min();
        break;
    }
    return minimumRawValue;
}

double Signal::maximumRawValue() const {
    /* calculate maximum raw value */
    double maximumRawValue = 0.0;
    switch (extendedValueType) {
    case ExtendedValueType::Undefined:
    case ExtendedValueType::Integer:
        if (valueType == ValueType::Signed) {
            maximumRawValue = (2 << (bitSize - 2)) - 1; // bitSize-- because shift instead of pow
        } else {
            maximumRawValue = (2 << (bitSize - 1)) - 1; // bitSize-- because shift instead of pow
        }
        break;

    case ExtendedValueType::Float:
        maximumRawValue = std::numeric_limits<float>::max();
        break;

    case ExtendedValueType::Double:
        maximumRawValue = std::numeric_limits<double>::max();
        break;
    }
    return maximumRawValue;
}

uint64_t Signal::decode(std::vector<uint8_t> & data) const {
    /* safety check */
    if (bitSize == 0)
        return 0;

    /* copy bits */
    uint64_t retVal = 0;
    if (byteOrder == ByteOrder::BigEndian) {
        /* start with MSB */
        unsigned int srcBit = startBit;
        unsigned int dstBit = bitSize - 1;
        for (uint32_t i = 0; i < bitSize; ++i) {
            /* copy bit */
            if (data[srcBit / 8] & (1 << (srcBit % 8)))
                retVal |= (1ULL << dstBit);

            /* calculate next position */
            if ((srcBit % 8) == 0)
                srcBit += 15;
            else
                --srcBit;
            --dstBit;
        }
    } else {
        /* start with LSB */
        unsigned int srcBit = startBit;
        unsigned int dstBit = 0;
        for (uint32_t i = 0; i < bitSize; ++i) {
            /* copy bit */
            if (data[srcBit / 8] & (1 << (srcBit % 8)))
                retVal |= (1ULL << dstBit);

            /* calculate next position */
            ++srcBit;
            ++dstBit;
        }
    }

    /* if signed, then fill all bits above MSB with 1 */
    if (valueType == ValueType::Signed) {
        if (retVal & (1 << (bitSize - 1))) {
            for (auto i = bitSize; i < 64; ++i)
                retVal |= (1ULL << i);
        }
    }

    return retVal;
}

void Signal::encode(std::vector<uint8_t> & data, uint64_t rawValue) const {
    /* safety check */
    if (bitSize == 0)
        return;

    /* copy bits */
    if (byteOrder == ByteOrder::BigEndian) {
        /* start with MSB */
        unsigned int srcBit = startBit;
        unsigned int dstBit = bitSize - 1;
        for (uint32_t i = 0; i < bitSize; ++i) {
            /* copy bit */
            if (rawValue & (1ULL << dstBit))
                data[srcBit / 8] |= (1 << (srcBit % 8));
            else
                data[srcBit / 8] &= ~(1 << (srcBit % 8));

            /* calculate next position */
            if ((srcBit % 8) == 0)
                srcBit += 15;
            else
                --srcBit;
            --dstBit;
        }
    } else {
        /* start with LSB */
        unsigned int srcBit = startBit;
        unsigned int dstBit = 0;
        for (uint32_t i = 0; i < bitSize; ++i) {
            /* copy bit */
            if (rawValue & (1ULL << dstBit))
                data[srcBit / 8] |= (1 << (srcBit % 8));
            else
                data[srcBit / 8] &= ~(1 << (srcBit % 8));

            /* calculate next position */
            ++srcBit;
            ++dstBit;
        }
    }
}

std::ostream & operator<<(std::ostream & os, const Signal & signal) {
    /* Name */
    os << " SG_ " << signal.name << ' ';

    /* Multiplexed Signal, Multiplexor Switch/Signal */
    switch (signal.multiplexor) {
    case Signal::Multiplexor::NoMultiplexor:
        os << ' ';
        break;
    case Signal::Multiplexor::MultiplexedSignal:
        os << 'm' << signal.multiplexerSwitchValue;
        break;
    case Signal::Multiplexor::MultiplexorSwitch:
        os << 'M';
        break;
    }
    os << ": ";

    /* Start Bit, Size, Byte Order, Value Type */
    os << signal.startBit << '|' << signal.bitSize << '@' << char(signal.byteOrder) << char(signal.valueType);

    /* Factor, Offset */
    os << " (" << signal.factor << ',' << signal.offset << ')';

    /* Minimum, Maximum */
    os << " [" << signal.minimum << '|' << signal.maximum << ']';

    /* Unit */
    os << " \"" << signal.unit << "\" ";

    /* Receivers */
    if (signal.receivers.empty())
        os << "Vector__XXX";
    else {
        // 2023-03-22 STW: Add comma for receiver list to fix import errors after export
        os << " ";
        for (std::set<std::string>::const_iterator receiver = signal.receivers.begin();
             receiver != signal.receivers.end(); ++receiver)
        {
            if(receiver != signal.receivers.begin())
                os << ",";
            os << *receiver;
        }
    }
    os << endl;

    return os;
}

}
}
