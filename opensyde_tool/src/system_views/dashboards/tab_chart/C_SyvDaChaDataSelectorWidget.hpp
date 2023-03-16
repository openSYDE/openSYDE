//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing information and offering configuration of all chart data series (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACHADATASELECTORWIDGET_H
#define C_SYVDACHADATASELECTORWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QSpacerItem>
#include <QColor>
#include <vector>

#include "stwtypes.hpp"

#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_SyvDaChaDataItemWidget.hpp"
#include "C_OscNodeDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaChaDataSelectorWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaChaDataSelectorWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaChaDataSelectorWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaChaDataSelectorWidget(void) override;

   void SetView(const uint32_t ou32_ViewIndex);

   void AddDataSerie(const uint32_t ou32_DataPoolElementConfigIndex,
                     const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                     const QString & orc_DisplayName, const bool oq_Active, const QColor & orc_Color,
                     const QString & orc_Unit, const bool oq_Warning = false, const bool oq_Invalid = false,
                     const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType = stw::opensyde_core::C_OscNodeDataPool::eDIAG, const QString & orc_ToolTipErrorTextHeading = "", const QString & orc_ToolTipErrorText = "");
   bool RemoveDataSerie(const uint32_t ou32_DataPoolElementConfigIndex);
   void UpdateDataSerieColor(const uint32_t ou32_DataPoolElementConfigIndex, const QColor & orc_Color) const;
   void UpdateDataSerieValue(const uint32_t ou32_DataPoolElementConfigIndex, const QString & orc_Value) const;
   void SetMeasurementState(const C_SyvDaChaDataItemWidget::E_MeasurementValueState oe_MeasurementValueState);
   void UpdateDataSerieMeasurementValue(const uint32_t ou32_DataPoolElementConfigIndex,
                                        const C_SyvDaChaDataItemWidget::E_MeasurementValueState oe_MeasurementValueVariant, const QString & orc_Value);
   void UpdateError(const uint32_t ou32_DataElementIndex, const QString & orc_ErrorText,
                    const bool oq_IsTransmissionError, const bool oq_ErrorActive) const;
   void ResetError(void) const;
   void UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value);
   QString GetDataElementName(const uint32_t ou32_DataPoolElementConfigIndex) const;
   QString GetDataElementUnit(const uint32_t ou32_DataPoolElementConfigIndex) const;
   bool GetDataElementToggledState(const uint32_t ou32_DataPoolElementConfigIndex) const;
   void SetDataElementUnit(const uint32_t ou32_DataPoolElementConfigIndex, const QString & orc_DisplayName,
                           const QString & orc_Unit);
   bool GetCurrentDataSerie(uint32_t & oru32_DataPoolElementConfigIndex) const;
   bool IsAnyDataSerieOnPosition(const QPoint & orc_Pos) const;
   void SelectDataSerie(const uint32_t ou32_DataPoolElementConfigIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDataItemToggled(const uint32_t ou32_DataPoolElementConfigIndex, const bool oq_Checked);
   void SigDataItemSelected(const uint32_t ou32_DataPoolElementConfigIndex);

protected:
   bool event(QEvent * const opc_Event) override;

private:
   Ui::C_SyvDaChaDataSelectorWidget * mpc_Ui;
   QSpacerItem * mpc_Spacer;
   uint32_t mu32_ViewIndex;

   std::vector<C_SyvDaChaDataItemWidget *> mc_DataPoolElementsDataItemWidgets;

   uint32_t mu32_CurrentSelectedIndex;
   C_SyvDaChaDataItemWidget::E_MeasurementValueState me_MeasurementValueState;

   void m_DataItemSelectedSlot(const uint32_t ou32_DataPoolElementConfigIndex);
   void m_DataItemSelected(const uint32_t ou32_DataPoolElementConfigIndex, const bool oq_SendSignal = true);

   //Avoid call
   C_SyvDaChaDataSelectorWidget(const C_SyvDaChaDataSelectorWidget &);
   C_SyvDaChaDataSelectorWidget & operator =(const C_SyvDaChaDataSelectorWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
