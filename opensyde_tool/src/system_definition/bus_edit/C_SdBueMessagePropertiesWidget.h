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
   void SetMessageId(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SetNodeDataPool(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex);
   void SetBusId(const stw_types::uint32 ou32_BusIndex);
   void OnConnectionChange(void);
   void SetComProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value);
   void SelectName(void) const;

   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> GetMatchingMessageIds(void) const;

private:
   //Avoid call
   C_SdBueMessagePropertiesWidget(const C_SdBueMessagePropertiesWidget &);
   C_SdBueMessagePropertiesWidget & operator =(const C_SdBueMessagePropertiesWidget &);

   Ui::C_SdBueMessagePropertiesWidget * mpc_Ui;
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ComProtocol;
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices mc_MessageId;
   stw_types::uint32 mu32_BusIndex;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_InterfaceIndex;
   bool mq_ModeSingleNode;
   std::vector<stw_types::uint32> mc_NodeIndexes;
   std::vector<stw_types::uint32> mc_InterfaceIndexes;
   bool mq_InternalRxChange;
   void m_CheckMessageName(void) const;
   void m_CheckMessageId(void) const;
   void m_CheckEarlyTime(void) const;
   void m_TrimmMessageName(void) const;
   void m_RegisterChange(void);
   void m_LoadFromData(void);
   void m_OnExtendedChange(const bool & orq_Extended) const;
   void m_OnTxMethodChange(const stw_types::sint32 & ors32_State) const;
   void m_OnDlcChange(const stw_types::sintn & orsn_NewValue) const;
   static stw_types::sint32 h_TxMethodToIndex(const stw_opensyde_core::C_OSCCanMessage::E_TxMethodType & ore_TxMethod);
   static stw_opensyde_core::C_OSCCanMessage::E_TxMethodType h_IndexToTxMethod(const stw_types::sint32 & ors32_Index);
   void m_OnNameChanged(void);
   void m_OnIdChanged(void);
   void m_OnDlcChanged(void);
   void m_OnCycleTimeChanged(void);
   void m_OnTxMethodChanged(void);
   void m_OnEarlyOrLaterTimeChanged(void);
   void m_OnPropertiesChanged(void);
   void m_OnDirectionChanged(void);
   void m_OnTxChanged(void);
   void m_OnRxChanged(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                      const bool oq_Checked);
   void m_OnRxTimeoutFlagChanged(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                                 const bool oq_UseAuto);
   void m_OnRxTimeoutValueChanged(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                                  const stw_types::uint32 ou32_TimeoutValue);
   void m_OnSingleTimeoutFlagChange(void);
   void m_OnSingleTimeoutValueChange(void);
   void m_ConnectProtocolSpecificFields(void);
   void m_DisconnectProtocolSpecificFields(void);
   void m_ConnectNodeSpecificFields(void);
   void m_DisconnectNodeSpecificFields(void);
   void m_ConnectAllChanges(void);
   void m_DisconnectAllChanges(void);
   void m_ReloadNodes(void);
   void m_UpdateTxSelection(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds) const;
   void m_UpdateRxAfterTxSelection(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds,
      const bool oq_SkipDisconnect = false);
   void m_HandleCriticalMessagesAndRx(const bool oq_HandleSignals = true);

   void m_SyncCycleToLater(void) const;
   void m_SyncLaterToCycle(void) const;

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
