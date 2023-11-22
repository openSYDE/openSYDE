//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Adapter to convert extern data structures (implementation)

   Extern data structures are defined by import classes of DBC, DCF, EDS, etc
   file formats. This class is an adapter to convert these data structures to
   project internal openSYDE format for the data pool lists
   (see also C_CieDataPoolListStructure.h).

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <vector>
#include "TglUtils.hpp"
#include "C_CieDataPoolListAdapter.hpp"
#include "C_CieDataPoolListStructure.hpp"
#include "C_CieConverter.hpp"
#include "C_OscCanMessage.hpp"
#include "C_SclString.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "stwerrors.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::errors;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
static const stw::scl::C_SclString mc_MessageLineBreak = "\n"; // must be '\n' for later converting in HTML tag <br>

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert DBC import data structure to openSYDE data structure.

   Assumptions:
   * consistent data (guaranteed by DBC import mechanism, this is no customer API function)

   \param[in]  orc_CieNode    data of DBC node

   \return  project internal openSYDE data structure
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieDataPoolListStructure C_CieDataPoolListAdapter::h_GetStructureFromDbcFileImport(
   const C_CieConverter::C_CieNode & orc_CieNode)
{
   C_CieDataPoolListStructure c_DataStructure; // return value

   // get node name
   c_DataStructure.c_NodeName = orc_CieNode.c_Properties.c_Name.c_str();

   // get Tx core data
   mh_FillUpCoreStructureByDbcValues(orc_CieNode.c_TxMessages,
                                     c_DataStructure.c_Core.c_OscTxMessageData,
                                     c_DataStructure.c_Core.c_OscTxSignalData,
                                     c_DataStructure.c_Core.c_WarningMessagesPerTxMessage);

   // get Rx core data
   mh_FillUpCoreStructureByDbcValues(orc_CieNode.c_RxMessages,
                                     c_DataStructure.c_Core.c_OscRxMessageData,
                                     c_DataStructure.c_Core.c_OscRxSignalData,
                                     c_DataStructure.c_Core.c_WarningMessagesPerRxMessage);

   // get data for user interface functionality
   C_CieDataPoolListAdapter::mh_FillUpUiStructure(c_DataStructure, false);

   return c_DataStructure;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add all necessary elements to the imported core elements

   \param[in]  orc_OscRxMessageData          Imported core Rx message data
   \param[in]  orc_OscTxMessageData          Imported core Tx message data
   \param[in]  orc_InfoMessagesPerMessage    Information messages per message

   \return
   Complete structure as required by our interface
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieDataPoolListStructure C_CieDataPoolListAdapter::h_GetStructureFromDcfAndEdsFileImport(
   const C_OscEdsDcfImportMessageGroup & orc_OscRxMessageData,
   const C_OscEdsDcfImportMessageGroup & orc_OscTxMessageData,
   const std::vector<std::vector<stw::scl::C_SclString> > & orc_InfoMessagesPerMessage)
{
   C_CieDataPoolListStructure c_Retval;

   //Copy
   c_Retval.c_Core.c_OscRxMessageData = orc_OscRxMessageData.c_OscMessageData;
   c_Retval.c_Core.c_OscRxSignalData = orc_OscRxMessageData.c_OscSignalData;
   c_Retval.c_Core.c_OscTxMessageData = orc_OscTxMessageData.c_OscMessageData;
   c_Retval.c_Core.c_OscTxSignalData = orc_OscTxMessageData.c_OscSignalData;

   //Check the flags
   tgl_assert(orc_OscRxMessageData.c_SignalDefaultMinMaxValuesUsed.size() ==
              orc_OscRxMessageData.c_OscSignalData.size());
   tgl_assert(orc_OscTxMessageData.c_SignalDefaultMinMaxValuesUsed.size() ==
              orc_OscTxMessageData.c_OscSignalData.size());

   //Handle messages
   tgl_assert(orc_InfoMessagesPerMessage.size() ==
              (orc_OscRxMessageData.c_OscMessageData.size() + orc_OscTxMessageData.c_OscMessageData.size()));
   if (orc_InfoMessagesPerMessage.size() ==
       (orc_OscRxMessageData.c_OscMessageData.size() + orc_OscTxMessageData.c_OscMessageData.size()))
   {
      c_Retval.c_Core.c_WarningMessagesPerRxMessage.reserve(orc_OscRxMessageData.c_OscMessageData.size());
      c_Retval.c_Core.c_WarningMessagesPerTxMessage.reserve(orc_OscTxMessageData.c_OscMessageData.size());
      for (uint32_t u32_ItInfoMessage = 0; u32_ItInfoMessage < orc_InfoMessagesPerMessage.size(); ++u32_ItInfoMessage)
      {
         QString c_CombinedMessages;
         const std::vector<stw::scl::C_SclString> & rc_MessagesForOneCanMessage =
            orc_InfoMessagesPerMessage[u32_ItInfoMessage];
         for (uint32_t u32_ItGroupedMessage = 0; u32_ItGroupedMessage < rc_MessagesForOneCanMessage.size();
              ++u32_ItGroupedMessage)
         {
            const stw::scl::C_SclString & rc_OneMessage = rc_MessagesForOneCanMessage[u32_ItGroupedMessage];
            c_CombinedMessages += rc_OneMessage.c_str();
            c_CombinedMessages += mc_MessageLineBreak.c_str();
         }
         if (u32_ItInfoMessage < orc_OscRxMessageData.c_OscMessageData.size())
         {
            c_Retval.c_Core.c_WarningMessagesPerRxMessage.push_back(c_CombinedMessages);
         }
         else
         {
            c_Retval.c_Core.c_WarningMessagesPerTxMessage.push_back(c_CombinedMessages);
         }
      }
   }

   //Ui
   mh_FillUpUiStructure(c_Retval, true, &orc_OscRxMessageData.c_SignalDefaultMinMaxValuesUsed,
                        &orc_OscTxMessageData.c_SignalDefaultMinMaxValuesUsed);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Assign node

   \param[in]      orc_Id              Id
   \param[in,out]  orc_OscMessageData  OSC message data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDataPoolListAdapter::h_AssignNode(const C_OscCanInterfaceId & orc_Id,
                                            std::vector<C_OscCanMessage> & orc_OscMessageData)
{
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_OscMessageData.size(); ++u32_ItMessage)
   {
      C_OscCanMessage & rc_Message = orc_OscMessageData[u32_ItMessage];
      rc_Message.c_CanOpenManagerOwnerNodeIndex = orc_Id;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert DBC import data structure to openSYDE core data structure.

   Assumptions:
   * consistent data (guaranteed by DBC import mechanism, this is no customer API function)

   \param[in]   orc_CieNodeMessages    DBC CAN message data of node
   \param[out]  orc_CanMessages        project internal core CAN message data
   \param[out]  orc_CanSignalData      project internal core CAN signal data
   \param[out]  orc_WarningMessages    warning message of each CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDataPoolListAdapter::mh_FillUpCoreStructureByDbcValues(
   const std::vector<C_CieConverter::C_CieNodeMessage> & orc_CieNodeMessages,
   std::vector<C_OscCanMessage> & orc_CanMessages, std::vector<C_OscNodeDataPoolListElement> & orc_CanSignalData,
   std::vector<QString> & orc_WarningMessages)
{
   uint32_t u32_SignalIndex = 0; // signal index for serialized signal data structure

   // fill up can messages
   std::vector<C_CieConverter::C_CieNodeMessage>::const_iterator c_CanMessageIter;
   for (c_CanMessageIter = orc_CieNodeMessages.begin();
        c_CanMessageIter != orc_CieNodeMessages.end();
        ++c_CanMessageIter)
   {
      // restore warnings
      if (c_CanMessageIter->c_Warnings.GetCount() > 0)
      {
         stw::scl::C_SclString c_Tmp;
         for (uint32_t u32_Pos = 0; u32_Pos < c_CanMessageIter->c_Warnings.GetCount(); u32_Pos++)
         {
            c_Tmp += c_CanMessageIter->c_Warnings.Strings[u32_Pos];
            c_Tmp += mc_MessageLineBreak;
         }
         orc_WarningMessages.emplace_back(c_Tmp.c_str());
      }
      else
      {
         // for each message an entry is expected therefore push an empty string
         orc_WarningMessages.emplace_back("");
      }

      // get CAN message content
      C_OscCanMessage c_CanMessage;
      c_CanMessage.c_Name = c_CanMessageIter->c_CanMessage.c_Name;
      c_CanMessage.c_Comment = c_CanMessageIter->c_CanMessage.c_Comment;
      c_CanMessage.u32_CanId = c_CanMessageIter->c_CanMessage.u32_CanId;
      c_CanMessage.q_IsExtended = c_CanMessageIter->c_CanMessage.q_IsExtended;
      c_CanMessage.u16_Dlc = c_CanMessageIter->c_CanMessage.u16_Dlc;
      c_CanMessage.e_TxMethod = c_CanMessageIter->c_CanMessage.e_TxMethod;
      c_CanMessage.u32_CycleTimeMs = c_CanMessageIter->c_CanMessage.u32_CycleTimeMs;

      if (c_CanMessage.e_TxMethod != C_OscCanMessage::eTX_METHOD_ON_EVENT)
      {
         c_CanMessage.u32_TimeoutMs = C_PuiSdUtil::h_GetMessageAutoTimeoutTime(c_CanMessage.u32_CycleTimeMs);
      }
      else
      {
         c_CanMessage.u32_TimeoutMs = 0U;
      }

      // fill up signals
      std::vector<C_CieConverter::C_CieCanSignal>::const_iterator c_CanMessageSignalsIter;
      for (c_CanMessageSignalsIter = c_CanMessageIter->c_CanMessage.c_Signals.begin();
           c_CanMessageSignalsIter != c_CanMessageIter->c_CanMessage.c_Signals.end();
           ++c_CanMessageSignalsIter)
      {
         C_OscCanSignal c_CanSignal;
         c_CanSignal.u16_ComBitStart = c_CanMessageSignalsIter->u16_ComBitStart;
         c_CanSignal.u16_ComBitLength = c_CanMessageSignalsIter->u16_ComBitLength;
         c_CanSignal.e_ComByteOrder = c_CanMessageSignalsIter->e_ComByteOrder;
         c_CanSignal.u32_ComDataElementIndex = u32_SignalIndex;
         c_CanSignal.e_MultiplexerType = c_CanMessageSignalsIter->e_MultiplexerType;
         c_CanSignal.u16_MultiplexValue = c_CanMessageSignalsIter->u16_MultiplexValue;
         c_CanSignal.u32_J1939SuspectParameterNumber = c_CanMessageSignalsIter->u32_J1939Spn;
         u32_SignalIndex++;
         // store signal in CAN message structure
         c_CanMessage.c_Signals.push_back(c_CanSignal);

         // store further signal data for data pool list elements in separate signal container
         const C_CieConverter::C_CieDataPoolElement * const pc_Element = &(c_CanMessageSignalsIter->c_Element);
         C_OscNodeDataPoolListElement c_DataPoolListElement;

         c_DataPoolListElement.c_Comment = pc_Element->c_Comment;
         c_DataPoolListElement.SetType(pc_Element->c_MaxValue.GetType());
         c_DataPoolListElement.c_MaxValue = pc_Element->c_MaxValue;
         c_DataPoolListElement.SetType(pc_Element->c_MinValue.GetType());
         c_DataPoolListElement.c_MinValue = pc_Element->c_MinValue;
         c_DataPoolListElement.c_Name = pc_Element->c_Name;
         c_DataPoolListElement.c_Unit = pc_Element->c_Unit;
         c_DataPoolListElement.f64_Factor = pc_Element->f64_Factor;
         c_DataPoolListElement.f64_Offset = pc_Element->f64_Offset;
         // set initial value which is the first and only element of c_DataSetValues
         c_DataPoolListElement.c_DataSetValues.push_back(pc_Element->c_DataSetValues.at(0));

         // placeholder variables, we must set the type and array
         c_DataPoolListElement.c_Value.SetType(pc_Element->c_MinValue.GetType()); // all elements have the same type
         c_DataPoolListElement.c_Value.SetArray(false);
         c_DataPoolListElement.c_NvmValue.SetType(pc_Element->c_MinValue.GetType()); // all elements have the same type
         c_DataPoolListElement.c_NvmValue.SetArray(false);

         // entries for current signal of can message adapted and complete
         orc_CanSignalData.push_back(c_DataPoolListElement);
      }

      // entries for current can message adapted and complete
      orc_CanMessages.push_back(c_CanMessage);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Internal adapter function to fill up data for Ui functionality.

   This function handles TX methods for messages and calls mh_FillUpUiStructureForSignal for signals.

   \param[in,out]  orc_DataPoolListStructure           openSYDE data structure for data pool lists
   \param[in,out]  oq_ActivateAutoMinMaxForSignals     Flag to automatically set the auto min max flag if necessary
   \param[in]      opc_RxSignalDefaultMinMaxValuesUsed Optional flags if imported core Rx signal data uses the default
                                                       min max values or or specific set values
   \param[in]      opc_TxSignalDefaultMinMaxValuesUsed Optional flags if imported core Tx signal data uses the default
                                                       min max values or or specific set values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDataPoolListAdapter::mh_FillUpUiStructure(C_CieDataPoolListStructure & orc_DataPoolListStructure,
                                                    const bool oq_ActivateAutoMinMaxForSignals,
                                                    const std::vector<uint8_t> * const opc_RxSignalDefaultMinMaxValuesUsed,
                                                    const std::vector<uint8_t> * const opc_TxSignalDefaultMinMaxValuesUsed)
{
   C_PuiSdNodeDataPoolListElement c_DefaultUiSignal;
   uint32_t u32_SignalCounter = 0U;
   const uint8_t * pu8_DefaultMinMaxValuesUsed = NULL;

   //Set import default values (different from default values)
   if (oq_ActivateAutoMinMaxForSignals == true)
   {
      c_DefaultUiSignal.q_AutoMinMaxActive = true;
   }
   else
   {
      c_DefaultUiSignal.q_AutoMinMaxActive = false;
   }
   c_DefaultUiSignal.q_InterpretAsString = false;

   // resize Ui vectors
   // Rx messages
   orc_DataPoolListStructure.c_Ui.c_UiRxMessageData.reserve(orc_DataPoolListStructure.c_Core.c_OscRxMessageData.size());
   std::vector<C_OscCanMessage>::const_iterator c_MessageIter;
   for (c_MessageIter = orc_DataPoolListStructure.c_Core.c_OscRxMessageData.begin();
        c_MessageIter != orc_DataPoolListStructure.c_Core.c_OscRxMessageData.end();
        ++c_MessageIter)
   {
      C_PuiSdNodeCanMessage c_UiMessage;
      c_UiMessage.c_Signals.resize(c_MessageIter->c_Signals.size());
      // Adaption of Ui specific timeout mode
      if (c_MessageIter->u32_TimeoutMs == 0U)
      {
         if ((c_MessageIter->e_TxMethod == C_OscCanMessage::eTX_METHOD_ON_EVENT) ||
             (c_MessageIter->e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254) ||
             (c_MessageIter->e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255))
         {
            // Special case: 0 means disabled timeout check for on event
            c_UiMessage.e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_DISABLED;
         }
         else
         {
            // Using default value in case of cyclic and on change
            c_UiMessage.e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO;
         }
      }
      else
      {
         if ((c_MessageIter->e_TxMethod == C_OscCanMessage::eTX_METHOD_ON_EVENT) ||
             (c_MessageIter->u32_TimeoutMs != C_PuiSdUtil::h_GetMessageAutoTimeoutTime(c_MessageIter->u32_CycleTimeMs)))
         {
            // Using the specific value as custom value
            c_UiMessage.e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_CUSTOM;
         }
         else
         {
            // In case of a cyclic message the timeout time matches the auto value
            c_UiMessage.e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO;
         }
      }

      if (opc_RxSignalDefaultMinMaxValuesUsed != NULL)
      {
         pu8_DefaultMinMaxValuesUsed = &(*opc_RxSignalDefaultMinMaxValuesUsed)[u32_SignalCounter];
      }

      mh_FillUpUiStructureForSignals(*c_MessageIter, false, c_DefaultUiSignal, orc_DataPoolListStructure,
                                     pu8_DefaultMinMaxValuesUsed);
      u32_SignalCounter += c_MessageIter->c_Signals.size();
      orc_DataPoolListStructure.c_Ui.c_UiRxMessageData.push_back(c_UiMessage);
   }
   // Tx messages
   u32_SignalCounter = 0U;
   pu8_DefaultMinMaxValuesUsed = NULL;
   orc_DataPoolListStructure.c_Ui.c_UiTxMessageData.reserve(orc_DataPoolListStructure.c_Core.c_OscTxMessageData.size());
   for (c_MessageIter = orc_DataPoolListStructure.c_Core.c_OscTxMessageData.begin();
        c_MessageIter != orc_DataPoolListStructure.c_Core.c_OscTxMessageData.end();
        ++c_MessageIter)
   {
      C_PuiSdNodeCanMessage c_UiMessage;
      c_UiMessage.c_Signals.resize(c_MessageIter->c_Signals.size());

      // Adaption of Ui specific timeout mode
      if ((c_MessageIter->e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254) ||
          (c_MessageIter->e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255))
      {
         if (c_MessageIter->u32_TimeoutMs == 0U)
         {
            // Special case CANopen: 0 means disabled timeout check for on event and the Tx message has the information
            // about receiving timeout too
            c_UiMessage.e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_DISABLED;
         }
         else
         {
            // Second special case CANopen:
            c_UiMessage.e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_CUSTOM;
         }
      }

      if (opc_TxSignalDefaultMinMaxValuesUsed != NULL)
      {
         pu8_DefaultMinMaxValuesUsed = &(*opc_TxSignalDefaultMinMaxValuesUsed)[u32_SignalCounter];
      }

      mh_FillUpUiStructureForSignals(*c_MessageIter, true, c_DefaultUiSignal, orc_DataPoolListStructure,
                                     pu8_DefaultMinMaxValuesUsed);
      u32_SignalCounter += c_MessageIter->c_Signals.size();
      orc_DataPoolListStructure.c_Ui.c_UiTxMessageData.push_back(c_UiMessage);
   }

   tgl_assert(orc_DataPoolListStructure.c_Ui.c_UiRxSignalData.size() ==
              orc_DataPoolListStructure.c_Core.c_OscRxSignalData.size());
   tgl_assert(orc_DataPoolListStructure.c_Ui.c_UiTxSignalData.size() ==
              orc_DataPoolListStructure.c_Core.c_OscTxSignalData.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Internal adapter function to fill up data for Ui functionality for signals.

   Handles adaption of Ui specific auto min/max setting (multiplexed signal only)

   \param[in]      orc_Message                        Message that holds the signals
   \param[in]      orc_DefaultUiSig                   Default Ui signal
   \param[in,out]  orc_DataPoolListStructure          Data pool list structure
   \param[in]      oq_TxMessage                       Tx message
   \param[in]      opu8_SignalDefaultMinMaxValuesUsed Optional flags if imported core signal data uses the default
                                                      min max values or or specific set values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDataPoolListAdapter::mh_FillUpUiStructureForSignals(const C_OscCanMessage & orc_Message,
                                                              const bool oq_TxMessage,
                                                              const C_PuiSdNodeDataPoolListElement & orc_DefaultUiSig,
                                                              C_CieDataPoolListStructure & orc_DataPoolListStructure,
                                                              const uint8_t * const opu8_SignalDefaultMinMaxValuesUsed)
{
   C_PuiSdNodeDataPoolListElement c_AdaptedUiSignal;
   uint32_t u32_SignalCounter;

   for (u32_SignalCounter = 0U; u32_SignalCounter < orc_Message.c_Signals.size(); ++u32_SignalCounter)
   {
      c_AdaptedUiSignal = orc_DefaultUiSig;
      if (orc_Message.c_Signals[u32_SignalCounter].e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         c_AdaptedUiSignal.q_AutoMinMaxActive = true;
      }
      else if (opu8_SignalDefaultMinMaxValuesUsed != NULL)
      {
         c_AdaptedUiSignal.q_AutoMinMaxActive =
            (opu8_SignalDefaultMinMaxValuesUsed[u32_SignalCounter] == 1U) ? true : false;
      }
      else
      {
         // Nothing to do
      }

      if (oq_TxMessage == true)
      {
         orc_DataPoolListStructure.c_Ui.c_UiTxSignalData.push_back(c_AdaptedUiSignal);
      }
      else
      {
         orc_DataPoolListStructure.c_Ui.c_UiRxSignalData.push_back(c_AdaptedUiSignal);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert openSYDE CAN message data structure to DBC data structure for a single message

   \param[in]   ou32_BusIndex       bus index
   \param[in]   oe_Type             CAN protocol type (e.g. Layer 2, ECeS, ECoS)
   \param[in]   orc_OscCanMessage   openSYDE CAN message structure
   \param[out]  orc_CieNodeMessage  CAN message structure of DBC import/export
   \param[out]  orc_Warnings        message container to give user feedback

   \return
   C_NO_ERR    successful
   C_WARN      message(s) without (correct) signals, see message container with warnings
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieDataPoolListAdapter::h_ConvertToDbcImportMessage(const uint32_t ou32_BusIndex,
                                                              const C_OscCanProtocol::E_Type oe_Type,
                                                              const C_OscCanMessage & orc_OscCanMessage,
                                                              C_CieConverter::C_CieNodeMessage & orc_CieNodeMessage,
                                                              C_SclStringList & orc_Warnings)
{
   int32_t s32_Return = C_NO_ERR;

   // fill up message data
   orc_CieNodeMessage.c_CanMessage.c_Name = orc_OscCanMessage.c_Name;
   orc_CieNodeMessage.c_CanMessage.c_Comment = orc_OscCanMessage.c_Comment;
   orc_CieNodeMessage.c_CanMessage.e_TxMethod = orc_OscCanMessage.e_TxMethod;
   orc_CieNodeMessage.c_CanMessage.q_IsExtended = orc_OscCanMessage.q_IsExtended;
   orc_CieNodeMessage.c_CanMessage.u16_Dlc = orc_OscCanMessage.u16_Dlc;
   orc_CieNodeMessage.c_CanMessage.u32_CanId = orc_OscCanMessage.u32_CanId;
   orc_CieNodeMessage.c_CanMessage.u32_CycleTimeMs = orc_OscCanMessage.u32_CycleTimeMs;

   // to get the data pool list element for each signal we need the message sync manager
   C_PuiSdNodeCanMessageSyncManager c_MessageSyncManager;
   c_MessageSyncManager.Init(ou32_BusIndex, oe_Type);
   C_OscCanMessageIdentificationIndices c_MessageId;
   if (c_MessageSyncManager.GetMessageIdForMessageName(orc_OscCanMessage.c_Name.c_str(), c_MessageId) == C_NO_ERR)
   {
      // get Signals
      for (uint32_t u32_PosSignal = 0; u32_PosSignal < orc_OscCanMessage.c_Signals.size(); u32_PosSignal++)
      {
         C_CieConverter::C_CieCanSignal c_CurrentCieSignal;
         const C_OscCanSignal & rc_CurrentOscCanSignal = orc_OscCanMessage.c_Signals[u32_PosSignal];

         // fill up data of signal
         // (u32_ComDataElementIndex is not used in DBC files)
         c_CurrentCieSignal.u16_ComBitLength = rc_CurrentOscCanSignal.u16_ComBitLength;
         c_CurrentCieSignal.u16_ComBitStart = rc_CurrentOscCanSignal.u16_ComBitStart;
         c_CurrentCieSignal.e_ComByteOrder = rc_CurrentOscCanSignal.e_ComByteOrder;
         c_CurrentCieSignal.e_MultiplexerType = rc_CurrentOscCanSignal.e_MultiplexerType;
         c_CurrentCieSignal.u16_MultiplexValue = rc_CurrentOscCanSignal.u16_MultiplexValue;
         c_CurrentCieSignal.u32_J1939Spn = rc_CurrentOscCanSignal.u32_J1939SuspectParameterNumber;

         // fill up data pool list elements of signal
         const C_OscNodeDataPoolListElement * const pc_OscElement = C_PuiSdHandler::h_GetInstance()->
                                                                    GetOscCanDataPoolListElement(c_MessageId,
                                                                                                 u32_PosSignal);
         if (pc_OscElement != NULL)
         {
            C_CieConverter::C_CieDataPoolElement & rc_CieElement = c_CurrentCieSignal.c_Element;
            rc_CieElement.c_Comment = pc_OscElement->c_Comment;
            rc_CieElement.c_MaxValue.SetType(pc_OscElement->GetType());
            rc_CieElement.c_MaxValue = pc_OscElement->c_MaxValue;
            rc_CieElement.c_MinValue.SetType(pc_OscElement->GetType());
            rc_CieElement.c_MinValue = pc_OscElement->c_MinValue;
            rc_CieElement.c_Name = pc_OscElement->c_Name;
            rc_CieElement.c_Unit = pc_OscElement->c_Unit;
            rc_CieElement.f64_Factor = pc_OscElement->f64_Factor;
            rc_CieElement.f64_Offset = pc_OscElement->f64_Offset;
            // set initial value which is the first and only element of c_DataSetValues
            rc_CieElement.c_DataSetValues.push_back(pc_OscElement->c_DataSetValues.at(0));

            // store signal data
            orc_CieNodeMessage.c_CanMessage.c_Signals.push_back(c_CurrentCieSignal);
         }
         else
         {
            const C_SclString c_Message = "Can't find data elements for signal with position \"" +
                                          C_SclString::IntToStr(u32_PosSignal) + "\" in message \"" +
                                          orc_OscCanMessage.c_Name + "\" in bus \"" +
                                          C_SclString::IntToStr(ou32_BusIndex) + "\".";
            orc_Warnings.Append(c_Message);
            osc_write_log_warning("DBC Export", c_Message);
            s32_Return = C_WARN;
         }
      }
   }
   else
   {
      const C_SclString c_Message = "Can't find valid signal for message \"" + orc_OscCanMessage.c_Name +
                                    "\" in bus \"" + C_SclString::IntToStr(ou32_BusIndex) + "\".";
      orc_Warnings.Append(c_Message);
      osc_write_log_warning("DBC Export", c_Message);
      s32_Return = C_WARN;
   }

   return s32_Return;
}
