//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message and signal undo command stack (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOMANAGER_H
#define C_SDBUEUNOMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTreeWidget>
#include "C_UtiUndoStack.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"
#include "C_OscCanOpenManagerMappableSignal.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoManager :
   public C_UtiUndoStack
{
public:
   C_SdBueUnoManager(QObject * const opc_Parent = NULL);
   ~C_SdBueUnoManager(void) override;
   void DoConnectNodeToProt(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                            const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget);
   void DoConnectNodeToProtAndAddDataPool(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                          const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol,
                                          QWidget * const opc_Widget);
   void DoDisconnectNodeFromProt(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                 const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol,
                                 QWidget * const opc_Widget);
   void DoAddMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                     QTreeWidget * const opc_MessageTreeWidget);
   void DoPasteMessages(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                        const std::vector<stw::opensyde_core::C_OscCanMessage> & orc_Messages, const std::
                        vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> > & orc_OscSignalCommons,
                        const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UiSignalCommons,
                        const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UiSignals,
                        const std::vector<std::vector<QString> > & orc_OwnerNodeName,
                        const std::vector<std::vector<uint32_t> > & orc_OwnerNodeInterfaceIndex,
                        const std::vector<std::vector<uint32_t> > & orc_OwnerNodeDatapoolIndex,
                        const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag,
                        C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                        QTreeWidget * const opc_MessageTreeWidget,
                        std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_NewIds);
   void DoPasteSignals(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                       const uint32_t ou32_SignalIndex,
                       const std::vector<stw::opensyde_core::C_OscCanSignal> & orc_Signals,
                       const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalCommons,
                       const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                       const std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals,
                       C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                       QTreeWidget * const opc_MessageTreeWidget,
                       const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType);
   void DoDeleteMessages(
      const std::vector<std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> > & orc_SortedAscendingMessageGroups, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager, QTreeWidget * const opc_MessageTreeWidget);
   void DoAddSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                    const uint32_t ou32_SignalIndex, const uint16_t ou16_StartBit,
                    const stw::opensyde_core::C_OscCanSignal::E_MultiplexerType oe_MultiplexerType,
                    const uint16_t ou16_MultiplexerValue,
                    C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                    QTreeWidget * const opc_MessageTreeWidget);
   void DoAddCoSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                      const uint32_t ou32_SignalIndex, const uint16_t ou16_StartBit,
                      const std::vector<stw::opensyde_core::C_OscCanOpenManagerMappableSignal> & orc_NewSignalInfo,
                      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                      QTreeWidget * const opc_MessageTreeWidget);
   void DoDeleteSignals(const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageId,
                        const std::vector<uint32_t> & orc_SignalIndex,
                        C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                        QTreeWidget * const opc_MessageTreeWidget);
   void DoMoveSignal(const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_SourceMessageIds,
                     const std::vector<uint32_t> & orc_SourceSignalIndices,
                     const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_TargetMessageIds,
                     const std::vector<uint32_t> & orc_TargetSignalIndices,
                     C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                     QTreeWidget * const opc_MessageTreeWidget);
   void CommitDelete(void);

private:
   QUndoCommand * mpc_DeleteCommand;

   QUndoCommand * m_GetDeleteCommand(void);
   static void mh_PatchMessageId(const C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                 stw::opensyde_core::C_OscCanMessage & orc_Message);

   static void mh_HandleLastOwnersValidation(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      const std::vector<std::vector<QString> > & orc_LastOwnerNodeName,
      const std::vector<std::vector<uint32_t> > & orc_LastOwnerNodeInterfaceIndex,
      const std::vector<std::vector<uint32_t> > & orc_LastOwnerNodeDatapoolIndex,
      const std::vector<std::vector<bool> > & orc_LastOwnerIsTxFlag,
      std::vector<std::vector<QString> > & orc_NewOwnerNodeName,
      std::vector<std::vector<uint32_t> > & orc_NewOwnerNodeInterfaceIndex,
      std::vector<std::vector<uint32_t> > & orc_NewOwnerNodeDatapoolIndex,
      std::vector<std::vector<bool> > & orc_NewOwnerIsTxFlag);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
