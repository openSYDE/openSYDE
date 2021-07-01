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

   \param[in]      orc_MessageId             Message identification indices
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
   \param[in]      orc_Text                  Optional command text for informational display
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoMessageAddDeleteBaseCommand::C_SdBueUnoMessageAddDeleteBaseCommand(
   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageId,
   C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text,
   QUndoCommand * const opc_Parent) :
   C_SdBueUnoMessageBaseCommand(orc_MessageId, opc_MessageSyncManager, opc_MessageTreeWidget, orc_Text, opc_Parent),
   mc_LastMessageId(orc_MessageId)
{
   this->mc_MatchingIds.resize(this->mc_UniqueId.size(),
                               std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices>());
   this->mc_LastMessageId.resize(this->mc_UniqueId.size(), C_OSCCanMessageIdentificationIndices());
   this->mc_Message.resize(this->mc_UniqueId.size(), C_OSCCanMessage());
   this->mc_OSCSignalCommons.resize(this->mc_UniqueId.size(),
                                    std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement>());
   this->mc_UISignalCommons.resize(this->mc_UniqueId.size(), std::vector<C_PuiSdNodeDataPoolListElement>());
   this->mc_UISignals.resize(this->mc_UniqueId.size(), std::vector<C_PuiSdNodeCanSignal>());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generic add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddDeleteBaseCommand::m_Add(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      for (uint32 u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
      {
         //Adapt default values for protocol
         if (this->mc_LastMessageId[u32_ItStep].e_ComProtocol == C_OSCCanProtocol::eECES)
         {
            //Tx method
            this->mc_Message[u32_ItStep].e_TxMethod = C_OSCCanMessage::eTX_METHOD_CYCLIC;
         }
         else if (this->mc_LastMessageId[u32_ItStep].e_ComProtocol == C_OSCCanProtocol::eCAN_OPEN_SAFETY)
         {
            //Message id
            this->mc_Message[u32_ItStep].q_IsExtended = false;
            if ((this->mc_Message[u32_ItStep].u32_CanId % 2U) == 0U)
            {
               this->mc_Message[u32_ItStep].u32_CanId = this->mc_Message[u32_ItStep].u32_CanId - 1U;
            }
            if (this->mc_Message[u32_ItStep].u32_CanId < mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN)
            {
               this->mc_Message[u32_ItStep].u32_CanId = mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN;
            }
            if (this->mc_Message[u32_ItStep].u32_CanId > mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX)
            {
               this->mc_Message[u32_ItStep].u32_CanId = mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX;
            }
            //Tx method
            this->mc_Message[u32_ItStep].e_TxMethod = C_OSCCanMessage::eTX_METHOD_CYCLIC;
            //Dlc
            this->mc_Message[u32_ItStep].u16_Dlc = 8U;
         }
         else
         {
            //Layer 2 -> all
         }
         //Cycle time
         if (this->mc_Message[u32_ItStep].u32_CycleTimeMs == 0U)
         {
            this->mc_Message[u32_ItStep].u32_CycleTimeMs = 1U;
         }
         tgl_assert(this->mpc_MessageSyncManager->AddCanMessage(this->mc_LastMessageId[u32_ItStep].u32_NodeIndex,
                                                                this->mc_LastMessageId[u32_ItStep].e_ComProtocol,
                                                                this->mc_LastMessageId[u32_ItStep].u32_InterfaceIndex,
                                                                this->mc_LastMessageId[u32_ItStep].u32_DatapoolIndex,
                                                                this->mc_LastMessageId[u32_ItStep].q_MessageIsTx,
                                                                this->mc_Message[u32_ItStep],
                                                                this->mc_OSCSignalCommons[u32_ItStep],
                                                                this->mc_UISignalCommons[u32_ItStep],
                                                                this->mc_UISignals[u32_ItStep],
                                                                this->mc_LastMessageId[u32_ItStep].u32_MessageIndex) ==
                    C_NO_ERR);
         for (uint32 u32_ItMessage = 0U; u32_ItMessage < this->mc_MatchingIds[u32_ItStep].size(); ++u32_ItMessage)
         {
            const C_OSCCanMessageIdentificationIndices & rc_CurMessageId =
               this->mc_MatchingIds[u32_ItStep][u32_ItMessage];
            //Skip already added one
            if ((rc_CurMessageId.u32_NodeIndex != this->mc_LastMessageId[u32_ItStep].u32_NodeIndex) ||
                (rc_CurMessageId.u32_InterfaceIndex != this->mc_LastMessageId[u32_ItStep].u32_InterfaceIndex) ||
                (rc_CurMessageId.u32_DatapoolIndex != this->mc_LastMessageId[u32_ItStep].u32_DatapoolIndex))
            {
               //Always add as Rx
               tgl_assert(this->mpc_MessageSyncManager->AddCanMessageRx(this->mc_LastMessageId[u32_ItStep],
                                                                        rc_CurMessageId.u32_NodeIndex,
                                                                        rc_CurMessageId.u32_InterfaceIndex,
                                                                        rc_CurMessageId.u32_DatapoolIndex) ==
                          C_NO_ERR);
               //Change to Tx
               if (rc_CurMessageId.q_MessageIsTx == true)
               {
                  const C_OSCCanMessageContainer * const pc_Container =
                     C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
                        rc_CurMessageId.u32_NodeIndex,
                        this->mc_LastMessageId[u32_ItStep].e_ComProtocol,
                        rc_CurMessageId.u32_InterfaceIndex,
                        rc_CurMessageId.u32_DatapoolIndex);

                  if ((pc_Container != NULL) && (pc_Container->c_RxMessages.size() > 0UL))
                  {
                     //Should be the newest Rx message
                     const C_OSCCanMessageIdentificationIndices c_Tmp(rc_CurMessageId.u32_NodeIndex,
                                                                      this->mc_LastMessageId[u32_ItStep].e_ComProtocol,
                                                                      rc_CurMessageId.u32_InterfaceIndex,
                                                                      rc_CurMessageId.u32_DatapoolIndex,
                                                                      false,
                                                                      pc_Container->c_RxMessages.size() - 1UL);
                     tgl_assert(this->mpc_MessageSyncManager->SetCanMessageDirection(c_Tmp, true) ==
                                C_NO_ERR);
                  }
               }
            }
         }
         //Update unique ID
         this->mc_UniqueId[u32_ItStep] = this->mpc_MessageSyncManager->GetUniqueId(this->mc_LastMessageId[u32_ItStep]);
         //UI
         if (this->mpc_MessageTreeWidget != NULL)
         {
            this->mpc_MessageTreeWidget->InternalAddMessage(this->mc_LastMessageId[u32_ItStep]);
         }
      }
      //UI
      if (this->mpc_MessageTreeWidget != NULL)
      {
         this->mpc_MessageTreeWidget->InternalAddMessageCommit();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generic delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddDeleteBaseCommand::m_Delete(void)
{
   m_Store();
   m_Remove();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store current values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddDeleteBaseCommand::m_Store(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      for (uint32 u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
      {
         {
            const C_OSCCanMessageIdentificationIndices c_LastMessageId =
               this->mpc_MessageSyncManager->GetMessageIdForUniqueId(this->mc_UniqueId[u32_ItStep]);
            if (u32_ItStep < this->mc_LastMessageId.size())
            {
               this->mc_LastMessageId[u32_ItStep] = c_LastMessageId;
            }
            else
            {
               this->mc_LastMessageId.push_back(c_LastMessageId);
            }
         }
         {
            C_OSCCanMessage c_Message;
            std::vector<C_OSCNodeDataPoolListElement> c_OSCSignalCommons;
            std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommons;
            std::vector<C_PuiSdNodeCanSignal> c_UISignal;
            tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanMessageComplete(this->mpc_MessageSyncManager->
                                                                              GetMessageIdForUniqueId(this->
                                                                                                      mc_UniqueId[
                                                                                                         u32_ItStep]),
                                                                              c_Message,
                                                                              c_OSCSignalCommons, c_UISignalCommons,
                                                                              c_UISignal) ==
                       C_NO_ERR);
            this->mc_Message[u32_ItStep] = c_Message;
            this->mc_OSCSignalCommons[u32_ItStep] = c_OSCSignalCommons;
            this->mc_UISignalCommons[u32_ItStep] = c_UISignalCommons;
            this->mc_UISignals[u32_ItStep] = c_UISignal;
         }
         {
            const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingIds =
               this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mpc_MessageSyncManager->
                                                                      GetMessageIdForUniqueId(
                                                                         this->
                                                                         mc_UniqueId
                                                                         [u32_ItStep]));
            this->mc_MatchingIds[u32_ItStep] = c_MatchingIds;
         }
      }
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
      uint32 u32_InternalMessageIndex = 0UL;
      for (uint32 u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
      {
         tgl_assert(this->mpc_MessageSyncManager->DeleteCanMessage(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                      this->
                                                                      mc_UniqueId[u32_ItStep])) ==
                    C_NO_ERR);
         if (this->mpc_MessageTreeWidget != NULL)
         {
            //At this point we can't get the message ID by unique ID because it was already deleted
            // but this should be no problem as we do always remember the message ID anyways
            u32_InternalMessageIndex =
               this->mpc_MessageTreeWidget->InternalDeleteMessage(this->mc_LastMessageId[u32_ItStep]);
         }
      }
      if (this->mpc_MessageTreeWidget != NULL)
      {
         //At this point we can't get the message ID by unique ID because it was already deleted
         // but this should be no problem as we do always remember the message ID anyways
         this->mpc_MessageTreeWidget->InternalDeleteMessageCommit(u32_InternalMessageIndex);
      }
   }
}
