//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Message and signal undo command stack (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEUNOMANAGER_H
#define C_SDBUEUNOMANAGER_H

/* -- Includes ------------------------------------------------------------- */
#include <QTreeWidget>
#include "C_UtiUndoStack.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueUnoManager :
   public C_UtiUndoStack
{
public:
   C_SdBueUnoManager(QObject * const opc_Parent);
   ~C_SdBueUnoManager(void);
   void DoConnectNodeToProt(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                            const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget);
   void DoConnectNodeToProtAndAddDataPool(const stw_types::uint32 ou32_NodeIndex,
                                          const stw_types::uint32 ou32_InterfaceIndex,
                                          const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol,
                                          QWidget * const opc_Widget);
   void DoDisconnectNodeFromProt(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                                 const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol,
                                 QWidget * const opc_Widget);
   void DoAddDataPool(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                      const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget);
   void DoAddMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                     QTreeWidget * const opc_MessageTreeWidget);
   void DoPasteMessages(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                        const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_Messages,
                        const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> > & orc_OSCSignalCommons, const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UISignalCommons, const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UISignals, const std::vector<std::vector<QString> > & orc_OwnerNodeName, const std::vector<std::vector<stw_types::uint32> > & orc_OwnerNodeInterfaceIndex, const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager, QTreeWidget * const opc_MessageTreeWidget);
   void DoPasteSignals(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                       const stw_types::uint32 & oru32_SignalIndex,
                       const std::vector<stw_opensyde_core::C_OSCCanSignal> & orc_Signals,
                       const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                       const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                       const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals,
                       C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                       QTreeWidget * const opc_MessageTreeWidget);
   void DoDeleteMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                        C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                        QTreeWidget * const opc_MessageTreeWidget);
   void DoAddSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                    const stw_types::uint32 & oru32_SignalIndex, const stw_types::uint16 ou16_StartBit,
                    C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                    QTreeWidget * const opc_MessageTreeWidget);
   void DoDeleteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                       const stw_types::uint32 & oru32_SignalIndex,
                       C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                       QTreeWidget * const opc_MessageTreeWidget);
   void DoMoveSignal(const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_SourceMessageIds,
                     const std::vector<stw_types::uint32> & oru32_SourceSignalIndices,
                     const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_TargetMessageIds,
                     const std::vector<stw_types::uint32> & oru32_TargetSignalIndices,
                     C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                     QTreeWidget * const opc_MessageTreeWidget);
   void CommitDelete(void);

private:
   QUndoCommand * mpc_DeleteCommand;

   QUndoCommand * m_GetDeleteCommand(void);
   static void mh_PatchMessageId(const C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                 stw_opensyde_core::C_OSCCanMessage & orc_Message);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
