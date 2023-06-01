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

#include <Vector/DBC/AttributeDefinition.h>

namespace Vector {
namespace DBC {

std::ostream & operator<<(std::ostream & os, const AttributeDefinition & attributeDefinition) {
    /* Object Type */
    switch (attributeDefinition.objectType) {
    case AttributeObjectType::Network:
        os << "BA_DEF_ ";
        break;
    case AttributeObjectType::Node:
        os << "BA_DEF_ BU_ ";
        break;
    case AttributeObjectType::Message:
        os << "BA_DEF_ BO_ ";
        break;
    case AttributeObjectType::Signal:
        os << "BA_DEF_ SG_ ";
        break;
    case AttributeObjectType::EnvironmentVariable:
        os << "BA_DEF_ EV_ ";
        break;
    case AttributeObjectType::ControlUnitEnvironmentVariable:
        os << "BA_DEF_REL_ BU_EV_REL_ ";
        break;
    case AttributeObjectType::NodeTxMessage:
        os << "BA_DEF_REL_ BU_BO_REL_ ";
        break;
    case AttributeObjectType::NodeMappedRxSignal:
        os << "BA_DEF_REL_ BU_SG_REL_ ";
        break;
    }

    /* Name */
    os << " \"" << attributeDefinition.name << "\" ";

    /* Value Type */
    os << attributeDefinition.valueType;

    os << ";" << endl;

    return os;
}

}
}
