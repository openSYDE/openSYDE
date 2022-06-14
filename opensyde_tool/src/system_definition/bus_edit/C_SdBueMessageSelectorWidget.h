//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for bus messages (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGESELECTORWIDGET_H
#define C_SDBUEMESSAGESELECTORWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include "C_SdBueUnoManager.h"
#include "C_OgeContextMenu.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueMessageSelectorWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageSelectorWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageSelectorWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueMessageSelectorWidget();
   void SetNodeId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                  const std::vector<stw_types::uint32> & orc_DatapoolIndexes) const;
   void SetBusId(const stw_types::uint32 ou32_BusIndex) const;
   void SetUndoManager(stw_opensyde_gui_logic::C_SdBueUnoManager * const opc_Value) const;
   void SetProtocolType(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value);
   void UpdateButtonText(void) const;
   void SetMessageSyncManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value) const;
   void InitFromData(void) const;
   void InitStaticNames(void) const;
   void OnMessageIdChange(void) const;
   void OnMessageNameChange(void) const;
   void OnSignalNameChange(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void OnSignalStartBitChange(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void OnSignalPositionChange(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void OnNodeDisconnected(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex) const;
   void RecheckErrorGlobal(void) const;
   void RecheckError(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void SetInitialFocus(void) const;
   void SelectMessagesWithoutSignal(void);
   void SelectMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void SelectSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint32 & oru32_SignalIndex) const;
   bool IsSelectionEmpty(void) const;

   void AddSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                  const stw_types::uint16 ou16_StartBit) const;
   void AddSignalMultiplexed(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                             const stw_types::uint16 ou16_StartBit, const stw_types::uint16 ou16_MultiplexValue) const;
   void CopySignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                   const stw_types::uint32 ou32_SignalIndex) const;
   void CutSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                  const stw_types::uint32 ou32_SignalIndex) const;
   void PasteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                    const stw_types::uint16 ou16_StartBit) const;
   void DeleteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint32 ou32_SignalIndex) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMessageSelected(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalSelected(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                          const stw_types::uint32 & oru32_SignalIndex);
   void SigSignalCountOfMessageChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigMessagesSelected(void);
   void SigMessageCountChanged(void);
   void SigSelectName(void);
   void SigRefreshSelection(void);
   void SigErrorChanged(void);
   void SigCommDataPoolAdded(void);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SdBueMessageSelectorWidget(const C_SdBueMessageSelectorWidget &);
   C_SdBueMessageSelectorWidget & operator =(const C_SdBueMessageSelectorWidget &);

   Ui::C_SdBueMessageSelectorWidget * mpc_Ui;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   bool mq_MessagesActive;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ProtocolType;
   QAction * mpc_AddMessageAction;
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
   void m_SelectMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void m_SelectSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                       const stw_types::uint32 & oru32_SignalIndex);
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_OnSignalCountOfMessageChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void m_OnMessageCountChanged(void);
};
}

#endif // C_SDBUEMESSAGESELECTORWIDGET_H
