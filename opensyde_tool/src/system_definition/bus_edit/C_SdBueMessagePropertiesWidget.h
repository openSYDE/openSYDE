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
#include "stwtypes.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_OSCCanMessage.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueMessagePropertiesWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessagePropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessagePropertiesWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueMessagePropertiesWidget(void);

   void InitStaticNames(void) const;
   void SetMessageSyncManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void SetMessageId(const bool oq_IsValid,
                     const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SetNodeId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                  const std::vector<stw_types::uint32> & orc_DatapoolIndexes);
   void SetBusId(const stw_types::uint32 ou32_BusIndex);
   void OnConnectionChange(void);
   void SetComProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value);
   void SelectName(void) const;

   void OnNodeDisconnected(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex);

   void ConnectAllChanges(void) const;
   void DisconnectAllChanges(void) const;

   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> GetMatchingMessageIds(void) const;

private:
   //Avoid call
   C_SdBueMessagePropertiesWidget(const C_SdBueMessagePropertiesWidget &);
   C_SdBueMessagePropertiesWidget & operator =(const C_SdBueMessagePropertiesWidget &);

   Ui::C_SdBueMessagePropertiesWidget * mpc_Ui;
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ComProtocol;
   bool mq_IdIsValid;
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices mc_MessageId;
   stw_types::uint32 mu32_BusIndex;                       // Used by bus mode
   stw_types::uint32 mu32_NodeIndex;                      // Used by node mode
   stw_types::uint32 mu32_InterfaceIndex;                 // Used by node mode
   std::vector<stw_types::uint32> mc_NodeDatapoolIndexes; // Used by node mode
   bool mq_ModeSingleNode;

   // Used by bus mode
   // All vectors have a 1:1 mapping and must have the same size
   std::vector<stw_types::uint32> mc_BusNodeIndexes;
   std::vector<stw_types::uint32> mc_BusInterfaceIndexes;
   std::vector<stw_types::uint32> mc_BusDatapoolIndexes;

   // Mapping from the indexes of the combo boxes of node and Datapool to the vector index of mc_NodeIndexes,
   // mc_InterfaceIndexes and mc_DatapoolIndexes
   // The outer vector equals the index of the node combo box and the inner vector equals the index of the
   // Datapool combo box and the saved index is the index of the three vectors
   std::vector<std::vector<stw_types::uint32> > mc_MappingTxSelection;
   // Same relation like in mc_MappingTxSelection but with the names of the associated Datapool names
   std::vector<std::vector<QString> > mc_DatapoolNamesTxSelection;

   bool mq_InternalRxChange;

   // CANopen specific
   bool mq_CoDeviceIsTransmitter;
   bool mq_CoEventTimerDisabled;

   void m_CheckMessageName(void) const;
   void m_CheckMessageId(void) const;
   void m_CheckEarlyTime(void) const;
   void m_TrimmMessageName(void) const;
   void m_RegisterChange(void);
   void m_LoadFromData(void);
   void m_CoLoadEdsRestricitions(const stw_opensyde_core::C_OSCCanMessage * const opc_Message);
   void m_OnExtendedChangeWithoutDataAccess(const bool & orq_Extended) const;
   void m_OnTxMethodChange(const stw_types::sint32 & ors32_State) const;
   static stw_types::sint32 mh_TxMethodToIndex(const stw_opensyde_core::C_OSCCanMessage::E_TxMethodType & ore_TxMethod);
   static stw_opensyde_core::C_OSCCanMessage::E_TxMethodType mh_IndexToTxMethod(const stw_types::sint32 & ors32_Index);
   void m_OnNameChanged(void);
   void m_OnIdChanged(void);
   void m_OnCobIdChanged(void);
   void m_OnExtendedChanged(void);
   void m_OnDlcChanged(void);
   void m_OnCycleTimeChanged(void);
   void m_OnTxMethodChanged(void);
   void m_OnEarlyOrLaterTimeChanged(void);
   void m_OnPropertiesChanged(void);
   void m_OnDirectionChanged(void);
   void m_OnTxChangedReloadDatapools(void);
   void m_OnTxChanged(void);
   void m_OnRxChanged(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                      const stw_types::uint32 ou32_DatapoolIndex, const bool oq_Checked);
   void m_OnRxTimeoutFlagChanged(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                                 const stw_types::uint32 ou32_DatapoolIndex,
                                 const stw_opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode oe_TimeoutMode);
   void m_OnRxTimeoutValueChanged(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                                  const stw_types::uint32 ou32_DatapoolIndex,
                                  const stw_types::uint32 ou32_TimeoutValue);
   void m_ConnectProtocolSpecificFields(void) const;
   void m_DisconnectProtocolSpecificFields(void) const;
   void m_ConnectNodeSpecificFields(void) const;
   void m_DisconnectNodeSpecificFields(void) const;
   void m_SetModeSpecificWidgetsVisible(const bool oq_BusMode) const;
   void m_SetNodeModeTransmitVisible(const bool oq_Visible) const;
   void m_SetNodeModeReceiveVisible(const bool oq_Visible) const;
   void m_ReloadNodes(void);
   void m_UpdateTxSelection(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds);
   void m_UpdateRxAfterTxSelection(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds,
      const bool oq_SkipDisconnect = false);
   void m_HandleCriticalMessagesAndRx(const bool oq_UpdateRx, const bool oq_HandleSignals = true);
   stw_types::sint32 m_GetVectorIndexOfComboBoxSelection(void) const;
   void m_GetComboBoxIndexesByVectorIndex(const stw_types::uint32 ou32_VectorIndex,
                                          stw_types::uint32 & oru32_ComboBoxNodeIndex,
                                          stw_types::uint32 & oru32_ComboBoxDatapoolIndex) const;

   void m_SyncCycleToLater(void) const;
   void m_SyncLaterToCycle(void) const;

   void m_BusModeAdaptTransmitterComboBoxesValid(const bool oq_Valid) const;

   void m_NodeModeDirectionChanged(const bool oq_Disconnect = true);
   void m_NodeModeTransmitDatapoolChanged(void);

   stw_types::uint32 m_GetDefaultTimeout(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigMessageIdChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigMessageNameChanged(void);
   void SigMessageDlcChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigRecheckError(void);
   void SigMessageRxChanged(void);
};
}

#endif // C_SDBUEMESSAGEPROPERTIESWIDGET_H
