//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a chart with its data configuration and data selection. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITCHARTWIDGET_H
#define C_SYVDAITCHARTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QLineSeries>
#include <QList>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QColor>
#include <vector>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

#include "stwtypes.h"

#include "C_OgeChaChartBase.h"
#include "C_OgeChaViewBase.h"

#include "C_PuiSvDbDataElementContent.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"
#include "C_PuiSvDbWidgetBase.h"
#include "C_PuiSvDbChart.h"

namespace Ui
{
class C_SyvDaItChartWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItChartWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItChartWidget(const stw_types::uint32 ou32_ViewIndex,
                                 const stw_types::uint32 ou32_MaximumDataElements, QWidget * const opc_Parent = NULL);
   virtual ~C_SyvDaItChartWidget();

   void SetData(const stw_opensyde_gui_logic::C_PuiSvDbChart & orc_Data);
   stw_opensyde_gui_logic::C_PuiSvDbChart & GetData(void);

   void SetDrawingActive(const bool oq_Active);
   void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode);
   void SetWidthOfDataSeriesSelector(const stw_types::sintn osn_Width) const;
   stw_types::sintn GetWidthOfDataSeriesSelector(void) const;
   bool GetCurrentDataSerie(stw_types::uint32 & oru32_DataPoolElementConfigIndex) const;

   void ConnectionActiveChanged(const bool oq_Active);

   stw_types::sint32 AddNewDataSerie(
      const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
      const stw_opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_ElementScaling);

   bool RemoveDataSerie(stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_ElementId);
   void SelectDataSeriesAxis(const stw_types::uint32 ou32_DataPoolElementConfigIndex);
   void AddDataSerieContent(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                            const QVector<stw_types::float64> & orc_Values,
                            const QVector<stw_types::uint32> & orc_Timestamps);
   void SetScaling(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const QString & orc_DisplayName,
                   const stw_opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_ElementScaling);
   stw_types::uint32 GetCountDataSeries(void) const;
   void UpdateTimeAxis(void);
   void UpdateValueAxis(void);
   void UpdateError(const stw_types::uint32 ou32_DataElementIndex, const QString & orc_ErrorText,
                    const bool oq_IsTransmissionError, const bool oq_ErrorActive) const;
   void UpdateTransparence(const stw_types::uint32 ou32_DataElementIndex, const stw_types::sintn osn_Value) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_SyvDaItChartWidget * mpc_Ui;

   const stw_types::uint32 mu32_ViewIndex;

   stw_opensyde_gui_logic::C_PuiSvDbChart mc_Data;
   stw_opensyde_gui_elements::C_OgeChaChartBase * mpc_Chart;
   stw_opensyde_gui_elements::C_OgeChaViewBase * mpc_ChartView;

   QtCharts::QValueAxis * mpc_AxisTime;
   QtCharts::QValueAxis * mpc_AxisValue;
   QtCharts::QValueAxis * mpc_AxisValueInvisible;

   const stw_types::uint32 mu32_MaximumDataElements;
   const stw_types::sintn msn_WidthLineSelected;
   const stw_types::sintn msn_WidthLineDefault;

   stw_types::float64 mf64_MaxTime;
   stw_types::float64 mf64_MaxValue;
   stw_types::float64 mf64_MinValue;
   bool mq_ValueZoomed;
   stw_types::float64 mf64_ZoomFactor;
   stw_types::float64 mf64_ScrollOffsetX;
   stw_types::float64 mf64_ScrollOffsetY;
   stw_types::float64 mf64_MaxValueZoomed;
   stw_types::float64 mf64_MinValueZoomed;
   stw_types::float64 mf64_ReferenceMaxValueZoomed;
   stw_types::float64 mf64_ReferenceMinValueZoomed;
   stw_types::float64 mf64_ReferenceRangeZoomed;
   stw_types::uint32 mu32_TimeStampOfStart;
   const stw_types::float64 mf64_DefaultTimeSlot;
   stw_types::float64 mf64_CurrentTimeSlot;

   std::vector<QtCharts::QLineSeries *> mc_DataPoolElementsDataSeries;
   std::vector<stw_types::uint32> mc_DataPoolElementsDataColorIndexes;
   // Saving for each datapool element the minimum content to have the type
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> mc_DataPoolElementContentMin;

   // Color configuration for the data series
   static const QColor mhac_DataColors[10];
   std::vector<bool> mc_DataColorsUsed;
   bool mq_DrawingActive;

   std::map<stw_types::uint32, stw_types::uint32> mc_ElementHandlerRegIndexToDataElementIndex;

   void m_AddDataSerie(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const bool oq_Warning = false,
                       const bool oq_Invalid = false, const QString & orc_InvalidPlaceholderName = "",
                       const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType = stw_opensyde_core::C_OSCNodeDataPool::eDIAG, const QString & orc_ToolTipErrorTextHeading = "", const QString & orc_ToolTipErrorText = "");
   void m_ResetChart(void);
   QColor m_GetColor(void);
   void m_DataItemToggled(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const bool oq_Checked);
   void m_DataItemSelected(const stw_types::uint32 ou32_DataPoolElementConfigIndex);
   void m_SettingZoomModeChanged(const stw_opensyde_gui_logic::C_PuiSvDbChart::E_SettingZoomMode oe_SettingZoomMode);
   void m_YZoomed(const stw_types::float64 of64_Factor);
   void m_Scrolled(const stw_types::float64 of64_X, const stw_types::float64 of64_Y);
   void m_ZoomReseted(void);

   //Avoid call
   C_SyvDaItChartWidget(const C_SyvDaItChartWidget &);
   C_SyvDaItChartWidget & operator =(const C_SyvDaItChartWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
