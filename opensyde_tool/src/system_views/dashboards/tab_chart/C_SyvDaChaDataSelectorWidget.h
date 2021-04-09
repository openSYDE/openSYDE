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

#include "stwtypes.h"

#include "C_PuiSvDbNodeDataPoolListElementId.h"
#include "C_SyvDaChaDataItemWidget.h"
#include "C_OSCNodeDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaChaDataSelectorWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaChaDataSelectorWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaChaDataSelectorWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SyvDaChaDataSelectorWidget(void);

   void SetView(const stw_types::uint32 ou32_ViewIndex);

   void AddDataSerie(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                     const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                     const QString & orc_DisplayName, const bool oq_Active, const QColor & orc_Color,
                     const QString & orc_Unit, const bool oq_Warning = false, const bool oq_Invalid = false,
                     const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType = stw_opensyde_core::C_OSCNodeDataPool::eDIAG, const QString & orc_ToolTipErrorTextHeading = "", const QString & orc_ToolTipErrorText = "");
   bool RemoveDataSerie(const stw_types::uint32 ou32_DataPoolElementConfigIndex);
   void UpdateDataSerieColor(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const QColor & orc_Color) const;
   void UpdateDataSerieValue(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const QString & orc_Value) const;
   void SetMeasurementState(const C_SyvDaChaDataItemWidget::E_MeasurementValueState oe_MeasurementValueState);
   void UpdateDataSerieMeasurementValue(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                                        const C_SyvDaChaDataItemWidget::E_MeasurementValueState oe_MeasurementValueVariant, const QString & orc_Value);
   void UpdateError(const stw_types::uint32 ou32_DataElementIndex, const QString & orc_ErrorText,
                    const bool oq_IsTransmissionError, const bool oq_ErrorActive) const;
   void ResetError(void) const;
   void UpdateTransparence(const stw_types::uint32 ou32_DataElementIndex, const stw_types::sintn osn_Value);
   QString GetDataElementName(const stw_types::uint32 ou32_DataPoolElementConfigIndex) const;
   QString GetDataElementUnit(const stw_types::uint32 ou32_DataPoolElementConfigIndex) const;
   bool GetDataElementToggledState(const stw_types::uint32 ou32_DataPoolElementConfigIndex) const;
   void SetDataElementUnit(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const QString & orc_DisplayName,
                           const QString & orc_Unit);
   bool GetCurrentDataSerie(stw_types::uint32 & oru32_DataPoolElementConfigIndex) const;
   bool IsADataSerieOnPosition(const QPoint & orc_Pos) const;
   void SelectDataSerie(const stw_types::uint32 ou32_DataPoolElementConfigIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDataItemToggled(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const bool oq_Checked);
   void SigDataItemSelected(const stw_types::uint32 ou32_DataPoolElementConfigIndex);

protected:
   virtual bool event(QEvent * const opc_Event) override;

private:
   Ui::C_SyvDaChaDataSelectorWidget * mpc_Ui;
   QSpacerItem * mpc_Spacer;
   stw_types::uint32 mu32_ViewIndex;

   std::vector<C_SyvDaChaDataItemWidget *> mc_DataPoolElementsDataItemWidgets;

   stw_types::uint32 mu32_CurrentSelectedIndex;
   C_SyvDaChaDataItemWidget::E_MeasurementValueState me_MeasurementValueState;

   void m_DataItemSelectedSlot(const stw_types::uint32 ou32_DataPoolElementConfigIndex);
   void m_DataItemSelected(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const bool oq_SendSignal = true);

   //Avoid call
   C_SyvDaChaDataSelectorWidget(const C_SyvDaChaDataSelectorWidget &);
   C_SyvDaChaDataSelectorWidget & operator =(const C_SyvDaChaDataSelectorWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
