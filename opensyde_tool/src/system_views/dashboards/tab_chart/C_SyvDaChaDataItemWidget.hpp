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

#include "stwtypes.hpp"
#include "C_OgeWiWithToolTip.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaChaDataItemWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaChaDataItemWidget :
   public stw::opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   enum E_MeasurementValueState
   {
      eMEASUREMENT_VAL_DEACTIVATED,
      eMEASUREMENT_VAL_FIRST,
      eMEASUREMENT_VAL_SECOND
   };

   explicit C_SyvDaChaDataItemWidget(const uint32_t ou32_ViewIndex, QWidget * const opc_Parent = NULL);
   ~C_SyvDaChaDataItemWidget() override;

   void InitWidget(const uint32_t ou32_DataPoolElementConfigIndex,
                   const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                   const QString & orc_DisplayName, const bool oq_Active, const QColor & orc_DataPoolElementColor,
                   const QString & orc_Unit, const bool oq_Warning = false, const bool oq_Invalid = false,
                   const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType = stw::opensyde_core::C_OscNodeDataPool::eDIAG, const QString & orc_ToolTipErrorTextHeading = "", const QString & orc_ToolTipErrorText = "");
   void UpdateIndex(const uint32_t ou32_DataPoolElementConfigIndex);

   void UpdateColor(const QColor & orc_Color);
   void UpdateValue(const QString & orc_Value);
   void SetMeasurementState(const E_MeasurementValueState oe_MeasurementValueState);
   void UpdateMeasurementValue(const E_MeasurementValueState oe_MeasurementValueVariant, const QString & orc_Value);
   void UpdateError(const QString & orc_ErrorText, const bool oq_IsTransmissionError, const bool oq_ErrorActive);
   void ResetError(void);
   void UpdateTransparency(const int32_t os32_Value) const;

   QString GetDataElementName(void) const;
   QString GetDataElementUnit(void) const;
   bool GetDataElementToggledState(void) const;
   void SetDataElementUnit(const QString & orc_DisplayName, const QString & orc_Unit);

   void SetDataElementSelected(const bool oq_Selected);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDataItemToggled(const uint32_t ou32_DataPoolElementConfigIndex, const bool oq_Checked);
   void SigDataItemSelected(const uint32_t ou32_DataPoolElementConfigIndex);

protected:
   bool event(QEvent * const opc_Event) override;
   void paintEvent(QPaintEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;

private:
   Ui::C_SyvDaChaDataItemWidget * mpc_Ui;
   const uint32_t mu32_ViewIndex;
   stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId mc_DataPoolElementId;
   uint32_t mu32_DataPoolElementConfigIndex;
   bool mq_Selected;
   bool mq_Warning;
   bool mq_Error;
   bool mq_Invalid;

   QString mc_ToolTipErrorTextHeading;
   QString mc_ToolTipErrorText;
   stw::opensyde_core::C_OscNodeDataPool::E_Type me_PlaceholderDataPoolType;

   QString mc_Unit;

   QPixmap mc_IconSignal;
   QPixmap mc_IconVariable;
   QPixmap mc_IconParameter;
   QPixmap mc_IconHalInput;
   QPixmap mc_IconHalOutput;
   QPixmap mc_IconHalOther;
   QPixmap mc_IconSignalInactive;
   QPixmap mc_IconVariableInactive;
   QPixmap mc_IconParameterInactive;
   QPixmap mc_IconHalInputInactive;
   QPixmap mc_IconHalOutputInactive;
   QPixmap mc_IconHalOtherInactive;
   QPixmap mc_IconSignalWarning;
   QPixmap mc_IconVariableWarning;
   QPixmap mc_IconParameterWarning;
   QPixmap mc_IconHalInputWarning;
   QPixmap mc_IconHalOutputWarning;
   QPixmap mc_IconHalOtherWarning;
   QPixmap mc_IconSignalError;
   QPixmap mc_IconVariableError;
   QPixmap mc_IconParameterError;
   QPixmap mc_IconHalInputError;
   QPixmap mc_IconHalOutputError;
   QPixmap mc_IconHalOtherError;

   E_MeasurementValueState me_MeasurementValueState;
   QString mc_CurrentValue;
   QString mc_MeasurementValue1;
   QString mc_MeasurementValue2;

   void m_DataItemToggled(const bool oq_Checked);
   void m_UpdateIcon(void) const;

   //Avoid call
   C_SyvDaChaDataItemWidget(const C_SyvDaChaDataItemWidget &);
   C_SyvDaChaDataItemWidget & operator =(const C_SyvDaChaDataItemWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
