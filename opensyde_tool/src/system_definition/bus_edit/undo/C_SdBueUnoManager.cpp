//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Message and signal undo command stack (implementation)

   Message and signal undo command stack

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdBueUnoManager.h"
#include "C_SdBueUnoBusProtNodeConnectAndCreateCommand.h"
#include "C_SdBueUnoBusProtNodeConnectCommand.h"
#include "C_SdBueUnoBusProtNodeDisconnectCommand.h"
#include "C_SdBueUnoBusProtNodeCreateCommand.h"
#include "C_SdBueMessageSelectorTreeWidget.h"
#include "C_SdBueUnoMessageAddCommand.h"
#include "C_SdBueUnoMessageDeleteCommand.h"
#include "C_SdBueUnoSignalAddCommand.h"
#include "C_SdBueUnoSignalDeleteCommand.h"
#include "C_SdBueUnoSignalMoveCommand.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdBueUnoManager::C_SdBueUnoManager(QObject * const opc_Parent) :
   C_UtiUndoStack(opc_Parent),
   mpc_DeleteCommand(NULL)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     24.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdBueUnoManager::~C_SdBueUnoManager(void)
{
   if (this->mpc_DeleteCommand != NULL)
   {
      delete (mpc_DeleteCommand);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do connect node to protocol

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes

   \created     21.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoConnectNodeToProt(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                            const C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeConnectCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol, opc_Widget));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do connect node to protocol and create data pool

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes

   \created     21.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoConnectNodeToProtAndAddDataPool(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                          const C_OSCCanProtocol::E_Type oe_Protocol,
                                                          QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeConnectAndCreateCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol,
                                                                 opc_Widget));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do disconnect node from protocol

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes

   \created     21.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoDisconnectNodeFromProt(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                 const C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeDisconnectCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol,
                                                           opc_Widget));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do add datapool to protocol

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes

   \created     14.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoAddDataPool(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                      const C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeCreateCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol, opc_Widget));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do add message

   \param[in]     orc_MessageId          Message identification indices
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoAddMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                     C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                     QTreeWidget * const opc_MessageTreeWidget)
{
   C_OSCCanMessage c_Message;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
      dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
   C_SdBueUnoMessageAddCommand * const pc_UndoCommand = new C_SdBueUnoMessageAddCommand(orc_MessageId,
                                                                                        opc_MessageSyncManager,
                                                                                        pc_MessageTreeWidget);

   mh_PatchMessageId(opc_MessageSyncManager, c_Message);
   pc_UndoCommand->SetInitialData(c_Message, std::vector<C_OSCNodeDataPoolListElement>(),
                                  std::vector<C_PuiSdNodeDataPoolListElement>(),
                                  std::vector<C_PuiSdNodeCanSignal>(), std::vector<QString>(),
                                  std::vector<uint32>(), std::vector<bool>());

   this->DoPush(pc_UndoCommand);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do paste messages

   \param[in]     orc_MessageId               Message identification indices
   \param[in]     orc_Messages                Message data
   \param[in]     orc_OSCSignalCommons        Signal common osc data
   \param[in]     orc_UISignalCommons         Signal common ui data
   \param[in]     orc_UISignals               Signal ui data
   \param[in]     orc_OwnerNodeName           Owner node names
   \param[in]     orc_OwnerNodeInterfaceIndex Owner node interface index
   \param[in]     orc_OwnerIsTxFlag           Owner has message as TX flags
   \param[in,out] opc_MessageSyncManager      Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget       Message tree widget to perform actions on

   \created     24.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoPasteMessages(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const std::vector<C_OSCCanMessage> & orc_Messages,
                                        const std::vector<std::vector<C_OSCNodeDataPoolListElement> > & orc_OSCSignalCommons, const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UISignalCommons, const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UISignals, const std::vector<std::vector<QString> > & orc_OwnerNodeName, const std::vector<std::vector<uint32> > & orc_OwnerNodeInterfaceIndex, const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                        QTreeWidget * const opc_MessageTreeWidget)
{
   //Check if consistent size
   if ((((orc_Messages.size() == orc_OSCSignalCommons.size()) &&
         (orc_Messages.size() == orc_UISignalCommons.size()))) && (orc_Messages.size() == orc_UISignals.size()))
   {
      if (orc_Messages.size() > 0)
      {
         C_OSCCanMessageIdentificationIndices c_CurrentMessageId = orc_MessageId;
         QUndoCommand * const pc_PasteCommand = new QUndoCommand("Paste Messages");
         for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
         {
            C_OSCCanMessage c_Tmp = orc_Messages[u32_ItMessage];
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
               dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
            C_SdBueUnoMessageAddCommand * const pc_AddCommand = new C_SdBueUnoMessageAddCommand(c_CurrentMessageId,
                                                                                                opc_MessageSyncManager,
                                                                                                pc_MessageTreeWidget,
                                                                                                pc_PasteCommand);
            if (((u32_ItMessage < orc_OwnerNodeName.size()) && (u32_ItMessage < orc_OwnerNodeInterfaceIndex.size())) &&
                (u32_ItMessage < orc_OwnerIsTxFlag.size()))
            {
               pc_AddCommand->SetInitialData(c_Tmp, orc_OSCSignalCommons[u32_ItMessage],
                                             orc_UISignalCommons[u32_ItMessage], orc_UISignals[u32_ItMessage],
                                             orc_OwnerNodeName[u32_ItMessage],
                                             orc_OwnerNodeInterfaceIndex[u32_ItMessage],
                                             orc_OwnerIsTxFlag[u32_ItMessage]);
            }
            else
            {
               std::vector<QString> c_EmptyOwnerNodeName;
               std::vector<uint32>  c_EmptyOwnerNodeInterfaceIndex;
               std::vector<bool>  c_EmptyOwnerIsTxFlag;
               pc_AddCommand->SetInitialData(c_Tmp, orc_OSCSignalCommons[u32_ItMessage],
                                             orc_UISignalCommons[u32_ItMessage], orc_UISignals[u32_ItMessage],
                                             c_EmptyOwnerNodeName, c_EmptyOwnerNodeInterfaceIndex,
                                             c_EmptyOwnerIsTxFlag);
            }
            ++c_CurrentMessageId.u32_MessageIndex;
            //lint -e429 Qt command stack will take care of it
         }
         this->DoPush(pc_PasteCommand);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do paste signals

   \param[in]     orc_MessageId          Message identification indices
   \param[in]     orc_Signals            Signals data
   \param[in]     orc_OSCSignalCommons   Signal common osc data
   \param[in]     orc_UISignalCommons    Signal common ui data
   \param[in]     orc_UISignals          Signal ui data
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on

   \created     24.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoPasteSignals(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                       const uint32 & oru32_SignalIndex,
                                       const std::vector<C_OSCCanSignal> & orc_Signals,
                                       const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                       const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                       const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals,
                                       C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                       QTreeWidget * const opc_MessageTreeWidget)
{
   //Check if consitent size
   if ((((orc_Signals.size() == orc_OSCSignalCommons.size()) &&
         (orc_Signals.size() == orc_UISignalCommons.size()))) && (orc_Signals.size() == orc_UISignals.size()))
   {
      if (orc_Signals.size() > 0)
      {
         uint32 u32_CurrentSignalIndex = oru32_SignalIndex;
         QUndoCommand * const pc_PasteCommand = new QUndoCommand("Paste Signals");
         for (uint32 u32_ItSignal = 0; u32_ItSignal < orc_Signals.size(); ++u32_ItSignal)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
               dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
            C_SdBueUnoSignalAddCommand * const pc_AddCommand = new C_SdBueUnoSignalAddCommand(orc_MessageId,
                                                                                              u32_CurrentSignalIndex,
                                                                                              orc_Signals[u32_ItSignal].u16_ComBitStart,
                                                                                              opc_MessageSyncManager,
                                                                                              pc_MessageTreeWidget,
                                                                                              pc_PasteCommand);
            pc_AddCommand->SetInitialData(orc_Signals[u32_ItSignal], orc_OSCSignalCommons[u32_ItSignal],
                                          orc_UISignalCommons[u32_ItSignal], orc_UISignals[u32_ItSignal]);
            ++u32_CurrentSignalIndex;
         }
         this->DoPush(pc_PasteCommand);
         //lint -e429 Qt command stack will take care of it
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do delete message

   \param[in]     orc_MessageId          Message identification indices
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoDeleteMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                        QTreeWidget * const opc_MessageTreeWidget)
{
   QUndoCommand * const pc_Parent = this->m_GetDeleteCommand();

   if (pc_Parent != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
         dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
      new C_SdBueUnoMessageDeleteCommand(orc_MessageId, opc_MessageSyncManager, pc_MessageTreeWidget, pc_Parent);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do add signal

   \param[in]     orc_MessageId          Message identification indices
   \param[in]     oru32_SignalIndex      Signal index
   \param[in]     ou16_StartBit          Start bit of signal
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoAddSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                    const uint32 & oru32_SignalIndex, const uint16 ou16_StartBit,
                                    C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                    QTreeWidget * const opc_MessageTreeWidget)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
      dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);

   this->DoPush(new C_SdBueUnoSignalAddCommand(orc_MessageId, oru32_SignalIndex, ou16_StartBit, opc_MessageSyncManager,
                                               pc_MessageTreeWidget));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do delete signal

   \param[in]     orc_MessageId          Message identification indices
   \param[in]     oru32_SignalIndex      Signal index
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoDeleteSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                       const uint32 & oru32_SignalIndex,
                                       C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                       QTreeWidget * const opc_MessageTreeWidget)
{
   QUndoCommand * const pc_Parent = this->m_GetDeleteCommand();

   if (pc_Parent != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
         dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
      new C_SdBueUnoSignalDeleteCommand(orc_MessageId, oru32_SignalIndex, opc_MessageSyncManager, pc_MessageTreeWidget,
                                        pc_Parent);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do move signal

   \param[in]     orc_SourceMessageIds      Source message identification indices
   \param[in]     oru32_SourceSignalIndices Source signal index
   \param[in]     orc_TargetMessageIds      Target message identification indices
   \param[in]     oru32_TargetSignalIndices Target signal index
   \param[in,out] opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget     Message tree widget to perform actions on

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::DoMoveSignal(const std::vector<C_OSCCanMessageIdentificationIndices> & orc_SourceMessageIds,
                                     const std::vector<uint32> & oru32_SourceSignalIndices,
                                     const std::vector<C_OSCCanMessageIdentificationIndices> & orc_TargetMessageIds,
                                     const std::vector<uint32> & oru32_TargetSignalIndices,
                                     C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                     QTreeWidget * const opc_MessageTreeWidget)
{
   if (orc_SourceMessageIds.size() > 0)
   {
      if (((orc_SourceMessageIds.size() == oru32_SourceSignalIndices.size()) &&
           (orc_SourceMessageIds.size() == orc_TargetMessageIds.size())) &&
          (orc_SourceMessageIds.size() == oru32_TargetSignalIndices.size()))
      {
         std::vector<uint32> c_AdaptedSourceSignalIndices;
         QUndoCommand * const pc_MoveCommand = new QUndoCommand("Move Signals");
         for (uint32 u32_ItMove = 0; u32_ItMove < orc_SourceMessageIds.size(); ++u32_ItMove)
         {
            uint32 u32_CurSourceSignalIndex = oru32_SourceSignalIndices[u32_ItMove];
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
               dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
            //Adapt index to previously moved signals
            for (uint32 u32_ItAdapted = 0; u32_ItAdapted < c_AdaptedSourceSignalIndices.size(); ++u32_ItAdapted)
            {
               if (c_AdaptedSourceSignalIndices[u32_ItAdapted] <= oru32_SourceSignalIndices[u32_ItMove])
               {
                  if (u32_CurSourceSignalIndex != 0)
                  {
                     --u32_CurSourceSignalIndex;
                  }
               }
            }
            new C_SdBueUnoSignalMoveCommand(orc_SourceMessageIds[u32_ItMove], u32_CurSourceSignalIndex,
                                            orc_TargetMessageIds[u32_ItMove],
                                            oru32_TargetSignalIndices[u32_ItMove], opc_MessageSyncManager,
                                            pc_MessageTreeWidget, pc_MoveCommand);
            c_AdaptedSourceSignalIndices.push_back(u32_CurSourceSignalIndex);
         }
         this->DoPush(pc_MoveCommand);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Commit delete command to undo stack

   \created     24.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::CommitDelete(void)
{
   if (this->mpc_DeleteCommand != NULL)
   {
      this->DoPush(this->mpc_DeleteCommand);
      this->mpc_DeleteCommand = NULL;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get cureent delete command (created if necessary)

   \return
   Guaranteed pointer to current delete command

   \created     24.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QUndoCommand * C_SdBueUnoManager::m_GetDeleteCommand(void)
{
   if (this->mpc_DeleteCommand == NULL)
   {
      this->mpc_DeleteCommand = new QUndoCommand("Delete any items");
   }
   return this->mpc_DeleteCommand;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Attempt patch of message id

   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] orc_Message            Message to patch message id

   \created     09.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoManager::mh_PatchMessageId(const C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                          C_OSCCanMessage & orc_Message)
{
   if (opc_MessageSyncManager != NULL)
   {
      orc_Message.u32_CanId = opc_MessageSyncManager->GetNextValidMessageId(orc_Message.q_IsExtended);
   }
}
