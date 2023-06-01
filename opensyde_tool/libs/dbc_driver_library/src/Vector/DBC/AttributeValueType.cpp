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

#include <Vector/DBC/AttributeValueType.h>

namespace Vector {
namespace DBC {

std::ostream & operator<<(std::ostream & os, const AttributeValueType & attributeValueType) {
    /* Value Type */
    switch (attributeValueType.type) {
    case AttributeValueType::Type::Int:
        os << "INT ";
        os << attributeValueType.integerValue.minimum;
        os << " ";
        os << attributeValueType.integerValue.maximum;
        break;
    case AttributeValueType::Type::Hex:
        os << "HEX ";
        os << attributeValueType.hexValue.minimum;
        os << " ";
        os << attributeValueType.hexValue.maximum;
        break;
    case AttributeValueType::Type::Float:
        os << "FLOAT ";
        os << attributeValueType.floatValue.minimum;
        os << " ";
        os << attributeValueType.floatValue.maximum;
        break;
    case AttributeValueType::Type::String:
        os << "STRING ";
        break;
    case AttributeValueType::Type::Enum:
        os << "ENUM  ";
        bool first = true;
        for (const auto & enumValue : attributeValueType.enumValues) {
            if (first)
                first = false;
            else
                os << ',';
            os << "\"" << enumValue << "\"";
        }
        break;
    }

    return os;
}

}
}
