//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_SdBueUnoMessageAddDeleteBaseCommand.h"
#include "C_PuiSdHandler.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_types;
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

   \param[in]     orc_MessageId          Message identification indices
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
   \param[in]     orc_Text               Optional command text for informational display
   \param[in,out] opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoMessageAddDeleteBaseCommand::C_SdBueUnoMessageAddDeleteBaseCommand(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId,
   C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text,
   QUndoCommand * const opc_Parent) :
   C_SdBueUnoMessageBaseCommand(orc_MessageId, opc_MessageSyncManager, opc_MessageTreeWidget, orc_Text, opc_Parent),
   mc_LastMessageId(orc_MessageId)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generic add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddDeleteBaseCommand::Add(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      //Adapt default values for protocol
      if (this->mc_LastMessageId.e_ComProtocol == C_OSCCanProtocol::eECES)
      {
         //Tx method
         this->mc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CYCLIC;
      }
      else if (this->mc_LastMessageId.e_ComProtocol == C_OSCCanProtocol::eCAN_OPEN_SAFETY)
      {
         //Message id
         this->mc_Message.q_IsExtended = false;
         if ((this->mc_Message.u32_CanId % 2) == 0)
         {
            --this->mc_Message.u32_CanId;
         }
         if (this->mc_Message.u32_CanId < mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN)
         {
            this->mc_Message.u32_CanId = mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN;
         }
         if (this->mc_Message.u32_CanId > mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX)
         {
            this->mc_Message.u32_CanId = mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX;
         }
         //Tx method
         this->mc_Message.e_TxMethod = C_OSCCanMessage::eTX_METHOD_CYCLIC;
         //Dlc
         this->mc_Message.u16_Dlc = 8;
      }
      else
      {
         //Layer 2 -> all
      }
      //Cycle time
      if (this->mc_Message.u32_CycleTimeMs == 0)
      {
         this->mc_Message.u32_CycleTimeMs = 1;
      }
      tgl_assert(this->mpc_MessageSyncManager->AddCanMessage(this->mc_LastMessageId.u32_NodeIndex,
                                                             this->mc_LastMessageId.e_ComProtocol,
                                                             this->mc_LastMessageId.u32_InterfaceIndex,
                                                             this->mc_LastMessageId.q_MessageIsTx,
                                                             this->mc_Message,
                                                             this->mc_OSCSignalCommons,
                                                             this->mc_UISignalCommons,
                                                             this->mc_UISignals,
                                                             this->mc_LastMessageId.u32_MessageIndex) == C_NO_ERR);
      for (uint32 u32_ItMessage = 0; u32_ItMessage < this->mc_MatchingIds.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessageIdentificationIndices & rc_CurMessageId = this->mc_MatchingIds[u32_ItMessage];
         //Skip already added one
         if ((rc_CurMessageId.u32_NodeIndex != this->mc_LastMessageId.u32_NodeIndex) ||
             (rc_CurMessageId.u32_InterfaceIndex != this->mc_LastMessageId.u32_InterfaceIndex))
         {
            //Always add as RX
            tgl_assert(this->mpc_MessageSyncManager->AddCanMessageRx(this->mc_LastMessageId,
                                                                     rc_CurMessageId.u32_NodeIndex,
                                                                     rc_CurMessageId.u32_InterfaceIndex) ==
                       C_NO_ERR);
            //Change to TX
            if (rc_CurMessageId.q_MessageIsTx == true)
            {
               const C_OSCCanMessageContainer * const pc_Container =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(rc_CurMessageId.u32_NodeIndex,
                                                                                  this->mc_LastMessageId.e_ComProtocol,
                                                                                  rc_CurMessageId.u32_InterfaceIndex);
               if ((pc_Container != NULL) && (pc_Container->c_RxMessages.size() > 0UL))
               {
                  //Should be the newest RX message
                  const C_OSCCanMessageIdentificationIndices c_Tmp(rc_CurMessageId.u32_NodeIndex,
                                                                   this->mc_LastMessageId.e_ComProtocol,
                                                                   rc_CurMessageId.u32_InterfaceIndex, false,
                                                                   pc_Container->c_RxMessages.size() - 1UL);
                  tgl_assert(this->mpc_MessageSyncManager->SetCanMessageDirection(c_Tmp, true) ==
                             C_NO_ERR);
               }
            }
         }
      }
      //Update unique ID
      this->mu64_UniqueId = this->mpc_MessageSyncManager->GetUniqueId(this->mc_LastMessageId);
   }
   if (this->mpc_MessageTreeWidget != NULL)
   {
      this->mpc_MessageTreeWidget->InternalAddMessage(this->mc_LastMessageId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generic delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddDeleteBaseCommand::Delete(void)
{
   m_Store();
   m_Remove();
   if ((this->mpc_MessageTreeWidget != NULL) && (this->mpc_MessageSyncManager != NULL))
   {
      //At this point we can't get the message ID by unique ID because it was already deleted
      // but this should be no problem as we do always remember the message ID anyways
      this->mpc_MessageTreeWidget->InternalDeleteMessage(this->mc_LastMessageId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store current values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddDeleteBaseCommand::m_Store(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      this->mc_LastMessageId = this->mpc_MessageSyncManager->GetMessageIdForUniqueId(this->mu64_UniqueId);
      tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanMessageComplete(this->mpc_MessageSyncManager->
                                                                        GetMessageIdForUniqueId(this->
                                                                                                mu64_UniqueId),
                                                                        this->mc_Message,
                                                                        this->mc_OSCSignalCommons,
                                                                        this->mc_UISignalCommons,
                                                                        this->mc_UISignals) == C_NO_ERR);
      this->mc_MatchingIds = this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mpc_MessageSyncManager->
                                                                                    GetMessageIdForUniqueId(this->
                                                                                                            mu64_UniqueId));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove current message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddDeleteBaseCommand::m_Remove(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      tgl_assert(this->mpc_MessageSyncManager->DeleteCanMessage(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                   this->
                                                                   mu64_UniqueId)) ==
                 C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Debug function to check if UI part still in sync

   \param[in] orc_MessageId Message to check for
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdBueUnoMessageAddDeleteBaseCommand::mh_CheckSync(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_MessageId.u32_NodeIndex);
   const C_PuiSdNode * const pc_UiNode = C_PuiSdHandler::h_GetInstance()->GetUINode(orc_MessageId.u32_NodeIndex);

   if ((pc_OSCNode != NULL) && (pc_UiNode != NULL))
   {
      if (pc_OSCNode->c_DataPools.size() == pc_UiNode->c_UIDataPools.size())
      {
         const C_OSCNodeDataPool * const pc_OSCDataPool = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPool(
            orc_MessageId.u32_NodeIndex, orc_MessageId.e_ComProtocol);
         const C_PuiSdNodeDataPool * const pc_UiDataPool = C_PuiSdHandler::h_GetInstance()->GetUiCanDataPool(
            orc_MessageId.u32_NodeIndex, orc_MessageId.e_ComProtocol);
         if ((pc_OSCDataPool != NULL) && (pc_UiDataPool != NULL))
         {
            if (pc_OSCNode->c_ComProtocols.size() == pc_UiNode->c_UICanProtocols.size())
            {
               const C_OSCCanProtocol * const pc_OSCProtocol = C_PuiSdHandler::h_GetInstance()->GetCanProtocol(
                  orc_MessageId.u32_NodeIndex,
                  orc_MessageId.e_ComProtocol);
               const C_PuiSdNodeCanProtocol * const pc_UiProtocol =
                  C_PuiSdHandler::h_GetInstance()->GetUiCanProtocolConst(orc_MessageId.u32_NodeIndex,
                                                                         orc_MessageId.e_ComProtocol);
               if ((pc_OSCProtocol != NULL) && (pc_UiProtocol != NULL))
               {
                  if (pc_OSCProtocol->c_ComMessages.size() == pc_UiProtocol->c_ComMessages.size())
                  {
                     if (orc_MessageId.u32_InterfaceIndex < pc_OSCProtocol->c_ComMessages.size())
                     {
                        const C_OSCCanMessageContainer & rc_OSCContainer =
                           pc_OSCProtocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
                        const C_PuiSdNodeCanMessageContainer & rc_UiContainer =
                           pc_UiProtocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
                        if ((rc_OSCContainer.c_RxMessages.size() == rc_UiContainer.c_RxMessages.size()) &&
                            (rc_OSCContainer.c_TxMessages.size() == rc_UiContainer.c_TxMessages.size()))
                        {
                           uint32 u32_SumRxSi = 0;
                           uint32 u32_SumTxSi = 0;
                           uint32 u32_TxListIndex;
                           uint32 u32_RxListIndex;
                           for (uint32 u32_ItTxM = 0; u32_ItTxM < rc_OSCContainer.c_TxMessages.size(); ++u32_ItTxM)
                           {
                              const C_OSCCanMessage & rc_CurOSCMessage = rc_OSCContainer.c_TxMessages[u32_ItTxM];
                              const C_PuiSdNodeCanMessage & rc_CurUiMessage = rc_UiContainer.c_TxMessages[u32_ItTxM];
                              u32_SumTxSi += rc_CurOSCMessage.c_Signals.size();
                              if (rc_CurOSCMessage.c_Signals.size() != rc_CurUiMessage.c_Signals.size())
                              {
                                 s32_Retval = C_CONFIG;
                              }
                           }
                           for (uint32 u32_ItRxM = 0; u32_ItRxM < rc_OSCContainer.c_RxMessages.size(); ++u32_ItRxM)
                           {
                              const C_OSCCanMessage & rc_CurOSCMessage = rc_OSCContainer.c_RxMessages[u32_ItRxM];
                              const C_PuiSdNodeCanMessage & rc_CurUiMessage = rc_UiContainer.c_RxMessages[u32_ItRxM];
                              u32_SumRxSi += rc_CurOSCMessage.c_Signals.size();
                              if (rc_CurOSCMessage.c_Signals.size() != rc_CurUiMessage.c_Signals.size())
                              {
                                 s32_Retval = C_CONFIG;
                              }
                           }
                           if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                                   true, u32_TxListIndex) == C_NO_ERR)
                           {
                              if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                                      false, u32_RxListIndex) == C_NO_ERR)
                              {
                                 if ((u32_TxListIndex < pc_OSCDataPool->c_Lists.size()) &&
                                     (u32_RxListIndex < pc_OSCDataPool->c_Lists.size()))
                                 {
                                    const C_OSCNodeDataPoolList & rc_OSCTxList =
                                       pc_OSCDataPool->c_Lists[u32_TxListIndex];
                                    const C_OSCNodeDataPoolList & rc_OSCRxList =
                                       pc_OSCDataPool->c_Lists[u32_RxListIndex];
                                    const C_PuiSdNodeDataPoolList & rc_UiTxList =
                                       pc_UiDataPool->c_DataPoolLists[u32_TxListIndex];
                                    const C_PuiSdNodeDataPoolList & rc_UiRxList =
                                       pc_UiDataPool->c_DataPoolLists[u32_RxListIndex];
                                    if ((rc_OSCTxList.c_Elements.size() == rc_UiTxList.c_DataPoolListElements.size()) &&
                                        (rc_OSCRxList.c_Elements.size() == rc_UiRxList.c_DataPoolListElements.size()))
                                    {
                                       if ((rc_OSCTxList.c_Elements.size() == u32_SumTxSi) &&
                                           (rc_OSCRxList.c_Elements.size() == u32_SumRxSi))
                                       {
                                          //All fine
                                       }
                                       else
                                       {
                                          s32_Retval = C_CONFIG;
                                       }
                                    }
                                    else
                                    {
                                       s32_Retval = C_CONFIG;
                                    }
                                 }
                                 else
                                 {
                                    s32_Retval = C_CONFIG;
                                 }
                              }
                              else
                              {
                                 s32_Retval = C_CONFIG;
                              }
                           }
                           else
                           {
                              s32_Retval = C_CONFIG;
                           }
                        }
                        else
                        {
                           s32_Retval = C_CONFIG;
                        }
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}
