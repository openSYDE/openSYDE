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

#include <functional>

#include <Vector/DBC/Network.h>
#include <Vector/DBC/Status.h>

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/**
 * File
 */
class VECTOR_DBC_EXPORT File
{
public:
    File();

    /**
     * @brief Load database file
     * @param[out] network Network
     * @param[in] filename File name
     * @return Status code
     *
     * Loads database file.
     */
    Status load(Network & network, const char * filename);

    /**
     * @brief Load database file
     * @param[out] network Network
     * @param[in] filename File Name
     * @return Status code
     *
     * Loads database file.
     */
    Status load(Network & network, std::string & filename);

    /**
     * @brief Save database file
     * @param[in] network Network
     * @param[in] filename File Name
     * @return Status code
     *
     * Saves database file.
     */
    Status save(Network & network, const char * filename);

    /**
     * @brief Save database file
     * @param[in] network Network
     * @param[in] filename File Name
     * @return Status code
     *
     * Saves database file.
     */
    Status save(Network & network, std::string & filename);

    /**
     * Progress Callback function type
     */
    using ProgressCallback = std::function<void(Network & network, float numerator, float denominator)>;

    /**
     * @brief Set Progress Callback function
     * @param[in] function Progress Callback function
     *
     * Set the callback function to get progress information.
     * On load this is the file position, so can be used for debugging also.
     */
    void setProgressCallback(ProgressCallback function);

    /**
     * Status Callback function type
     */
    using StatusCallback = std::function<void(Network & network, Status status)>;

    /**
     * @brief Set Status Callback function
     * @param[in] function Status Callback function
     *
     * Set the callback function to get status information.
     * Even if the database loads fine, it might step over some warnings that can only be seen here.
     */
    void setStatusCallback(StatusCallback function);

private:
    /** Progress Callback function */
    ProgressCallback progressCallback;

    /** Status Callback function */
    StatusCallback statusCallback;

    /** Remove windows/unix/mac line endings */
    void chomp(std::string & line);

    /** stod with C locale */
    double stod(const std::string & str);

    /** Read Version (VERSION) */
    void readVersion(Network & network, std::string & line);

    /** Write Version (VERSION) */
    void writeVersion(std::ofstream & ofs, Network & network);

    /** Read New Symbols (NS) */
    void readNewSymbols(Network & network, std::ifstream & ifs, std::string & line);

    /** Write New Symbols (NS) */
    void writeNewSymbols(std::ofstream & ofs, Network & network);

    /** Read Bit Timing (BS) */
    void readBitTiming(Network & network, std::string & line);

    /** Write Bit Timing (BS) */
    void writeBitTiming(std::ofstream & ofs, Network & network);

    /** Read Nodes (BU) */
    void readNodes(Network & network, std::string & line);

    /** Write Nodes (BU) */
    void writeNodes(std::ofstream & ofs, Network & network);

    /** Read Value Table (VAL_TABLE) */
    void readValueTable(Network & network, std::string & line);

    /** Write Value Tables (VAL_TABLE) */
    void writeValueTables(std::ofstream & ofs, Network & network);

    /** Read Signal (SG) */
    void readSignal(Network & network, Message & message, std::string & line);

    /** Write Signals (SG) */
    void writeSignals(std::ofstream & ofs, Message & message);

    /** Read Message (BO) */
    Message * readMessage(Network & network, std::string & line);

    /** Write Messages (BO) */
    void writeMessages(std::ofstream & ofs, Network & network);

    /** Read Message Transmitter (BO_TX_BU) */
    void readMessageTransmitter(Network & network, std::string & line);

    /** Write Message Transmitters (BO_TX_BU) */
    void writeMessageTransmitters(std::ofstream & ofs, Network & network);

    /** Read Environment Variable (EV) */
    void readEnvironmentVariable(Network & network, std::string & line);

    /** Write Environment Variables (EV) */
    void writeEnvironmentVariables(std::ofstream & ofs, Network & network);

    /** Read Environment Variable Data (ENVVAR_DATA) */
    void readEnvironmentVariableData(Network & network, std::string & line);

    /** Write Environment Variable Data (ENVVAR_DATA) */
    void writeEnvironmentVariableData(std::ofstream & ofs, Network & network);

    /** Read Signal Type (SGTYPE, obsolete) */
    void readSignalType(Network & network, std::string & line);

    /** Write Signal Types (SGTYPE, obsolete) */
    void writeSignalTypes(std::ofstream & ofs, Network & network);

    /** Read Comment (CM) for Network */
    bool readCommentNetwork(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) for Node (BU) */
    bool readCommentNode(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) for Message (BO) */
    bool readCommentMessage(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) for Signal (SG) */
    bool readCommentSignal(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) for Environment Variable (EV) */
    bool readCommentEnvironmentVariable(Network & network, std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) */
    void readComment(Network & network, std::ifstream & ifs, std::string & line);

    /** Write Comments (CM) for Networks */
    void writeCommentsNetworks(std::ofstream & ofs, Network & network);

    /** Write Comments (CM) for Node (BU) */
    void writeCommentsNodes(std::ofstream & ofs, Network & network);

    /** Write Comments (CM) for Message (BO) */
    void writeCommentsMessages(std::ofstream & ofs, Network & network);

