//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Importing data from Vector DBC file (implementation)

   Importing bus definition with all nodes from a Vector DBC file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <cstdlib>
#include <cmath>

#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "C_CieImportDbc.hpp"

#include "TglFile.hpp"
#include "TglUtils.hpp"

#include "C_OscNodeDataPoolContent.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"
#include "C_OscUtils.hpp"
#include "C_SdNdeDpContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const C_SclString C_CieImportDbc::mhc_SEND_TYPE = "GenMsgSendType";
const C_SclString C_CieImportDbc::mhc_CYCLE_TIME = "GenMsgCycleTime";
const C_SclString C_CieImportDbc::mhc_INITIAL_VALUE = "GenSigStartValue";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */
Vector::DBC::AttributeDefinition C_CieImportDbc::mhc_AttributeSendType;
C_SclString C_CieImportDbc::mhc_DefaultSendTypeValue; // get value from network attribute default values
float32_t C_CieImportDbc::mhf32_DefaultInitialValue;  // default initial value of DBC file or openSYDE
bool C_CieImportDbc::mhq_DefaultValueDefined;         // in DBC file
C_SclStringList C_CieImportDbc::mhc_WarningMessages;  // empty list
C_SclString C_CieImportDbc::mhc_ErrorMessage = "";    // empty string

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Import network configuration from .dbc file

   The caller is responsible to provided valid pointers for the function parameters.

   \param[in]   orc_File                  path and name of .dbc file
   \param[out]  orc_Definition            communication stack definition filled with data read from .dbc file
   \param[out]  orc_WarningMessages       list of global warnings e.g. why some messages could not be imported
   \param[out]  orc_ErrorMessage          error message with reason for failed import
   \param[in]   oq_AddUnmappedMessages    Optional flag to include unmapped messages in the output

   \return
   C_NO_ERR    required data from file successfully stored in orc_Definition
   C_RANGE     orc_File is empty string
   C_CONFIG    orc_File does not point to a valid file
   C_RD_WR     error while reading file
   C_WARN      unknown parameter found -> default value set and error reported
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportDbc::h_ImportNetwork(const C_SclString & orc_File,
                                        C_CieConverter::C_CieCommDefinition & orc_Definition,
                                        C_SclStringList & orc_WarningMessages, C_SclString & orc_ErrorMessage,
                                        const bool oq_AddUnmappedMessages)
{
   int32_t s32_Return;

   mhc_WarningMessages.Clear(); // clear old warning messages for this import
   mhc_ErrorMessage = "";       // clear old error message for this import
   Vector::DBC::Network c_DbcNetwork;

   osc_write_log_info("DBC file import", "Reading DBC file \"" + orc_File + "\" ...");
   s32_Return = mh_ReadFile(orc_File, c_DbcNetwork);

   if (s32_Return == C_NO_ERR)
   {
      // get attribute definitions for send type
      osc_write_log_info("DBC file import", "Reading attribute definitions of DBC file ...");
      s32_Return = mh_GetAttributeDefinitions(c_DbcNetwork);
   }

   if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
   {
      uint32_t u32_Nodes = 0U;
      stw::scl::C_SclString c_FileName = stw::tgl::TglExtractFileName(orc_File);
      c_FileName.SetLength(c_FileName.Length() - 4U); //no extension

      // add bus information
      orc_Definition.c_Bus.c_Name = c_FileName; // file name means network name
      orc_Definition.c_Bus.c_Comment = mh_ReEscapeCriticalSymbols(c_DbcNetwork.comment.c_str());

      osc_write_log_info("DBC file import",
                         "Reading node and messages with signals of network \"" + c_FileName + "\" ...");

      // assign node definitions with messages
      // there is also a check if each message was assigned
      std::set<std::string> c_MessageAssignment;
      for (const auto & rc_DbcMessage : c_DbcNetwork.messages)
      {
         c_MessageAssignment.insert(rc_DbcMessage.second.name);
      }
      orc_Definition.c_Nodes.resize(c_DbcNetwork.nodes.size());
      for (const auto & rc_DbcNode : c_DbcNetwork.nodes)
      {
         C_CieConverter::C_CieNode & rc_Node = orc_Definition.c_Nodes[u32_Nodes];
         mh_GetNode(rc_DbcNode.second, rc_Node);
         osc_write_log_info("DBC file import", "Reading messages with signals for node \"" +
                            rc_Node.c_Properties.c_Name + "\" ...");

         for (const auto & rc_DbcMessage : c_DbcNetwork.messages)
         {
            const int32_t s32_Tmp = mh_GetMessage(c_DbcNetwork, rc_DbcMessage.second, rc_Node);
            // check if message is assigned to a node
            if (s32_Tmp != C_CONFIG)
            {
               // message is assigned to a node, then we erase this message from temporary message assignment set
               const std::set<std::string>::const_iterator c_Iter = c_MessageAssignment.find(rc_DbcMessage.second.name);
               if (c_Iter != c_MessageAssignment.end())
               {
                  c_MessageAssignment.erase(c_Iter);
               }
               // check if there are any warnings
               if (s32_Tmp != C_NO_ERR)
               {
                  s32_Return = C_WARN;
               }
            }
         }
         u32_Nodes++;
      }
      // check if there are some messages left which are not assigned to a node
      if (c_MessageAssignment.size() != 0)
      {
         // some messages left, add to unmapped messages/report ignore
         std::set<std::string>::const_iterator c_Iter;
         for (c_Iter = c_MessageAssignment.begin(); c_Iter != c_MessageAssignment.end(); ++c_Iter)
         {
            if (oq_AddUnmappedMessages == true)
            {
               const auto c_DbcMessage = find_if(
                  c_DbcNetwork.messages.begin(), c_DbcNetwork.messages.end(), [c_Iter] (auto & orc_Message) -> bool
               {
                  return orc_Message.second.name.compare(*c_Iter) == 0;
               }
                  );

               if (c_DbcMessage != c_DbcNetwork.messages.end())
               {
                  //Add found valid message to unmapped messages
                  if (mh_ConvertAndAddMessage(c_DbcNetwork, c_DbcMessage->second,
                                              orc_Definition.c_UnmappedMessages) != C_NO_ERR)
                  {
                     s32_Return = C_WARN;
                  }
               }
            }
            else
            {
               const C_SclString c_String = (*c_Iter).c_str();
               //Report issue
               osc_write_log_warning("DBC file import",
                                     "message \"" + c_String + "\" is not assigned to a node and ignored.");
               mhc_WarningMessages.Append("Message \"" + c_String + "\" is not assigned to a node and ignored.");
            }
         }
         if (oq_AddUnmappedMessages == false)
         {
            s32_Return = C_WARN;
         }
      }
   }

   if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
   {
      osc_write_log_info("DBC file import", "DBC network successfully imported.");
   }

   orc_WarningMessages = mhc_WarningMessages; // set warning messages for caller
   orc_ErrorMessage = mhc_ErrorMessage;       // set error message for caller

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Read data from .dbc file

   \param[in]   orc_File      path and name of .dbc file
   \param[out]  orc_Network   data read from .dbc file

   \return
   C_NO_ERR    all data successfully read from file
   C_RANGE     orc_File is empty string
   C_CONFIG    orc_File does not point to a valid file
   C_RD_WR     error while reading file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportDbc::mh_ReadFile(const C_SclString & orc_File, Vector::DBC::Network & orc_Network)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_File == "")
   {
      s32_Return = C_RANGE;
   }
   else if (stw::tgl::TglFileExists(orc_File) == false)
   {
      mhc_ErrorMessage = "DBC file \"" + orc_File + "\" does not exist.";
      osc_write_log_error("DBC file import", mhc_ErrorMessage);
      s32_Return = C_CONFIG;
   }
   else
   {
      // load database file
      bool q_ReadError = false;

      //try/catch the load function due to the dbc library may throw exceptions when reading files with not supported
      // content
      try
      {
         std::ifstream c_InputFile(orc_File.c_str());
         if (c_InputFile.is_open())
         {
            c_InputFile >> orc_Network;
            c_InputFile.close();
            if (orc_Network.successfullyParsed == false)
            {
               q_ReadError = true;
            }
         }
         else
         {
            q_ReadError = true;
         }
      }
      catch (...)
      {
         osc_write_log_info("DBC file import", "Reading DBC file \"" + orc_File + "\" failed (mh_ReadFile try/catch).");
         q_ReadError = true;
      }

      //read error?
      if (q_ReadError == true)
      {
         mhc_ErrorMessage = "Can't read DBC file \"" + orc_File + "\".";
         osc_write_log_error("DBC file import", mhc_ErrorMessage);
         s32_Return = C_RD_WR;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get node definitions

   \param[in]  orc_DbcNode           node definitions from DBC file
   \param[out] orc_Node              target node definitions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDbc::mh_GetNode(const Vector::DBC::Node & orc_DbcNode, C_CieConverter::C_CieNode & orc_Node)
{
   orc_Node.c_Properties.c_Name = orc_DbcNode.name.c_str();
   orc_Node.c_Properties.c_Comment = mh_ReEscapeCriticalSymbols(orc_DbcNode.comment.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get message definition

   \param[in]       orc_DbcNetwork  Dbc network
   \param[in]       orc_DbcMessage  message definition from DBC file
   \param[in, out]  orc_Node        target node definitions

   \return
   C_NO_ERR    all data successfully retrieved
   C_WARN      unknown parameter found -> default value set and error reported
   C_CONFIG    node does not contain this message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportDbc::mh_GetMessage(const Vector::DBC::Network & orc_DbcNetwork,
                                      const Vector::DBC::Message & orc_DbcMessage, C_CieConverter::C_CieNode & orc_Node)
{
   int32_t s32_Return = C_NO_ERR;
   bool q_IsTxMessage = false;
   bool q_IsRxMessage = false;

   // check on sender node
   if (orc_DbcMessage.transmitter.size() > 0U)
   {
      if (orc_Node.c_Properties.c_Name.AnsiCompare(orc_DbcMessage.transmitter.c_str()) == 0)
      {
         q_IsTxMessage = true;
      }
   }
   else if (orc_DbcMessage.transmitters.empty() == false)
   {
      for (const auto & rc_DbcTransmitter : orc_DbcMessage.transmitters)
      {
         if (orc_Node.c_Properties.c_Name.AnsiCompare(rc_DbcTransmitter.c_str()) == 0)
         {
            q_IsTxMessage = true;
            break;
         }
      }
   }
   else
   {
      // nothing to do here
   }

   // check on receiver nodes
   for (const auto & rc_DbcSignal : orc_DbcMessage.signals)
   {
      for (const auto & rc_DbcReceiver : rc_DbcSignal.second.receivers)
      {
         if (orc_Node.c_Properties.c_Name.AnsiCompare(rc_DbcReceiver.c_str()) == 0)
         {
            // node found
            q_IsRxMessage = true;
            break;
         }
      }

      if (q_IsRxMessage == true)
      {
         // node found
         break;
      }
   }

   if (q_IsTxMessage == true)
   {
      // add message if valid
      if (mh_ConvertAndAddMessage(orc_DbcNetwork, orc_DbcMessage, orc_Node.c_TxMessages) != C_NO_ERR)
      {
         s32_Return = C_WARN;
      }
   }
   else if (q_IsRxMessage == true)
   {
      // add message if valid
      if (mh_ConvertAndAddMessage(orc_DbcNetwork, orc_DbcMessage, orc_Node.c_RxMessages) != C_NO_ERR)
      {
         s32_Return = C_WARN;
      }
   }
   else
   {
      // message is not assigned to this node
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse and convert message

   \param[in]      orc_DbcNetwork   Dbc network
   \param[in]      orc_DbcMessage   Message to convert
   \param[in,out]  orc_Message      Converted message

   \retval   C_NO_ERR   No error occurred
   \retval   C_WARN     Could not find at least one thing
   \retval   C_CONFIG   Message ID invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportDbc::mh_PrepareMessage(const Vector::DBC::Network & orc_DbcNetwork,
                                          const Vector::DBC::Message & orc_DbcMessage,
                                          C_CieConverter::C_CieNodeMessage & orc_Message)
{
   int32_t s32_Return = C_NO_ERR;

   orc_Message.c_CanMessage.c_Name = orc_DbcMessage.name.c_str();
   orc_Message.c_CanMessage.c_Comment = mh_ReEscapeCriticalSymbols(orc_DbcMessage.comment.c_str());
   if ((orc_DbcMessage.id & (static_cast<uint32_t>(1U) << 31U)) != 0U)
   {
      orc_Message.c_CanMessage.q_IsExtended = true;
   }
   else
   {
      orc_Message.c_CanMessage.q_IsExtended = false;
   }

   orc_Message.c_CanMessage.u32_CanId = static_cast<uint32_t>(orc_DbcMessage.id) & 0x7FFFFFFFU;
   orc_Message.c_CanMessage.u16_Dlc = static_cast<uint16_t>(orc_DbcMessage.size);

   if (orc_Message.c_CanMessage.u32_CanId > 0x1FFFFFFFU)
   {
      s32_Return = C_CONFIG;
      osc_write_log_error("DBC file import", "CAN message \"" + orc_Message.c_CanMessage.c_Name + "\" has invalid ID.");
   }

   if (s32_Return == C_NO_ERR)
   {
      bool q_MessageAdapted = false;

      osc_write_log_info("DBC file import",
                         "Importing signals for CAN message \"" + orc_Message.c_CanMessage.c_Name + "\" ...");

      // get all signal definitions for this message
      for (const auto & rc_DbcSignal : orc_DbcMessage.signals)
      {
         // If a multiplexer signal exists the message is a multiplexed message
         if (mh_GetSignal(orc_DbcNetwork, rc_DbcSignal.second, q_MessageAdapted, orc_Message) != C_NO_ERR)
         {
            s32_Return = C_WARN;
         }
      }

      // get transmission definitions
      mh_GetTransmission(orc_DbcNetwork, orc_DbcMessage, orc_Message);

      if (q_MessageAdapted == true)
      {
         s32_Return = C_WARN;
         osc_write_log_warning("DBC file import",
                               "CAN Message \"" + orc_Message.c_CanMessage.c_Name +
                               "\" was adapted due to a multiplexed message.");
         orc_Message.c_Warnings.Append("The multiplexer signal properties were adapted due to a multiplexed message.\n"
                                       "Multiplexer signal restrictions:\n"
                                       "- Auto min/max: active\n"
                                       "- Factor: 1\n"
                                       "- Offset: 0\n"
                                       "- Init value: 0\n"
                                       "- Unit: disabled\n"
                                       "- Length: Maximum 16 bit\n"
                                       "- Type: unsigned");
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add valid messages to list of import messages.

   \param[in]      orc_DbcNetwork   Dbc network
   \param[in]      orc_DbcMessage   DBC message to convert
   \param[in,out]  orc_Messages     Messages where imported message should be added if valid

   \retval   C_NO_ERR   No error occurred
   \retval   C_WARN     Could not find at least one thing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportDbc::mh_ConvertAndAddMessage(const Vector::DBC::Network & orc_DbcNetwork,
                                                const Vector::DBC::Message & orc_DbcMessage,
                                                std::vector<C_CieConverter::C_CieNodeMessage> & orc_Messages)
{
   int32_t s32_Return = C_WARN;

   C_CieConverter::C_CieNodeMessage c_Message;
   const int32_t s32_RetPrepMessage = C_CieImportDbc::mh_PrepareMessage(orc_DbcNetwork, orc_DbcMessage, c_Message);

   // Add message if not invalid
   if ((s32_RetPrepMessage == C_NO_ERR) || (s32_RetPrepMessage == C_WARN))
   {
      orc_Messages.push_back(c_Message);
      s32_Return = s32_RetPrepMessage;
   }
   else
   {
      osc_write_log_warning("DBC file import",
                            "Invalid CAN message \"" + c_Message.c_CanMessage.c_Name + "\" was not imported.");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get signal definition

   \param[in]      orc_DbcNetwork      Dbc network
   \param[in]      orc_DbcSignal       signal definition from DBC file
   \param[in,out]  orq_SignalAdapted   flag if signal was adapted, will not be reset to false
   \param[out]     orc_Message         target message definitions

   \return
   C_NO_ERR    all data successfully retrieved
   C_WARN      unknown parameter found -> default value set and warning reported
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportDbc::mh_GetSignal(const Vector::DBC::Network & orc_DbcNetwork,
                                     const Vector::DBC::Signal & orc_DbcSignal, bool & orq_SignalAdapted,
                                     C_CieConverter::C_CieNodeMessage & orc_Message)
{
   int32_t s32_Return = C_NO_ERR;

   std::map<int64_t, std::string>::const_iterator c_ItValueDescr;
   C_CieConverter::C_CieCanSignal c_Signal;
   bool q_MultiplexerSignal = false;

   // Handle multiplexing information
   switch (orc_DbcSignal.multiplexor)
   {
   case Vector::DBC::Signal::Multiplexor::MultiplexorSwitch:
      c_Signal.e_MultiplexerType = C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL;
      c_Signal.u16_MultiplexValue = 0; // default
      q_MultiplexerSignal = true;
      break;
   case Vector::DBC::Signal::Multiplexor::MultiplexedSignal:
      c_Signal.e_MultiplexerType = C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL;
      c_Signal.u16_MultiplexValue = static_cast<uint16_t>(orc_DbcSignal.multiplexerSwitchValue);
      break;
   case Vector::DBC::Signal::Multiplexor::NoMultiplexor:
   default:
      c_Signal.e_MultiplexerType = C_OscCanSignal::eMUX_DEFAULT;
      c_Signal.u16_MultiplexValue = 0; // default
      break;
   }

   c_Signal.u16_ComBitStart = static_cast<uint16_t>(orc_DbcSignal.startBit);
   if ((q_MultiplexerSignal == true) &&
       (orc_DbcSignal.bitSize > 16U))
   {
      // Multiplexer signal restriction
      c_Signal.u16_ComBitLength = 16U;
      orq_SignalAdapted = true;
   }
   else
   {
      c_Signal.u16_ComBitLength = static_cast<uint16_t>(orc_DbcSignal.bitSize);
   }

   // avoid bit length 0
   if (c_Signal.u16_ComBitLength == 0U)
   {
      const C_SclString c_String = orc_DbcSignal.name.c_str();

      c_Signal.u16_ComBitLength = 1U;
      osc_write_log_warning("DBC file import",
                            "Invalid bit length of signal \"" + c_String + "\". Bit length set to \"1\".");
      orc_Message.c_Warnings.Append("Invalid bit length of signal \"" + c_String + "\". Bit length set to \"1\".");
      s32_Return = C_WARN;
   }

   if ((orc_DbcSignal.byteOrder == Vector::DBC::ByteOrder::Motorola) ||
       (orc_DbcSignal.byteOrder == Vector::DBC::ByteOrder::BigEndian))
   {
      c_Signal.e_ComByteOrder = C_OscCanSignal::E_ByteOrderType::eBYTE_ORDER_MOTOROLA;
   }
   else if ((orc_DbcSignal.byteOrder == Vector::DBC::ByteOrder::Intel) ||
            (orc_DbcSignal.byteOrder == Vector::DBC::ByteOrder::LittleEndian))
   {
      c_Signal.e_ComByteOrder = C_OscCanSignal::E_ByteOrderType::eBYTE_ORDER_INTEL;
   }
   else
   {
      const C_SclString c_String = orc_DbcSignal.name.c_str();

      // default value
      c_Signal.e_ComByteOrder = C_OscCanSignal::E_ByteOrderType::eBYTE_ORDER_INTEL;
      osc_write_log_warning("DBC file import", "\"" + c_String + "\" signal byte order type error");
      orc_Message.c_Warnings.Append("\"" + c_String + "\" signal byte order type error");
      s32_Return = C_WARN;
   }

   // Copy and convert the value descriptions
   for (c_ItValueDescr = orc_DbcSignal.valueDescriptions.begin();
        c_ItValueDescr != orc_DbcSignal.valueDescriptions.end();
        ++c_ItValueDescr)
   {
      c_Signal.c_ValueDescription.emplace(std::pair<int64_t, C_SclString>(
                                             c_ItValueDescr->first,
                                             c_ItValueDescr->second.c_str()));
   }

   c_Signal.c_Element.c_Name = orc_DbcSignal.name.c_str();
   c_Signal.c_Element.c_Comment = mh_ReEscapeCriticalSymbols(orc_DbcSignal.comment.c_str());
   if (q_MultiplexerSignal == false)
   {
      c_Signal.c_Element.f64_Factor = orc_DbcSignal.factor;
      c_Signal.c_Element.f64_Offset = orc_DbcSignal.offset;
      c_Signal.c_Element.c_Unit = orc_DbcSignal.unit.c_str();
   }
   else
   {
      // Multiplexer signal restriction
      if ((C_OscUtils::h_IsFloat64NearlyEqual(orc_DbcSignal.factor, 1.0) == false) ||
          (C_OscUtils::h_IsFloat64NearlyEqual(orc_DbcSignal.offset, 0.0) == false) ||
          (orc_DbcSignal.unit != ""))
      {
         orq_SignalAdapted = true;
      }

      c_Signal.c_Element.f64_Factor = 1.0;
      c_Signal.c_Element.f64_Offset = 0.0;
      c_Signal.c_Element.c_Unit = "";
   }

   C_CieImportDbc::mh_GetSignalSpnInfo(orc_DbcNetwork, orc_DbcSignal, c_Signal);

   if (mh_GetSignalValues(orc_DbcNetwork, orc_DbcSignal, q_MultiplexerSignal, orq_SignalAdapted,
                          c_Signal.c_Element, orc_Message.c_Warnings) != C_NO_ERR)
   {
      s32_Return = C_WARN;
   }

   C_CieImportDbc::mh_VerifySignalValueTable(c_Signal);

   orc_Message.c_CanMessage.c_Signals.push_back(c_Signal);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get signal spn info

   \param[in]      orc_DbcNetwork   Dbc network
   \param[in]      orc_DbcSignal    Dbc signal
   \param[in,out]  orc_Signal       Signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDbc::mh_GetSignalSpnInfo(const Vector::DBC::Network & orc_DbcNetwork,
                                         const Vector::DBC::Signal & orc_DbcSignal,
                                         C_CieConverter::C_CieCanSignal & orc_Signal)
{
   std::map<std::string, Vector::DBC::Attribute>::const_iterator c_IterSpn;
   c_IterSpn = orc_DbcSignal.attributeValues.find("SPN");
   if (c_IterSpn != orc_DbcSignal.attributeValues.end())
   {
      const std::map<std::string,
                     Vector::DBC::AttributeDefinition>::const_iterator c_IterSpnType =
         orc_DbcNetwork.attributeDefinitions.find("SPN");
      if (c_IterSpnType != orc_DbcNetwork.attributeDefinitions.end())
      {
         if (c_IterSpnType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Int)
         {
            orc_Signal.u32_J1939Spn = static_cast<uint32_t>(c_IterSpn->second.integerValue);
         }
         else if (c_IterSpnType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Float)
         {
            orc_Signal.u32_J1939Spn = static_cast<uint32_t>(c_IterSpn->second.floatValue);
         }
         else if (c_IterSpnType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Hex)
         {
            orc_Signal.u32_J1939Spn = static_cast<uint32_t>(c_IterSpn->second.hexValue);
         }
         else if (c_IterSpnType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Enum)
         {
            orc_Signal.u32_J1939Spn = static_cast<uint32_t>(c_IterSpn->second.enumValue);
         }
         else
         {
            orc_Signal.u32_J1939Spn = 0UL;
         }
      }
      else
      {
         orc_Signal.u32_J1939Spn = 0UL;
      }
   }
   else
   {
      orc_Signal.u32_J1939Spn = 0UL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Verify if all signal values are inside of the signal value range, if not: remove

   \param[in,out]  orc_DbcSignal    Signal for which to clean up the value tables
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDbc::mh_VerifySignalValueTable(C_CieConverter::C_CieCanSignal & orc_DbcSignal)
{
   if (orc_DbcSignal.u16_ComBitLength <= 64U)
   {
      const uint64_t u64_MaxVal =
         (orc_DbcSignal.u16_ComBitLength == 64U) ?
         std::numeric_limits<uint64_t>::max() : ((static_cast<uint64_t>(1ULL) << orc_DbcSignal.u16_ComBitLength) -
                                                 1ULL);
      for (std::map<int64_t, stw::scl::C_SclString>::const_iterator c_ItValueDescr =
              orc_DbcSignal.c_ValueDescription.begin();
           c_ItValueDescr != orc_DbcSignal.c_ValueDescription.end();)
      {
         if (static_cast<uint64_t>(c_ItValueDescr->first) <= u64_MaxVal)
         {
            //Fine, iterate to next position
            ++c_ItValueDescr;
         }
         else
         {
            const C_SclString & rc_String = orc_DbcSignal.c_Element.c_Name.c_str();
            const C_SclString & rc_String2 = c_ItValueDescr->second;
            osc_write_log_warning("DBC file import",
                                  "signal \"" + rc_String + "\" value \"" + rc_String2 +
                                  "\" removed, because value " + C_SclString::IntToStr(
                                     c_ItValueDescr->first) + " out of range of " + C_SclString::IntToStr(
                                     orc_DbcSignal.u16_ComBitLength) + " bit");
            //Remove, new item at current position
            c_ItValueDescr = orc_DbcSignal.c_ValueDescription.erase(c_ItValueDescr);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get signal values and type

   Set minimum and maximum values for a signal.

   \param[in]      orc_DbcNetwork         Dbc network
   \param[in]      orc_DbcSignal          signal definition from DBC file
   \param[in]      oq_MultiplexerSignal   flag if signal is multiplexer signal
   \param[in,out]  orq_SignalAdapted      flag if signal was adapted, will not be reset to false
   \param[out]     orc_Element            element for CAN message signals
   \param[out]     orc_WarningMessages    list of warnings for each message

   \return
   C_NO_ERR    all data successfully retrieved
   C_WARN      unknown parameter found -> default value set and error reported
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportDbc::mh_GetSignalValues(const Vector::DBC::Network & orc_DbcNetwork,
                                           const Vector::DBC::Signal & orc_DbcSignal, const bool oq_MultiplexerSignal,
                                           bool & orq_SignalAdapted, C_CieConverter::C_CieDataPoolElement & orc_Element,
                                           C_SclStringList & orc_WarningMessages)
{
   int32_t s32_Return = C_NO_ERR;
   const C_SclString c_String = orc_DbcSignal.name.c_str();

   Vector::DBC::Signal c_DbcSignal = orc_DbcSignal; // remove const binding because of minimumRawValue() and
                                                    // maximumRawValue()

   C_OscNodeDataPoolContent::E_Type e_CurrentType; // guarantees the same type for min, max and initial value
   const float64_t f64_DEFAULT = mhf32_DefaultInitialValue;
   float64_t f64_MinValue = f64_DEFAULT;     // default
   float64_t f64_MaxValue = f64_DEFAULT;     // default
   float64_t f64_InitialValue = f64_DEFAULT; // default
   float64_t f64_MinValuePhy;                // used to display warning messages with physical values to user
   float64_t f64_MaxValuePhy;                //                             -"-
   float64_t f64_InitialValuePhy;            //                             -"-

   // Check if adaptions are necessary due to a multiplexer signal
   if (oq_MultiplexerSignal == true)
   {
      // Multiplexer signal restrictions
      if ((c_DbcSignal.extendedValueType != Vector::DBC::Signal::ExtendedValueType::Integer) &&
          (c_DbcSignal.extendedValueType != Vector::DBC::Signal::ExtendedValueType::Undefined))
      {
         c_DbcSignal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
         orq_SignalAdapted = true;
      }

      if (c_DbcSignal.valueType != Vector::DBC::ValueType::Unsigned)
      {
         c_DbcSignal.valueType = Vector::DBC::ValueType::Unsigned;
         orq_SignalAdapted = true;
      }

      // avoid bit length 0
      if (c_DbcSignal.bitSize == 0)
      {
         c_DbcSignal.bitSize = 1U;
         orq_SignalAdapted = true;
      }

      // avoid bit length >16
      if (c_DbcSignal.bitSize > 16U)
      {
         c_DbcSignal.bitSize = 16U;
         orq_SignalAdapted = true;
      }

      if (C_OscUtils::h_IsFloat64NearlyEqual(c_DbcSignal.factor, 1.0) == false)
      {
         c_DbcSignal.factor = 1.0;
         orq_SignalAdapted = true;
      }

      if (C_OscUtils::h_IsFloat64NearlyEqual(c_DbcSignal.offset, 0.0) == false)
      {
         c_DbcSignal.offset = 0.0;
         orq_SignalAdapted = true;
      }

      // 0.0 equals auto/default (correct value is set below)
      if (C_OscUtils::h_IsFloat64NearlyEqual(c_DbcSignal.maximum, 0.0) == false)
      {
         c_DbcSignal.maximum = 0.0;
         orq_SignalAdapted = true;
      }

      // 0.0 equals auto/default (correct value is set below)
      if (C_OscUtils::h_IsFloat64NearlyEqual(c_DbcSignal.minimum, 0.0) == false)
      {
         c_DbcSignal.minimum = 0.0;
         orq_SignalAdapted = true;
      }
   }

   // extendedValueType is currently set only in case of variable type float or double
   if ((c_DbcSignal.extendedValueType == Vector::DBC::Signal::ExtendedValueType::Integer) ||
       (c_DbcSignal.extendedValueType == Vector::DBC::Signal::ExtendedValueType::Undefined))
   {
      // set type
      if (c_DbcSignal.bitSize <= 8U)
      {
         if (c_DbcSignal.valueType == Vector::DBC::ValueType::Unsigned)
         {
            e_CurrentType = C_OscNodeDataPoolContent::eUINT8;
         }
         else
         {
            e_CurrentType = C_OscNodeDataPoolContent::eSINT8;
         }
      }
      else if (c_DbcSignal.bitSize <= 16U)
      {
         if (c_DbcSignal.valueType == Vector::DBC::ValueType::Unsigned)
         {
            e_CurrentType = C_OscNodeDataPoolContent::eUINT16;
         }
         else
         {
            e_CurrentType = C_OscNodeDataPoolContent::eSINT16;
         }
      }
      else if (c_DbcSignal.bitSize <= 32U)
      {
         if (c_DbcSignal.valueType == Vector::DBC::ValueType::Unsigned)
         {
            e_CurrentType = C_OscNodeDataPoolContent::eUINT32;
         }
         else
         {
            e_CurrentType = C_OscNodeDataPoolContent::eSINT32;
         }
      }
      else if (c_DbcSignal.bitSize <= 64U)
      {
         if (c_DbcSignal.valueType == Vector::DBC::ValueType::Unsigned)
         {
            e_CurrentType = C_OscNodeDataPoolContent::eUINT64;
         }
         else
         {
            e_CurrentType = C_OscNodeDataPoolContent::eSINT64;
         }
      }
      else
      {
         //Unnecessary
      }

      // set values if valid bit size otherwise default (0) value
      if (c_DbcSignal.bitSize <= 64U)
      {
         f64_MinValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.minimum);
         f64_MaxValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.maximum);
         f64_MinValuePhy = c_DbcSignal.minimum;
         f64_MaxValuePhy = c_DbcSignal.maximum;
      }
      else
      {
         // default values
         e_CurrentType = C_OscNodeDataPoolContent::eUINT8;
         // raw values are already set, only set physical values
         f64_MinValuePhy = (f64_MinValue * c_DbcSignal.factor) + c_DbcSignal.offset;
         f64_MaxValuePhy = (f64_MaxValue * c_DbcSignal.factor) + c_DbcSignal.offset;

         osc_write_log_warning("DBC file import", "Unknown value type error for signal \"" + c_String + "\".");
         orc_WarningMessages.Append("Unknown value type error for signal \"" + c_String + "\".");
         s32_Return = C_WARN;
      }
   }
   else if (c_DbcSignal.extendedValueType == Vector::DBC::Signal::ExtendedValueType::Float)
   {
      e_CurrentType = C_OscNodeDataPoolContent::eFLOAT32;
      f64_MinValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.minimum);
      f64_MaxValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.maximum);
      f64_MinValuePhy = c_DbcSignal.minimum;
      f64_MaxValuePhy = c_DbcSignal.maximum;
   }
   else if (c_DbcSignal.extendedValueType == Vector::DBC::Signal::ExtendedValueType::Double)
   {
      e_CurrentType = C_OscNodeDataPoolContent::eFLOAT64;
      f64_MinValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.minimum);
      f64_MaxValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.maximum);
      f64_MinValuePhy = c_DbcSignal.minimum;
      f64_MaxValuePhy = c_DbcSignal.maximum;
   }
   else
   {
      // default values
      e_CurrentType = C_OscNodeDataPoolContent::eUINT8;
      // raw values are already set, only set physical values
      f64_MinValuePhy = (f64_MinValue * c_DbcSignal.factor) + c_DbcSignal.offset;
      f64_MaxValuePhy = (f64_MaxValue * c_DbcSignal.factor) + c_DbcSignal.offset;

      osc_write_log_warning("DBC file import", "Unknown value type error for signal \"" + c_String + "\".");
      orc_WarningMessages.Append("Unknown value type error for signal \"" + c_String + "\".");
      s32_Return = C_WARN;
   }

   // set the same types for all values
   orc_Element.c_MinValue.SetType(e_CurrentType);
   orc_Element.c_MaxValue.SetType(e_CurrentType);
   // check if raw values of DBC signals are in range for data type
   if (mh_CheckRange(f64_MinValue, e_CurrentType) != C_NO_ERR)
   {
      const C_SclString c_Message = "Signal \"" + c_String +
                                    "\": violation of datatype range in DBC file for minimum raw value \"" +
                                    QString::number(f64_MinValue).toStdString().c_str() +
                                    "\". Correct datatype with \"Start Bit\" and \"Length\" has to be set manually.";
      osc_write_log_warning("DBC file import", c_Message);
      orc_WarningMessages.Append(c_Message);
      s32_Return = C_WARN;
   }
   if (mh_CheckRange(f64_MaxValue, e_CurrentType) != C_NO_ERR)
   {
      const C_SclString c_Message = "Signal \"" + c_String +
                                    "\": violation of datatype range in DBC file for maximum raw value \"" +
                                    QString::number(f64_MaxValue).toStdString().c_str() +
                                    "\". Correct datatype with \"Start Bit\" and \"Length\" has to be set manually.";
      osc_write_log_warning("DBC file import", c_Message);
      orc_WarningMessages.Append(c_Message);
      s32_Return = C_WARN;
   }
   // set values via comfort data pool util interface

   C_OscNodeDataPoolContentUtil::h_SetValueInContent(f64_MinValue, orc_Element.c_MinValue);
   C_OscNodeDataPoolContentUtil::h_SetValueInContent(f64_MaxValue, orc_Element.c_MaxValue);

   if (oq_MultiplexerSignal == true)
   {
      C_SdNdeDpContentUtil::h_InitMinForSignal(orc_Element.c_MinValue, static_cast<uint16_t>(c_DbcSignal.bitSize));
      C_SdNdeDpContentUtil::h_InitMaxForSignal(orc_Element.c_MaxValue, static_cast<uint16_t>(c_DbcSignal.bitSize));
   }

   // get optional initial value if available
   C_OscNodeDataPoolContent c_InitialValue;
   c_InitialValue.SetType(e_CurrentType);
   std::map<std::string, Vector::DBC::Attribute>::const_iterator c_Iter;
   c_Iter = c_DbcSignal.attributeValues.find("GenSigStartValue");
   if (c_Iter != c_DbcSignal.attributeValues.end())
   {
      const Vector::DBC::Attribute c_StartValue = c_Iter->second;
      const std::map<std::string,
                     Vector::DBC::AttributeDefinition>::const_iterator c_IterType =
         orc_DbcNetwork.attributeDefinitions.find(
            c_StartValue.name);
      if (c_IterType != orc_DbcNetwork.attributeDefinitions.end())
      {
         // initial value available
         // check type of initial value, is already raw value
         if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Int)
         {
            f64_InitialValue = static_cast<float64_t>(c_StartValue.integerValue);
         }
         else if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Float)
         {
            f64_InitialValue = c_StartValue.floatValue;
         }
         else if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Hex) // has the same
                                                                                                   // representation
         // as int
         {
            f64_InitialValue = static_cast<float64_t>(c_StartValue.hexValue);
         }
         else if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Enum)
         {
            f64_InitialValue = static_cast<float64_t>(c_StartValue.enumValue);
         }
         else
         {
            const float64_t f64_DefaultPhy = (f64_DEFAULT * c_DbcSignal.factor) + c_DbcSignal.offset;
            const C_SclString c_Message = "Signal \"" + c_String +
                                          "\": Type for initial value unknown. Initial value set to default value \"" +
                                          QString::number(f64_DefaultPhy).toStdString().c_str() + "\".";
            osc_write_log_warning("DBC file import", c_Message);
            orc_WarningMessages.Append(c_Message);
            f64_InitialValue = f64_DEFAULT;
            s32_Return = C_WARN;
         }

         if ((oq_MultiplexerSignal == true) &&
             (C_OscUtils::h_IsFloat64NearlyEqual(f64_InitialValue, 0.0) == false))
         {
            // Multiplexer signal restrictions
            f64_InitialValue = 0.0;
            orq_SignalAdapted = true;
         }

         f64_InitialValuePhy = (f64_InitialValue * c_DbcSignal.factor) + c_DbcSignal.offset;

         // check if raw value of DBC signal are in range for data type
         if (mh_CheckRange(f64_InitialValue, e_CurrentType) != C_NO_ERR)
         {
            const C_SclString c_Message = "Signal \"" + c_String +
                                          "\": violation of datatype range in DBC file for initial raw value \"" +
                                          QString::number(f64_InitialValue).toStdString().c_str() +
                                          "\". Correct datatype with \"Start Bit\" and \"Length\"" \
                                          " has to be set manually.";
            osc_write_log_warning("DBC file import", c_Message);
            orc_WarningMessages.Append(c_Message);
            s32_Return = C_WARN;
         }
         // set available initial value
         C_OscNodeDataPoolContentUtil::h_SetValueInContent(f64_InitialValue, c_InitialValue);
         // set value in range, leave initial value if in range
         C_OscNodeDataPoolContentUtil::E_ValueChangedTo e_ValueChangedTo;
         const int32_t s32_Tmp =
            C_OscNodeDataPoolContentUtil::h_SetValueInMinMaxRange(orc_Element.c_MinValue,
                                                                  orc_Element.c_MaxValue,
                                                                  c_InitialValue,
                                                                  e_ValueChangedTo,
                                                                  C_OscNodeDataPoolContentUtil::eLEAVE_VALUE);
         if (s32_Tmp == C_RANGE)
         {
            // min and max values are interchanged
            // rare case with no practical scenario yet
            // do not know how to handle this better than leave all values as they are
            // and display warning to user
            const C_SclString c_Message = "Signal \"" + c_String + "\": Minimum raw value \"" +
                                          QString::number(f64_MinValue).toStdString().c_str() +
                                          "\" and maximum raw value \"" +
                                          QString::number(f64_MaxValue).toStdString().c_str() +
                                          "\" are interchanged. This is not supported. Values left as they are.";
            osc_write_log_warning("DBC file import", c_Message);
            orc_WarningMessages.Append(c_Message);
            s32_Return = C_WARN;
         }
         else
         {
            if (e_ValueChangedTo != C_OscNodeDataPoolContentUtil::eNO_CHANGE)
            {
               // value was out of range, take closer min or max value
               C_SclString c_PlaceholderMinMax;
               s32_Return = C_WARN;
               if (e_ValueChangedTo == C_OscNodeDataPoolContentUtil::eMIN)
               {
                  c_PlaceholderMinMax = "minimum";
                  f64_InitialValue = f64_MinValue; // set value for later datatype range check
               }
               else if (e_ValueChangedTo == C_OscNodeDataPoolContentUtil::eMAX)
               {
                  c_PlaceholderMinMax = "maximum";
                  f64_InitialValue = f64_MaxValue; // set value for later datatype range check
               }
               else
               {
                  //nothing more to do
               }

               const C_SclString c_Message = "Signal \"" + c_String + "\": Initial value \"" +
                                             QString::number(f64_InitialValuePhy).toStdString().c_str() +
                                             "\" is not between minimum \"" +
                                             QString::number(f64_MinValuePhy).toStdString().c_str() +
                                             "\" and maximum \"" +
                                             QString::number(f64_MaxValuePhy).toStdString().c_str() +
                                             "\" value. Initial value set to " +
                                             c_PlaceholderMinMax + " value.";
               osc_write_log_warning("DBC file import", c_Message);
               orc_WarningMessages.Append(c_Message);
            }
         }
      }
      else
      {
         const C_SclString c_Message = "Signal \"" + c_String +
                                       "\": Type not found.";
         osc_write_log_warning("DBC file import", c_Message);
         orc_WarningMessages.Append(c_Message);
         s32_Return = C_WARN;
      }
   }
   else
   {
      // No specific initial value available, therefore take global or define one
      // and create warning message.
      // Initial value is default if between min and max value otherwise
      // initial value is set to min value.
      // Hint: No range check for global or default initial raw value datatype of DBC signal
      //       because range conflicts can happen very often in dependence of DBC file.

      if (oq_MultiplexerSignal == false)
      {
         // set initial value to default
         C_OscNodeDataPoolContentUtil::h_SetValueInContent(f64_DEFAULT, c_InitialValue);
      }
      else
      {
         // Multiplexer signal -> restriction to zero but no warning due to no real change
         // set initial value to zero
         C_OscNodeDataPoolContentUtil::h_SetValueInContent(0.0, c_InitialValue);
         f64_InitialValue = 0.0;
      }

      // set value in range, leave default initial value if in range
      C_OscNodeDataPoolContentUtil::E_ValueChangedTo e_ValueChangedTo;
      const int32_t s32_Tmp =
         C_OscNodeDataPoolContentUtil::h_SetValueInMinMaxRange(orc_Element.c_MinValue,
                                                               orc_Element.c_MaxValue,
                                                               c_InitialValue,
                                                               e_ValueChangedTo,
                                                               C_OscNodeDataPoolContentUtil::eLEAVE_VALUE);
      if (s32_Tmp == C_RANGE)
      {
         // min and max values are interchanged
         // rare case with no practical scenario yet
         // do not know how to handle this better than leave all values as they are
         // and display warning to user
         const C_SclString c_Message = "Signal \"" + c_String + "\": Minimum raw value \"" +
                                       QString::number(f64_MinValue).toStdString().c_str() +
                                       "\" and maximum raw value \"" +
                                       QString::number(f64_MaxValue).toStdString().c_str() +
                                       "\" are interchanged. This is not supported. Values left as they are.";
         osc_write_log_warning("DBC file import", c_Message);
         orc_WarningMessages.Append(c_Message);
         s32_Return = C_WARN;
      }
      else
      {
         if (e_ValueChangedTo == C_OscNodeDataPoolContentUtil::eNO_CHANGE)
         {
            f64_InitialValuePhy = (f64_InitialValue * c_DbcSignal.factor) + c_DbcSignal.offset;
            if (mhq_DefaultValueDefined == false) // true is standard case; if false, then openSYDE default value is
                                                  // used
            {
               const C_SclString c_Message = "No initial value for signal \"" + c_String +
                                             "\" available. Initial value set to default \"" +
                                             QString::number(f64_InitialValuePhy).toStdString().c_str() +
                                             "\".";
               osc_write_log_warning("DBC file import", c_Message);
               orc_WarningMessages.Append(c_Message);
               s32_Return = C_WARN;
            }
         }
         else
         {
            // initial value was not in min and max range, took closer min or max value
            C_SclString c_PlaceholderMinMax;
            C_SclString c_PlaceholderValue;
            if (e_ValueChangedTo == C_OscNodeDataPoolContentUtil::eMIN)
            {
               c_PlaceholderMinMax = "minimum";
               c_PlaceholderValue = QString::number(f64_MinValuePhy).toStdString().c_str();
               f64_InitialValue = f64_MinValue; // set value for later datatype range check
            }
            else
            {
               c_PlaceholderMinMax = "maximum";
               c_PlaceholderValue = QString::number(f64_MaxValuePhy).toStdString().c_str();
               f64_InitialValue = f64_MaxValue; // set value for later datatype range check
            }

            if (mhq_DefaultValueDefined == true)
            {
               const float64_t f64_DefaultPhy = (f64_DEFAULT * c_DbcSignal.factor) + c_DbcSignal.offset;
               const C_SclString c_Message = "Signal \"" + c_String + "\": Global initial value \"" +
                                             QString::number(f64_DefaultPhy).toStdString().c_str() +
                                             "\" is not between minimum \"" +
                                             QString::number(f64_MinValuePhy).toStdString().c_str() +
                                             "\" and maximum \"" +
                                             QString::number(f64_MaxValuePhy).toStdString().c_str() +
                                             "\" value. Initial value set to " + c_PlaceholderMinMax + " value.";
               osc_write_log_warning("DBC file import", c_Message);
               orc_WarningMessages.Append(c_Message);
            }
            else
            {
               const C_SclString c_Message = "No valid initial value for signal \""  + c_String +
                                             "\" available. Initial value set to " + c_PlaceholderMinMax + " value \"" +
                                             c_PlaceholderValue + "\".";
               osc_write_log_warning("DBC file import", c_Message);
               orc_WarningMessages.Append(c_Message);
            }

            s32_Return = C_WARN;
         }
      }
   }

   // check if raw value of DBC signal are in range for data type after it was set automatically
   if (mh_CheckRange(f64_InitialValue, e_CurrentType) != C_NO_ERR)
   {
      const C_SclString c_Message = "Signal \"" + c_String +
                                    "\": violation of datatype range in DBC file for initial raw value \"" +
                                    QString::number(f64_InitialValue).toStdString().c_str() +
                                    "\" after it was set automatically. Correct datatype with \"Start Bit\" and \"Length\"" \
                                    " has to be set manually.";
      osc_write_log_warning("DBC file import", c_Message);
      orc_WarningMessages.Append(c_Message);
      s32_Return = C_WARN;
   }

   orc_Element.c_DataSetValues.push_back(c_InitialValue); // init value is the first and only element of c_DataSetValues

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get attribute definition

   Search all attribute definitions for the prepared strings and set according
   attribute definition.

   \param[in]  orc_DbcNetwork    network definition from DBC file

   \return
   C_NO_ERR    attribute definitions found
   C_WARN      attribute definition not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportDbc::mh_GetAttributeDefinitions(const Vector::DBC::Network & orc_DbcNetwork)
{
   int32_t s32_Return = C_NO_ERR;

   // search Send Type
   const auto c_DbcAttributeDefinition = orc_DbcNetwork.attributeDefinitions.find(mhc_SEND_TYPE.c_str());

   // attribute found?
   if (c_DbcAttributeDefinition != orc_DbcNetwork.attributeDefinitions.end())
   {
      mhc_AttributeSendType = c_DbcAttributeDefinition->second;
   }
   else
   {
      s32_Return = C_WARN;
      mhc_WarningMessages.Append("Missing attribute definition for send type \"" + mhc_SEND_TYPE + "\".");
      osc_write_log_warning("DBC file import", "Missing attribute definition for send type \"" + mhc_SEND_TYPE + "\".");
   }

   // search default send type
   const auto c_DbcAttributeDefaults = orc_DbcNetwork.attributeDefaults.find(mhc_SEND_TYPE.c_str());

   // attribute found?
   if (c_DbcAttributeDefaults != orc_DbcNetwork.attributeDefaults.end())
   {
      mhc_DefaultSendTypeValue = (c_DbcAttributeDefaults->second).stringValue.c_str();
   }
   else
   {
      s32_Return = C_WARN;
      mhc_DefaultSendTypeValue = "OnEvent";
      mhc_WarningMessages.Append("Missing default attribute definition for send type \"" + mhc_SEND_TYPE + "\"."
                                 " Set to \"" + mhc_DefaultSendTypeValue + "\".");
      osc_write_log_warning("DBC file import",
                            "Missing default attribute definition for send type \"" + mhc_SEND_TYPE + "\".");
   }

   // search default initial value
   const auto c_DbcAttributeDefaultsInit = orc_DbcNetwork.attributeDefaults.find(mhc_INITIAL_VALUE.c_str());

   // attribute found?
   mhq_DefaultValueDefined = false;
   if (c_DbcAttributeDefaultsInit != orc_DbcNetwork.attributeDefaults.end())
   {
      const std::map<std::string,
                     Vector::DBC::AttributeDefinition>::const_iterator c_IterType =
         orc_DbcNetwork.attributeDefinitions.find(
            c_DbcAttributeDefaultsInit->second.name);
      if (c_IterType != orc_DbcNetwork.attributeDefinitions.end())
      {
         if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Int)
         {
            mhf32_DefaultInitialValue = static_cast<float32_t>((c_DbcAttributeDefaultsInit->second).integerValue);
            mhq_DefaultValueDefined = true;
         }
         else if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Hex)
         {
            mhf32_DefaultInitialValue = static_cast<float32_t>((c_DbcAttributeDefaultsInit->second).hexValue);
            mhq_DefaultValueDefined = true;
         }
         else if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Float)
         {
            mhf32_DefaultInitialValue = static_cast<float32_t>((c_DbcAttributeDefaultsInit->second).floatValue);
            mhq_DefaultValueDefined = true;
         }
         else if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Enum)
         {
            mhf32_DefaultInitialValue = static_cast<float32_t>((c_DbcAttributeDefaultsInit->second).enumValue);
            mhq_DefaultValueDefined = true;
         }
         else if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::String)
         {
            const C_SclString c_DefaultInitialValue = (c_DbcAttributeDefaultsInit->second).stringValue.c_str();
            // try to convert to DBC standard raw float value
            try
            {
               mhf32_DefaultInitialValue = static_cast<float32_t>(c_DefaultInitialValue.ToDouble());
               mhq_DefaultValueDefined = true;
            }
            catch (...)
            {
               // strange, print a warning to user
               s32_Return = C_WARN;
               mhc_WarningMessages.Append("Default initial value for signals \"" + c_DefaultInitialValue +
                                          "\" could not be interpreted. Default value set to \"0\".");
               osc_write_log_warning("DBC file import",
                                     "Default initial value for signals \"" + c_DefaultInitialValue +
                                     "\" could not be interpreted. Default value set to \"0\".");
            }
         }
         else
         {
            // strange, print a warning to user
            s32_Return = C_WARN;
            mhc_WarningMessages.Append("Default type value for signals \"" + mhc_SEND_TYPE +
                                       "\". Default value set to \"0\".");
            osc_write_log_warning("DBC file import",
                                  "Default type value for signals \"" + mhc_SEND_TYPE +
                                  "\". Default value set to \"0\".");
         }
      }
      else
      {
         // strange, print a warning to user
         s32_Return = C_WARN;
         mhc_WarningMessages.Append("Default definition for signals \"" + c_DbcAttributeDefaultsInit->second.name +
                                    "\" could not be found. Default value set to \"0\".");
         osc_write_log_warning("DBC file import",
                               "Default definition for signals \"" + c_DbcAttributeDefaultsInit->second.name +
                               "\" could not be found. Default value set to \"0\".");
      }
   }
   // default attribute found?
   if (mhq_DefaultValueDefined == false)
   {
      // no warning, this is allowed!
      mhf32_DefaultInitialValue = 0.0F; // set to openSYDE default value
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get transmission type and time

   Search attribute values for required parameters and copy values to converter
   structure.
   Hint: If the default value in DBC shall used, may be the respective entry in .dbc file
         is missing. Thus the default value is used. (Option: don't accept default
         values but set the parameters for each message explicitly)

   \param[in]   orc_DbcNetwork   Dbc network
   \param[in]   orc_DbcMessage   message definition from DBC file
   \param[out]  orc_Message      target message definitions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDbc::mh_GetTransmission(const Vector::DBC::Network & orc_DbcNetwork,
                                        const Vector::DBC::Message & orc_DbcMessage,
                                        C_CieConverter::C_CieNodeMessage & orc_Message)
{
   C_SclString c_MessageType;
   bool q_CycleTimeFound = false;

   // this is the default (in case of attribute missing)
   orc_Message.c_CanMessage.u32_CycleTimeMs = 100U;

   // search for attribute cycle time (cycle time also means cyclic transmission method)
   for (const auto & rc_DbcAttributeValue : orc_DbcMessage.attributeValues)
   {
      if (mhc_CYCLE_TIME.AnsiCompare(rc_DbcAttributeValue.first.c_str()) == 0)
      {
         const std::map<std::string,
                        Vector::DBC::AttributeDefinition>::const_iterator c_IterType =
            orc_DbcNetwork.attributeDefinitions.find(rc_DbcAttributeValue.first);
         if (c_IterType != orc_DbcNetwork.attributeDefinitions.end())
         {
            if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Int)
            {
               orc_Message.c_CanMessage.u32_CycleTimeMs =
                  static_cast<uint32_t>(rc_DbcAttributeValue.second.integerValue);
               q_CycleTimeFound = true;
               break;
            }
         }
      }
   }

   // set global default send type
   // do not print any warnings if specific message has no explicit send type
   if (mhc_DefaultSendTypeValue.LowerCase().Pos("cyclic") != 0U)
   {
      orc_Message.c_CanMessage.e_TxMethod = C_OscCanMessage::E_TxMethodType::eTX_METHOD_CYCLIC;
   }
   else
   {
      orc_Message.c_CanMessage.e_TxMethod = C_OscCanMessage::E_TxMethodType::eTX_METHOD_ON_EVENT;
   }

   // search for attribute send type (keyword 'cyclic' shall always indicate openSYDE method eTX_METHOD_CYCLIC,
   // otherwise it shall be of type eTX_METHOD_ON_EVENT)
   for (const auto & rc_DbcAttributeValue : orc_DbcMessage.attributeValues)
   {
      // check for message send type
      if (mhc_SEND_TYPE.AnsiCompare(rc_DbcAttributeValue.first.c_str()) == 0)
      {
         const std::map<std::string,
                        Vector::DBC::AttributeDefinition>::const_iterator c_IterType =
            orc_DbcNetwork.attributeDefinitions.find(rc_DbcAttributeValue.first);
         if (c_IterType != orc_DbcNetwork.attributeDefinitions.end())
         {
            // attribute send type found -> overwrite default value
            // there must be an enum! if not, then take default value
            if (c_IterType->second.valueType.type == Vector::DBC::AttributeValueType::Type::Enum)
            {
               const uint32_t u32_Type = static_cast<uint32_t>(rc_DbcAttributeValue.second.enumValue);
               c_MessageType = mhc_AttributeSendType.valueType.enumValues[u32_Type].c_str();
               const C_SclString c_Tmp = c_MessageType.LowerCase();
               if (c_Tmp != "")
               {
                  if (c_Tmp.Pos("cyclic") > 0)
                  {
                     orc_Message.c_CanMessage.e_TxMethod = C_OscCanMessage::E_TxMethodType::eTX_METHOD_CYCLIC;
                     // only display warning if it does not match exactly (case-insensitive)
                     if (c_Tmp != "cyclic")
                     {
                        const C_SclString c_Message = "Message type \"" + c_MessageType +
                                                      "\" interpreted as \"Cyclic\".";

                        osc_write_log_warning("DBC file import", c_Message);
                        orc_Message.c_Warnings.Append(c_Message);
                     }
                  }
                  else
                  {
                     // all other messages send types shall be interpreted as "OnEvent"
                     orc_Message.c_CanMessage.e_TxMethod = C_OscCanMessage::E_TxMethodType::eTX_METHOD_ON_EVENT;
                     // only display warning if it does not match exactly (case-insensitive)
                     if (c_Tmp != "onevent")
                     {
                        const C_SclString c_Message = "Message type \"" + c_MessageType +
                                                      "\" interpreted as \"OnEvent\".";
                        osc_write_log_warning("DBC file import", c_Message);
                        orc_Message.c_Warnings.Append(c_Message);
                     }
                  }
                  break;
               }
            }
         }
      }
   }

   if ((orc_Message.c_CanMessage.e_TxMethod == C_OscCanMessage::E_TxMethodType::eTX_METHOD_CYCLIC) &&
       (q_CycleTimeFound == false))
   {
      const C_SclString c_Message = "Message cycle time was 0ms or not valid, default set to " +
                                    C_SclString::IntToStr(orc_Message.c_CanMessage.u32_CycleTimeMs) + "ms.";

      osc_write_log_warning("DBC file import", c_Message);
      orc_Message.c_Warnings.Append(c_Message);
   }
   else if (orc_Message.c_CanMessage.e_TxMethod == C_OscCanMessage::E_TxMethodType::eTX_METHOD_ON_EVENT)
   {
      orc_Message.c_CanMessage.u32_CycleTimeMs = 0U;
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Check if datatype is big enough for value.

   \param[in]  of64_Value            value to check for range condition
   \param[in]  oe_Datatype           datatype of value

   \return
   C_NO_ERR    value in range
   C_RANGE     value can't be stored fully for this datatype
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieImportDbc::mh_CheckRange(const float64_t of64_Value, const C_OscNodeDataPoolContent::E_Type oe_Datatype)
{
   int32_t s32_Return;

   if ((oe_Datatype == C_OscNodeDataPoolContent::eUINT8) &&
       ((of64_Value < 0.0) || (of64_Value >= std::pow(2.0, 8.0))))
   {
      s32_Return = C_RANGE;
   }
   else if ((oe_Datatype == C_OscNodeDataPoolContent::eSINT8) &&
            ((of64_Value < -std::pow(2.0, 7.0)) || (of64_Value >= std::pow(2.0, 7.0))))
   {
      s32_Return = C_RANGE;
   }
   else if ((oe_Datatype == C_OscNodeDataPoolContent::eUINT16) &&
            ((of64_Value < 0.0) || (of64_Value >= std::pow(2.0, 16.0))))
   {
      s32_Return = C_RANGE;
   }
   else if ((oe_Datatype == C_OscNodeDataPoolContent::eSINT16) &&
            ((of64_Value < -std::pow(2.0, 15.0)) || (of64_Value >= std::pow(2.0, 15.0))))
   {
      s32_Return = C_RANGE;
   }
   else if ((oe_Datatype == C_OscNodeDataPoolContent::eUINT32) &&
            ((of64_Value < 0.0) || (of64_Value >= std::pow(2.0, 32.0))))
   {
      s32_Return = C_RANGE;
   }
   else if ((oe_Datatype == C_OscNodeDataPoolContent::eSINT32) &&
            ((of64_Value < -std::pow(2.0, 31.0)) || (of64_Value >= std::pow(2.0, 31.0))))
   {
      s32_Return = C_RANGE;
   }
   else if ((oe_Datatype == C_OscNodeDataPoolContent::eUINT64) &&
            ((of64_Value < 0.0) || (of64_Value >= std::pow(2.0, 64.0))))
   {
      s32_Return = C_RANGE;
   }
   else if ((oe_Datatype == C_OscNodeDataPoolContent::eSINT64) &&
            ((of64_Value < -std::pow(2.0, 63.0)) || (of64_Value >= std::pow(2.0, 63.0))))
   {
      s32_Return = C_RANGE;
   }
   else
   {
      // OK, we do not check for float range because there should be no case in practice
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle/re-escape critical symbols for DBC

   Current:
   Only handle " character: Replace \" by ".

   Cf. C_CieExportDbc::mh_EscapeCriticalSymbols() for counter part.

   \param[in] orc_String String to escape

   \return
   Escaped string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CieImportDbc::mh_ReEscapeCriticalSymbols(const C_SclString & orc_String)
{
   QString c_Temp(orc_String.c_str());

   c_Temp.replace("\\\"", "\"");

   return c_Temp.toStdString().c_str();
}
