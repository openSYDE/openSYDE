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
#include <ostream>
#include <string>

#include <Vector/DBC/ByteOrder.h>
#include <Vector/DBC/ValueType.h>

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/**
 * Signal Type (SGTYPE)
 */
struct VECTOR_DBC_EXPORT SignalType {
    /** Name */
    std::string name {};

    /** Size */
    uint32_t size {};

    /** Byte Order */
    ByteOrder byteOrder { ByteOrder::BigEndian };

    /** Value Type */
    ValueType valueType { ValueType::Unsigned };

    /** Factor */
    double factor {};

    /** Offset */
    double offset {};

    /** Minimum Physical Value */
    double minimum {};

    /** Maximum Physical Value */
    double maximum {};

    /** Unit */
    std::string unit {};

    /** Default Value */
    double defaultValue {};

    /** Value Table */
    std::string valueTable {};
};

std::ostream & operator<<(std::ostream & os, const SignalType & signalType);

}
}
