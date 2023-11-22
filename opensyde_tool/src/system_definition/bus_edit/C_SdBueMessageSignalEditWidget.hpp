//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing messages and / or signals

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGESIGNALEDITWIDGET_HPP
#define C_SDBUEMESSAGESIGNALEDITWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueMessageSignalEditWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageSignalEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageSignalEditWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessageSignalEditWidget(void) override;

   void SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value) const;
   void SetComProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Value);
   void SelectMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void SelectSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t & oru32_SignalIndex) const;
   void PartialSelectSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                            const uint32_t & oru32_SignalIndex) const;
   void Hide(void);
   void SetNodeId(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                  const std::vector<uint32_t> & orc_DatapoolIndexes) const;
   void SetBusId(const uint32_t ou32_BusIndex) const;
   void OnConnectionChange(void) const;
   void OnNodeDisconnected(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex);
   void OnSignalCountOfMessageChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId)
   const;
   void InitStaticNames(void) const;
   void SelectName(void) const;
   void RefreshSelection(void);
   void GetLastSelection(bool & orq_MessageSelected, QString * const opc_MessageName, bool & orq_SignalSelected,
                         QString * const opc_SignalName,
                         stw::opensyde_core::C_OscCanMessageIdentificationIndices * const opc_MessageId = NULL,
                         uint32_t * const opu32_SignalIndex = NULL) const;
   void RefreshColors(void) const;
   void ConnectAllChanges(void) const;
   void DisconnectAllChanges(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMessageIdChanged(void);
   void SigMessageNameChanged(void);
   void SigSignalNameChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalStartBitChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalPositionChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigRecheckErrorGlobal(void);
   void SigRecheckError(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                          const uint32_t & oru32_SignalIndex);
   void SigChanged(void);
   void SigMessageRxChanged(void);

   void SigAddSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint16_t ou16_StartBit);
   void SigAddSignalMultiplexed(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                const uint16_t ou16_StartBit, const uint16_t ou16_MultiplexValue);
   void SigCopySignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                      const uint32_t ou32_SignalIndex);
   void SigCutSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t ou32_SignalIndex);
   void SigPasteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                       const uint16_t ou16_StartBit);
   void SigDeleteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                        const uint32_t ou32_SignalIndex);

protected:
   void showEvent(QShowEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueMessageSignalEditWidget(const C_SdBueMessageSignalEditWidget &);
   C_SdBueMessageSignalEditWidget & operator =(const C_SdBueMessageSignalEditWidget &) &;
   void m_OnMessageIdChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_OnMessageNameChange(void);
   void m_OnMessageDlcChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnSignalUpdatedViaProperties(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                       const uint32_t ou32_SignalIndex)
   const;
   void m_OnSignalUpdatedViaSelector(void);
   void m_OnSignalActivated(const uint32_t ou32_SignalIndex);
   void m_OnSignalNameChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_OnSignalStartBitChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_OnSignalPositionChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_OnSignalTypeChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                              const uint32_t ou32_SignalIndex) const;
   void m_RecheckErrorGlobal(void);
   void m_RecheckError(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   int32_t m_GetMessageId(stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnChange(void);
   void m_HandleDisabledPdoInfo(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;

   Ui::C_SdBueMessageSignalEditWidget * mpc_Ui;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_ProtocolType;
};
}
}

#endif
