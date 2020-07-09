//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing signal properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUESIGNALPROPERTIESWIDGET_H
#define C_SDBUESIGNALPROPERTIESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QDoubleSpinBox>
#include "stwtypes.h"
#include "C_OgeWiSpinBoxGroup.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"
#include "C_OgeSpxInt64Properties.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueSignalPropertiesWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueSignalPropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueSignalPropertiesWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueSignalPropertiesWidget(void);

   void InitStaticNames(void) const;
   void SetMessageSyncManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void SetSignalId(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                    const stw_types::uint32 ou32_SignalIndex);
   void ReloadPosition(void);
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices GetMessageId(void) const;
   stw_types::uint32 GetSignalIndex(void) const;
   void SelectName(void) const;

   void OnNodeDisconnected(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex);

private:
   //Avoid call
   C_SdBueSignalPropertiesWidget(const C_SdBueSignalPropertiesWidget &);
   C_SdBueSignalPropertiesWidget & operator =(const C_SdBueSignalPropertiesWidget &);

   enum E_Change
   {
      eCHA_NAME,
      eCHA_COMMENT,
      eCHA_AUTO_MIN_MAX,
      eCHA_MIN,
      eCHA_MAX,
      eCHA_FACTOR,
      eCHA_OFFSET,
      eCHA_INIT,
      eCHA_UNIT,
      eCHA_BYTE_ORDER,
      eCHA_VALUE_TYPE,
      eCHA_MLV,
      eCHA_LENGTH,
      eCHA_START_BIT,
      eCHA_MUX_TYPE,
      eCHA_MUX_VALUE
   };

   enum E_Type
   {
      eTY_UNSIGNED = 0,
      eTY_SIGNED = 1,
      eTY_FLOAT32 = 2,
      eTY_FLOAT64 = 3
   };

   Ui::C_SdBueSignalPropertiesWidget * mpc_Ui;

   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices mc_MessageId;
   stw_types::uint32 mu32_SignalIndex;
   E_Type me_DataType;
   stw_opensyde_core::C_OSCCanSignal mc_DataOSCSignal;
   stw_opensyde_gui_logic::C_PuiSdNodeCanSignal mc_DataUiSignal;
   stw_opensyde_core::C_OSCNodeDataPoolListElement mc_DataOSCSignalCommon;
   stw_opensyde_gui_logic::C_PuiSdNodeDataPoolListElement mc_DataUiSignalCommon;

   void m_CheckSignalName(const bool & orq_SignalErrorChange = true);
   void m_CheckMUXType(const bool & orq_SignalErrorChange = true);
   void m_CheckMUXValue(const bool & orq_SignalErrorChange = true);
   void m_CheckMessagePosition(const bool & orq_SignalErrorChange = true);
   void m_HandleNameChangeWithoutSignal(void);
   void m_HandleNameChangeWithSignal(void);
   void m_HandleCommentChange(void);
   void m_LoadFromData(void);
   void m_HandleAutoMinMaxCheckBoxChange(void);
   void m_HandleMinChange(void);
   void m_HandleMaxChange(void);
   void m_HandleFactorChange(void);
   void m_HandleOffsetChange(void);
   void m_HandleInitChange(void);
   void m_HandleUnitChange(void);
   void m_HandleByteOrderChange(void);
   void m_HandleValueTypeChange(void);
   void m_HandleValueLengthChange(void);
   void m_HandleStartBitChange(void);
   void m_HandleMuxTypeChange(void);
   void m_HandleMuxValueChange(void);
   static void mh_AdaptValueToSignalLength(const stw_types::uint16 ou16_BitLength,
                                           stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content);
   stw_types::sint32 m_LoadGeneric(stw_opensyde_gui_elements::C_OgeWiSpinBoxGroup * const opc_Widget,
                                   const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                   const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset,
                                   const stw_types::uint16 ou16_BitLength,
                                   const stw_opensyde_core::C_OSCNodeDataPoolContent * const opc_Min,
                                   const stw_opensyde_core::C_OSCNodeDataPoolContent * const opc_Max) const;
   static stw_types::sint32 mh_InitMin(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                       const stw_types::uint16 ou16_BitLength);
   static stw_types::sint32 mh_InitMax(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                       const stw_types::uint16 ou16_BitLength);
   stw_types::sint32 m_SaveGeneric(const stw_opensyde_gui_elements::C_OgeWiSpinBoxGroup * const opc_Widget,
                                   stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                   const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset) const;
   stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type m_GetCurrentType(void) const;
   void m_SaveToData(const E_Change oe_Change);
   stw_opensyde_core::C_OSCCanSignal::E_MultiplexerType m_GetMuxType(void) const;
   void m_InitComboBox(const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                       const stw_types::uint32 ou32_SignalIndex) const;
   void m_HandleMuxValueRange(const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                              const stw_types::uint32 ou32_SignalIndex) const;

   void m_HandleAnyChange(const E_Change oe_Change, const bool oq_AllowSignalsToInformOtherWidgets = true);
   void m_ApplyNewValueFromUI(const E_Change oe_Change);
   void m_AdaptOtherValues(const E_Change oe_Change, std::list<E_Change> & orc_Changes,
                           std::list<E_Change> & orc_ErrorChanges, QString & orc_UserNotificationText,
                           QString & orc_UserNotificationAdditionalInformation);
   void m_InitializeDataWithPotentialNewType(const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_Type);
   void m_HandleMinValueRange(void);
   void m_HandleMaxValueRange(void);
   void m_HandleInitValueRange(void);
   void m_UpdateOtherSignalsForChange(const E_Change oe_Change) const;
   void m_UpdateUIForChange(const E_Change oe_Change);
   void m_UpdateErrorForChange(const E_Change oe_Change);
   void m_SendSignalForChange(const E_Change oe_Change);
   void m_ConnectAll(void);
   void m_DisconnectAll(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigNameChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigStartBitChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigUpdateMlv(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint32 ou32_SignalIndex);
   void SigRecheckError(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
};
}

#endif // C_SDBUESIGNALPROPERTIESWIDGET_H
