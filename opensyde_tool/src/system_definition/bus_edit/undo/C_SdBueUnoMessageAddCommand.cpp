//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message add undo command (implementation)

   Message add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiSdHandler.h"
#include "C_SdBueUnoMessageAddCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
   \param[in,out] opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoMessageAddCommand::C_SdBueUnoMessageAddCommand(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                         C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager, C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
                                                         QUndoCommand * const opc_Parent) :
   C_SdBueUnoMessageAddDeleteBaseCommand(orc_MessageId, opc_MessageSyncManager, opc_MessageTreeWidget, "Add Message",
                                         opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set initial data

   \param[in] orc_Message                 Message data
   \param[in] orc_OSCSignalCommons        Signals data (osc common)
   \param[in] orc_UISignalCommons         Signals data (ui common)
   \param[in] orc_UISignals               Signals data (ui)
   \param[in] orc_OwnerNodeName           Owner node names
   \param[in] orc_OwnerNodeInterfaceIndex Owner node interface index
   \param[in] orc_OwnerNodeDatapoolIndex  Owner node Datapool index
   \param[in] orc_OwnerIsTxFlag           Owner has message as Tx flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddCommand::SetInitialData(const C_OSCCanMessage & orc_Message,
                                                 const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                                 const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals, const std::vector<QString>  & orc_OwnerNodeName, const std::vector<stw_types::uint32>  & orc_OwnerNodeInterfaceIndex, const std::vector<stw_types::uint32> & orc_OwnerNodeDatapoolIndex,
                                                 const std::vector<bool>  & orc_OwnerIsTxFlag)
{
   this->mc_Message = orc_Message;
   this->mc_OSCSignalCommons = orc_OSCSignalCommons;
   this->mc_UISignalCommons = orc_UISignalCommons;
   this->mc_UISignals = orc_UISignals;
   //Convert to internal structure
   if ((orc_OwnerNodeName.size() == orc_OwnerNodeInterfaceIndex.size()) &&
       (orc_OwnerNodeName.size() == orc_OwnerNodeDatapoolIndex.size()) &&
       (orc_OwnerNodeName.size() == orc_OwnerIsTxFlag.size()) &&
       (orc_OwnerNodeName.size() > 0UL))
   {
      bool q_First = true;
      for (stw_types::uint32 u32_ItOwner = 0; u32_ItOwner < orc_OwnerNodeName.size(); ++u32_ItOwner)
      {
         bool q_Found = false;
         const QString & rc_CurName = orc_OwnerNodeName[u32_ItOwner];
         C_OSCCanMessageIdentificationIndices c_CurId;
         //Search node index
         for (stw_types::uint32 u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize();
              ++u32_ItNode)
         {
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_ItNode);
            if ((pc_Node != NULL) && (pc_Node->c_Properties.c_Name == rc_CurName.toStdString().c_str()))
            {
               const C_OSCCanMessageContainer * const pc_Container =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
                     u32_ItNode,
                     this->mc_LastMessageId.e_ComProtocol,
                     orc_OwnerNodeInterfaceIndex[u32_ItOwner],
                     orc_OwnerNodeDatapoolIndex[u32_ItOwner]);

               c_CurId.u32_NodeIndex = u32_ItNode;
               //Check if valid
               if (pc_Container != NULL)
               {
                  q_Found = true;
                  //There actually could be better matches in other nodes if this one does not work in any way
                  break;
               }
            }
         }
         if (q_Found == true)
         {
            //Assign other values
            c_CurId.e_ComProtocol = this->mc_LastMessageId.e_ComProtocol;
            c_CurId.u32_InterfaceIndex = orc_OwnerNodeInterfaceIndex[u32_ItOwner];
            c_CurId.u32_DatapoolIndex = orc_OwnerNodeDatapoolIndex[u32_ItOwner];
            c_CurId.q_MessageIsTx = orc_OwnerIsTxFlag[u32_ItOwner];
            //First one has to replace default message ID
            if (q_First == true)
            {
               q_First = false;
               this->mc_LastMessageId = c_CurId;
            }
            else
            {
               bool q_AlreadyAdded = false;
               //Only add if unique (might prevent strange behavior)
               if ((this->mc_LastMessageId.u32_NodeIndex == c_CurId.u32_NodeIndex) &&
                   (this->mc_LastMessageId.u32_DatapoolIndex == c_CurId.u32_DatapoolIndex) &&
                   (this->mc_LastMessageId.u32_InterfaceIndex == c_CurId.u32_InterfaceIndex))
               {
                  q_AlreadyAdded = true;
               }
               else
               {
                  for (stw_types::uint32 u32_ItId = 0; u32_ItId < this->mc_MatchingIds.size(); ++u32_ItId)
                  {
                     const C_OSCCanMessageIdentificationIndices & rc_CurId = this->mc_MatchingIds[u32_ItId];
                     if ((rc_CurId.u32_NodeIndex == c_CurId.u32_NodeIndex) &&
                         (rc_CurId.u32_DatapoolIndex == c_CurId.u32_DatapoolIndex) &&
                         (rc_CurId.u32_InterfaceIndex == c_CurId.u32_InterfaceIndex))
                     {
                        q_AlreadyAdded = true;
                        break;
                     }
                  }
               }
               if (q_AlreadyAdded == false)
               {
                  this->mc_MatchingIds.push_back(c_CurId);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the last known value for the message index

   Warning: use this function carefully as the message index cannot be relied on

   \return
   Last known value for the message index
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanMessageIdentificationIndices C_SdBueUnoMessageAddCommand::GetLastMessageId(void) const
{
   return this->mc_LastMessageId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddCommand::redo(void)
{
   this->Add();
   C_SdBueUnoMessageAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddCommand::undo(void)
{
   C_SdBueUnoMessageAddDeleteBaseCommand::undo();
   this->Delete();
}
