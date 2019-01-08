//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for editing signal properties

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUESIGNALPROPERTIESWIDGET_H
#define C_SDBUESIGNALPROPERTIESWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QDoubleSpinBox>
#include "stwtypes.h"
#include "C_OgeWiSpinBoxGroup.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"
#include "C_OgeSpxInt64Properties.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SdBueSignalPropertiesWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

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
   void ReloadPosition(const bool & orq_SignalErrorChange = true);
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices GetMessageId(void) const;
   stw_types::uint32 GetSignalIndex(void) const;
   void SetProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Type);
   void SelectName(void) const;

private:
   //Avoid call
   C_SdBueSignalPropertiesWidget(const C_SdBueSignalPropertiesWidget &);
   C_SdBueSignalPropertiesWidget & operator =(const C_SdBueSignalPropertiesWidget &);

   Ui::C_SdBueSignalPropertiesWidget * mpc_Ui;
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices mc_MessageId;
   stw_types::uint32 mu32_SignalIndex;
   bool mq_DataChangesAllowed;
   bool mq_PositionUpdate;

   void m_CheckSignalName(const bool & orq_SignalErrorChange = true);
   void m_CheckMessagePosition(const bool & orq_SignalErrorChange = true);
   void m_CheckMinMaxAndInitValue(const bool & orq_SignalErrorChange = true);
   void m_TrimmMessageName(void) const;
   void m_RegisterChange(void);
   void m_OnTextChanged(void);
   void m_RegisterNameChange(void);
   void m_RegisterScalingChange(void);
   void m_RegisterPositionChange(void);
   void m_RegisterMinMaxAndInitChange(void);
   void m_LoadFromData(void);
   void m_HandleAutoMinMaxCheckBox(const bool & orq_AutoMinMaxActive);
   void m_HandleValueType(const stw_types::sint32 & ors32_Index);
   void m_HandleValueLengthChange(const stw_types::sint32 & ors32_Value);
   void m_UpdateAutoMinMax(void);
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
   void m_ReInitMinMaxAndInit(const stw_types::float64 * const opf64_Factor = NULL,
                              const stw_types::float64 * const opf64_Offset = NULL) const;
   void m_SaveToData(void);
   void m_ConnectPositionUpdate(void);
   void m_DisconnectPositionUpdate(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigNameChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                       const stw_types::uint32 ou32_SignalIndex);
   void SigPositionChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                           const stw_types::uint32 ou32_SignalIndex);
   void SigTypeChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                       const stw_types::uint32 ou32_SignalIndex);
   void SigRecheckError(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
};
}

#endif // C_SDBUESIGNALPROPERTIESWIDGET_H
