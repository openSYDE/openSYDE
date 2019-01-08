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

#include <map>
#include <set>
#include <string>

#include <Vector/DBC/Attribute.h>
#include <Vector/DBC/ValueDescriptions.h>

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/**
 * Environment Variable (EV)
 */
class VECTOR_DBC_EXPORT EnvironmentVariable
{
public:
    EnvironmentVariable();

    /** Name */
    std::string name;

    /** Type */
    enum class Type : char {
        Integer, /**< Integer */
        Float, /**< Float */
        String, /**< String */
        Data /**< Data */
    };

    /** Type */
    Type type;

    /** Minimum */
    double minimum;

    /** Maximum */
    double maximum;

    /** Unit */
    std::string unit;

    /** Initial Value */
    double initialValue;

    /** Identifier */
    unsigned int id;

    /** Access Type */
    enum class AccessType {
        Unrestricted = 0, /**< Unrestricted */
        Read = 1, /**< Read */
        Write = 2, /**< Write */
        ReadWrite = 3 /**< Read and Write */
    };

    /** Access Type */
    AccessType accessType;

    /** Access Nodes */
    std::set<std::string> accessNodes;

    /** Value Descriptions (VAL) */
    ValueDescriptions valueDescriptions;

    /** Environment Variables Data (ENVVAR_DATA) */
    unsigned int dataSize;

    /** Comment (CM) */
    std::string comment;

    /** Attribute Values (BA) */
    std::map<std::string, Attribute> attributeValues;
};

}
}
