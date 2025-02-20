//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget which shows the QCustomplot and handles its chart functionality

   Widget which shows the QCustomplot and handles its chart functionality

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPushButton>

#include "C_SyvDaChaPlotHandlerWidget.hpp"
#include "ui_C_SyvDaChaPlotHandlerWidget.h"

#include "constants.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "TglTime.hpp"
#include "TglFile.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OscUtils.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiSvData.hpp"
#include "C_PuiSvDbNodeDataElementConfig.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_PuiProject.hpp"
#include "C_Uti.hpp"
#include "C_PuiUtil.hpp"
#include "C_SclString.hpp"
#include "C_UsHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint8_t C_SyvDaChaPlotHandlerWidget::mhu8_COUNT_COLORS = 47U;
const QColor C_SyvDaChaPlotHandlerWidget::mhac_DATA_COLORS[mhu8_COUNT_COLORS] =
{
   QColor(192, 0, 0),
   QColor(0, 112, 192),
   QColor(0, 176, 80),
   QColor(255, 0, 0),
   QColor(255, 192, 0),
   QColor(146, 208, 80),
   QColor(0, 176, 240),
   QColor(112, 48, 160),
   QColor(244, 154, 193),
   QColor(246, 153, 157),
   QColor(110, 208, 246),
   QColor(123, 204, 199),
   QColor(0, 174, 239),
   QColor(203, 219, 43),
   QColor(144, 129, 125),
   QColor(0, 112, 70),
   QColor(255, 28, 129),
   QColor(189, 141, 191),
   QColor(189, 27, 141),
   QColor(0, 112, 127),
   QColor(254, 58, 163),
   QColor(126, 168, 217),
   QColor(146, 39, 143),
   QColor(211, 74, 162),
   QColor(0, 114, 188),
   QColor(0, 171, 196),
   QColor(86, 86, 179),
   QColor(247, 151, 122),
   QColor(237, 20, 91),
   QColor(0, 140, 142),
   QColor(248, 148, 30),
   QColor(195, 179, 139),
   QColor(0, 138, 75),
   QColor(193, 138, 123),
   QColor(192, 36, 94),
   QColor(200, 177, 47),
   QColor(0, 142, 176),
   QColor(196, 137, 47),
   QColor(131, 202, 157),
   QColor(139, 165, 141),
   QColor(149, 44, 97),
   QColor(0, 166, 80),
   QColor(141, 198, 63),
   QColor(145, 162, 61),
   QColor(148, 127, 58),
   QColor(151, 46, 52),
   QColor(0, 169, 157)
};

const int32_t C_SyvDaChaPlotHandlerWidget::mhs32_WIDTH_LINE_SELECTED = 2;
const int32_t C_SyvDaChaPlotHandlerWidget::mhs32_WIDTH_LINE_DEFAULT = 1;
const int32_t C_SyvDaChaPlotHandlerWidget::mhs32_INTERVAL_MS = 10;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaPlotHandlerWidget::C_SyvDaChaPlotHandlerWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaChaPlotHandlerWidget),
   mq_Initialized(false),
   mq_DrawingActive(true),
   mq_DarkMode(false),
   mu32_MaximumDataElements(1U),
   mf64_MaxValue(5.0),
   mf64_MinValue(0.0),
   mu32_TimeStampOfStart(0U),
   mq_IsConnected(false),
   mq_ManualOperationPossible(false),
   mq_PauseState(false),
   mq_ShowSamplePointState(true),
   me_SettingCursorMode(eSETTING_CM_NO_CURSOR),
   mpc_FirstCursor(NULL),
   mpc_SecondCursor(NULL),
   mf64_MeasuredTimeFirstCursor(0.0),
   mf64_MeasuredTimeSecondCursor(0.0),
   mq_IsSecondCursorClicked(false),
   mc_Out(&mc_File)
{
   const bool q_ServiceModeActive = C_PuiSvHandler::h_GetInstance()->GetServiceModeActive();

   //Menus
   this->mpc_MenuZoomMode = new C_OgeMuTabChartFit(this);
   this->mpc_MenuCursorMode = new C_OgeMuTabChartFit(this);
   this->mpc_MenuOptions = new C_OgeMuTabChartFit(this);

   this->mpc_Ui->setupUi(this);

   // Button icons are set in m_SetButtonIcons on dark mode set resp. in m_OnChangeZoomMode for zoom icons

   this->mpc_Ui->pc_PushButtonManualRead->setEnabled(false);
   this->mpc_Ui->pc_PushButtonManualAbort->setEnabled(false);
   this->mpc_Ui->pc_PushButtonManualAbort->setVisible(false);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_PushButtonPause, "BackgroundChecked", false);

   // initially cursor labels should be invisible
   this->mpc_Ui->pc_LabelMeasCursor1->setVisible(false);
   this->mpc_Ui->pc_LabelMeasCursor2->setVisible(false);
   this->mpc_Ui->pc_LabelMeasDiff->setVisible(false);

   // Zoom setting
   this->mpc_ActionZoomSettingHorizontal =
      this->mpc_MenuZoomMode->addAction(C_GtGetText::h_GetText("Adjust X-Axis"),
                                        this, &C_SyvDaChaPlotHandlerWidget::m_ZoomModeHorizontal);
   this->mpc_ActionZoomSettingHorizontal->setCheckable(true);

   this->mpc_ActionZoomSettingVertical =
      this->mpc_MenuZoomMode->addAction(C_GtGetText::h_GetText("Adjust Y-Axis"),
                                        this, &C_SyvDaChaPlotHandlerWidget::m_ZoomModeVertical);
   this->mpc_ActionZoomSettingVertical->setCheckable(true);

   this->mpc_ActionZoomSettingHorizontalVertical =
      this->mpc_MenuZoomMode->addAction(C_GtGetText::h_GetText("Adjust All Directions"),
                                        this, &C_SyvDaChaPlotHandlerWidget::m_ZoomModeHorizontalVertical);
   this->mpc_ActionZoomSettingHorizontalVertical->setCheckable(true);

   this->mpc_Ui->pc_PushButtonZoomMode->setMenu(this->mpc_MenuZoomMode);

   // Cursor setting
   this->mpc_ActionCursorNoCursor =
      this->mpc_MenuCursorMode->addAction(C_GtGetText::h_GetText("No Cursor"),
                                          this, &C_SyvDaChaPlotHandlerWidget::m_CursorModeNoCursor);
   this->mpc_ActionCursorNoCursor->setCheckable(true);
   this->mpc_ActionCursorMeasurementCursor =
      this->mpc_MenuCursorMode->addAction(C_GtGetText::h_GetText("Measurement Cursor"),
                                          this, &C_SyvDaChaPlotHandlerWidget::m_CursorModeMeasurementCursor);
   this->mpc_ActionCursorMeasurementCursor->setCheckable(true);
   this->mpc_ActionCursorTwoDiffCursor =
      this->mpc_MenuCursorMode->addAction(C_GtGetText::h_GetText("Difference Cursor"),
                                          this, &C_SyvDaChaPlotHandlerWidget::m_CursorModeTwoDiffCursor);
   this->mpc_ActionCursorTwoDiffCursor->setCheckable(true);

   // No cursor is default and start setting for cursor mode. It is not a saved configuration.
   this->mpc_ActionCursorNoCursor->setChecked(true);

   this->mpc_Ui->pc_PushButtonCursor->setMenu(this->mpc_MenuCursorMode);

   // Options menu
   this->mpc_ActionToggleSamples =
      this->mpc_MenuOptions->addAction(C_GtGetText::h_GetText("Toggle Samples"),
                                       this, &C_SyvDaChaPlotHandlerWidget::m_ShowSamplePoints);
   this->mpc_ActionToggleSamples->setCheckable(true);
   this->mpc_ActionVerticalAxisSettingAllVisible =
      this->mpc_MenuOptions->addAction(C_GtGetText::h_GetText("Show All Y-Axes"),
                                       this, &C_SyvDaChaPlotHandlerWidget::m_VerticalAxisSettingAllVisible);
   this->mpc_ActionVerticalAxisSettingAllVisible->setCheckable(true);

   this->mpc_Ui->pc_PushButtonOptions->setMenu(this->mpc_MenuOptions);

   //Configure timer
   this->mc_CyclicChartUpdateTrigger.setInterval(C_SyvDaChaPlotHandlerWidget::mhs32_INTERVAL_MS);

   this->InitStaticNames();

   this->mpc_Ui->pc_LabelTitle->SetFontPixel(13, true); // color is dark/bright mode specific
   this->mpc_Ui->pc_LabelTitleMeas->SetFontPixel(13, true);
   this->mpc_Ui->pc_LabelTitleMeas->SetForegroundColor(0);
   this->mpc_Ui->pc_Splitter->SetMargins(0, 0);
   // if there is more space stretch right widget (i.e. index 1)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 1);

   // Handle the service mode
   this->mpc_Ui->pc_ButtonAddData->setEnabled(!q_ServiceModeActive);

   connect(this->mpc_Ui->pc_Splitter, &C_OgeSpiStandard::splitterMoved,
           this, &C_SyvDaChaPlotHandlerWidget::m_OnSplitterMoved);
   connect(this->mpc_Ui->pc_ButtonAddData, &QPushButton::clicked,
           this, &C_SyvDaChaPlotHandlerWidget::SigAddSignalClicked);
   connect(this->mpc_Ui->pc_PushButtonPause, &C_OgePubIconChart::toggled, this,
           &C_SyvDaChaPlotHandlerWidget::m_PauseClicked);
   connect(this->mpc_Ui->pc_PushButtonZoomVsDragMode, &C_OgePubIconChart::toggled, this,
           &C_SyvDaChaPlotHandlerWidget::m_SetZoomVsDragMode);
   connect(this->mpc_Ui->pc_PushButtonZoomIn, &C_OgePubIconChart::clicked, this,
           &C_SyvDaChaPlotHandlerWidget::m_ZoomIn);
   connect(this->mpc_Ui->pc_PushButtonZoomOut, &C_OgePubIconChart::clicked, this,
           &C_SyvDaChaPlotHandlerWidget::m_ZoomOut);
   connect(this->mpc_Ui->pc_PushButtonManualRead, &C_OgePubIconChart::clicked, this,
           &C_SyvDaChaPlotHandlerWidget::m_ManualReadTriggered);
   connect(this->mpc_Ui->pc_PushButtonManualAbort, &C_OgePubIconChart::clicked, this,
           &C_SyvDaChaPlotHandlerWidget::SigManualReadAbortTriggered);

   connect(&this->mc_CyclicChartUpdateTrigger, &QTimer::timeout, this,
           &C_SyvDaChaPlotHandlerWidget::m_CyclicUpdateHorizontalAxis);

   connect(this->mpc_Ui->pc_Plot, &C_SyvDaChaPlot::SigCursorItemMovedOnHorizontalAxis,
           this, &C_SyvDaChaPlotHandlerWidget::m_CursorItemMovedOnHorizontalAxis);

   connect(this->mpc_Ui->pc_PushButtonFitXY, &QPushButton::clicked,
           this, &C_SyvDaChaPlotHandlerWidget::m_FitHorizontalVerticalSignals);
   connect(this->mpc_Ui->pc_PushButtonFitY, &QPushButton::clicked,
           this, &C_SyvDaChaPlotHandlerWidget::m_FitVerticalSignals);
   connect(this->mpc_Ui->pc_PushButtonFitX, &QPushButton::clicked,
           this, &C_SyvDaChaPlotHandlerWidget::m_FitHorizontalSignals);

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_Plot->xAxis,
           static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this,
           &C_SyvDaChaPlotHandlerWidget::m_RangeChangedHorizontalAxis);

   connect(this->mpc_Ui->pc_Plot, &C_SyvDaChaPlot::SigCursorItemClicked,
           this, &C_SyvDaChaPlotHandlerWidget::m_CursorItemClicked);
   connect(this->mpc_Ui->pc_Plot, &C_SyvDaChaPlot::SigCursorItemReleased,
           this, &C_SyvDaChaPlotHandlerWidget::m_CursorItemReleased);
   connect(this->mpc_Ui->pc_PushButtonExtractData, &QPushButton::clicked,
           this, &C_SyvDaChaPlotHandlerWidget::m_SetSaveLocationToCsv);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1579}  no memory leak because of the parent of mpc_FirstCursor and mpc_SecondCursor and the Qt memory
