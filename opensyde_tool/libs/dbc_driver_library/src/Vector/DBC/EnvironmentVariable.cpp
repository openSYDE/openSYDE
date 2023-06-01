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

#include <Vector/DBC/EnvironmentVariable.h>

namespace Vector {
namespace DBC {

std::ostream & operator<<(std::ostream & os, const EnvironmentVariable & environmentVariable) {
    os << "EV_ " << environmentVariable.name << ": ";

    /* Type */
    switch (environmentVariable.type) {
    case EnvironmentVariable::Type::Integer:
    // [[fallthrough]]
    case EnvironmentVariable::Type::String:
    // [[fallthrough]]
    case EnvironmentVariable::Type::Data:
        os << '0';
        break;
    case EnvironmentVariable::Type::Float:
        os << '1';
        break;
    }

    /* Minimum, Maximum */
    os << " [";
    os << environmentVariable.minimum;
    os << '|';
    os << environmentVariable.maximum;
    os << ']';

    /* Unit */
    os << " \"";
    os << environmentVariable.unit;
    os << "\" ";

    /* Initial Value */
    os << environmentVariable.initialValue;
    os << ' ';

    /* ID */
    os << environmentVariable.id;

    /* Access Type */
    os << " DUMMY_NODE_VECTOR";
    os << std::hex;
    if (environmentVariable.type == EnvironmentVariable::Type::String)
        os << (static_cast<uint16_t>(environmentVariable.accessType) | 0x8000);
    else
        os << static_cast<uint16_t>(environmentVariable.accessType);
    os << std::dec;
    os << ' ';

    /* Access Nodes */
    if (environmentVariable.accessNodes.empty())
        os << "Vector__XXX";
    else {
        os << ' ';
        bool first = true;
        for (const auto & accessNode : environmentVariable.accessNodes) {
            if (first)
                first = false;
            else
                os << ',';
            os << accessNode;
        }
    }
    os << ";" << endl;

    return os;
}

}
}
