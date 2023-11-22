//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing signal properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUESIGNALPROPERTIESWIDGET_HPP
#define C_SDBUESIGNALPROPERTIESWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QDoubleSpinBox>
#include "stwtypes.hpp"
#include "C_OgeWiSpinBoxGroup.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"
#include "C_OgeSpxInt64Properties.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueSignalPropertiesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueSignalPropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueSignalPropertiesWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueSignalPropertiesWidget(void) noexcept override;

   void InitStaticNames(void) const;
   void SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void SetSignalId(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                    const uint32_t ou32_SignalIndex);
   void SetComProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Value);
   void ReloadPosition(void);
   stw::opensyde_core::C_OscCanMessageIdentificationIndices GetMessageId(void) const;
   uint32_t GetSignalIndex(void) const;
   void SelectName(void) const;
   void RecheckMessagePosition(void);

   void OnNodeDisconnected(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex);

private:
   //Avoid call
   C_SdBueSignalPropertiesWidget(const C_SdBueSignalPropertiesWidget &);
   C_SdBueSignalPropertiesWidget & operator =(const C_SdBueSignalPropertiesWidget &) &;

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
      eCHA_MUX_VALUE,
      eCHA_J1939_SPN
   };

   enum E_Type
   {
      eTY_UNSIGNED = 0,
      eTY_SIGNED = 1,
      eTY_FLOAT32 = 2,
      eTY_FLOAT64 = 3
   };

   Ui::C_SdBueSignalPropertiesWidget * mpc_Ui;

   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_ComProtocol;
   stw::opensyde_core::C_OscCanMessageIdentificationIndices mc_MessageId;
   uint32_t mu32_SignalIndex;
   E_Type me_DataType;
   stw::opensyde_core::C_OscCanSignal mc_DataOscSignal;
   stw::opensyde_gui_logic::C_PuiSdNodeCanSignal mc_DataUiSignal;
   stw::opensyde_core::C_OscNodeDataPoolListElement mc_DataOscSignalCommon;
   stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement mc_DataUiSignalCommon;

   void m_CheckSignalName(const bool & orq_SignalErrorChange = true);
   void m_CheckMuxType(const bool & orq_SignalErrorChange = true);
   void m_CheckMuxValue(const bool & orq_SignalErrorChange = true);
   void m_CheckMessagePosition(const bool & orq_SignalErrorChange = true);
   void m_HandleNameChangeWithoutSignal(void);
   void m_HandleNameChangeWithSignal(void);
   void m_HandleCommentChange(void);
   void m_LoadFromData(void);
   void m_CoLoadEdsRestricitions(void);
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
   void m_HandleJ1939SpnChange(void);
   static void mh_AdaptValueToSignalLength(const uint16_t ou16_BitLength,
                                           stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content);
   int32_t m_LoadGeneric(stw::opensyde_gui_elements::C_OgeWiSpinBoxGroup * const opc_Widget,
                         const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content, const float64_t of64_Factor,
                         const float64_t of64_Offset, const uint16_t ou16_BitLength,
                         const stw::opensyde_core::C_OscNodeDataPoolContent * const opc_Min,
                         const stw::opensyde_core::C_OscNodeDataPoolContent * const opc_Max) const;
   int32_t m_SaveGeneric(const stw::opensyde_gui_elements::C_OgeWiSpinBoxGroup * const opc_Widget,
                         stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content, const float64_t of64_Factor,
                         const float64_t of64_Offset) const;
   stw::opensyde_core::C_OscNodeDataPoolContent::E_Type m_GetCurrentType(void) const;
   void m_SaveToData(const E_Change oe_Change);
   stw::opensyde_core::C_OscCanSignal::E_MultiplexerType m_GetMuxType(void) const;
   void m_InitComboBox(const stw::opensyde_core::C_OscCanMessage & orc_Message, const uint32_t ou32_SignalIndex) const;
   void m_HandleMuxValueRange(const stw::opensyde_core::C_OscCanMessage & orc_Message,
                              const uint32_t ou32_SignalIndex) const;

   void m_HandleAnyChange(const E_Change oe_Change, const bool oq_AllowSignalsToInformOtherWidgets = true);
   void m_ApplyNewValueFromUi(const E_Change oe_Change);
   void m_AdaptOtherValues(const E_Change oe_Change, std::list<E_Change> & orc_Changes,
                           std::list<E_Change> & orc_ErrorChanges, QString & orc_UserNotificationText,
                           QString & orc_UserNotificationAdditionalInformation);
   void m_InitializeDataWithPotentialNewType(const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Type);
   void m_HandleMinValueRange(void);
   void m_HandleMaxValueRange(void);
   void m_HandleInitValueRange(void);
   void m_UpdateOtherSignalsForChange(const E_Change oe_Change) const;
   void m_UpdateUiForChange(const E_Change oe_Change);
   void m_UpdateErrorForChange(const E_Change oe_Change);
   void m_SendSignalForChange(const E_Change oe_Change);
   void m_ConnectAll(void) const;
   void m_DisconnectAll(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigNameChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigStartBitChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalPositionChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigUpdateMlv(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t ou32_SignalIndex);
   void SigRecheckError(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
};
}
}

#endif // C_SDBUESIGNALPROPERTIESWIDGET_HPP
