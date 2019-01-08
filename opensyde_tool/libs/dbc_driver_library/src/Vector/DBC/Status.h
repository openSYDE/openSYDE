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

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/**
 * Status
 */
enum class Status : int {
    /* Informations */
    Ok = 0, /**< General success */

    /* Warnings */
    Warning = 0x40000000, /**< General warning */
    Unknown, /** Unknown entry in database file */
    Unsupported, /** Unsupported entry in database file */
    MalformedVersion, /**< Incorrect format of Version (VERSION) */
    MalformedBitTiming, /**< Incorrect format of Bit Timing (BS) */
    MalformedValueTable, /**< Incorrect format of Value Table (VAL_TABLE) */
    MalformedSignal, /**< Incorrect format of Signal (SG) */
    MalformedMessage, /**< Incorrect format of Message (BO) */
    MalformedMessageTransmitter, /**< Incorrect format of Message Transmitter (BO_TX_BU) */
    MalformedEnvironmentVariable, /**< Incorrect format of Environment Variable (EV) */
    MalformedEnvironmentVariableData, /**< Incorrect format of Environment Variable Data (ENVVAR_DATA) */
    MalformedSignalType, /**< Incorrect format of Signal Type (SGTYPE) */
    MalformedComment, /**< Incorrect format of Comment (CM) */
    MalformedAttributeDefinition, /**< Incorrect format of Attribute Definition (BA_DEF) */
    MalformedAttributeDefinitionRelation, /**< Incorrect format of Attribute Definition at Relation (BA_DEF_REL) */
    MalformedAttributeDefault, /**< Incorrect format of Attribute Default (BA_DEF_DEF) */
    MalformedAttributeDefaultRelation, /**< Incorrect format of Attribute Default at Relation (BA_DEF_DEF_REL) */
    MalformedAttributeValue, /**< Incorrect format of Attribute Value (BA) */
    MalformedAttributeRelationValue, /**< Incorrect format of Attribute Value at Relation (BA_REF) */
    MalformedValueDescription, /**< Incorrect format of Value Description (VAL) */
    MalformedSignalGroup, /**< Incorrect format of Signal Group (SIG_GROUP) */
    MalformedSignalExtendedValueType, /**< Incorrect format of Signal Extended Value Type (SIG_VALTYPE) */
    MalformedExtendedMultiplexor, /**< Incorrect format of Extended Multiplexor (SG_MUL_VAL) */
    SignalWithoutMessage, /**< Signal (SG) followed a line, which was not a Message (BO) or Signal (SG) */

    /* Errors */
    Error = -1, /**< General error */
    FileOpenError = -2 /**< Error on file open */
};

// #define SUCCEEDED(code) ((int)(code) >= 0)
// #define WARNED(code) ((int)(code) & 0x40000000)
// #define FAILED(code) ((int)(code) < 0)

}
}
