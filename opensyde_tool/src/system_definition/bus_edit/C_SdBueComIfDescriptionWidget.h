//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing and editing COM interface description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUECOMIFDESCRIPTIONWIDGET_H
#define C_SDBUECOMIFDESCRIPTIONWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.h"

#include "C_OSCNode.h"
#include "C_OSCCanProtocol.h"
#include "C_OSCCanMessage.h"
#include "C_SdBueUnoManager.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueComIfDescriptionWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueComIfDescriptionWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueComIfDescriptionWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueComIfDescriptionWidget(void);

   void InitStaticNames(void) const;
   void SetNodeId(const stw_types::uint32 ou32_NodeIndex,
                  const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol);
   void SetProtocolByDataPool(const stw_types::uint32 ou32_DataPoolIndexw);
   void SetBusId(const stw_types::uint32 ou32_BusIndex);
   void SetInitialFocus(void) const;
   void PartialReload(void);
   void SelectMessageSearch(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                            const stw_types::uint32 ou32_ListIndex, const stw_types::uint32 ou32_MessageIndex) const;

   void SelectSignalSearch(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                           const stw_types::uint32 ou32_ListIndex, const stw_types::uint32 ou32_ElementIndex) const;
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

   void ImportMessages(void);

   stw_opensyde_core::C_OSCCanProtocol::E_Type GetActProtocol(void) const;
   void TriggerLoadOfSplitterUserSettings(void) const;
   void TriggerSaveOfSplitterUserSettings(void) const;
   void SaveUserSettings(void) const;
   void LoadUserSettings(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigErrorChange(void) const;
   // In case of node mode
   void SigSwitchToBus(const stw_types::uint32 & oru32_BusIndex, const QString & orc_BusName) const;
   void SigCommDataPoolAdded(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void hideEvent(QHideEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueComIfDescriptionWidget(const C_SdBueComIfDescriptionWidget &);
   C_SdBueComIfDescriptionWidget & operator =(const C_SdBueComIfDescriptionWidget &);

   void m_ConnectNodeToProt(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex);
   void m_ConnectNodeToProtAndAddDataPool(const stw_types::uint32 ou32_NodeIndex,
                                          const stw_types::uint32 ou32_InterfaceIndex);
   void m_DisconnectNodeFromProt(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex);

   void m_ProtocolChanged(void);
   void m_InterfaceChanged(void);
   void m_Reload(void);
   void m_ReloadMessages(void);
   void m_SelectMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void m_SelectSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                       const stw_types::uint32 & oru32_SignalIndex) const;
   void m_FillNodeDatapoolIndexes(const stw_opensyde_core::C_OSCNode * const opc_Node);
   static stw_types::sintn mh_GetIndexOfProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol);
   void m_SetProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol) const;
   void m_SelectMessageProperties(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void m_SelectSignalProperties(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                 const stw_types::uint32 & oru32_SignalIndex) const;
   void m_OnMessageIdChange(void) const;
   void m_OnMessageNameChange(void) const;
   void m_OnSignalNameChange(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnSignalStartBitChange(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnSignalPositionChange(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnConnectionChange(void);
   void m_OnSignalCountOfMessageChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void m_RecheckErrorGlobal(void) const;
   void m_RecheckError(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnMessagesSelected(void) const;
   void m_OnMessageCountChanged(void);
   void m_OnMessageRxChanged(void) const;
   void m_OnChange(void);
   const std::vector<stw_opensyde_core::C_OSCCanMessage> * m_PrepareMessageId(const stw_types::uint32 ou32_NodeIndex,
                                                                              const stw_types::uint32 ou32_DataPoolIndex, const stw_types::uint32 ou32_ListIndex, stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId)
   const;
   void m_OnLinkSwitchToBus(const QString & orc_Link) const;
   void m_UpdateText(void);
   void m_UpdateTabText(void) const;
   void m_UpdateTabText(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol) const;
   void m_UpdateInterfaceText(void) const;
   void m_UpdateInterfaceText(const stw_types::uint32 ou32_InterfaceIndex) const;
   void m_GetNodeMessageAndSignalCount(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol,
                                       const stw_types::uint32 ou32_InterfaceIndex,
                                       stw_types::uint32 & oru32_MessageCount,
                                       stw_types::uint32 & oru32_SignalCount) const;

   Ui::C_SdBueComIfDescriptionWidget * mpc_Ui;
   stw_types::uint32 mu32_BusIndex;                      // For bus mode
   stw_types::uint32 mu32_NodeIndex;                     // For single node mode
   stw_types::uint32 mu32_InterfaceIndex;                // For single node mode
   std::vector<std::vector<bool> > mc_ProtocolUsedOnBus; // For single node mode
   // 1st layer is matching to the protocol types
   // 2nd layer is matching to the combo box entries

   // Information for the interface combo box in node mode
   std::vector<QString> mc_InterfaceNames;
   std::vector<QString> mc_BusNames;
   std::vector<stw_types::uint32> mc_BusIndexes;
   // First layer is for protocols
   // In case of node mode second layer is for interface
   // In case of bus mode second layer has fixed size 1
   std::vector<std::vector<stw_types::uint32> > mc_MessageCount;
   std::vector<std::vector<stw_types::uint32> > mc_SignalCount;

   std::vector<stw_types::uint32> mc_DatapoolIndexes; // For single node mode
   bool mq_ModeSingleNode;
   bool mq_LinkOnly;
   bool mq_IndexValid;
   stw_opensyde_gui_logic::C_SdBueUnoManager mc_UndoManager;
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager mc_MessageSyncManager;
};
}

#endif
