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

#include <Vector/DBC/AttributeRelation.h>

namespace Vector {
namespace DBC {

bool operator<(const AttributeRelation & lhs, const AttributeRelation & rhs) {
    bool retval = false;

    /* compare name */
    if (lhs.name == rhs.name) {

        /* compare relationType */
        if (lhs.objectType == rhs.objectType) {

            /* relationType based optimizations */
            switch (lhs.objectType) {
            case AttributeObjectType::Network:
            case AttributeObjectType::Node:
            case AttributeObjectType::Message:
            case AttributeObjectType::Signal:
            case AttributeObjectType::EnvironmentVariable:
                /* not handled here */
                break;
            case AttributeObjectType::ControlUnitEnvironmentVariable:
                /* only compare nodeName, environmentVariableName */
                if (lhs.nodeName == rhs.nodeName)
                    retval = lhs.environmentVariableName < rhs.environmentVariableName;
                else
                    retval = lhs.nodeName < rhs.nodeName;
                break;

            case AttributeObjectType::NodeTxMessage:
                /* only compare nodeName, messageId */
                if (lhs.nodeName == rhs.nodeName)
                    retval = lhs.messageId < rhs.messageId;
                else
                    retval = lhs.nodeName < rhs.nodeName;
                break;

            case AttributeObjectType::NodeMappedRxSignal:
                /* only compare nodeName, messageId, signalName */
                if (lhs.nodeName == rhs.nodeName) {
                    if (lhs.messageId == rhs.messageId)
                        retval = lhs.signalName < rhs.signalName;
                    else
                        retval = lhs.messageId < rhs.messageId;
                } else
                    retval = lhs.nodeName < rhs.nodeName;
                break;
            }
        } else
            retval = lhs.objectType < rhs.objectType;
    } else
        retval = lhs.name < rhs.name;

    return retval;
}

}
}