    /** Write Comments (CM) for Signal (SG) */
    void writeCommentsSignals(std::ofstream & ofs, Network & network);

    /** Write Comments (CM) for Environment Variable (EV) */
    void writeCommentsEnvironmentVariables(std::ofstream & ofs, Network & network);

    /** Write Comments (CM) */
    void writeComments(std::ofstream & ofs, Network & network);

    /** Read Attribute Definition (BA_DEF) */
    void readAttributeDefinition(Network & network, std::string & line);

    /** Read Attribute Definition at Relation (BA_DEF_REL) */
    void readAttributeDefinitionRelation(Network & network, std::string & line);

    /** Write Attribute Definitions (BA_DEF) and Attribute Definitions at Relations (BA_DEF_REL) */
    void writeAttributeDefinitions(std::ofstream & ofs, Network & network);

    /* Read Sigtype Attr List (?, obsolete) */

    /* Write Sigtype Attr Lists (?, obsolete) */

    /** Read Attribute Default (BA_DEF_DEF) */
    void readAttributeDefault(Network & network, std::string & line);

    /** Read Attribute Default at Relation (BA_DEF_DEF_REL) */
    void readAttributeDefaultRelation(Network & network, std::string & line);

    /** Write Attribute Defaults (BA_DEF_DEF) and Attribute Defaults at Relations (BA_DEF_DEF_REL) */
    void writeAttributeDefaults(std::ofstream & ofs, Network & network);

    /** Read Attribute Value (BA) for Network */
    bool readAttributeValueNetwork(Network & network, std::string & line);

    /** Read Attribute Value (BA) for Node (BU) */
    bool readAttributeValueNode(Network & network, std::string & line);

    /** Read Attribute Value (BA) for Message (BO) */
    bool readAttributeValueMessage(Network & network, std::string & line);

    /** Read Attribute Value (BA) for Signal (SG) */
    bool readAttributeValueSignal(Network & network, std::string & line);

    /** Read Attribute Value (BA) for Environment Variable (EV) */
    bool readAttributeValueEnvironmentVariable(Network & network, std::string & line);

    /** Read Attribute Value (BA) */
    void readAttributeValue(Network & network, std::string & line);

    /** Write Attribute Values (BA) for Networks */
    void writeAttributeValuesNetworks(std::ofstream & ofs, Network & network);

    /** Write Attribute Values (BA) for Nodes (BU) */
    void writeAttributeValuesNodes(std::ofstream & ofs, Network & network);

    /** Write Attribute Values (BA) for Messages (BO) */
    void writeAttributeValuesMessages(std::ofstream & ofs, Network & network);

    /** Write Attribute Values (BA) for Signals (SG) */
    void writeAttributeValuesSignals(std::ofstream & ofs, Network & network);

    /** Write Attribute Values (BA) for Environment Variables (EV) */
    void writeAttributeValuesEnvironmentVariables(std::ofstream & ofs, Network & network);

    /** Read Attribute Value at Relation (BA_REL) */
    void readAttributeRelationValue(Network & network, std::string & line);

    /** Write Attribute Values at Relations (BA_REL) */
    void writeAttributeRelationValues(std::ofstream & ofs, Network & network);

    /** Read Value Description (VAL) for Signal (SG) */
    bool readValueDescriptionSignal(Network & network, std::string & line);

    /** Read Value Description (VAL) for Environment Variable (EV) */
    bool readValueDescriptionEnvironmentVariable(Network & network, std::string & line);

    /** Read Value Description (VAL) */
    void readValueDescription(Network & network, std::string & line);

    /** Write Value Descriptions (VAL) for Signals (SG) */
    void writeValueDescriptionsSignals(std::ofstream & ofs, Network & network);

    /** Write Value Descriptions (VAL) for Environment Variables (EV) */
    void writeValueDescriptionsEnvironmentVariables(std::ofstream & ofs, Network & network);

    /* Read Category Definition (?, obsolete) */

    /* Write Category Definitions (?, obsolete) */

    /* Read Category (?, obsolete) */

    /* Write Categories (?, obsolete) */

    /* Read Filter (?, obsolete) */

    /* Write Filters (?, obsolete) */

    /* Read Signal Type Ref (SGTYPE, obsolete) */
    // see above readSignalType

    /* Write Signal Type Refs (SGTYPE, obsolete) */
    // see above writeSignalTypes

    /** Read Signal Group (SIG_GROUP) */
    void readSignalGroup(Network & network, std::string & line);

    /** Write Signal Groups (SIG_GROUP) */
    void writeSignalGroups(std::ofstream & ofs, Network & network);

    /** Read Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    void readSignalExtendedValueType(Network & network, std::string & line);

    /** Write Signal Extended Value Types (SIG_VALTYPE, obsolete) */
    void writeSignalExtendedValueTypes(std::ofstream & ofs, Network & network);

    /** Read Extended Multiplexor (SG_MUL_VAL) */
    void readExtendedMultiplexor(Network & network, std::string & line);

    /** Write Extended Multiplexors (SG_MUL_VAL) */
    void writeExtendedMultiplexors(std::ofstream & ofs, Network & network);
};

}
}
