//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for bus messages (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGESELECTORWIDGET_HPP
#define C_SDBUEMESSAGESELECTORWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include "C_SdBueUnoManager.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueMessageSelectorWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageSelectorWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageSelectorWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessageSelectorWidget() override;

   void SetNodeId(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                  const std::vector<uint32_t> & orc_DatapoolIndexes);
   void SetBusId(const uint32_t ou32_BusIndex);
   void SetUndoManager(stw::opensyde_gui_logic::C_SdBueUnoManager * const opc_Value) const;
   void SetProtocolType(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Value);
   void UpdateButtonText(void) const;
   void SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void InitFromData(void) const;
   void InitStaticNames(void) const;
   void OnMessageIdChange(void) const;
   void OnMessageNameChange(void) const;
   void OnSignalNameChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void OnSignalStartBitChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void OnSignalPositionChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void OnNodeDisconnected(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex) const;
   void RecheckErrorGlobal(void) const;
   void RecheckError(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void RecheckProtocolError(void) const;
   void SetInitialFocus(void) const;
   void SelectMessagesWithoutSignal(void);
   void SelectMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void SelectSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t & oru32_SignalIndex) const;
   bool IsSelectionEmpty(void) const;

   void AddSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                  const uint16_t ou16_StartBit) const;
   void AddSignalMultiplexed(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                             const uint16_t ou16_StartBit, const uint16_t ou16_MultiplexValue) const;
   void CopySignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                   const uint32_t ou32_SignalIndex) const;
   void CutSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                  const uint32_t ou32_SignalIndex) const;
   void PasteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                    const uint16_t ou16_StartBit) const;
   void DeleteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t ou32_SignalIndex) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMessageSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                          const uint32_t & oru32_SignalIndex);
   void SigSignalCountOfMessageChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigMessagesSelected(void);
   void SigMessageCountChanged(void);
   void SigSelectName(void);
   void SigRefreshSelection(void);
   void SigErrorChanged(void);
   void SigCommDataPoolAdded(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SdBueMessageSelectorWidget(const C_SdBueMessageSelectorWidget &);
   C_SdBueMessageSelectorWidget & operator =(const C_SdBueMessageSelectorWidget &) &;

   Ui::C_SdBueMessageSelectorWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   bool mq_ModeSingleNode;
   uint32_t mu32_NodeIndex;      // Used by node mode
   uint32_t mu32_InterfaceIndex; // Used by node mode
   uint32_t mu32_BusIndex;       // Used by bus mode
   bool mq_MessagesActive;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_ProtocolType;
   QAction * mpc_AddMessageAction;
   QAction * mpc_AddMessageFromCatalogAction;
   QAction * mpc_AddSignalAction;
   QAction * mpc_AddSignalActionWithKey;
   QAction * mpc_ExpandAllAction;
   QAction * mpc_CollapseAllAction;
   QAction * mpc_CopyAction;
   QAction * mpc_CutAction;
   QAction * mpc_PasteAction;
   QAction * mpc_DeleteAction;

   void m_AddMessageButtonClicked(void) const;
   void m_MessagesButtonClicked(void);
   void m_MessagesSelectionChanged(void);
   void m_SelectMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_SelectSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                       const uint32_t & oru32_SignalIndex);
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_OnSignalCountOfMessageChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_OnMessageCountChanged(void);
   void m_AddMessageFromCatalogButtonClicked(void) const;
};
}
}

#endif // C_SDBUEMESSAGESELECTORWIDGET_HPP
