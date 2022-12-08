//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing message properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGEPROPERTIESWIDGET_H
#define C_SDBUEMESSAGEPROPERTIESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include "stwtypes.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_OscCanMessage.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueMessagePropertiesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessagePropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessagePropertiesWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessagePropertiesWidget(void) override;

   void InitStaticNames(void) const;
   void SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void SetMessageId(const bool oq_IsValid,
                     const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SetNodeId(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                  const std::vector<uint32_t> & orc_DatapoolIndexes);
   void SetBusId(const uint32_t ou32_BusIndex);
   void OnConnectionChange(void);
   void SetComProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Value);
   void SelectName(void) const;

   void OnNodeDisconnected(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex);

   void ConnectAllChanges(void) const;
   void DisconnectAllChanges(void) const;

   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> GetMatchingMessageIds(void) const;

private:
   //Avoid call
   C_SdBueMessagePropertiesWidget(const C_SdBueMessagePropertiesWidget &);
   C_SdBueMessagePropertiesWidget & operator =(const C_SdBueMessagePropertiesWidget &) &;

   Ui::C_SdBueMessagePropertiesWidget * mpc_Ui;
   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_ComProtocol;
   bool mq_IdIsValid;
   stw::opensyde_core::C_OscCanMessageIdentificationIndices mc_MessageId;
   uint32_t mu32_BusIndex;                       // Used by bus mode
   uint32_t mu32_NodeIndex;                      // Used by node mode
   uint32_t mu32_InterfaceIndex;                 // Used by node mode
   std::vector<uint32_t> mc_NodeDatapoolIndexes; // Used by node mode
   bool mq_ModeSingleNode;

   // Used by bus mode
   // All vectors have a 1:1 mapping and must have the same size
   std::vector<uint32_t> mc_BusNodeIndexes;
   std::vector<uint32_t> mc_BusInterfaceIndexes;
   std::vector<uint32_t> mc_BusDatapoolIndexes;

   // Mapping from the indexes of the combo boxes of node and Datapool to the vector index of mc_NodeIndexes,
   // mc_InterfaceIndexes and mc_DatapoolIndexes
   // The outer vector equals the index of the node combo box and the inner vector equals the index of the
   // Datapool combo box and the saved index is the index of the three vectors
   std::vector<std::vector<uint32_t> > mc_MappingTxSelection;
   // Same relation like in mc_MappingTxSelection but with the names of the associated Datapool names
   std::vector<std::vector<QString> > mc_DatapoolNamesTxSelection;

   bool mq_InternalRxChange;

   // CANopen specific
   bool mq_CoDeviceIsTransmitter;
   bool mq_CoEventTimerDisabled;

   void m_CheckMessageName(void) const;
   void m_CheckMessageId(void) const;
   void m_CheckEarlyTime(void) const;
   void m_CheckTxMethod(void) const;
   void m_TrimmMessageName(void) const;
   void m_RegisterChange(void);
   void m_LoadFromData(void);
   void m_CoLoadEdsRestricitions(const stw::opensyde_core::C_OscCanMessage * const opc_Message);
   void m_OnExtendedChangeWithoutDataAccess(const bool & orq_Extended) const;
   void m_OnTxMethodChange(const int32_t & ors32_State) const;
   static int32_t mh_TxMethodToIndex(const stw::opensyde_core::C_OscCanMessage::E_TxMethodType & ore_TxMethod);
   static stw::opensyde_core::C_OscCanMessage::E_TxMethodType mh_IndexToTxMethod(const int32_t & ors32_Index);
   void m_OnNameChanged(void);
   void m_OnIdChanged(void);
   void m_OnCobIdChanged(void);
   uint8_t m_GetCoNodeId(const stw::opensyde_core::C_OscCanMessage & orc_MessageData);
   void m_UpdateCobIdText(const stw::opensyde_core::C_OscCanMessage & orc_MessageData,
                          const uint8_t ou8_CoDeviceNodeId);
   void m_OnExtendedChanged(void);
   void m_OnDlcChanged(void);
   void m_OnCycleTimeChanged(void);
   void m_OnTxMethodChanged(void);
   void m_OnEarlyOrLaterTimeChanged(void);
   void m_OnCoPdoSyncNumberChanged(void);
   void m_OnPropertiesChanged(void);
   void m_OnDirectionChanged(void);
   void m_OnTxChangedReloadDatapools(void);
   void m_OnTxChanged(void);
   void m_OnRxChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                      const uint32_t ou32_DatapoolIndex, const bool oq_Checked);
   void m_OnRxTimeoutFlagChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                 const uint32_t ou32_DatapoolIndex,
                                 const stw::opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode oe_TimeoutMode);
   void m_OnRxTimeoutValueChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                  const uint32_t ou32_DatapoolIndex, const uint32_t ou32_TimeoutValue);
   void m_ConnectProtocolSpecificFields(void) const;
   void m_DisconnectProtocolSpecificFields(void) const;
   void m_ConnectNodeSpecificFields(void) const;
   void m_DisconnectNodeSpecificFields(void) const;
   void m_SetModeSpecificWidgetsVisible(const bool oq_BusMode) const;
   void m_SetNodeModeTransmitVisible(const bool oq_Visible) const;
   void m_SetNodeModeReceiveVisible(const bool oq_Visible) const;
   void m_ReloadNodes(void);
   void m_UpdateTxSelection(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MatchingMessageIds);
   void m_UpdateRxAfterTxSelection(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MatchingMessageIds,
      const bool oq_SkipDisconnect = false);
   void m_HandleCriticalMessagesAndRx(const bool oq_UpdateRx, const bool oq_HandleSignals = true);
   int32_t m_GetVectorIndexOfComboBoxSelection(void) const;
   void m_GetComboBoxIndexesByVectorIndex(const uint32_t ou32_VectorIndex, uint32_t & oru32_ComboBoxNodeIndex,
                                          uint32_t & oru32_ComboBoxDatapoolIndex) const;

   void m_SyncCycleToLater(void) const;
   void m_SyncLaterToCycle(void) const;

   void m_BusModeAdaptTransmitterComboBoxesValid(const bool oq_Valid) const;

   void m_NodeModeDirectionChanged(const bool oq_Disconnect = true);
   void m_NodeModeTransmitDatapoolChanged(void);

   uint32_t m_GetDefaultTimeout(void) const;
   uint32_t m_GetCoPdoSyncTxMethodCycleEquivalent(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigMessageIdChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigMessageNameChanged(void);
   void SigMessageDlcChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigRecheckError(void);
   void SigMessageRxChanged(void);
};
}
}

#endif // C_SDBUEMESSAGEPROPERTIESWIDGET_H
