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
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_CieExportDbc.h"
#include "CSCLString.h"
#include "DBC.h"
#include "C_CieConverter.h"
#include "C_OSCCanSignal.h"
#include "C_OSCNodeDataPoolContent.h"
#include "TGLFile.h"
#include "DBC/Status.h"
#include "TGLUtils.h"
#include "C_SdNdeDataPoolContentUtil.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_tgl;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const std::string C_CieExportDbc::mhc_SigInitialValue = "GenSigStartValue";
const std::string C_CieExportDbc::mhc_MsgCycleTime = "GenMsgCycleTime";
const std::string C_CieExportDbc::mhc_MsgSendType = "GenMsgSendType";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */
stw_scl::C_SCLStringList C_CieExportDbc::mhc_WarningMessages;          // global warnings e.g. why some messages could
                                                                       // not be exported
stw_scl::C_SCLString C_CieExportDbc::mhc_ErrorMessage;                 // description of error which caused the export
                                                                       // to fail
std::map<C_SCLString, C_SCLString> C_CieExportDbc::mhc_NodeMapping;    // to receive niceified names after export to DBC
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

   \param[in]     orc_File              output DBC file (full file path required)
   \param[in]     orc_Definition        content for DBC file
   \param[out]    orc_WarningMessages   warning messages after export
   \param[out]    orc_ErrorMessage      error message after export

   \return
   C_NO_ERR        successful export to DBC file
   C_WARN          cycle time is negative, perhaps uint32 value does not fit in integer
                   value type of signal not supported
                   warning on writing to DBC file (see log for specific details)
   C_CONFIG        path of directory for DBC file does not exist
                   no node in network for DBC file export
   C_UNKNOWN_ERR   general error when writing to DBC file, no more details
   C_BUSY          error on writing to DBC file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieExportDbc::h_ExportNetwork(const stw_scl::C_SCLString & orc_File,
                                       const C_CieConverter::C_CIECommDefinition & orc_Definition,
                                       stw_scl::C_SCLStringList & orc_WarningMessages,
                                       stw_scl::C_SCLString & orc_ErrorMessage)
{
   sint32 s32_Return = C_NO_ERR;
   C_SCLString c_Message;

   Vector::DBC::Network c_DbcNetwork;
   Vector::DBC::File c_File;

   mhc_WarningMessages.Clear(); // clear old warning messages for this export
   mhc_ErrorMessage = "";       // clear old error message for this export
   mhc_NodeMapping.clear();     // clear old node mapping for this export
   mhq_ValidDbcExport = false;  // only true if DBC file export was successful
   // reset export statistics
   mhc_ExportStatistic.u32_NumOfMessages = 0;
   mhc_ExportStatistic.u32_NumOfSignals = 0;

   // check file path
   if (TGL_DirectoryExists(TGL_ExtractFilePath(orc_File)) == false)
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
      c_DbNameAttribute.name = "DBName";
      c_DbNameAttribute.stringValue = mh_EscapeCriticalSymbols(orc_Definition.c_Bus.c_Name).c_str();
      c_DbNameAttribute.valueType = Vector::DBC::AttributeValueType::String;
      c_DbcNetwork.attributeValues.insert(std::pair<std::string, Vector::DBC::Attribute>(
                                             c_DbName, c_DbNameAttribute));
      c_DbcNetwork.comment = std::string(mh_EscapeCriticalSymbols(orc_Definition.c_Bus.c_Comment).c_str());

      c_Message = "Filling up general network information, symbols and attributes for network \"" +
                  orc_Definition.c_Bus.c_Name + "\" ...";
      osc_write_log_info("DBC file export", c_Message);

      // set symbols and attributes
      mh_SetNewSymbols(c_DbcNetwork.newSymbols);
      mh_SetAttributeDefaults(c_DbcNetwork.attributeDefaults);
      mh_SetAttributeDefinitions(c_DbcNetwork.attributeDefinitions);
   }

   // set nodes
   if (s32_Return == C_NO_ERR)
   {
      c_Message = "Inserting nodes for network ...";
      osc_write_log_info("DBC file export", c_Message);
      s32_Return = mh_SetNodes(orc_Definition.c_Nodes, c_DbcNetwork.nodes);
   }

   // set messages
   if (s32_Return == C_NO_ERR)
   {
      c_Message = "Inserting messages for network ...";
      osc_write_log_info("DBC file export", c_Message);
      s32_Return = mh_SetMessages(orc_Definition.c_Nodes, c_DbcNetwork.messages);
      mhc_ExportStatistic.u32_NumOfMessages = c_DbcNetwork.messages.size(); // in any case
   }

   // save DBC export to file
   if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
   {
      c_Message = "Saving network to file ...";
      osc_write_log_info("DBC file export", c_Message);
      Vector::DBC::Status c_Status = c_File.save(c_DbcNetwork, orc_File.c_str());

      sint32 s32_Tmp = mh_CheckDbcFileStatus(c_Status);

      if (s32_Tmp != C_NO_ERR)
      {
         s32_Return = s32_Tmp; // C_WARN or worse
      }
   }

   // set status flag of this export
   if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
   {
      mhq_ValidDbcExport = true;
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
sint32 C_CieExportDbc::h_GetNodeMapping(std::map<C_SCLString, C_SCLString> & orc_NodeMapping)
{
   sint32 s32_Return = C_NOACT;

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
sint32 C_CieExportDbc::h_GetExportStatistic(C_ExportStatistic & orc_ExportStatistic)
{
   sint32 s32_Return = C_NOACT;

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

   \param[in]     orc_CIENodes   input node data structure to export
   \param[out]    orc_DBCNodes   node data structure for DBC file export library

   \return
   C_NO_ERR    all nodes successfully set
   C_CONFIG    no node in network
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieExportDbc::mh_SetNodes(const std::vector<C_CieConverter::C_CIENode> & orc_CIENodes, std::map<std::string,
                                                                                                         Vector::DBC::Node> & orc_DBCNodes)
{
   sint32 s32_Return = C_NO_ERR;

   if (orc_CIENodes.size() > 0)
   {
      std::vector<C_CieConverter::C_CIENode>::const_iterator c_Iter;
      for (c_Iter = orc_CIENodes.begin(); c_Iter != orc_CIENodes.end(); ++c_Iter)
      {
         // key
         // the same content as value node name --> tested by example file (SLS AMG)
         C_SCLString c_Niceified = mh_NiceifyStringForDbcSymbol(c_Iter->c_Properties.c_Name);
         mhc_NodeMapping.insert(std::pair<C_SCLString, C_SCLString>(c_Iter->c_Properties.c_Name, c_Niceified));
         std::string c_Name = mh_EscapeCriticalSymbols(c_Niceified).c_str();

         // value
         Vector::DBC::Node c_Node; // store name and comment, we have no attribute values
         c_Node.name = c_Name;
         c_Node.comment = mh_EscapeCriticalSymbols(c_Iter->c_Properties.c_Comment).c_str();
         // store current key value entry to map
         orc_DBCNodes.insert(std::pair<std::string, Vector::DBC::Node>(c_Name, c_Node));
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

   \param[in]     orc_CIENodes      input message data structure to export
   \param[out]    orc_DBCMessages   message data structure for DBC file export

   \return
   C_NO_ERR    all messages and signals successfully set
   C_WARN      cycle time is negative, perhaps uint32 value does not fit in integer
   C_WARN      value type not supported
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieExportDbc::mh_SetMessages(const std::vector<C_CieConverter::C_CIENode> & orc_CIENodes, std::map<uintn,
                                                                                                            Vector::DBC::Message> & orc_DBCMessages)
{
   C_SCLString c_Message;

   std::vector<C_CieConverter::C_CIENode>::const_iterator c_Iter;
   for (c_Iter = orc_CIENodes.begin(); c_Iter != orc_CIENodes.end(); ++c_Iter)
   {
      const stw_scl::C_SCLString c_NodeName = mh_NiceifyStringForDbcSymbol(c_Iter->c_Properties.c_Name);

      // get Tx messages of node
      std::vector<C_CieConverter::C_CIENodeMessage>::const_iterator c_MsgIter;
      for (c_MsgIter = c_Iter->c_TxMessages.begin(); c_MsgIter != c_Iter->c_TxMessages.end(); ++c_MsgIter)
      {
         C_CieConverter::C_CIECanMessage c_CanMessage = c_MsgIter->c_CanMessage;
         uint32 u32_CanId = c_CanMessage.u32_CanId;
         // only add new CAN messages
         std::map<uintn, Vector::DBC::Message>::iterator c_DBCMsgIter;
         c_DBCMsgIter = orc_DBCMessages.find(u32_CanId);
         if (c_DBCMsgIter == orc_DBCMessages.end())
         {
            // new CAN message
            Vector::DBC::Message c_DBCMessage;
            if (c_CanMessage.q_IsExtended == true)
            {
               c_DBCMessage.id = u32_CanId | 0x80000000UL;
            }
            else
            {
               c_DBCMessage.id = u32_CanId;
            }
            c_DBCMessage.size = c_CanMessage.u16_Dlc;
            c_DBCMessage.comment = mh_EscapeCriticalSymbols(c_CanMessage.c_Comment).c_str();
            c_DBCMessage.name = c_CanMessage.c_Name.c_str();
            // store first transmitter for new message
            c_Message = "Setting node \"" + c_NodeName + "\" as transmitter for CAN message \"" +
                        c_CanMessage.c_Name + "\".";
            osc_write_log_info("DBC file export", c_Message);
            c_DBCMessage.transmitter = c_NodeName.c_str();

            // store signals with receivers
            c_Message = "Filling up signals and receivers for CAN Tx message \"" + c_CanMessage.c_Name + "\" ...";
            osc_write_log_info("DBC file export", c_Message);
            mh_SetSignals(c_CanMessage.c_Signals, orc_CIENodes, c_DBCMessage);

            // set transmission type for new CAN message
            mh_SetTransmission(*c_MsgIter, c_DBCMessage);

            // store new CAN message
            orc_DBCMessages.insert(std::pair<uintn, Vector::DBC::Message>(u32_CanId, c_DBCMessage));

            c_Message = "CAN Tx message with transmitter and signals \"" + c_CanMessage.c_Name + "\" inserted.";
            osc_write_log_info("DBC file export", c_Message);
         }
         else
         {
            // Convention is that a CAN message can only have one (!) transmitter.
            // DBC library supports more than one transmitter, maybe in case by activating
            // different nodes, but this is not supported in openSYDE.
            c_Message = "Setting node \"" + c_NodeName + "\" as transmitter for CAN message \"" +
                        C_SCLString(c_DBCMsgIter->second.name.c_str()) + "\".";
            osc_write_log_info("DBC file export", c_Message);
            c_DBCMsgIter->second.transmitter = c_NodeName.c_str();
         }
      }
      // get Rx messages of node
      for (c_MsgIter = c_Iter->c_RxMessages.begin(); c_MsgIter != c_Iter->c_RxMessages.end(); ++c_MsgIter)
      {
         C_CieConverter::C_CIECanMessage c_CanMessage = c_MsgIter->c_CanMessage;
         uint32 u32_CanId = c_CanMessage.u32_CanId;
         // only add new CAN messages with signals and transceivers
         // (there is nothing in 'else case' to be done because we have already all information)
         std::map<uintn, Vector::DBC::Message>::iterator c_DBCMsgIter;
         c_DBCMsgIter = orc_DBCMessages.find(u32_CanId);
         if (c_DBCMsgIter == orc_DBCMessages.end())
         {
            // new CAN message
            Vector::DBC::Message c_DBCMessage;
            if (c_CanMessage.q_IsExtended == true)
            {
               c_DBCMessage.id = u32_CanId | 0x80000000UL;
            }
            else
            {
               c_DBCMessage.id = u32_CanId;
            }
            c_DBCMessage.size = c_CanMessage.u16_Dlc;
            c_DBCMessage.comment = mh_EscapeCriticalSymbols(c_CanMessage.c_Comment).c_str();
            c_DBCMessage.name = c_CanMessage.c_Name.c_str();

            // store signals with receivers
            c_Message = "Filling up signals and receivers for CAN Rx message \"" + c_CanMessage.c_Name + "\" ...";
            osc_write_log_info("DBC file export", c_Message);
            mh_SetSignals(c_CanMessage.c_Signals, orc_CIENodes, c_DBCMessage);

            // set transmission type for new CAN message
            mh_SetTransmission(*c_MsgIter, c_DBCMessage);

            // store new CAN message
            orc_DBCMessages.insert(std::pair<uintn, Vector::DBC::Message>(u32_CanId, c_DBCMessage));

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

   \param[in]     orc_CIESignals      input CAN signals structure to export
   \param[in]     orc_RxMessages      input CAN node structure to get signal receivers
   \param[out]    orc_DBCMessage      DBC message with signal data structure with receivers for DBC file export

   \return
   C_NO_ERR    all messages and signals successfully set
   C_WARN      value type of signal not supported
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieExportDbc::mh_SetSignals(const std::vector<C_CieConverter::C_CIECanSignal> & orc_CIESignals,
                                     const std::vector<C_CieConverter::C_CIENode> & orc_CIENodes,
                                     Vector::DBC::Message & orc_DBCMessage)
{
   sint32 s32_Return = C_NO_ERR;

   for (auto c_CIESignal : orc_CIESignals)
   {
      Vector::DBC::Signal c_DBCSignal;
      // set byte order
      if (c_CIESignal.e_ComByteOrder == stw_opensyde_core::C_OSCCanSignal::eBYTE_ORDER_INTEL)
      {
         c_DBCSignal.byteOrder = Vector::DBC::ByteOrder::Intel;
      }
      else
      {
         c_DBCSignal.byteOrder = Vector::DBC::ByteOrder::Motorola;
      }
      // set start bit pos and length
      c_DBCSignal.startBit = c_CIESignal.u16_ComBitStart;
      c_DBCSignal.bitSize = c_CIESignal.u16_ComBitLength;

      // set multiplexing information
      switch (c_CIESignal.e_MultiplexerType)
      {
      case C_OSCCanSignal::eMUX_DEFAULT:
         c_DBCSignal.multiplexedSignal = false;
         c_DBCSignal.multiplexorSwitch = false;
         c_DBCSignal.multiplexerSwitchValue = 0;
         break;
      case C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL:
         c_DBCSignal.multiplexedSignal = false;
         c_DBCSignal.multiplexorSwitch = true;
         c_DBCSignal.multiplexerSwitchValue = 0;
         break;
      case C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL:
         c_DBCSignal.multiplexedSignal = true;
         c_DBCSignal.multiplexorSwitch = false;
         c_DBCSignal.multiplexerSwitchValue = c_CIESignal.u16_MultiplexValue;
         break;
      }

      // set signal values
      C_CieConverter::C_CIEDataPoolElement & c_Element = c_CIESignal.c_Element;
      s32_Return = mh_SetSignalValues(c_Element, c_DBCSignal);
      tgl_assert(s32_Return == C_NO_ERR);

      if (s32_Return == C_NO_ERR)
      {
         // fill up receivers for signal
         for (auto c_Node : orc_CIENodes)
         {
            const std::string c_NodeName = mh_NiceifyStringForDbcSymbol(c_Node.c_Properties.c_Name).c_str();
            for (auto c_Receiver : c_Node.c_RxMessages)
            {
               // if we have the same message, then check if message is receiver of signal
               if (c_Receiver.c_CanMessage.c_Name.AnsiCompare(stw_scl::C_SCLString(orc_DBCMessage.name.c_str())) == 0)
               {
                  std::vector<C_CieConverter::C_CIECanSignal> & c_Signals = c_Receiver.c_CanMessage.c_Signals;
                  for (auto c_Signal : c_Signals)
                  {
                     // check if node with Rx messages has signal
                     if (c_Signal.c_Element.c_Name.AnsiCompare(stw_scl::C_SCLString(c_DBCSignal.name.c_str())) == 0)
                     {
                        // receiver for signal found -> add node as receiver if not already exists
                        std::set<std::string>::iterator c_Iter = c_DBCSignal.receivers.find(c_NodeName);
                        if (c_Iter == c_DBCSignal.receivers.end())
                        {
                           c_DBCSignal.receivers.insert(c_NodeName);
                        }
                     }
                  }
               }
            }
         }
         // got all information for signal, therefore store signal for message
         orc_DBCMessage.signals.insert(std::pair<std::string, Vector::DBC::Signal>(c_DBCSignal.name, c_DBCSignal));
         mhc_ExportStatistic.u32_NumOfSignals++;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills up the content values of a signal.

   Assumptions:
   * there is no consistency check for signal values (must be secured by openSYDE GUI)

   \param[in]     orc_Element    input signal data structure to export
   \param[out]    orc_DbcSignal  values of a signal for DBC file export

   \return
   C_NO_ERR    all messages and signals successfully set
   C_WARN      value type not supported
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieExportDbc::mh_SetSignalValues(const C_CieConverter::C_CIEDataPoolElement & orc_Element,
                                          Vector::DBC::Signal & orc_DbcSignal)
{
   sint32 s32_Return = C_NO_ERR;

   // set name, comment, values and unit
   orc_DbcSignal.name = orc_Element.c_Name.c_str();
   orc_DbcSignal.comment = mh_EscapeCriticalSymbols(orc_Element.c_Comment).c_str();
   orc_DbcSignal.factor = orc_Element.f64_Factor;
   orc_DbcSignal.offset = orc_Element.f64_Offset;
   orc_DbcSignal.unit = orc_Element.c_Unit.c_str();
   float64 f64_MinVal;
   C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(orc_Element.c_MinValue, f64_MinVal); // raw value
   orc_DbcSignal.minimumPhysicalValue = orc_DbcSignal.rawToPhysicalValue(f64_MinVal);   // phy value
   float64 f64_MaxVal;
   C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(orc_Element.c_MaxValue, f64_MaxVal); // raw value
   orc_DbcSignal.maximumPhysicalValue = orc_DbcSignal.rawToPhysicalValue(f64_MaxVal);   // phy value

   // set value type
   // the types of the min, max and init values must be the same!
   // this is guaranteed by the openSYDE system tool.
   C_OSCNodeDataPoolContent::E_Type e_CurrentType = orc_Element.c_MinValue.GetType();

   // value type
   if (e_CurrentType == C_OSCNodeDataPoolContent::eUINT8)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Unsigned;
   }
   else if (e_CurrentType == C_OSCNodeDataPoolContent::eSINT8)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OSCNodeDataPoolContent::eUINT16)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Unsigned;
   }
   else if (e_CurrentType == C_OSCNodeDataPoolContent::eSINT16)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OSCNodeDataPoolContent::eUINT32)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Unsigned;
   }
   else if (e_CurrentType == C_OSCNodeDataPoolContent::eSINT32)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OSCNodeDataPoolContent::eUINT64)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Unsigned;
   }
   else if (e_CurrentType == C_OSCNodeDataPoolContent::eSINT64)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OSCNodeDataPoolContent::eFLOAT32)
   {
      orc_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Float;
      orc_DbcSignal.valueType = Vector::DBC::ValueType::Signed;
   }
   else if (e_CurrentType == C_OSCNodeDataPoolContent::eFLOAT64)
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
   c_Attribute.name = mhc_SigInitialValue;

   float64 f64_Value;
   C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(orc_Element.c_DataSetValues.at(0), f64_Value); // raw value
   f64_Value = orc_DbcSignal.rawToPhysicalValue(f64_Value);                                       // phy value
   if ((e_CurrentType == C_OSCNodeDataPoolContent::eFLOAT32) || (e_CurrentType == C_OSCNodeDataPoolContent::eFLOAT64))
   {
      c_Attribute.floatValue = f64_Value;
   }
   else
   {
      c_Attribute.integerValue = static_cast<sintn>(f64_Value);
   }

   orc_DbcSignal.attributeValues.insert(std::pair<std::string, Vector::DBC::Attribute>(c_Attribute.name, c_Attribute));

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function sets transmission type for message.

   Hint: openSYDE only has the two transmission types 'Cyclic' and 'OnEvent'.

   \param[in]     orc_Message      input message data structure for message type
   \param[out]    orc_DbcMessage   output message data structure for DBC file export

   \return
   C_NO_ERR    transmission type of message successfully set
   C_WARN      cycle time is negative, perhaps uint32 value does not fit in integer
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieExportDbc::mh_SetTransmission(const C_CieConverter::C_CIENodeMessage & orc_Message,
                                          Vector::DBC::Message & orc_DbcMessage)
{
   sint32 s32_Return = C_NO_ERR;
   C_SCLString c_Message = "Setting transmission mode of message \"" + orc_Message.c_CanMessage.c_Name + "\"...";

   osc_write_log_info("DBC file export", c_Message);

   Vector::DBC::Attribute c_TransmissionMode;
   c_TransmissionMode.name = mhc_MsgSendType;
   if (orc_Message.c_CanMessage.e_TxMethod == C_OSCCanMessage::E_TxMethodType::eTX_METHOD_CYCLIC)
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
   orc_DbcMessage.attributeValues.insert(std::pair<std::string, Vector::DBC::Attribute>(
                                            c_TransmissionMode.name, c_TransmissionMode));

   // set cycle time
   if (orc_Message.c_CanMessage.e_TxMethod == C_OSCCanMessage::E_TxMethodType::eTX_METHOD_CYCLIC)
   {
      Vector::DBC::Attribute c_CycleTime;
      c_CycleTime.name = mhc_MsgCycleTime;
      sintn sn_CycleTime = static_cast<sintn>(orc_Message.c_CanMessage.u32_CycleTimeMs); // cast is OK,
      // because only expecting low cycle times of max. some seconds
      if (sn_CycleTime < 0)
      {
         sn_CycleTime = 0;
         c_Message = "Negative cycle time for CAN message \"" + orc_Message.c_CanMessage.c_Name +
                     "\". Cycle time set to 0.";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
         s32_Return = C_WARN;
      }
      c_CycleTime.integerValue = sn_CycleTime;

      orc_DbcMessage.attributeValues.insert(std::pair<std::string, Vector::DBC::Attribute>(
                                               c_CycleTime.name, c_CycleTime));
   }

   c_Message = "Transmission mode set.";
   osc_write_log_info("DBC file export", c_Message);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Interprets status of Vector DBC library.

   If any warning or error, then global warning list or error string is filled.

   \param[in]     orc_Status     status of Vector DBC library

   \return
   C_NO_ERR        no error
   C_UNKNOWN_ERR   general error, no more details
   C_BUSY          error on writing to file
   C_WARN          see global warning list
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieExportDbc::mh_CheckDbcFileStatus(const Vector::DBC::Status & orc_Status)
{
   sint32 s32_Return;

   osc_write_log_info("DBC file export", "Interpreting status of Vector DBC file...");

   if (orc_Status == Vector::DBC::Status::Ok)
   {
      s32_Return = C_NO_ERR;
      osc_write_log_info("DBC file export", "Status of Vector DBC file OK.");
   }
   else if (orc_Status == Vector::DBC::Status::Error)
   {
      s32_Return = C_UNKNOWN_ERR;
      mhc_ErrorMessage = "Status error message of DBC file export: \"General Error\".";
      osc_write_log_error("DBC file export", mhc_ErrorMessage);
   }
   else if (orc_Status == Vector::DBC::Status::FileOpenError)
   {
      s32_Return = C_BUSY;
      mhc_ErrorMessage = "Status error message of DBC file export: \"Error on file open\".";
      osc_write_log_error("DBC file export", mhc_ErrorMessage);
   }
   else
   {
      // warnings
      s32_Return = C_WARN;

      C_SCLString c_Message = "Status warning message(s) of DBC file export: ";
      mhc_WarningMessages.Append(c_Message);
      osc_write_log_warning("DBC file export", c_Message);

      if (orc_Status == Vector::DBC::Status::Warning)
      {
         c_Message = "\"General warning\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::Unknown)
      {
         c_Message = "\"Unknown entry in database file\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::Unsupported)
      {
         c_Message = "\"Unsupported entry in database file\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedVersion)
      {
         c_Message = "\"Incorrect format of Version (VERSION)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedBitTiming)
      {
         c_Message = "\"Incorrect format of Bit Timing (BS)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedValueTable)
      {
         c_Message = "\"Incorrect format of Value Table (VAL_TABLE)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedSignal)
      {
         c_Message = "\"Incorrect format of Signal (SG)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedMessage)
      {
         c_Message = "\"Incorrect format of Message (BO)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedMessageTransmitter)
      {
         c_Message = "\"Incorrect format of Message Transmitter (BO_TX_BU)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedEnvironmentVariable)
      {
         c_Message = "\"Incorrect format of Environment Variable (EV)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedEnvironmentVariableData)
      {
         c_Message = "\"Incorrect format of Environment Variable Data (ENVVAR_DATA)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedSignalType)
      {
         c_Message = "\"Incorrect format of Signal Type (SGTYPE)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedComment)
      {
         c_Message = "\"Incorrect format of Comment (CM)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedAttributeDefinition)
      {
         c_Message = "\"Incorrect format of Attribute Definition (BA_DEF)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedAttributeDefinitionRelation)
      {
         c_Message = "\"Incorrect format of Attribute Definition at Relation (BA_DEF_REL)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedAttributeDefault)
      {
         c_Message = "\"Incorrect format of Attribute Default (BA_DEF_DEF)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedAttributeDefaultRelation)
      {
         c_Message = "\"Incorrect format of Attribute Default at Relation (BA_DEF_DEF_REL)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedAttributeValue)
      {
         c_Message = "\"Incorrect format of Attribute Value (BA)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedAttributeRelationValue)
      {
         c_Message = "\"Incorrect format of Attribute Value at Relation (BA_REF)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedValueDescription)
      {
         c_Message = "\"Incorrect format of Value Description (VAL)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedSignalGroup)
      {
         c_Message = "\"Incorrect format of Signal Group (SIG_GROUP)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedSignalExtendedValueType)
      {
         c_Message = "\"Incorrect format of Signal Extended Value Type (SIG_VALTYPE)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::MalformedExtendedMultiplexor)
      {
         c_Message = "\"Incorrect format of Extended Multiplexer (SG_MUL_VAL)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
      if (orc_Status == Vector::DBC::Status::SignalWithoutMessage)
      {
         c_Message = "\"Signal (SG) followed a line, which was not a Message (BO) or Signal (SG)\".";
         mhc_WarningMessages.Append(c_Message);
         osc_write_log_warning("DBC file export", c_Message);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills up the obligatory new symbol entries of a DBC file.

   \param[in,out] orc_NewSymbols   symbol list to fill with standard entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportDbc::mh_SetNewSymbols(std::list<std::string> & orc_NewSymbols)
{
   orc_NewSymbols.push_back("NS_DESC_");
   orc_NewSymbols.push_back("CM_");
   orc_NewSymbols.push_back("BA_DEF_");
   orc_NewSymbols.push_back("BA_");
   orc_NewSymbols.push_back("VAL_");
   orc_NewSymbols.push_back("CAT_DEF_");
   orc_NewSymbols.push_back("CAT_");
   orc_NewSymbols.push_back("FILTER");
   orc_NewSymbols.push_back("BA_DEF_DEF_");
   orc_NewSymbols.push_back("EV_DATA_");
   orc_NewSymbols.push_back("ENVVAR_DATA_");
   orc_NewSymbols.push_back("SGTYPE_");
   orc_NewSymbols.push_back("SGTYPE_VAL_");
   orc_NewSymbols.push_back("BA_DEF_SGTYPE_");
   orc_NewSymbols.push_back("BA_SGTYPE_");
   orc_NewSymbols.push_back("SIG_TYPE_REF_");
   orc_NewSymbols.push_back("VAL_TABLE_");
   orc_NewSymbols.push_back("SIG_GROUP_");
   orc_NewSymbols.push_back("SIG_VALTYPE_");
   orc_NewSymbols.push_back("SIGTYPE_VALTYPE_");
   orc_NewSymbols.push_back("BO_TX_BU_");
   orc_NewSymbols.push_back("BA_DEF_REL_");
   orc_NewSymbols.push_back("BA_REL_");
   orc_NewSymbols.push_back("BA_DEF_DEF_REL_");
   orc_NewSymbols.push_back("BU_SG_REL_");
   orc_NewSymbols.push_back("BU_EV_REL_");
   orc_NewSymbols.push_back("BU_BO_REL_");
   orc_NewSymbols.push_back("SG_MUL_VAL_");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills up the obligatory attribute default map.

   \param[in,out] orc_AttributeDefaults   attribute default map to fill with standard values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportDbc::mh_SetAttributeDefaults(std::map<std::string, Vector::DBC::Attribute> & orc_AttributeDefaults)
{
   // bus type
   const std::string c_BusTypeName = "BusType";

   Vector::DBC::Attribute c_BusTypeAttribute;
   c_BusTypeAttribute.name = c_BusTypeName;
   c_BusTypeAttribute.valueType = Vector::DBC::AttributeValueType::String;
   c_BusTypeAttribute.stringValue = "";
   orc_AttributeDefaults.insert(std::pair<std::string, Vector::DBC::Attribute>(c_BusTypeName, c_BusTypeAttribute));

   // database name
   const std::string c_DbName = "DBName";
   Vector::DBC::Attribute c_DbNameAttribute;
   c_DbNameAttribute.name = c_DbName;
   c_DbNameAttribute.valueType = Vector::DBC::AttributeValueType::String;
   c_DbNameAttribute.stringValue = "";
   orc_AttributeDefaults.insert(std::pair<std::string, Vector::DBC::Attribute>(c_DbName, c_DbNameAttribute));

   // general message cycle time
   const std::string c_MsgCycleTimeName = "GenMsgCycleTime";
   Vector::DBC::Attribute c_MsgCycleTimeAttribute;
   c_MsgCycleTimeAttribute.name = c_MsgCycleTimeName;
   c_MsgCycleTimeAttribute.valueType = Vector::DBC::AttributeValueType::Int;
   c_MsgCycleTimeAttribute.integerValue = 0;
   orc_AttributeDefaults.insert(std::pair<std::string, Vector::DBC::Attribute>(c_MsgCycleTimeName,
                                                                               c_MsgCycleTimeAttribute));

   // general message send type
   const std::string c_MsgSendTypeName = "GenMsgSendType";
   Vector::DBC::Attribute c_MsgSendTypeAttribute;
   c_MsgSendTypeAttribute.name = c_MsgSendTypeName;
   c_MsgSendTypeAttribute.valueType = Vector::DBC::AttributeValueType::Enum;
   c_MsgSendTypeAttribute.stringValue = "NoMsgSendType";
   orc_AttributeDefaults.insert(std::pair<std::string, Vector::DBC::Attribute>(c_MsgSendTypeName,
                                                                               c_MsgSendTypeAttribute));

   // general signal inactive value
   const std::string c_SigInactiveName = "GenSigInactiveValue";
   Vector::DBC::Attribute c_SigInactiveAttribute;
   c_SigInactiveAttribute.name = c_SigInactiveName;
   c_SigInactiveAttribute.valueType = Vector::DBC::AttributeValueType::Int;
   c_SigInactiveAttribute.integerValue = 0;
   orc_AttributeDefaults.insert(std::pair<std::string, Vector::DBC::Attribute>(c_SigInactiveName,
                                                                               c_SigInactiveAttribute));

   // general signal send type
   const std::string c_SigSendTypeName = "GenSigSendType";
   Vector::DBC::Attribute c_SigSendTypeAttribute;
   c_SigSendTypeAttribute.name = c_SigSendTypeName;
   c_SigSendTypeAttribute.valueType = Vector::DBC::AttributeValueType::Enum;
   c_SigSendTypeAttribute.stringValue = "Cyclic";
   orc_AttributeDefaults.insert(std::pair<std::string, Vector::DBC::Attribute>(c_SigSendTypeName,
                                                                               c_SigSendTypeAttribute));

   // network management station address
   // do not know if I have to set this, but in default configuration with one node this variable is set
   // from https://vector.com/portal/medien/cmc/manuals/CANisterConfigurator_Manual_EN.pdf
   // "NmStationAddress: The number of the control unit for network management (node attribute)."
   const std::string c_NmStationName = "NmStationAddress";
   Vector::DBC::Attribute c_NmStationAttribute;
   c_NmStationAttribute.name = c_NmStationName;
   c_NmStationAttribute.valueType = Vector::DBC::AttributeValueType::Hex;
   c_NmStationAttribute.hexValue = 0;
   orc_AttributeDefaults.insert(std::pair<std::string, Vector::DBC::Attribute>(c_NmStationName, c_NmStationAttribute));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills up the obligatory attribute definition map.

   \param[in,out] orc_AttributeDefinitions   attribute definition map to fill with standard values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieExportDbc::mh_SetAttributeDefinitions(std::map<std::string,
                                                         Vector::DBC::AttributeDefinition> & orc_AttributeDefinitions)
{
   // set bus type
   std::string c_BusTypeName = "BusType";
   Vector::DBC::AttributeDefinition c_BusTypeAttributeDef;
   c_BusTypeAttributeDef.name = c_BusTypeName;
   c_BusTypeAttributeDef.objectType = Vector::DBC::AttributeDefinition::ObjectType::Network;
   c_BusTypeAttributeDef.valueType = Vector::DBC::AttributeValueType::String;
   orc_AttributeDefinitions.insert(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                      c_BusTypeName, c_BusTypeAttributeDef));

   // set database name
   std::string c_DbName = "DBName";
   Vector::DBC::AttributeDefinition c_DbNameAttributeDef;
   c_DbNameAttributeDef.name = c_DbName;
   c_DbNameAttributeDef.objectType = Vector::DBC::AttributeDefinition::ObjectType::Network;
   c_DbNameAttributeDef.valueType = Vector::DBC::AttributeValueType::String;
   c_DbNameAttributeDef.enumValues.push_back("");
   orc_AttributeDefinitions.insert(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                      c_DbName, c_DbNameAttributeDef));

   // set global attribute definition for cycle time (default)
   Vector::DBC::AttributeDefinition c_CycleTimeAttributeDef;
   c_CycleTimeAttributeDef.name = mhc_MsgCycleTime;
   c_CycleTimeAttributeDef.objectType = Vector::DBC::AttributeDefinition::ObjectType::Message;
   c_CycleTimeAttributeDef.valueType = Vector::DBC::AttributeValueType::Int;
   orc_AttributeDefinitions.insert(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                      c_CycleTimeAttributeDef.name, c_CycleTimeAttributeDef));

   // set global attribute definitions for openSYDE message send types 'Cyclic' and 'OnEvent'
   Vector::DBC::AttributeDefinition c_MsgSendTypeAttributeDef;
   c_MsgSendTypeAttributeDef.name = mhc_MsgSendType;
   c_MsgSendTypeAttributeDef.objectType = Vector::DBC::AttributeDefinition::ObjectType::Message;
   c_MsgSendTypeAttributeDef.valueType = Vector::DBC::AttributeValueType::Enum;
   c_MsgSendTypeAttributeDef.enumValues.push_back("Cyclic");
   c_MsgSendTypeAttributeDef.enumValues.push_back("OnEvent");
   orc_AttributeDefinitions.insert(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                      c_MsgSendTypeAttributeDef.name, c_MsgSendTypeAttributeDef));

   // set general signal inactive value
   std::string c_SigInactiveValueName = "GenSigInactiveValue";
   Vector::DBC::AttributeDefinition c_SigInactiveValueAttributeDef;
   c_SigInactiveValueAttributeDef.name = c_SigInactiveValueName;
   c_SigInactiveValueAttributeDef.objectType = Vector::DBC::AttributeDefinition::ObjectType::Signal;
   c_SigInactiveValueAttributeDef.valueType = Vector::DBC::AttributeValueType::Int;
   orc_AttributeDefinitions.insert(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                      c_SigInactiveValueName, c_SigInactiveValueAttributeDef));

   // set general signal send type
   std::string c_SigSendTypeName = "GenSigSendType";
   Vector::DBC::AttributeDefinition c_SigSendTypeAttributeDef;
   c_SigSendTypeAttributeDef.name = c_SigSendTypeName;
   c_SigSendTypeAttributeDef.objectType = Vector::DBC::AttributeDefinition::ObjectType::Signal;
   c_SigSendTypeAttributeDef.valueType = Vector::DBC::AttributeValueType::Enum;

   c_SigSendTypeAttributeDef.enumValues.push_back("NoSigSendType");
   orc_AttributeDefinitions.insert(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                      c_SigSendTypeName, c_SigSendTypeAttributeDef));

   // set network management station address
   // do not know if I have to set this, but in default configuration with one node this variable is set
   // from https://vector.com/portal/medien/cmc/manuals/CANisterConfigurator_Manual_EN.pdf
   // "NmStationAddress: The number of the control unit for network management (node attribute)."
   std::string c_NmStationName = "NmStationAddress";
   Vector::DBC::AttributeDefinition c_NmStationAttributeDef;
   c_NmStationAttributeDef.name = c_NmStationName;
   c_NmStationAttributeDef.objectType = Vector::DBC::AttributeDefinition::ObjectType::Node;
   c_NmStationAttributeDef.valueType = Vector::DBC::AttributeValueType::Hex;
   orc_AttributeDefinitions.insert(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                      c_NmStationName, c_NmStationAttributeDef));

   // set initial value
   std::string c_SigInitialValueName = mhc_SigInitialValue.c_str();
   Vector::DBC::AttributeDefinition c_SigInitialValueAttribute;
   c_SigInitialValueAttribute.name = c_SigInitialValueName;
   c_SigInitialValueAttribute.objectType = Vector::DBC::AttributeDefinition::ObjectType::Signal;
   c_SigInitialValueAttribute.valueType = Vector::DBC::AttributeValueType::Int; // I am not sure, but setting this to
                                                                                // integer because we have raw values
                                                                                // here
   orc_AttributeDefinitions.insert(std::pair<std::string, Vector::DBC::AttributeDefinition>(
                                      c_SigInitialValueName, c_SigInitialValueAttribute));
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

   \param[in]     orc_String         Original string

   \return
   Niceified string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_CieExportDbc::mh_NiceifyStringForDbcSymbol(const C_SCLString & orc_String)
{
   C_SCLString c_Result;

   // first character must not contain a digit, in this case place '_' before digit.
   if (std::isdigit(orc_String.c_str()[0]) != 0)
   {
      // first character is digit, add '_'
      c_Result = '_';
   }

   for (uint32 u32_Index = 0U; u32_Index < orc_String.Length(); u32_Index++)
   {
      const charn cn_Character = orc_String.c_str()[u32_Index];
      if ((std::isalnum(cn_Character) == 0) && (cn_Character != '_'))
      {
         c_Result += C_SCLString::IntToStr(cn_Character);
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

   \param[in] orc_String String to escape

   \return
   Escaped string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_CieExportDbc::mh_EscapeCriticalSymbols(const C_SCLString & orc_String)
{
   C_SCLString c_Retval;

   for (sint32 s32_Char = 0; s32_Char < static_cast<sint32>(orc_String.Length()); ++s32_Char)
   {
      const charn c_Char = orc_String[s32_Char + 1];
      if (c_Char == '\"')
      {
         c_Retval += "\\\"";
      }
      else
      {
         c_Retval += c_Char;
      }
   }
   return c_Retval;
}
