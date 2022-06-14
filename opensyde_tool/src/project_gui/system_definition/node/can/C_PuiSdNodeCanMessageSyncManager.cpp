//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to initially match all signals for one bus & then keep them sync for operations (implementation)

   Class to initially match all signals for one bus & then keep them sync for operations:
   Set message -> sync
   Delete message -> sync & update message indices
   Any signal operation -> sync & update signal indices
   Add message -> sync & update message indices

   One instance PER bus.

   Usage:
   Any change should be forwarded to this class.
   This class then does:
   1. Search for the message in its member vectors
   2. Do action for all matching messages (Second level vector)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <iostream>

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"
#include "C_PuiSdHandler.h"
#include "C_OSCUtils.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNodeCanMessageSyncManager::C_PuiSdNodeCanMessageSyncManager(QObject * const opc_Parent) :
   QObject(opc_Parent),
   mq_SingleNodeMode(false),
   me_Protocol(C_OSCCanProtocol::eLAYER2)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init message matches

   \param[in]  oru32_BusIndex    Bus index
   \param[in]  ore_ComProtocol   Protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::Init(const uint32 & oru32_BusIndex,
                                            const C_OSCCanProtocol::E_Type & ore_ComProtocol)
{
   //Protocol
   this->me_Protocol = ore_ComProtocol;

   //Mode
   this->mq_SingleNodeMode = false;

   //Clear
   this->mc_MessageMatches.clear();
   this->mc_MessageMatchUniqueIds.clear();
   this->mc_CriticalMessageMatches.clear();

   //Init
   mh_Init(oru32_BusIndex, ore_ComProtocol, this->mc_MessageMatches, &this->mc_MessageMatchUniqueIds,
           &this->mc_CriticalMessageMatches);

   //Other protocols
   this->mc_OtherProtocols.clear();
   this->mc_MessageMatchesForOtherProtocols.clear();
   this->mc_MessageMatchesForOtherProtocols.resize(C_OSCCanProtocol::hc_ALL_PROTOCOLS.size() - 1);
   for (uint32 u32_ItProt = 0UL; u32_ItProt < C_OSCCanProtocol::hc_ALL_PROTOCOLS.size(); ++u32_ItProt)
   {
      if (ore_ComProtocol != C_OSCCanProtocol::hc_ALL_PROTOCOLS[u32_ItProt])
      {
         this->mc_OtherProtocols.push_back(C_OSCCanProtocol::hc_ALL_PROTOCOLS[u32_ItProt]);
      }
   }
   tgl_assert(this->mc_OtherProtocols.size() == this->mc_MessageMatchesForOtherProtocols.size());
   if (this->mc_OtherProtocols.size() == this->mc_MessageMatchesForOtherProtocols.size())
   {
      for (uint32 u32_ItProt = 0UL; u32_ItProt < this->mc_OtherProtocols.size(); ++u32_ItProt)
      {
         mh_Init(oru32_BusIndex, this->mc_OtherProtocols[u32_ItProt],
                 this->mc_MessageMatchesForOtherProtocols[u32_ItProt],
                 NULL, NULL);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init message matches

   \param[in]  oru32_NodeIndex         Node index
   \param[in]  oru32_InterfaceIndex    Interface index
   \param[in]  ore_ComProtocol         Protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::Init(const uint32 & oru32_NodeIndex, const uint32 & oru32_InterfaceIndex,
                                            const C_OSCCanProtocol::E_Type & ore_ComProtocol)
{
   //Protocol
   this->me_Protocol = ore_ComProtocol;

   //Mode
   this->mq_SingleNodeMode = true;

   //Clear
   this->mc_MessageMatches.clear();
   this->mc_MessageMatchUniqueIds.clear();
   this->mc_CriticalMessageMatches.clear();

   //Init
   mh_Init(oru32_NodeIndex, oru32_InterfaceIndex, ore_ComProtocol, this->mc_MessageMatches,
           &this->mc_MessageMatchUniqueIds,
           &this->mc_CriticalMessageMatches);

   //Other protocols
   this->mc_OtherProtocols.clear();
   this->mc_MessageMatchesForOtherProtocols.clear();
   this->mc_MessageMatchesForOtherProtocols.resize(C_OSCCanProtocol::hc_ALL_PROTOCOLS.size() - 1);
   for (uint32 u32_ItProt = 0UL; u32_ItProt < C_OSCCanProtocol::hc_ALL_PROTOCOLS.size(); ++u32_ItProt)
   {
      if (ore_ComProtocol != C_OSCCanProtocol::hc_ALL_PROTOCOLS[u32_ItProt])
      {
         this->mc_OtherProtocols.push_back(C_OSCCanProtocol::hc_ALL_PROTOCOLS[u32_ItProt]);
      }
   }
   tgl_assert(this->mc_OtherProtocols.size() == this->mc_MessageMatchesForOtherProtocols.size());
   if (this->mc_OtherProtocols.size() == this->mc_MessageMatchesForOtherProtocols.size())
   {
      for (uint32 u32_ItProt = 0UL; u32_ItProt < this->mc_OtherProtocols.size(); ++u32_ItProt)
      {
         mh_Init(oru32_NodeIndex, oru32_InterfaceIndex, this->mc_OtherProtocols[u32_ItProt],
                 this->mc_MessageMatchesForOtherProtocols[u32_ItProt], NULL, NULL);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get vector of different, unique message ids

   \return
   Different, unique message ids
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCCanMessageIdentificationIndices> C_PuiSdNodeCanMessageSyncManager::GetUniqueMessages(void) const
{
   return mh_GetUniqueMessages(this->mc_MessageMatches);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of the unique messages of a protocol

   \param[in]   oe_ComProtocol      Protocol type
   \param[out]  opu32_SignalCount   Number of unique signals of all unique messages (optional)

   \return
   Number of messages of the protocol
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdNodeCanMessageSyncManager::GetUniqueMessageCount(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                                               stw_types::uint32 * const opu32_SignalCount) const
{
   uint32 u32_Count = 0U;

   const std::vector<std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> > * pc_UniqueMessages;

   if (oe_ComProtocol == this->me_Protocol)
   {
      pc_UniqueMessages = &this->mc_MessageMatches;
   }
   else
   {
      uint32 u32_ProtocolIndex = 0U;

      for (uint32 u32_ItProt = 0UL; u32_ItProt < this->mc_OtherProtocols.size(); ++u32_ItProt)
      {
         if (this->mc_OtherProtocols[u32_ItProt] == oe_ComProtocol)
         {
            u32_ProtocolIndex = u32_ItProt;
         }
      }

      pc_UniqueMessages = &this->mc_MessageMatchesForOtherProtocols[u32_ProtocolIndex];
   }

   if (opu32_SignalCount == NULL)
   {
      u32_Count = mh_GetUniqueMessages(*pc_UniqueMessages).size();
   }
   else
   {
      std::vector<C_OSCCanMessageIdentificationIndices> c_UniqueMsgIds = mh_GetUniqueMessages(*pc_UniqueMessages);
      uint32 u32_MsgCounter;

      // Message count
      u32_Count = c_UniqueMsgIds.size();

      // Signal count
      *opu32_SignalCount = 0;
      for (u32_MsgCounter = 0U; u32_MsgCounter < u32_Count; ++u32_MsgCounter)
      {
         const C_OSCCanMessage * const pc_Message =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_UniqueMsgIds[u32_MsgCounter]);

         if (pc_Message != NULL)
         {
            *opu32_SignalCount += pc_Message->c_Signals.size();
         }
      }
   }

   return u32_Count;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current set protocol type

   \return
   Current protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanProtocol::E_Type C_PuiSdNodeCanMessageSyncManager::GetCurrentComProtocol(void) const
{
   return this->me_Protocol;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all node indexes which are connected to the bus and use the active protocol

   \param[in]   ou32_BusIndex          Bus index
   \param[in]   ore_ComProtocol        Active protocol
   \param[out]  orc_NodeIndexes        Vector with all node ids which are connected to the bus
   \param[out]  orc_InterfaceIndexes   Vector with all node interface ids which are connected to the bus
   \param[out]  orc_DatapoolIndexes    Vector with all Datapool indexes which are connected to the bus and associated
                                       to the protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::h_GetConnectedAndActiveInterfaces(const uint32 ou32_BusIndex,
                                                                         const C_OSCCanProtocol::E_Type & ore_ComProtocol, std::vector<uint32> & orc_NodeIndexes, std::vector<uint32> & orc_InterfaceIndexes,
                                                                         std::vector<uint32> & orc_DatapoolIndexes)
{
   //Init selector widget
   orc_NodeIndexes.clear();
   orc_InterfaceIndexes.clear();
   orc_DatapoolIndexes.clear();

   C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeAndComDpIndexesOfBus(ou32_BusIndex,
                                                                                              ore_ComProtocol,
                                                                                              orc_NodeIndexes,
                                                                                              orc_InterfaceIndexes,
                                                                                              orc_DatapoolIndexes);

   //Remove all not connected
   if ((orc_NodeIndexes.size() == orc_InterfaceIndexes.size()) &&
       (orc_NodeIndexes.size() == orc_DatapoolIndexes.size()))
   {
      //lint -e{850} Index modified for erase index step
      for (uint32 u32_ItNode = 0; u32_ItNode < orc_NodeIndexes.size(); ++u32_ItNode)
      {
         const C_OSCCanMessageContainer * const pc_Container =
            C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_NodeIndexes[u32_ItNode],
                                                                            ore_ComProtocol,
                                                                            orc_InterfaceIndexes[u32_ItNode],
                                                                            orc_DatapoolIndexes[u32_ItNode]);
         if (pc_Container != NULL)
         {
            if (pc_Container->q_IsComProtocolUsedByInterface == false)
            {
               orc_NodeIndexes.erase(orc_NodeIndexes.begin() + u32_ItNode);
               orc_InterfaceIndexes.erase(orc_InterfaceIndexes.begin() + u32_ItNode);
               orc_DatapoolIndexes.erase(orc_DatapoolIndexes.begin() + u32_ItNode);
               //Same index is new node/interface
               --u32_ItNode;
            }
         }
         else
         {
            //Not even a data pool available
            orc_NodeIndexes.erase(orc_NodeIndexes.begin() + u32_ItNode);
            orc_InterfaceIndexes.erase(orc_InterfaceIndexes.begin() + u32_ItNode);
            orc_DatapoolIndexes.erase(orc_DatapoolIndexes.begin() + u32_ItNode);
            //Same index is new node/interface
            --u32_ItNode;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can message

   * Do only change message direction using this function.

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  orq_NewMessageIsTx   Flag if message direction is now transmit

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::SetCanMessageDirection(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const bool & orq_NewMessageIsTx)
{
   sint32 s32_Retval = C_RANGE;
   const C_OSCCanMessage * const pc_CanMessage = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
   const uint32 u32_ItDifferentMessage = m_GetMatchingMessageVectorIndex(orc_MessageId);

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   if ((pc_CanMessage != NULL) && (u32_ItDifferentMessage < this->mc_MessageMatches.size()))
   {
      //save data
      //copy current message
      const C_OSCCanMessage c_MessageData = *pc_CanMessage;
      std::vector<C_OSCCanMessageIdentificationIndices> & rc_MatchingMessageIds =
         this->mc_MessageMatches[u32_ItDifferentMessage];

      for (uint32 u32_ItMessageId = 0; u32_ItMessageId < rc_MatchingMessageIds.size();
           ++u32_ItMessageId)
      {
         C_OSCCanMessageIdentificationIndices & rc_MatchingMessage = rc_MatchingMessageIds[u32_ItMessageId];

         //Probably helps if only the matching message ID is adapted to the new type....
         if ((rc_MatchingMessage.u32_NodeIndex == orc_MessageId.u32_NodeIndex) &&
             (rc_MatchingMessage.u32_InterfaceIndex == orc_MessageId.u32_InterfaceIndex) &&
             (rc_MatchingMessage.u32_DatapoolIndex == orc_MessageId.u32_DatapoolIndex))
         {
            s32_Retval = C_PuiSdHandler::h_GetInstance()->SetCanMessageProperties(rc_MatchingMessage,
                                                                                  orq_NewMessageIsTx, c_MessageData);
            //Update message id
            if (s32_Retval == C_NO_ERR)
            {
               const C_OSCCanMessageContainer * const pc_MatchingMessageContainer =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(rc_MatchingMessage.u32_NodeIndex,
                                                                                  rc_MatchingMessage.e_ComProtocol,
                                                                                  rc_MatchingMessage.u32_InterfaceIndex,
                                                                                  rc_MatchingMessage.u32_DatapoolIndex);
               if (pc_MatchingMessageContainer != NULL)
               {
                  const std::vector<C_OSCCanMessage> & rc_Messages = pc_MatchingMessageContainer->GetMessagesConst(
                     orq_NewMessageIsTx);
                  rc_MatchingMessage.q_MessageIsTx = orq_NewMessageIsTx;
                  rc_MatchingMessage.u32_MessageIndex = static_cast<uint32>(rc_Messages.size()) - 1UL;
               }
            }
            //There should be only one entry with this node index and interface index
            //-> this should also just affect one item
            break;
         }
      }
      //Resync matching node & com protocol & not new messageistx flag because of array size change
      m_UpdateIndicesToCanMessageDirectionChange(orc_MessageId);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can message

   * Do not change the signal size using this function.
   * Do not change message direction using this function.
   * Do not change message receive timeout using this function.

   \param[in]  orc_MessageId  Message identification indices
   \param[in]  orc_Message    Message data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const C_OSCCanMessage & orc_Message) const
{
   sint32 s32_Retval = C_RANGE;
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);
   C_OSCCanMessage c_Copy = orc_Message;

   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      const C_OSCCanMessageIdentificationIndices & rc_CurMessageId = c_MatchingMessageIds[u32_ItMessageId];
      const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(rc_CurMessageId);
      if (pc_Message != NULL)
      {
         //Keep the original timeout value
         c_Copy.u32_TimeoutMs = pc_Message->u32_TimeoutMs;
         s32_Retval = C_PuiSdHandler::h_GetInstance()->SetCanMessageProperties(rc_CurMessageId,
                                                                               rc_CurMessageId.q_MessageIsTx,
                                                                               c_Copy);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      //Stop on error
      if (s32_Retval != C_NO_ERR)
      {
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can message receive timeout

   \param[in]  orc_MessageId        Message identification indices -> message
   \param[in]  ou32_NodeIndex       Node index -> which receiver
   \param[in]  ou32_InterfaceIndex  Interface index -> which receiver
   \param[in]  ou32_DatapoolIndex   Datapool index -> which receiver
   \param[in]  ou32_ReceiveTimeout  Receive timeout

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::SetCanMessageReceiveTimeout(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const uint32 ou32_NodeIndex,
   const uint32 ou32_InterfaceIndex, const uint32 ou32_DatapoolIndex, const uint32 ou32_ReceiveTimeout) const
{
   sint32 s32_Retval = C_RANGE;
   //Get all matching Ids
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   //Look for specific receiver
   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      const C_OSCCanMessageIdentificationIndices & rc_CurMessageId = c_MatchingMessageIds[u32_ItMessageId];
      if ((rc_CurMessageId.u32_NodeIndex == ou32_NodeIndex) &&
          (rc_CurMessageId.u32_InterfaceIndex == ou32_InterfaceIndex) &&
          (rc_CurMessageId.u32_DatapoolIndex == ou32_DatapoolIndex))
      {
         const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(rc_CurMessageId);

         if (pc_Message != NULL)
         {
            C_OSCCanMessage c_Copy = *pc_Message;
            //Only set timeout value
            c_Copy.u32_TimeoutMs = ou32_ReceiveTimeout;
            s32_Retval = C_PuiSdHandler::h_GetInstance()->SetCanMessageProperties(rc_CurMessageId,
                                                                                  rc_CurMessageId.q_MessageIsTx,
                                                                                  c_Copy);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         //Stop on error
         if (s32_Retval != C_NO_ERR)
         {
            break;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can message auto receive timeout flag

   \param[in]  orc_MessageId           Message identification indices -> message
   \param[in]  ou32_NodeIndex          Node index -> which receiver
   \param[in]  ou32_InterfaceIndex     Interface index -> which receiver
   \param[in]  ou32_DatapoolIndex      Datapool index -> which receiver
   \param[in]  ore_ReceiveTimeoutMode  Receive timeout mode

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::SetCanMessageReceiveTimeoutAutoFlag(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const uint32 ou32_NodeIndex,
   const uint32 ou32_InterfaceIndex, const uint32 ou32_DatapoolIndex,
   const C_PuiSdNodeCanMessage::E_RxTimeoutMode & ore_ReceiveTimeoutMode) const
{
   sint32 s32_Retval = C_RANGE;
   //Get all matching Ids
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   //Look for specific receiver
   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      const C_OSCCanMessageIdentificationIndices & rc_CurMessageId = c_MatchingMessageIds[u32_ItMessageId];
      if ((rc_CurMessageId.u32_NodeIndex == ou32_NodeIndex) &&
          (rc_CurMessageId.u32_InterfaceIndex == ou32_InterfaceIndex) &&
          (rc_CurMessageId.u32_DatapoolIndex == ou32_DatapoolIndex))
      {
         const C_PuiSdNodeCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetUiCanMessage(
            rc_CurMessageId);

         if (pc_Message != NULL)
         {
            C_PuiSdNodeCanMessage c_Copy = *pc_Message;
            //Only set timeout flag
            c_Copy.e_ReceiveTimeoutMode = ore_ReceiveTimeoutMode;
            s32_Retval = C_PuiSdHandler::h_GetInstance()->SetUiCanMessage(rc_CurMessageId,
                                                                          c_Copy);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         //Stop on error
         if (s32_Retval != C_NO_ERR)
         {
            break;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  orc_OSCSignal        Signal data (osc)
   \param[in]  orc_OSCSignalCommon  Signal data (osc common)
   \param[in]  orc_UISignalCommon   Signal data (ui common)
   \param[in]  orc_UISignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::SetCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                      const uint32 & oru32_SignalIndex,
                                                      const C_OSCCanSignal & orc_OSCSignal,
                                                      const C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                                      const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                                      const C_PuiSdNodeCanSignal & orc_UISignal) const
{
   sint32 s32_Retval = C_RANGE;
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      s32_Retval = C_PuiSdHandler::h_GetInstance()->SetCanSignal(c_MatchingMessageIds[u32_ItMessageId],
                                                                 oru32_SignalIndex, orc_OSCSignal,
                                                                 orc_OSCSignalCommon, orc_UISignalCommon, orc_UISignal);
      //Stop on error
      if (s32_Retval != C_NO_ERR)
      {
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can signal common part

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  ou16_MultiplexValue  New multiplex value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::SetCanSignalMUXValue(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const uint32 & oru32_SignalIndex,
   const stw_types::uint16 ou16_MultiplexValue)
const
{
   sint32 s32_Retval = C_RANGE;
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      s32_Retval = C_PuiSdHandler::h_GetInstance()->SetCanSignalMUXValue(c_MatchingMessageIds[u32_ItMessageId],
                                                                         oru32_SignalIndex, ou16_MultiplexValue);
      //Stop on error
      if (s32_Retval != C_NO_ERR)
      {
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can signal position part

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  orc_OSCSignal        Signal data (osc)
   \param[in]  orc_UISignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::SetCanSignalPosition(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const uint32 & oru32_SignalIndex,
   const C_OSCCanSignal & orc_OSCSignal, const C_PuiSdNodeCanSignal & orc_UISignal) const
{
   sint32 s32_Retval = C_RANGE;
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      s32_Retval = C_PuiSdHandler::h_GetInstance()->SetCanSignalPosition(c_MatchingMessageIds[u32_ItMessageId],
                                                                         oru32_SignalIndex, orc_OSCSignal,
                                                                         orc_UISignal);
      //Stop on error
      if (s32_Retval != C_NO_ERR)
      {
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add can message

   \param[in]   oru32_NodeIndex        Node index
   \param[in]   ore_ComType            Can protocol type
   \param[in]   oru32_InterfaceIndex   Node interface index
   \param[in]   oru32_DatapoolIndex    Datapool index
   \param[in]   orq_MessageIsTx        Flag if message is tx type (else rx type assumed)
   \param[in]   orc_Message            Message data
   \param[in]   orc_OSCSignalCommons   Signals data (osc common)
   \param[in]   orc_UISignalCommons    Signals data (ui common)
   \param[in]   orc_UISignals          Signals data (ui)
   \param[out]  oru32_MessageIndex     New message index, only valid if C_NO_ERR

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::AddCanMessage(const uint32 & oru32_NodeIndex,
                                                       const C_OSCCanProtocol::E_Type & ore_ComType,
                                                       const uint32 & oru32_InterfaceIndex,
                                                       const uint32 & oru32_DatapoolIndex, const bool & orq_MessageIsTx,
                                                       const C_OSCCanMessage & orc_Message,
                                                       const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals,
                                                       uint32 & oru32_MessageIndex)
{
   sint32 s32_Retval = C_RANGE;
   const C_OSCCanMessageContainer * const pc_MessageContainer =
      C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(oru32_NodeIndex, ore_ComType,
                                                                      oru32_InterfaceIndex, oru32_DatapoolIndex);

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   if (pc_MessageContainer != NULL)
   {
      const std::vector<C_OSCCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(orq_MessageIsTx);
      const C_OSCCanMessageIdentificationIndices c_MessageId(oru32_NodeIndex, ore_ComType, oru32_InterfaceIndex,
                                                             oru32_DatapoolIndex, orq_MessageIsTx, rc_Messages.size());
      C_PuiSdNodeCanMessage c_UIMessage;

      c_UIMessage.c_Signals = orc_UISignals;
      s32_Retval = this->InsertCanMessage(c_MessageId, orc_Message,
                                          orc_OSCSignalCommons,
                                          orc_UISignalCommons, c_UIMessage);
      if (s32_Retval == C_NO_ERR)
      {
         oru32_MessageIndex = static_cast<uint32>(rc_Messages.size()) - 1UL;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert can message

   \param[in]  orc_MessageId           Message identification indices
   \param[in]  orc_Message             Message data
   \param[in]  orc_OSCSignalCommons    Signals data (osc common)
   \param[in]  orc_UISignalCommons     Signals data (ui common)
   \param[in]  orc_UIMessage           Message data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::InsertCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                          const C_OSCCanMessage & orc_Message,
                                                          const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                                          const C_PuiSdNodeCanMessage & orc_UIMessage)
{
   const sint32 s32_Retval = C_PuiSdHandler::h_GetInstance()->InsertCanMessage(orc_MessageId, orc_Message,
                                                                               orc_OSCSignalCommons,
                                                                               orc_UISignalCommons, orc_UIMessage,
                                                                               true);

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   m_UpdateIndicesToNewCanMessage(orc_MessageId);
   this->m_RegisterIfNecessary(orc_MessageId);
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete can message

   \param[in]  orc_MessageId  Message identification indices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::DeleteCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   sint32 s32_Retval = C_RANGE;
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      s32_Retval = m_DeleteCanMessage(c_MatchingMessageIds[u32_ItMessageId], true);
      //Stop on error
      if (s32_Retval != C_NO_ERR)
      {
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change can message transmitter

   \param[in]  orc_MessageId           Message identification indices
   \param[in]  oru32_NodeIndex         New transmitter node index
   \param[in]  oru32_InterfaceIndex    New transmitter node interface index
   \param[in]  ou32_DatapoolIndex      New transmitter node Datapool index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::ChangeCanMessageTx(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                            const uint32 & oru32_NodeIndex,
                                                            const uint32 & oru32_InterfaceIndex,
                                                            const uint32 ou32_DatapoolIndex)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCCanMessageContainer * const pc_MessageContainer =
      C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(oru32_NodeIndex,
                                                                      orc_MessageId.e_ComProtocol,
                                                                      oru32_InterfaceIndex,
                                                                      ou32_DatapoolIndex);

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   if (pc_MessageContainer != NULL)
   {
      const std::vector<C_OSCCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(true);
      const uint32 u32_MessageIndex = rc_Messages.size();
      const C_OSCCanMessageIdentificationIndices c_NewId(oru32_NodeIndex, orc_MessageId.e_ComProtocol,
                                                         oru32_InterfaceIndex, ou32_DatapoolIndex, true,
                                                         u32_MessageIndex);
      const uint32 u32_MatchingMessageIdsIndex = this->m_GetMatchingMessageVectorIndex(orc_MessageId);
      C_OSCCanMessage c_Message;

      std::vector<C_OSCNodeDataPoolListElement> c_OSCSignalCommons;
      std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommons;
      C_PuiSdNodeCanMessage c_UIMessage;
      std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds;

      //Get matching message ids
      if (u32_MatchingMessageIdsIndex < this->mc_MessageMatches.size())
      {
         c_MatchingMessageIds = this->mc_MessageMatches[u32_MatchingMessageIdsIndex];
      }
      else
      {
         c_MatchingMessageIds.push_back(orc_MessageId);
      }

      //Get complete message
      tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanMessageComplete(orc_MessageId, c_Message, c_OSCSignalCommons,
                                                                        c_UISignalCommons,
                                                                        c_UIMessage.c_Signals) == C_NO_ERR);
      //Add without register
      tgl_assert(C_PuiSdHandler::h_GetInstance()->InsertCanMessage(c_NewId, c_Message, c_OSCSignalCommons,
                                                                   c_UISignalCommons,
                                                                   c_UIMessage, false) == C_NO_ERR);
      //Delete existing Tx and matching Rx
      //lint -e{850} Index modified for erase index step
      for (uint32 u32_ItMessageId = 0; (u32_ItMessageId < c_MatchingMessageIds.size()) && (s32_Retval == C_NO_ERR);
           ++u32_ItMessageId)
      {
         bool q_Deleted = false;
         const C_OSCCanMessageIdentificationIndices & rc_MessageId = c_MatchingMessageIds[u32_ItMessageId];
         //Other Tx
         if (rc_MessageId.q_MessageIsTx == true)
         {
            s32_Retval = m_DeleteCanMessage(rc_MessageId, false);
            q_Deleted = true;
         }
         //Matching Rx
         // All Rx variants of the node, interface and protocol with all Datapools must be removed
         else if ((rc_MessageId.u32_NodeIndex == c_NewId.u32_NodeIndex) &&
                  (rc_MessageId.u32_InterfaceIndex == c_NewId.u32_InterfaceIndex) &&
                  (rc_MessageId.e_ComProtocol == c_NewId.e_ComProtocol))
         {
            s32_Retval = m_DeleteCanMessage(rc_MessageId, false);
            q_Deleted = true;
         }
         else
         {
            //No handling
         }
         if ((q_Deleted == true) && (s32_Retval == C_NO_ERR))
         {
            //Delete from ids
            c_MatchingMessageIds.erase(c_MatchingMessageIds.begin() + u32_ItMessageId);
            //Revert iterator step (new item at current position)
            --u32_ItMessageId;
         }
         tgl_assert(s32_Retval == C_NO_ERR);
      }
      //Append new transmitter
      c_MatchingMessageIds.push_back(c_NewId);
      //Update matching messages
      if (u32_MatchingMessageIdsIndex < this->mc_MessageMatches.size())
      {
         this->mc_MessageMatches[u32_MatchingMessageIdsIndex] = c_MatchingMessageIds;
      }
      else
      {
         this->mc_MessageMatches.push_back(c_MatchingMessageIds);
         this->mc_MessageMatchUniqueIds.push_back(this->m_GetNewUniqueId());
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add can message receiver

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  ou32_NodeIndex       New receiver node index
   \param[in]  ou32_InterfaceIndex  New receiver node interface index
   \param[in]  ou32_DatapoolIndex   Node Datapool index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::AddCanMessageRx(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                         const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                         const uint32 ou32_DatapoolIndex)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCCanMessageContainer * const pc_MessageContainer =
      C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(ou32_NodeIndex,
                                                                      orc_MessageId.e_ComProtocol,
                                                                      ou32_InterfaceIndex,
                                                                      ou32_DatapoolIndex);

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   if (pc_MessageContainer != NULL)
   {
      const std::vector<C_OSCCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(false);
      const uint32 u32_MessageIndex = rc_Messages.size();
      const C_OSCCanMessageIdentificationIndices c_NewId(ou32_NodeIndex, orc_MessageId.e_ComProtocol,
                                                         ou32_InterfaceIndex, ou32_DatapoolIndex, false,
                                                         u32_MessageIndex);
      const uint32 u32_MatchingMessageIdsIndex = this->m_GetMatchingMessageVectorIndex(orc_MessageId);
      C_OSCCanMessage c_Message;

      std::vector<C_OSCNodeDataPoolListElement> c_OSCSignalCommons;
      std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommons;
      C_PuiSdNodeCanMessage c_UIMessage;

      //Get complete message
      tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanMessageComplete(orc_MessageId, c_Message, c_OSCSignalCommons,
                                                                        c_UISignalCommons,
                                                                        c_UIMessage.c_Signals) == C_NO_ERR);
      //Add without register
      tgl_assert(C_PuiSdHandler::h_GetInstance()->InsertCanMessage(c_NewId, c_Message, c_OSCSignalCommons,
                                                                   c_UISignalCommons,
                                                                   c_UIMessage, false) == C_NO_ERR);
      //Append to matching messages
      if (u32_MatchingMessageIdsIndex < this->mc_MessageMatches.size())
      {
         this->mc_MessageMatches[u32_MatchingMessageIdsIndex].push_back(c_NewId);
      }
      else
      {
         std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds;
         c_MatchingMessageIds.push_back(orc_MessageId);
         this->mc_MessageMatches.push_back(c_MatchingMessageIds);
         this->mc_MessageMatchUniqueIds.push_back(this->m_GetNewUniqueId());
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete can message receiver

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  ou32_NodeIndex       Previous receiver node index
   \param[in]  ou32_InterfaceIndex  Previous receiver node interface index
   \param[in]  ou32_DatapoolIndex   Node Datapool index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::DeleteCanMessageRx(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                            const uint32 ou32_NodeIndex,
                                                            const uint32 ou32_InterfaceIndex,
                                                            const uint32 ou32_DatapoolIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   //Get matching message ids
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   //Search for node and interface and rx
   for (uint32 u32_ItMessageId = 0; (u32_ItMessageId < c_MatchingMessageIds.size()) && (s32_Retval == C_NO_ERR);
        ++u32_ItMessageId)
   {
      const C_OSCCanMessageIdentificationIndices & rc_MessageId = c_MatchingMessageIds[u32_ItMessageId];

      //protocol does not need to be checked as the matching messages should only match if they are of the same protocol
      if ((rc_MessageId.q_MessageIsTx == false) &&
          (rc_MessageId.u32_NodeIndex == ou32_NodeIndex) &&
          (rc_MessageId.u32_InterfaceIndex == ou32_InterfaceIndex) &&
          (rc_MessageId.u32_DatapoolIndex == ou32_DatapoolIndex))
      {
         s32_Retval = m_DeleteCanMessage(rc_MessageId, true);
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add can signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  orc_Signal           Signal data (osc)
   \param[in]  orc_OSCSignalCommon  Signal data (osc common)
   \param[in]  orc_UISignalCommon   Signal data (ui common)
   \param[in]  orc_UISignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::AddCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                      const C_OSCCanSignal & orc_Signal,
                                                      const C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                                      const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                                      const C_PuiSdNodeCanSignal & orc_UISignal) const
{
   sint32 s32_Retval = C_RANGE;
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      s32_Retval = C_PuiSdHandler::h_GetInstance()->AddCanSignal(c_MatchingMessageIds[u32_ItMessageId], orc_Signal,
                                                                 orc_OSCSignalCommon,
                                                                 orc_UISignalCommon, orc_UISignal);
      //Stop on error
      if (s32_Retval != C_NO_ERR)
      {
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert can signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  orc_Signal           Signal data (osc)
   \param[in]  orc_OSCSignalCommon  Signal data (osc common)
   \param[in]  orc_UISignalCommon   Signal data (ui common)
   \param[in]  orc_UISignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::InsertCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                         const uint32 & oru32_SignalIndex,
                                                         const C_OSCCanSignal & orc_Signal,
                                                         const C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                                         const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                                         const C_PuiSdNodeCanSignal & orc_UISignal) const
{
   sint32 s32_Retval = C_RANGE;
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      s32_Retval = C_PuiSdHandler::h_GetInstance()->InsertCanSignal(c_MatchingMessageIds[u32_ItMessageId],
                                                                    oru32_SignalIndex, orc_Signal,
                                                                    orc_OSCSignalCommon, orc_UISignalCommon,
                                                                    orc_UISignal);
      //Stop on error
      if (s32_Retval != C_NO_ERR)
      {
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete can signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::DeleteCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                         const uint32 & oru32_SignalIndex) const
{
   sint32 s32_Retval = C_RANGE;
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds = this->GetMatchingMessageVector(
      orc_MessageId);

   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size();
        ++u32_ItMessageId)
   {
      s32_Retval = C_PuiSdHandler::h_GetInstance()->DeleteCanSignal(c_MatchingMessageIds[u32_ItMessageId],
                                                                    oru32_SignalIndex);
      //Stop on error
      if (s32_Retval != C_NO_ERR)
      {
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get vector of matching message ids

   \param[in]  orc_MessageId  Message identification indices to search for

   \return
   All matching message ids (including the searched message id)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCCanMessageIdentificationIndices> C_PuiSdNodeCanMessageSyncManager::GetMatchingMessageVector(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   std::vector<C_OSCCanMessageIdentificationIndices> c_Retval;
   const uint32 u32_ItDifferentMessage = m_GetMatchingMessageVectorIndex(orc_MessageId);
   //Return found vector or return message only
   if (u32_ItDifferentMessage < this->mc_MessageMatches.size())
   {
      c_Retval = this->mc_MessageMatches[u32_ItDifferentMessage];
   }
   else
   {
      c_Retval.push_back(orc_MessageId);
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Searches a matching message id with an other node and interface index

   If no other message id with an other node and interface index is available,
   orc_MessageId will not be changed.
   If an other message id was found, orc_MessageId will be set to the found matching message id

   \param[in,out]  orc_MessageId    Message id for searching and the result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::ReplaceMessageIdWithMatchingId(
   C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds =
      this->GetMatchingMessageVector(orc_MessageId);
   uint32 u32_MatchingMsgCounter;

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   for (u32_MatchingMsgCounter = 0U; u32_MatchingMsgCounter < c_MatchingMessageIds.size();
        ++u32_MatchingMsgCounter)
   {
      const C_OSCCanMessageIdentificationIndices & rc_MatchingMsgId =
         c_MatchingMessageIds[u32_MatchingMsgCounter];

      if ((orc_MessageId.u32_NodeIndex != rc_MatchingMsgId.u32_NodeIndex) ||
          (orc_MessageId.u32_InterfaceIndex != rc_MatchingMsgId.u32_InterfaceIndex))
      {
         // A matching message id with an other node or interface index. It can be used as replacement
         orc_MessageId = rc_MatchingMsgId;
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if message ID matches a critical message ID (Message with two transmitters)

   \param[in]  orc_MessageId  Message identification indices

   \return
   True  Critical message (Message with two transmitters)
   False Standard message
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdNodeCanMessageSyncManager::CheckCriticalMessageId(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
const
{
   bool q_Retval = false;

   const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessages = this->GetMatchingMessageVector(
      orc_MessageId);

   for (std::vector<C_OSCCanMessageIdentificationIndices>::const_iterator c_ItCriticalMessage =
           this->mc_CriticalMessageMatches.begin();
        c_ItCriticalMessage != this->mc_CriticalMessageMatches.end(); ++c_ItCriticalMessage)
   {
      for (std::vector<C_OSCCanMessageIdentificationIndices>::const_iterator c_ItMatchingMessage =
              c_MatchingMessages.begin(); c_ItMatchingMessage != c_MatchingMessages.end();
           ++c_ItMatchingMessage)
      {
         if ((*c_ItCriticalMessage) == (*c_ItMatchingMessage))
         {
            q_Retval = true;
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check error for bus & connected node data pools

   \param[out]  opq_MessageNameInvalid                An error type, found for a bus
   \param[out]  opq_MessageIdInvalid                  An error type, found for a bus
   \param[out]  opq_MessagesHaveNoTx                  An error type, found for a bus
   \param[out]  opq_DelayTimeInvalid                  An error type, found for a bus
   \param[out]  opq_MessageSignalInvalid              An error type, found for a bus
   \param[in]   ou32_CANMessageValidSignalsDLCOffset  CAN message DLC offset for valid signal range check
   \param[in]   oq_CANMessageSignalGapsValid          Flag if gaps between signals are valid or handled as errors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::CheckErrorBus(bool * const opq_MessageNameInvalid,
                                                     bool * const opq_MessageIdInvalid,
                                                     bool * const opq_MessagesHaveNoTx,
                                                     bool * const opq_DelayTimeInvalid,
                                                     bool * const opq_MessageSignalInvalid,
                                                     const stw_types::uint32 ou32_CANMessageValidSignalsDLCOffset,
                                                     const bool oq_CANMessageSignalGapsValid) const
{
   std::vector<C_OSCCanMessageIdentificationIndices> c_UniqueMessageIds;

   c_UniqueMessageIds = this->GetUniqueMessages();

   if ((opq_MessageNameInvalid != NULL) || (opq_MessageIdInvalid != NULL))
   {
      if (opq_MessageNameInvalid != NULL)
      {
         *opq_MessageNameInvalid = false;
      }
      if (opq_MessageIdInvalid != NULL)
      {
         *opq_MessageIdInvalid = false;
      }
      for (uint32 u32_ItUniqueMessageId = 0; u32_ItUniqueMessageId < c_UniqueMessageIds.size(); ++u32_ItUniqueMessageId)
      {
         const C_OSCCanMessage * const pc_Message =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_UniqueMessageIds[u32_ItUniqueMessageId]);
         if (pc_Message != NULL)
         {
            if (opq_MessageNameInvalid != NULL)
            {
               bool q_MessageNameValid;
               this->CheckMessageNameBus(pc_Message->c_Name, q_MessageNameValid,
                                         &c_UniqueMessageIds[u32_ItUniqueMessageId]);
               if (q_MessageNameValid == false)
               {
                  *opq_MessageNameInvalid = true;
               }
            }
            if (opq_MessageIdInvalid != NULL)
            {
               bool q_MessageIdValid;
               this->CheckMessageIdBus(C_OSCCanMessageUniqueId(pc_Message->u32_CanId,
                                                               pc_Message->q_IsExtended), q_MessageIdValid,
                                       &c_UniqueMessageIds[u32_ItUniqueMessageId]);
               if (q_MessageIdValid == false)
               {
                  *opq_MessageIdInvalid = true;
               }
            }
         }
      }
   }

   if (opq_MessagesHaveNoTx != NULL)
   {
      *opq_MessagesHaveNoTx = false;
      for (uint32 u32_ItUniqueMessageId = 0; u32_ItUniqueMessageId < c_UniqueMessageIds.size(); ++u32_ItUniqueMessageId)
      {
         bool q_MessageHasTx;
         this->CheckMessageHasTx(q_MessageHasTx, c_UniqueMessageIds[u32_ItUniqueMessageId]);
         if (q_MessageHasTx == false)
         {
            *opq_MessagesHaveNoTx = true;
         }
      }
   }
   if (opq_DelayTimeInvalid != NULL)
   {
      *opq_DelayTimeInvalid = false;
      for (uint32 u32_ItUniqueMessageId = 0; u32_ItUniqueMessageId < c_UniqueMessageIds.size(); ++u32_ItUniqueMessageId)
      {
         const C_OSCCanMessageIdentificationIndices & rc_MessageId = c_UniqueMessageIds[u32_ItUniqueMessageId];
         const C_OSCCanMessageContainer * const pc_MessageContainer =
            C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(rc_MessageId.u32_NodeIndex,
                                                                            rc_MessageId.e_ComProtocol,
                                                                            rc_MessageId.u32_InterfaceIndex,
                                                                            rc_MessageId.u32_DatapoolIndex);
         if (pc_MessageContainer != NULL)
         {
            bool q_DelayTimeInvalid;
            //Don't use opq_DelayTimeInvalid directly as this might then reset a previous error
            pc_MessageContainer->CheckMessageLocalError(NULL, rc_MessageId.u32_MessageIndex, rc_MessageId.q_MessageIsTx,
                                                        NULL, NULL, &q_DelayTimeInvalid, NULL, NULL, NULL,
                                                        ou32_CANMessageValidSignalsDLCOffset,
                                                        oq_CANMessageSignalGapsValid);
            if (q_DelayTimeInvalid == true)
            {
               *opq_DelayTimeInvalid = true;
            }
         }
      }
   }
   if (opq_MessageSignalInvalid != NULL)
   {
      *opq_MessageSignalInvalid = false;
      for (uint32 u32_ItUniqueMessageId = 0; u32_ItUniqueMessageId < c_UniqueMessageIds.size(); ++u32_ItUniqueMessageId)
      {
         const C_OSCCanMessageIdentificationIndices & rc_MessageId = c_UniqueMessageIds[u32_ItUniqueMessageId];
         const C_OSCCanMessage * const pc_Message =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(rc_MessageId);
         const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
            rc_MessageId.u32_NodeIndex, rc_MessageId.e_ComProtocol, rc_MessageId.u32_InterfaceIndex,
            rc_MessageId.u32_DatapoolIndex, rc_MessageId.q_MessageIsTx);
         if ((pc_Message != NULL) && (pc_List != NULL))
         {
            for (uint32 u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
            {
               if (pc_Message->CheckErrorSignal(pc_List, u32_ItSignal, ou32_CANMessageValidSignalsDLCOffset,
                                                oq_CANMessageSignalGapsValid))
               {
                  *opq_MessageSignalInvalid = true;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if message id valid

   \param[in]      orc_MessageId          Message id
   \param[out]     orq_Valid              Flag if valid
   \param[in]      opc_SkipMessage        Optional parameter to skip one message
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in,out]  opq_EcosRangeError     Optional output for ECOS range check result
   \param[in,out]  opq_EcosEvenError      Optional output for ECOS even check result
   \param[in,out]  opq_DuplicateDetected  Optional output for duplicate check result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::CheckMessageIdBus(const C_OSCCanMessageUniqueId & orc_MessageId,
                                                         bool & orq_Valid,
                                                         const C_OSCCanMessageIdentificationIndices * const opc_SkipMessage, bool * const opq_EcosRangeError, bool * const opq_EcosEvenError,
                                                         bool * const opq_DuplicateDetected)
const
{
   const std::vector<C_OSCCanMessageIdentificationIndices> c_UniqueMessageIds =
      this->m_GetAllUniqueMessages();
   bool q_TestSkip = false;
   bool q_Found = false;
   uint32 u32_SkipIndex = 0;

   orq_Valid = true;
   if (opq_EcosRangeError != NULL)
   {
      *opq_EcosRangeError = false;
   }
   if (opq_EcosEvenError != NULL)
   {
      *opq_EcosEvenError = false;
   }
   if (opc_SkipMessage != NULL)
   {
      u32_SkipIndex = this->m_GetMatchingMessageVectorIndex(*opc_SkipMessage);
      q_TestSkip = true;
      //Check ECoS
      if (opc_SkipMessage->e_ComProtocol == C_OSCCanProtocol::eCAN_OPEN_SAFETY)
      {
         if ((orc_MessageId.u32_CanId < mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN) ||
             (orc_MessageId.u32_CanId > mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX))
         {
            orq_Valid = false;
            if (opq_EcosRangeError != NULL)
            {
               *opq_EcosRangeError = true;
            }
         }
         if ((orc_MessageId.u32_CanId % 2) == 0)
         {
            orq_Valid = false;
            if (opq_EcosEvenError != NULL)
            {
               *opq_EcosEvenError = true;
            }
         }
      }
   }

   for (uint32 u32_ItUniqueMessageId = 0; u32_ItUniqueMessageId < c_UniqueMessageIds.size(); ++u32_ItUniqueMessageId)
   {
      bool q_Skip = false;
      //Check which index to skip
      if (q_TestSkip == true)
      {
         if (u32_SkipIndex == u32_ItUniqueMessageId)
         {
            q_Skip = true;
         }
      }
      if (q_Skip == false)
      {
         //Compare
         const C_OSCCanMessage * const pc_CurrentMessage =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_UniqueMessageIds[u32_ItUniqueMessageId]);
         if (pc_CurrentMessage != NULL)
         {
            if ((pc_CurrentMessage->u32_CanId == orc_MessageId.u32_CanId) &&
                (pc_CurrentMessage->q_IsExtended == orc_MessageId.q_IsExtended))
            {
               q_Found = true;
               break;
            }
         }
      }
   }
   if (q_Found == true)
   {
      orq_Valid = false;
   }
   if (opq_DuplicateDetected != NULL)
   {
      *opq_DuplicateDetected = q_Found;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if message name valid

   \param[in]      orc_MessageName     Message name
   \param[out]     orq_Valid           Flag if valid
   \param[in]      opc_SkipMessage     Optional parameter to skip one message
                                       (Use-case: skip current message to avoid conflict with itself)
   \param[in,out]  opq_InvalidName     Optional output for invalid name check result
   \param[in,out]  opq_DuplicateName   Optional output for duplicate name check result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::CheckMessageNameBus(const C_SCLString & orc_MessageName, bool & orq_Valid,
                                                           const C_OSCCanMessageIdentificationIndices * const opc_SkipMessage, bool * const opq_InvalidName,
                                                           bool * const opq_DuplicateName)
const
{
   orq_Valid = C_OSCUtils::h_CheckValidCName(orc_MessageName);
   if (opq_InvalidName != NULL)
   {
      *opq_InvalidName = !orq_Valid;
   }
   if ((orq_Valid == true) || (opq_DuplicateName != NULL))
   {
      bool q_Found = false;
      const std::vector<C_OSCCanMessageIdentificationIndices> c_UniqueMessageIds =
         this->m_GetAllUniqueMessages();
      bool q_TestSkip = false;
      uint32 u32_SkipIndex = 0;

      //Calc skip index
      if (opc_SkipMessage != NULL)
      {
         u32_SkipIndex = this->m_GetMatchingMessageVectorIndex(*opc_SkipMessage);
         q_TestSkip = true;
      }

      //iterate over all unique messages)
      for (uint32 u32_ItUniqueMessageId = 0; u32_ItUniqueMessageId < c_UniqueMessageIds.size(); ++u32_ItUniqueMessageId)
      {
         bool q_Skip = false;
         if (q_TestSkip == true)
         {
            if (u32_SkipIndex == u32_ItUniqueMessageId)
            {
               q_Skip = true;
            }
         }
         if (q_Skip == false)
         {
            //Compare
            const C_OSCCanMessage * const pc_CurrentMessage =
               C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_UniqueMessageIds[u32_ItUniqueMessageId]);
            if (pc_CurrentMessage != NULL)
            {
               if (pc_CurrentMessage->c_Name == orc_MessageName)
               {
                  q_Found = true;
               }
            }
         }
      }
      if (q_Found == true)
      {
         orq_Valid = false;
      }
      if (opq_DuplicateName != NULL)
      {
         *opq_DuplicateName = q_Found;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if message has transmitter

   \param[out]  orq_Valid     Flag if valid
   \param[in]   orc_Message   Message to check
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::CheckMessageHasTx(bool & orq_Valid,
                                                         const C_OSCCanMessageIdentificationIndices & orc_Message) const
{
   if (this->mq_SingleNodeMode == false)
   {
      if (this->me_Protocol != C_OSCCanProtocol::eCAN_OPEN)
      {
         const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessages =
            this->GetMatchingMessageVector(orc_Message);

         orq_Valid = false;
         for (uint32 u32_ItMessage = 0; u32_ItMessage < c_MatchingMessages.size(); ++u32_ItMessage)
         {
            const C_OSCCanMessageIdentificationIndices & rc_CurId = c_MatchingMessages[u32_ItMessage];
            if (rc_CurId.q_MessageIsTx == true)
            {
               orq_Valid = true;
            }
         }
      }
      else
      {
         //CANopen does not use message sync
         orq_Valid = true;
      }
   }
   else
   {
      orq_Valid = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get next usable message id (if any)

   \param[in]  orq_Extended   Extended flag

   \return
   Next valid id if available (otherwise may be invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdNodeCanMessageSyncManager::GetNextValidMessageId(const bool & orq_Extended) const
{
   uint32 u32_MessageId;
   uint32 u32_Maximum = 0;
   bool q_Valid = true;
   const std::vector<C_OSCCanMessageIdentificationIndices> c_Messages = this->m_GetAllUniqueMessages();

   std::vector<uint32> c_MessageIds;

   this->m_ReportCANopenUsage(TGL_UTIL_FUNC_ID);

   //Extract message ids once for performance reasons
   c_MessageIds.reserve(c_Messages.size());
   for (uint32 u32_ItMessage = 0; u32_ItMessage < c_Messages.size(); ++u32_ItMessage)
   {
      const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
         c_Messages[u32_ItMessage]);
      if (pc_Message != NULL)
      {
         c_MessageIds.push_back(pc_Message->u32_CanId);
      }
   }

   if (this->me_Protocol == C_OSCCanProtocol::eCAN_OPEN_SAFETY)
   {
      u32_MessageId = mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN;
      u32_Maximum = mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX;
   }
   else
   {
      u32_MessageId = 0;
      if (orq_Extended == true)
      {
         u32_Maximum = 0x1FFFFFFFUL;
      }
      else
      {
         u32_Maximum = 0x7FF;
      }
   }
   do
   {
      //Skip initial step
      if (q_Valid == false)
      {
         if (this->me_Protocol == C_OSCCanProtocol::eCAN_OPEN_SAFETY)
         {
            u32_MessageId += 2;
         }
         else
         {
            ++u32_MessageId;
         }
      }
      q_Valid = true;
      //Always allow maximum
      if (u32_Maximum != u32_MessageId)
      {
         for (uint32 u32_ItMessage = 0; u32_ItMessage < c_MessageIds.size(); ++u32_ItMessage)
         {
            if (c_MessageIds[u32_ItMessage] == u32_MessageId)
            {
               q_Valid = false;
            }
         }
      }
   }
   while (q_Valid == false);
   return u32_MessageId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if message is still critical

   \param[in]   orc_MessageId       Message identification indices
   \param[out]  orc_NewMessageId    New message identification indices to use instead of the old ones if necessary

   \return
   True  The input message identification index is no longer valid (use the output parameter instead)
   False The input message identification index is still valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdNodeCanMessageSyncManager::RecheckCriticalMessage(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, C_OSCCanMessageIdentificationIndices & orc_NewMessageId)
{
   bool q_Retval = false;
   const bool q_Critical = m_RecheckCriticalMessage(orc_MessageId);

   orc_NewMessageId = orc_MessageId;

   //If not critical any more remove entry
   if (q_Critical == false)
   {
      for (std::vector<C_OSCCanMessageIdentificationIndices>::iterator c_ItCriticalMessage =
              this->mc_CriticalMessageMatches.begin();
           c_ItCriticalMessage != this->mc_CriticalMessageMatches.end();)
      {
         if (*c_ItCriticalMessage == orc_MessageId)
         {
            c_ItCriticalMessage = this->mc_CriticalMessageMatches.erase(c_ItCriticalMessage);
         }
         else
         {
            //Recheck other critical messages
            if (m_RecheckCriticalMessage(*c_ItCriticalMessage) == false)
            {
               c_ItCriticalMessage = this->mc_CriticalMessageMatches.erase(c_ItCriticalMessage);
            }
            else
            {
               ++c_ItCriticalMessage;
            }
         }
      }
   }
   else
   {
      bool q_Found = false;
      //Add if necessary
      for (std::vector<C_OSCCanMessageIdentificationIndices>::iterator c_ItCriticalMessage =
              this->mc_CriticalMessageMatches.begin();
           c_ItCriticalMessage != this->mc_CriticalMessageMatches.end(); ++c_ItCriticalMessage)
      {
         if (*c_ItCriticalMessage == orc_MessageId)
         {
            q_Found = true;
         }
      }
      //New conflict, so better check them all
      if (q_Found == false)
      {
         std::vector<C_OSCCanMessageIdentificationIndices> c_NewCriticalMessageIds;
         //Recheck all
         for (std::vector<std::vector<C_OSCCanMessageIdentificationIndices> >::const_iterator c_ItUniqueMessage =
                 this->mc_MessageMatches.begin(); c_ItUniqueMessage != this->mc_MessageMatches.end();
              ++c_ItUniqueMessage)
         {
            //Only check one of the identical messages
            if (c_ItUniqueMessage->size() > 0)
            {
               //Check if critical
               if (m_RecheckCriticalMessage((*c_ItUniqueMessage)[0]) == true)
               {
                  //Check if already registered
                  if (this->CheckCriticalMessageId((*c_ItUniqueMessage)[0]) == false)
                  {
                     bool q_AlternativeFound = false;
                     C_OSCCanMessageIdentificationIndices c_Alternative;
                     std::vector<C_OSCCanMessageIdentificationIndices> c_ToBeDeleted;

                     //Clean up all Rx for this message
                     //-> critical message matches can not have Rx items as the assignment which transmitter
                     //   cannot be performed
                     for (std::vector<C_OSCCanMessageIdentificationIndices>::const_iterator c_ItMatchingMessage =
                             c_ItUniqueMessage->begin(); c_ItMatchingMessage != c_ItUniqueMessage->end();
                          ++c_ItMatchingMessage)
                     {
                        if (c_ItMatchingMessage->q_MessageIsTx == false)
                        {
                           c_ToBeDeleted.push_back(*c_ItMatchingMessage);
                        }
                        else
                        {
                           q_AlternativeFound = true;
                           c_Alternative = *c_ItMatchingMessage;
                        }
                     }
                     if (q_AlternativeFound == true)
                     {
                        //Register as new
                        c_NewCriticalMessageIds.push_back(c_Alternative);

                        for (std::vector<C_OSCCanMessageIdentificationIndices>::const_iterator c_ItDelteMessage =
                                c_ToBeDeleted.begin(); c_ItDelteMessage != c_ToBeDeleted.end();
                             ++c_ItDelteMessage)
                        {
                           tgl_assert(DeleteCanMessageRx(c_Alternative, c_ItDelteMessage->u32_NodeIndex,
                                                         c_ItDelteMessage->u32_InterfaceIndex,
                                                         c_ItDelteMessage->u32_DatapoolIndex) == C_NO_ERR);
                           //Check if message matches current message ID and set alternative as new
                           //The currently known message id for the caller might be a Rx message which just might have
                           //been deleted therefore its better to return the first found transmitter (still the same
                           //message)
                           if (*c_ItDelteMessage == orc_MessageId)
                           {
                              orc_NewMessageId = c_Alternative;
                              //We have found an alternative
                              q_Retval = true;
                           }
                        }
                     }
                  }
               }
            }
         }
         //Add new message IDs
         for (std::vector<C_OSCCanMessageIdentificationIndices>::iterator c_ItNewCriticalMessage =
                 c_NewCriticalMessageIds.begin();
              c_ItNewCriticalMessage != c_NewCriticalMessageIds.end(); ++c_ItNewCriticalMessage)
         {
            this->mc_CriticalMessageMatches.push_back(*c_ItNewCriticalMessage);
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current unique ID for specified message

   \param[in]  orc_MessageId  Message ID to search for

   \return
   Current unique ID for specified message
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_PuiSdNodeCanMessageSyncManager::GetUniqueId(const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   uint64 u64_Retval = 0ULL;

   tgl_assert(this->mc_MessageMatches.size() == this->mc_MessageMatchUniqueIds.size());
   if (this->mc_MessageMatches.size() == this->mc_MessageMatchUniqueIds.size())
   {
      const uint32 u32_Index = this->m_GetMatchingMessageVectorIndex(orc_MessageId);
      //May happen in certain cases (eg message add, there are no messages...)
      if (u32_Index < this->mc_MessageMatches.size())
      {
         u64_Retval = this->mc_MessageMatchUniqueIds[u32_Index];
      }
   }

   return u64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get message ID for specified unique ID

   \param[in]  ou64_UniqueId  unique ID to search for

   \return
   Message ID for specified unique ID
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanMessageIdentificationIndices C_PuiSdNodeCanMessageSyncManager::GetMessageIdForUniqueId(
   const uint64 ou64_UniqueId) const
{
   C_OSCCanMessageIdentificationIndices c_Retval;

   tgl_assert(this->mc_MessageMatches.size() == this->mc_MessageMatchUniqueIds.size());
   if (this->mc_MessageMatches.size() == this->mc_MessageMatchUniqueIds.size())
   {
      for (uint32 u32_ItUniqueId = 0; u32_ItUniqueId < this->mc_MessageMatchUniqueIds.size(); ++u32_ItUniqueId)
      {
         if (this->mc_MessageMatchUniqueIds[u32_ItUniqueId] == ou64_UniqueId)
         {
            const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & rc_MessageMatches =
               this->mc_MessageMatches[u32_ItUniqueId];
            if (rc_MessageMatches.size() > 0UL)
            {
               c_Retval = rc_MessageMatches[0UL];
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Search for message ID by message name

   \param[in]   orc_MessageName  Message name (ID)
   \param[out]  orc_MessageId    Found message ID

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::GetMessageIdForMessageName(const QString & orc_MessageName,
                                                                    C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   sint32 s32_Retval = C_RANGE;

   //parse different messages
   for (uint32 u32_It = 0; u32_It < this->mc_MessageMatches.size(); ++u32_It)
   {
      const std::vector<C_OSCCanMessageIdentificationIndices> & rc_MatchingMessageIds =
         this->mc_MessageMatches[u32_It];
      if (rc_MatchingMessageIds.size() > 0UL)
      {
         const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
            rc_MatchingMessageIds[0UL]);
         if (pc_Message != NULL)
         {
            if (pc_Message->c_Name == orc_MessageName.toStdString().c_str())
            {
               orc_MessageId = rc_MatchingMessageIds[0UL];
               s32_Retval = C_NO_ERR;
               break;
            }
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of vector of matching message ids

   \param[in]  orc_MessageId  Message identification indices to search for

   \return
   Index of vector of matching message ids (otherwise out of range -> size of mc_MessageMatches)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdNodeCanMessageSyncManager::m_GetMatchingMessageVectorIndex(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   uint32 u32_Retval;

   //parse different messages
   for (u32_Retval = 0; u32_Retval < this->mc_MessageMatches.size(); ++u32_Retval)
   {
      const std::vector<C_OSCCanMessageIdentificationIndices> & rc_MatchingMessageIds =
         this->mc_MessageMatches[u32_Retval];
      bool q_Found = false;
      //Check if any message id matches
      for (uint32 u32_ItMessage = 0; (u32_ItMessage < rc_MatchingMessageIds.size()) && (q_Found == false);
           ++u32_ItMessage)
      {
         const C_OSCCanMessageIdentificationIndices & rc_MessageId = rc_MatchingMessageIds[u32_ItMessage];
         if (orc_MessageId == rc_MessageId)
         {
            //Found
            q_Found = true;
            break;
         }
      }
      //Stop if match found
      if (q_Found == true)
      {
         break;
      }
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add this message id to the internal matching message vector

   \param[in]  orc_MessageId  Message identification indices to register
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::m_RegisterIfNecessary(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   mh_RegisterIfNecessary(orc_MessageId, this->mc_MessageMatches, &this->mc_MessageMatchUniqueIds,
                          &this->mc_CriticalMessageMatches);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to check if message id already stored

   \param[in]  orc_MessageId  Message identification indices to search for
   \param[in]  orc_Input      Message vector to check

   \return
   true: exist
   false: does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdNodeCanMessageSyncManager::mh_CheckIfAlreadyExisting(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId,
   const std::vector<std::vector<C_OSCCanMessageIdentificationIndices> > & orc_Input)
{
   bool q_Retval = false;

   //parse different messages
   for (uint32 u32_ItDifferentMessage = 0; (u32_ItDifferentMessage < orc_Input.size()) && (q_Retval == false);
        ++u32_ItDifferentMessage)
   {
      const std::vector<C_OSCCanMessageIdentificationIndices> & rc_MatchingMessageIds =
         orc_Input[u32_ItDifferentMessage];
      //Check if any message id matches
      for (uint32 u32_ItMessage = 0; (u32_ItMessage < rc_MatchingMessageIds.size()) && (q_Retval == false);
           ++u32_ItMessage)
      {
         const C_OSCCanMessageIdentificationIndices & rc_MessageId = rc_MatchingMessageIds[u32_ItMessage];
         if (orc_MessageId == rc_MessageId)
         {
            //Stop if match found
            q_Retval = true;
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add this message id to the internal matching message vector

   \param[in]      orc_MessageId                Message identification indices to register
   \param[in,out]  orc_Output                   Message vector to update
   \param[in,out]  opc_OutputUniqueIds          Optional vector to store the new unique IDs in
   \param[in,out]  opc_CriticalMessageMatches   Optional vector for critical messages with two transmitter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::mh_RegisterIfNecessary(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId,
   std::vector<std::vector<C_OSCCanMessageIdentificationIndices> > & orc_Output,
   std::vector<stw_types::uint64 > * const opc_OutputUniqueIds,
   std::vector<C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches)
{
   if (mh_CheckIfAlreadyExisting(orc_MessageId, orc_Output) == false)
   {
      std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds;
      sint32 s32_Result;
      if (orc_MessageId.e_ComProtocol == C_OSCCanProtocol::eCAN_OPEN)
      {
         // Always exactly one for CANopen
         c_MatchingMessageIds.push_back(orc_MessageId);
         s32_Result = C_NO_ERR;
      }
      else
      {
         s32_Result = mh_GetNodeIndexesMatchingForMessage(orc_MessageId, c_MatchingMessageIds,
                                                          opc_CriticalMessageMatches);
      }
      if (s32_Result == C_NO_ERR)
      {
         //Should always include itself
         tgl_assert(c_MatchingMessageIds.size() > 0);
         orc_Output.push_back(c_MatchingMessageIds);
         if (opc_OutputUniqueIds != NULL)
         {
            opc_OutputUniqueIds->push_back(mh_GetNewUniqueId(*opc_OutputUniqueIds));
         }
      }
   }
   else
   {
      C_PuiSdNodeCanMessageSyncManager::mh_ReportCANopenUsage(orc_MessageId.e_ComProtocol, TGL_UTIL_FUNC_ID);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove this message id from the internal matching message vector

   \param[in]  orc_MessageId     Message identification indices to remove
   \param[in]  oq_AllowDelete    Optional allow delete of matching messages vector flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::m_RemoveAndUpdateIndices(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const bool oq_AllowDelete)
{
   //lint -e{850} Index modified for erase index step
   for (uint32 u32_ItDifferentMessage = 0; u32_ItDifferentMessage < this->mc_MessageMatches.size();
        ++u32_ItDifferentMessage)
   {
      std::vector<C_OSCCanMessageIdentificationIndices> & rc_DifferentMessage =
         this->mc_MessageMatches[u32_ItDifferentMessage];
      mh_RemoveAndUpdateIndices(orc_MessageId, rc_DifferentMessage);
      //Check if all instances deleted
      if ((rc_DifferentMessage.size() == 0) && (oq_AllowDelete == true))
      {
         this->mc_MessageMatches.erase(this->mc_MessageMatches.begin() + u32_ItDifferentMessage);
         this->mc_MessageMatchUniqueIds.erase(this->mc_MessageMatchUniqueIds.begin() + u32_ItDifferentMessage);
         //New message at current position
         --u32_ItDifferentMessage;
      }
   }
   //Critical messages
   mh_RemoveAndUpdateIndices(orc_MessageId, this->mc_CriticalMessageMatches);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove this message id from the internal matching message vector

   \param[in]      orc_MessageId    Message identification indices to remove
   \param[in,out]  orc_MessageIds   Message ID vector to adapt
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::mh_RemoveAndUpdateIndices(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId,
   std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageIds)
{
   for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_MessageIds.size();)
   {
      C_OSCCanMessageIdentificationIndices & rc_MessageId = orc_MessageIds[u32_ItMessage];
      if (rc_MessageId == orc_MessageId)
      {
         orc_MessageIds.erase(orc_MessageIds.begin() + u32_ItMessage);
      }
      else
      {
         //Check if same vector
         if ((rc_MessageId.u32_NodeIndex == orc_MessageId.u32_NodeIndex) &&
             (rc_MessageId.u32_InterfaceIndex == orc_MessageId.u32_InterfaceIndex) &&
             (rc_MessageId.u32_DatapoolIndex == orc_MessageId.u32_DatapoolIndex) &&
             (rc_MessageId.e_ComProtocol == orc_MessageId.e_ComProtocol) &&
             (rc_MessageId.q_MessageIsTx == orc_MessageId.q_MessageIsTx))
         {
            //Update all affected ones to changed position
            if (rc_MessageId.u32_MessageIndex > orc_MessageId.u32_MessageIndex)
            {
               --rc_MessageId.u32_MessageIndex;
            }
         }
         //Important iterator step
         ++u32_ItMessage;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all nodes matching the specified message (including the source node)

   \param[in]      orc_MessageId                Message identification indices
   \param[out]     orc_MatchingMessageIds       Matching message identification indices
   \param[in,out]  opc_CriticalMessageMatches   Optional vector for critical messages with two transmitter

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::mh_GetNodeIndexesMatchingForMessage(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId,
   std::vector<C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds,
   std::vector<C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches)
{
   bool q_CriticalMatchFoundASecondTime = false;
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_OrgNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_MessageId.u32_NodeIndex);

   C_PuiSdNodeCanMessageSyncManager::mh_ReportCANopenUsage(orc_MessageId.e_ComProtocol, TGL_UTIL_FUNC_ID);

   //Setp 1: Find connected bus
   tgl_assert(pc_OrgNode != NULL);
   if (pc_OrgNode != NULL)
   {
      const C_OSCCanMessageContainer * const pc_MessageContainer =
         C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_MessageId.u32_NodeIndex,
                                                                         orc_MessageId.e_ComProtocol,
                                                                         orc_MessageId.u32_InterfaceIndex,
                                                                         orc_MessageId.u32_DatapoolIndex);
      tgl_assert(orc_MessageId.u32_InterfaceIndex < pc_OrgNode->c_Properties.c_ComInterfaces.size());
      if ((orc_MessageId.u32_InterfaceIndex < pc_OrgNode->c_Properties.c_ComInterfaces.size()) &&
          (pc_MessageContainer != NULL))
      {
         std::vector<uint32> c_NodeIndices;
         std::vector<uint32> c_InterfaceIndices;
         std::vector<uint32> c_DatapoolIndices;
         const C_OSCNodeComInterfaceSettings & rc_ComInterface =
            pc_OrgNode->c_Properties.c_ComInterfaces[orc_MessageId.u32_InterfaceIndex];
         //Step 2: Find all other nodes and interfaces (if connected)
         if ((rc_ComInterface.GetBusConnected() == true) &&
             (pc_MessageContainer->q_IsComProtocolUsedByInterface == true))
         {
            C_PuiSdNodeCanMessageSyncManager::h_GetConnectedAndActiveInterfaces(rc_ComInterface.u32_BusIndex,
                                                                                orc_MessageId.e_ComProtocol,
                                                                                c_NodeIndices, c_InterfaceIndices,
                                                                                c_DatapoolIndices);
         }
         else
         {
            // Add all Datapools for the protocol
            uint32 u32_ProtCounter;

            for (u32_ProtCounter = 0U; u32_ProtCounter < pc_OrgNode->c_ComProtocols.size(); ++u32_ProtCounter)
            {
               const C_OSCCanProtocol & rc_Prot =  pc_OrgNode->c_ComProtocols[u32_ProtCounter];

               tgl_assert(orc_MessageId.u32_InterfaceIndex < rc_Prot.c_ComMessages.size());
               if (rc_Prot.e_Type == orc_MessageId.e_ComProtocol)
               {
                  c_NodeIndices.push_back(orc_MessageId.u32_NodeIndex);
                  c_InterfaceIndices.push_back(orc_MessageId.u32_InterfaceIndex);
                  c_DatapoolIndices.push_back(rc_Prot.u32_DataPoolIndex);
               }
            }
         }

         if ((c_NodeIndices.size() == c_InterfaceIndices.size()) &&
             (c_NodeIndices.size() == c_DatapoolIndices.size()))
         {
            for (uint32 u32_ItNode = 0; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
            {
               const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                  c_NodeIndices[u32_ItNode]);
               if (pc_Node != NULL)
               {
                  const C_OSCCanProtocol * const pc_Protocol = pc_Node->GetCANProtocolConst(
                     orc_MessageId.e_ComProtocol,
                     c_DatapoolIndices[u32_ItNode]);

                  if (pc_Protocol != NULL)
                  {
                     if (c_InterfaceIndices[u32_ItNode] < pc_Protocol->c_ComMessages.size())
                     {
                        C_OSCCanMessageIdentificationIndices c_MessageId(c_NodeIndices[u32_ItNode],
                                                                         orc_MessageId.e_ComProtocol,
                                                                         c_InterfaceIndices[u32_ItNode],
                                                                         c_DatapoolIndices[u32_ItNode], false);
                        const C_OSCCanMessageContainer & rc_MessageContainer =
                           pc_Protocol->c_ComMessages[c_InterfaceIndices[u32_ItNode]];
                        //Toggle tx and rx
                        for (uint32 u32_Toggle = 0; u32_Toggle < 2; ++u32_Toggle)
                        {
                           bool q_Tx;
                           if (u32_Toggle == 0)
                           {
                              q_Tx = true;
                           }
                           else
                           {
                              q_Tx = false;
                           }
                           {
                              const std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(
                                 q_Tx);
                              //Compare all tx and rx messages
                              c_MessageId.q_MessageIsTx = q_Tx;
                              for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_Messages.size(); ++u32_ItMessage)
                              {
                                 c_MessageId.u32_MessageIndex = u32_ItMessage;
                                 mh_HandleMessageComparison(orc_MessageId, c_MessageId, &orc_MatchingMessageIds,
                                                            opc_CriticalMessageMatches,
                                                            &q_CriticalMatchFoundASecondTime, NULL);
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   if (q_CriticalMatchFoundASecondTime == true)
   {
      //Remove all rx informations
      //For the second message which causes the critical message all receivers were deleted and therefore the
      //matching message vector has to be cleaned up as well
      for (std::vector<C_OSCCanMessageIdentificationIndices>::iterator c_ItMessageId = orc_MatchingMessageIds.begin();
           c_ItMessageId != orc_MatchingMessageIds.end();)
      {
         if (c_ItMessageId->q_MessageIsTx == false)
         {
            c_ItMessageId = orc_MatchingMessageIds.erase(c_ItMessageId);
         }
         else
         {
            ++c_ItMessageId;
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete can message

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  oq_AllowDelete    Optional allow delete of matching messages vector flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdNodeCanMessageSyncManager::m_DeleteCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                            const bool oq_AllowDelete)
{
   const sint32 s32_Retval = C_PuiSdHandler::h_GetInstance()->DeleteCanMessage(orc_MessageId);

   this->m_RemoveAndUpdateIndices(orc_MessageId, oq_AllowDelete);
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal message id vector for changes by this action

   REQUIRES: Call before adding new id to vector

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::m_UpdateIndicesToNewCanMessage(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   for (uint32 u32_ItDifferentMessage = 0; u32_ItDifferentMessage < this->mc_MessageMatches.size();
        ++u32_ItDifferentMessage)
   {
      std::vector<C_OSCCanMessageIdentificationIndices> & rc_DifferentMessage =
         this->mc_MessageMatches[u32_ItDifferentMessage];
      mh_UpdateIndicesToNewCanMessage(orc_MessageId, rc_DifferentMessage);
   }
   //Critical messages
   mh_UpdateIndicesToNewCanMessage(orc_MessageId, this->mc_CriticalMessageMatches);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal message id vector for changes by this action

   REQUIRES: Call before adding new id to vector

   \param[in]      orc_MessageId    Message identification indices
   \param[in,out]  orc_MessageIds   Message ID vector to adapt
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::mh_UpdateIndicesToNewCanMessage(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId,
   std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageIds)
{
   for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_MessageIds.size(); ++u32_ItMessage)
   {
      C_OSCCanMessageIdentificationIndices & rc_MessageId = orc_MessageIds[u32_ItMessage];
      //Check if same vector
      if ((rc_MessageId.u32_NodeIndex == orc_MessageId.u32_NodeIndex) &&
          (rc_MessageId.u32_InterfaceIndex == orc_MessageId.u32_InterfaceIndex) &&
          (rc_MessageId.u32_DatapoolIndex == orc_MessageId.u32_DatapoolIndex) &&
          (rc_MessageId.e_ComProtocol == orc_MessageId.e_ComProtocol) &&
          (rc_MessageId.q_MessageIsTx == orc_MessageId.q_MessageIsTx))
      {
         //Update all affected ones to changed position
         if (rc_MessageId.u32_MessageIndex >= orc_MessageId.u32_MessageIndex)
         {
            ++rc_MessageId.u32_MessageIndex;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal message id vector for changes by this action

   REQUIRES: Call with previous message id

   \param[in]  orc_MessageId  Message identification indices (previous direction)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::m_UpdateIndicesToCanMessageDirectionChange(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   //All unique messages
   for (std::vector<std::vector<C_OSCCanMessageIdentificationIndices> >::iterator
        c_ItUniqueMessage = this->mc_MessageMatches.begin();
        c_ItUniqueMessage != this->mc_MessageMatches.end(); ++c_ItUniqueMessage)
   {
      std::vector<C_OSCCanMessageIdentificationIndices> & rc_UniqueMessage = *c_ItUniqueMessage;
      mh_UpdateIndicesToCanMessageDirectionChange(orc_MessageId, rc_UniqueMessage);
   }
   //Critical messages
   mh_UpdateIndicesToCanMessageDirectionChange(orc_MessageId, this->mc_CriticalMessageMatches);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal message id vector for changes by this action

   REQUIRES: Call with previous message id

   \param[in]      orc_MessageId    Message identification indices (previous direction)
   \param[in,out]  orc_MessageIds   Message ID vector to adapt
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::mh_UpdateIndicesToCanMessageDirectionChange(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId,
   std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageIds)
{
   //All matching messages
   for (std::vector<C_OSCCanMessageIdentificationIndices>::iterator
        c_ItMatchingMessage = orc_MessageIds.begin();
        c_ItMatchingMessage != orc_MessageIds.end(); ++c_ItMatchingMessage)
   {
      //All IDs
      C_OSCCanMessageIdentificationIndices & rc_CurMessageId = *c_ItMatchingMessage;
      if ((rc_CurMessageId.u32_NodeIndex == orc_MessageId.u32_NodeIndex) &&
          ((rc_CurMessageId.e_ComProtocol == orc_MessageId.e_ComProtocol) &&
           (rc_CurMessageId.q_MessageIsTx ==
            orc_MessageId.q_MessageIsTx)))
      {
         if (rc_CurMessageId.u32_MessageIndex > orc_MessageId.u32_MessageIndex)
         {
            --rc_CurMessageId.u32_MessageIndex;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init message matches

   \param[in]      oru32_BusIndex               Bus index
   \param[in]      ore_ComProtocol              Protocol type
   \param[out]     orc_Output                   Created message vector
                                                1./Outer layer: all non unique messages
                                                2./Inner layer: all matching IDs for one unique message
   \param[in,out]  opc_OutputUniqueIds          Optional vector to store the new unique IDs in
   \param[in,out]  opc_CriticalMessageMatches   Optional vector for critical messages with two transmitter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::mh_Init(const uint32 & oru32_BusIndex,
                                               const C_OSCCanProtocol::E_Type & ore_ComProtocol,
                                               std::vector<std::vector<C_OSCCanMessageIdentificationIndices> > & orc_Output, std::vector<stw_types::uint64 > * const opc_OutputUniqueIds,
                                               std::vector<C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches)
{
   std::vector<uint32> c_NodeIndexes;
   std::vector<uint32> c_InterfaceIndexes;
   std::vector<uint32> c_DatapoolIndexes;

   C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeAndComDpIndexesOfBus(oru32_BusIndex,
                                                                                              ore_ComProtocol,
                                                                                              c_NodeIndexes,
                                                                                              c_InterfaceIndexes,
                                                                                              c_DatapoolIndexes);

   if ((c_NodeIndexes.size() == c_InterfaceIndexes.size()) &&
       (c_NodeIndexes.size() == c_DatapoolIndexes.size()))
   {
      for (uint32 u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndexes[u32_ItNode]);
         if (pc_Node != NULL)
         {
            const C_OSCCanProtocol * const pc_Protcol = pc_Node->GetCANProtocolConst(ore_ComProtocol,
                                                                                     c_DatapoolIndexes[u32_ItNode]);
            if (pc_Protcol != NULL)
            {
               if (c_InterfaceIndexes[u32_ItNode] < pc_Protcol->c_ComMessages.size())
               {
                  const C_OSCCanMessageContainer & rc_MessageContainer =
                     pc_Protcol->c_ComMessages[c_InterfaceIndexes[u32_ItNode]];
                  if (rc_MessageContainer.q_IsComProtocolUsedByInterface == true)
                  {
                     C_OSCCanMessageIdentificationIndices c_MessageId(c_NodeIndexes[u32_ItNode], ore_ComProtocol,
                                                                      c_InterfaceIndexes[u32_ItNode],
                                                                      c_DatapoolIndexes[u32_ItNode], true);
                     const std::vector<C_OSCCanMessage> & rc_TxMessages =  rc_MessageContainer.GetMessagesConst(true);
                     const std::vector<C_OSCCanMessage> & rc_RxMessages =  rc_MessageContainer.GetMessagesConst(false);

                     //Tx
                     for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_TxMessages.size(); ++u32_ItMessage)
                     {
                        c_MessageId.u32_MessageIndex = u32_ItMessage;
                        mh_RegisterIfNecessary(c_MessageId, orc_Output, opc_OutputUniqueIds,
                                               opc_CriticalMessageMatches);
                     }

                     //Rx
                     c_MessageId.q_MessageIsTx = false;
                     for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_RxMessages.size(); ++u32_ItMessage)
                     {
                        c_MessageId.u32_MessageIndex = u32_ItMessage;
                        mh_RegisterIfNecessary(c_MessageId, orc_Output, opc_OutputUniqueIds,
                                               opc_CriticalMessageMatches);
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init message matches

   \param[in]      oru32_NodeIndex              Node index
   \param[in]      oru32_InterfaceIndex         Interface index
   \param[in]      ore_ComProtocol              Protocol type
   \param[out]     orc_Output                   Created message vector
   \param[in,out]  opc_OutputUniqueIds          Output unique ids
   \param[in,out]  opc_CriticalMessageMatches   Optional vector for critical messages with two transmitter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::mh_Init(const uint32 & oru32_NodeIndex, const uint32 & oru32_InterfaceIndex,
                                               const C_OSCCanProtocol::E_Type & ore_ComProtocol,
                                               std::vector<std::vector<C_OSCCanMessageIdentificationIndices> > & orc_Output, std::vector<stw_types::uint64 > * const opc_OutputUniqueIds,
                                               std::vector<C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(oru32_NodeIndex);

   if (pc_Node != NULL)
   {
      std::vector<const C_OSCCanProtocol *> c_Protocols = pc_Node->GetCANProtocolsConst(ore_ComProtocol);
      uint32 u32_ProtocolCounter;

      for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < c_Protocols.size(); ++u32_ProtocolCounter)
      {
         const C_OSCCanProtocol * const pc_Protcol = c_Protocols[u32_ProtocolCounter];
         if (pc_Protcol != NULL)
         {
            if ((oru32_InterfaceIndex < pc_Protcol->c_ComMessages.size()) &&
                (oru32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
            {
               const C_OSCCanMessageContainer & rc_MessageContainer =
                  pc_Protcol->c_ComMessages[oru32_InterfaceIndex];
               const C_OSCNodeComInterfaceSettings & rc_Interface =
                  pc_Node->c_Properties.c_ComInterfaces[oru32_InterfaceIndex];
               if ((rc_MessageContainer.q_IsComProtocolUsedByInterface == false) ||
                   (rc_Interface.GetBusConnected() == false))
               {
                  C_OSCCanMessageIdentificationIndices c_MessageId(oru32_NodeIndex, ore_ComProtocol,
                                                                   oru32_InterfaceIndex, pc_Protcol->u32_DataPoolIndex,
                                                                   true);
                  const std::vector<C_OSCCanMessage> & rc_TxMessages =  rc_MessageContainer.GetMessagesConst(true);
                  const std::vector<C_OSCCanMessage> & rc_RxMessages =  rc_MessageContainer.GetMessagesConst(false);

                  //Tx
                  for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_TxMessages.size(); ++u32_ItMessage)
                  {
                     c_MessageId.u32_MessageIndex = u32_ItMessage;
                     mh_RegisterIfNecessary(c_MessageId, orc_Output, opc_OutputUniqueIds, opc_CriticalMessageMatches);
                  }

                  //Rx
                  c_MessageId.q_MessageIsTx = false;
                  for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_RxMessages.size(); ++u32_ItMessage)
                  {
                     c_MessageId.u32_MessageIndex = u32_ItMessage;
                     mh_RegisterIfNecessary(c_MessageId, orc_Output, opc_OutputUniqueIds, opc_CriticalMessageMatches);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get vector of different, unique message ids for all protocols

   \return
   Different, unique message ids for all protocols
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCCanMessageIdentificationIndices> C_PuiSdNodeCanMessageSyncManager::m_GetAllUniqueMessages(void) const
{
   std::vector<C_OSCCanMessageIdentificationIndices> c_Retval;
   mh_Append(mh_GetUniqueMessages(this->mc_MessageMatches), c_Retval);
   if (!this->mq_SingleNodeMode)
   {
      for (uint32 u32_ItProt = 0UL; u32_ItProt < this->mc_MessageMatchesForOtherProtocols.size(); ++u32_ItProt)
      {
         mh_Append(mh_GetUniqueMessages(this->mc_MessageMatchesForOtherProtocols[u32_ItProt]), c_Retval);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get vector of different, unique message ids

   \param[in]  orc_Input   Message vector to check

   \return
   Different, unique message ids
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCCanMessageIdentificationIndices> C_PuiSdNodeCanMessageSyncManager::mh_GetUniqueMessages(
   const std::vector<std::vector<C_OSCCanMessageIdentificationIndices> > & orc_Input)
{
   std::vector<C_OSCCanMessageIdentificationIndices> c_Retval;
   c_Retval.reserve(orc_Input.size());
   for (uint32 u32_ItDifferentMessage = 0; u32_ItDifferentMessage < orc_Input.size();
        ++u32_ItDifferentMessage)
   {
      const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds =
         orc_Input[u32_ItDifferentMessage];
      if (orc_MatchingMessageIds.size() > 0)
      {
         c_Retval.push_back(orc_MatchingMessageIds[0]);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append vector elemnts to output

   \param[in]      orc_Input     Input vector
   \param[in,out]  orc_Output    Output vector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::mh_Append(const std::vector<C_OSCCanMessageIdentificationIndices> & orc_Input,
                                                 std::vector<C_OSCCanMessageIdentificationIndices> & orc_Output)
{
   const stw_types::uint32 u32_PreviousEnd = orc_Output.size();

   orc_Output.resize(orc_Output.size() + orc_Input.size());
   for (uint32 u32_ItInput = 0; u32_ItInput < orc_Input.size(); ++u32_ItInput)
   {
      orc_Output[static_cast<std::vector< C_OSCCanMessageIdentificationIndices>::size_type >
                 (u32_PreviousEnd + u32_ItInput)] = orc_Input[u32_ItInput];
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle message comparison with all cases

   \param[in]      orc_MessageIdReference             Message identification indices (Current check)
   \param[in]      orc_MessageIdNew                   Message identification indices (Current iteration)
   \param[in,out]  opc_MatchingMessageIds             Vector for matching message IDs
   \param[in,out]  opc_CriticalMessageMatches         Vector for critical message matches
   \param[in,out]  opq_CriticalMatchFoundASecondTime  Flag if current critical match is second occurrence
   \param[in,out]  opq_CriticalMatchFound             Flag if current comparison was a critical match
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::mh_HandleMessageComparison(
   const C_OSCCanMessageIdentificationIndices & orc_MessageIdReference,
   const C_OSCCanMessageIdentificationIndices & orc_MessageIdNew,
   std::vector<C_OSCCanMessageIdentificationIndices> * const opc_MatchingMessageIds,
   std::vector<C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches,
   bool * const opq_CriticalMatchFoundASecondTime, bool * const opq_CriticalMatchFound)
{
   bool q_Match = false;

   if (opq_CriticalMatchFound != NULL)
   {
      *opq_CriticalMatchFound = false;
   }
   //Compare members with Tx check active
   tgl_assert(C_PuiSdHandler::h_GetInstance()->CheckMessageMatch(orc_MessageIdReference,
                                                                 orc_MessageIdNew,
                                                                 q_Match) == C_NO_ERR);
   if (q_Match == true)
   {
      if (opc_MatchingMessageIds != NULL)
      {
         bool q_Allowed = true;
         //Cross check with existing messages
         for (uint32 u32_ItExistingMatches = 0;
              u32_ItExistingMatches < opc_MatchingMessageIds->size(); ++u32_ItExistingMatches)
         {
            if (orc_MessageIdNew == (*opc_MatchingMessageIds)[u32_ItExistingMatches])
            {
               q_Allowed = false;
            }
         }
         if (q_Allowed == true)
         {
            opc_MatchingMessageIds->push_back(orc_MessageIdNew);
         }
      }
   }
   else
   {
      //Check for matching Tx conflict
      if (((orc_MessageIdReference.q_MessageIsTx == true) &&
           (orc_MessageIdNew.q_MessageIsTx == true)) &&
          ((orc_MessageIdNew == orc_MessageIdReference) == false))
      {
         //Detailed comparison
         tgl_assert(C_PuiSdHandler::h_GetInstance()->CheckMessageMatch(orc_MessageIdReference,
                                                                       orc_MessageIdNew,
                                                                       q_Match,
                                                                       true) ==
                    C_NO_ERR);
         if (q_Match == true)
         {
            //Handle same message two tx use case
            if (opc_CriticalMessageMatches != NULL)
            {
               opc_CriticalMessageMatches->push_back(orc_MessageIdReference);
               if (opq_CriticalMatchFoundASecondTime != NULL)
               {
                  //If second remove all rx information
                  for (std::vector<C_OSCCanMessageIdentificationIndices>::const_iterator
                       c_ItCriticalMessageId = opc_CriticalMessageMatches->begin();
                       c_ItCriticalMessageId != opc_CriticalMessageMatches->end();
                       ++c_ItCriticalMessageId)
                  {
                     if (*c_ItCriticalMessageId == orc_MessageIdNew)
                     {
                        *opq_CriticalMatchFoundASecondTime = true;
                     }
                  }
               }
            }
            if (opq_CriticalMatchFound != NULL)
            {
               *opq_CriticalMatchFound = true;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if message ID is critical

   \param[in]  orc_MessageId  Message identification indices

   \return
   True  Critical
   False Not critical
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdNodeCanMessageSyncManager::m_RecheckCriticalMessage(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   bool q_Retval = false;

   //Compare with all messages, search for critical matches
   for (std::vector<std::vector<C_OSCCanMessageIdentificationIndices> >::const_iterator c_ItUniqueMessage =
           this->mc_MessageMatches.begin();
        (c_ItUniqueMessage != this->mc_MessageMatches.end()) && (q_Retval == false); ++c_ItUniqueMessage)
   {
      for (std::vector<C_OSCCanMessageIdentificationIndices>::const_iterator c_ItMessageId =
              c_ItUniqueMessage->begin();
           (c_ItMessageId != c_ItUniqueMessage->end()) && (q_Retval == false); ++c_ItMessageId)
      {
         bool q_CurrentCritical = true;
         mh_HandleMessageComparison(orc_MessageId, *c_ItMessageId, NULL, NULL, NULL, &q_CurrentCritical);
         if (q_CurrentCritical == true)
         {
            q_Retval = true;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get next free unique ID

   \return
   Next free unique ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_PuiSdNodeCanMessageSyncManager::m_GetNewUniqueId(void) const
{
   return mh_GetNewUniqueId(this->mc_MessageMatchUniqueIds);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get next free unique ID

   \param[in]  orc_ExistingUniqueIds   Current existing unique Ids

   \return
   Next free unique ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_PuiSdNodeCanMessageSyncManager::mh_GetNewUniqueId(const std::vector<uint64> & orc_ExistingUniqueIds)
{
   uint64 u64_Retval = 1ULL;
   bool q_Found;

   //If performance is an issue you might use a static variable instead
   do
   {
      q_Found = false;
      for (uint32 u32_ItExistingId = 0; u32_ItExistingId < orc_ExistingUniqueIds.size(); ++u32_ItExistingId)
      {
         if (u64_Retval == orc_ExistingUniqueIds[u32_ItExistingId])
         {
            q_Found = true;
            ++u64_Retval;
            break;
         }
      }
   }
   while (q_Found == true);
   return u64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report CANopen usage

   \param[in]  orc_Function   Function name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::m_ReportCANopenUsage(const QString & orc_Function) const
{
   C_PuiSdNodeCanMessageSyncManager::mh_ReportCANopenUsage(this->me_Protocol, orc_Function);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report CANopen usage

   \param[in]  oe_Type        Protocol type
   \param[in]  orc_Function   Function name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessageSyncManager::mh_ReportCANopenUsage(const C_OSCCanProtocol::E_Type oe_Type,
                                                             const QString & orc_Function)
{
   if (oe_Type == C_OSCCanProtocol::eCAN_OPEN)
   {
      std::cout << "Invalid call of function \"" << orc_Function.toStdString() << "\" in CANopen protocol use case" <<
         std::endl;
      tgl_assert(false);
   }
}
