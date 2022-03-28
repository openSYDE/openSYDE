//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget which shows the QCustomplot and handles its chart functionality

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACHAPLOTHANDLERWIDGET_H
#define C_SYVDACHAPLOTHANDLERWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QColor>
#include <QTimer>
#include <vector>
#include <qcustomplot.h>

#include "stwtypes.h"

#include "C_PuiSvDbNodeDataPoolListElementId.h"
#include "C_PuiSvDbDataElementScaling.h"

#include "C_PuiSvDbTabChart.h"
#include "C_SyvDaChaPlot.h"

#include "C_OgeMuTabChartFit.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaChaPlotHandlerWidget;
}

namespace stw_opensyde_gui
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
   ~C_SyvDaChaPlotHandlerWidget(void);

   void InitStaticNames(void) const;
   void Init(const stw_types::uint32 ou32_MaximumDataElements);
   void SetData(const stw_opensyde_gui_logic::C_PuiSvDbTabChart & orc_Data, const stw_types::uint32 ou32_ViewIndex);
   stw_opensyde_gui_logic::C_PuiSvDbTabChart & GetData(void);

   void SetDarkMode(const bool oq_DarkMode);
   void SetDrawingActive(const bool oq_Active);

   bool IsPaused(void) const;
   void ConnectionActiveChanged(const bool oq_Active);

   stw_types::sint32 AddNewGraph(
      const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
      const stw_opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_ElementScaling);
   bool RemoveSpecificGraph(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                            stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_ElementId);
   bool RemoveCurrentGraph(stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_ElementId);
   void SelectGraph(const stw_types::uint32 ou32_DataPoolElementConfigIndex);
   bool GetCurrentGraph(stw_types::uint32 & oru32_DataPoolElementConfigIndex) const;
   bool IsADataSerieOnPosition(const QPoint & orc_Pos) const;
   void AddGraphContent(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                        const QVector<stw_types::float64> & orc_Values,
                        const QVector<stw_types::uint32> & orc_Timestamps);
   void SetScaling(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const QString & orc_DisplayName,
                   const stw_opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_ElementScaling);
   stw_types::uint32 GetCountGraphs(void) const;
   void UpdateError(const stw_types::uint32 ou32_DataElementIndex, const QString & orc_ErrorText,
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
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void showEvent(QShowEvent * const opc_Event) override;

private:
   Ui::C_SyvDaChaPlotHandlerWidget * mpc_Ui;

   // Menus
   stw_opensyde_gui_elements::C_OgeMuTabChartFit * mpc_MenuZoomMode;
   stw_opensyde_gui_elements::C_OgeMuTabChartFit * mpc_MenuCursorMode;
   stw_opensyde_gui_elements::C_OgeMuTabChartFit * mpc_MenuOptions;

   // Menu actions for zoom mode
   QAction * mpc_ActionZoomSettingXY;
   QAction * mpc_ActionZoomSettingX;
   QAction * mpc_ActionZoomSettingY;

   // Menu actions for cursor mode
   QAction * mpc_ActionCursorNoCursor;
   QAction * mpc_ActionCursorMeasurementCursor;
   QAction * mpc_ActionCursorTwoDiffCursor;

   // Menu actions for options
   QAction * mpc_ActionToggleSamples;
   QAction * mpc_ActionYAxisSettingAllVisible;

   stw_opensyde_gui_logic::C_PuiSvDbTabChart mc_Data;

   bool mq_Initialized;
   bool mq_DrawingActive;
   bool mq_DarkMode;

   stw_types::uint32 mu32_MaximumDataElements;
   static const stw_types::sintn mhsn_WIDTH_LINE_SELECTED;
   static const stw_types::sintn mhsn_WIDTH_LINE_DEFAULT;

   stw_types::float64 mf64_MaxValue;
   stw_types::float64 mf64_MinValue;
   stw_types::uint32 mu32_TimeStampOfStart;

   // Saving for each datapool element the minimum content to have the type
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> mc_DataPoolElementContentMin;

   // Color configuration for the data series
   static const stw_types::uint8 mhu8_COUNT_COLORS;
   static const QColor mhac_DATA_COLORS[47];
   std::vector<std::array<bool, 47> > mc_DataColorsUsed;

   std::map<stw_types::uint32, stw_types::uint32> mc_ElementHandlerRegIndexToDataElementIndex;

   // Index of list equals data element config index
   QList<QCPAxis *> mc_DataElementConfigIndexToYAxis;

   //Cyclic update
   bool mq_IsConnected;
   bool mq_ManualOperationPossible;
   bool mq_PauseState;
   bool mq_ShowSamplePointState;
   static const stw_types::sintn mhsn_INTERVAL_MS;
   QTimer mc_CyclicChartUpdateTrigger;

   // Cursors
   E_SettingCursorMode me_SettingCursorMode;
   C_SyvDaChaPlotCursorItem * mpc_FirstCursor;
   C_SyvDaChaPlotCursorItem * mpc_SecondCursor;
   stw_types::float64 mf64_MeasuredTimeFirstCursor;
   stw_types::float64 mf64_MeasuredTimeSecondCursor;
   bool mq_IsSecondCursorClicked;

   // Index of list equals data element config index
   QList<QCPItemTracer *> mc_ItemTracers;

   //Avoid call
   C_SyvDaChaPlotHandlerWidget(const C_SyvDaChaPlotHandlerWidget &);
   C_SyvDaChaPlotHandlerWidget & operator =(const C_SyvDaChaPlotHandlerWidget &);

   void m_AddGraph(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const bool oq_Warning = false,
                   const bool oq_Invalid = false, const QString & orc_InvalidPlaceholderName = "",
                   const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType = stw_opensyde_core::C_OSCNodeDataPool::eDIAG, const QString & orc_ToolTipErrorTextHeading = "", const QString & orc_ToolTipErrorText = "");
   void m_ResetChart(void);
   void m_DataItemToggled(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const bool oq_Checked);
   void m_DataItemSelected(const stw_types::uint32 ou32_DataPoolElementConfigIndex);
   void m_RedrawGraph(void);
   void m_UpdateElementCounter(void);
   void m_ResizeSelectorWidget(void);
   void m_OnSplitterMoved(void);
   void m_CyclicUpdateXAxis(void);
   void m_RangeChangedXAxis(void);
   void m_SetButtonIcons(const bool oq_DarkMode);

   // Color functions
   stw_types::uint8 m_GetNextNotUsedColor(void);
   void m_SetConcreteColorAsUsed(const stw_types::uint8 ou8_Index);
   void m_SetColorUnused(const stw_types::uint8 ou8_Index);
   void m_PrepareNextColorSection(void);

   // Y Axis functions
   QCPAxis * m_GetYAxis(const stw_types::uint32 ou32_DataPoolElementConfigIndex) const;
   void m_UpdateYAxisLabel(const stw_types::uint32 ou32_DataPoolElementConfigIndex);
   static void mh_AdaptYAxisStyle(QCPAxis * const opc_Axis, const bool oq_Selected);
   static void mh_AdaptAxisColor(QCPAxis * const opc_Axis, const QColor & orc_Color, const bool oq_DarkMode);
   void m_AdaptAllYAxisGridColors(const bool oq_DarkMode);
   static void mh_AdaptAxisGridColor(const QCPAxis * const opc_Axis, const bool oq_DarkMode);
   void m_AdaptYAxisWithSpace(QCPAxis * const opc_Axis);

   // Measurement cursor functions
   void m_CursorItemClicked(const C_SyvDaChaPlotCursorItem * const opc_CursorItem);
   void m_CursorItemReleased(C_SyvDaChaPlotCursorItem * const opc_CursorItem);
   void m_CursorItemMovedOnXAxis(const C_SyvDaChaPlotCursorItem * const opc_CursorItem,
                                 const stw_types::float64 of64_PosX);
   void m_UpdateMeasurementCursors(void);
   void m_CheckMeasurementCursorsVisibility(void);

   // Active zooming
   void m_ZoomIn(void);
   void m_ZoomOut(void);
   void m_Zoom(const bool oq_ZoomIn);

   // Configuration of zoom mode
   void m_ZoomModeXY(void);
   void m_ZoomModeX(void);
   void m_ZoomModeY(void);
   void m_SetZoomMode(const stw_opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingZoomMode oe_SettingZoomMode);
   void m_OnChangeZoomMode(const stw_opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingZoomMode oe_SettingZoomMode);
   void m_ConfigureZoomMode(const stw_opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingZoomMode oe_SettingZoomMode);

   // Configuration of drag mode
   void m_UpdateDragAxesConfiguration(void);

   // Switching between drag and zoom mode
   void m_SetZoomVsDragMode(const bool oq_Active);
   void m_ConfigureZoomVsDragMode(const bool oq_Active);

   // Configuration of y axes mode
   void m_YAxisSettingAllVisible(void);
   void m_SetYAxisMode(const stw_opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingYAxisMode oe_SettingYAxisMode);
   void m_ConfigureYAxisMode(const stw_opensyde_gui_logic::C_PuiSvDbTabChart::E_SettingYAxisMode oe_SettingYAxisMode);

   // Measurement cursor
   void m_CursorModeNoCursor(void);
   void m_CursorModeMeasurementCursor(void);
   void m_CursorModeTwoDiffCursor(void);
   void m_AdaptCursorMode(void);
   void m_ConfigureCursorMode(const E_SettingCursorMode oe_SettingCursorMode);
   void m_CreateCursor(C_SyvDaChaPlotCursorItem ** const oppc_ItemCursor, const stw_types::float64 of64_InitXPos,
                       const QColor & orc_Color, const QString & orc_LabelText);

   // Chart communication handling
   void m_PauseClicked(const bool oq_Pause);
   void m_UpdatePause(const bool oq_Pause);
   void m_ManualReadTriggered(void);

   // Signal fitting
   void m_FitXYSignals(void);
   void m_FitYSignals(void);
   void m_FitXSignals(void);

   // Sample points
   void m_ShowSamplePoints(const bool oq_ShowSamplePoints);

   // load and save
   void m_LoadState(const bool oq_IsPaused, const bool orq_AreSamplePointsShown,
                    const std::vector<std::array<stw_types::float64, 4> > & orc_ScreenState);
   void m_SaveState(bool & orq_IsPaused, stw_types::sint32 & ors32_SplitterLeftWidth, bool & orq_AreSamplePointsShown,
                    std::vector<std::array<stw_types::float64, 4> > & orc_ScreenState);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
