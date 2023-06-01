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

#include <Vector/DBC/Message.h>

namespace Vector {
namespace DBC {

std::ostream & operator<<(std::ostream & os, const Message & message) {
    os << "BO_ " << message.id;
    os << " " << message.name;
    os << ": " << message.size << " ";
    if (message.transmitter.empty())
        os << "Vector__XXX";
    else
        os << message.transmitter;
    os << endl;

    /* Signals (SG) */
    for (const auto & signal : message.signals)
        os << signal.second;

    os << endl;

    return os;
}

}
}
