//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing and editing COM interface description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUECOMIFDESCRIPTIONWIDGET_HPP
#define C_SDBUECOMIFDESCRIPTIONWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.hpp"

#include "C_OscNode.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscCanMessage.hpp"
#include "C_SdBueUnoManager.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueComIfDescriptionWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueComIfDescriptionWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueComIfDescriptionWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueComIfDescriptionWidget(void) override;

   void InitStaticNames(void) const;
   void SetNodeId(const uint32_t ou32_NodeIndex, const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol);
   void SetProtocolByDataPool(const uint32_t ou32_DataPoolIndexw);
   void SetBusId(const uint32_t ou32_BusIndex);
   void SetInitialFocus(void) const;
   void SetProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol);

   void PartialReload(void);
   void SelectMessageSearch(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                            const uint32_t ou32_ListIndex, const uint32_t ou32_MessageIndex);

   void SelectSignalSearch(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                           const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex);
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

   void ImportMessages(void);

   stw::opensyde_core::C_OscCanProtocol::E_Type GetActProtocol(void) const;
   void TriggerLoadOfSplitterUserSettings(void) const;
   void TriggerSaveOfSplitterUserSettings(void) const;
   void SaveUserSettings(void) const;
   void LoadUserSettings(void);
   static stw::opensyde_core::C_OscCanProtocol::E_Type h_GetProtocolOfIndex(const uint32_t ou32_ProtocolIndex);
   static int32_t h_GetIndexOfProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Protocol);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigErrorChange(void) const;
   // In case of node mode

   void SigSwitchToBus(const uint32_t & oru32_BusIndex, const QString & orc_BusName) const;
   void SigSwitchToBusProtocol(const uint32_t & oru32_BusIndex, const QString & orc_BusName,
                               const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType) const;
   void SigSwitchToCoManager(const uint32_t ou32_NodeIndex, const QString & orc_NodeName,
                             const uint8_t ou8_InterfaceNumber) const;
   void SigCommDataPoolAdded(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   void showEvent(QShowEvent * const opc_Event) override;
   void hideEvent(QHideEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueComIfDescriptionWidget(const C_SdBueComIfDescriptionWidget &);
   C_SdBueComIfDescriptionWidget & operator =(const C_SdBueComIfDescriptionWidget &) &;

   void m_ConnectNodeToProt(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex);
   void m_ConnectNodeToProtAndAddDataPool(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex);
   void m_DisconnectNodeFromProt(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex);

   void m_ProtocolChanged(void);
   void m_InterfaceChanged(void);
   void m_Reload(void);
   void m_ReloadMessages(void);
   void m_SelectMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void m_SelectSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                       const uint32_t & oru32_SignalIndex) const;
   void m_FillNodeDatapoolIndexes(const stw::opensyde_core::C_OscNode * const opc_Node);
   void m_SelectMessageProperties(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void m_SelectSignalProperties(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                 const uint32_t & oru32_SignalIndex) const;
   void m_OnMessageIdChange(void) const;
   void m_OnMessageNameChange(void) const;
   void m_OnSignalNameChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnSignalStartBitChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnSignalPositionChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnConnectionChange(void);
   void m_OnSignalCountOfMessageChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_RecheckErrorGlobal(void) const;
   void m_RecheckError(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnMessagesSelected(void) const;
   void m_OnMessageCountChanged(void);
   void m_OnMessageRxChanged(void) const;
   void m_OnChange(void);
   const std::vector<stw::opensyde_core::C_OscCanMessage> * m_PrepareMessageId(const uint32_t ou32_NodeIndex,
                                                                               const uint32_t ou32_DataPoolIndex,
                                                                               const uint32_t ou32_ListIndex,
                                                                               stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId)
   const;
   void m_OnLinkSwitchToBus(const QString & orc_Link) const;
   void m_UpdateText(void);
   void m_UpdateTabText(void);
   void m_UpdateTabText(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol);
   void m_UpdateInterfaceText(void) const;
   void m_UpdateInterfaceText(const uint32_t ou32_InterfaceIndex) const;
   void m_GetNodeMessageAndSignalCount(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol,
                                       const uint32_t ou32_InterfaceIndex, uint32_t & oru32_MessageCount,
                                       uint32_t & oru32_SignalCount) const;
   //int32_t m_GetIndexOfProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Protocol);
   //int32_t m_Test(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Protocol);

   Ui::C_SdBueComIfDescriptionWidget * mpc_Ui;
   uint32_t mu32_BusIndex;                               // For bus mode
   uint32_t mu32_NodeIndex;                              // For single node mode
   uint32_t mu32_InterfaceIndex;                         // For single node mode
   std::vector<std::vector<bool> > mc_ProtocolUsedOnBus; // For single node mode
   // 1st layer is matching to the protocol types
   // 2nd layer is matching to the combo box entries

   // Information for the interface combo box in node mode
   std::vector<QString> mc_InterfaceNames;
   std::vector<QString> mc_BusNames;
   std::vector<uint32_t> mc_BusIndexes;
   // First layer is for protocols
   // In case of node mode second layer is for interface
   // In case of bus mode second layer has fixed size 1
   std::vector<std::vector<uint32_t> > mc_MessageCount;
   std::vector<std::vector<uint32_t> > mc_SignalCount;

   std::vector<uint32_t> mc_DatapoolIndexes; // For single node mode
   bool mq_ModeSingleNode;
   bool mq_LinkOnly;
   bool mq_IsCoDevice;              // Special case CANopen: mq_LinkOnly == false,
                                    // mq_ModeSingleNode == true and current node is a
                                    // CANopen device
   uint32_t mu32_CoDeviceIntfIndex; // Interface index of device which is connected to the
   // Manager
   stw::opensyde_core::C_OscCanInterfaceId mc_CoDeviceInterfaceId; // Intf Id of device for CANopen Manager
                                                                   // identification
   uint32_t mu32_CoManagerNodeIndexOfCoDevice;                     // Node id of manager the device is assigned to
   uint8_t mu8_CoManagerNodeIndexIntfNumber;                       // Interface number of manager the device is assigned
   // to
   bool mq_IndexValid;
   stw::opensyde_gui_logic::C_SdBueUnoManager mc_UndoManager;
   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager mc_MessageSyncManager;
   bool mq_SkipLoadUserSettings;
};
}
}

#endif
