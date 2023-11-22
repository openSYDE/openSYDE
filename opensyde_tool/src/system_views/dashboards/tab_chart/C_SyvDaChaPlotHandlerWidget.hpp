//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget which shows the QCustomplot and handles its chart functionality

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACHAPLOTHANDLERWIDGET_HPP
#define C_SYVDACHAPLOTHANDLERWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QColor>
#include <QTimer>
#include <vector>
#include <qcustomplot.h>

#include "stwtypes.hpp"

#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_PuiSvDbDataElementScaling.hpp"
#include "C_PuiSvDbDataElementDisplayFormatterConfig.hpp"

#include "C_PuiSvDbTabChart.hpp"
#include "C_SyvDaChaPlot.hpp"

#include "C_OgeMuTabChartFit.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaChaPlotHandlerWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaChaPlotHandlerWidget :
   public QWidget
{
   Q_OBJECT

public:
   enum E_SettingCursorMode
   {
      eSETTING_CM_NO_CURSOR,
      eSETTING_CM_MEASUREMENT_CURSOR,
      eSETTING_CM_TWO_DIFF_CURSOR
   };

   explicit C_SyvDaChaPlotHandlerWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaChaPlotHandlerWidget(void) override;

   void InitStaticNames(void) const;
   void Init(const uint32_t ou32_MaximumDataElements);
   void SetData(const stw::opensyde_gui_logic::C_PuiSvDbTabChart & orc_Data, const uint32_t ou32_ViewIndex);
   stw::opensyde_gui_logic::C_PuiSvDbTabChart & GetData(void);

   void SetDisplayFormatterConfig(
      const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
      const stw::opensyde_gui_logic::C_PuiSvDbDataElementDisplayFormatterConfig & orc_Config);

   void SetDarkMode(const bool oq_DarkMode);
   void SetDrawingActive(const bool oq_Active);

   bool IsPaused(void) const;
   void ConnectionActiveChanged(const bool oq_Active);

   int32_t AddNewGraph(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                       const stw::opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_ElementScaling);
   bool RemoveSpecificGraph(const uint32_t ou32_DataPoolElementConfigIndex,
                            stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_ElementId);
   bool RemoveCurrentGraph(stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_ElementId);
   void SelectGraph(const uint32_t ou32_DataPoolElementConfigIndex);
   bool GetCurrentGraph(uint32_t & oru32_DataPoolElementConfigIndex) const;
   bool IsAnyDataSerieOnPosition(const QPoint & orc_Pos) const;
   void AddGraphContent(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                        const QString & orc_FormattedLastValue, const QVector<float64_t> & orc_Values,
                        const QVector<uint32_t> & orc_Timestamps);
   void SetScaling(const uint32_t ou32_DataPoolElementConfigIndex, const QString & orc_DisplayName,
                   const stw::opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_ElementScaling);
   uint32_t GetCountGraphs(void) const;
   void UpdateError(const uint32_t ou32_DataElementIndex, const QString & orc_ErrorText,
                    const bool oq_IsTransmissionError, const bool oq_ErrorActive) const;

   void FitDefault(void);

   void RefreshColors(void);

   void SetManualOperationPossible(const bool oq_Possible);
   void ManualReadFinished(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChartDataChanged(void);
   void SigAddSignalClicked(void);

   void SigManualReadTriggered(void);
   void SigManualReadAbortTriggered(void);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void showEvent(QShowEvent * const opc_Event) override;

private:
   Ui::C_SyvDaChaPlotHandlerWidget * mpc_Ui;

   // Menus
   stw::opensyde_gui_elements::C_OgeMuTabChartFit * mpc_MenuZoomMode;
   stw::opensyde_gui_elements::C_OgeMuTabChartFit * mpc_MenuCursorMode;
   stw::opensyde_gui_elements::C_OgeMuTabChartFit * mpc_MenuOptions;

   // Menu actions for zoom mode
   QAction * mpc_ActionZoomSettingHorizontalVertical;
   QAction * mpc_ActionZoomSettingHorizontal;
   QAction * mpc_ActionZoomSettingVertical;

   // Menu actions for cursor mode
   QAction * mpc_ActionCursorNoCursor;
   QAction * mpc_ActionCursorMeasurementCursor;
   QAction * mpc_ActionCursorTwoDiffCursor;

   // Menu actions for options
   QAction * mpc_ActionToggleSamples;
   QAction * mpc_ActionVerticalAxisSettingAllVisible;

   stw::opensyde_gui_logic::C_PuiSvDbTabChart mc_Data;

   bool mq_Initialized;
   bool mq_DrawingActive;
   bool mq_DarkMode;

   uint32_t mu32_MaximumDataElements;
   static const int32_t mhs32_WIDTH_LINE_SELECTED;
   static const int32_t mhs32_WIDTH_LINE_DEFAULT;

   float64_t mf64_MaxValue;
   float64_t mf64_MinValue;
   uint32_t mu32_TimeStampOfStart;

   // Saving for each datapool element the minimum content to have the type
   std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> mc_DataPoolElementContentMin;
   std::vector<stw::opensyde_gui_logic::C_PuiSvDbDataElementDisplayFormatterConfig> mc_DataPoolElementDisplayFormatter;

   // Color configuration for the data series
   static const uint8_t mhu8_COUNT_COLORS;
   static const QColor mhac_DATA_COLORS[47];
   std::vector<std::array<bool, 47> > mc_DataColorsUsed;

   std::map<uint32_t, uint32_t> mc_ElementHandlerRegIndexToDataElementIndex;

   // Index of list equals data element config index
   QList<QCPAxis *> mc_DataElementConfigIndexToVerticalAxis;

   //Cyclic update
   bool mq_IsConnected;
   bool mq_ManualOperationPossible;
   bool mq_PauseState;
   bool mq_ShowSamplePointState;
   static const int32_t mhs32_INTERVAL_MS;
   QTimer mc_CyclicChartUpdateTrigger;

   // Cursors
   E_SettingCursorMode me_SettingCursorMode;
   C_SyvDaChaPlotCursorItem * mpc_FirstCursor;
   C_SyvDaChaPlotCursorItem * mpc_SecondCursor;
   float64_t mf64_MeasuredTimeFirstCursor;
   float64_t mf64_MeasuredTimeSecondCursor;
   bool mq_IsSecondCursorClicked;

   // Index of list equals data element config index
   QList<QCPItemTracer *> mc_ItemTracers;

   //Avoid call
   C_SyvDaChaPlotHandlerWidget(const C_SyvDaChaPlotHandlerWidget &);
   C_SyvDaChaPlotHandlerWidget & operator =(const C_SyvDaChaPlotHandlerWidget &) &;

   void m_AddGraph(const uint32_t ou32_DataPoolElementConfigIndex, const bool oq_Warning = false,
                   const bool oq_Invalid = false, const QString & orc_InvalidPlaceholderName = "",
                   const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType = stw::opensyde_core::C_OscNodeDataPool::eDIAG, const QString & orc_ToolTipErrorTextHeading = "", const QString & orc_ToolTipErrorText = "");
   void m_ResetChart(void);
   void m_DataItemToggled(const uint32_t ou32_DataPoolElementConfigIndex, const bool oq_Checked);
   void m_DataItemSelected(const uint32_t ou32_DataPoolElementConfigIndex);
   void m_RedrawGraph(void);
   void m_UpdateElementCounter(void);
   void m_ResizeSelectorWidget(void);
   void m_OnSplitterMoved(void);
   void m_CyclicUpdateHorizontalAxis(void);
   void m_RangeChangedHorizontalAxis(void);
   void m_SetButtonIcons(const bool oq_DarkMode);

   // Color functions
   uint8_t m_GetNextNotUsedColor(void);
   void m_SetConcreteColorAsUsed(const uint8_t ou8_Index);
   void m_SetColorUnused(const uint8_t ou8_Index);
   void m_PrepareNextColorSection(void);

   // Y Axis functions
   QCPAxis * m_GetVerticalAxis(const uint32_t ou32_DataPoolElementConfigIndex) const;
   void m_UpdateVerticalAxisLabel(const uint32_t ou32_DataPoolElementConfigIndex);
   static void mh_AdaptVerticalAxisStyle(QCPAxis * const opc_Axis, const bool oq_Selected);
   static void mh_AdaptAxisColor(QCPAxis * const opc_Axis, const QColor & orc_Color, const bool oq_DarkMode);
   void m_AdaptAllVerticalAxisGridColors(const bool oq_DarkMode);
   static void mh_AdaptAxisGridColor(const QCPAxis * const opc_Axis, const bool oq_DarkMode);
   void m_AdaptVerticalAxisWithSpace(QCPAxis * const opc_Axis);

   // Measurement cursor functions
   void m_CursorItemClicked(const C_SyvDaChaPlotCursorItem * const opc_CursorItem);
   void m_CursorItemReleased(C_SyvDaChaPlotCursorItem * const opc_CursorItem);
   void m_CursorItemMovedOnHorizontalAxis(const C_SyvDaChaPlotCursorItem * const opc_CursorItem,
                                          const float64_t of64_PosHorizontal);
   void m_UpdateMeasurementCursors(void);
   void m_CheckMeasurementCursorsVisibility(void);

   // Active zooming
   void m_ZoomIn(void);
   void m_ZoomOut(void);
   void m_Zoom(const bool oq_ZoomIn);

   // Configuration of zoom mode
   void m_ZoomModeHorizontalVertical(void);
   void m_ZoomModeHorizontal(void);
   void m_ZoomModeVertical(void);
   void m_SetZoomMode(const stw::opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingZoomMode oe_SettingZoomMode);
   void m_OnChangeZoomMode(const stw::opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingZoomMode oe_SettingZoomMode);
   void m_ConfigureZoomMode(const stw::opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingZoomMode oe_SettingZoomMode);

   // Configuration of drag mode
   void m_UpdateDragAxesConfiguration(void);

   // Switching between drag and zoom mode
   void m_SetZoomVsDragMode(const bool oq_Active);
   void m_ConfigureZoomVsDragMode(const bool oq_Active);

   // Configuration of y axes mode
   void m_VerticalAxisSettingAllVisible(void);
   void m_SetVerticalAxisMode(
      const stw::opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingVerticalAxisMode oe_SettingVerticalAxisMode);
   void m_ConfigureVerticalAxisMode(
      const stw::opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingVerticalAxisMode oe_SettingVerticalAxisMode);

   // Measurement cursor
   void m_CursorModeNoCursor(void);
   void m_CursorModeMeasurementCursor(void);
   void m_CursorModeTwoDiffCursor(void);
   void m_AdaptCursorMode(void);
   void m_ConfigureCursorMode(const E_SettingCursorMode oe_SettingCursorMode);
   void m_CreateCursor(C_SyvDaChaPlotCursorItem ** const oppc_ItemCursor, const float64_t of64_InitHorizontalPos,
                       const QColor & orc_Color, const QString & orc_LabelText);

   // Chart communication handling
   void m_PauseClicked(const bool oq_Pause);
   void m_UpdatePause(const bool oq_Pause);
   void m_ManualReadTriggered(void);

   // Signal fitting
   void m_FitHorizontalVerticalSignals(void);
   void m_FitVerticalSignals(void);
   void m_FitHorizontalSignals(void);

   // Sample points
   void m_ShowSamplePoints(const bool oq_ShowSamplePoints);

   // load and save
   void m_LoadState(const bool oq_IsPaused, const bool orq_AreSamplePointsShown, const std::vector<std::array<float64_t,
                                                                                                              4> > & orc_ScreenState);
   void m_SaveState(bool & orq_IsPaused, int32_t & ors32_SplitterLeftWidth, bool & orq_AreSamplePointsShown,
                    std::vector<std::array<float64_t, 4> > & orc_ScreenState);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
