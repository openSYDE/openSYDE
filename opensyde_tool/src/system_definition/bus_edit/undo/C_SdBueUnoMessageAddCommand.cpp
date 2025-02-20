//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message add undo command (implementation)

   Message add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdBueUnoMessageAddCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoMessageAddCommand::C_SdBueUnoMessageAddCommand(
   const std::vector<C_OscCanMessageIdentificationIndices> & orc_MessageId,
   C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text) :
   C_SdBueUnoMessageAddDeleteBaseCommand(orc_MessageId, opc_MessageSyncManager, opc_MessageTreeWidget,
                                         orc_Text)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set initial data

   \param[in]   orc_Message                  Message data
   \param[in]   orc_OscSignalCommons         Signals data (osc common)
   \param[in]   orc_UiSignalCommons          Signals data (ui common)
   \param[out]  orc_UiMessages               Message ui data
   \param[in]   orc_OwnerNodeName            Owner node names
   \param[in]   orc_OwnerNodeInterfaceIndex  Owner node interface index
   \param[in]   orc_OwnerNodeDatapoolIndex   Owner node Datapool index
   \param[in]   orc_OwnerIsTxFlag            Owner has message as Tx flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddCommand::SetInitialData(const std::vector<C_OscCanMessage> & orc_Message,
                                                 const std::vector<std::vector<C_OscNodeDataPoolListElement> > & orc_OscSignalCommons, const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UiSignalCommons, const std::vector<C_PuiSdNodeCanMessage> & orc_UiMessages, const std::vector<std::vector<QString> > & orc_OwnerNodeName, const std::vector<std::vector<uint32_t> > & orc_OwnerNodeInterfaceIndex, const std::vector<std::vector<uint32_t> > & orc_OwnerNodeDatapoolIndex,
                                                 const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag)
{
   this->mc_Message = orc_Message;
   this->mc_OscSignalCommons = orc_OscSignalCommons;
   this->mc_UiSignalCommons = orc_UiSignalCommons;
   this->mc_UiMessage = orc_UiMessages;
   for (uint32_t u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
   {
      //Convert to internal structure
      if ((((orc_OwnerNodeName.size() == orc_OwnerNodeInterfaceIndex.size()) &&
            (orc_OwnerNodeName.size() == orc_OwnerNodeDatapoolIndex.size())) &&
           (orc_OwnerNodeName.size() == orc_OwnerIsTxFlag.size())) &&
          (u32_ItStep < orc_OwnerNodeName.size()))
      {
         if ((orc_OwnerNodeName[u32_ItStep].size() == orc_OwnerNodeInterfaceIndex[u32_ItStep].size()) &&
             (orc_OwnerNodeName[u32_ItStep].size() == orc_OwnerNodeDatapoolIndex[u32_ItStep].size()) &&
             (orc_OwnerNodeName[u32_ItStep].size() == orc_OwnerIsTxFlag[u32_ItStep].size()) &&
             (orc_OwnerNodeName[u32_ItStep].size() > 0UL))
         {
            bool q_First = true;
            for (uint32_t u32_ItOwner = 0; u32_ItOwner < orc_OwnerNodeName[u32_ItStep].size(); ++u32_ItOwner)
            {
               bool q_Found = false;
               const QString & rc_CurName = orc_OwnerNodeName[u32_ItStep][u32_ItOwner];
               C_OscCanMessageIdentificationIndices c_CurId;
               //Search node index
               for (uint32_t u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize();
                    ++u32_ItNode)
               {
                  const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ItNode);
                  if ((pc_Node != NULL) && (pc_Node->c_Properties.c_Name == rc_CurName.toStdString().c_str()))
                  {
                     const C_OscCanMessageContainer * const pc_Container =
                        C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
                           u32_ItNode,
                           this->mc_LastMessageId[u32_ItStep].e_ComProtocol,
                           orc_OwnerNodeInterfaceIndex[u32_ItStep][u32_ItOwner],
                           orc_OwnerNodeDatapoolIndex[u32_ItStep][u32_ItOwner]);

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
                  c_CurId.e_ComProtocol = this->mc_LastMessageId[u32_ItStep].e_ComProtocol;
                  c_CurId.u32_InterfaceIndex = orc_OwnerNodeInterfaceIndex[u32_ItStep][u32_ItOwner];
                  c_CurId.u32_DatapoolIndex = orc_OwnerNodeDatapoolIndex[u32_ItStep][u32_ItOwner];
                  c_CurId.q_MessageIsTx = orc_OwnerIsTxFlag[u32_ItStep][u32_ItOwner];
                  //First one has to replace default message ID
                  if (q_First == true)
                  {
                     q_First = false;
                     this->mc_LastMessageId[u32_ItStep] = c_CurId;
                  }
                  else
                  {
                     bool q_AlreadyAdded = false;
                     //Only add if unique (might prevent strange behavior)
                     if ((this->mc_LastMessageId[u32_ItStep].u32_NodeIndex == c_CurId.u32_NodeIndex) &&
                         (this->mc_LastMessageId[u32_ItStep].u32_DatapoolIndex == c_CurId.u32_DatapoolIndex) &&
                         (this->mc_LastMessageId[u32_ItStep].u32_InterfaceIndex == c_CurId.u32_InterfaceIndex))
                     {
                        q_AlreadyAdded = true;
                     }
                     else
                     {
                        for (uint32_t u32_ItId = 0; u32_ItId < this->mc_MatchingIds[u32_ItStep].size();
                             ++u32_ItId)
                        {
                           const C_OscCanMessageIdentificationIndices & rc_CurId =
                              this->mc_MatchingIds[u32_ItStep][u32_ItId];
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
                        this->mc_MatchingIds[u32_ItStep].push_back(c_CurId);
                     }
                  }
               }
            }
         }
      }
   }
   tgl_assert(this->m_CheckMessagesSortedAscending());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the last known value for the message index

   Warning: use this function carefully as the message index cannot be relied on

   \return
   Last known value for the message index
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OscCanMessageIdentificationIndices> C_SdBueUnoMessageAddCommand::GetLastMessageIds(void) const
{
   return this->mc_LastMessageId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddCommand::redo(void)
{
   this->m_Add();
   C_SdBueUnoMessageAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoMessageAddCommand::undo(void)
{
   C_SdBueUnoMessageAddDeleteBaseCommand::undo();
   this->m_Delete();
}
