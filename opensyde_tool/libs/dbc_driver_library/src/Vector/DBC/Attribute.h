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
#include <string>

#include <Vector/DBC/AttributeObjectType.h>

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/**
 * Attribute Value (BA)
 *
 * Attribute Default (BA_DEF_DEF) and
 * Attribute Default Value on Relation (BA_DEF_DEF_REL)
 */
struct VECTOR_DBC_EXPORT Attribute {
    /** Name */
    std::string name {};

    /** Value Type */
    AttributeObjectType objectType { AttributeObjectType::Network };

    union {
        /** Integer Value of type AttributeValueType::Int */
        // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
        int64_t integerValue {};

        /** Hex Value of type AttributeValueType::Hex */
        // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
        int64_t hexValue;

        /** Float Value of type AttributeValueType::Float */
        double floatValue;

        // std::string doesn't work in a union, so it's below

        /** Enum Value of type AttributeValueType::Enum (used only for BA enums) */
        // 2023-03-15 STW: Replace int32 by int64 to allow compatibility with int64 DBC files
        int64_t enumValue;
    };

    /** String Value of type AttributeValueType::String (used only for BA_DEF_DEF enums) */
    std::string stringValue {};
};

}
}
