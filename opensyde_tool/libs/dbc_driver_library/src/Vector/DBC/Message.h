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
#include <Vector/DBC/Signal.h>
#include <Vector/DBC/SignalGroup.h>

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/**
 * Message (BO)
 */
class VECTOR_DBC_EXPORT Message
{
public:
    Message();

    /** Identifier (with bit 31 set this is extended CAN frame) */
    unsigned int id;

    /** Name */
    std::string name;

    /** Size */
    unsigned int size;

    /** Transmitter (empty string if the number of send nodes is zero or more than one) */
    std::string transmitter;

    /** Signals (SG) */
    std::map<std::string, Signal> signals;

    /** Message Transmitters (BO_TX_BU) */
    std::set<std::string> transmitters;

    /** Signal Groups (SIG_GROUP) */
    std::map<std::string, SignalGroup> signalGroups;

    /** Comment (CM) */
    std::string comment;

    /** Attribute Values (BA) */
    std::map<std::string, Attribute> attributeValues;
};

}
}