C_SyvDaChaPlotHandlerWidget::~C_SyvDaChaPlotHandlerWidget(void)
{
   // Deactivate and clean up measurement
   this->m_ConfigureCursorMode(eSETTING_CM_NO_CURSOR);

   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelTitle->setText(C_GtGetText::h_GetText("Data Elements (0)"));
   this->mpc_Ui->pc_ButtonAddData->setText(C_GtGetText::h_GetText("Add Data Element"));
   this->mpc_Ui->pc_LabelTitleMeas->setText(C_GtGetText::h_GetText("Cursor Information"));

   // remove ui file strings
   this->mpc_Ui->pc_LabelMeasCursor1->setText("");
   this->mpc_Ui->pc_LabelMeasCursor2->setText("");
   this->mpc_Ui->pc_LabelMeasDiff->setText("");
   this->mpc_Ui->pc_PushButtonManualAbort->setText("");
   this->mpc_Ui->pc_PushButtonManualRead->setText("");
   this->mpc_Ui->pc_PushButtonPause->setText("");
   this->mpc_Ui->pc_PushButtonZoomMode->setText("");
   this->mpc_Ui->pc_PushButtonZoomOut->setText("");
   this->mpc_Ui->pc_PushButtonZoomIn->setText("");
   this->mpc_Ui->pc_PushButtonZoomVsDragMode->setText("");
   this->mpc_Ui->pc_PushButtonFitXY->setText("");
   this->mpc_Ui->pc_PushButtonFitY->setText("");
   this->mpc_Ui->pc_PushButtonFitX->setText("");
   this->mpc_Ui->pc_PushButtonCursor->setText("");
   this->mpc_Ui->pc_PushButtonOptions->setText("");
   this->mpc_Ui->pc_PushButtonExtractData->setText("");

   // set tool tip information
   this->mpc_Ui->pc_PushButtonPause->SetToolTipInformation(C_GtGetText::h_GetText("Play/Pause"),
                                                           C_GtGetText::h_GetText("Pause/Resume Update"));
   this->mpc_Ui->pc_PushButtonZoomMode->SetToolTipInformation(C_GtGetText::h_GetText("Zoom Mode"),
                                                              C_GtGetText::h_GetText("Select Zoom Mode"));
   this->mpc_Ui->pc_PushButtonZoomOut->SetToolTipInformation(C_GtGetText::h_GetText("Zoom Mode"),
                                                             C_GtGetText::h_GetText("Zoom Out"));
   this->mpc_Ui->pc_PushButtonZoomIn->SetToolTipInformation(C_GtGetText::h_GetText("Zoom Mode"),
                                                            C_GtGetText::h_GetText("Zoom In"));
   this->mpc_Ui->pc_PushButtonZoomVsDragMode->SetToolTipInformation(C_GtGetText::h_GetText("Zoom Mode"),
                                                                    C_GtGetText::h_GetText(
                                                                       "Activate/Deactivate Drag Zoom Mode"));
   this->mpc_Ui->pc_PushButtonFitXY->SetToolTipInformation(C_GtGetText::h_GetText("All"),
                                                           C_GtGetText::h_GetText("Fit All Data Series"));
   this->mpc_Ui->pc_PushButtonFitY->SetToolTipInformation(C_GtGetText::h_GetText("Vertically"),
                                                          C_GtGetText::h_GetText("Fit All Data Series Vertically"));
   this->mpc_Ui->pc_PushButtonFitX->SetToolTipInformation(C_GtGetText::h_GetText("Horizontally"),
                                                          C_GtGetText::h_GetText("Fit All Data Series Horizontally"));
   this->mpc_Ui->pc_PushButtonCursor->SetToolTipInformation(C_GtGetText::h_GetText("Select"),
                                                            C_GtGetText::h_GetText("Select Cursor"));
   this->mpc_Ui->pc_PushButtonOptions->SetToolTipInformation(C_GtGetText::h_GetText("Select"),
                                                             C_GtGetText::h_GetText("Select Other Options"));
   this->mpc_Ui->pc_PushButtonExtractData->SetToolTipInformation(C_GtGetText::h_GetText("Extract Data"),
                                                                 C_GtGetText::h_GetText("Extract chart data to .csv"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialization of the chart

   \param[in]  ou32_MaximumDataElements   Maximum number of elements and graphs at the same time
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::Init(const uint32_t ou32_MaximumDataElements)
{
   QPen c_Pen;

   this->mu32_MaximumDataElements = ou32_MaximumDataElements;

   this->mpc_Ui->pc_Plot->xAxis->setLabel("[ms]");
   this->mpc_Ui->pc_Plot->setInteraction(QCP::iRangeDrag, true);
   this->mpc_Ui->pc_Plot->setInteraction(QCP::iRangeZoom, true);

   c_Pen = this->mpc_Ui->pc_Plot->xAxis->grid()->pen();
   c_Pen.setStyle(Qt::DashLine);
   this->mpc_Ui->pc_Plot->xAxis->grid()->setPen(c_Pen);

   connect(this->mpc_Ui->pc_ChartSelectorWidget, &C_SyvDaChaDataSelectorWidget::SigDataItemToggled,
           this, &C_SyvDaChaPlotHandlerWidget::m_DataItemToggled);
   connect(this->mpc_Ui->pc_ChartSelectorWidget, &C_SyvDaChaDataSelectorWidget::SigDataItemSelected,
           this, &C_SyvDaChaPlotHandlerWidget::m_DataItemSelected);

   this->mq_Initialized = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the chart data as initialize configuration

   \param[in]  orc_Data          Chart data with configured data elements
   \param[in]  ou32_ViewIndex    Index of view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::SetData(const C_PuiSvDbTabChart & orc_Data, const uint32_t ou32_ViewIndex)
{
   this->mc_Data = orc_Data;
   tgl_assert(this->mc_Data.c_DataPoolElementsActive.size() == this->mc_Data.c_DataPoolElementsConfig.size());
   tgl_assert(this->mc_Data.c_DataPoolElementsColorIndex.size() == this->mc_Data.c_DataPoolElementsConfig.size());

   // Adapt UI
   this->mpc_Ui->pc_PushButtonPause->setChecked(this->mc_Data.q_IsPaused);
   this->mpc_Ui->pc_PushButtonZoomVsDragMode->setChecked(this->mc_Data.q_IsZoomModeActive);
   this->mpc_ActionToggleSamples->setChecked(this->mc_Data.q_AreSamplePointsShown);
   this->mpc_Ui->pc_Splitter->SetFirstSegment(this->mc_Data.s32_SplitterLeftWidth);
   this->m_ResizeSelectorWidget(); // first add space before adding selector items

   switch (this->mc_Data.e_SettingZoomMode)
   {
   case C_PuiSvDbTabChart::eSETTING_ZM_XY:
      this->mpc_ActionZoomSettingHorizontalVertical->setChecked(true);
      break;
   case C_PuiSvDbTabChart::eSETTING_ZM_X:
      this->mpc_ActionZoomSettingHorizontal->setChecked(true);
      break;
   case C_PuiSvDbTabChart::eSETTING_ZM_Y:
      this->mpc_ActionZoomSettingVertical->setChecked(true);
      break;
   default:
      this->mpc_ActionZoomSettingHorizontalVertical->setChecked(true);
      break;
   }
   this->m_OnChangeZoomMode(this->mc_Data.e_SettingZoomMode);

   switch (this->mc_Data.e_SettingVerticalAxisMode)
   {
   case C_PuiSvDbTabChart::eSETTING_YA_ONE_VISIBLE:
      this->mpc_ActionVerticalAxisSettingAllVisible->setChecked(false);
      break;
   case C_PuiSvDbTabChart::eSETTING_YA_ALL_VISIBLE:
      this->mpc_ActionVerticalAxisSettingAllVisible->setChecked(true);
      break;
   default:
      this->mpc_ActionVerticalAxisSettingAllVisible->setChecked(false);
      break;
   }

   // Load elements
   for (uint32_t u32_It = 0; u32_It < this->mc_Data.c_DataPoolElementsConfig.size(); ++u32_It)
   {
      const C_PuiSvDbNodeDataElementConfig & rc_Config = this->mc_Data.c_DataPoolElementsConfig[u32_It];
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                    rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                    rc_Config.c_ElementId.u32_ListIndex,
                                                                    rc_Config.c_ElementId.u32_ElementIndex);
      bool q_Warning = false;
      bool q_Invalid = false;
      QString c_InvalidNamePlaceholder = "";
      C_OscNodeDataPool::E_Type e_InvalidDataPoolTypePlaceholder = C_OscNodeDataPool::eDIAG;
      QString c_ToolTipErrorTextHeading = "";
      QString c_ToolTipErrorText = "";

      if (pc_Element != NULL)
      {
         // Save the minimum to have a content instance of the element to have the type
         this->mc_DataPoolElementContentMin.push_back(pc_Element->c_MinValue);
      }
      else
      {
         // No datapool element exists, add dummy instance
         const C_OscNodeDataPoolContent c_Tmp;
         this->mc_DataPoolElementContentMin.push_back(c_Tmp);
      }

      if (rc_Config.c_ElementId.GetIsValid() == false)
      {
         // Element is invalid, it was deleted in the system definition
         q_Warning = true;
         q_Invalid = true;
         c_InvalidNamePlaceholder = rc_Config.c_ElementId.GetInvalidNamePlaceholder();
         e_InvalidDataPoolTypePlaceholder = rc_Config.c_ElementId.GetInvalidTypePlaceholder();
         c_ToolTipErrorTextHeading = C_GtGetText::h_GetText("Configuration warning");
         c_ToolTipErrorText = rc_Config.c_ElementId.GetInvalidItemPlaceholderTooltip();
      }
      else
      {
         std::vector<uint8_t> c_NodeActiveFlags;
         const int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
            ou32_ViewIndex,
            c_NodeActiveFlags);

         tgl_assert(s32_Retval == C_NO_ERR);
         if (s32_Retval == C_NO_ERR)
         {
            if (rc_Config.c_ElementId.u32_NodeIndex < c_NodeActiveFlags.size())
            {
               if (c_NodeActiveFlags[rc_Config.c_ElementId.u32_NodeIndex] == 0U)
               {
                  // Node with data element is not active in current view
                  q_Warning = true;
                  c_ToolTipErrorTextHeading = C_GtGetText::h_GetText("Configuration warning");
                  c_ToolTipErrorText = C_GtGetText::h_GetText("There is a data element of an inactive node");
               }
               else
               {
                  // Check for further warning reason
                  tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckViewNodeDashboardRoutingError(
                                ou32_ViewIndex,
                                rc_Config.c_ElementId.u32_NodeIndex,
                                q_Warning) == C_NO_ERR);

                  if (q_Warning == true)
                  {
                     c_ToolTipErrorTextHeading = C_GtGetText::h_GetText("Configuration warning");
                     c_ToolTipErrorText = C_GtGetText::h_GetText(
                        "There is a data element of a node with "
                        "disabled communication interface flags for Dashboard");
                  }
               }
            }
         }
      }

      // Mark the color as used
      // Adapt the color index in case of a to high index due to possible compatibility mechanisms
      this->mc_Data.c_DataPoolElementsColorIndex[u32_It] = this->mc_Data.c_DataPoolElementsColorIndex[u32_It] %
                                                           mhu8_COUNT_COLORS;
      this->m_SetConcreteColorAsUsed(this->mc_Data.c_DataPoolElementsColorIndex[u32_It]);

      // Add the concrete graph to the chart
      this->m_AddGraph(u32_It, q_Warning, q_Invalid, c_InvalidNamePlaceholder, e_InvalidDataPoolTypePlaceholder,
                       c_ToolTipErrorTextHeading, c_ToolTipErrorText);
   }

   tgl_assert(this->mc_Data.c_DataPoolElementsActive.size() == this->mc_DataPoolElementContentMin.size());

   this->m_UpdateElementCounter();
   this->m_ConfigureZoomMode(this->mc_Data.e_SettingZoomMode);
   this->m_ConfigureZoomVsDragMode(this->mc_Data.q_IsZoomModeActive);
   this->m_ConfigureVerticalAxisMode(this->mc_Data.e_SettingVerticalAxisMode);
   this->m_LoadState(this->mc_Data.q_IsPaused, this->mc_Data.q_AreSamplePointsShown, this->mc_Data.c_VisibleScreen);

   this->m_UpdateDragAxesConfiguration();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the chart data with the element configuration

   \return
   Chart data
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbTabChart & C_SyvDaChaPlotHandlerWidget::GetData(void)
{
   this->m_SaveState(this->mc_Data.q_IsPaused, this->mc_Data.s32_SplitterLeftWidth,
                     this->mc_Data.q_AreSamplePointsShown, this->mc_Data.c_VisibleScreen);

   //lint -e{1536} it is intended to expose a private member to avoid copies of rather big data structures
   return this->mc_Data;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the initialized display formatter

   \param[in]  orc_DataPoolElementId   Datapool element identificator
   \param[in]  orc_Config              Display formatter configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::SetDisplayFormatterConfig(
   const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
   const C_PuiSvDbDataElementDisplayFormatterConfig & orc_Config)
{
   uint32_t u32_ConfigCounter;

   for (u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_Data.c_DataPoolElementsConfig.size();
        ++u32_ConfigCounter)
   {
      const C_PuiSvDbNodeDataPoolListElementId & rc_CurDataPoolElementId =
         this->mc_Data.c_DataPoolElementsConfig[u32_ConfigCounter].c_ElementId;
      if (rc_CurDataPoolElementId == orc_DataPoolElementId)
      {
         if (u32_ConfigCounter >= this->mc_DataPoolElementDisplayFormatter.size())
         {
            this->mc_DataPoolElementDisplayFormatter.resize(static_cast<uint32_t>(u32_ConfigCounter + 1U));
         }
         this->mc_DataPoolElementDisplayFormatter[u32_ConfigCounter] = orc_Config;

         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply dark mode

   \param[in]  oq_DarkMode    Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::SetDarkMode(const bool oq_DarkMode)
{
   QBrush c_BackgroundBrush;

   c_BackgroundBrush.setStyle(Qt::SolidPattern);

   this->mq_DarkMode = oq_DarkMode;
   if (oq_DarkMode == true)
   {
      this->mpc_Ui->pc_ControlWidget->SetBackgroundColor(32);

      // Color for background
      c_BackgroundBrush.setColor(mc_STYLE_GUIDE_COLOR_33);
      this->mpc_Ui->pc_Plot->setBackground(c_BackgroundBrush);

      // Color for the splitter
      this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_8);

      // frame boxes
      this->mpc_Ui->pc_FrameLeft->SetBorderColor(1000); // rgb(0,0,0) is unfortunately not specified
      this->mpc_Ui->pc_FrameLeft->SetBackgroundColor(33);
      this->mpc_Ui->pc_FrameRight->SetBorderColor(1000);
      this->mpc_Ui->pc_FrameRight->SetBackgroundColor(33);

      // Customize the x axis and the x axis gridline
      mh_AdaptAxisColor(this->mpc_Ui->pc_Plot->xAxis, mc_STYLE_GUIDE_COLOR_10, oq_DarkMode);
      // The label is the only color of the axis in dark mode one with different color
      this->mpc_Ui->pc_Plot->xAxis->setLabelColor(mc_STYLE_GUIDE_COLOR_CH4);
      // Adapt all y axes grids
      this->m_AdaptAllVerticalAxisGridColors(oq_DarkMode);

      // Colors for the data element view
      this->mpc_Ui->pc_WidgetTitle->SetBackgroundColor(32);
      this->mpc_Ui->pc_LabelTitle->SetForegroundColor(0);
      this->mpc_Ui->pc_WiInfoTitle->SetBackgroundColor(32);
      this->mpc_Ui->pc_WiInfoContent->SetBackgroundColor(33);
   }
   else
   {
      this->mpc_Ui->pc_ControlWidget->SetBackgroundColor(11);

      // Color for background
      c_BackgroundBrush.setColor(mc_STYLE_GUIDE_COLOR_0);
      this->mpc_Ui->pc_Plot->setBackground(c_BackgroundBrush);

      // Color for the splitter
      this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_10);

      // frame boxes
      this->mpc_Ui->pc_FrameLeft->SetBorderColor(10);
      this->mpc_Ui->pc_FrameLeft->SetBackgroundColor(12);
      this->mpc_Ui->pc_FrameRight->SetBorderColor(10);
      this->mpc_Ui->pc_FrameRight->SetBackgroundColor(12);

      // Customize the x axis and the x axis gridline
      mh_AdaptAxisColor(this->mpc_Ui->pc_Plot->xAxis, mc_STYLE_GUIDE_COLOR_CH3, oq_DarkMode);
      // The tick label is the only color of the axis in bright mode one with different color
      this->mpc_Ui->pc_Plot->xAxis->setTickLabelColor(mc_STYLE_GUIDE_COLOR_9);
      // Adapt all y axes grids
      this->m_AdaptAllVerticalAxisGridColors(oq_DarkMode);

      // Colors for the data element view
      this->mpc_Ui->pc_WidgetTitle->SetBackgroundColor(11);
      this->mpc_Ui->pc_LabelTitle->SetForegroundColor(3);
      this->mpc_Ui->pc_WiInfoTitle->SetBackgroundColor(27);
      this->mpc_Ui->pc_WiInfoContent->SetBackgroundColor(11);
   }

   // Color for frame separators of info box
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameSeperatorBottom, "HasColor1000Background", oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameSeperatorBottom, "HasColor7Background", !oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameSeperatorTop, "HasColor1000Background", oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameSeperatorTop, "HasColor27Background", !oq_DarkMode);

   // Color for frame separators of button panel
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator1, "HasColor10Background", !oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator2, "HasColor10Background", !oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator3, "HasColor10Background", !oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator4, "HasColor10Background", !oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator1, "HasColor39Background", oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator2, "HasColor39Background", oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator3, "HasColor39Background", oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator4, "HasColor39Background", oq_DarkMode);

   // Color for menus
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_MenuZoomMode, "DarkMode", oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_MenuCursorMode, "DarkMode", oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_MenuOptions, "DarkMode", oq_DarkMode);

   // Color for buttons
   this->m_SetButtonIcons(oq_DarkMode);
   this->m_OnChangeZoomMode(this->mc_Data.e_SettingZoomMode);

   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to activate or deactivate drawing of performance heavy widgets

   \param[in]  oq_Active   Flag if widgets should currently be drawn
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::SetDrawingActive(const bool oq_Active)
{
   if ((this->mq_DrawingActive == false) &&
       (oq_Active == true))
   {
      // Redraw if not already active
      this->mpc_Ui->pc_Plot->replot();
   }

   this->mq_DrawingActive = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the pause state of the chart

   \retval   true   Chart is paused
   \retval   false  Chart is running
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaPlotHandlerWidget::IsPaused(void) const
{
   return this->mpc_Ui->pc_PushButtonPause->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::ConnectionActiveChanged(const bool oq_Active)
{
   this->mq_IsConnected = oq_Active;
   if (oq_Active == true)
   {
      this->m_ResetChart();
      this->mu32_TimeStampOfStart = stw::tgl::TglGetTickCount();
      //Reset window
      this->mpc_Ui->pc_Plot->xAxis->setRange(0.0, this->mpc_Ui->pc_Plot->xAxis->range().size(), Qt::AlignLeft);
      if (this->mq_PauseState == false)
      {
         this->mc_CyclicChartUpdateTrigger.start();
      }
      this->mpc_Ui->pc_ButtonAddData->setEnabled(false);
      this->mpc_Ui->pc_PushButtonManualRead->setEnabled(true);
   }
   else
   {
      this->mc_CyclicChartUpdateTrigger.stop();
      // In case of disconnect the transmission errors must be reseted, but not the warnings
      this->mpc_Ui->pc_ChartSelectorWidget->ResetError();
      this->mpc_Ui->pc_ButtonAddData->setEnabled(true);
      this->mpc_Ui->pc_PushButtonManualRead->setEnabled(false);
   }

   this->m_AdaptCursorMode();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a specific graph

   \param[in]  orc_DataPoolElementId   Datapool element identification
   \param[in]  orc_ElementScaling      Datapool element scaling configuration

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaChaPlotHandlerWidget::AddNewGraph(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                                 const C_PuiSvDbDataElementScaling & orc_ElementScaling)
{
   int32_t s32_Return = C_RANGE;

   tgl_assert(this->mq_Initialized == true);
   tgl_assert(this->mc_Data.c_DataPoolElementsConfig.size() == this->mc_Data.c_DataPoolElementsActive.size());
   tgl_assert(this->mc_Data.c_DataPoolElementsConfig.size() == this->mc_Data.c_DataPoolElementsColorIndex.size());

   if (this->mc_Data.c_DataPoolElementsConfig.size() < this->mu32_MaximumDataElements)
   {
      C_PuiSvDbNodeDataElementConfig c_Config;
      const uint32_t u32_NewIndex = this->mc_Data.c_DataPoolElementsConfig.size();

      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                    orc_DataPoolElementId.u32_DataPoolIndex,
                                                                    orc_DataPoolElementId.u32_ListIndex,
                                                                    orc_DataPoolElementId.u32_ElementIndex);
      if (pc_Element != NULL)
      {
         c_Config.c_ElementId = orc_DataPoolElementId;
         c_Config.c_ElementScaling = orc_ElementScaling;

         this->mc_Data.c_DataPoolElementsConfig.push_back(c_Config);
         this->mc_Data.c_DataPoolElementsActive.push_back(true);
         this->mc_Data.c_DataPoolElementsColorIndex.push_back(this->m_GetNextNotUsedColor());

         // Save the minimum to have a content instance of the element to have the type
         this->mc_DataPoolElementContentMin.push_back(pc_Element->c_MinValue);

         this->m_ResizeSelectorWidget(); // first add space before adding new selector item
         this->m_AddGraph(u32_NewIndex);
         this->m_UpdateElementCounter();
         this->m_UpdateDragAxesConfiguration();

         s32_Return = C_NO_ERR;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current graph

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification
   \param[out]  orc_ElementId    Data element id of removed data serie

   \return
   true     data element removed
   false    nothing removed or a multiple registration of an element id was removed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaPlotHandlerWidget::RemoveSpecificGraph(const uint32_t ou32_DataPoolElementConfigIndex,
                                                      C_PuiSvDbNodeDataPoolListElementId & orc_ElementId)
{
   bool q_Return = false;

   if (static_cast<int32_t>(ou32_DataPoolElementConfigIndex) < this->mpc_Ui->pc_Plot->graphCount())
   {
      q_Return = this->mpc_Ui->pc_ChartSelectorWidget->RemoveDataSerie(ou32_DataPoolElementConfigIndex);
   }

   if (q_Return == true)
   {
      // Remove data serie
      if (static_cast<int32_t>(ou32_DataPoolElementConfigIndex) < this->mpc_Ui->pc_Plot->graphCount())
      {
         if (static_cast<int32_t>(ou32_DataPoolElementConfigIndex) <
             this->mc_DataElementConfigIndexToVerticalAxis.size())
         {
            QCPAxis * const pc_Axis =
               this->mc_DataElementConfigIndexToVerticalAxis.at(static_cast<int32_t>(ou32_DataPoolElementConfigIndex));

            // Do not remove the last axis
            if (this->mc_DataElementConfigIndexToVerticalAxis.size() > 1)
            {
               this->mpc_Ui->pc_Plot->axisRect()->removeAxis(pc_Axis);
            }
            else
            {
               pc_Axis->setLabel("");
            }

            this->mc_DataElementConfigIndexToVerticalAxis.removeAt(static_cast<int32_t>(ou32_DataPoolElementConfigIndex));

            // Remove the tracer when measurement is active
            if ((this->me_SettingCursorMode != eSETTING_CM_NO_CURSOR) &&
                (static_cast<int32_t>(ou32_DataPoolElementConfigIndex) < this->mc_ItemTracers.size()))
            {
               QCPItemTracer * const pc_Tracer =
                  this->mc_ItemTracers.at(static_cast<int32_t>(ou32_DataPoolElementConfigIndex));
               if (pc_Tracer != NULL)
               {
                  this->mpc_Ui->pc_Plot->removeItem(pc_Tracer);
               }

               this->mc_ItemTracers.removeAt(static_cast<int32_t>(ou32_DataPoolElementConfigIndex));
            }
         }

         this->mpc_Ui->pc_Plot->removeGraph(ou32_DataPoolElementConfigIndex);
      }

      // Remove used color
      this->m_SetColorUnused(this->mc_Data.c_DataPoolElementsColorIndex[ou32_DataPoolElementConfigIndex]);

      if (this->mpc_Ui->pc_Plot->graphCount() > 0)
      {
         // Select an other item
         this->SelectGraph(0U);
         this->mpc_Ui->pc_ChartSelectorWidget->SelectDataSerie(0);
      }

      if (ou32_DataPoolElementConfigIndex < this->mc_Data.c_DataPoolElementsConfig.size())
      {
         uint32_t u32_Counter;

         orc_ElementId = this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementId;

         // Check if there is an further registration of this data element
         for (u32_Counter = 0U; u32_Counter < this->mc_Data.c_DataPoolElementsConfig.size(); ++u32_Counter)
         {
            if ((u32_Counter != ou32_DataPoolElementConfigIndex) &&
                (orc_ElementId == this->mc_Data.c_DataPoolElementsConfig[u32_Counter].c_ElementId))
            {
               // An other registration found. The upper layer shall think nothing happened
               q_Return = false;
            }
         }

         // Remove configuration itself
         tgl_assert(this->mc_Data.RemoveElement(ou32_DataPoolElementConfigIndex) == C_NO_ERR);
         this->mc_DataPoolElementContentMin.erase(
            this->mc_DataPoolElementContentMin.begin() + ou32_DataPoolElementConfigIndex);
         this->mc_DataPoolElementDisplayFormatter.erase(
            this->mc_DataPoolElementDisplayFormatter.begin() + ou32_DataPoolElementConfigIndex);
      }

      // resize selector widget
      this->m_ResizeSelectorWidget();

      // update counter
      this->m_UpdateElementCounter();

      // The drag axes configuration must be updated due to a removed axis
      this->m_UpdateDragAxesConfiguration();
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current graph

   \param[out]  orc_ElementId    Data element id of removed data serie

   \return
   true     data element removed
   false    nothing removed or a multiple registration of an element id was removed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaPlotHandlerWidget::RemoveCurrentGraph(C_PuiSvDbNodeDataPoolListElementId & orc_ElementId)
{
   bool q_Return;
   uint32_t u32_DataPoolElementConfigIndex;

   q_Return = this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(u32_DataPoolElementConfigIndex);

   if (q_Return == true)
   {
      q_Return = this->RemoveSpecificGraph(u32_DataPoolElementConfigIndex, orc_ElementId);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Selects and shows the axix for the specific datapool element

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::SelectGraph(const uint32_t ou32_DataPoolElementConfigIndex)
{
   if ((static_cast<int32_t>(ou32_DataPoolElementConfigIndex) < this->mpc_Ui->pc_Plot->graphCount()) &&
       (static_cast<int32_t>(ou32_DataPoolElementConfigIndex) < this->mc_DataElementConfigIndexToVerticalAxis.size()))
   {
      QCPAxisRect * const pc_AxisRect = this->mpc_Ui->pc_Plot->axisRect();
      QCPAxis * const pc_ActiveVerticalAxis =
         this->mc_DataElementConfigIndexToVerticalAxis[ou32_DataPoolElementConfigIndex];
      int32_t s32_CounterVerticalAxis;

      // Adapt the axis configuration
      for (s32_CounterVerticalAxis = 0; s32_CounterVerticalAxis < this->mc_DataElementConfigIndexToVerticalAxis.size();
           ++s32_CounterVerticalAxis)
      {
         QCPAxis * const pc_VerticalAxis = this->mc_DataElementConfigIndexToVerticalAxis.at(s32_CounterVerticalAxis);
         const bool q_Selected = (s32_CounterVerticalAxis == static_cast<int32_t>(ou32_DataPoolElementConfigIndex));

         // Adapt the visualization depending of the y axis mode
         if (this->mc_Data.e_SettingVerticalAxisMode == C_PuiSvDbTabChart::eSETTING_YA_ONE_VISIBLE)
         {
            pc_VerticalAxis->setVisible(q_Selected);
         }
         else
         {
            mh_AdaptVerticalAxisStyle(pc_VerticalAxis, q_Selected);
         }
      }

      // Configure zoom and scaling behavior
      pc_AxisRect->setRangeZoomAxes(this->mpc_Ui->pc_Plot->xAxis, pc_ActiveVerticalAxis);

      // Adapt the visualization of the trace when measurement is active
      if (this->me_SettingCursorMode != eSETTING_CM_NO_CURSOR)
      {
         int32_t s32_CounterTracers;

         tgl_assert(static_cast<int32_t>(ou32_DataPoolElementConfigIndex) < this->mc_ItemTracers.size());

         for (s32_CounterTracers = 0; s32_CounterTracers < this->mc_ItemTracers.size(); ++s32_CounterTracers)
         {
            QCPItemTracer * const pc_Tracer = this->mc_ItemTracers.at(s32_CounterTracers);

            if (pc_Tracer != NULL)
            {
               if (s32_CounterTracers == static_cast<int32_t>(ou32_DataPoolElementConfigIndex))
               {
                  // Current selected axis
                  pc_Tracer->setStyle(QCPItemTracer::tsSquare);
               }
               else
               {
                  pc_Tracer->setStyle(QCPItemTracer::tsNone);
               }
            }
         }

         this->m_UpdateMeasurementCursors();
      }
   }
   else
   {
      tgl_assert(false);
   }

   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current id of the selected data element

   \param[out]  oru32_DataPoolElementConfigIndex   Datapool element configuration index

   \return
   true     data element exists
   false    data element does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaPlotHandlerWidget::GetCurrentGraph(uint32_t & oru32_DataPoolElementConfigIndex) const
{
   return this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(oru32_DataPoolElementConfigIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns if a selector item widget of any data series is on a specific position

   \param[in]   orc_Pos                            Position to check for

   \return
   true     A data element is on the position
   false    No data element is on the position
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaPlotHandlerWidget::IsAnyDataSerieOnPosition(const QPoint & orc_Pos) const
{
   return this->mpc_Ui->pc_ChartSelectorWidget->IsAnyDataSerieOnPosition(this->mapFromParent(orc_Pos));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds the newest values of Datapool elements to its associated graph of the chart

   \param[in]  orc_DataPoolElementId   Datapool element identification
   \param[in]  orc_FormattedLastValue  Last element as formatted and scaled string
   \param[in]  orc_Values              List with all read scaled elements
   \param[in]  orc_Timestamps          List with all timestamps for each element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::AddGraphContent(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                                  const QString & orc_FormattedLastValue,
                                                  const QVector<float64_t> & orc_Values,
                                                  const QVector<uint32_t> & orc_Timestamps)
{
   // Find the correct data series
   if (orc_Values.size() > 0)
   {
      uint32_t u32_ConfigCounter;

      for (u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_Data.c_DataPoolElementsConfig.size();
           ++u32_ConfigCounter)
      {
         const C_PuiSvDbNodeDataPoolListElementId & rc_CurDataPoolElementId =
            this->mc_Data.c_DataPoolElementsConfig[u32_ConfigCounter].c_ElementId;
         if ((rc_CurDataPoolElementId == orc_DataPoolElementId) &&
             (static_cast<int32_t>(u32_ConfigCounter) < this->mpc_Ui->pc_Plot->graphCount()))
         {
            QCPGraph * const pc_Graph = this->mpc_Ui->pc_Plot->graph(u32_ConfigCounter);

            if (pc_Graph != NULL)
            {
               int32_t s32_ValueCounter;
               float64_t f64_Value;

               tgl_assert(orc_Values.size() == orc_Timestamps.size());

               for (s32_ValueCounter = 0U; s32_ValueCounter < orc_Values.size(); ++s32_ValueCounter)
               {
                  // Start time must be zero, adapt the read timestamp
                  const uint32_t u32_TimeStamp = orc_Timestamps[s32_ValueCounter];
                  float64_t f64_Timestamp;

                  if (u32_TimeStamp > this->mu32_TimeStampOfStart)
                  {
                     f64_Timestamp = static_cast<float64_t>(u32_TimeStamp);
                     f64_Timestamp -= static_cast<float64_t>(this->mu32_TimeStampOfStart);
                  }
                  else
                  {
                     f64_Timestamp = 0.0;
                  }

                  f64_Value = orc_Values[s32_ValueCounter];

                  // Adapt the automatic scaling only for visible elements
                  if (pc_Graph->visible() == true)
                  {
                     // Add a little free space to the values
                     const float64_t f64_FAC_HIGH = 1.1;
                     const float64_t f64_FAC_LOW = 0.9;
                     const float64_t f64_ValueFactorHigh = f64_Value * f64_FAC_HIGH;
                     const float64_t f64_ValueFactorLow = f64_Value * f64_FAC_LOW;

                     // Adapt range for value
                     if (f64_Value >= 0.0)
                     {
                        // Maximum
                        if (this->mf64_MaxValue < f64_ValueFactorHigh)
                        {
                           this->mf64_MaxValue = f64_ValueFactorHigh;
                        }

                        // Minimum
                        if (this->mf64_MinValue > f64_ValueFactorLow)
                        {
                           this->mf64_MinValue = f64_ValueFactorLow;
                        }
                     }
                     else
                     {
                        // Maximum
                        if (this->mf64_MaxValue < f64_ValueFactorLow)
                        {
                           this->mf64_MaxValue = f64_ValueFactorLow;
                        }

                        // Minimum
                        if (this->mf64_MinValue > f64_ValueFactorHigh)
                        {
                           this->mf64_MinValue = f64_ValueFactorHigh;
                        }
                     }
                  }

                  pc_Graph->addData(f64_Timestamp, f64_Value);
               }
               // Show the last formatted value in the selector widget
               this->mpc_Ui->pc_ChartSelectorWidget->UpdateDataSerieValue(u32_ConfigCounter, orc_FormattedLastValue);
            }
         }
      }

      this->m_RedrawGraph();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a scaling configuration for a specific datapool element configuration

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element configuration index
   \param[in]  orc_DisplayName                  Datapool element display name
   \param[in]  orc_ElementScaling               Datapool element scaling configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::SetScaling(const uint32_t ou32_DataPoolElementConfigIndex,
                                             const QString & orc_DisplayName,
                                             const C_PuiSvDbDataElementScaling & orc_ElementScaling)
{
   if (ou32_DataPoolElementConfigIndex < this->mc_Data.c_DataPoolElementsConfig.size())
   {
      this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_DisplayName = orc_DisplayName;
      this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementScaling = orc_ElementScaling;
      this->mpc_Ui->pc_ChartSelectorWidget->SetDataElementUnit(ou32_DataPoolElementConfigIndex, orc_DisplayName,
                                                               orc_ElementScaling.c_Unit);
   }
   this->m_UpdateVerticalAxisLabel(ou32_DataPoolElementConfigIndex);
   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the count of registered graphs

   \return
   Count of registered graphs
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvDaChaPlotHandlerWidget::GetCountGraphs(void) const
{
   return this->mpc_Ui->pc_Plot->graphCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Error update for data element

   \param[in]  ou32_DataElementIndex   Index of shown Datapool element in widget
   \param[in]  orc_ErrorText           Error description
   \param[in]  oq_IsTransmissionError  Flag if transmission error occurred
   \param[in]  oq_ErrorActive          Flag if error is active or should be cleared
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::UpdateError(const uint32_t ou32_DataElementIndex, const QString & orc_ErrorText,
                                              const bool oq_IsTransmissionError, const bool oq_ErrorActive) const
{
   const std::map<uint32_t, uint32_t>::const_iterator c_It =
      this->mc_ElementHandlerRegIndexToDataElementIndex.find(ou32_DataElementIndex);

   if (c_It != this->mc_ElementHandlerRegIndexToDataElementIndex.end())
   {
      this->mpc_Ui->pc_ChartSelectorWidget->UpdateError(c_It->second, orc_ErrorText, oq_IsTransmissionError,
                                                        oq_ErrorActive);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fit default
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::FitDefault(void)
{
   if (this->mc_Data.c_DataPoolElementsConfig.size() > 0UL)
   {
      uint32_t u32_ConfigCounter;

      for (u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_Data.c_DataPoolElementsConfig.size();
           ++u32_ConfigCounter)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_CurConfig =
            this->mc_Data.c_DataPoolElementsConfig[u32_ConfigCounter];
         const C_OscNodeDataPoolListElement * const pc_Elem =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(rc_CurConfig.c_ElementId);
         if (pc_Elem != NULL)
         {
            //Scaling
            float64_t f64_Factor;
            float64_t f64_Offset;
            float64_t f64_CurMin;
            float64_t f64_CurMax;
            if (rc_CurConfig.c_ElementScaling.q_UseDefault)
            {
               f64_Factor = pc_Elem->f64_Factor;
               f64_Offset = pc_Elem->f64_Offset;
            }
            else
            {
               f64_Factor = rc_CurConfig.c_ElementScaling.f64_Factor;
               f64_Offset = rc_CurConfig.c_ElementScaling.f64_Offset;
            }
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Elem->c_MinValue, f64_CurMin,
                                                      rc_CurConfig.c_ElementId.GetArrayElementIndexOrZero());
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Elem->c_MaxValue, f64_CurMax,
                                                      rc_CurConfig.c_ElementId.GetArrayElementIndexOrZero());
            f64_CurMin = C_OscUtils::h_GetValueScaled(f64_CurMin, f64_Factor, f64_Offset);
            f64_CurMax = C_OscUtils::h_GetValueScaled(f64_CurMax, f64_Factor, f64_Offset);

            this->mc_DataElementConfigIndexToVerticalAxis[u32_ConfigCounter]->setRange(f64_CurMin, f64_CurMax);
         }
      }
   }
   else
   {
      //Empty
      QCPAxis * const pc_Axis = this->m_GetVerticalAxis(0);
      if (pc_Axis != NULL)
      {
         pc_Axis->setRange(-100.0, 100.0);
      }
   }
   this->mpc_Ui->pc_Plot->xAxis->setRange(0.0, 5000.0, Qt::AlignLeft);
   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reload colors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::RefreshColors(void)
{
   uint32_t u32_Counter;

   // Remove all colors of all items
   for (u32_Counter = 0; u32_Counter < this->mc_Data.c_DataPoolElementsColorIndex.size(); ++u32_Counter)
   {
      this->m_SetColorUnused(this->mc_Data.c_DataPoolElementsColorIndex[u32_Counter]);
   }

   // Assign new selected colors again
   for (u32_Counter = 0; u32_Counter < this->mc_Data.c_DataPoolElementsColorIndex.size(); ++u32_Counter)
   {
      QColor c_Color;

      this->mc_Data.c_DataPoolElementsColorIndex[u32_Counter] = this->m_GetNextNotUsedColor();
      c_Color = mhac_DATA_COLORS[this->mc_Data.c_DataPoolElementsColorIndex[u32_Counter]];

      this->mpc_Ui->pc_ChartSelectorWidget->UpdateDataSerieColor(static_cast<uint32_t>(u32_Counter), c_Color);
      if (u32_Counter < static_cast<uint32_t>(this->mpc_Ui->pc_Plot->graphCount()))
      {
         QCPGraph * const pc_Graph = this->mpc_Ui->pc_Plot->graph(u32_Counter);
         if (pc_Graph != NULL)
         {
            QPen c_Pen = pc_Graph->pen();
            c_Pen.setColor(c_Color);
            pc_Graph->setPen(c_Pen);

            mh_AdaptAxisColor(pc_Graph->valueAxis(), c_Color, this->mq_DarkMode);
         }
      }
   }

   Q_EMIT (this->SigChartDataChanged());
   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flag if a manual operation is possible

   \param[in]  oq_Possible    New flag if manual operation is possible
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::SetManualOperationPossible(const bool oq_Possible)
{
   this->mq_ManualOperationPossible = oq_Possible;
   this->mpc_Ui->pc_PushButtonManualRead->setVisible(oq_Possible);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Final step of manual operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::ManualReadFinished(void)
{
   this->mpc_Ui->pc_PushButtonManualRead->setVisible(this->mq_ManualOperationPossible);
   this->mpc_Ui->pc_PushButtonManualAbort->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set current chart tab name

   \param[in]  orc_CurrentDashboardTabName
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::SetCurrentDashboardTabName(const QString & orc_CurrentDashboardTabName)
{
   this->mc_CurrentDashboardTabName = orc_CurrentDashboardTabName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden resize event

   Here: resize selector widget dynamically and make add button tiny if necessary

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);
   this->m_ResizeSelectorWidget();
   this->m_OnSplitterMoved();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden show event

   Here: resize selector widget dynamically

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::showEvent(QShowEvent * const opc_Event)
{
   QWidget::showEvent(opc_Event);
   this->m_ResizeSelectorWidget();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a specific data series

   \param[in]  ou32_DataPoolElementConfigIndex     Datapool element identification
   \param[in]  oq_Warning                          Flag if a warning for this data element was detected
   \param[in]  oq_Invalid                          Flag if data element is invalid and invalid placeholder info
                                                   shall be used
   \param[in]  orc_InvalidPlaceholderName          Placeholder name of data element in case of invalid data element
   \param[in]  oe_InvalidPlaceholderDataPoolType   Placeholder datapool type of data element in case of
                                                   invalid data element
   \param[in]  orc_ToolTipErrorTextHeading         Heading of tool tip in case of a warning
   \param[in]  orc_ToolTipErrorText                Text of tool tip in case of a warning
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_AddGraph(const uint32_t ou32_DataPoolElementConfigIndex, const bool oq_Warning,
                                             const bool oq_Invalid, const QString & orc_InvalidPlaceholderName,
                                             const C_OscNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType,
                                             const QString & orc_ToolTipErrorTextHeading,
                                             const QString & orc_ToolTipErrorText)
{
   // Color selection
   const QColor c_Color = mhac_DATA_COLORS[this->mc_Data.c_DataPoolElementsColorIndex[ou32_DataPoolElementConfigIndex]];
   const int32_t s32_GraphNumber = this->mpc_Ui->pc_Plot->graphCount();
   QCPAxisRect * const pc_AxisRect = this->mpc_Ui->pc_Plot->axisRect();
   QCPAxis * pc_VerticalAxis = NULL;
   QCPGraph * pc_Graph;
   QPen c_Pen;

   // Get the Y axis for the new graph and if the axis does not exist yet, add it
   tgl_assert(static_cast<int32_t>(ou32_DataPoolElementConfigIndex) == s32_GraphNumber);
   tgl_assert(pc_AxisRect != NULL);
   if (pc_AxisRect != NULL)
   {
      if (pc_AxisRect->axisCount(QCPAxis::atLeft) <= s32_GraphNumber)
      {
         // Add a new one
         pc_VerticalAxis = pc_AxisRect->addAxis(QCPAxis::atLeft);
      }
      else
      {
         // Use the first axis
         pc_VerticalAxis = pc_AxisRect->axis(QCPAxis::atLeft, 0);
      }
   }

   // Add the new graph
   pc_Graph = this->mpc_Ui->pc_Plot->addGraph(NULL, pc_VerticalAxis);

   // Add tracer
   if (this->me_SettingCursorMode != eSETTING_CM_NO_CURSOR)
   {
      // A new added graph can not have any data, so no concrete tracer is necessary
      this->mc_ItemTracers.append(NULL);
   } //lint !e429  //no memory leak because of the parent of pc_Tracer and the Qt memory management

   // Assign the axis to the graph by its index in a separate mapping
   this->mc_DataElementConfigIndexToVerticalAxis.append(pc_VerticalAxis);

   if (oq_Invalid == false)
   {
      this->mc_ElementHandlerRegIndexToDataElementIndex.emplace(
         std::pair<uint32_t, uint32_t>(
            static_cast<uint32_t>(this->mc_ElementHandlerRegIndexToDataElementIndex.size()),
            ou32_DataPoolElementConfigIndex));
   }

   if (oq_Warning == false)
   {
      this->mpc_Ui->pc_ChartSelectorWidget->AddDataSerie(
         ou32_DataPoolElementConfigIndex,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementId,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_DisplayName,
         this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex], c_Color,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementScaling.c_Unit);
   }
   else
   {
      this->mpc_Ui->pc_ChartSelectorWidget->AddDataSerie(
         ou32_DataPoolElementConfigIndex,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementId,
         orc_InvalidPlaceholderName,
         this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex], c_Color,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementScaling.c_Unit,
         oq_Warning, oq_Invalid, oe_InvalidPlaceholderDataPoolType, orc_ToolTipErrorTextHeading,
         orc_ToolTipErrorText);
   }

   // Configuration of line style
   c_Pen = pc_Graph->pen();
   c_Pen.setColor(c_Color);
   c_Pen.setWidth(mhs32_WIDTH_LINE_DEFAULT);
   pc_Graph->setPen(c_Pen);
   pc_Graph->setScatterStyle(QCPScatterStyle::ssDisc);

   tgl_assert(pc_VerticalAxis != NULL);
   if (pc_VerticalAxis != NULL)
   {
      // Configure axis
      this->m_UpdateVerticalAxisLabel(ou32_DataPoolElementConfigIndex);

      // Adapt all axis colors for this data element
      mh_AdaptAxisColor(pc_VerticalAxis, c_Color, this->mq_DarkMode);

      if (this->mc_Data.e_SettingVerticalAxisMode == C_PuiSvDbTabChart::eSETTING_YA_ALL_VISIBLE)
      {
         pc_VerticalAxis->setVisible(this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex]);
      }
   }

   pc_Graph->setVisible(this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex]);

   // Set the axis for the new data serie
   // SelectGraph configures the axis too
   this->SelectGraph(ou32_DataPoolElementConfigIndex);
} //lint !e429  //no memory leak because of the parent of pc_Series and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the chart and all graphs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ResetChart(void)
{
   int32_t s32_CounterItem;

   const QVector<float64_t> c_EmptyKeys;
   const QVector<float64_t> c_EmptyValues;

   for (s32_CounterItem = 0;
        s32_CounterItem < this->mpc_Ui->pc_Plot->graphCount();
        ++s32_CounterItem)
   {
      // Removing all previous data by setting empty data. Using clear function of data seems not to work
      this->mpc_Ui->pc_Plot->graph(s32_CounterItem)->setData(c_EmptyKeys, c_EmptyValues, true);
   }

   // Reset the range
   this->mf64_MaxValue = 1.0;
   this->mf64_MinValue = 0.0;

   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The data item visible state was changed

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification
   \param[in]  oq_Checked                       Flag if item is visible or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_DataItemToggled(const uint32_t ou32_DataPoolElementConfigIndex,
                                                    const bool oq_Checked)
{
   if (static_cast<int32_t>(ou32_DataPoolElementConfigIndex) < this->mpc_Ui->pc_Plot->graphCount())
   {
      this->mpc_Ui->pc_Plot->graph(ou32_DataPoolElementConfigIndex)->setVisible(oq_Checked);
   }

   // When all y axes are visible, the y axes of the deactivated elements shall not be visible
   if ((this->mc_Data.e_SettingVerticalAxisMode == C_PuiSvDbTabChart::eSETTING_YA_ALL_VISIBLE) &&
       (static_cast<int32_t>(ou32_DataPoolElementConfigIndex) < this->mc_DataElementConfigIndexToVerticalAxis.size()))
   {
      this->mc_DataElementConfigIndexToVerticalAxis.at(static_cast<int32_t>(ou32_DataPoolElementConfigIndex))->
      setVisible(
         oq_Checked);
   }

   if (ou32_DataPoolElementConfigIndex < this->mc_Data.c_DataPoolElementsActive.size())
   {
      auto && rc_Value = this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex];
      rc_Value = oq_Checked;
   }

   Q_EMIT (this->SigChartDataChanged());

   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The selection of a data item has changed

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_DataItemSelected(const uint32_t ou32_DataPoolElementConfigIndex)
{
   this->SelectGraph(ou32_DataPoolElementConfigIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redraws the graph
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_RedrawGraph(void)
{
   if (this->mq_DrawingActive == true)
   {
      this->mpc_Ui->pc_Plot->replot();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data element counter in title
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_UpdateElementCounter(void)
{
   // update counter label text
   this->mpc_Ui->pc_LabelTitle->setText(
      static_cast<QString>(C_GtGetText::h_GetText("Data Elements (%1)")).
      arg(this->mc_Data.c_DataPoolElementsConfig.size()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Resize selection widget

   Calculate minimum height of selector widget in correspondence to available space.
   This is necessary to make add button stick below last data element but avoid
   vanishing it if there are many elements.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ResizeSelectorWidget(void)
{
   // those values are also set in corresponding .ui file
   const int32_t s32_ELEMENT_HEIGHT = 25;
   const int32_t s32_MARGIN_ABOVE_ELEMENTS = 7;
   const int32_t s32_ElementVertSpace =
      s32_MARGIN_ABOVE_ELEMENTS +
      (static_cast<int32_t>(this->mc_Data.c_DataPoolElementsConfig.size()) * s32_ELEMENT_HEIGHT);
   const int32_t s32_AvailableSpace =
      this->mpc_Ui->pc_FrameLeft->height() -
      (this->mpc_Ui->pc_WidgetTitle->height() + this->mpc_Ui->pc_MeasurementInfoWidget->height() +
       this->mpc_Ui->pc_WidgetAddButton->height() + 26 /*some pixel puffer because spacer refuses height of 0px*/);

   if (s32_ElementVertSpace < s32_AvailableSpace)
   {
      // enough space available for all data elements
      this->mpc_Ui->pc_ChartSelectorWidget->setMinimumHeight(s32_ElementVertSpace);
      this->mpc_Ui->pc_ChartSelectorWidget->setMaximumHeight(s32_ElementVertSpace);
   }
   else
   {
      // show at least 4 data elements but make enough space for button
      const int32_t s32_Height = std::max((4 * s32_ELEMENT_HEIGHT), s32_AvailableSpace);

      // too much data elements, lets use the scroll area
      this->mpc_Ui->pc_ChartSelectorWidget->setMinimumHeight(s32_Height);
      this->mpc_Ui->pc_ChartSelectorWidget->setMaximumHeight(s32_Height);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of splitter move: make add button text tiny
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_OnSplitterMoved(void)
{
   if (this->mpc_Ui->pc_ChartSelectorWidget->width() > 200)
   {
      this->mpc_Ui->pc_ButtonAddData->setText(C_GtGetText::h_GetText("Add Data Element"));
      this->mpc_Ui->pc_ButtonAddData->setMaximumWidth(150);
      this->mpc_Ui->pc_ButtonAddData->setMinimumWidth(150);
   }
   else
   {
      this->mpc_Ui->pc_ButtonAddData->setText(C_GtGetText::h_GetText("Add"));
      this->mpc_Ui->pc_ButtonAddData->setMaximumWidth(70);
      this->mpc_Ui->pc_ButtonAddData->setMinimumWidth(70);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Cyclic update X axis
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_CyclicUpdateHorizontalAxis(void)
{
   const uint32_t u32_CurTime = stw::tgl::TglGetTickCount() - this->mu32_TimeStampOfStart;

   if (static_cast<float64_t>(u32_CurTime) > this->mpc_Ui->pc_Plot->xAxis->range().upper)
   {
      this->mpc_Ui->pc_Plot->xAxis->setRange(static_cast<float64_t>(u32_CurTime),
                                             this->mpc_Ui->pc_Plot->xAxis->range().size(), Qt::AlignRight);

      m_RedrawGraph();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for a changed range of the x axis
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_RangeChangedHorizontalAxis(void)
{
   if (this->mpc_Ui->pc_Plot->xAxis->range().lower <= 0.0)
   {
      if (this->mpc_Ui->pc_Plot->xAxis->range().upper <= 1.0)
      {
         this->mpc_Ui->pc_Plot->xAxis->setRangeUpper(1.0);
      }
      this->mpc_Ui->pc_Plot->xAxis->setRangeLower(0.0);
   }
   else if (this->mpc_Ui->pc_Plot->xAxis->range().upper <= 1.0)
   {
      this->mpc_Ui->pc_Plot->xAxis->setRangeUpper(1.0);
   }
   else
   {
      // Nothing to do
   }

   this->m_CheckMeasurementCursorsVisibility();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set icons depending on current dark / bright mode

   \param[in]  oq_DarkMode    Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_SetButtonIcons(const bool oq_DarkMode)
{
   this->mpc_Ui->pc_ButtonAddData->SetDarkMode(oq_DarkMode);

   if (oq_DarkMode == true)
   {
      this->mpc_Ui->pc_PushButtonManualRead->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconUpdateValueDark.svg",
         "://images/system_views/dashboards/tab_chart/IconUpdateValueDisabled.svg");
      this->mpc_Ui->pc_PushButtonManualAbort->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconUpdateValueDarkCancel.svg",
         "://images/system_views/dashboards/tab_chart/IconUpdateValueDisabledCancel.svg");

      this->mpc_Ui->pc_PushButtonPause->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconPauseDark.svg",
         "://images/system_views/dashboards/tab_chart/IconPauseDisabled.svg", "",
         "://images/system_views/dashboards/tab_chart/IconPlayDark.svg",
         "://images/system_views/dashboards/tab_chart/IconPlayDisabled.svg", "", "", "");

      this->mpc_ActionZoomSettingHorizontal->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconZoomAdjustXDark.svg"));
      this->mpc_ActionZoomSettingVertical->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconZoomAdjustYDark.svg"));
      this->mpc_ActionZoomSettingHorizontalVertical->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconZoomAdjustXYDark.svg"));

      this->mpc_Ui->pc_PushButtonZoomVsDragMode->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconZoomSelectionDark.svg",
         "://images/system_views/dashboards/tab_chart/IconZoomSelectionDisabled.svg");

      this->mpc_Ui->pc_PushButtonFitXY->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconFitXYDark.svg",
         "://images/system_views/dashboards/tab_chart/IconFitXYDisabled.svg");
      this->mpc_Ui->pc_PushButtonFitY->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconFitYDark.svg",
         "://images/system_views/dashboards/tab_chart/IconFitYDisabled.svg");
      this->mpc_Ui->pc_PushButtonFitX->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconFitXDark.svg",
         "://images/system_views/dashboards/tab_chart/IconFitXDisabled.svg");
      this->mpc_Ui->pc_PushButtonExtractData->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconExportCSVDark.svg",
         "://images/system_views/dashboards/tab_chart/IconExportCSVDisabled.svg");

      switch (me_SettingCursorMode)
      {
      case C_SyvDaChaPlotHandlerWidget::eSETTING_CM_MEASUREMENT_CURSOR:
         this->mpc_Ui->pc_PushButtonCursor->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDark.svg",
            "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg");
         break;
      case C_SyvDaChaPlotHandlerWidget::eSETTING_CM_TWO_DIFF_CURSOR:
         this->mpc_Ui->pc_PushButtonCursor->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconCursorDifferenceDark.svg",
            "://images/system_views/dashboards/tab_chart/IconCursorDifferenceDisabled.svg");
         break;
      case C_SyvDaChaPlotHandlerWidget::eSETTING_CM_NO_CURSOR: // Default case
      default:

         this->mpc_Ui->pc_PushButtonCursor->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg",
            "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg");
         break;
      }
      this->mpc_ActionCursorNoCursor->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg"));
      this->mpc_ActionCursorMeasurementCursor->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconCursorMeasurementDark.svg"));
      this->mpc_ActionCursorTwoDiffCursor->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconCursorDifferenceDark.svg"));

      this->mpc_Ui->pc_PushButtonOptions->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconChartOptionsDark.svg",
         "://images/system_views/dashboards/tab_chart/IconChartOptionsDisabled.svg");
      this->mpc_ActionToggleSamples->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconChartToggleSamplesDark.svg"));
      this->mpc_ActionVerticalAxisSettingAllVisible->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconShowSeveralYAxisDark.svg"));
   }
   else
   {
      this->mpc_Ui->pc_PushButtonManualRead->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconUpdateValue.svg",
         "://images/system_views/dashboards/tab_chart/IconUpdateValueDisabled.svg");
      this->mpc_Ui->pc_PushButtonManualAbort->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconUpdateValueCancel.svg",
         "://images/system_views/dashboards/tab_chart/IconUpdateValueDisabledCancel.svg");

      this->mpc_Ui->pc_PushButtonPause->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconPause.svg",
         "://images/system_views/dashboards/tab_chart/IconPauseDisabled.svg", "",
         "://images/system_views/dashboards/tab_chart/IconPlay.svg",
         "://images/system_views/dashboards/tab_chart/IconPlayDisabled.svg", "", "", "");

      this->mpc_ActionZoomSettingHorizontal->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconZoomAdjustX.svg"));
      this->mpc_ActionZoomSettingVertical->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconZoomAdjustY.svg"));
      this->mpc_ActionZoomSettingHorizontalVertical->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconZoomAdjustXY.svg"));

      this->mpc_Ui->pc_PushButtonZoomVsDragMode->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconZoomSelection.svg",
         "://images/system_views/dashboards/tab_chart/IconZoomSelectionDisabled.svg");

      this->mpc_Ui->pc_PushButtonFitXY->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconFitXY.svg",
         "://images/system_views/dashboards/tab_chart/IconFitXYDisabled.svg");
      this->mpc_Ui->pc_PushButtonFitY->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconFitY.svg",
         "://images/system_views/dashboards/tab_chart/IconFitYDisabled.svg");
      this->mpc_Ui->pc_PushButtonFitX->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconFitX.svg",
         "://images/system_views/dashboards/tab_chart/IconFitXDisabled.svg");
      this->mpc_Ui->pc_PushButtonExtractData->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconExportCSV.svg",
         "://images/system_views/dashboards/tab_chart/IconExportCSVDisabled.svg");

      switch (me_SettingCursorMode)
      {
      case C_SyvDaChaPlotHandlerWidget::eSETTING_CM_MEASUREMENT_CURSOR:
         this->mpc_Ui->pc_PushButtonCursor->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconCursorMeasurement.svg",
            "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg");
         break;
      case C_SyvDaChaPlotHandlerWidget::eSETTING_CM_TWO_DIFF_CURSOR:
         this->mpc_Ui->pc_PushButtonCursor->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconCursorDifference.svg",
            "://images/system_views/dashboards/tab_chart/IconCursorDifferenceDisabled.svg");
         break;
      case C_SyvDaChaPlotHandlerWidget::eSETTING_CM_NO_CURSOR: // Default case
      default:

         this->mpc_Ui->pc_PushButtonCursor->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg",
            "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg");
         break;
      }
      this->mpc_ActionCursorNoCursor->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg"));
      this->mpc_ActionCursorMeasurementCursor->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconCursorMeasurement.svg"));
      this->mpc_ActionCursorTwoDiffCursor->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconCursorDifference.svg"));

      this->mpc_Ui->pc_PushButtonOptions->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconChartOptions.svg",
         "://images/system_views/dashboards/tab_chart/IconChartOptionsDisabled.svg");
      this->mpc_ActionToggleSamples->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconChartToggleSamples.svg"));
      this->mpc_ActionVerticalAxisSettingAllVisible->setIcon(
         QIcon("://images/system_views/dashboards/tab_chart/IconShowSeveralYAxis.svg"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get a not already used random color

   There are 64 different colors. The colors will be selected by a random number. If all 64 colors are already used,
   a new section with the same 64 colors are used, which will be selected randomly again.

   New sections will be generated as long as necessary. No limitations.

   \return
   Found random signal color index
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_SyvDaChaPlotHandlerWidget::m_GetNextNotUsedColor(void)
{
   uint32_t u32_ItFree;
   uint32_t u32_SectionNumber;
   bool q_FreeColorFound = false;
   bool q_RandomColorFound = false;
   uint8_t u8_ColorIndex = 0U;

   srand(stw::tgl::TglGetTickCount());

   // Check for a free color in the already existing sections
   // Use the oldest section, if a free color is available
   for (u32_SectionNumber = 0U; u32_SectionNumber < this->mc_DataColorsUsed.size(); ++u32_SectionNumber)
   {
      for (u32_ItFree = 0UL; u32_ItFree < mhu8_COUNT_COLORS; ++u32_ItFree)
      {
         if (this->mc_DataColorsUsed[u32_SectionNumber][u32_ItFree] == false)
         {
            q_FreeColorFound = true;
            break;
         }
      }

      if (q_FreeColorFound == true)
      {
         break;
      }
   }

   if (q_FreeColorFound == false)
   {
      // No free colors in any section. Create a new one.
      this->m_PrepareNextColorSection();

      // u32_SectionNumber must be match with new added section
      tgl_assert(u32_SectionNumber < this->mc_DataColorsUsed.size());
   }

   // search a not used color with a random number in a section with at least one free entry
   do
   {
      const uint8_t u8_ColorCounter = static_cast<uint8_t>(rand()) % mhu8_COUNT_COLORS;

      if (this->mc_DataColorsUsed[u32_SectionNumber][u8_ColorCounter] == false)
      {
         u8_ColorIndex = u8_ColorCounter;
         // set the color variant as used
         this->mc_DataColorsUsed[u32_SectionNumber][u8_ColorCounter] = true;
         q_RandomColorFound = true;
      }
   }
   while (q_RandomColorFound == false);

   return u8_ColorIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns specific color

   The color will be marked as used in the first section which has the specific color not used already.
   If no section has the marked as unused, a new section will be added.

   \param[in]  ou8_Index   Index of color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_SetConcreteColorAsUsed(const uint8_t ou8_Index)
{
   // search the color to set the flag
   if (ou8_Index < mhu8_COUNT_COLORS)
   {
      uint32_t u32_SectionCounter;
      bool q_FreeSectionFound = false;

      // Search for a free entry in all sections
      for (u32_SectionCounter = 0U; u32_SectionCounter < this->mc_DataColorsUsed.size(); ++u32_SectionCounter)
      {
         if (this->mc_DataColorsUsed[u32_SectionCounter][ou8_Index] == false)
         {
            // set the color variant as used
            this->mc_DataColorsUsed[u32_SectionCounter][ou8_Index] = true;
            q_FreeSectionFound = true;
            break;
         }
      }

      if (q_FreeSectionFound == false)
      {
         // No free entry in the already existing sections, create a new one.
         this->m_PrepareNextColorSection();

         // u32_SectionNumber must be match with new added section
         tgl_assert(u32_SectionCounter < this->mc_DataColorsUsed.size());
         this->mc_DataColorsUsed[u32_SectionCounter][ou8_Index] = true;
      }
   }
   else
   {
      tgl_assert(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the used flag for a specific color to false

   In case of multiple sections, the color will be set to unused in the latest section.
   If the section is not the first section and has no further active colors, the section will be removed.

   \param[in]  ou8_Index   Index of color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_SetColorUnused(const uint8_t ou8_Index)
{
   uint32_t u32_ColorCounter;

   std::vector<std::array<bool, mhu8_COUNT_COLORS> >::reverse_iterator c_ItSection;

   // Search the color to reset the flag
   // Search the sections from behind to reduce the number of sections if possible
   for (c_ItSection = this->mc_DataColorsUsed.rbegin(); c_ItSection != this->mc_DataColorsUsed.rend();
        ++c_ItSection)
   {
      bool q_ColorFound = false;
      bool q_OtherColorUsed = false;

      if (ou8_Index < mhu8_COUNT_COLORS)
      {
         for (u32_ColorCounter = 0U; u32_ColorCounter < mhu8_COUNT_COLORS; ++u32_ColorCounter)
         {
            if ((u32_ColorCounter == ou8_Index) &&
                ((*c_ItSection)[u32_ColorCounter] == true))
            {
               // Set the used color to unused
               (*c_ItSection)[u32_ColorCounter] = false;
               q_ColorFound = true;
            }
            else if ((*c_ItSection)[u32_ColorCounter] == true)
            {
               // Check if any other color is used in the section too
               q_OtherColorUsed = true;
            }
            else
            {
               // Nothing to do
            }
         }
      }

      if (q_ColorFound == true)
      {
         if ((q_OtherColorUsed == false) &&
             (this->mc_DataColorsUsed.size() > 1))
         {
            // Color was disabled and no further active colors in this section and it is not the last section.
            // It can be removed.

            // the erase function needs the base iterator, but the base iterator has an offset of -1
            std::advance(c_ItSection, 1);
            this->mc_DataColorsUsed.erase(c_ItSection.base());
         }

         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prepares the next section with 64 bool values for the used color configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_PrepareNextColorSection(void)
{
   std::array<bool, mhu8_COUNT_COLORS> c_SingalsColorsUsed;
   c_SingalsColorsUsed.fill(false);
   this->mc_DataColorsUsed.push_back(c_SingalsColorsUsed);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the pointer the axis of a Datapool element index

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification

   \retval   Valid pointer
   \retval   NULL
*/
//----------------------------------------------------------------------------------------------------------------------
QCPAxis * C_SyvDaChaPlotHandlerWidget::m_GetVerticalAxis(const uint32_t ou32_DataPoolElementConfigIndex) const
{
   QCPAxis * pc_VerticalAxis = NULL;
   QCPAxisRect * const pc_AxisRect = this->mpc_Ui->pc_Plot->axisRect();

   tgl_assert(pc_AxisRect != NULL);
   if (pc_AxisRect != NULL)
   {
      pc_VerticalAxis = pc_AxisRect->axis(QCPAxis::atLeft, ou32_DataPoolElementConfigIndex);
   }

   return pc_VerticalAxis;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Styles an axis for selected or default state

   \param[in]  ou32_DataPoolElementConfigIndex  Data pool element configuration index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_UpdateVerticalAxisLabel(const uint32_t ou32_DataPoolElementConfigIndex)
{
   if (static_cast<int32_t>(ou32_DataPoolElementConfigIndex) < this->mc_DataElementConfigIndexToVerticalAxis.size())
   {
      const QString c_Name = this->mpc_Ui->pc_ChartSelectorWidget->GetDataElementName(ou32_DataPoolElementConfigIndex);
      const QString c_Unit = this->mpc_Ui->pc_ChartSelectorWidget->GetDataElementUnit(ou32_DataPoolElementConfigIndex);
      QCPAxis * const pc_VerticalAxis =
         this->mc_DataElementConfigIndexToVerticalAxis.at(ou32_DataPoolElementConfigIndex);

      if (pc_VerticalAxis != NULL)
      {
         if (c_Unit != "")
         {
            pc_VerticalAxis->setLabel(c_Name + " [" + c_Unit + "]");
         }
         else
         {
            pc_VerticalAxis->setLabel(c_Name);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Styles an axis for selected or default state

   \param[in]  opc_Axis       Axis for styling
   \param[in]  oq_Selected    Flag if styling for state selected or default
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::mh_AdaptVerticalAxisStyle(QCPAxis * const opc_Axis, const bool oq_Selected)
{
   int32_t s32_Width;
   bool q_Bold;
   QFont c_Font;
   QPen c_Pen;

   if (oq_Selected == true)
   {
      q_Bold = true;
      s32_Width = mhs32_WIDTH_LINE_SELECTED;
   }
   else
   {
      q_Bold = false;
      s32_Width = mhs32_WIDTH_LINE_DEFAULT;
   }

   // Change line thickness
   c_Pen = opc_Axis->basePen();
   c_Pen.setWidth(s32_Width);
   opc_Axis->setBasePen(c_Pen);

   c_Pen = opc_Axis->tickPen();
   c_Pen.setWidth(s32_Width);
   opc_Axis->setTickPen(c_Pen);

   c_Pen = opc_Axis->subTickPen();
   c_Pen.setWidth(s32_Width);
   opc_Axis->setSubTickPen(c_Pen);

   // Adapt font boldness
   c_Font = opc_Axis->labelFont();
   c_Font.setBold(q_Bold);
   opc_Axis->setLabelFont(c_Font);

   c_Font = opc_Axis->tickLabelFont();
   c_Font.setBold(q_Bold);
   opc_Axis->setTickLabelFont(c_Font);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Styles an axis for selected or default state

   \param[in]  opc_Axis    Axis for styling
   \param[in]  orc_Color   New color for axis
   \param[in]  oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::mh_AdaptAxisColor(QCPAxis * const opc_Axis, const QColor & orc_Color,
                                                    const bool oq_DarkMode)
{
   QPen c_Pen;

   opc_Axis->setLabelColor(orc_Color);
   opc_Axis->setTickLabelColor(orc_Color);
   c_Pen = opc_Axis->tickPen();
   c_Pen.setColor(orc_Color);
   opc_Axis->setTickPen(c_Pen);
   c_Pen = opc_Axis->subTickPen();
   c_Pen.setColor(orc_Color);
   opc_Axis->setSubTickPen(c_Pen);

   c_Pen = opc_Axis->basePen();
   c_Pen.setColor(orc_Color);
   opc_Axis->setBasePen(c_Pen);

   // Adapt the grid of the y axis
   mh_AdaptAxisGridColor(opc_Axis, oq_DarkMode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Styles all y axis grids

   \param[in]  oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_AdaptAllVerticalAxisGridColors(const bool oq_DarkMode)
{
   if (this->mc_DataElementConfigIndexToVerticalAxis.size() == 0)
   {
      // No data element with own y axis exist, adapt the default y axis
      mh_AdaptAxisGridColor(this->mpc_Ui->pc_Plot->axisRect()->axis(QCPAxis::atLeft, 0), oq_DarkMode);
   }
   else
   {
      int32_t s32_Counter;

      for (s32_Counter = 0; s32_Counter < this->mc_DataElementConfigIndexToVerticalAxis.size(); ++s32_Counter)
      {
         mh_AdaptAxisGridColor(this->mc_DataElementConfigIndexToVerticalAxis.at(s32_Counter), oq_DarkMode);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Styles an axis grid

   \param[in]  opc_Axis    Axis for styling
   \param[in]  oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::mh_AdaptAxisGridColor(const QCPAxis * const opc_Axis, const bool oq_DarkMode)
{
   QPen c_Pen;

   opc_Axis->grid()->setVisible(true);
   c_Pen = opc_Axis->grid()->pen();
   if (oq_DarkMode == false)
   {
      c_Pen.setColor(mc_STYLE_GUIDE_COLOR_11);
   }
   else
   {
      c_Pen.setColor(mc_STYLE_GUIDE_COLOR_52);
   }
   c_Pen.setStyle(Qt::DashLine);
   opc_Axis->grid()->setPen(c_Pen);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scales the current range of y axes with a little bit of space

   \param[in]  opc_Axis    Optional specific axis. If NULL all y axes will be adapted
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_AdaptVerticalAxisWithSpace(QCPAxis * const opc_Axis)
{
   // Offset of free space above and below the measurement points summed
   const float64_t f64_PIXEL_OFFSET = 10.0;
   const float64_t f64_Height = static_cast<float64_t>(this->mpc_Ui->pc_Plot->axisRect()->height());
   const float64_t f64_ScaleFactor = (f64_Height + f64_PIXEL_OFFSET) / f64_Height;

   if (opc_Axis != NULL)
   {
      opc_Axis->rescale();
      opc_Axis->scaleRange(f64_ScaleFactor, opc_Axis->range().center());
   }
   else
   {
      int32_t s32_Counter;

      for (s32_Counter = 0; s32_Counter < this->mc_DataElementConfigIndexToVerticalAxis.size(); ++s32_Counter)
      {
         if (s32_Counter < this->mpc_Ui->pc_Plot->graphCount())
         {
            QCPGraph * const pc_Graph = this->mpc_Ui->pc_Plot->graph(s32_Counter);

            if (((pc_Graph != NULL) && (pc_Graph->visible())) &&
                (pc_Graph->dataCount() > 0))

            {
               QCPAxis * const pc_Axis = this->mc_DataElementConfigIndexToVerticalAxis.at(s32_Counter);

               pc_Axis->scaleRange(f64_ScaleFactor, pc_Axis->range().center());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for the signal caused by a clicked line item

   Special case: The second cursor was clicked. The measured values of the second cursor will be shown
   on the selector widget till the mouse button is released

   \param[in]  opc_CursorItem    Pointer to the clicked item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_CursorItemClicked(const C_SyvDaChaPlotCursorItem * const opc_CursorItem)
{
   if ((opc_CursorItem == this->mpc_SecondCursor) &&
       (this->me_SettingCursorMode == eSETTING_CM_TWO_DIFF_CURSOR))
   {
      this->mpc_Ui->pc_ChartSelectorWidget->SetMeasurementState(C_SyvDaChaDataItemWidget::eMEASUREMENT_VAL_SECOND);
      this->mq_IsSecondCursorClicked = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for the signal caused by a released line item

   \param[in]  opc_CursorItem    Pointer to the clicked item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_CursorItemReleased(C_SyvDaChaPlotCursorItem * const opc_CursorItem)
{
   uint32_t u32_SelectedElement = 0U;
   bool q_CursorAdapted = false;

   // Reaction in case of the moved second cursor
   if ((opc_CursorItem == this->mpc_SecondCursor) &&
       (this->me_SettingCursorMode != eSETTING_CM_NO_CURSOR))
   {
      this->mpc_Ui->pc_ChartSelectorWidget->SetMeasurementState(C_SyvDaChaDataItemWidget::eMEASUREMENT_VAL_FIRST);
      this->mq_IsSecondCursorClicked = false;
   }

   // Adapt the position of the measurement cursor to the measurement point of the tracer
   if (this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(u32_SelectedElement) == true)
   {
      // If no data was received for the selected element, the tracer is NULL and nothing is to do in this case
      QCPItemTracer * const pc_Tracer = this->mc_ItemTracers.at(static_cast<int32_t>(u32_SelectedElement));
      if (pc_Tracer != NULL)
      {
         const float64_t f64_TimeOfPoint = pc_Tracer->position->key();
         opc_CursorItem->UpdatePosition(f64_TimeOfPoint);
         this->m_CursorItemMovedOnHorizontalAxis(opc_CursorItem, f64_TimeOfPoint);
         this->m_RedrawGraph();
         q_CursorAdapted = true;
      }
   }

   if (q_CursorAdapted == false)
   {
      // Check if the cursor was moved in below zero area of the x axis if not already adapted by a tracer
      if (opc_CursorItem->start->coords().x() < 0.0)
      {
         opc_CursorItem->UpdatePosition(0.0);
         this->m_CursorItemMovedOnHorizontalAxis(opc_CursorItem, 0.0);
         this->m_RedrawGraph();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for the signal caused by a moved line item

   \param[in]  opc_CursorItem    Pointer to the clicked item
   \param[in]  of64_PosHorizontal         New position of line on X axis
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_CursorItemMovedOnHorizontalAxis(
   const C_SyvDaChaPlotCursorItem * const opc_CursorItem, const float64_t of64_PosHorizontal)
{
   float64_t f64_TracerTimeSelectedElement = of64_PosHorizontal;

   // Update the values in the selector widget
   if ((opc_CursorItem == this->mpc_FirstCursor) ||
       ((this->mq_IsSecondCursorClicked == true) &&
        (opc_CursorItem == this->mpc_SecondCursor)))
   {
      // Only in this two combinations shown measurement values are updated
      uint32_t u32_ElementCounter;
      C_SyvDaChaDataItemWidget::E_MeasurementValueState e_MeasurementValueToUpdate;
      uint32_t u32_SelectedElement = 0U;
      this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(u32_SelectedElement);

      if (opc_CursorItem == this->mpc_FirstCursor)
      {
         e_MeasurementValueToUpdate = C_SyvDaChaDataItemWidget::eMEASUREMENT_VAL_FIRST;
      }
      else
      {
         e_MeasurementValueToUpdate = C_SyvDaChaDataItemWidget::eMEASUREMENT_VAL_SECOND;
      }

      // Update the time as key for all tracers. Then calling m_RedrawGraph to update all tracers
      for (u32_ElementCounter = 0UL; u32_ElementCounter < static_cast<uint32_t>(this->mc_ItemTracers.size());
           ++u32_ElementCounter)
      {
         QCPItemTracer * const pc_Tracer = this->mc_ItemTracers.at(static_cast<int32_t>(u32_ElementCounter));
         if (pc_Tracer != NULL)
         {
            pc_Tracer->setGraphKey(of64_PosHorizontal);
            pc_Tracer->updatePosition();
         }
      }

      // Redraw before using the updated tracers. The redraw causes a call of updatePosition of the tracers
      // So we save extra calls and save a little performance
      this->m_RedrawGraph();

      // Update all elements with updated values and updated positions of the tracers
      for (u32_ElementCounter = 0UL; u32_ElementCounter < static_cast<uint32_t>(this->mc_ItemTracers.size());
           ++u32_ElementCounter)
      {
         QString c_Value = C_GtGetText::h_GetText("NA");
         QCPItemTracer * const pc_Tracer = this->mc_ItemTracers.at(static_cast<int32_t>(u32_ElementCounter));

         if (pc_Tracer != NULL)
         {
            float64_t f64_Value;
            C_OscNodeDataPoolContent c_Tmp = this->mc_DataPoolElementContentMin[u32_ElementCounter];
            const C_PuiSvDbDataElementScaling c_TmpScaling;
            f64_Value = pc_Tracer->position->value();

            if (u32_ElementCounter == u32_SelectedElement)
            {
               f64_TracerTimeSelectedElement = pc_Tracer->position->key();
            }

            // Show the measured value in the selector widget
            C_OscNodeDataPoolContentUtil::h_SetValueInContent(f64_Value, c_Tmp, 0U);
            // No need of scaling here, it is already scaled
            c_Value = this->mc_DataPoolElementDisplayFormatter[u32_ElementCounter].
                      GetSingleValueContentFormatted(c_Tmp, 0U, c_TmpScaling, NULL);
         }

         this->mpc_Ui->pc_ChartSelectorWidget->UpdateDataSerieMeasurementValue(
            u32_ElementCounter,
            e_MeasurementValueToUpdate,
            c_Value);
      }
   }
   else
   {
      this->m_RedrawGraph();
   }

   // Check if the cursor was moved on the left side of the x axis
   this->m_CheckMeasurementCursorsVisibility();

   // Update the labels with the absolute time of the cursor
   if (opc_CursorItem == this->mpc_FirstCursor)
   {
      QString c_Text = static_cast<QString>(C_GtGetText::h_GetText("t1 = %1 ms")).arg(f64_TracerTimeSelectedElement);
      c_Text.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(), Qt::CaseInsensitive);

      this->mf64_MeasuredTimeFirstCursor = f64_TracerTimeSelectedElement;
      this->mpc_Ui->pc_LabelMeasCursor1->setText(c_Text);
   }
   else if (opc_CursorItem == this->mpc_SecondCursor)
   {
      QString c_Text = static_cast<QString>(C_GtGetText::h_GetText("t2 = %1 ms")).arg(f64_TracerTimeSelectedElement);
      c_Text.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(), Qt::CaseInsensitive);

      this->mf64_MeasuredTimeSecondCursor = f64_TracerTimeSelectedElement;
      this->mpc_Ui->pc_LabelMeasCursor2->setText(c_Text);
   }
   else
   {
      tgl_assert(false);
   }

   // If the second cursor exist, the difference is available and possible
   if (this->mpc_SecondCursor != NULL)
   {
      const float64_t f64_Diff = this->mf64_MeasuredTimeSecondCursor - this->mf64_MeasuredTimeFirstCursor;

      QString c_Text = static_cast<QString>(C_GtGetText::h_GetText("dt = %1 ms")).arg(f64_Diff);
      c_Text.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(), Qt::CaseInsensitive);

      this->mpc_Ui->pc_LabelMeasDiff->setText(c_Text);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for updating both cursors in case of changed coordinates
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_UpdateMeasurementCursors(void)
{
   if (this->me_SettingCursorMode != eSETTING_CM_NO_CURSOR)
   {
      // In case of active measurement, the measurement must be repeated when zoomed or dragged
      const float64_t f64_CurrPosFirstCursorHorizontal = this->mpc_FirstCursor->start->coords().x();
      this->m_CursorItemMovedOnHorizontalAxis(this->mpc_FirstCursor, f64_CurrPosFirstCursorHorizontal);

      if (this->me_SettingCursorMode == eSETTING_CM_TWO_DIFF_CURSOR)
      {
         const float64_t f64_CurrPosSecondCursorHorizontal = this->mpc_SecondCursor->start->coords().x();
         this->m_CursorItemMovedOnHorizontalAxis(this->mpc_SecondCursor, f64_CurrPosSecondCursorHorizontal);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for setting visibility of cursors in case of changed position

   Hide the measurement cursors if the left plot border is reached
   The line will be get invisible, but the tag and label will not
   On the right side of the plot it is no problem
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_CheckMeasurementCursorsVisibility(void)
{
   const float64_t f64_HorizontalAxisLower = this->mpc_Ui->pc_Plot->xAxis->range().lower;

   if (this->mpc_FirstCursor != NULL)
   {
      this->mpc_FirstCursor->SetItemsNextToPlotVisible(
         this->mpc_FirstCursor->start->coords().x() >= f64_HorizontalAxisLower);

      if (this->mpc_SecondCursor != NULL)
      {
         this->mpc_SecondCursor->SetItemsNextToPlotVisible(this->mpc_SecondCursor->start->coords().x() >=
                                                           f64_HorizontalAxisLower);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Zoom the chart one step in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ZoomIn(void)
{
   this->m_Zoom(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Zoom the chart one step out
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ZoomOut(void)
{
   this->m_Zoom(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Zooming in or out dependent of the zoom mode

   \param[in]  oq_ZoomIn   true  -> Zooming in
                           false -> Zooming out
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_Zoom(const bool oq_ZoomIn)
{
   QCPAxisRect * const pc_AxisRect = this->mpc_Ui->pc_Plot->axisRect();

   tgl_assert(pc_AxisRect != NULL);
   if (pc_AxisRect != NULL)
   {
      float64_t f64_Factor;
      QRectF c_RectChart = pc_AxisRect->rect();
      const QPoint c_Center = pc_AxisRect->center();

      if (oq_ZoomIn == true)
      {
         f64_Factor = 0.85;
      }
      else
      {
         f64_Factor = 1.176;
      }

      if ((this->mc_Data.e_SettingZoomMode == C_PuiSvDbTabChart::eSETTING_ZM_XY) ||
          (this->mc_Data.e_SettingZoomMode == C_PuiSvDbTabChart::eSETTING_ZM_X))
      {
         c_RectChart.setWidth(c_RectChart.width() * f64_Factor);
      }
      if ((this->mc_Data.e_SettingZoomMode == C_PuiSvDbTabChart::eSETTING_ZM_XY) ||
          (this->mc_Data.e_SettingZoomMode == C_PuiSvDbTabChart::eSETTING_ZM_Y))
      {
         c_RectChart.setHeight(c_RectChart.height() * f64_Factor);
      }

      // Zooming without changing the position
      c_RectChart.moveCenter(c_Center);

      pc_AxisRect->zoom(c_RectChart);
      this->m_RedrawGraph();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set zoom mode XY
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ZoomModeHorizontalVertical(void)
{
   this->m_SetZoomMode(C_PuiSvDbTabChart::eSETTING_ZM_XY);

   this->mpc_ActionZoomSettingHorizontal->setChecked(false);
   this->mpc_ActionZoomSettingVertical->setChecked(false);
   this->mpc_ActionZoomSettingHorizontalVertical->setChecked(true);

   this->m_OnChangeZoomMode(C_PuiSvDbTabChart::eSETTING_ZM_XY);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set zoom mode X
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ZoomModeHorizontal(void)
{
   this->m_SetZoomMode(C_PuiSvDbTabChart::eSETTING_ZM_X);

   this->mpc_ActionZoomSettingHorizontal->setChecked(true);
   this->mpc_ActionZoomSettingVertical->setChecked(false);
   this->mpc_ActionZoomSettingHorizontalVertical->setChecked(false);

   this->m_OnChangeZoomMode(C_PuiSvDbTabChart::eSETTING_ZM_X);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set zoom mode Y
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ZoomModeVertical(void)
{
   this->m_SetZoomMode(C_PuiSvDbTabChart::eSETTING_ZM_Y);

   this->mpc_ActionZoomSettingHorizontal->setChecked(false);
   this->mpc_ActionZoomSettingVertical->setChecked(true);
   this->mpc_ActionZoomSettingHorizontalVertical->setChecked(false);

   this->m_OnChangeZoomMode(C_PuiSvDbTabChart::eSETTING_ZM_Y);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the changed zoom mode

   \param[in]  oe_SettingZoomMode   Current used zoom mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_SetZoomMode(const C_PuiSvDbTabChart::E_SettingZoomMode oe_SettingZoomMode)
{
   this->m_ConfigureZoomMode(oe_SettingZoomMode);

   this->mc_Data.e_SettingZoomMode = oe_SettingZoomMode;
   Q_EMIT (this->SigChartDataChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adaption of the UI when the zoom mode is changed

   \param[in]  oe_SettingZoomMode   Current used zoom mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_OnChangeZoomMode(const C_PuiSvDbTabChart::E_SettingZoomMode oe_SettingZoomMode)
{
   if (this->mq_DarkMode == true)
   {
      switch (oe_SettingZoomMode)
      {
      case C_PuiSvDbTabChart::eSETTING_ZM_X:
         this->mpc_Ui->pc_PushButtonZoomMode->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomAdjustXDark.svg");
         this->mpc_Ui->pc_PushButtonZoomIn->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepXPlusDark.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepXPlusDisabled.svg");
         this->mpc_Ui->pc_PushButtonZoomOut->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepXMinusDark.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepXMinusDisabled.svg");
         break;
      case C_PuiSvDbTabChart::eSETTING_ZM_Y:
         this->mpc_Ui->pc_PushButtonZoomMode->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomAdjustYDark.svg");
         this->mpc_Ui->pc_PushButtonZoomIn->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepYPlusDark.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepYPlusDisabled.svg");
         this->mpc_Ui->pc_PushButtonZoomOut->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepYMinusDark.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepYMinusDisabled.svg");
         break;
      case C_PuiSvDbTabChart::eSETTING_ZM_XY: // Default case
      default:
         this->mpc_Ui->pc_PushButtonZoomMode->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomAdjustXYDark.svg");
         this->mpc_Ui->pc_PushButtonZoomIn->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepXYPlusDark.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepXYPlusDisabled.svg");
         this->mpc_Ui->pc_PushButtonZoomOut->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepXYMinusDark.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepXYMinusDisabled.svg");
         break;
      }
   }
   else
   {
      switch (oe_SettingZoomMode)
      {
      case C_PuiSvDbTabChart::eSETTING_ZM_X:
         this->mpc_Ui->pc_PushButtonZoomMode->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomAdjustX.svg");
         this->mpc_Ui->pc_PushButtonZoomIn->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepXPlus.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepXPlusDisabled.svg");
         this->mpc_Ui->pc_PushButtonZoomOut->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepXMinus.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepXMinusDisabled.svg");
         break;
      case C_PuiSvDbTabChart::eSETTING_ZM_Y:
         this->mpc_Ui->pc_PushButtonZoomMode->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomAdjustY.svg");
         this->mpc_Ui->pc_PushButtonZoomIn->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepYPlus.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepYPlusDisabled.svg");
         this->mpc_Ui->pc_PushButtonZoomOut->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepYMinus.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepYMinusDisabled.svg");
         break;
      case C_PuiSvDbTabChart::eSETTING_ZM_XY: // Default case
      default:
         this->mpc_Ui->pc_PushButtonZoomMode->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomAdjustXY.svg");
         this->mpc_Ui->pc_PushButtonZoomIn->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepXYPlus.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepXYPlusDisabled.svg");
         this->mpc_Ui->pc_PushButtonZoomOut->SetSvg(
            "://images/system_views/dashboards/tab_chart/IconZoomStepXYMinus.svg",
            "://images/system_views/dashboards/tab_chart/IconZoomStepXYMinusDisabled.svg");
         break;
      }
   }

   this->mpc_Ui->pc_PushButtonZoomMode->repaint();
   this->mpc_Ui->pc_PushButtonZoomIn->repaint();
   this->mpc_Ui->pc_PushButtonZoomOut->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the changed zoom mode

   \param[in]  oe_SettingZoomMode   Current used zoom mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ConfigureZoomMode(const C_PuiSvDbTabChart::E_SettingZoomMode oe_SettingZoomMode)
{
   QCPAxisRect * const pc_AxisRect = this->mpc_Ui->pc_Plot->axisRect();

   tgl_assert(pc_AxisRect != NULL);
   if (pc_AxisRect != NULL)
   {
      switch (oe_SettingZoomMode)
      {
      case C_PuiSvDbTabChart::eSETTING_ZM_XY:
         pc_AxisRect->setRangeZoom(Qt::Horizontal | Qt::Vertical);
         break;
      case C_PuiSvDbTabChart::eSETTING_ZM_X:
         pc_AxisRect->setRangeZoom(Qt::Horizontal);
         break;
      case C_PuiSvDbTabChart::eSETTING_ZM_Y:
         pc_AxisRect->setRangeZoom(Qt::Vertical);
         break;
      default:
         pc_AxisRect->setRangeZoom(Qt::Horizontal | Qt::Vertical);
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Short function description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_UpdateDragAxesConfiguration(void)
{
   QCPAxisRect * const pc_AxisRect = this->mpc_Ui->pc_Plot->axisRect();

   tgl_assert(pc_AxisRect != NULL);
   if (pc_AxisRect != NULL)
   {
      pc_AxisRect->setRangeDragAxes(pc_AxisRect->axes(QCPAxis::atBottom), pc_AxisRect->axes(QCPAxis::atLeft));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the active flag for the drag mode

   \param[in]  oq_Active   Flag if zoom mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_SetZoomVsDragMode(const bool oq_Active)
{
   this->m_ConfigureZoomVsDragMode(oq_Active);

   this->mc_Data.q_IsZoomModeActive = oq_Active;
   Q_EMIT (this->SigChartDataChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the active flag for the drag mode

   \param[in]  oq_Active   Flag if zoom mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ConfigureZoomVsDragMode(const bool oq_Active)
{
   if (oq_Active == true)
   {
      this->mpc_Ui->pc_Plot->setSelectionRectMode(QCP::srmZoom);
   }
   else
   {
      this->mpc_Ui->pc_Plot->setSelectionRectMode(QCP::srmNone);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set zoom mode Y
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_VerticalAxisSettingAllVisible(void)
{
   if (this->mpc_ActionVerticalAxisSettingAllVisible->isChecked() == true)
   {
      this->m_SetVerticalAxisMode(C_PuiSvDbTabChart::eSETTING_YA_ALL_VISIBLE);
   }
   else
   {
      this->m_SetVerticalAxisMode(C_PuiSvDbTabChart::eSETTING_YA_ONE_VISIBLE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the changed Y axis mode

   \param[in]  oe_SettingVerticalAxisMode  Current used Y axis mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_SetVerticalAxisMode(
   const C_PuiSvDbTabChart::E_SettingVerticalAxisMode oe_SettingVerticalAxisMode)
{
   this->m_ConfigureVerticalAxisMode(oe_SettingVerticalAxisMode);

   this->mc_Data.e_SettingVerticalAxisMode = oe_SettingVerticalAxisMode;
   Q_EMIT (this->SigChartDataChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the changed Y Axis mode

   \param[in]  oe_SettingVerticalAxisMode  Current used Y axis mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ConfigureVerticalAxisMode(
   const C_PuiSvDbTabChart::E_SettingVerticalAxisMode oe_SettingVerticalAxisMode)
{
   QCPAxisRect * const pc_AxisRect = this->mpc_Ui->pc_Plot->axisRect();

   tgl_assert(pc_AxisRect != NULL);
   if (pc_AxisRect != NULL)
   {
      // Adapt all Y axes
      const QList<QCPAxis *> c_ListAxes = pc_AxisRect->axes(QCPAxis::atLeft);
      uint32_t u32_CurrentElement;

      if (this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(u32_CurrentElement) == true)
      {
         int32_t s32_CounterVerticalAxis;
         tgl_assert(c_ListAxes.size() == static_cast<int32_t>(this->mc_Data.c_DataPoolElementsActive.size()));

         for (s32_CounterVerticalAxis = 0; s32_CounterVerticalAxis < c_ListAxes.size(); ++s32_CounterVerticalAxis)
         {
            QCPAxis * const pc_VerticalAxis = c_ListAxes.at(s32_CounterVerticalAxis);

            switch (oe_SettingVerticalAxisMode)
            {
            case C_PuiSvDbTabChart::eSETTING_YA_ONE_VISIBLE:
               // Restore styling
               mh_AdaptVerticalAxisStyle(pc_VerticalAxis, false);

               if (static_cast<int32_t>(u32_CurrentElement) == s32_CounterVerticalAxis)
               {
                  pc_VerticalAxis->setVisible(true);
               }
               else
               {
                  pc_VerticalAxis->setVisible(false);
               }
               break;
            case C_PuiSvDbTabChart::eSETTING_YA_ALL_VISIBLE:
               pc_VerticalAxis->setVisible(this->mc_Data.c_DataPoolElementsActive[s32_CounterVerticalAxis]);

               if (static_cast<int32_t>(u32_CurrentElement) == s32_CounterVerticalAxis)
               {
                  mh_AdaptVerticalAxisStyle(pc_VerticalAxis, true);
               }
               break;
            default:
               break;
            }
         }

         this->m_RedrawGraph();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set cursor mode to no cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_CursorModeNoCursor(void)
{
   // No adaption on data layer necessary due it is not a saved configuration
   this->m_ConfigureCursorMode(C_SyvDaChaPlotHandlerWidget::eSETTING_CM_NO_CURSOR);

   this->mpc_ActionCursorNoCursor->setChecked(true);
   this->mpc_ActionCursorMeasurementCursor->setChecked(false);
   this->mpc_ActionCursorTwoDiffCursor->setChecked(false);

   this->mpc_Ui->pc_PushButtonCursor->SetSvg(
      "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg",
      "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg");
   this->mpc_Ui->pc_PushButtonCursor->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set cursor mode to no cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_CursorModeMeasurementCursor(void)
{
   // No adaption on data layer necessary due it is not a saved configuration
   this->m_ConfigureCursorMode(C_SyvDaChaPlotHandlerWidget::eSETTING_CM_MEASUREMENT_CURSOR);

   this->mpc_ActionCursorNoCursor->setChecked(false);
   this->mpc_ActionCursorMeasurementCursor->setChecked(true);
   this->mpc_ActionCursorTwoDiffCursor->setChecked(false);

   if (this->mq_DarkMode == true)
   {
      this->mpc_Ui->pc_PushButtonCursor->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDark.svg",
         "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg");
   }
   else
   {
      this->mpc_Ui->pc_PushButtonCursor->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconCursorMeasurement.svg",
         "://images/system_views/dashboards/tab_chart/IconCursorMeasurementDisabled.svg");
   }
   this->mpc_Ui->pc_PushButtonCursor->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set cursor mode to no cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_CursorModeTwoDiffCursor(void)
{
   // No adaption on data layer necessary due it is not a saved configuration
   this->m_ConfigureCursorMode(C_SyvDaChaPlotHandlerWidget::eSETTING_CM_TWO_DIFF_CURSOR);

   this->mpc_ActionCursorNoCursor->setChecked(false);
   this->mpc_ActionCursorMeasurementCursor->setChecked(false);
   this->mpc_ActionCursorTwoDiffCursor->setChecked(true);

   if (this->mq_DarkMode == true)
   {
      this->mpc_Ui->pc_PushButtonCursor->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconCursorDifferenceDark.svg",
         "://images/system_views/dashboards/tab_chart/IconCursorDifferenceDisabled.svg");
   }
   else
   {
      this->mpc_Ui->pc_PushButtonCursor->SetSvg(
         "://images/system_views/dashboards/tab_chart/IconCursorDifference.svg",
         "://images/system_views/dashboards/tab_chart/IconCursorDifferenceDisabled.svg");
   }
   this->mpc_Ui->pc_PushButtonCursor->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the cursor mode depending of pause and connection state changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_AdaptCursorMode(void)
{
   if ((this->mq_IsConnected == true) &&
       (this->mpc_Ui->pc_PushButtonPause->isChecked() == false))
   {
      this->mpc_ActionCursorNoCursor->setChecked(true);
      // The chart is getting new data. In this scenario the measurement features is deactivated
      this->m_CursorModeNoCursor();

      this->mpc_Ui->pc_PushButtonCursor->setEnabled(false);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonCursor->setEnabled(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the changed cursor mode

   \param[in]  oe_SettingCursorMode    Current used curosr mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ConfigureCursorMode(const E_SettingCursorMode oe_SettingCursorMode)
{
   // Calculate start position depending on x axis range
   const float64_t f64_StartHorizontalAxis = this->mpc_Ui->pc_Plot->xAxis->range().lower;
   const float64_t f64_RangeHorizontalAxis = this->mpc_Ui->pc_Plot->xAxis->range().upper - f64_StartHorizontalAxis;
   const float64_t f64_InitHorizontalPosFirstCursor = f64_StartHorizontalAxis + (f64_RangeHorizontalAxis * 0.3);
   const float64_t f64_InitHorizontalPosSecondCursor = f64_StartHorizontalAxis + (f64_RangeHorizontalAxis * 0.6);

   // No real change, do nothing
   if (this->me_SettingCursorMode != oe_SettingCursorMode)
   {
      bool q_RemoveSecondCursor = false;
      if (oe_SettingCursorMode == eSETTING_CM_NO_CURSOR)
      {
         int32_t s32_CounterTracers;

         // Configure the selector widget
         this->mpc_Ui->pc_ChartSelectorWidget->SetMeasurementState(
            C_SyvDaChaDataItemWidget::eMEASUREMENT_VAL_DEACTIVATED);

         // Deactivate all cursors
         this->mpc_Ui->pc_Plot->setInteraction(QCP::iSelectItems, false);

         if (this->mpc_FirstCursor != NULL)
         {
            this->mpc_Ui->pc_Plot->RemoveCursorItem(this->mpc_FirstCursor);
            this->mpc_FirstCursor = NULL;
         }

         q_RemoveSecondCursor = true;

         // Remove all item tracers
         for (s32_CounterTracers = 0; s32_CounterTracers < this->mc_ItemTracers.size(); ++s32_CounterTracers)
         {
            QCPItemTracer * const pc_Tracer = this->mc_ItemTracers[s32_CounterTracers];
            if (pc_Tracer != NULL)
            {
               this->mpc_Ui->pc_Plot->removeItem(pc_Tracer);
            }
         }
         this->mc_ItemTracers.clear();

         this->mpc_Ui->pc_LabelMeasCursor1->setVisible(false);
      }
      else
      {
         this->mpc_Ui->pc_Plot->setInteraction(QCP::iSelectItems, true);
         this->mpc_Ui->pc_LabelMeasCursor1->setVisible(true);

         // Configure the selector widget
         // Set always the first measurement value. The second measurement value will be set when the cursor is clicked
         // actively
         this->mpc_Ui->pc_ChartSelectorWidget->SetMeasurementState(C_SyvDaChaDataItemWidget::eMEASUREMENT_VAL_FIRST);

         // Add the item tracers if not already happened due to an changed mode
         if (this->mc_ItemTracers.size() == 0)
         {
            int32_t s32_GraphCounter;
            uint32_t u32_CurrentElement = 0U;
            const bool q_ElementExist = this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(u32_CurrentElement);

            for (s32_GraphCounter = 0; s32_GraphCounter < this->mpc_Ui->pc_Plot->graphCount(); ++s32_GraphCounter)
            {
               QCPGraph * const pc_Graph = this->mpc_Ui->pc_Plot->graph(s32_GraphCounter);
               if (pc_Graph->dataCount() > 0)
               {
                  QCPItemTracer * const pc_Tracer = new QCPItemTracer(this->mpc_Ui->pc_Plot);
                  if ((q_ElementExist == true) &&
                      (static_cast<int32_t>(u32_CurrentElement) == s32_GraphCounter))
                  {
                     pc_Tracer->setStyle(QCPItemTracer::tsSquare);
                  }
                  else
                  {
                     pc_Tracer->setStyle(QCPItemTracer::tsNone);
                  }
                  pc_Tracer->setGraph(pc_Graph);
                  this->mc_ItemTracers.append(pc_Tracer);
               } //lint !e429  //no memory leak because of the parent of pc_Tracer and the Qt memory management
               else
               {
                  // No points added to the chart, no tracer necessary. In addition to that a tracer connected to
                  // a graph wiht no data causes debug outputs
                  this->mc_ItemTracers.append(NULL);
               }
            }
         }

         // Activate in both cases the first cursor
         if (this->mpc_FirstCursor == NULL)
         {
            // Activate the second cursor
            this->m_CreateCursor(&this->mpc_FirstCursor, f64_InitHorizontalPosFirstCursor, mc_STYLE_GUIDE_COLOR_CH1,
                                 static_cast<QString>(C_GtGetText::h_GetText(" t1 ")));
         }

         if (oe_SettingCursorMode == eSETTING_CM_TWO_DIFF_CURSOR)
         {
            if (this->mpc_SecondCursor == NULL)
            {
               // Activate the second cursor
               this->m_CreateCursor(&this->mpc_SecondCursor, f64_InitHorizontalPosSecondCursor,
                                    mc_STYLE_GUIDE_COLOR_CH2, static_cast<QString>(C_GtGetText::h_GetText(" t2 ")));
            }
         }
         else
         {
            q_RemoveSecondCursor = true;
         }
      }
      this->mpc_Ui->pc_LabelMeasCursor2->setVisible(!q_RemoveSecondCursor);
      this->mpc_Ui->pc_LabelMeasDiff->setVisible(!q_RemoveSecondCursor);

      if (q_RemoveSecondCursor == true)
      {
         // Deactivate the second cursor
         if (this->mpc_SecondCursor != NULL)
         {
            this->mpc_Ui->pc_Plot->RemoveCursorItem(this->mpc_SecondCursor);
            this->mpc_SecondCursor = NULL;
         }
      }

      // Save the changed setting
      this->me_SettingCursorMode = oe_SettingCursorMode;

      this->m_RedrawGraph();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create cursor

   \param[out]  oppc_ItemCursor  Pointer to pointer for cursor line which will be created here
   \param[in]   of64_InitHorizontalPos    Initial position of line and tag
   \param[in]   orc_Color        Color of line and tag
   \param[in]   orc_LabelText    Text of label of cursor tag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_CreateCursor(C_SyvDaChaPlotCursorItem ** const oppc_ItemCursor,
                                                 const float64_t of64_InitHorizontalPos, const QColor & orc_Color,
                                                 const QString & orc_LabelText)
{
   C_SyvDaChaPlotCursorItem * const pc_Cursor = new C_SyvDaChaPlotCursorItem(this->mpc_Ui->pc_Plot,
                                                                             of64_InitHorizontalPos,
                                                                             orc_LabelText);

   // Set color
   pc_Cursor->UpdateColors(mc_STYLE_GUIDE_COLOR_0, orc_Color);

   // Assign the output pointer with the new pointer
   (*oppc_ItemCursor) = pc_Cursor;

   this->mpc_Ui->pc_Plot->RegisterCursorItem(pc_Cursor);

   // Init the measurement info
   this->m_CursorItemMovedOnHorizontalAxis(*oppc_ItemCursor, of64_InitHorizontalPos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update pause

   \param[in]  oq_Pause    Pause
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_PauseClicked(const bool oq_Pause)
{
   this->m_UpdatePause(oq_Pause);
   this->m_AdaptCursorMode();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update pause

   \param[in]  oq_Pause    Pause
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_UpdatePause(const bool oq_Pause)
{
   this->mq_PauseState = oq_Pause;
   if (oq_Pause == true)
   {
      this->mc_CyclicChartUpdateTrigger.stop();
      this->mpc_Ui->pc_PushButtonManualRead->setEnabled(false);
   }
   else
   {
      if (this->mq_IsConnected == true)
      {
         this->mc_CyclicChartUpdateTrigger.start();
         this->mpc_Ui->pc_PushButtonManualRead->setEnabled(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Triggering a manual read of data elements configured with "On Trigger"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ManualReadTriggered(void)
{
   this->mpc_Ui->pc_PushButtonManualRead->setVisible(false);
   this->mpc_Ui->pc_PushButtonManualAbort->setVisible(this->mq_ManualOperationPossible);

   Q_EMIT (this->SigManualReadTriggered());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fit XY signals
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_FitHorizontalVerticalSignals(void)
{
   this->mpc_Ui->pc_Plot->rescaleAxes(true);
   // Scale all y axes with a little bit space around
   this->m_AdaptVerticalAxisWithSpace(NULL);
   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fit Y signals
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_FitVerticalSignals(void)
{
   bool q_IsFirst = true;

   for (int32_t s32_ItGraph = 0; s32_ItGraph < this->mpc_Ui->pc_Plot->graphCount(); ++s32_ItGraph)
   {
      QCPGraph * const pc_Graph = this->mpc_Ui->pc_Plot->graph(s32_ItGraph);

      if (((pc_Graph != NULL) && (pc_Graph->visible())) &&
          (pc_Graph->dataCount() > 0))

      {
         pc_Graph->rescaleValueAxis(!q_IsFirst, false);
         this->m_AdaptVerticalAxisWithSpace(pc_Graph->valueAxis());
         q_IsFirst = false;
      }
   }
   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fit X signals
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_FitHorizontalSignals(void)
{
   bool q_IsFirst = true;

   for (int32_t s32_ItGraph = 0; s32_ItGraph < this->mpc_Ui->pc_Plot->graphCount(); ++s32_ItGraph)
   {
      QCPGraph * const pc_Graph = this->mpc_Ui->pc_Plot->graph(s32_ItGraph);

      if ((pc_Graph != NULL) && (pc_Graph->visible()))
      {
         pc_Graph->rescaleKeyAxis(!q_IsFirst);
         q_IsFirst = false;
      }
   }
   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show or hide sample points

   \param[in]  oq_ShowSamplePoints  Flag for showing sample points
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ShowSamplePoints(const bool oq_ShowSamplePoints)
{
   this->mq_ShowSamplePointState = oq_ShowSamplePoints;
   for (int32_t s32_Counter = 0; s32_Counter < this->mpc_Ui->pc_Plot->graphCount(); s32_Counter++)
   {
      if (oq_ShowSamplePoints == true)
      {
         this->mpc_Ui->pc_Plot->graph(s32_Counter)->setScatterStyle(QCPScatterStyle::ssDisc);
      }
      else
      {
         this->mpc_Ui->pc_Plot->graph(s32_Counter)->setScatterStyle(QCPScatterStyle::ssNone);
      }
   }
   this->m_RedrawGraph();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load state

   \param[in]  oq_IsPaused                Is paused
   \param[in]  orq_AreSamplePointsShown   Flag if sample points are shown
   \param[in]  orc_ScreenState            Screen state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_LoadState(const bool oq_IsPaused, const bool orq_AreSamplePointsShown,
                                              const std::vector<std::array<float64_t, 4> > & orc_ScreenState)
{
   uint32_t u32_ScreenStateCounter;

   this->m_UpdatePause(oq_IsPaused);
   this->m_ShowSamplePoints(orq_AreSamplePointsShown);

   if (orc_ScreenState.size() > 0UL)
   {
      // Adapt X axis
      const std::array<float64_t, 4> & rc_DisplayRange = orc_ScreenState[0UL];
      this->mpc_Ui->pc_Plot->xAxis->setRange(rc_DisplayRange[0UL], rc_DisplayRange[1UL]);
   }
   else
   {
      this->FitDefault();
   }

   // Adapt all Y axes
   for (u32_ScreenStateCounter = 0U; u32_ScreenStateCounter < orc_ScreenState.size(); ++u32_ScreenStateCounter)
   {
      const QCPAxis * const pc_VerticalAxis = this->m_GetVerticalAxis(u32_ScreenStateCounter);
      if (pc_VerticalAxis != NULL)
      {
         const std::array<float64_t, 4> & rc_DisplayRange = orc_ScreenState[u32_ScreenStateCounter];
         if (u32_ScreenStateCounter == 0U)
         {
            this->mpc_Ui->pc_Plot->yAxis->setRange(rc_DisplayRange[2UL], rc_DisplayRange[3UL]);
         }
         if (static_cast<int32_t>(u32_ScreenStateCounter) < this->mc_DataElementConfigIndexToVerticalAxis.size())
         {
            this->mc_DataElementConfigIndexToVerticalAxis[u32_ScreenStateCounter]->setRange(rc_DisplayRange[2UL],
                                                                                            rc_DisplayRange[3UL]);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save state

   \param[out]     orq_IsPaused              Is paused
   \param[out]     ors32_SplitterLeftWidth   Width of left splitter widget
   \param[out]     orq_AreSamplePointsShown  Flag for showing sample points
   \param[in,out]  orc_ScreenState           Screen state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_SaveState(bool & orq_IsPaused, int32_t & ors32_SplitterLeftWidth,
                                              bool & orq_AreSamplePointsShown, std::vector<std::array<float64_t, 4> > &
                                              orc_ScreenState)
{
   int32_t s32_VerticalAxisCounter;
   const QList<int32_t> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

   // The values for the X axis are always the same
   std::array<float64_t, 4> c_DisplayRange =
   {
      {this->mpc_Ui->pc_Plot->xAxis->range().lower, this->mpc_Ui->pc_Plot->xAxis->range().upper, 0.0, 0.0}
   };

   orc_ScreenState.clear();

   for (s32_VerticalAxisCounter = 0U; s32_VerticalAxisCounter < this->mc_DataElementConfigIndexToVerticalAxis.size();
        ++s32_VerticalAxisCounter)
   {
      QCPAxis * const pc_VerticalAxis = this->m_GetVerticalAxis(s32_VerticalAxisCounter);
      if (pc_VerticalAxis != NULL)
      {
         c_DisplayRange[2] = pc_VerticalAxis->range().lower;
         c_DisplayRange[3] = pc_VerticalAxis->range().upper;
      }
      orc_ScreenState.push_back(c_DisplayRange);
   }

   orq_IsPaused = this->mq_PauseState;
   orq_AreSamplePointsShown = this->mq_ShowSamplePointState;
   if ((c_Sizes.size() > 0) && (c_Sizes[0] > 0))
   {
      ors32_SplitterLeftWidth = c_Sizes[0];
   }
   else
   {
      ors32_SplitterLeftWidth = 300;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Importing Chart data as .CSV file and saving it in a prefered local location
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_ExtractDataToCsv(void)
{
   if (mc_File.open(QIODevice::ReadWrite  | QIODevice::Truncate))
   {
      mc_File.resize(0); //Truncate the file
      QString c_DataElements;
      const uint32_t u32_DataElementSize = this->mc_Data.c_DataPoolElementsConfig.size();
      const auto pc_Plot = this->mpc_Ui->pc_Plot;
      const auto pc_ChartSelectorWidget = this->mpc_Ui->pc_ChartSelectorWidget;
      int32_t s32_MaxDataElement = 0;
      if (u32_DataElementSize > 0)
      {
         s32_MaxDataElement = pc_Plot->graph(0)->dataCount();
      }

      for (uint32_t u32_DataElementCounter = 0U; u32_DataElementCounter < u32_DataElementSize; ++u32_DataElementCounter)
      {
         const QCPGraph * const pc_Graph = pc_Plot->graph(u32_DataElementCounter);
         if (pc_Graph->visible())
         {
            if (pc_Graph->dataCount() > s32_MaxDataElement)
            {
               s32_MaxDataElement = pc_Graph->dataCount();
            }
            //Creating here headings for all data elements and units
            c_DataElements += "Timestamp (ms);" + pc_ChartSelectorWidget->GetDataElementName(
               u32_DataElementCounter) + " (" +
                              pc_ChartSelectorWidget->GetDataElementUnit(u32_DataElementCounter) + ")";
            if (u32_DataElementCounter < (u32_DataElementSize - 1))
            {
               c_DataElements += ";";
            }
         }
      }

      //Writing created data elements and units headings to .csv file
      mc_Out << c_DataElements + "\n";

      for (int32_t s32_GraphDataCounter = 0U; s32_GraphDataCounter < s32_MaxDataElement;
           ++s32_GraphDataCounter)
      {
         for (uint32_t u32_DataElementCounter = 0U; u32_DataElementCounter < u32_DataElementSize;
              ++u32_DataElementCounter)
         {
            const QCPGraph * const pc_Graph = pc_Plot->graph(u32_DataElementCounter);
            if (pc_Graph->visible())
            {
               if (s32_GraphDataCounter < pc_Graph->dataCount())
               {
                  mc_Out << pc_Graph->data()->at(s32_GraphDataCounter)->key << ";" << pc_Graph->data()->at(
                     s32_GraphDataCounter)->mainValue();
               }
               else
               {
                  mc_Out << ";";
               }
               if (u32_DataElementCounter < (u32_DataElementSize - 1))
               {
                  mc_Out << ";";
               }
            }
         }
         mc_Out << "\n";
      }

      mc_File.close();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the location to save chart data export csv file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotHandlerWidget::m_SetSaveLocationToCsv(void)
{
   Q_EMIT (this->SigGetCurrentDashboardTabName());
   QString c_Folder = C_UsHandler::h_GetInstance()->GetLastKnownCsvExportPath();

   if (c_Folder.isEmpty())
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   const QString c_DefaultFilename = m_SaveCsvAs() + ".csv";

   const QString c_FilterName = C_GtGetText::h_GetText("CSV (*.csv)");
   const QString c_ExtractCsvFilePath = C_OgeWiUtil::h_GetSaveFileName(
      this, C_GtGetText::h_GetText("Save Export data as CSV"), c_Folder, c_FilterName, c_DefaultFilename,
      QFileDialog::DontConfirmOverwrite); // overwrite is handled later

   if (!c_ExtractCsvFilePath.isEmpty())
   {
      mc_File.setFileName(c_ExtractCsvFilePath);
      C_UsHandler::h_GetInstance()->SetLastKnownCsvExportPath(c_ExtractCsvFilePath);
      this->m_ExtractDataToCsv();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gives string value for saving CSV with name of format yyyy_MM_dd__HH_mm_ss_zzz_ProjectName_
 *  DashboardName

   \return
   return c_SaveCsvAs, the name of the csv to be saved as
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaChaPlotHandlerWidget::m_SaveCsvAs(void) const
{
   const QDateTime c_DateTime = QDateTime::currentDateTime();
   const QString c_SaveCsvAs = c_DateTime.toString("yyyy_MM_dd__HH_mm_ss_zzz") + "__" +
                               C_PuiProject::h_GetInstance()->GetName() + "__" + this->mc_CurrentDashboardTabName;

   return c_SaveCsvAs;
}
