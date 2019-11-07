//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message and signal undo command stack (implementation)

   Message and signal undo command stack

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_PuiSdHandler.h"
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

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoManager::C_SdBueUnoManager(QObject * const opc_Parent) :
   C_UtiUndoStack(opc_Parent),
   mpc_DeleteCommand(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoManager::~C_SdBueUnoManager(void)
{
   if (this->mpc_DeleteCommand != NULL)
   {
      delete (mpc_DeleteCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do connect node to protocol

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoConnectNodeToProt(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                            const C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeConnectCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol, opc_Widget));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do connect node to protocol and create data pool

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoConnectNodeToProtAndAddDataPool(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                          const C_OSCCanProtocol::E_Type oe_Protocol,
                                                          QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeConnectAndCreateCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol,
                                                                 opc_Widget));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do disconnect node from protocol

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoDisconnectNodeFromProt(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                 const C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeDisconnectCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol,
                                                           opc_Widget));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add datapool to protocol

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoAddDataPool(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                      const C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeCreateCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol, opc_Widget));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add message

   \param[in]     orc_MessageId          Message identification indices
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
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
                                  std::vector<uint32>(), std::vector<uint32>(), std::vector<bool>());

   this->DoPush(pc_UndoCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do paste messages

   \param[in]     orc_MessageId               Message identification indices
   \param[in]     orc_Messages                Message data
   \param[in]     orc_OSCSignalCommons        Signal common osc data
   \param[in]     orc_UISignalCommons         Signal common ui data
   \param[in]     orc_UISignals               Signal ui data
   \param[in]     orc_OwnerNodeName           Owner node names
   \param[in]     orc_OwnerNodeInterfaceIndex Owner node interface index
   \param[in]     orc_OwnerNodeDatapoolIndex  Owner node Datapool index
   \param[in]     orc_OwnerIsTxFlag           Owner has message as TX flags
   \param[in,out] opc_MessageSyncManager      Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget       Message tree widget to perform actions on
   \param[out]    orc_NewIds                  New pasted message IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoPasteMessages(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const std::vector<C_OSCCanMessage> & orc_Messages,
                                        const std::vector<std::vector<C_OSCNodeDataPoolListElement> > & orc_OSCSignalCommons, const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UISignalCommons, const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UISignals, const std::vector<std::vector<QString> > & orc_OwnerNodeName, const std::vector<std::vector<uint32> > & orc_OwnerNodeInterfaceIndex, const std::vector<std::vector<stw_types::uint32> > & orc_OwnerNodeDatapoolIndex, const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager, QTreeWidget * const opc_MessageTreeWidget,
                                        std::vector<C_OSCCanMessageIdentificationIndices> & orc_NewIds)
{
   //Check if consistent size
   if ((((orc_Messages.size() == orc_OSCSignalCommons.size()) &&
         (orc_Messages.size() == orc_UISignalCommons.size()))) && (orc_Messages.size() == orc_UISignals.size()))
   {
      if (orc_Messages.size() > 0)
      {
         std::vector<const C_SdBueUnoMessageAddCommand *> c_AddCommands;
         C_OSCCanMessageIdentificationIndices c_CurrentMessageId = orc_MessageId;
         QUndoCommand * const pc_PasteCommand = new QUndoCommand("Paste Messages");

         //Reserve
         orc_NewIds.reserve(orc_Messages.size());
         c_AddCommands.reserve(orc_Messages.size());

         //Create add command for each new message (for paste multiple can be added at once)
         for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
         {
            std::vector<QString> c_OwnerNodeName;
            std::vector<uint32>  c_OwnerNodeInterfaceIndex;
            std::vector<uint32>  c_OwnerNodeDatapoolIndex;
            std::vector<bool>  c_OwnerIsTxFlag;
            C_OSCCanMessage c_Tmp = orc_Messages[u32_ItMessage];
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
               dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
            C_SdBueUnoMessageAddCommand * const pc_AddCommand = new C_SdBueUnoMessageAddCommand(c_CurrentMessageId,
                                                                                                opc_MessageSyncManager,
                                                                                                pc_MessageTreeWidget,
                                                                                                pc_PasteCommand);
            //Check last owners storage valid
            if ((u32_ItMessage < orc_OwnerNodeName.size()) &&
                (u32_ItMessage < orc_OwnerNodeInterfaceIndex.size()) &&
                (u32_ItMessage < orc_OwnerNodeDatapoolIndex.size()) &&
                (u32_ItMessage < orc_OwnerIsTxFlag.size()))
            {
               C_SdBueUnoManager::mh_HandleLastOwnersValidation(orc_MessageId, orc_OwnerNodeName[u32_ItMessage],
                                                                orc_OwnerNodeInterfaceIndex[u32_ItMessage],
                                                                orc_OwnerNodeDatapoolIndex[u32_ItMessage],
                                                                orc_OwnerIsTxFlag[u32_ItMessage], c_OwnerNodeName,
                                                                c_OwnerNodeInterfaceIndex, c_OwnerNodeDatapoolIndex,
                                                                c_OwnerIsTxFlag);
            }
            //If vector are empty default behavior is used instead
            pc_AddCommand->SetInitialData(c_Tmp, orc_OSCSignalCommons[u32_ItMessage],
                                          orc_UISignalCommons[u32_ItMessage],
                                          orc_UISignals[u32_ItMessage],
                                          c_OwnerNodeName, c_OwnerNodeInterfaceIndex, c_OwnerNodeDatapoolIndex,
                                          c_OwnerIsTxFlag);

            c_AddCommands.push_back(pc_AddCommand);

            //The message index increment might be highly inaccurate but it should never be used anyways
            ++c_CurrentMessageId.u32_MessageIndex;

            //lint -e429 Qt command stack will take care of it
         }

         //Execute add
         this->DoPush(pc_PasteCommand);

         //AFTER add step -> at this point the message IDs and especially the message index should be accurate
         for (uint32 u32_ItMessage = 0; u32_ItMessage < c_AddCommands.size(); ++u32_ItMessage)
         {
            const C_SdBueUnoMessageAddCommand * const pc_AddCommand = c_AddCommands[u32_ItMessage];
            if (pc_AddCommand != NULL)
            {
               orc_NewIds.push_back(pc_AddCommand->GetLastMessageId());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do paste signals

   \param[in]     orc_MessageId          Message identification indices
   \param[in]     orc_Signals            Signals data
   \param[in]     orc_OSCSignalCommons   Signal common osc data
   \param[in]     orc_UISignalCommons    Signal common ui data
   \param[in]     orc_UISignals          Signal ui data
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
   \param[in]     oe_ProtocolType        Current active protocol to handle necessary adaptations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoPasteSignals(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                       const uint32 & oru32_SignalIndex,
                                       const std::vector<C_OSCCanSignal> & orc_Signals,
                                       const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                       const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                       const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals,
                                       C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                       QTreeWidget * const opc_MessageTreeWidget,
                                       const C_OSCCanProtocol::E_Type oe_ProtocolType)
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
                                                                                              C_OSCCanSignal::eMUX_DEFAULT, 0,
                                                                                              opc_MessageSyncManager,
                                                                                              pc_MessageTreeWidget,
                                                                                              pc_PasteCommand);
            pc_AddCommand->SetInitialData(orc_Signals[u32_ItSignal], orc_OSCSignalCommons[u32_ItSignal],
                                          orc_UISignalCommons[u32_ItSignal], orc_UISignals[u32_ItSignal],
                                          oe_ProtocolType);
            ++u32_CurrentSignalIndex;
         }
         this->DoPush(pc_PasteCommand);
         //lint -e429 Qt command stack will take care of it
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do delete message

   \param[in]     orc_MessageId          Message identification indices
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add signal

   \param[in]     orc_MessageId          Message identification indices
   \param[in]     oru32_SignalIndex      Signal index
   \param[in]     ou16_StartBit          Start bit of signal
   \param[in]     oe_MultiplexerType     Multiplexer signal type
   \param[in]     ou16_MultiplexerValue  Multiplexer value (only relevant if
                                         oe_MultiplexerType is eMUX_MULTIPLEXED_SIGNAL)
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoAddSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                    const uint32 & oru32_SignalIndex, const uint16 ou16_StartBit,
                                    const C_OSCCanSignal::E_MultiplexerType oe_MultiplexerType,
                                    const uint16 ou16_MultiplexerValue,
                                    C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                    QTreeWidget * const opc_MessageTreeWidget)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
      dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);

   this->DoPush(new C_SdBueUnoSignalAddCommand(orc_MessageId, oru32_SignalIndex, ou16_StartBit, oe_MultiplexerType,
                                               ou16_MultiplexerValue, opc_MessageSyncManager,
                                               pc_MessageTreeWidget));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do delete signal

   \param[in]     orc_MessageId          Message identification indices
   \param[in]     oru32_SignalIndex      Signal index
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do move signal

   \param[in]     orc_SourceMessageIds      Source message identification indices
   \param[in]     oru32_SourceSignalIndices Source signal index
   \param[in]     orc_TargetMessageIds      Target message identification indices
   \param[in]     oru32_TargetSignalIndices Target signal index
   \param[in,out] opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget     Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Commit delete command to undo stack
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::CommitDelete(void)
{
   if (this->mpc_DeleteCommand != NULL)
   {
      this->DoPush(this->mpc_DeleteCommand);
      this->mpc_DeleteCommand = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get cureent delete command (created if necessary)

   \return
   Guaranteed pointer to current delete command
*/
//----------------------------------------------------------------------------------------------------------------------
QUndoCommand * C_SdBueUnoManager::m_GetDeleteCommand(void)
{
   if (this->mpc_DeleteCommand == NULL)
   {
      this->mpc_DeleteCommand = new QUndoCommand("Delete any items");
   }
   return this->mpc_DeleteCommand;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Attempt patch of message id

   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] orc_Message            Message to patch message id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::mh_PatchMessageId(const C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                          C_OSCCanMessage & orc_Message)
{
   if (opc_MessageSyncManager != NULL)
   {
      orc_Message.u32_CanId = opc_MessageSyncManager->GetNextValidMessageId(orc_Message.q_IsExtended);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert last known owners as restored from copy operation to current connected nodes (as possible)

   \param[in]  orc_MessageId                   New message id from current bus to use as fallback
   \param[in]  orc_LastOwnerNodeName           Original owners node names
   \param[in]  orc_LastOwnerNodeInterfaceIndex Original owners interface indices
   \param[in]  orc_LastOwnerNodeDatapoolIndex  Original owners Datapool indices
   \param[in]  orc_LastOwnerIsTxFlag           Original owners TX flags
   \param[out] orc_NewOwnerNodeName            New/valid owners node names (empty if no matches found)
   \param[out] orc_NewOwnerNodeInterfaceIndex  New/valid owners interface indices (empty if no matches found)
   \param[out] orc_NewOwnerNodeDatapoolIndex   New/valid owners Datapool indices (empty if no matches found)
   \param[out] orc_NewOwnerIsTxFlag            New/valid owners TX flags (empty if no matches found)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::mh_HandleLastOwnersValidation(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                      const std::vector<QString> & orc_LastOwnerNodeName,
                                                      const std::vector<uint32> & orc_LastOwnerNodeInterfaceIndex,
                                                      const std::vector<uint32> & orc_LastOwnerNodeDatapoolIndex,
                                                      const std::vector<bool> & orc_LastOwnerIsTxFlag,
                                                      std::vector<QString> & orc_NewOwnerNodeName,
                                                      std::vector<uint32> & orc_NewOwnerNodeInterfaceIndex,
                                                      std::vector<uint32> & orc_NewOwnerNodeDatapoolIndex,
                                                      std::vector<bool> & orc_NewOwnerIsTxFlag)
{
   //Confirm connected to bus state part 1 (container)
   const C_OSCCanMessageContainer * const pc_Container =
      C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_MessageId.u32_NodeIndex,
                                                                      orc_MessageId.e_ComProtocol,
                                                                      orc_MessageId.u32_InterfaceIndex,
                                                                      orc_MessageId.u32_DatapoolIndex);

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
      orc_MessageId.u32_NodeIndex);

   if ((pc_Node != NULL) &&
       (pc_Container != NULL))
   {
      if (orc_MessageId.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         const C_OSCNodeComInterfaceSettings & rc_Interface =
            pc_Node->c_Properties.c_ComInterfaces[orc_MessageId.u32_InterfaceIndex];

         //Confirm connected to bus state part 2 (interface) -> Bus mode
         if ((rc_Interface.q_IsBusConnected == true) &&
             (pc_Container->q_IsComProtocolUsedByInterface == true))
         {
            std::vector<uint32> c_NodeIndexes;
            std::vector<uint32> c_InterfaceIndexes;
            std::vector<uint32> c_DatapoolIndexes;

            C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeAndComDpIndexesOfBus(
               rc_Interface.u32_BusIndex, orc_MessageId.e_ComProtocol, c_NodeIndexes, c_InterfaceIndexes,
               c_DatapoolIndexes);

            //Check if operation was successful
            if ((c_NodeIndexes.size() == c_InterfaceIndexes.size()) &&
                (c_NodeIndexes.size() == c_DatapoolIndexes.size()))
            {
               //Confirm last owners still valid
               for (uint32 u32_ItLastOwner = 0UL; u32_ItLastOwner < orc_LastOwnerNodeName.size();
                    ++u32_ItLastOwner)
               {
                  const QString & rc_CurName = orc_LastOwnerNodeName[u32_ItLastOwner];
                  //Iterate over all connected nodes
                  for (uint32 u32_ItConnectedNode = 0UL;
                       u32_ItConnectedNode < c_NodeIndexes.size();
                       ++u32_ItConnectedNode)
                  {
                     const C_OSCNode * const pc_ConnectedNode =
                        C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndexes[u32_ItConnectedNode]);
                     if (pc_ConnectedNode != NULL)
                     {
                        if (rc_CurName.compare(pc_ConnectedNode->c_Properties.c_Name.c_str()) == 0)
                        {
                           bool q_ExactMatch = false;
                           //Match!
                           //1. simple check: check if connected bus matches expected bus
                           if (orc_LastOwnerNodeInterfaceIndex[u32_ItLastOwner] <
                               pc_ConnectedNode->c_Properties.c_ComInterfaces.size())
                           {
                              const C_OSCNodeComInterfaceSettings & rc_LastInterface =
                                 pc_Node->c_Properties.c_ComInterfaces[orc_LastOwnerNodeInterfaceIndex[u32_ItLastOwner]];

                              if ((rc_LastInterface.q_IsBusConnected == true) &&
                                  (rc_LastInterface.u32_BusIndex == rc_Interface.u32_BusIndex))
                              {
                                 const C_OSCCanProtocol * const pc_Protocol =
                                    C_PuiSdHandler::h_GetInstance()->GetCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                                    orc_MessageId.e_ComProtocol,
                                                                                    orc_LastOwnerNodeDatapoolIndex[
                                                                                       u32_ItLastOwner]);

                                 //Exact match use same info
                                 orc_NewOwnerNodeName.push_back(orc_LastOwnerNodeName[u32_ItLastOwner]);
                                 orc_NewOwnerNodeInterfaceIndex.push_back(orc_LastOwnerNodeInterfaceIndex[
                                                                             u32_ItLastOwner]);
                                 orc_NewOwnerIsTxFlag.push_back(orc_LastOwnerIsTxFlag[u32_ItLastOwner]);
                                 //Signal stop
                                 q_ExactMatch = true;

                                 // Check if the Datapool does match too
                                 if ((pc_Protocol != NULL) &&
                                     (orc_LastOwnerNodeDatapoolIndex[u32_ItLastOwner] < pc_Node->c_DataPools.size()))
                                 {
                                    orc_NewOwnerNodeDatapoolIndex.push_back(orc_LastOwnerNodeDatapoolIndex[
                                                                               u32_ItLastOwner]);
                                 }
                                 else
                                 {
                                    //The Datapool has to be changed to the matching one
                                    orc_NewOwnerNodeDatapoolIndex.push_back(c_DatapoolIndexes[u32_ItConnectedNode]);
                                 }
                              }
                           }
                           //Only continue if change necessary -> adapt last owner!
                           if (q_ExactMatch == false)
                           {
                              //node name seems to match so no change necessary
                              orc_NewOwnerNodeName.push_back(orc_LastOwnerNodeName[u32_ItLastOwner]);
                              //The direction should not be a problem
                              orc_NewOwnerIsTxFlag.push_back(orc_LastOwnerIsTxFlag[u32_ItLastOwner]);

                              //The interface has to be changed to the only connected one
                              orc_NewOwnerNodeInterfaceIndex.push_back(c_InterfaceIndexes[u32_ItConnectedNode]);
                              //The Datapool has to be changed to the matching one
                              orc_NewOwnerNodeDatapoolIndex.push_back(c_DatapoolIndexes[u32_ItConnectedNode]);
                           }
                           //Always stop if matching node name was found
                           // (avoid match with same node and different interface!)
                           break;
                        }
                     }
                  }
               }
            }
         }
         else
         {
            // Node mode
            //Confirm last owners still valid
            for (uint32 u32_ItLastOwner = 0UL; u32_ItLastOwner < orc_LastOwnerNodeName.size(); ++u32_ItLastOwner)
            {
               const QString & rc_CurName = orc_LastOwnerNodeName[u32_ItLastOwner];

               if (rc_CurName.compare(pc_Node->c_Properties.c_Name.c_str()) == 0)
               {
                  bool q_ExactMatch = false;
                  const C_OSCCanProtocol * const pc_Protocol =
                     C_PuiSdHandler::h_GetInstance()->GetCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                     orc_MessageId.e_ComProtocol,
                                                                     orc_LastOwnerNodeDatapoolIndex[u32_ItLastOwner]);

                  //Match!
                  if ((pc_Protocol != NULL) &&
                      (orc_LastOwnerNodeDatapoolIndex[u32_ItLastOwner] < pc_Node->c_DataPools.size()))
                  {
                     //Exact match use same info
                     orc_NewOwnerNodeDatapoolIndex.push_back(orc_LastOwnerNodeDatapoolIndex[u32_ItLastOwner]);
                     //Signal stop
                     q_ExactMatch = true;
                  }
                  //Only continue if change necessary -> adapt last owner!
                  if (q_ExactMatch == false)
                  {
                     //The Datapool has to be changed to the matching one
                     orc_NewOwnerNodeDatapoolIndex.push_back(orc_MessageId.u32_DatapoolIndex);
                  }

                  //node name seems to match so no change necessary
                  orc_NewOwnerNodeName.push_back(orc_LastOwnerNodeName[u32_ItLastOwner]);
                  //The direction should not be a problem
                  orc_NewOwnerIsTxFlag.push_back(orc_LastOwnerIsTxFlag[u32_ItLastOwner]);
                  // In node mode it is not relevant what the old interface index was, it is always the current one
                  orc_NewOwnerNodeInterfaceIndex.push_back(orc_MessageId.u32_InterfaceIndex);
               }
            }
         }
      }
   }
}
