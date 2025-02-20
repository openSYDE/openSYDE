//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export data to Vector DBC file (implementation)

   After writing network to DBC file with h_ExportNetwork, the
   two functions h_GetNodeMapping and h_GetExportStatistic can be called.

   h_GetNodeMapping delivers the converted openSYDE node names to DBC conform
   symbol names.

   h_GetExportStatistic delivers some statics like number of messages and signals.

   The DBC file can be read by Vector tool CANdb++ editor.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_CieExportDbc.hpp"
#include "C_SclString.hpp"
#include "DBC.h"
#include "C_CieConverter.hpp"
#include "C_OscCanSignal.hpp"
#include "C_OscNodeDataPoolContent.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_OscLoggingHandler.hpp"

#include <algorithm>
#include <unordered_set>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::tgl;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const std::string C_CieExportDbc::mhc_SIG_INITIAL_VALUE = "GenSigStartValue";
const std::string C_CieExportDbc::mhc_MSG_CYCLE_TIME = "GenMsgCycleTime";
const std::string C_CieExportDbc::mhc_MSG_SEND_TYPE = "GenMsgSendType";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */
stw::scl::C_SclStringList C_CieExportDbc::mhc_WarningMessages;         // global warnings e.g. why some messages could
                                                                       // not be exported
stw::scl::C_SclString C_CieExportDbc::mhc_ErrorMessage;                // description of error which caused the export
                                                                       // to fail
std::map<C_SclString, C_SclString> C_CieExportDbc::mhc_NodeMapping;    // to receive niceified names after export to DBC
                                                                       // file
