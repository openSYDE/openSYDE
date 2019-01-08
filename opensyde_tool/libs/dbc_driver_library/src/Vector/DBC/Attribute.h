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

#include <string>

#include <Vector/DBC/AttributeValueType.h>

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/**
 * Attribute Value (BA) and
 * Attribute Default (BA_DEF_DEF) and
 * Attribute Default Value on Relation (BA_DEF_DEF_REL)
 */
class VECTOR_DBC_EXPORT Attribute
{
public:
    Attribute();

    /** Name */
    std::string name;

    /** Value Type */
    AttributeValueType valueType;

    union {
        /** Integer Value of type AttributeValueType::Int */
        int integerValue;

        /** Hex Value of type AttributeValueType::Hex */
        int hexValue;

        /** Float Value of type AttributeValueType::Float */
        double floatValue;

        /** Enum Value of type AttributeValueType::Enum (used only for BA enums) */
        int enumValue;

        // std::string doesn't work in a union, so it's below
    };

    /** String Value of type AttributeValueType::String (used only for BA_DEF_DEF enums) */
    std::string stringValue;
};

}
}
