//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing information and offering configuration of one chart data series (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACHADATAITEMWIDGET_H
#define C_SYVDACHADATAITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPixmap>

#include "stwtypes.h"
#include "C_OgeWiWithToolTip.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaChaDataItemWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaChaDataItemWidget :
   public stw_opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   enum E_MeasurementValueState
   {
      eMEASUREMENT_VAL_DEACTIVATED,
      eMEASUREMENT_VAL_FIRST,
      eMEASUREMENT_VAL_SECOND
   };

   explicit C_SyvDaChaDataItemWidget(const stw_types::uint32 ou32_ViewIndex, QWidget * const opc_Parent = NULL);
   virtual ~C_SyvDaChaDataItemWidget();

   void InitWidget(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                   const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                   const QString & orc_DisplayName, const bool oq_Active, const QColor & orc_DataPoolElementColor,
                   const QString & orc_Unit, const bool oq_Warning = false, const bool oq_Invalid = false,
                   const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType = stw_opensyde_core::C_OSCNodeDataPool::eDIAG, const QString & orc_ToolTipErrorTextHeading = "", const QString & orc_ToolTipErrorText = "");
   void UpdateIndex(const stw_types::uint32 ou32_DataPoolElementConfigIndex);

   void UpdateColor(const QColor & orc_Color);
   void UpdateValue(const QString & orc_Value);
   void SetMeasurementState(const E_MeasurementValueState oe_MeasurementValueState);
   void UpdateMeasurementValue(const E_MeasurementValueState oe_MeasurementValueVariant, const QString & orc_Value);
   void UpdateError(const QString & orc_ErrorText, const bool oq_IsTransmissionError, const bool oq_ErrorActive);
   void ResetError(void);
   void UpdateTransparence(const stw_types::sintn osn_Value) const;

   QString GetDataElementName(void) const;
   QString GetDataElementUnit(void) const;
   bool GetDataElementToggledState(void) const;
   void SetDataElementUnit(const QString & orc_DisplayName, const QString & orc_Unit);

   void SetDataElementSelected(const bool oq_Selected);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDataItemToggled(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const bool oq_Checked);
   void SigDataItemSelected(const stw_types::uint32 ou32_DataPoolElementConfigIndex);

protected:
   virtual bool event(QEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;

private:
   Ui::C_SyvDaChaDataItemWidget * mpc_Ui;
   const stw_types::uint32 mu32_ViewIndex;
   stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId mc_DataPoolElementId;
   stw_types::uint32 mu32_DataPoolElementConfigIndex;
   bool mq_Selected;
   bool mq_Warning;
   bool mq_Error;
   bool mq_Invalid;

   QString mc_ToolTipErrorTextHeading;
   QString mc_ToolTipErrorText;
   stw_opensyde_core::C_OSCNodeDataPool::E_Type me_PlaceholderDataPoolType;

   QString mc_Unit;

   QPixmap mc_IconSignal;
   QPixmap mc_IconVariable;
   QPixmap mc_IconParameter;
   QPixmap mc_IconHALInput;
   QPixmap mc_IconHALOutput;
   QPixmap mc_IconHALOther;
   QPixmap mc_IconSignalInactive;
   QPixmap mc_IconVariableInactive;
   QPixmap mc_IconParameterInactive;
   QPixmap mc_IconHALInputInactive;
   QPixmap mc_IconHALOutputInactive;
   QPixmap mc_IconHALOtherInactive;
   QPixmap mc_IconSignalWarning;
   QPixmap mc_IconVariableWarning;
   QPixmap mc_IconParameterWarning;
   QPixmap mc_IconHALInputWarning;
   QPixmap mc_IconHALOutputWarning;
   QPixmap mc_IconHALOtherWarning;
   QPixmap mc_IconSignalError;
   QPixmap mc_IconVariableError;
   QPixmap mc_IconParameterError;
   QPixmap mc_IconHALInputError;
   QPixmap mc_IconHALOutputError;
   QPixmap mc_IconHALOtherError;

   E_MeasurementValueState me_MeasurementValueState;
   QString mc_CurrentValue;
   QString mc_MeasurementValue1;
   QString mc_MeasurementValue2;

   void m_DataItemToggled(const bool oq_Checked);
   void m_UpdateIcon(void) const;

   //Avoid call
   C_SyvDaChaDataItemWidget(const C_SyvDaChaDataItemWidget &);
   C_SyvDaChaDataItemWidget & operator =(const C_SyvDaChaDataItemWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