bool C_CieExportDbc::mhq_ValidDbcExport = false;                       // for public getter functions
C_CieExportDbc::C_ExportStatistic C_CieExportDbc::mhc_ExportStatistic; // for public getter function of export
                                                                       // statistics

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Export DBC network from CIE data structure.

   Assumptions:
   * DBC network must have at least one node
   * CIE data structure must be correctly converted from openSYDE structures
     by C_CieDataPoolListAdapter::h_ConvertToDBCImportMessage

   \param[in]   orc_File               output DBC file (full file path required)
   \param[in]   orc_Definition         content for DBC file
   \param[out]  orc_WarningMessages    warning messages after export
   \param[out]  orc_ErrorMessage       error message after export

   \return
   C_NO_ERR        successful export to DBC file
   C_WARN          cycle time is negative, perhaps uint32_t value does not fit in integer
                   value type of signal not supported
                   warning on writing to DBC file (see log for specific details)
   C_CONFIG        path of directory for DBC file does not exist
                   no node in network for DBC file export
   C_UNKNOWN_ERR   general error when writing to DBC file, no more details
   C_BUSY          error on writing to DBC file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieExportDbc::h_ExportNetwork(const stw::scl::C_SclString & orc_File,
                                        const C_CieConverter::C_CieCommDefinition & orc_Definition,
                                        stw::scl::C_SclStringList & orc_WarningMessages,
                                        stw::scl::C_SclString & orc_ErrorMessage)
{
   int32_t s32_Return = C_NO_ERR;
   C_SclString c_Message;

   Vector::DBC::Network c_DbcNetwork;

   mhc_WarningMessages.Clear(); // clear old warning messages for this export
   mhc_ErrorMessage = "";       // clear old error message for this export
   mhc_NodeMapping.clear();     // clear old node mapping for this export
   mhq_ValidDbcExport = false;  // only true if DBC file export was successful
   // reset export statistics
   mhc_ExportStatistic.u32_NumOfMessages = 0;
   mhc_ExportStatistic.u32_NumOfSignals = 0;

   // check file path
   if (TglDirectoryExists(TglExtractFilePath(orc_File)) == false)
   {
      orc_ErrorMessage = "Path \"" + orc_File + "\" does not exist.";
      osc_write_log_warning("DBC file export", orc_ErrorMessage);
      s32_Return = C_CONFIG;
   }

   // set common information
   if (s32_Return == C_NO_ERR)
   {
      // set network information (bus name is network name)
      const std::string c_DbName = "DBName";
      Vector::DBC::Attribute c_DbNameAttribute;
      Vector::DBC::AttributeDefinition c_DbNameAttributeType;
      c_DbNameAttribute.name = "DBName";
      c_DbNameAttribute.stringValue = mh_EscapeCriticalSymbols(orc_Definition.c_Bus.c_Name).c_str();
      c_DbNameAttributeType.name = "DBName";
      c_DbNameAttributeType.valueType.type = Vector::DBC::AttributeValueType::Type::String;
      c_DbcNetwork.attributeValues.emplace(std::pair<std::string, Vector::DBC::Attribute>(
                                              c_DbName, c_DbNameAttribute));
      c_DbcNetwork.attributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                                   c_DbName, c_DbNameAttributeType));
      c_DbcNetwork.comment = mh_EscapeCriticalSymbols(orc_Definition.c_Bus.c_Comment).c_str();

      c_Message = "Filling up general network information, symbols and attributes for network \"" +
                  orc_Definition.c_Bus.c_Name + "\" ...";
      osc_write_log_info("DBC file export", c_Message);

      // set symbols and attributes
      mh_SetNewSymbols(c_DbcNetwork.newSymbols);
      mh_SetAttributeDefaults(c_DbcNetwork.attributeDefaults);
      mh_SetAttributeDefinitions(c_DbcNetwork.attributeDefinitions);

      // set nodes
      c_Message = "Inserting nodes for network ...";
      osc_write_log_info("DBC file export", c_Message);
      s32_Return = mh_SetNodes(orc_Definition.c_Nodes, c_DbcNetwork.nodes);
   }

   if (s32_Return == C_NO_ERR)
   {
      // set messages
      c_Message = "Inserting messages for network ...";
      osc_write_log_info("DBC file export", c_Message);
      s32_Return = mh_SetMessages(orc_Definition.c_Nodes, c_DbcNetwork.messages);
      mhc_ExportStatistic.u32_NumOfMessages = c_DbcNetwork.messages.size(); // in any case
   }

   // save DBC export to file
   if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
   {
      std::ofstream c_File(orc_File.c_str());

      if (c_File.is_open())
      {
         c_Message = "Saving network to file ...";
         osc_write_log_info("DBC file export", c_Message);
         c_File << c_DbcNetwork;

         // set status flag of this export
         mhq_ValidDbcExport = true;
      }
      else
      {
         orc_ErrorMessage = "Path \"" + orc_File + "\" could not be opened.";
         osc_write_log_warning("DBC file export", orc_ErrorMessage);
         s32_Return = C_BUSY;
      }
   }

   orc_WarningMessages = mhc_WarningMessages; // set warning messages for caller
   orc_ErrorMessage = mhc_ErrorMessage;       // set error message for caller

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Getter for the node mapping for DBC file export.

   DBC symbol names does not contain any special characters
   (see C_CieExportDbc::mh_NiceifyStringForDbcSymbol).

   \param[in,out] orc_NodeMapping    key:   openSYDE node name
                                     value: exported DBC node name

   \return
   C_NO_ERR     mapping valid
   C_NOACT      no valid export to DBC file executed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieExportDbc::h_GetNodeMapping(std::map<C_SclString, C_SclString> & orc_NodeMapping)
{
   int32_t s32_Return = C_NOACT;

   if (mhq_ValidDbcExport == true)
   {
      orc_NodeMapping = mhc_NodeMapping;
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Getter for the export statistics.

   E.g.: Number of Tx/Rx messages and signals.

   \param[out]    orc_ExportStatistic    output parameter description

   \return
   C_NO_ERR     export statistic valid
   C_NOACT      no valid export to DBC file executed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieExportDbc::h_GetExportStatistic(C_ExportStatistic & orc_ExportStatistic)
{
   int32_t s32_Return = C_NOACT;

   if (mhq_ValidDbcExport == true)
   {
      orc_ExportStatistic = mhc_ExportStatistic;
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fill up nodes of DBC network.

   Node names have to be "niceified" for DBC file export because they are
   instead of messages and signals not C-compliant.

   \param[in]     orc_CieNodes   input node data structure to export
   \param[out]    orc_DbcNodes   node data structure for DBC file export library

   \return
   C_NO_ERR    all nodes successfully set
   C_CONFIG    no node in network
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieExportDbc::mh_SetNodes(const std::vector<C_CieConverter::C_CieNode> & orc_CieNodes, std::map<std::string,
                                                                                                          Vector::DBC::Node> & orc_DbcNodes)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_CieNodes.size() > 0)
   {
      std::vector<C_CieConverter::C_CieNode>::const_iterator c_Iter;
      for (c_Iter = orc_CieNodes.begin(); c_Iter != orc_CieNodes.end(); ++c_Iter)
      {
         // key
         // the same content as value node name --> tested by example file (SLS AMG)
         C_SclString c_Niceified = mh_NiceifyStringForDbcSymbol(c_Iter->c_Properties.c_Name);
         mhc_NodeMapping.emplace(std::pair<C_SclString, C_SclString>(c_Iter->c_Properties.c_Name, c_Niceified));
         std::string c_Name = mh_EscapeCriticalSymbols(c_Niceified).c_str();

         // value
         Vector::DBC::Node c_Node; // store name and comment, we have no attribute values
         c_Node.name = c_Name;
         c_Node.comment = mh_EscapeCriticalSymbols(c_Iter->c_Properties.c_Comment).c_str();
         // store current key value entry to map
         orc_DbcNodes.emplace(std::pair<std::string, Vector::DBC::Node>(c_Name, c_Node));
      }
   }
   else
   {
      // strange case should not happen
      mhc_ErrorMessage = "No node in network for DBC file export.";
      osc_write_log_error("DBC file export", mhc_ErrorMessage);
      s32_Return = C_CONFIG;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fill up messages with signals.

   \param[in]     orc_CieNodes      input message data structure to export
   \param[out]    orc_DbcMessages   message data structure for DBC file export

   \return
   C_NO_ERR    all messages and signals successfully set
   C_WARN      cycle time is negative, perhaps uint32_t value does not fit in integer
   C_WARN      value type not supported
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieExportDbc::mh_SetMessages(const std::vector<C_CieConverter::C_CieNode> & orc_CieNodes, std::map<uint32_t,
                                                                                                             Vector::DBC::Message> & orc_DbcMessages)
{
   C_SclString c_Message;

   std::vector<C_CieConverter::C_CieNode>::const_iterator c_Iter;
   for (c_Iter = orc_CieNodes.begin(); c_Iter != orc_CieNodes.end(); ++c_Iter)
   {
      const stw::scl::C_SclString c_NodeName = mh_NiceifyStringForDbcSymbol(c_Iter->c_Properties.c_Name);

      // get Tx messages of node
      std::vector<C_CieConverter::C_CieNodeMessage>::const_iterator c_MsgIter;
      for (c_MsgIter = c_Iter->c_TxMessages.begin(); c_MsgIter != c_Iter->c_TxMessages.end(); ++c_MsgIter)
      {
         const C_CieConverter::C_CieCanMessage c_CanMessage = c_MsgIter->c_CanMessage;
         uint32_t u32_CanId = c_CanMessage.u32_CanId;
         // only add new CAN messages
         std::map<uint32_t, Vector::DBC::Message>::iterator c_DbcMsgIter;
         c_DbcMsgIter = orc_DbcMessages.find(u32_CanId);
         if (c_DbcMsgIter == orc_DbcMessages.end())
         {
            // new CAN message
            Vector::DBC::Message c_DbcMessage;
            if (c_CanMessage.q_IsExtended == true)
            {
               c_DbcMessage.id = static_cast<uint32_t>(u32_CanId | 0x80000000UL);
            }
            else
            {
               c_DbcMessage.id = u32_CanId;
            }
            c_DbcMessage.size = c_CanMessage.u16_Dlc;
            c_DbcMessage.comment = mh_EscapeCriticalSymbols(c_CanMessage.c_Comment).c_str();
            c_DbcMessage.name = c_CanMessage.c_Name.c_str();
            // store first transmitter for new message
            c_Message = "Setting node \"" + c_NodeName + "\" as transmitter for CAN message \"" +
                        c_CanMessage.c_Name + "\".";
            osc_write_log_info("DBC file export", c_Message);
            c_DbcMessage.transmitter = c_NodeName.c_str();

            // store signals with receivers
            c_Message = "Filling up signals and receivers for CAN Tx message \"" + c_CanMessage.c_Name + "\" ...";
            osc_write_log_info("DBC file export", c_Message);
            mh_SetSignals(c_CanMessage.c_Signals, orc_CieNodes, c_DbcMessage);

            // set transmission type for new CAN message
            mh_SetTransmission(*c_MsgIter, c_DbcMessage);

            // store new CAN message
            orc_DbcMessages.emplace(std::pair<uint32_t, Vector::DBC::Message>(u32_CanId, c_DbcMessage));

            c_Message = "CAN Tx message with transmitter and signals \"" + c_CanMessage.c_Name + "\" inserted.";
            osc_write_log_info("DBC file export", c_Message);
         }
         else
         {
            // Convention is that a CAN message can only have one (!) transmitter.
            // DBC library supports more than one transmitter, maybe in case by activating
            // different nodes, but this is not supported in openSYDE.
            c_Message = "Setting node \"" + c_NodeName + "\" as transmitter for CAN message \"" +
                        c_DbcMsgIter->second.name.c_str() + "\".";
            osc_write_log_info("DBC file export", c_Message);
            c_DbcMsgIter->second.transmitter = c_NodeName.c_str();
         }
      }
      // get Rx messages of node
      for (c_MsgIter = c_Iter->c_RxMessages.begin(); c_MsgIter != c_Iter->c_RxMessages.end(); ++c_MsgIter)
      {
         const C_CieConverter::C_CieCanMessage c_CanMessage = c_MsgIter->c_CanMessage;
         uint32_t u32_CanId = c_CanMessage.u32_CanId;
         // only add new CAN messages with signals and transceivers
         // (there is nothing in 'else case' to be done because we have already all information)
         std::map<uint32_t, Vector::DBC::Message>::iterator c_DbcMsgIter;
         c_DbcMsgIter = orc_DbcMessages.find(u32_CanId);
         if (c_DbcMsgIter == orc_DbcMessages.end())
         {
            // new CAN message
            Vector::DBC::Message c_DbcMessage;
            if (c_CanMessage.q_IsExtended == true)
            {
               c_DbcMessage.id = static_cast<uint32_t>(u32_CanId | 0x80000000UL);
            }
            else
            {
               c_DbcMessage.id = u32_CanId;
            }
            c_DbcMessage.size = c_CanMessage.u16_Dlc;
            c_DbcMessage.comment = mh_EscapeCriticalSymbols(c_CanMessage.c_Comment).c_str();
            c_DbcMessage.name = c_CanMessage.c_Name.c_str();

            // store signals with receivers
            c_Message = "Filling up signals and receivers for CAN Rx message \"" + c_CanMessage.c_Name + "\" ...";
            osc_write_log_info("DBC file export", c_Message);
            mh_SetSignals(c_CanMessage.c_Signals, orc_CieNodes, c_DbcMessage);

            // set transmission type for new CAN message
            mh_SetTransmission(*c_MsgIter, c_DbcMessage);

            // store new CAN message
            orc_DbcMessages.emplace(std::pair<uint32_t, Vector::DBC::Message>(u32_CanId, c_DbcMessage));

            c_Message = "CAN Rx message with receivers and signals \"" + c_CanMessage.c_Name + "\" inserted.";
            osc_write_log_info("DBC file export", c_Message);
         }
      }
   }

   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills up signals for one message of a node.

   Assumptions:
   * there is no consistency check for signals (must be secured by openSYDE GUI)

   \param[in]   orc_CieSignals   input CAN signals structure to export
   \param[in]   orc_CieNodes     input node data structure to export
   \param[out]  orc_DbcMessage   DBC message with signal data structure with receivers for DBC file export

   \return
   C_NO_ERR    all messages and signals successfully set
   C_WARN      value type of signal not supported
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieExportDbc::mh_SetSignals(const std::vector<C_CieConverter::C_CieCanSignal> & orc_CieSignals,
                                      const std::vector<C_CieConverter::C_CieNode> & orc_CieNodes,
                                      Vector::DBC::Message & orc_DbcMessage)
{
   int32_t s32_Return = C_NO_ERR;

   for (const auto c_CieSignal : orc_CieSignals)
   {
      Vector::DBC::Signal c_DbcSignal;
      // set byte order
      if (c_CieSignal.e_ComByteOrder == stw::opensyde_core::C_OscCanSignal::eBYTE_ORDER_INTEL)
      {
         c_DbcSignal.byteOrder = Vector::DBC::ByteOrder::Intel;
      }
      else
      {
         c_DbcSignal.byteOrder = Vector::DBC::ByteOrder::Motorola;
      }
      // set start bit pos and length
      c_DbcSignal.startBit = c_CieSignal.u16_ComBitStart;
      c_DbcSignal.bitSize = c_CieSignal.u16_ComBitLength;

      // set multiplexing information
      switch (c_CieSignal.e_MultiplexerType)
      {
      case C_OscCanSignal::eMUX_DEFAULT:
         c_DbcSignal.multiplexor = Vector::DBC::Signal::Multiplexor::NoMultiplexor;
         c_DbcSignal.multiplexerSwitchValue = 0;
         break;
      case C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL:
         c_DbcSignal.multiplexor = Vector::DBC::Signal::Multiplexor::MultiplexorSwitch;
         c_DbcSignal.multiplexerSwitchValue = 0;
         break;
      case C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL:
         c_DbcSignal.multiplexor = Vector::DBC::Signal::Multiplexor::MultiplexedSignal;
         c_DbcSignal.multiplexerSwitchValue = c_CieSignal.u16_MultiplexValue;
         break;
      default:
         tgl_assert(false);
         break;
      }

      C_CieExportDbc::mh_SetSignalSpnValue(c_CieSignal, c_DbcSignal);

      // set signal values
      const C_CieConverter::C_CieDataPoolElement & rc_Element = c_CieSignal.c_Element;
      s32_Return = mh_SetSignalValues(rc_Element, c_DbcSignal);
      tgl_assert(s32_Return == C_NO_ERR);

      if (s32_Return == C_NO_ERR)
      {
         const std::unordered_set<std::string> c_ReceiverNodes(c_DbcSignal.receivers.begin(),
                                                               c_DbcSignal.receivers.end());

         // fill up receivers for signal
         for (const auto & rc_Node : orc_CieNodes)
         {
            const std::string c_NodeName = mh_NiceifyStringForDbcSymbol(rc_Node.c_Properties.c_Name).c_str();
            if (c_ReceiverNodes.find(c_NodeName) == c_ReceiverNodes.end())
            {
               for (const auto & rc_Receiver : rc_Node.c_RxMessages)
               {
                  // if we have the same message, then check if message is receiver of signal
                  if (rc_Receiver.c_CanMessage.c_Name.AnsiCompare(orc_DbcMessage.name.c_str()) == 0)
                  {
                     const std::vector<C_CieConverter::C_CieCanSignal> & rc_Signals =
                        rc_Receiver.c_CanMessage.c_Signals;
                     for (const auto & rc_Signal : rc_Signals)
                     {
                        // check if node with Rx messages has signal
                        if (rc_Signal.c_Element.c_Name.AnsiCompare(c_DbcSignal.name.c_str()) == 0)
                        {
                           // receiver for signal found -> add node as receiver if not already exists
                           c_DbcSignal.receivers.insert(c_NodeName);
                        }
                     }
                  }
               }
            }
         }
         // got all information for signal, therefore store signal for message
         orc_DbcMessage.signals.emplace(std::pair<std::string, Vector::DBC::Signal>(c_DbcSignal.name, c_DbcSignal));
         mhc_ExportStatistic.u32_NumOfSignals++;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills up the content values of a signal.

   Assumptions:
   * there is no consistency check for signal values (must be secured by openSYDE GUI)

   \param[in]   orc_Element      input signal data structure to export
   \param[out]  orc_DbcSignal    values of a signal for DBC file export

   \return
   C_NO_ERR    all messages and signals successfully set
   C_WARN      value type not supported
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieExportDbc::mh_SetSignalValues(const C_CieConverter::C_CieDataPoolElement & orc_Element,
                                           Vector::DBC::Signal & orc_DbcSignal)
{
   int32_t s32_Return = C_NO_ERR;

   // set name, comment, values and unit
   orc_DbcSignal.name = orc_Element.c_Name.c_str();
   orc_DbcSignal.comment = mh_EscapeCriticalSymbols(orc_Element.c_Comment).c_str();
   orc_DbcSignal.factor = orc_Element.f64_Factor;
   orc_DbcSignal.offset = orc_Element.f64_Offset;
   orc_DbcSignal.unit = orc_Element.c_Unit.c_str();
   float64_t f64_MinVal;
   C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Element.c_MinValue, f64_MinVal, 0UL); // raw value
   orc_DbcSignal.minimum = orc_DbcSignal.rawToPhysicalValue(f64_MinVal);               // phy value
   float64_t f64_MaxVal;
   C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Element.c_MaxValue, f64_MaxVal, 0UL); // raw value
   orc_DbcSignal.maximum = orc_DbcSignal.rawToPhysicalValue(f64_MaxVal);               // phy value

   // set value type
   // the types of the min, max and init values must be the same!
   // this is guaranteed by the openSYDE system tool.
   const C_OscNodeDataPoolContent::E_Type e_CurrentType = orc_Element.c_MinValue.GetType();

   // value type
   if (e_CurrentType == C_OscNodeDataPoolContent::eUINT8)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Unsigned;
   }
   else if (e_CurrentType == C_OscNodeDataPoolContent::eSINT8)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OscNodeDataPoolContent::eUINT16)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Unsigned;
   }
   else if (e_CurrentType == C_OscNodeDataPoolContent::eSINT16)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OscNodeDataPoolContent::eUINT32)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Unsigned;
   }
   else if (e_CurrentType == C_OscNodeDataPoolContent::eSINT32)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OscNodeDataPoolContent::eUINT64)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Unsigned;
   }
   else if (e_CurrentType == C_OscNodeDataPoolContent::eSINT64)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OscNodeDataPoolContent::eFLOAT32)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Float;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OscNodeDataPoolContent::eFLOAT64)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Double;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else
   {
      s32_Return = C_WARN; // value type not supported
   }

   // set initial value
   Vector::DBC::Attribute c_Attribute;
   c_Attribute.name = mhc_SIG_INITIAL_VALUE;

   float64_t f64_Value;
   C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Element.c_DataSetValues.at(0), f64_Value, 0UL); // raw value

   //raw value shall be used for export
   if ((e_CurrentType == C_OscNodeDataPoolContent::eFLOAT32) || (e_CurrentType == C_OscNodeDataPoolContent::eFLOAT64))
   {
      c_Attribute.floatValue = f64_Value;
   }
   else
   {
      c_Attribute.integerValue = static_cast<int32_t>(f64_Value);
   }

   orc_DbcSignal.attributeValues.emplace(std::pair<std::string, Vector::DBC::Attribute>(c_Attribute.name, c_Attribute));

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set signal spn value

   \param[in]      orc_Signal       Signal
   \param[in,out]  orc_DbcSignal    Dbc signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportDbc::mh_SetSignalSpnValue(const C_CieConverter::C_CieCanSignal & orc_Signal,
                                          Vector::DBC::Signal & orc_DbcSignal)
{
   // set spn
   Vector::DBC::Attribute c_SpnAttribute;
   c_SpnAttribute.name = "SPN";
   c_SpnAttribute.integerValue = orc_Signal.u32_J1939Spn;
   orc_DbcSignal.attributeValues.emplace(std::pair<std::string,
                                                   Vector::DBC::Attribute>(c_SpnAttribute.
                                                                           name,
                                                                           c_SpnAttribute));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function sets transmission type for message.

   Hint: openSYDE only has the two transmission types 'Cyclic' and 'OnEvent'.

   \param[in]   orc_Message      input message data structure for message type
   \param[out]  orc_DbcMessage   output message data structure for DBC file export

   \return
   C_NO_ERR    transmission type of message successfully set
   C_WARN      cycle time is negative, perhaps uint32_t value does not fit in integer
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieExportDbc::mh_SetTransmission(const C_CieConverter::C_CieNodeMessage & orc_Message,
                                           Vector::DBC::Message & orc_DbcMessage)
{
   int32_t s32_Return = C_NO_ERR;
   C_SclString c_Message = "Setting transmission mode of message \"" + orc_Message.c_CanMessage.c_Name + "\"...";

   osc_write_log_info("DBC file export", c_Message);

   Vector::DBC::Attribute c_TransmissionMode;
   c_TransmissionMode.name = mhc_MSG_SEND_TYPE;
   if (C_OscCanMessage::h_IsTransmissionTypeOfCyclicType(orc_Message.c_CanMessage.e_TxMethod))
   {
      // cyclic message -> also set cycle time
      c_TransmissionMode.enumValue = 0;
      c_TransmissionMode.stringValue = "Cyclic";
   }
   else
   {
      c_TransmissionMode.enumValue = 1;
      c_TransmissionMode.stringValue = "OnEvent";
   }

   // set transmission type for DBC message
   orc_DbcMessage.attributeValues.emplace(std::pair<std::string, Vector::DBC::Attribute>(
                                             c_TransmissionMode.name, c_TransmissionMode));

   // set cycle time
   if (C_OscCanMessage::h_IsTransmissionTypeOfCyclicType(orc_Message.c_CanMessage.e_TxMethod))
   {
      Vector::DBC::Attribute c_CycleTime;
      c_CycleTime.name = mhc_MSG_CYCLE_TIME;
      int32_t s32_CycleTime = static_cast<int32_t>(orc_Message.c_CanMessage.u32_CycleTimeMs); // cast is OK,
      // because only expecting low cycle times of max. some seconds
      if (s32_CycleTime < 0)
      {
         s32_CycleTime = 0;
         c_Message = "Negative cycle time for CAN message \"" + orc_Message.c_CanMessage.c_Name +
                     "\". Cycle time set to 0.";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
         s32_Return = C_WARN;
      }
      c_CycleTime.integerValue = s32_CycleTime;

      orc_DbcMessage.attributeValues.emplace(std::pair<std::string, Vector::DBC::Attribute>(
                                                c_CycleTime.name, c_CycleTime));
   }

   c_Message = "Transmission mode set.";
   osc_write_log_info("DBC file export", c_Message);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills up the obligatory new symbol entries of a DBC file.

   \param[in,out]  orc_NewSymbols   symbol list to fill with standard entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportDbc::mh_SetNewSymbols(std::vector<std::string> & orc_NewSymbols)
{
   orc_NewSymbols.emplace_back("NS_DESC_");
   orc_NewSymbols.emplace_back("CM_");
   orc_NewSymbols.emplace_back("BA_DEF_");
   orc_NewSymbols.emplace_back("BA_");
   orc_NewSymbols.emplace_back("VAL_");
   orc_NewSymbols.emplace_back("CAT_DEF_");
   orc_NewSymbols.emplace_back("CAT_");
   orc_NewSymbols.emplace_back("FILTER");
   orc_NewSymbols.emplace_back("BA_DEF_DEF_");
   orc_NewSymbols.emplace_back("EV_DATA_");
   orc_NewSymbols.emplace_back("ENVVAR_DATA_");
   orc_NewSymbols.emplace_back("SGTYPE_");
   orc_NewSymbols.emplace_back("SGTYPE_VAL_");
   orc_NewSymbols.emplace_back("BA_DEF_SGTYPE_");
   orc_NewSymbols.emplace_back("BA_SGTYPE_");
   orc_NewSymbols.emplace_back("SIG_TYPE_REF_");
   orc_NewSymbols.emplace_back("VAL_TABLE_");
   orc_NewSymbols.emplace_back("SIG_GROUP_");
   orc_NewSymbols.emplace_back("SIG_VALTYPE_");
   orc_NewSymbols.emplace_back("SIGTYPE_VALTYPE_");
   orc_NewSymbols.emplace_back("BO_TX_BU_");
   orc_NewSymbols.emplace_back("BA_DEF_REL_");
   orc_NewSymbols.emplace_back("BA_REL_");
   orc_NewSymbols.emplace_back("BA_DEF_DEF_REL_");
   orc_NewSymbols.emplace_back("BU_SG_REL_");
   orc_NewSymbols.emplace_back("BU_EV_REL_");
   orc_NewSymbols.emplace_back("BU_BO_REL_");
   orc_NewSymbols.emplace_back("SG_MUL_VAL_");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills up the obligatory attribute default map.

   \param[in,out]  orc_AttributeDefaults  attribute default map to fill with standard values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportDbc::mh_SetAttributeDefaults(std::map<std::string, Vector::DBC::Attribute> & orc_AttributeDefaults)
{
   // bus type
   const std::string c_BusTypeName = "BusType";

   Vector::DBC::Attribute c_BusTypeAttribute;
   c_BusTypeAttribute.name = c_BusTypeName;
   c_BusTypeAttribute.stringValue = "";
   orc_AttributeDefaults.emplace(std::pair<std::string, Vector::DBC::Attribute>(c_BusTypeName, c_BusTypeAttribute));

   // database name
   const std::string c_DbName = "DBName";
   Vector::DBC::Attribute c_DbNameAttribute;
   c_DbNameAttribute.name = c_DbName;
   c_DbNameAttribute.stringValue = "";
   orc_AttributeDefaults.emplace(std::pair<std::string, Vector::DBC::Attribute>(c_DbName, c_DbNameAttribute));

   // general message cycle time
   const std::string c_MsgCycleTimeName = "GenMsgCycleTime";
   Vector::DBC::Attribute c_MsgCycleTimeAttribute;
   c_MsgCycleTimeAttribute.name = c_MsgCycleTimeName;
   c_MsgCycleTimeAttribute.integerValue = 0;
   orc_AttributeDefaults.emplace(std::pair<std::string, Vector::DBC::Attribute>(c_MsgCycleTimeName,
                                                                                c_MsgCycleTimeAttribute));

   // general message send type
   const std::string c_MsgSendTypeName = "GenMsgSendType";
   Vector::DBC::Attribute c_MsgSendTypeAttribute;
   c_MsgSendTypeAttribute.name = c_MsgSendTypeName;
   c_MsgSendTypeAttribute.stringValue = "NoMsgSendType";
   orc_AttributeDefaults.emplace(std::pair<std::string, Vector::DBC::Attribute>(c_MsgSendTypeName,
                                                                                c_MsgSendTypeAttribute));

   // general signal inactive value
   const std::string c_SigInactiveName = "GenSigInactiveValue";
   Vector::DBC::Attribute c_SigInactiveAttribute;
   c_SigInactiveAttribute.name = c_SigInactiveName;
   c_SigInactiveAttribute.integerValue = 0;
   orc_AttributeDefaults.emplace(std::pair<std::string, Vector::DBC::Attribute>(c_SigInactiveName,
                                                                                c_SigInactiveAttribute));

   // general signal send type
   const std::string c_SigSendTypeName = "GenSigSendType";
   Vector::DBC::Attribute c_SigSendTypeAttribute;
   c_SigSendTypeAttribute.name = c_SigSendTypeName;
   c_SigSendTypeAttribute.stringValue = "Cyclic";
   orc_AttributeDefaults.emplace(std::pair<std::string, Vector::DBC::Attribute>(c_SigSendTypeName,
                                                                                c_SigSendTypeAttribute));

   // network management station address
   // do not know if I have to set this, but in default configuration with one node this variable is set
   // from https://vector.com/portal/medien/cmc/manuals/CANisterConfigurator_Manual_EN.pdf
   // "NmStationAddress: The number of the control unit for network management (node attribute)."
   const std::string c_NmStationName = "NmStationAddress";
   Vector::DBC::Attribute c_NmStationAttribute;
   c_NmStationAttribute.name = c_NmStationName;
   c_NmStationAttribute.hexValue = 0;
   orc_AttributeDefaults.emplace(std::pair<std::string, Vector::DBC::Attribute>(c_NmStationName, c_NmStationAttribute));

   // set spn
   std::string c_SpnName = "SPN";
   Vector::DBC::Attribute c_SpnAttribute;
   c_SpnAttribute.name = c_SpnName;
   c_SpnAttribute.integerValue = 0;
   orc_AttributeDefaults.emplace(std::pair<std::string, Vector::DBC::Attribute>(
                                    c_SpnName, c_SpnAttribute));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills up the obligatory attribute definition map.

   \param[in,out]  orc_AttributeDefinitions  attribute definition map to fill with standard values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportDbc::mh_SetAttributeDefinitions(std::map<std::string,
                                                         Vector::DBC::AttributeDefinition> & orc_AttributeDefinitions)
{
   // set bus type
   std::string c_BusTypeName = "BusType";
   Vector::DBC::AttributeDefinition c_BusTypeAttributeDef;
   c_BusTypeAttributeDef.name = c_BusTypeName;
   c_BusTypeAttributeDef.objectType = Vector::DBC::AttributeObjectType::Network;
   c_BusTypeAttributeDef.valueType.type = Vector::DBC::AttributeValueType::Type::String;
   orc_AttributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                       c_BusTypeName, c_BusTypeAttributeDef));

   // set database name
   std::string c_DbName = "DBName";
   Vector::DBC::AttributeDefinition c_DbNameAttributeDef;
   c_DbNameAttributeDef.name = c_DbName;
   c_DbNameAttributeDef.objectType = Vector::DBC::AttributeObjectType::Network;
   c_DbNameAttributeDef.valueType.type = Vector::DBC::AttributeValueType::Type::String;
   c_DbNameAttributeDef.valueType.enumValues.emplace_back("");
   orc_AttributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                       c_DbName, c_DbNameAttributeDef));

   // set global attribute definition for cycle time (default)
   Vector::DBC::AttributeDefinition c_CycleTimeAttributeDef;
   c_CycleTimeAttributeDef.name = mhc_MSG_CYCLE_TIME;
   c_CycleTimeAttributeDef.objectType = Vector::DBC::AttributeObjectType::Message;
   c_CycleTimeAttributeDef.valueType.type = Vector::DBC::AttributeValueType::Type::Int;
   orc_AttributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                       c_CycleTimeAttributeDef.name, c_CycleTimeAttributeDef));

   // set global attribute definitions for openSYDE message send types 'Cyclic' and 'OnEvent'
   Vector::DBC::AttributeDefinition c_MsgSendTypeAttributeDef;
   c_MsgSendTypeAttributeDef.name = mhc_MSG_SEND_TYPE;
   c_MsgSendTypeAttributeDef.objectType = Vector::DBC::AttributeObjectType::Message;
   c_MsgSendTypeAttributeDef.valueType.type = Vector::DBC::AttributeValueType::Type::Enum;
   c_MsgSendTypeAttributeDef.valueType.enumValues.emplace_back("Cyclic");
   c_MsgSendTypeAttributeDef.valueType.enumValues.emplace_back("OnEvent");
   orc_AttributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                       c_MsgSendTypeAttributeDef.name, c_MsgSendTypeAttributeDef));

   // set general signal inactive value
   std::string c_SigInactiveValueName = "GenSigInactiveValue";
   Vector::DBC::AttributeDefinition c_SigInactiveValueAttributeDef;
   c_SigInactiveValueAttributeDef.name = c_SigInactiveValueName;
   c_SigInactiveValueAttributeDef.objectType = Vector::DBC::AttributeObjectType::Signal;
   c_SigInactiveValueAttributeDef.valueType.type = Vector::DBC::AttributeValueType::Type::Int;
   orc_AttributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                       c_SigInactiveValueName, c_SigInactiveValueAttributeDef));

   // set general signal send type
   std::string c_SigSendTypeName = "GenSigSendType";
   Vector::DBC::AttributeDefinition c_SigSendTypeAttributeDef;
   c_SigSendTypeAttributeDef.name = c_SigSendTypeName;
   c_SigSendTypeAttributeDef.objectType = Vector::DBC::AttributeObjectType::Signal;
   c_SigSendTypeAttributeDef.valueType.type = Vector::DBC::AttributeValueType::Type::Enum;

   c_SigSendTypeAttributeDef.valueType.enumValues.emplace_back("NoSigSendType");
   orc_AttributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                       c_SigSendTypeName, c_SigSendTypeAttributeDef));

   // set network management station address
   // do not know if I have to set this, but in default configuration with one node this variable is set
   // from https://vector.com/portal/medien/cmc/manuals/CANisterConfigurator_Manual_EN.pdf
   // "NmStationAddress: The number of the control unit for network management (node attribute)."
   std::string c_NmStationName = "NmStationAddress";
   Vector::DBC::AttributeDefinition c_NmStationAttributeDef;
   c_NmStationAttributeDef.name = c_NmStationName;
   c_NmStationAttributeDef.objectType = Vector::DBC::AttributeObjectType::Node;
   c_NmStationAttributeDef.valueType.type = Vector::DBC::AttributeValueType::Type::Hex;
   orc_AttributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                       c_NmStationName, c_NmStationAttributeDef));

   // set initial value
   std::string c_SigInitialValueName = mhc_SIG_INITIAL_VALUE;
   Vector::DBC::AttributeDefinition c_SigInitialValueAttribute;
   c_SigInitialValueAttribute.name = c_SigInitialValueName;
   c_SigInitialValueAttribute.objectType = Vector::DBC::AttributeObjectType::Signal;
   c_SigInitialValueAttribute.valueType.type = Vector::DBC::AttributeValueType::Type::Int; // I am not sure, but setting
                                                                                           // this
                                                                                           // to
                                                                                           // integer because we have
                                                                                           // raw
                                                                                           // values
                                                                                           // here
   orc_AttributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                       c_SigInitialValueName, c_SigInitialValueAttribute));

   // set spn
   std::string c_SpnName = "SPN";
   Vector::DBC::AttributeDefinition c_SpnAttributeDef;
   c_SpnAttributeDef.name = c_SpnName;
   c_SpnAttributeDef.objectType = Vector::DBC::AttributeObjectType::Signal;
   c_SpnAttributeDef.valueType.type = Vector::DBC::AttributeValueType::Type::Int;
   c_SpnAttributeDef.valueType.integerValue.minimum = 0;
   c_SpnAttributeDef.valueType.integerValue.maximum = 524287;
   orc_AttributeDefinitions.emplace(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                       c_SpnName, c_SpnAttributeDef));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Replace special characters in string for DBC files.

   Aims:
   * convert into strings that can be used for DBC files as symbol names
   * prevent accidentally rendering unique names identical
     (e.g.: "ITEM!§" and "ITEM%&" should not result in the same string)

   Symbol names in DBC files must only contain digits, alphabetic letters and
   underscores '_'.

   First digit must not be a digit and in this case an underscore '_' is placed
   before.

   As a result the length of the string might change.

   \param[in]  orc_String  Original string

   \return
   Niceified string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CieExportDbc::mh_NiceifyStringForDbcSymbol(const C_SclString & orc_String)
{
   C_SclString c_Result;

   // first character must not contain a digit, in this case place '_' before digit.
   if (std::isdigit(orc_String.c_str()[0]) != 0)
   {
      // first character is digit, add '_'
      c_Result = '_';
   }

   for (uint32_t u32_Index = 0U; u32_Index < orc_String.Length(); u32_Index++)
   {
      const char_t cn_Character = orc_String.c_str()[u32_Index];
      if ((std::isalnum(cn_Character) == 0) && (cn_Character != '_'))
      {
         c_Result += C_SclString::IntToStr(cn_Character);
      }
      else
      {
         c_Result += cn_Character;
      }
   }
   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle/escape critical symbols for DBC

   Current:
   Only handle " character

   Cf. C_CieImportDbc::mh_ReEscapeCriticalSymbols for counter part.

   \param[in] orc_String String to escape

   \return
   Escaped string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CieExportDbc::mh_EscapeCriticalSymbols(const C_SclString & orc_String)
{
   C_SclString c_Retval;

   for (int32_t s32_Char = 0; s32_Char < static_cast<int32_t>(orc_String.Length()); ++s32_Char)
   {
      const char_t cn_Char = orc_String[static_cast<uint32_t>(s32_Char + 1)];
      if (cn_Char == '\"')
      {
         c_Retval += "\\\"";
      }
      else
      {
         c_Retval += cn_Char;
      }
   }
   return c_Retval;
}
