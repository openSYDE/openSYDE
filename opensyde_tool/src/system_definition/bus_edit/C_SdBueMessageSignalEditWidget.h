//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing messages and / or signals

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGESIGNALEDITWIDGET_H
#define C_SDBUEMESSAGESIGNALEDITWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueMessageSignalEditWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageSignalEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageSignalEditWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueMessageSignalEditWidget(void);
   void SetMessageSyncManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value) const;
   void SetComProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value) const;
   void SelectMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void SelectSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint32 & oru32_SignalIndex) const;
   void SetNodeDataPool(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex) const;
   void SetBusId(const stw_types::uint32 ou32_BusIndex) const;
   void OnConnectionChange(void) const;
   void OnSignalCountOfMessageChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId)
   const;
   void InitStaticNames(void) const;
   void SelectName(void) const;
   void GetLastSelection(bool & orq_MessageSelected, QString & orc_MessageName, bool & orq_SignalSelected,
                         QString & orc_SignalName) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMessageIdChanged(void);
   void SigMessageNameChanged(void);
   void SigSignalNameChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigRecheckErrorGlobal(void);
   void SigRecheckError(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalSelected(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                          const stw_types::uint32 & oru32_SignalIndex);
   void SigChanged(void);
   void SigMessageRxChanged(void);

   void SigAddSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint16 ou16_StartBit);
   void SigCopySignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                      const stw_types::uint32 ou32_SignalIndex);
   void SigCutSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint32 ou32_SignalIndex);
   void SigPasteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                       const stw_types::uint16 ou16_StartBit);
   void SigDeleteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                        const stw_types::uint32 ou32_SignalIndex);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdBueMessageSignalEditWidget(const C_SdBueMessageSignalEditWidget &);
   C_SdBueMessageSignalEditWidget & operator =(const C_SdBueMessageSignalEditWidget &);
   void m_OnMessageIdChange(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void m_OnMessageNameChange(void);
   void m_OnMessageDlcChange(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnSignalUpdatedViaProperties(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                       const stw_types::uint32 ou32_SignalIndex)
   const;
   void m_OnSignalUpdatedViaSelector(void);
   void m_OnSignalActivated(const stw_types::uint32 ou32_SignalIndex);
   void m_OnSignalNameChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                              const stw_types::uint32 ou32_SignalIndex);
   void m_OnSignalTypeChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                              const stw_types::uint32 ou32_SignalIndex) const;
   void m_RecheckErrorGlobal(void);
   void m_RecheckError(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   stw_types::sint32 m_GetMessageId(stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void m_OnChange(void);

   Ui::C_SdBueMessageSignalEditWidget * mpc_Ui;
};
}

#endif
