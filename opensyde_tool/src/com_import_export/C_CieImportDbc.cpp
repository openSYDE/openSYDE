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

#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_CieImportDbc.h"

#include "TGLFile.h"
#include "TGLUtils.h"

#include "C_OSCNodeDataPoolContent.h"
#include "C_SdNdeDataPoolContentUtil.h"
#include "C_OSCLoggingHandler.h"
#include "C_CieUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_scl;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const C_SCLString C_CieImportDbc::mhc_SendType = "GenMsgSendType";
const C_SCLString C_CieImportDbc::mhc_CycleTime = "GenMsgCycleTime";
const C_SCLString C_CieImportDbc::mhc_InitialValue = "GenSigStartValue";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */
Vector::DBC::AttributeDefinition C_CieImportDbc::mhc_AttributeSendType;
C_SCLString C_CieImportDbc::mhc_DefaultSendTypeValue; // get value from network attribute default values
float32 C_CieImportDbc::mhf32_DefaultInitialValue;    // default initial value of DBC file or openSYDE
bool C_CieImportDbc::mhq_DefaultValueDefined;         // in DBC file
C_SCLStringList C_CieImportDbc::mhc_WarningMessages;  // empty list
C_SCLString C_CieImportDbc::mhc_ErrorMessage = "";    // empty string

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Import network configuration from .dbc file

   The caller is responsible to provided valid pointers for the function parameters.

   \param[in]  orc_File               path and name of .dbc file
   \param[out] orc_Definition         communication stack definition filled with data read from .dbc file
   \param[out] orc_WarningMessages    list of global warnings e.g. why some messages could not be imported
   \param[out] orc_ErrorMessage       error message with reason for failed import
   \param[in]  oq_AddUnmappedMessages Optional flag to include unmapped messages in the output

   \return
   C_NO_ERR    required data from file successfully stored in orc_Definition
   C_RANGE     orc_File is empty string
   C_CONFIG    orc_File does not point to a valid file
   C_RD_WR     error while reading file
   C_WARN      unknown parameter found -> default value set and error reported
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportDbc::h_ImportNetwork(const C_SCLString & orc_File,
                                       C_CieConverter::C_CIECommDefinition & orc_Definition,
                                       C_SCLStringList & orc_WarningMessages, C_SCLString & orc_ErrorMessage,
                                       const bool oq_AddUnmappedMessages)
{
   sint32 s32_Return;

   mhc_WarningMessages.Clear(); // clear old warning messages for this import
   mhc_ErrorMessage = "";       // clear old error message for this import
   Vector::DBC::Network c_DbcNetwork;

   osc_write_log_info("DBC file import", "Reading DBC file \"" + orc_File + "\" ...");
   s32_Return = mh_ReadFile(orc_File, c_DbcNetwork);

   if (s32_Return == stw_errors::C_NO_ERR)
   {
      // get attribute definitions for send type
      osc_write_log_info("DBC file import", "Reading attribute definitions of DBC file ...");
      s32_Return = mh_GetAttributeDefinitions(c_DbcNetwork);
   }

   if ((s32_Return == stw_errors::C_NO_ERR) || (s32_Return == stw_errors::C_WARN))
   {
      uint32 u32_Nodes = 0U;
      stw_scl::C_SCLString c_FileName = stw_tgl::TGL_ExtractFileName(orc_File);
      c_FileName.SetLength(c_FileName.Length() - 4U); //no extension

      // add bus information
      orc_Definition.c_Bus.c_Name = c_FileName; // file name means network name
      orc_Definition.c_Bus.c_Comment = c_DbcNetwork.comment.c_str();

      osc_write_log_info("DBC file import",
                         "Reading node and messages with signals of network \"" + c_FileName + "\" ...");

      // assign node definitions with messages
      // there is also a check if each message was assigned
      std::set<std::string> c_MessageAssignment;
      for (auto c_DbcMessage : c_DbcNetwork.messages)
      {
         c_MessageAssignment.insert(c_DbcMessage.second.name);
      }
      orc_Definition.c_Nodes.resize(c_DbcNetwork.nodes.size());
      for (auto c_DbcNode : c_DbcNetwork.nodes)
      {
         C_CieConverter::C_CIENode & c_Node = orc_Definition.c_Nodes[u32_Nodes];

         if (mh_GetNode(c_DbcNode.second, c_Node) != stw_errors::C_NO_ERR)
         {
            s32_Return = stw_errors::C_WARN;
         }

         osc_write_log_info("DBC file import", "Reading messages with signals for node \"" + c_Node.c_Properties.c_Name +
                            "\" ...");

         for (auto c_DbcMessage : c_DbcNetwork.messages)
         {
            sint32 s32_Tmp = mh_GetMessage(c_DbcMessage.second, c_Node);
            // check if message is assigned to a node
            if (s32_Tmp != stw_errors::C_CONFIG)
            {
               // message is assigned to a node, then we erase this message from temporary message assignment set
               std::set<std::string>::const_iterator c_Iter = c_MessageAssignment.find(c_DbcMessage.second.name);
               if (c_Iter != c_MessageAssignment.end())
               {
                  c_MessageAssignment.erase(c_Iter);
               }
               // check if there are any warnings
               if (s32_Tmp != stw_errors::C_NO_ERR)
               {
                  s32_Return = stw_errors::C_WARN;
               }
            }
         }
         u32_Nodes++;
      }
      // check if there are some messages left which are not assigned to a node
      if (c_MessageAssignment.size() != 0)
      {
         // some messages left, print warnings
         std::set<std::string>::const_iterator c_Iter;
         for (c_Iter = c_MessageAssignment.begin(); c_Iter != c_MessageAssignment.end(); ++c_Iter)
         {
            if (oq_AddUnmappedMessages)
            {
               for (auto c_DbcMessage : c_DbcNetwork.messages)
               {
                  if (c_DbcMessage.second.name.compare(*c_Iter) == 0)
                  {
                     //Add found message to unmapped messages
                     C_CieConverter::C_CIENodeMessage c_Message;

                     if (C_CieImportDbc::mh_PrepareMessage(c_DbcMessage.second, c_Message) != stw_errors::C_NO_ERR)
                     {
                        s32_Return = stw_errors::C_WARN;
                     }

                     orc_Definition.c_UnmappedMessages.push_back(c_Message);

                     //Stop searching after finding the message
                     break;
                  }
               }
            }
            else
            {
               const C_SCLString c_String = C_SCLString((*c_Iter).c_str());
               //Report issue
               osc_write_log_warning("DBC file import",
                                     "message \"" + c_String + "\" is not assigned to a node and ignored.");
               mhc_WarningMessages.Append("Message \"" + c_String + "\" is not assigned to a node and ignored.");
            }
         }
         if (oq_AddUnmappedMessages == false)
         {
            s32_Return = stw_errors::C_WARN;
         }
      }
   }

   osc_write_log_info("DBC file import", "DBC network successfully imported.");

   orc_WarningMessages = mhc_WarningMessages; // set warning messages for caller
   orc_ErrorMessage = mhc_ErrorMessage;       // set error message for caller

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Read data from .dbc file

   \param[in]  orc_File        path and name of .dbc file
   \param[out] orc_Network     data read from .dbc file

   \return
   C_NO_ERR    all data successfully read from file
   C_RANGE     orc_File is empty string
   C_CONFIG    orc_File does not point to a valid file
   C_RD_WR     error while reading file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportDbc::mh_ReadFile(const C_SCLString & orc_File, Vector::DBC::Network & orc_Network)
{
   sint32 s32_Return = stw_errors::C_NO_ERR;

   if (orc_File == "")
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else if (stw_tgl::TGL_FileExists(orc_File) == false)
   {
      mhc_ErrorMessage = "DBC file \"" + orc_File + "\" does not exist.";
      osc_write_log_error("DBC file import", mhc_ErrorMessage);
      s32_Return = stw_errors::C_CONFIG;
   }
   else
   {
      // load database file
      Vector::DBC::File c_File;
      if (c_File.load(orc_Network, orc_File.c_str()) != Vector::DBC::Status::Ok)
      {
         mhc_ErrorMessage = "Can't read DBC file \"" + orc_File + "\".";
         osc_write_log_error("DBC file import", mhc_ErrorMessage);
         s32_Return = stw_errors::C_RD_WR;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get node definitions

   \param[in]  orc_DbcNode           node definitions from DBC file
   \param[out] orc_Node              target node definitions

   \return
   C_NO_ERR    all data successfully retrieved
   C_WARN      unknown parameter found -> default value set and error reported
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportDbc::mh_GetNode(const Vector::DBC::Node & orc_DbcNode, C_CieConverter::C_CIENode & orc_Node)
{
   sint32 s32_Return = stw_errors::C_NO_ERR;

   orc_Node.c_Properties.c_Name = orc_DbcNode.name.c_str();
   orc_Node.c_Properties.c_Comment = orc_DbcNode.comment.c_str();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get message definition

   \param[in]      orc_DbcMessage        message definition from DBC file
   \param[in, out] orc_Node              target node definitions

   \return
   C_NO_ERR    all data successfully retrieved
   C_WARN      unknown parameter found -> default value set and error reported
   C_CONFIG    node does not contain this message
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportDbc::mh_GetMessage(const Vector::DBC::Message & orc_DbcMessage, C_CieConverter::C_CIENode & orc_Node)
{
   sint32 s32_Return = stw_errors::C_NO_ERR;
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
      for (auto c_DbcTransmitter : orc_DbcMessage.transmitters)
      {
         if (orc_Node.c_Properties.c_Name.AnsiCompare(c_DbcTransmitter.c_str()) == 0)
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

   // check on receiver node
   for (auto c_DbcSignal : orc_DbcMessage.signals)
   {
      for (auto c_DbcReceiver : c_DbcSignal.second.receivers)
      {
         if (orc_Node.c_Properties.c_Name.AnsiCompare(c_DbcReceiver.c_str()) == 0)
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
      C_CieConverter::C_CIENodeMessage c_TxMessage;

      if (C_CieImportDbc::mh_PrepareMessage(orc_DbcMessage, c_TxMessage) != stw_errors::C_NO_ERR)
      {
         s32_Return = stw_errors::C_WARN;
      }

      orc_Node.c_TxMessages.push_back(c_TxMessage);
   }
   else if (q_IsRxMessage == true)
   {
      C_CieConverter::C_CIENodeMessage c_RxMessage;

      if (C_CieImportDbc::mh_PrepareMessage(orc_DbcMessage, c_RxMessage) != stw_errors::C_NO_ERR)
      {
         s32_Return = stw_errors::C_WARN;
      }

      orc_Node.c_RxMessages.push_back(c_RxMessage);
   }
   else
   {
      // message is not assigned to this node
      s32_Return = stw_errors::C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse and convert message

   \param[in]     orc_DbcMessage       Message to convert
   \param[in,out] orc_Message          Converted message

   \retval   C_NO_ERR   No error occurred
   \retval   C_WARN     Could not find at least one thing
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportDbc::mh_PrepareMessage(const Vector::DBC::Message & orc_DbcMessage,
                                         C_CieConverter::C_CIENodeMessage & orc_Message)
{
   sint32 s32_Return = stw_errors::C_NO_ERR;

   orc_Message.c_CanMessage.c_Name = orc_DbcMessage.name.c_str();
   orc_Message.c_CanMessage.c_Comment = orc_DbcMessage.comment.c_str();
   if (orc_DbcMessage.id & (1 << 31U))
   {
      orc_Message.c_CanMessage.q_IsExtended = true;
   }
   else
   {
      orc_Message.c_CanMessage.q_IsExtended = false;
   }

   orc_Message.c_CanMessage.u32_CanId = orc_DbcMessage.id & 0x7FFFFFFF;
   orc_Message.c_CanMessage.u16_Dlc = static_cast<uint16>(orc_DbcMessage.size);

   osc_write_log_info("DBC file import",
                      "Importing signals for CAN message \"" + orc_Message.c_CanMessage.c_Name + "\" ...");

   // get all signal definitions for this message
   for (auto c_DbcSignal : orc_DbcMessage.signals)
   {
      if (mh_GetSignal(c_DbcSignal.second, orc_Message) != stw_errors::C_NO_ERR)
      {
         s32_Return = stw_errors::C_WARN;
      }
   }

   // get transmission definitions
   if (mh_GetTransmission(orc_DbcMessage, orc_Message) != stw_errors::C_NO_ERR)
   {
      s32_Return = stw_errors::C_WARN;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get signal definition

   \param[in]  orc_DbcSignal         signal definition from DBC file
   \param[out] orc_Message           target message definitions

   \return
   C_NO_ERR    all data successfully retrieved
   C_WARN      unknown parameter found -> default value set and warning reported
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportDbc::mh_GetSignal(const Vector::DBC::Signal & orc_DbcSignal,
                                    C_CieConverter::C_CIENodeMessage & orc_Message)
{
   sint32 s32_Return = stw_errors::C_NO_ERR;

   std::map<uintn, std::string>::const_iterator c_ItValueDescr;

   C_CieConverter::C_CIECanSignal c_Signal;

   c_Signal.u16_ComBitStart = static_cast<uint16>(orc_DbcSignal.startBit);
   c_Signal.u16_ComBitLength = static_cast<uint16>(orc_DbcSignal.bitSize);
   if ((orc_DbcSignal.byteOrder == Vector::DBC::ByteOrder::Motorola) ||
       (orc_DbcSignal.byteOrder == Vector::DBC::ByteOrder::BigEndian))
   {
      c_Signal.e_ComByteOrder = C_OSCCanSignal::E_ByteOrderType::eBYTE_ORDER_MOTOROLA;
   }
   else if ((orc_DbcSignal.byteOrder == Vector::DBC::ByteOrder::Intel) ||
            (orc_DbcSignal.byteOrder == Vector::DBC::ByteOrder::LittleEndian))
   {
      c_Signal.e_ComByteOrder = C_OSCCanSignal::E_ByteOrderType::eBYTE_ORDER_INTEL;
   }
   else
   {
      C_SCLString c_String = orc_DbcSignal.name.c_str();

      // default value
      c_Signal.e_ComByteOrder = C_OSCCanSignal::E_ByteOrderType::eBYTE_ORDER_INTEL;
      osc_write_log_warning("DBC file import", "\"" + c_String + "\" signal byte order type error");
      orc_Message.c_Warnings.Append("\"" + c_String + "\" signal byte order type error");
      s32_Return = stw_errors::C_WARN;
   }

   // Copy and convert the value descriptions
   for (c_ItValueDescr = orc_DbcSignal.valueDescriptions.begin();
        c_ItValueDescr != orc_DbcSignal.valueDescriptions.end();
        ++c_ItValueDescr)
   {
      c_Signal.c_ValueDescription.insert(std::pair<uint32, C_SCLString>(
                                            static_cast<uint32>(c_ItValueDescr->first),
                                            C_SCLString(c_ItValueDescr->second.c_str())));
   }

   c_Signal.c_Element.c_Name = orc_DbcSignal.name.c_str();
   c_Signal.c_Element.c_Comment = orc_DbcSignal.comment.c_str();
   c_Signal.c_Element.f64_Factor = orc_DbcSignal.factor;
   c_Signal.c_Element.f64_Offset = orc_DbcSignal.offset;
   c_Signal.c_Element.c_Unit = orc_DbcSignal.unit.c_str();
   if (mh_GetSignalValues(orc_DbcSignal, c_Signal.c_Element, orc_Message.c_Warnings) != stw_errors::C_NO_ERR)
   {
      s32_Return = stw_errors::C_WARN;
   }

   // Handle multiplexing information
   if (orc_DbcSignal.multiplexorSwitch == true)
   {
      c_Signal.e_MultiplexerType = C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL;
      c_Signal.u16_MultiplexValue = 0; //
   }
   else if (orc_DbcSignal.multiplexedSignal == true)
   {
      c_Signal.e_MultiplexerType = C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL;
      c_Signal.u16_MultiplexValue = orc_DbcSignal.multiplexerSwitchValue;
   }
   else
   {
      c_Signal.e_MultiplexerType = C_OSCCanSignal::eMUX_DEFAULT;
      c_Signal.u16_MultiplexValue = 0; // default
   }
   C_CieImportDbc::mh_VerifySignalValueTable(c_Signal);

   orc_Message.c_CanMessage.c_Signals.push_back(c_Signal);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Verify if all signal values are inside of the signal value range, if not: remove

   \param[in,out] orc_DbcSignal Signal for which to clean up the value tables
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDbc::mh_VerifySignalValueTable(C_CieConverter::C_CIECanSignal & orc_DbcSignal)
{
   if (orc_DbcSignal.u16_ComBitLength <= 64U)
   {
      const uint64 u64_MaxVal = orc_DbcSignal.u16_ComBitLength ==
                                64U ? std::numeric_limits<uint64>::max() : ((1ULL << orc_DbcSignal.u16_ComBitLength) -
                                                                            1ULL);
      for (std::map<uint32, stw_scl::C_SCLString>::const_iterator c_ItValueDescr =
              orc_DbcSignal.c_ValueDescription.begin();
           c_ItValueDescr != orc_DbcSignal.c_ValueDescription.end();)
      {
         if (static_cast<uint64>(c_ItValueDescr->first) <= u64_MaxVal)
         {
            //Fine, iterate to next position
            ++c_ItValueDescr;
         }
         else
         {
            const C_SCLString & rc_String = orc_DbcSignal.c_Element.c_Name.c_str();
            const C_SCLString & c_String2 = c_ItValueDescr->second;
            osc_write_log_warning("DBC file import",
                                  "signal \"" + rc_String + "\" value \"" + c_String2 +
                                  "\" removed, because value " + C_SCLString::IntToStr(
                                     c_ItValueDescr->first) + " out of range of " + C_SCLString::IntToStr(
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

   \param[in]  orc_DbcSignal         signal definition from DBC file
   \param[out] orc_Element           element for CAN message signals
   \param[out] orc_WarningMessages   list of warnings for each message

   \return
   C_NO_ERR    all data successfully retrieved
   C_WARN      unknown parameter found -> default value set and error reported
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportDbc::mh_GetSignalValues(const Vector::DBC::Signal & orc_DbcSignal,
                                          C_CieConverter::C_CIEDataPoolElement & orc_Element,
                                          C_SCLStringList & orc_WarningMessages)
{
   sint32 s32_Return = stw_errors::C_NO_ERR;
   C_SCLString c_String = orc_DbcSignal.name.c_str();

   Vector::DBC::Signal c_DbcSignal = orc_DbcSignal; // remove const binding because of minimumRawValue() and
                                                    // maximumRawValue()

   C_OSCNodeDataPoolContent::E_Type e_CurrentType; // guarantees the same type for min, max and initial value
   const float64 f64_DEFAULT = mhf32_DefaultInitialValue;
   float64 f64_MinValue = f64_DEFAULT;     // default
   float64 f64_MaxValue = f64_DEFAULT;     // default
   float64 f64_InitialValue = f64_DEFAULT; // default
   float64 f64_MinValuePhy;                // used to display warning messages with physical values to user
   float64 f64_MaxValuePhy;                //                             -"-
   float64 f64_InitialValuePhy;            //                             -"-

   // extendedValueType is currently set only in case of variable type float or double
   if ((c_DbcSignal.extendedValueType == Vector::DBC::Signal::ExtendedValueType::Integer) ||
       (c_DbcSignal.extendedValueType == Vector::DBC::Signal::ExtendedValueType::Undefined))
   {
      // set type
      if (c_DbcSignal.bitSize <= 8U)
      {
         if (c_DbcSignal.valueType == Vector::DBC::ValueType::Unsigned)
         {
            e_CurrentType = C_OSCNodeDataPoolContent::eUINT8;
         }
         else
         {
            e_CurrentType = C_OSCNodeDataPoolContent::eSINT8;
         }
      }
      else if (c_DbcSignal.bitSize <= 16U)
      {
         if (c_DbcSignal.valueType == Vector::DBC::ValueType::Unsigned)
         {
            e_CurrentType = C_OSCNodeDataPoolContent::eUINT16;
         }
         else
         {
            e_CurrentType = C_OSCNodeDataPoolContent::eSINT16;
         }
      }
      else if (c_DbcSignal.bitSize <= 32U)
      {
         if (c_DbcSignal.valueType == Vector::DBC::ValueType::Unsigned)
         {
            e_CurrentType = C_OSCNodeDataPoolContent::eUINT32;
         }
         else
         {
            e_CurrentType = C_OSCNodeDataPoolContent::eSINT32;
         }
      }
      else if (c_DbcSignal.bitSize <= 64U)
      {
         if (c_DbcSignal.valueType == Vector::DBC::ValueType::Unsigned)
         {
            e_CurrentType = C_OSCNodeDataPoolContent::eUINT64;
         }
         else
         {
            e_CurrentType = C_OSCNodeDataPoolContent::eSINT64;
         }
      }
      else
      {
         //Unnecessary
      }

      // set values if valid bit size otherwise default (0) value
      if (c_DbcSignal.bitSize <= 64U)
      {
         f64_MinValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.minimumPhysicalValue);
         f64_MaxValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.maximumPhysicalValue);
         f64_MinValuePhy = c_DbcSignal.minimumPhysicalValue;
         f64_MaxValuePhy = c_DbcSignal.maximumPhysicalValue;
      }
      else
      {
         // default values
         e_CurrentType = C_OSCNodeDataPoolContent::eUINT8;
         // raw values are already set, only set physical values
         f64_MinValuePhy = (f64_MinValue * c_DbcSignal.factor) + c_DbcSignal.offset;
         f64_MaxValuePhy = (f64_MaxValue * c_DbcSignal.factor) + c_DbcSignal.offset;

         osc_write_log_warning("DBC file import", "Unknown value type error for signal \"" + c_String + "\".");
         orc_WarningMessages.Append("Unknown value type error for signal \"" + c_String + "\".");
         s32_Return = stw_errors::C_WARN;
      }
   }
   else if (c_DbcSignal.extendedValueType == Vector::DBC::Signal::ExtendedValueType::Float)
   {
      e_CurrentType = C_OSCNodeDataPoolContent::eFLOAT32;
      f64_MinValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.minimumPhysicalValue);
      f64_MaxValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.maximumPhysicalValue);
      f64_MinValuePhy = c_DbcSignal.minimumPhysicalValue;
      f64_MaxValuePhy = c_DbcSignal.maximumPhysicalValue;
   }
   else if (c_DbcSignal.extendedValueType == Vector::DBC::Signal::ExtendedValueType::Double)
   {
      e_CurrentType = C_OSCNodeDataPoolContent::eFLOAT64;
      f64_MinValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.minimumPhysicalValue);
      f64_MaxValue = c_DbcSignal.physicalToRawValue(c_DbcSignal.maximumPhysicalValue);
      f64_MinValuePhy = c_DbcSignal.minimumPhysicalValue;
      f64_MaxValuePhy = c_DbcSignal.maximumPhysicalValue;
   }
   else
   {
      // default values
      e_CurrentType = C_OSCNodeDataPoolContent::eUINT8;
      // raw values are already set, only set physical values
      f64_MinValuePhy = (f64_MinValue * c_DbcSignal.factor) + c_DbcSignal.offset;
      f64_MaxValuePhy = (f64_MaxValue * c_DbcSignal.factor) + c_DbcSignal.offset;

      osc_write_log_warning("DBC file import", "Unknown value type error for signal \"" + c_String + "\".");
      orc_WarningMessages.Append("Unknown value type error for signal \"" + c_String + "\".");
      s32_Return = stw_errors::C_WARN;
   }

   // set the same types for all values
   orc_Element.c_MinValue.SetType(e_CurrentType);
   orc_Element.c_MaxValue.SetType(e_CurrentType);
   // check if raw values of DBC signals are in range for data type
   if (mh_CheckRange(f64_MinValue, e_CurrentType) != stw_errors::C_NO_ERR)
   {
      const C_SCLString c_Message = "Signal \"" + c_String +
                                    "\": violation of datatype range in DBC file for minimum raw value \"" +
                                    C_SCLString(QString::number(f64_MinValue).toStdString().c_str()) +
                                    "\". Correct datatype with \"Start Bit\" and \"Length\" has to be set manually.";
      osc_write_log_warning("DBC file import", c_Message);
      orc_WarningMessages.Append(c_Message);
      s32_Return = stw_errors::C_WARN;
   }
   if (mh_CheckRange(f64_MaxValue, e_CurrentType) != stw_errors::C_NO_ERR)
   {
      const C_SCLString c_Message = "Signal \"" + c_String +
                                    "\": violation of datatype range in DBC file for maximum raw value \"" +
                                    C_SCLString(QString::number(f64_MaxValue).toStdString().c_str()) +
                                    "\". Correct datatype with \"Start Bit\" and \"Length\" has to be set manually.";
      osc_write_log_warning("DBC file import", c_Message);
      orc_WarningMessages.Append(c_Message);
      s32_Return = stw_errors::C_WARN;
   }
   // set values via comfort data pool util interface
   C_SdNdeDataPoolContentUtil::h_SetValueInContent(f64_MinValue, orc_Element.c_MinValue);
   C_SdNdeDataPoolContentUtil::h_SetValueInContent(f64_MaxValue, orc_Element.c_MaxValue);

   // get optional initial value if available
   C_OSCNodeDataPoolContent c_InitialValue;
   c_InitialValue.SetType(e_CurrentType);
   std::map<std::string, Vector::DBC::Attribute>::const_iterator c_Iter;
   c_Iter = c_DbcSignal.attributeValues.find("GenSigStartValue");
   if (c_Iter != c_DbcSignal.attributeValues.end())
   {
      // initial value available
      Vector::DBC::Attribute c_StartValue = c_Iter->second;
      // check type of initial value, is already raw value
      if (c_StartValue.valueType == Vector::DBC::AttributeValueType::Int)
      {
         f64_InitialValue = static_cast<float64>(c_StartValue.integerValue);
      }
      else if (c_StartValue.valueType == Vector::DBC::AttributeValueType::Float)
      {
         f64_InitialValue = c_StartValue.floatValue;
      }
      else if (c_StartValue.valueType == Vector::DBC::AttributeValueType::Hex) // has the same representation as int
      {
         f64_InitialValue = static_cast<float64>(c_StartValue.hexValue);
      }
      else if (c_StartValue.valueType == Vector::DBC::AttributeValueType::Enum)
      {
         f64_InitialValue = static_cast<float64>(c_StartValue.enumValue);
      }
      else
      {
         float f64_DefaultPhy = (f64_DEFAULT * c_DbcSignal.factor) + c_DbcSignal.offset;
         const C_SCLString c_Message = "Signal \"" + c_String +
                                       "\": Type for initial value unknown. Initial value set to default value \"" +
                                       C_SCLString(QString::number(f64_DefaultPhy).toStdString().c_str()) + "\".";
         osc_write_log_warning("DBC file import", c_Message);
         orc_WarningMessages.Append(c_Message);
         f64_InitialValue = f64_DEFAULT;
         s32_Return = stw_errors::C_WARN;
      }
      f64_InitialValuePhy = (f64_InitialValue * c_DbcSignal.factor) + c_DbcSignal.offset;

      // check if raw value of DBC signal are in range for data type
      if (mh_CheckRange(f64_InitialValue, e_CurrentType) != stw_errors::C_NO_ERR)
      {
         const C_SCLString c_Message = "Signal \"" + c_String +
                                       "\": violation of datatype range in DBC file for initial raw value \"" +
                                       C_SCLString(QString::number(f64_InitialValue).toStdString().c_str()) +
                                       "\". Correct datatype with \"Start Bit\" and \"Length\"" \
                                       " has to be set manually.";
         osc_write_log_warning("DBC file import", c_Message);
         orc_WarningMessages.Append(c_Message);
         s32_Return = stw_errors::C_WARN;
      }
      // set available initial value
      C_SdNdeDataPoolContentUtil::h_SetValueInContent(f64_InitialValue, c_InitialValue);
      // set value in range, leave initial value if in range
      C_SdNdeDataPoolContentUtil::E_ValueChangedTo eValueChangedTo;
      sint32 s32_Tmp = C_SdNdeDataPoolContentUtil::h_SetValueInMinMaxRange(orc_Element.c_MinValue,
                                                                           orc_Element.c_MaxValue,
                                                                           c_InitialValue,
                                                                           eValueChangedTo,
                                                                           C_SdNdeDataPoolContentUtil::eLEAVE_VALUE);
      if (s32_Tmp == stw_errors::C_RANGE)
      {
         // min and max values are interchanged
         // rare case with no practical scenario yet
         // do not know how to handle this better than leave all values as they are
         // and display warning to user
         const C_SCLString c_Message = "Signal \"" + c_String + "\": Minimum raw value \"" +
                                       C_SCLString(QString::number(f64_MinValue).toStdString().c_str()) +
                                       "\" and maximum raw value \"" +
                                       C_SCLString(QString::number(f64_MaxValue).toStdString().c_str()) +
                                       "\" are interchanged. This is not supported. Values left as they are.";
         osc_write_log_warning("DBC file import", c_Message);
         orc_WarningMessages.Append(c_Message);
         s32_Return = stw_errors::C_WARN;
      }
      else
      {
         if (eValueChangedTo != C_SdNdeDataPoolContentUtil::eNO_CHANGE)
         {
            // value was out of range, take closer min or max value
            s32_Return = stw_errors::C_WARN;
            C_SCLString c_PlaceholderMinMax;
            if (eValueChangedTo == C_SdNdeDataPoolContentUtil::eMIN)
            {
               c_PlaceholderMinMax = "minimum";
               f64_InitialValue = f64_MinValue; // set value for later datatype range check
            }
            else if (eValueChangedTo == C_SdNdeDataPoolContentUtil::eMAX)
            {
               c_PlaceholderMinMax = "maximum";
               f64_InitialValue = f64_MaxValue; // set value for later datatype range check
            }
            const C_SCLString c_Message = "Signal \"" + c_String + "\": Initial value \"" +
                                          C_SCLString(QString::number(f64_InitialValuePhy).toStdString().c_str()) +
                                          "\" is not between minimum \"" +
                                          C_SCLString(QString::number(f64_MinValuePhy).toStdString().c_str()) +
                                          "\" and maximum \"" +
                                          C_SCLString(QString::number(f64_MaxValuePhy).toStdString().c_str()) +
                                          "\" value. Initial value set to " +
                                          c_PlaceholderMinMax + " value.";
            osc_write_log_warning("DBC file import", c_Message);
            orc_WarningMessages.Append(c_Message);
         }
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

      // set initial value to default
      C_SdNdeDataPoolContentUtil::h_SetValueInContent(f64_DEFAULT, c_InitialValue);
      // set value in range, leave default initial value if in range
      C_SdNdeDataPoolContentUtil::E_ValueChangedTo eValueChangedTo;
      sint32 s32_Tmp = C_SdNdeDataPoolContentUtil::h_SetValueInMinMaxRange(orc_Element.c_MinValue,
                                                                           orc_Element.c_MaxValue,
                                                                           c_InitialValue,
                                                                           eValueChangedTo,
                                                                           C_SdNdeDataPoolContentUtil::eLEAVE_VALUE);
      if (s32_Tmp == stw_errors::C_RANGE)
      {
         // min and max values are interchanged
         // rare case with no practical scenario yet
         // do not know how to handle this better than leave all values as they are
         // and display warning to user
         const C_SCLString c_Message = "Signal \"" + c_String + "\": Minimum raw value \"" +
                                       C_SCLString(QString::number(f64_MinValue).toStdString().c_str()) +
                                       "\" and maximum raw value \"" +
                                       C_SCLString(QString::number(f64_MaxValue).toStdString().c_str()) +
                                       "\" are interchanged. This is not supported. Values left as they are.";
         osc_write_log_warning("DBC file import", c_Message);
         orc_WarningMessages.Append(c_Message);
         s32_Return = stw_errors::C_WARN;
      }
      else
      {
         if (eValueChangedTo == C_SdNdeDataPoolContentUtil::eNO_CHANGE)
         {
            f64_InitialValuePhy = (f64_InitialValue * c_DbcSignal.factor) + c_DbcSignal.offset;
            if (mhq_DefaultValueDefined == false) // true is standard case; if false, then openSYDE default value is
                                                  // used
            {
               const C_SCLString c_Message = "No initial value for signal \"" + c_String +
                                             "\" available. Initial value set to default \"" +
                                             C_SCLString(QString::number(f64_InitialValuePhy).toStdString().c_str()) +
                                             "\".";
               osc_write_log_warning("DBC file import", c_Message);
               orc_WarningMessages.Append(c_Message);
               s32_Return = stw_errors::C_WARN;
            }
         }
         else
         {
            // initial value was not in min and max range, took closer min or max value
            C_SCLString c_PlaceholderMinMax;
            C_SCLString c_PlaceholderValue;
            if (eValueChangedTo == C_SdNdeDataPoolContentUtil::eMIN)
            {
               c_PlaceholderMinMax = "minimum";
               c_PlaceholderValue = C_SCLString(QString::number(f64_MinValuePhy).toStdString().c_str());
               f64_InitialValue = f64_MinValue; // set value for later datatype range check
            }
            else
            {
               c_PlaceholderMinMax = "maximum";
               c_PlaceholderValue = C_SCLString(QString::number(f64_MaxValuePhy).toStdString().c_str());
               f64_InitialValue = f64_MaxValue; // set value for later datatype range check
            }

            if (mhq_DefaultValueDefined == true)
            {
               const float64 f64_DefaultPhy = (f64_DEFAULT * c_DbcSignal.factor) + c_DbcSignal.offset;
               const C_SCLString c_Message = "Signal \"" + c_String + "\": Global initial value \"" +
                                             C_SCLString(QString::number(f64_DefaultPhy).toStdString().c_str()) +
                                             "\" is not between minimum \"" +
                                             C_SCLString(QString::number(f64_MinValuePhy).toStdString().c_str()) +
                                             "\" and maximum \"" +
                                             C_SCLString(QString::number(f64_MaxValuePhy).toStdString().c_str()) +
                                             "\" value. Initial value set to " + c_PlaceholderMinMax + " value.";
               osc_write_log_warning("DBC file import", c_Message);
               orc_WarningMessages.Append(c_Message);
            }
            else
            {
               const C_SCLString c_Message = "No valid initial value for signal \""  + c_String +
                                             "\" available. Initial value set to " + c_PlaceholderMinMax + " value \"" +
                                             c_PlaceholderValue + "\".";
               osc_write_log_warning("DBC file import", c_Message);
               orc_WarningMessages.Append(c_Message);
            }

            s32_Return = stw_errors::C_WARN;
         }
      }
   }
   // check if raw value of DBC signal are in range for data type after it was set automatically
   if (mh_CheckRange(f64_InitialValue, e_CurrentType) != stw_errors::C_NO_ERR)
   {
      const C_SCLString c_Message = "Signal \"" + c_String +
                                    "\": violation of datatype range in DBC file for initial raw value \"" +
                                    C_SCLString(QString::number(f64_InitialValue).toStdString().c_str()) +
                                    "\" after it was set automatically. Correct datatype with \"Start Bit\" and \"Length\"" \
                                    " has to be set manually.";
      osc_write_log_warning("DBC file import", c_Message);
      orc_WarningMessages.Append(c_Message);
      s32_Return = stw_errors::C_WARN;
   }

   orc_Element.c_DataSetValues.push_back(c_InitialValue); // init value is the first and only element of c_DataSetValues

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get attribute definition

   Search all attribute definitions for the prepared strings and set according
   attribute definition.

   \param[in]  orc_DbcNetwork                network definition from DBC file

   \return
   C_NO_ERR    attribute definitions found
   C_WARN      attribute definition not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportDbc::mh_GetAttributeDefinitions(const Vector::DBC::Network & orc_DbcNetwork)
{
   sint32 s32_Return = stw_errors::C_NO_ERR;
   bool q_ValueFound;

   // search Send Type
   q_ValueFound = false;
   for (auto c_DbcAttributeDefinition : orc_DbcNetwork.attributeDefinitions)
   {
      // global enum message send type, exists only one time
      if (mhc_SendType.AnsiCompare(c_DbcAttributeDefinition.first.c_str()) == 0)
      {
         mhc_AttributeSendType = c_DbcAttributeDefinition.second;
         q_ValueFound = true;
         break;
      }
   }
   // attribute found?
   if (q_ValueFound == false)
   {
      s32_Return = stw_errors::C_WARN;
      mhc_WarningMessages.Append("Missing attribute definition for send type \"" + mhc_SendType + "\".");
      osc_write_log_warning("DBC file import", "Missing attribute definition for send type \"" + mhc_SendType + "\".");
   }

   // search default send type
   q_ValueFound = false;
   for (auto c_DbcAttributeDefaults : orc_DbcNetwork.attributeDefaults)
   {
      // global enum message send type, exists only one time
      if (mhc_SendType.AnsiCompare(c_DbcAttributeDefaults.first.c_str()) == 0)
      {
         mhc_DefaultSendTypeValue = C_SCLString((c_DbcAttributeDefaults.second).stringValue.c_str());
         q_ValueFound = true;
         break;
      }
   }
   // default attribute found?
   if (q_ValueFound == false)
   {
      s32_Return = stw_errors::C_WARN;
      mhc_DefaultSendTypeValue = "OnEvent";
      mhc_WarningMessages.Append("Missing default attribute definition for send type \"" + mhc_SendType + "\"."
                                 " Set to \"" + mhc_DefaultSendTypeValue + "\".");
      osc_write_log_warning("DBC file import",
                            "Missing default attribute definition for send type \"" + mhc_SendType + "\".");
   }

   // search default initial value
   mhq_DefaultValueDefined = false;
   for (auto c_DbcAttributeDefaults : orc_DbcNetwork.attributeDefaults)
   {
      // global enum message send type, exists only one time
      if (mhc_InitialValue.AnsiCompare(c_DbcAttributeDefaults.first.c_str()) == 0)
      {
         C_SCLString c_DefaultInitialValue = C_SCLString((c_DbcAttributeDefaults.second).stringValue.c_str());
         // try to convert to DBC standard raw float value
         try
         {
            mhf32_DefaultInitialValue = static_cast<float32>(c_DefaultInitialValue.ToDouble());
            mhq_DefaultValueDefined = true;
         }
         catch (...)
         {
            // could be of type enum
            if ((c_DbcAttributeDefaults.second).valueType == Vector::DBC::AttributeValueType::Enum)
            {
               mhf32_DefaultInitialValue = static_cast<float32>((c_DbcAttributeDefaults.second).enumValue);
               mhq_DefaultValueDefined = true;
            }
            else
            {
               // strange, print a warning to user
               s32_Return = stw_errors::C_WARN;
               mhc_WarningMessages.Append("Default initial value for signals \"" + c_DefaultInitialValue +
                                          "\" could not be interpreted. Default value set to \"0\".");
               osc_write_log_warning("DBC file import",
                                     "Default initial value for signals \"" + c_DefaultInitialValue +
                                     "\" could not be interpreted. Default value set to \"0\".");
            }
         }
         break;
      }
   }
   // default attribute found?
   if (mhq_DefaultValueDefined == false)
   {
      // no warning, this is allowed!
      mhf32_DefaultInitialValue = 0.0f; // set to openSYDE default value
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

   \param[in]  orc_DbcMessage        message definition from DBC file
   \param[out] orc_Message           target message definitions

   \return
   C_NO_ERR    all data successfully retrieved
   C_WARN      unknown parameter found -> default value set and error reported
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieImportDbc::mh_GetTransmission(const Vector::DBC::Message & orc_DbcMessage,
                                          C_CieConverter::C_CIENodeMessage & orc_Message)
{
   sint32 s32_Return = stw_errors::C_NO_ERR;
   C_SCLString c_MessageType;

   // this is the default (in case of attribute missing)
   orc_Message.c_CanMessage.u32_CycleTimeMs = 0U;

   // search for attribute cycle time (cycle time also means cyclic transmission method)
   for (auto c_DbcAttributeValue : orc_DbcMessage.attributeValues)
   {
      if (mhc_CycleTime.AnsiCompare(c_DbcAttributeValue.first.c_str()) == 0)
      {
         if (c_DbcAttributeValue.second.valueType == Vector::DBC::AttributeValueType::Int)
         {
            orc_Message.c_CanMessage.u32_CycleTimeMs = c_DbcAttributeValue.second.integerValue;
            break;
         }
      }
   }

   // set global default send type
   // do not print any warnings if specific message has no explicit send type
   if (mhc_DefaultSendTypeValue.LowerCase().Pos("cyclic"))
   {
      orc_Message.c_CanMessage.e_TxMethod = C_OSCCanMessage::E_TxMethodType::eTX_METHOD_CYCLIC;
   }
   else
   {
      orc_Message.c_CanMessage.e_TxMethod = C_OSCCanMessage::E_TxMethodType::eTX_METHOD_ON_EVENT;
   }

   // search for attribute send type (keyword 'cyclic' shall always indicate openSYDE method eTX_METHOD_CYCLIC,
   // otherwise it shall be of type eTX_METHOD_ON_EVENT)
   for (auto c_DbcAttributeValue : orc_DbcMessage.attributeValues)
   {
      c_MessageType = ""; // clear old value
      // check for message send type
      if (mhc_SendType.AnsiCompare(c_DbcAttributeValue.first.c_str()) == 0)
      {
         // attribute send type found -> overwrite default value
         // there must be an enum! if not, then take default value
         if (c_DbcAttributeValue.second.valueType == Vector::DBC::AttributeValueType::Enum)
         {
            const uint32 u32_Type = c_DbcAttributeValue.second.enumValue;
            c_MessageType = mhc_AttributeSendType.enumValues[u32_Type].c_str();
            C_SCLString c_Tmp = c_MessageType.LowerCase();
            if (c_Tmp != "")
            {
               if (c_Tmp.Pos("cyclic") > 0)
               {
                  orc_Message.c_CanMessage.e_TxMethod = C_OSCCanMessage::E_TxMethodType::eTX_METHOD_CYCLIC;
                  // only display warning if it does not match exactly (case-insensitive)
                  if (c_Tmp != "cyclic")
                  {
                     const C_SCLString c_Message = "Message type \"" + c_MessageType + "\" interpreted as \"Cyclic\".";

                     osc_write_log_warning("DBC file import", c_Message);
                     orc_Message.c_Warnings.Append(c_Message);
                  }
               }
               else
               {
                  // all other messages send types shall be interpreted as "OnEvent"
                  orc_Message.c_CanMessage.e_TxMethod = C_OSCCanMessage::E_TxMethodType::eTX_METHOD_ON_EVENT;
                  // only display warning if it does not match exactly (case-insensitive)
                  if (c_Tmp != "onevent")
                  {
                     const C_SCLString c_Message = "Message type \"" + c_MessageType + "\" interpreted as \"OnEvent\".";
                     osc_write_log_warning("DBC file import", c_Message);
                     orc_Message.c_Warnings.Append(c_Message);
                  }
               }
               break;
            }
         }
      }
   }

   return s32_Return;
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
stw_types::sint32 C_CieImportDbc::mh_CheckRange(float64 of64_Value, C_OSCNodeDataPoolContent::E_Type oe_Datatype)
{
   sint32 s32_Return;

   if ((oe_Datatype == C_OSCNodeDataPoolContent::eUINT8) &&
       ((of64_Value < 0) || (of64_Value >= pow(2, 8))))
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else if ((oe_Datatype == C_OSCNodeDataPoolContent::eSINT8) &&
            ((of64_Value < -pow(2, 7)) || (of64_Value >= pow(2, 7))))
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else if ((oe_Datatype == C_OSCNodeDataPoolContent::eUINT16) &&
            ((of64_Value < 0) || (of64_Value >= pow(2, 16))))
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else if ((oe_Datatype == C_OSCNodeDataPoolContent::eSINT16) &&
            ((of64_Value < -pow(2, 15)) || (of64_Value >= pow(2, 15))))
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else if ((oe_Datatype == C_OSCNodeDataPoolContent::eUINT32) &&
            ((of64_Value < 0) || (of64_Value >= pow(2, 32))))
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else if ((oe_Datatype == C_OSCNodeDataPoolContent::eSINT32) &&
            ((of64_Value < -pow(2, 31)) || (of64_Value >= pow(2, 31))))
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else if ((oe_Datatype == C_OSCNodeDataPoolContent::eUINT64) &&
            ((of64_Value < 0) || (of64_Value >= std::pow(2, 64))))
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else if ((oe_Datatype == C_OSCNodeDataPoolContent::eSINT64) &&
            ((of64_Value < -pow(2, 63)) || (of64_Value >= pow(2, 63))))
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else
   {
      // OK, we do not check for float range because there should be no case in practice
      s32_Return = stw_errors::C_NO_ERR;
   }

   return s32_Return;
}
