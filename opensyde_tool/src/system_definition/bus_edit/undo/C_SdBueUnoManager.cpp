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

   \param[in,out]  opc_Parent    Optional pointer to parent
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

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_InterfaceIndex    Interface index
   \param[in]      oe_Protocol            Com protocol
   \param[in,out]  opc_Widget             Widget to notify for changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoConnectNodeToProt(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                            const C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeConnectCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol, opc_Widget));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do connect node to protocol and create data pool

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_InterfaceIndex    Interface index
   \param[in]      oe_Protocol            Com protocol
   \param[in,out]  opc_Widget             Widget to notify for changes
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

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_InterfaceIndex    Interface index
   \param[in]      oe_Protocol            Com protocol
   \param[in,out]  opc_Widget             Widget to notify for changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoDisconnectNodeFromProt(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                 const C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget)
{
   this->DoPush(new C_SdBueUnoBusProtNodeDisconnectCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol,
                                                           opc_Widget));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add message

   \param[in]      orc_MessageId             Message identification indices
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoAddMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                     C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                     QTreeWidget * const opc_MessageTreeWidget)
{
   const std::vector<C_OSCCanMessageIdentificationIndices> c_MessageIds = {orc_MessageId};

   std::vector<C_OSCCanMessage> c_Messages;
   C_OSCCanMessage c_Message;

   C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
      dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
   C_SdBueUnoMessageAddCommand * const pc_UndoCommand = new C_SdBueUnoMessageAddCommand(c_MessageIds,
                                                                                        opc_MessageSyncManager,
                                                                                        pc_MessageTreeWidget,
                                                                                        "Add Message");

   mh_PatchMessageId(opc_MessageSyncManager, c_Message);
   c_Messages.push_back(c_Message);
   pc_UndoCommand->SetInitialData(c_Messages, std::vector<std::vector<C_OSCNodeDataPoolListElement> >(),
                                  std::vector<std::vector<C_PuiSdNodeDataPoolListElement> >(),
                                  std::vector<std::vector<C_PuiSdNodeCanSignal> >(),
                                  std::vector<std::vector<QString> >(),
                                  std::vector<std::vector<uint32> >(), std::vector<std::vector<uint32> >(),
                                  std::vector<std::vector<bool> >());

   this->DoPush(pc_UndoCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do paste messages

   \param[in]      orc_MessageId                Message identification indices
   \param[in]      orc_Messages                 Message data
   \param[in]      orc_OSCSignalCommons         Signal common osc data
   \param[in]      orc_UISignalCommons          Signal common ui data
   \param[in]      orc_UISignals                Signal ui data
   \param[in]      orc_OwnerNodeName            Owner node names
   \param[in]      orc_OwnerNodeInterfaceIndex  Owner node interface index
   \param[in]      orc_OwnerNodeDatapoolIndex   Owner node Datapool index
   \param[in]      orc_OwnerIsTxFlag            Owner has message as Tx flags
   \param[in,out]  opc_MessageSyncManager       Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget        Message tree widget to perform actions on
   \param[out]     orc_NewIds                   New pasted message IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoPasteMessages(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const std::vector<C_OSCCanMessage> & orc_Messages,
                                        const std::vector<std::vector<C_OSCNodeDataPoolListElement> > & orc_OSCSignalCommons, const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UISignalCommons, const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UISignals, const std::vector<std::vector<QString> > & orc_OwnerNodeName, const std::vector<std::vector<uint32> > & orc_OwnerNodeInterfaceIndex, const std::vector<std::vector<stw_types::uint32> > & orc_OwnerNodeDatapoolIndex, const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager, QTreeWidget * const opc_MessageTreeWidget,
                                        std::vector<C_OSCCanMessageIdentificationIndices> & orc_NewIds)
{
   //Check if consistent size
   if (((((((orc_Messages.size() == orc_OSCSignalCommons.size()) &&
            (orc_Messages.size() == orc_UISignalCommons.size()))) && (orc_Messages.size() == orc_UISignals.size())) &&
         (orc_OwnerNodeName.size() == orc_OwnerNodeInterfaceIndex.size())) &&
        (orc_OwnerNodeName.size() == orc_OwnerNodeDatapoolIndex.size())) &&
       (orc_OwnerNodeName.size() == orc_OwnerIsTxFlag.size()))
   {
      if (orc_Messages.size() > 0)
      {
         std::vector<C_OSCCanMessageIdentificationIndices > c_Ids;
         std::vector<std::vector<QString> > c_OwnerNodeName;
         std::vector<std::vector<uint32> > c_OwnerNodeInterfaceIndex;
         std::vector<std::vector<uint32> > c_OwnerNodeDatapoolIndex;
         std::vector<std::vector<bool> > c_OwnerIsTxFlag;
         C_OSCCanMessageIdentificationIndices c_CurrentMessageId = orc_MessageId;

         //Reserve
         c_Ids.reserve(orc_Messages.size());

         //Check last owners storage valid
         C_SdBueUnoManager::mh_HandleLastOwnersValidation(orc_MessageId, orc_OwnerNodeName,
                                                          orc_OwnerNodeInterfaceIndex,
                                                          orc_OwnerNodeDatapoolIndex,
                                                          orc_OwnerIsTxFlag, c_OwnerNodeName,
                                                          c_OwnerNodeInterfaceIndex,
                                                          c_OwnerNodeDatapoolIndex,
                                                          c_OwnerIsTxFlag);

         //Create add command for each new message (for paste multiple can be added at once)
         for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
         {
            //The message index increment might be highly inaccurate but it should never be used anyways
            ++c_CurrentMessageId.u32_MessageIndex;
            c_Ids.push_back(c_CurrentMessageId);
         }
         {
            C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
               dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
            C_SdBueUnoMessageAddCommand * const pc_AddCommand = new C_SdBueUnoMessageAddCommand(c_Ids,
                                                                                                opc_MessageSyncManager,
                                                                                                pc_MessageTreeWidget,
                                                                                                "Paste Message(s)");
            //If vector are empty default behavior is used instead
            pc_AddCommand->SetInitialData(orc_Messages, orc_OSCSignalCommons,
                                          orc_UISignalCommons,
                                          orc_UISignals,
                                          c_OwnerNodeName, c_OwnerNodeInterfaceIndex,
                                          c_OwnerNodeDatapoolIndex,
                                          c_OwnerIsTxFlag);

            //Execute add
            this->DoPush(pc_AddCommand);

            //AFTER add step -> at this point the message IDs and especially the message index should be accurate
            orc_NewIds = pc_AddCommand->GetLastMessageIds();
            //lint -e429 Qt command stack will take care of it
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do paste signals

   \param[in]      orc_MessageId             Message identification indices
   \param[in]      ou32_SignalIndex          Signal index
   \param[in]      orc_Signals               Signals data
   \param[in]      orc_OSCSignalCommons      Signal common osc data
   \param[in]      orc_UISignalCommons       Signal common ui data
   \param[in]      orc_UISignals             Signal ui data
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
   \param[in]      oe_ProtocolType           Current active protocol to handle necessary adaptations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoPasteSignals(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                       const uint32 ou32_SignalIndex, const std::vector<C_OSCCanSignal> & orc_Signals,
                                       const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                       const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                       const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals,
                                       C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                       QTreeWidget * const opc_MessageTreeWidget,
                                       const C_OSCCanProtocol::E_Type oe_ProtocolType)
{
   //Check if consistent size
   if ((((orc_Signals.size() == orc_OSCSignalCommons.size()) &&
         (orc_Signals.size() == orc_UISignalCommons.size()))) && (orc_Signals.size() == orc_UISignals.size()))
   {
      if (orc_Signals.size() > 0)
      {
         std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> c_MessageId;
         std::vector<stw_types::uint32> c_SignalIndex;
         std::vector<stw_types::uint16> c_StartBit;
         std::vector<stw_opensyde_core::C_OSCCanSignal::E_MultiplexerType> c_MultiplexerType;
         std::vector<stw_types::uint16> c_MultiplexerValue;
         std::vector<C_OSCCanProtocol::E_Type> c_ProtocolType;
         uint32 u32_CurrentSignalIndex = ou32_SignalIndex;
         //Reserve
         c_MessageId.reserve(orc_Signals.size());
         c_SignalIndex.reserve(orc_Signals.size());
         c_StartBit.reserve(orc_Signals.size());
         c_MultiplexerType.reserve(orc_Signals.size());
         c_MultiplexerValue.reserve(orc_Signals.size());
         c_ProtocolType.reserve(orc_Signals.size());
         for (uint32 u32_ItSignal = 0; u32_ItSignal < orc_Signals.size(); ++u32_ItSignal)
         {
            c_MessageId.push_back(orc_MessageId);
            c_SignalIndex.push_back(u32_CurrentSignalIndex);
            c_StartBit.push_back(orc_Signals[u32_ItSignal].u16_ComBitStart);
            c_MultiplexerType.push_back(C_OSCCanSignal::eMUX_DEFAULT);
            c_MultiplexerValue.push_back(0);
            c_ProtocolType.push_back(oe_ProtocolType);
            ++u32_CurrentSignalIndex;
         }
         {
            C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
               dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
            C_SdBueUnoSignalAddCommand * const pc_AddCommand = new C_SdBueUnoSignalAddCommand(c_MessageId,
                                                                                              c_SignalIndex,
                                                                                              c_StartBit,
                                                                                              c_MultiplexerType,
                                                                                              c_MultiplexerValue,
                                                                                              opc_MessageSyncManager,
                                                                                              pc_MessageTreeWidget,
                                                                                              "Paste Signal(s)");
            pc_AddCommand->SetInitialData(orc_Signals, orc_OSCSignalCommons,
                                          orc_UISignalCommons, orc_UISignals,
                                          c_ProtocolType);
            this->DoPush(pc_AddCommand);
            //lint -e429 Qt command stack will take care of it
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do delete messages

   \param[in]      orc_SortedAscendingMessageGroups   Sorted ascending message groups
   \param[in,out]  opc_MessageSyncManager             Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget              Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoDeleteMessages(
   const std::vector<std::vector<C_OSCCanMessageIdentificationIndices> > & orc_SortedAscendingMessageGroups,
   C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager, QTreeWidget * const opc_MessageTreeWidget)
{
   QUndoCommand * const pc_Parent = this->m_GetDeleteCommand();

   if (pc_Parent != NULL)
   {
      std::vector<C_OSCCanMessageIdentificationIndices> c_AllMessages;
      //Consolidate
      for (uint32 u32_ItGroup = 0UL; u32_ItGroup < orc_SortedAscendingMessageGroups.size(); ++u32_ItGroup)
      {
         const std::vector<C_OSCCanMessageIdentificationIndices> & rc_Messages =
            orc_SortedAscendingMessageGroups[u32_ItGroup];
         c_AllMessages.reserve(c_AllMessages.size() + rc_Messages.size());
         for (uint32 u32_ItMessage = 0UL; u32_ItMessage < rc_Messages.size(); ++u32_ItMessage)
         {
            c_AllMessages.push_back(rc_Messages[u32_ItMessage]);
         }
      }
      {
         C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
            dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
         new C_SdBueUnoMessageDeleteCommand(c_AllMessages, opc_MessageSyncManager, pc_MessageTreeWidget, pc_Parent);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add signal

   \param[in]      orc_MessageId             Message identification indices
   \param[in]      ou32_SignalIndex          Signal index
   \param[in]      ou16_StartBit             Start bit of signal
   \param[in]      oe_MultiplexerType        Multiplexer signal type
   \param[in]      ou16_MultiplexerValue     Multiplexer value (only relevant if
                                             oe_MultiplexerType is eMUX_MULTIPLEXED_SIGNAL)
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoAddSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                    const uint32 ou32_SignalIndex, const uint16 ou16_StartBit,
                                    const C_OSCCanSignal::E_MultiplexerType oe_MultiplexerType,
                                    const uint16 ou16_MultiplexerValue,
                                    C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                    QTreeWidget * const opc_MessageTreeWidget)
{
   const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> c_MessageId = {orc_MessageId};
   const std::vector<stw_types::uint32> c_SignalIndex = {ou32_SignalIndex};
   const std::vector<stw_types::uint16> c_StartBit = {ou16_StartBit};
   const std::vector<stw_opensyde_core::C_OSCCanSignal::E_MultiplexerType> c_MultiplexerType = {oe_MultiplexerType};
   const std::vector<stw_types::uint16> c_MultiplexerValue = {ou16_MultiplexerValue};

   C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
      dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);

   this->DoPush(new C_SdBueUnoSignalAddCommand(c_MessageId, c_SignalIndex, c_StartBit, c_MultiplexerType,
                                               c_MultiplexerValue, opc_MessageSyncManager,
                                               pc_MessageTreeWidget,
                                               "Add Signal"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add CANopen signal

   \param[in]      orc_MessageId             Message identification indices
   \param[in]      ou32_SignalIndex          Signal index
   \param[in]      ou16_StartBit             Start bit of signal
   \param[in]      orc_NewSignalInfo         New signal info
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoAddCoSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                      const uint32 ou32_SignalIndex, const uint16 ou16_StartBit,
                                      const std::vector<C_OSCCanOpenManagerMappableSignal> & orc_NewSignalInfo,
                                      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                      QTreeWidget * const opc_MessageTreeWidget)
{
   const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> c_MessageId =
      std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices>(orc_NewSignalInfo.size(), orc_MessageId);

   std::vector<stw_types::uint32> c_SignalIndex;
   std::vector<stw_types::uint16> c_StartBit;
   const std::vector<stw_opensyde_core::C_OSCCanSignal::E_MultiplexerType> c_MultiplexerType =
      std::vector<stw_opensyde_core::C_OSCCanSignal::E_MultiplexerType>(
         orc_NewSignalInfo.size(), C_OSCCanSignal::eMUX_DEFAULT);
   const std::vector<stw_types::uint16> c_MultiplexerValue =
      std::vector<stw_types::uint16>(orc_NewSignalInfo.size(), 0U);

   std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommon;
   const std::vector<C_PuiSdNodeCanSignal> c_UISignal = std::vector<C_PuiSdNodeCanSignal>(
      orc_NewSignalInfo.size(), C_PuiSdNodeCanSignal());
   const std::vector<C_OSCCanProtocol::E_Type> c_ProtocolType = std::vector<C_OSCCanProtocol::E_Type>(
      orc_NewSignalInfo.size(), C_OSCCanProtocol::eCAN_OPEN);

   std::vector<C_OSCCanSignal> c_Signal;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> c_OSCSignalCommon;

   C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
      dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
   bool q_EndReached = false;
   uint16 u16_StartBit = ou16_StartBit;

   c_SignalIndex.reserve(orc_NewSignalInfo.size());
   c_StartBit.reserve(orc_NewSignalInfo.size());
   c_Signal.reserve(orc_NewSignalInfo.size());
   c_OSCSignalCommon.reserve(orc_NewSignalInfo.size());
   c_UISignalCommon.reserve(orc_NewSignalInfo.size());

   for (uint32 u32_SignalCounter = 0; u32_SignalCounter < orc_NewSignalInfo.size(); ++u32_SignalCounter)
   {
      const C_OSCCanOpenManagerMappableSignal & rc_Result = orc_NewSignalInfo[u32_SignalCounter];
      C_OSCCanSignal c_SignalData = rc_Result.c_SignalData;
      C_OSCNodeDataPoolListElement c_SignalCommonData = rc_Result.c_DatapoolData;
      C_PuiSdNodeDataPoolListElement c_UISignalCommonData;

      if (c_SignalCommonData.c_DataSetValues.size() != 1)
      {
         // Bring to default values if no init value is set already
         c_SignalCommonData.c_DataSetValues.resize(1);
         c_SignalCommonData.c_DataSetValues[0] = c_SignalCommonData.c_MinValue;
      }

      // Change the default values
      c_SignalData.u16_ComBitStart = u16_StartBit;

      // Change the default values
      c_UISignalCommonData.q_AutoMinMaxActive = rc_Result.q_AutoMinMaxUsed;

      c_SignalIndex.push_back(ou32_SignalIndex + u32_SignalCounter);
      c_StartBit.push_back(u16_StartBit);
      c_Signal.push_back(c_SignalData);
      c_OSCSignalCommon.push_back(c_SignalCommonData);
      c_UISignalCommon.push_back(c_UISignalCommonData);

      if (q_EndReached == false)
      {
         u16_StartBit += rc_Result.c_SignalData.u16_ComBitLength;
         if (u16_StartBit > 63U)
         {
            u16_StartBit = 0U;
            q_EndReached = true;
         }
      }
   }
   {
      C_SdBueUnoSignalAddCommand * const pc_AddCommand = new C_SdBueUnoSignalAddCommand(c_MessageId,
                                                                                        c_SignalIndex,
                                                                                        c_StartBit,
                                                                                        c_MultiplexerType,
                                                                                        c_MultiplexerValue,
                                                                                        opc_MessageSyncManager,
                                                                                        pc_MessageTreeWidget,
                                                                                        "Add CANopen Signal");

      // Set the CANopen specific adapted signal configuration
      pc_AddCommand->SetInitialData(c_Signal, c_OSCSignalCommon, c_UISignalCommon, c_UISignal, c_ProtocolType);

      this->DoPush(pc_AddCommand);
   }
   //lint -e429 Qt command stack will take care of it
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do delete signals

   \param[in]      orc_MessageId             Message identification indices
   \param[in]      orc_SignalIndex           Signal index
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoDeleteSignals(const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageId,
                                        const std::vector<uint32> & orc_SignalIndex,
                                        C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                        QTreeWidget * const opc_MessageTreeWidget)
{
   if (orc_MessageId.size() == orc_SignalIndex.size())
   {
      QUndoCommand * const pc_Parent = this->m_GetDeleteCommand();

      if (pc_Parent != NULL)
      {
         C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
            dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
         new C_SdBueUnoSignalDeleteCommand(orc_MessageId, orc_SignalIndex,
                                           opc_MessageSyncManager, pc_MessageTreeWidget, pc_Parent);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do move signal

   \param[in]      orc_SourceMessageIds      Source message identification indices
   \param[in]      orc_SourceSignalIndices   Source signal indices
   \param[in]      orc_TargetMessageIds      Target message identification indices
   \param[in]      orc_TargetSignalIndices   Target signal indices
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::DoMoveSignal(const std::vector<C_OSCCanMessageIdentificationIndices> & orc_SourceMessageIds,
                                     const std::vector<uint32> & orc_SourceSignalIndices,
                                     const std::vector<C_OSCCanMessageIdentificationIndices> & orc_TargetMessageIds,
                                     const std::vector<uint32> & orc_TargetSignalIndices,
                                     C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                     QTreeWidget * const opc_MessageTreeWidget)
{
   if (orc_SourceMessageIds.size() > 0)
   {
      if (((orc_SourceMessageIds.size() == orc_SourceSignalIndices.size()) &&
           (orc_SourceMessageIds.size() == orc_TargetMessageIds.size())) &&
          (orc_SourceMessageIds.size() == orc_TargetSignalIndices.size()))
      {
         std::vector<uint32> c_AdaptedSourceSignalIndices;

         C_SdBueMessageSelectorTreeWidget * const pc_MessageTreeWidget =
            dynamic_cast<C_SdBueMessageSelectorTreeWidget * const>(opc_MessageTreeWidget);
         for (uint32 u32_ItMove = 0; u32_ItMove < orc_SourceMessageIds.size(); ++u32_ItMove)
         {
            uint32 u32_CurSourceSignalIndex = orc_SourceSignalIndices[u32_ItMove];
            //Adapt index to previously moved signals
            for (uint32 u32_ItAdapted = 0; u32_ItAdapted < c_AdaptedSourceSignalIndices.size(); ++u32_ItAdapted)
            {
               if (c_AdaptedSourceSignalIndices[u32_ItAdapted] <= orc_SourceSignalIndices[u32_ItMove])
               {
                  if (u32_CurSourceSignalIndex != 0)
                  {
                     --u32_CurSourceSignalIndex;
                  }
               }
            }
            c_AdaptedSourceSignalIndices.push_back(u32_CurSourceSignalIndex);
         }
         this->DoPush(new C_SdBueUnoSignalMoveCommand(orc_SourceMessageIds, c_AdaptedSourceSignalIndices,
                                                      orc_TargetMessageIds,
                                                      orc_TargetSignalIndices, opc_MessageSyncManager,
                                                      pc_MessageTreeWidget));
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

   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  orc_Message               Message to patch message id
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

   \param[in]   orc_MessageId                      New message id from current bus to use as fallback
   \param[in]   orc_LastOwnerNodeName              Original owners node names
   \param[in]   orc_LastOwnerNodeInterfaceIndex    Original owners interface indices
   \param[in]   orc_LastOwnerNodeDatapoolIndex     Original owners Datapool indices
   \param[in]   orc_LastOwnerIsTxFlag              Original owners Tx flags
   \param[out]  orc_NewOwnerNodeName               New/valid owners node names (empty if no matches found)
   \param[out]  orc_NewOwnerNodeInterfaceIndex     New/valid owners interface indices (empty if no matches found)
   \param[out]  orc_NewOwnerNodeDatapoolIndex      New/valid owners Datapool indices (empty if no matches found)
   \param[out]  orc_NewOwnerIsTxFlag               New/valid owners Tx flags (empty if no matches found)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoManager::mh_HandleLastOwnersValidation(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                      const std::vector<std::vector<QString> > & orc_LastOwnerNodeName,
                                                      const std::vector<std::vector<uint32> > & orc_LastOwnerNodeInterfaceIndex, const std::vector<std::vector<uint32> > & orc_LastOwnerNodeDatapoolIndex, const std::vector<std::vector<bool> > & orc_LastOwnerIsTxFlag, std::vector<std::vector<QString> > & orc_NewOwnerNodeName, std::vector<std::vector<uint32> > & orc_NewOwnerNodeInterfaceIndex, std::vector<std::vector<uint32> > & orc_NewOwnerNodeDatapoolIndex,
                                                      std::vector<std::vector<bool> > & orc_NewOwnerIsTxFlag)
{
   //Confirm connected to bus state part 1 (container)
   const C_OSCCanMessageContainer * const pc_Container =
      C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_MessageId.u32_NodeIndex,
                                                                      orc_MessageId.e_ComProtocol,
                                                                      orc_MessageId.u32_InterfaceIndex,
                                                                      orc_MessageId.u32_DatapoolIndex);

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
      orc_MessageId.u32_NodeIndex);

   //Reserve
   orc_NewOwnerNodeName.reserve(orc_LastOwnerNodeName.size());
   orc_NewOwnerNodeInterfaceIndex.reserve(orc_LastOwnerNodeInterfaceIndex.size());
   orc_NewOwnerNodeDatapoolIndex.reserve(orc_LastOwnerNodeDatapoolIndex.size());
   orc_NewOwnerIsTxFlag.reserve(orc_LastOwnerIsTxFlag.size());

   if ((pc_Node != NULL) &&
       (pc_Container != NULL))
   {
      if (orc_MessageId.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         const C_OSCNodeComInterfaceSettings & rc_Interface =
            pc_Node->c_Properties.c_ComInterfaces[orc_MessageId.u32_InterfaceIndex];

         //Confirm connected to bus state part 2 (interface) -> Bus mode
         if ((rc_Interface.GetBusConnected() == true) &&
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
               for (uint32 u32_ItMessage = 0UL; u32_ItMessage < orc_LastOwnerNodeName.size(); ++u32_ItMessage)
               {
                  std::vector<QString> c_NewOwnerNodeName;
                  std::vector<stw_types::uint32> c_NewOwnerNodeInterfaceIndex;
                  std::vector<stw_types::uint32> c_NewOwnerNodeDatapoolIndex;
                  std::vector<bool> c_NewOwnerIsTxFlag;

                  //Reserve
                  c_NewOwnerNodeName.reserve(orc_LastOwnerNodeName[u32_ItMessage].size());
                  c_NewOwnerNodeInterfaceIndex.reserve(orc_LastOwnerNodeName[u32_ItMessage].size());
                  c_NewOwnerNodeDatapoolIndex.reserve(orc_LastOwnerNodeName[u32_ItMessage].size());
                  c_NewOwnerIsTxFlag.reserve(orc_LastOwnerNodeName[u32_ItMessage].size());

                  for (uint32 u32_ItLastOwner = 0UL; u32_ItLastOwner < orc_LastOwnerNodeName[u32_ItMessage].size();
                       ++u32_ItLastOwner)
                  {
                     const QString & rc_CurName = orc_LastOwnerNodeName[u32_ItMessage][u32_ItLastOwner];
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
                              if (orc_LastOwnerNodeInterfaceIndex[u32_ItMessage][u32_ItLastOwner] <
                                  pc_ConnectedNode->c_Properties.c_ComInterfaces.size())
                              {
                                 const C_OSCNodeComInterfaceSettings & rc_LastInterface =
                                    pc_Node->c_Properties.c_ComInterfaces[orc_LastOwnerNodeInterfaceIndex[u32_ItMessage]
                                                                          [
                                                                             u32_ItLastOwner]];

                                 if ((rc_LastInterface.GetBusConnected() == true) &&
                                     (rc_LastInterface.u32_BusIndex == rc_Interface.u32_BusIndex))
                                 {
                                    const C_OSCCanProtocol * const pc_Protocol =
                                       C_PuiSdHandler::h_GetInstance()->GetCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                                       orc_MessageId.e_ComProtocol,
                                                                                       orc_LastOwnerNodeDatapoolIndex[
                                                                                          u32_ItMessage][
                                                                                          u32_ItLastOwner]);

                                    //Exact match use same info
                                    c_NewOwnerNodeName.push_back(orc_LastOwnerNodeName[u32_ItMessage][u32_ItLastOwner]);
                                    c_NewOwnerNodeInterfaceIndex.push_back(orc_LastOwnerNodeInterfaceIndex[u32_ItMessage][
                                                                              u32_ItLastOwner]);
                                    c_NewOwnerIsTxFlag.push_back(orc_LastOwnerIsTxFlag[u32_ItMessage][u32_ItLastOwner]);
                                    //Signal stop
                                    q_ExactMatch = true;

                                    // Check if the Datapool does match too
                                    if ((pc_Protocol != NULL) &&
                                        (orc_LastOwnerNodeDatapoolIndex[u32_ItMessage][u32_ItLastOwner] <
                                         pc_Node->c_DataPools.size()))
                                    {
                                       c_NewOwnerNodeDatapoolIndex.push_back(orc_LastOwnerNodeDatapoolIndex[
                                                                                u32_ItMessage][
                                                                                u32_ItLastOwner]);
                                    }
                                    else
                                    {
                                       //The Datapool has to be changed to the matching one
                                       c_NewOwnerNodeDatapoolIndex.push_back(c_DatapoolIndexes[u32_ItConnectedNode]);
                                    }
                                 }
                              }
                              //Only continue if change necessary -> adapt last owner!
                              if (q_ExactMatch == false)
                              {
                                 //node name seems to match so no change necessary
                                 c_NewOwnerNodeName.push_back(orc_LastOwnerNodeName[u32_ItMessage][u32_ItLastOwner]);
                                 //The direction should not be a problem
                                 c_NewOwnerIsTxFlag.push_back(orc_LastOwnerIsTxFlag[u32_ItMessage][u32_ItLastOwner]);

                                 //The interface has to be changed to the only connected one
                                 c_NewOwnerNodeInterfaceIndex.push_back(c_InterfaceIndexes[u32_ItConnectedNode]);
                                 //The Datapool has to be changed to the matching one
                                 c_NewOwnerNodeDatapoolIndex.push_back(c_DatapoolIndexes[u32_ItConnectedNode]);
                              }
                              //Always stop if matching node name was found
                              // (avoid match with same node and different interface!)
                              break;
                           }
                        }
                     }
                  }
                  orc_NewOwnerNodeName.push_back(c_NewOwnerNodeName);
                  orc_NewOwnerNodeInterfaceIndex.push_back(c_NewOwnerNodeInterfaceIndex);
                  orc_NewOwnerNodeDatapoolIndex.push_back(c_NewOwnerNodeDatapoolIndex);
                  orc_NewOwnerIsTxFlag.push_back(c_NewOwnerIsTxFlag);
               }
            }
         }
         else
         {
            // Node mode
            //Confirm last owners still valid
            for (uint32 u32_ItMessage = 0UL; u32_ItMessage < orc_LastOwnerNodeName.size(); ++u32_ItMessage)
            {
               std::vector<QString> c_NewOwnerNodeName;
               std::vector<stw_types::uint32> c_NewOwnerNodeInterfaceIndex;
               std::vector<stw_types::uint32> c_NewOwnerNodeDatapoolIndex;
               std::vector<bool> c_NewOwnerIsTxFlag;

               //Reserve
               c_NewOwnerNodeName.reserve(orc_LastOwnerNodeName[u32_ItMessage].size());
               c_NewOwnerNodeInterfaceIndex.reserve(orc_LastOwnerNodeName[u32_ItMessage].size());
               c_NewOwnerNodeDatapoolIndex.reserve(orc_LastOwnerNodeName[u32_ItMessage].size());
               c_NewOwnerIsTxFlag.reserve(orc_LastOwnerNodeName[u32_ItMessage].size());

               for (uint32 u32_ItLastOwner = 0UL; u32_ItLastOwner < orc_LastOwnerNodeName[u32_ItMessage].size();
                    ++u32_ItLastOwner)
               {
                  const QString & rc_CurName = orc_LastOwnerNodeName[u32_ItMessage][u32_ItLastOwner];

                  if (rc_CurName.compare(pc_Node->c_Properties.c_Name.c_str()) == 0)
                  {
                     bool q_ExactMatch = false;
                     const C_OSCCanProtocol * const pc_Protocol =
                        C_PuiSdHandler::h_GetInstance()->GetCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                        orc_MessageId.e_ComProtocol,
                                                                        orc_LastOwnerNodeDatapoolIndex[u32_ItMessage][
                                                                           u32_ItLastOwner]);

                     //Match!
                     if ((pc_Protocol != NULL) &&
                         (orc_LastOwnerNodeDatapoolIndex[u32_ItMessage][u32_ItLastOwner] < pc_Node->c_DataPools.size()))
                     {
                        //Exact match use same info
                        c_NewOwnerNodeDatapoolIndex.push_back(orc_LastOwnerNodeDatapoolIndex[u32_ItMessage][
                                                                 u32_ItLastOwner]);
                        //Signal stop
                        q_ExactMatch = true;
                     }
                     //Only continue if change necessary -> adapt last owner!
                     if (q_ExactMatch == false)
                     {
                        //The Datapool has to be changed to the matching one
                        c_NewOwnerNodeDatapoolIndex.push_back(orc_MessageId.u32_DatapoolIndex);
                     }

                     //node name seems to match so no change necessary
                     c_NewOwnerNodeName.push_back(orc_LastOwnerNodeName[u32_ItMessage][u32_ItLastOwner]);
                     //The direction should not be a problem
                     c_NewOwnerIsTxFlag.push_back(orc_LastOwnerIsTxFlag[u32_ItMessage][u32_ItLastOwner]);
                     // In node mode it is not relevant what the old interface index was, it is always the current one
                     c_NewOwnerNodeInterfaceIndex.push_back(orc_MessageId.u32_InterfaceIndex);
                  }
               }
               orc_NewOwnerNodeName.push_back(c_NewOwnerNodeName);
               orc_NewOwnerNodeInterfaceIndex.push_back(c_NewOwnerNodeInterfaceIndex);
               orc_NewOwnerNodeDatapoolIndex.push_back(c_NewOwnerNodeDatapoolIndex);
               orc_NewOwnerIsTxFlag.push_back(c_NewOwnerIsTxFlag);
            }
         }
      }
   }
}
