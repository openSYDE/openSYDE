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
                                                             this->mc_LastMessageId.u32_DatapoolIndex,
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
             (rc_CurMessageId.u32_InterfaceIndex != this->mc_LastMessageId.u32_InterfaceIndex) ||
             (rc_CurMessageId.u32_DatapoolIndex != this->mc_LastMessageId.u32_DatapoolIndex))
         {
            //Always add as Rx
            tgl_assert(this->mpc_MessageSyncManager->AddCanMessageRx(this->mc_LastMessageId,
                                                                     rc_CurMessageId.u32_NodeIndex,
                                                                     rc_CurMessageId.u32_InterfaceIndex,
                                                                     rc_CurMessageId.u32_DatapoolIndex) ==
                       C_NO_ERR);
            //Change to Tx
            if (rc_CurMessageId.q_MessageIsTx == true)
            {
               const C_OSCCanMessageContainer * const pc_Container =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(rc_CurMessageId.u32_NodeIndex,
                                                                                  this->mc_LastMessageId.e_ComProtocol,
                                                                                  rc_CurMessageId.u32_InterfaceIndex,
                                                                                  rc_CurMessageId.u32_DatapoolIndex);

               if ((pc_Container != NULL) && (pc_Container->c_RxMessages.size() > 0UL))
               {
                  //Should be the newest Rx message
                  const C_OSCCanMessageIdentificationIndices c_Tmp(rc_CurMessageId.u32_NodeIndex,
                                                                   this->mc_LastMessageId.e_ComProtocol,
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
