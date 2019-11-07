//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a chart with its data configuration and data selection.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPen>

#include "stwerrors.h"

#include "constants.h"

#include "C_SyvDaItChartWidget.h"
#include "ui_C_SyvDaItChartWidget.h"

#include "C_GtGetText.h"
#include "C_OSCUtils.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvDbWidgetBase.h"
#include "TGLTime.h"
#include "TGLUtils.h"
#include "C_OSCLoggingHandler.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"
#include "C_OSCNodeDataPoolContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace QtCharts;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QColor C_SyvDaItChartWidget::mhac_DataColors[10] =
{
   QColor(192, 0, 0),
   QColor(0, 112, 192),
   QColor(0, 176, 80),
   QColor(255, 0, 0),
   QColor(255, 192, 0),
   QColor(146, 208, 80),
   QColor(0, 176, 240),
   QColor(0, 32, 96),
   QColor(255, 242, 0),
   QColor(112, 48, 160)
};

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex             Index of system view
   \param[in]     ou32_MaximumDataElements   Maximum number of shown data elements of the widget
   \param[in,out] opc_Parent                 Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItChartWidget::C_SyvDaItChartWidget(const uint32 ou32_ViewIndex, const uint32 ou32_MaximumDataElements,
                                           QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItChartWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_MaximumDataElements(ou32_MaximumDataElements),
   msn_WidthLineSelected(3),
   msn_WidthLineDefault(2),
   mf64_MaxTime(60000.0),
   // 60 seconds as initial value
   mf64_MaxValue(5.0),
   mf64_MinValue(0.0),
   mq_ValueZoomed(false),
   mf64_ZoomFactor(1.0),
   mf64_ScrollOffsetX(0.0),
   mf64_ScrollOffsetY(0.0),
   mf64_MaxValueZoomed(5.0),
   mf64_MinValueZoomed(0.0),
   mf64_ReferenceMaxValueZoomed(0.0),
   mf64_ReferenceMinValueZoomed(0.0),
   mf64_ReferenceRangeZoomed(0.0),
   mu32_TimeStampOfStart(0U),
   mf64_DefaultTimeSlot(60000.0),
   // 60 seconds as default value
   mf64_CurrentTimeSlot(this->mf64_DefaultTimeSlot),
   mq_DrawingActive(true)
{
   QPen c_Pen;
   QFont c_Font;

   this->mc_DataColorsUsed.resize(this->mu32_MaximumDataElements);

   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_ChartSelectorWidget->SetView(ou32_ViewIndex);

   // Create chart and its view
   mpc_Chart = new C_OgeChaChartBase();
   this->mpc_Chart->legend()->setVisible(false);

   mpc_ChartView = new C_OgeChaViewBase(this->mpc_Chart, this->mpc_Ui->pc_ChartViewWidget);
   this->mpc_Ui->pc_VerticalLayoutChartView->addWidget(this->mpc_ChartView);
   this->mpc_ChartView->setRenderHint(QPainter::Antialiasing);

   // Create the axis
   mpc_AxisTime = new QValueAxis();
   mpc_AxisValue = new QValueAxis();
   mpc_AxisValueInvisible = new QValueAxis();

   // Customize the axis
   c_Pen.setWidth(1);

   //Convert point to pixel
   c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
   c_Font.setPixelSize(c_Font.pointSize());

   this->mpc_AxisTime->setLabelsFont(c_Font);
   this->mpc_AxisTime->setTitleFont(c_Font);
   this->mpc_AxisTime->setLinePen(c_Pen);
   this->mpc_AxisValue->setLabelsFont(c_Font);
   this->mpc_AxisValue->setTitleFont(c_Font);
   this->mpc_AxisValue->setLinePen(c_Pen);

   this->mpc_AxisTime->setGridLineVisible(true);
   this->mpc_AxisValue->setGridLineVisible(true);

   this->mpc_AxisValueInvisible->setVisible(false);

   this->mpc_Chart->addAxis(this->mpc_AxisTime, Qt::AlignBottom);
   this->mpc_Chart->addAxis(this->mpc_AxisValue, Qt::AlignLeft);
   this->mpc_Chart->addAxis(this->mpc_AxisValueInvisible, Qt::AlignLeft);

   // Init the range
   this->mpc_AxisValue->setMax(this->mf64_MaxValue);
   this->mpc_AxisValue->setMin(this->mf64_MinValue);
   this->mpc_AxisValueInvisible->setMax(this->mf64_MaxValue);
   this->mpc_AxisValueInvisible->setMin(this->mf64_MinValue);
   this->mpc_AxisTime->setMax(this->mf64_MaxTime);
   this->mpc_AxisTime->setMin(0.0);
   this->mpc_AxisTime->setTitleText("[ms]");

   // Configure the splitter
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 1);

   connect(this->mpc_Ui->pc_ChartSelectorWidget, &C_SyvDaItChartDataSelectorWidget::SigDataItemToggled,
           this, &C_SyvDaItChartWidget::m_DataItemToggled);
   connect(this->mpc_Ui->pc_ChartSelectorWidget, &C_SyvDaItChartDataSelectorWidget::SigDataItemSelected,
           this, &C_SyvDaItChartWidget::m_DataItemSelected);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_SyvDaItChartSettingsWidget::SigZoomModeChanged,
           this, &C_SyvDaItChartWidget::m_SettingZoomModeChanged);
   connect(this->mpc_ChartView, &C_OgeChaViewBase::SigYZoomed,
           this, &C_SyvDaItChartWidget::m_YZoomed);
   connect(this->mpc_ChartView, &C_OgeChaViewBase::SigZoomReseted,
           this, &C_SyvDaItChartWidget::m_ZoomReseted);
   connect(this->mpc_ChartView, &C_OgeChaViewBase::SigScrolled,
           this, &C_SyvDaItChartWidget::m_Scrolled);

   this->SetDisplayStyle(C_PuiSvDbWidgetBase::eOPENSYDE, false);
   this->SetWidthOfDataSeriesSelector(330);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItChartWidget::~C_SyvDaItChartWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of the elements and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the chart data as initialize configuration

   \param[in]     orc_Data       Chart data with configured data elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::SetData(const C_PuiSvDbChart & orc_Data)
{
   this->mc_Data = orc_Data;

   tgl_assert(this->mpc_Chart != NULL);
   if (this->mpc_Chart != NULL)
   {
      tgl_assert(this->mc_Data.c_DataPoolElementsActive.size() == this->mc_Data.c_DataPoolElementsConfig.size());

      for (uint32 u32_It = 0; u32_It < this->mc_Data.c_DataPoolElementsConfig.size(); ++u32_It)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = this->mc_Data.c_DataPoolElementsConfig[u32_It];
         const C_OSCNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                       rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                       rc_Config.c_ElementId.u32_ListIndex,
                                                                       rc_Config.c_ElementId.u32_ElementIndex);
         bool q_Warning = false;
         bool q_Invalid = false;
         QString c_InvalidNamePlaceholder = "";
         C_OSCNodeDataPool::E_Type e_InvalidDataPoolTypePlaceholder = C_OSCNodeDataPool::eDIAG;
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
            C_OSCNodeDataPoolContent c_Tmp;
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
            c_ToolTipErrorText = C_GtGetText::h_GetText("Data element was deleted in SYSTEM DEFINITION");
         }
         else
         {
            const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
            tgl_assert(pc_View != NULL);
            if (pc_View != NULL)
            {
               const std::vector<uint8> & rc_ActiveFlags = pc_View->GetNodeActiveFlags();
               if (rc_Config.c_ElementId.u32_NodeIndex < rc_ActiveFlags.size())
               {
                  if (rc_ActiveFlags[rc_Config.c_ElementId.u32_NodeIndex] == 0U)
                  {
                     // Node with data element is not active in current view
                     q_Warning = true;
                     c_ToolTipErrorTextHeading = C_GtGetText::h_GetText("Configuration warning");
                     c_ToolTipErrorText = C_GtGetText::h_GetText("There is a data element of an inactive node");
                  }
               }
            }
         }

         this->m_AddDataSerie(u32_It, q_Warning, q_Invalid, c_InvalidNamePlaceholder, e_InvalidDataPoolTypePlaceholder,
                              c_ToolTipErrorTextHeading,
                              c_ToolTipErrorText);
      }

      tgl_assert(this->mc_Data.c_DataPoolElementsActive.size() == this->mc_DataPoolElementContentMin.size());

      this->mpc_Ui->pc_SettingsWidget->SetData(this->mc_Data);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the chart data with the element configuration

   \return
   Chart data
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbChart & C_SyvDaItChartWidget::GetData(void)
{
   return this->mc_Data;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to activate or deactivate drawing of performance heavy widgets

   \param[in] oq_Active Flag if widgets should currently be drawn
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::SetDrawingActive(const bool oq_Active)
{
   this->mq_DrawingActive = oq_Active;

   //Painting
   this->update();
   if (this->mpc_Chart != NULL)
   {
      this->mpc_Chart->SetDrawingActive(oq_Active);
      this->mpc_Chart->update();
   }
   if (this->mpc_ChartView != NULL)
   {
      this->mpc_ChartView->SetDrawingActive(oq_Active);
      this->mpc_ChartView->update();
   }

   //Axis
   this->mpc_AxisTime->setVisible(oq_Active);
   this->mpc_AxisValue->setVisible(oq_Active);
   this->UpdateTimeAxis();
   this->UpdateValueAxis();

   //Line series
   for (uint32 u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_Data.c_DataPoolElementsConfig.size();
        ++u32_ConfigCounter)
   {
      if (u32_ConfigCounter < this->mc_DataPoolElementsDataSeries.size())
      {
         QLineSeries * const pc_LineSerie = this->mc_DataPoolElementsDataSeries[u32_ConfigCounter];
         if (pc_LineSerie != NULL)
         {
            // Activate the drawing of the line only if the element is enabled
            const bool q_Enabled = this->mpc_Ui->pc_ChartSelectorWidget->GetDataElementToggledState(u32_ConfigCounter);
            pc_LineSerie->setVisible(oq_Active && q_Enabled);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style     New style type
   \param[in] oq_DarkMode  Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   if (this->mpc_Chart != NULL)
   {
      QBrush c_Brush;

      switch (oe_Style)
      {
      case C_PuiSvDbWidgetBase::eOPENSYDE:
         if (oq_DarkMode == true)
         {
            // Color for background
            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_36);
            this->mpc_Chart->setBackgroundBrush(c_Brush);

            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_8);

            // Customize the axis
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_2);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_2);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_2);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }
         else
         {
            // Color for background
            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_12);
            this->mpc_Chart->setBackgroundBrush(c_Brush);

            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_10);

            // Customize the axis and gridline
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_34);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }

         break;

      case C_PuiSvDbWidgetBase::eFLAT:
         if (oq_DarkMode == true)
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_8);

            // Customize the axis
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_0);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }
         else
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_26); //34);

            // Customize the axis and gridline
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_34);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }

         break;

      case C_PuiSvDbWidgetBase::eSKEUOMORPH:
         if (oq_DarkMode == true)
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_34);

            // Customize the axis
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_0);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_34);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }
         else
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_10);

            // Customize the axis and gridline
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_33);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_34);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }

         break;

      case C_PuiSvDbWidgetBase::eOPENSYDE_2:
         if (oq_DarkMode == true)
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_8);

            // Customize the axis
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_0);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }
         else
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_10);

            // Customize the axis and gridline
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_33);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_6);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_6);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }

         break;

      default:
         // Nothing to do
         break;
      }
      //this->ReInitializeSize();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the width of the data series selector widget left of the splitter

   \param[in]     osn_Width       Width of the selector widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::SetWidthOfDataSeriesSelector(const sintn osn_Width) const
{
   this->mpc_Ui->pc_Splitter->SetFirstSegment(osn_Width);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the width of the data series selector widget left of the splitter

   \return
   Width of the selector widget
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SyvDaItChartWidget::GetWidthOfDataSeriesSelector(void) const
{
   sintn sn_Width = 250;
   const QList<sintn> & rc_List = this->mpc_Ui->pc_Splitter->sizes();

   if (rc_List.size() > 0)
   {
      sn_Width = rc_List.at(0);
   }

   return sn_Width;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current id of the selected data element

   \param[out]     oru32_DataPoolElementConfigIndex         Datapool element configuration index

   \return
   true     data element exists
   false    data element does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItChartWidget::GetCurrentDataSerie(stw_types::uint32 & oru32_DataPoolElementConfigIndex) const
{
   return this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(oru32_DataPoolElementConfigIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active      Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::ConnectionActiveChanged(const bool oq_Active)
{
   if (oq_Active == true)
   {
      this->m_ResetChart();
      this->mu32_TimeStampOfStart = stw_tgl::TGL_GetTickCount();
   }
   else
   {
      // In case of disconnect the transmission errors must be reseted, but not the warnings
      this->mpc_Ui->pc_ChartSelectorWidget->ResetError();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a specific data serie

   \param[in]     orc_DataPoolElementId   Datapool element identification
   \param[in]     orc_ElementScaling      Datapool element scaling configuration

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
   C_CONFIG Chart not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDaItChartWidget::AddNewDataSerie(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                             const C_PuiSvDbDataElementScaling & orc_ElementScaling)
{
   sint32 s32_Return = C_CONFIG;

   tgl_assert(this->mpc_Chart != NULL);
   if (this->mpc_Chart != NULL)
   {
      if (this->mc_Data.c_DataPoolElementsConfig.size() < this->mu32_MaximumDataElements)
      {
         C_PuiSvDbNodeDataElementConfig c_Config;
         const uint32 u32_NewIndex = this->mc_Data.c_DataPoolElementsConfig.size();

         const C_OSCNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                       orc_DataPoolElementId.u32_DataPoolIndex,
                                                                       orc_DataPoolElementId.u32_ListIndex,
                                                                       orc_DataPoolElementId.u32_ElementIndex);
         if (pc_Element != NULL)
         {
            c_Config.c_ElementId = orc_DataPoolElementId;
            c_Config.c_ElementScaling = orc_ElementScaling;

            this->mc_Data.c_DataPoolElementsConfig.push_back(c_Config);
            this->mc_Data.c_DataPoolElementsActive.push_back(true);

            // Save the minimum to have a content instance of the element to have the type
            this->mc_DataPoolElementContentMin.push_back(pc_Element->c_MinValue);

            this->m_AddDataSerie(u32_NewIndex);

            s32_Return = C_NO_ERR;
         }
         else
         {
            s32_Return = C_RANGE;
         }
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current data serie

   \param[out]    orc_ElementId     Data element id of removed data serie

   \return
   true     data element removed
   false    nothing removed or a multiple registration of an element id was removed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItChartWidget::RemoveDataSerie(C_PuiSvDbNodeDataPoolListElementId & orc_ElementId)
{
   bool q_Return;
   uint32 u32_DataPoolElementConfigIndex;

   q_Return = this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(u32_DataPoolElementConfigIndex);

   if (q_Return == true)
   {
      q_Return = this->mpc_Ui->pc_ChartSelectorWidget->RemoveDataSerie(u32_DataPoolElementConfigIndex);
   }

   if ((q_Return == true) &&
       (this->mpc_Chart != NULL))
   {
      uint32 u32_Counter;

      // Remove data serie
      if (u32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataSeries.size())
      {
         this->mpc_Chart->removeSeries(this->mc_DataPoolElementsDataSeries[u32_DataPoolElementConfigIndex]);
         delete this->mc_DataPoolElementsDataSeries[u32_DataPoolElementConfigIndex];
         this->mc_DataPoolElementsDataSeries.erase(
            this->mc_DataPoolElementsDataSeries.begin() + u32_DataPoolElementConfigIndex);
      }

      // Remove used color
      if (u32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataColorIndexes.size())
      {
         tgl_assert(this->mc_DataColorsUsed.size() >
                    this->mc_DataPoolElementsDataColorIndexes[u32_DataPoolElementConfigIndex]);
         std::vector<bool>::reference c_Value =
            this->mc_DataColorsUsed[this->mc_DataPoolElementsDataColorIndexes[u32_DataPoolElementConfigIndex]];

         c_Value = false;

         this->mc_DataPoolElementsDataColorIndexes.erase(
            this->mc_DataPoolElementsDataColorIndexes.begin() + u32_DataPoolElementConfigIndex);
      }

      if (this->mc_DataPoolElementsDataSeries.size() > 0)
      {
         // Select an other item
         this->SelectDataSeriesAxis(0);
         this->mpc_Ui->pc_ChartSelectorWidget->SelectDataSerie(0);
      }
      else
      {
         this->mpc_AxisValue->setTitleText("");
      }

      if (u32_DataPoolElementConfigIndex < this->mc_Data.c_DataPoolElementsConfig.size())
      {
         orc_ElementId = this->mc_Data.c_DataPoolElementsConfig[u32_DataPoolElementConfigIndex].c_ElementId;

         // Check if there is an further registration of this data element
         for (u32_Counter = 0U; u32_Counter < this->mc_Data.c_DataPoolElementsConfig.size(); ++u32_Counter)
         {
            if ((u32_Counter != u32_DataPoolElementConfigIndex) &&
                (orc_ElementId == this->mc_Data.c_DataPoolElementsConfig[u32_Counter].c_ElementId))
            {
               // An other registration found. The upper layer shall think nothing happened
               q_Return = false;
            }
         }

         // Remove configuration itself
         tgl_assert(this->mc_Data.RemoveElement(u32_DataPoolElementConfigIndex) == C_NO_ERR);
         this->mc_DataPoolElementContentMin.erase(
            this->mc_DataPoolElementContentMin.begin() + u32_DataPoolElementConfigIndex);
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Selects and shows the axix for the specific datapool element

   \param[in]     orc_DataPoolElementId   Datapool element identification

   \return
   possible return value(s) and description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::SelectDataSeriesAxis(const uint32 ou32_DataPoolElementConfigIndex)
{
   if ((ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataSeries.size()) &&
       (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataColorIndexes.size()))
   {
      QLineSeries * const pc_LineSerie = this->mc_DataPoolElementsDataSeries[ou32_DataPoolElementConfigIndex];
      const QColor c_Color = C_SyvDaItChartWidget::mhac_DataColors[
         this->mc_DataPoolElementsDataColorIndexes[ou32_DataPoolElementConfigIndex]];
      uint32 u32_CounterLine;
      const QString c_Name = this->mpc_Ui->pc_ChartSelectorWidget->GetDataElementName(ou32_DataPoolElementConfigIndex);
      const QString c_Unit = this->mpc_Ui->pc_ChartSelectorWidget->GetDataElementUnit(ou32_DataPoolElementConfigIndex);
      QBrush c_Brush;
      QPen c_Pen;

      pc_LineSerie->attachAxis(this->mpc_AxisValue);

      // Adapt the width of the selected data series
      c_Pen = pc_LineSerie->pen();
      c_Pen.setWidth(msn_WidthLineSelected);
      pc_LineSerie->setPen(c_Pen);

      // Adapt all axis colors for this data element
      this->mpc_AxisValue->setLinePenColor(c_Color);
      this->mpc_AxisValue->setLabelsColor(c_Color);
      c_Brush.setColor(c_Color);
      this->mpc_AxisValue->setTitleBrush(c_Brush);
      this->mpc_AxisValue->setTitleText(c_Name + " [" + c_Unit + "]");

      // Detach all other data series from the visible axis
      for (u32_CounterLine = 0U;
           u32_CounterLine < this->mc_DataPoolElementsDataSeries.size();
           ++u32_CounterLine)
      {
         if (u32_CounterLine != ou32_DataPoolElementConfigIndex)
         {
            const QList<QAbstractAxis *> & rc_ListAxis =
               this->mc_DataPoolElementsDataSeries[u32_CounterLine]->attachedAxes();
            QList<QAbstractAxis *>::const_iterator c_ItAxis;

            // Detach the axis only if attached. Search it.
            for (c_ItAxis = rc_ListAxis.begin(); c_ItAxis != rc_ListAxis.end(); ++c_ItAxis)
            {
               if ((*c_ItAxis) == this->mpc_AxisValue)
               {
                  this->mc_DataPoolElementsDataSeries[u32_CounterLine]->detachAxis(this->mpc_AxisValue);
               }
            }

            // Adapt the drawn line of the data series
            c_Pen = this->mc_DataPoolElementsDataSeries[u32_CounterLine]->pen();
            c_Pen.setWidth(msn_WidthLineDefault);
            this->mc_DataPoolElementsDataSeries[u32_CounterLine]->setPen(c_Pen);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds the newest values of Datapool elements to its associated data series of the chart

   \param[in]     orc_DataPoolElementId   Datapool element identification
   \param[in]     orc_Values              List with all read elements
   \param[in]     orc_Timestamps          List with all timestamps for each element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::AddDataSerieContent(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                               const QVector<float64> & orc_Values,
                                               const QVector<uint32> & orc_Timestamps)
{
   // Find the correct data series
   if (orc_Values.size() > 0)
   {
      uint32 u32_ConfigCounter;

      for (u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_Data.c_DataPoolElementsConfig.size();
           ++u32_ConfigCounter)
      {
         const C_PuiSvDbNodeDataPoolListElementId & rc_CurDataPoolElementId =
            this->mc_Data.c_DataPoolElementsConfig[u32_ConfigCounter].c_ElementId;
         if ((rc_CurDataPoolElementId.CheckSameDataElement(orc_DataPoolElementId)) &&
             (u32_ConfigCounter < this->mc_DataPoolElementsDataSeries.size()))
         {
            QLineSeries * const pc_LineSerie = this->mc_DataPoolElementsDataSeries[u32_ConfigCounter];

            if (pc_LineSerie != NULL)
            {
               sintn sn_ValueCounter;
               float64 f64_Value = 0.0;
               C_OSCNodeDataPoolContent c_Tmp = this->mc_DataPoolElementContentMin[u32_ConfigCounter];
               QString c_Value;
               const uint32 u32_Index = rc_CurDataPoolElementId.GetArrayElementIndexOrZero();

               tgl_assert(orc_Values.size() == orc_Timestamps.size());

               for (sn_ValueCounter = 0U; sn_ValueCounter < orc_Values.size(); ++sn_ValueCounter)
               {
                  // Start time must be zero, adapt the read timestamp
                  const uint32 u32_TimeStamp = orc_Timestamps[sn_ValueCounter];
                  float64 f64_Timestamp;

                  if (u32_TimeStamp > this->mu32_TimeStampOfStart)
                  {
                     f64_Timestamp = static_cast<float64>(u32_TimeStamp);
                     f64_Timestamp -= static_cast<float64>(this->mu32_TimeStampOfStart);
                  }
                  else
                  {
                     f64_Timestamp = 0.0;
                  }

                  f64_Value = orc_Values[sn_ValueCounter];

                  f64_Value = C_OSCUtils::h_GetValueScaled(
                     f64_Value,
                     this->mc_Data.c_DataPoolElementsConfig[u32_ConfigCounter].c_ElementScaling.f64_Factor,
                     this->mc_Data.c_DataPoolElementsConfig[u32_ConfigCounter].c_ElementScaling.f64_Offset);

                  // Adapt the automatic scaling only for visible elements
                  if (pc_LineSerie->isVisible() == true)
                  {
                     // Add a little free space to the values
                     const float64 f64_FacHigh = 1.1;
                     const float64 f64_FacLow = 0.9;
                     const float64 f64_ValueFactorHigh = f64_Value * f64_FacHigh;
                     const float64 f64_ValueFactorLow = f64_Value * f64_FacLow;

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

                  pc_LineSerie->append(f64_Timestamp, f64_Value);
               }

               // Show the last value in the selector widget
               C_OSCNodeDataPoolContentUtil::h_SetValueInContent(f64_Value, c_Tmp, u32_Index);
               // No need of scaling here, it is already scaled
               C_SdNdeDpContentUtil::h_GetValueAsScaledString(c_Tmp, 1.0, 0.0, c_Value, u32_Index);
               this->mpc_Ui->pc_ChartSelectorWidget->UpdateDataSerieValue(u32_ConfigCounter, c_Value);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a scaling configuration for a specific datapool element configuration

   \param[in]     ou32_DataPoolElementConfigIndex         Datapool element configuration index
   \param[in]     orc_DisplayName                         Datapool element display name
   \param[in]     orc_ElementScaling                      Datapool element scaling configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::SetScaling(const uint32 ou32_DataPoolElementConfigIndex, const QString & orc_DisplayName,
                                      const C_PuiSvDbDataElementScaling & orc_ElementScaling)
{
   if (ou32_DataPoolElementConfigIndex < this->mc_Data.c_DataPoolElementsConfig.size())
   {
      this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_DisplayName = orc_DisplayName;
      this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementScaling = orc_ElementScaling;
      this->mpc_Ui->pc_ChartSelectorWidget->SetDataElementUnit(ou32_DataPoolElementConfigIndex, orc_DisplayName,
                                                               orc_ElementScaling.c_Unit);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the count of registered data series

   \return
   Count of registered data series
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvDaItChartWidget::GetCountDataSeries(void) const
{
   return this->mc_DataPoolElementsDataSeries.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts and updates the time axis if necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::UpdateTimeAxis(void)
{
   if (this->mq_DrawingActive == true)
   {
      const uint32 u32_CurrentTimeStamp = stw_tgl::TGL_GetTickCount() - this->mu32_TimeStampOfStart;
      const uint32 u32_NextTimeStamp = u32_CurrentTimeStamp + static_cast<uint32>(msn_TIMER_GUI_REFRESH);

      if (this->mf64_MaxTime < (static_cast<float64>(u32_NextTimeStamp)))
      {
         const float64 f64_Diff = static_cast<float64>(u32_NextTimeStamp) - this->mf64_MaxTime;

         // Save the new maximum
         this->mf64_MaxTime = static_cast<float64>(u32_NextTimeStamp);

         // Do not use the new maximum for the axis. In case of zooming, we need an dynamic adaption
         this->mpc_AxisTime->setMax(this->mpc_AxisTime->max() + f64_Diff);

         // Adapt minimum
         this->mpc_AxisTime->setMin(this->mpc_AxisTime->min() + f64_Diff);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts and updates the value axis if necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::UpdateValueAxis(void)
{
   if (this->mq_DrawingActive == true)
   {
      if (this->mq_ValueZoomed == false)
      {
         // Auto scaling active, use the detected range values
         if (this->mpc_AxisValue->max() < this->mf64_MaxValue)
         {
            this->mpc_AxisValue->setMax(this->mf64_MaxValue);
         }
         if (this->mpc_AxisValueInvisible->max() < this->mf64_MaxValue)
         {
            this->mpc_AxisValueInvisible->setMax(this->mf64_MaxValue);
         }
         if (this->mpc_AxisValue->min() > this->mf64_MinValue)
         {
            this->mpc_AxisValue->setMin(this->mf64_MinValue);
         }
         if (this->mpc_AxisValueInvisible->min() > this->mf64_MinValue)
         {
            this->mpc_AxisValueInvisible->setMin(this->mf64_MinValue);
         }
      }
      else
      {
         // The user had scrolled or zoomed
         const float64 f64_Max = this->mf64_MaxValueZoomed + this->mf64_ScrollOffsetY;
         const float64 f64_Min = this->mf64_MinValueZoomed + this->mf64_ScrollOffsetY;

         this->mpc_AxisValue->setRange(f64_Min, f64_Max);
         this->mpc_AxisValueInvisible->setRange(f64_Min, f64_Max);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Error update for data element

   \param[in] ou32_DataElementIndex           Index of shown Datapool element in widget
   \param[in] orc_ErrorText                   Error description
   \param[in] oq_IsTransmissionError          Flag if transmission error occurred
   \param[in] oq_ErrorActive                  Flag if error is active or should be cleared
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::UpdateError(const uint32 ou32_DataElementIndex, const QString & orc_ErrorText,
                                       const bool oq_IsTransmissionError, const bool oq_ErrorActive) const
{
   std::map<stw_types::uint32, stw_types::uint32>::const_iterator c_It =
      this->mc_ElementHandlerRegIndexToDataElementIndex.find(ou32_DataElementIndex);

   if (c_It != this->mc_ElementHandlerRegIndexToDataElementIndex.end())
   {
      this->mpc_Ui->pc_ChartSelectorWidget->UpdateError(c_It->second, orc_ErrorText, oq_IsTransmissionError,
                                                        oq_ErrorActive);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparence value configured by the actual timeout state

   \param[in] ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[in] osn_Value                       Value for transparence (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::UpdateTransparence(const uint32 ou32_DataElementIndex, const sintn osn_Value) const
{
   std::map<stw_types::uint32, stw_types::uint32>::const_iterator c_It =
      this->mc_ElementHandlerRegIndexToDataElementIndex.find(ou32_DataElementIndex);

   if (c_It != this->mc_ElementHandlerRegIndexToDataElementIndex.end())
   {
      this->mpc_Ui->pc_ChartSelectorWidget->UpdateTransparence(c_It->second, osn_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::paintEvent(QPaintEvent * const opc_Event)
{
   if (this->mq_DrawingActive)
   {
      stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);
      QWidget::paintEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a specific data serie

   \param[in]     orc_DataPoolElementId               Datapool element identification
   \param[in]     oq_Warning                          Flag if a warning for this data element was detected
   \param[in]     oq_Invalid                          Flag if data elment is invalid and invalid placeholder info
                                                      shall be used
   \param[in]     orc_InvalidPlaceholderName          Placeholder name of data element in case of invalid data element
   \param[in]     oe_InvalidPlaceholderDataPoolType   Placeholder datapool type of data element in case of
                                                      invalid data element
   \param[in]     orc_ToolTipErrorTextHeading         Heading of tool tip in case of a warning
   \param[in]     orc_ToolTipErrorText                Text of tool tip in case of a warning
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_AddDataSerie(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                                          const bool oq_Warning, const bool oq_Invalid,
                                          const QString & orc_InvalidPlaceholderName,
                                          const C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType,
                                          const QString & orc_ToolTipErrorTextHeading,
                                          const QString & orc_ToolTipErrorText)
{
   // Color selection
   QColor c_Color = this->m_GetColor();
   QLineSeries * const pc_Serie = new QLineSeries();
   QPen c_Pen;

   if (oq_Invalid == false)
   {
      this->mc_ElementHandlerRegIndexToDataElementIndex.insert(
         std::pair<uint32, uint32>(
            static_cast<uint32>(this->mc_ElementHandlerRegIndexToDataElementIndex.size()),
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
         oq_Warning, oq_Invalid, oe_InvalidPlaceholderDataPoolType, orc_ToolTipErrorTextHeading, orc_ToolTipErrorText);
   }

   c_Pen.setColor(c_Color);
   c_Pen.setWidth(msn_WidthLineDefault);
   pc_Serie->setPen(c_Pen);
   this->mpc_Chart->addSeries(pc_Serie);
   pc_Serie->attachAxis(this->mpc_AxisTime);
   // Each data serie needs minimum one y axis. Use the invisible one as default
   pc_Serie->attachAxis(this->mpc_AxisValueInvisible);
   pc_Serie->setVisible(this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex]);
   this->mc_DataPoolElementsDataSeries.push_back(pc_Serie);

   // Set the axis for the new data serie
   this->SelectDataSeriesAxis(ou32_DataPoolElementConfigIndex);
   //lint -e{429}  no memory leak because of the parent of pc_Series and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_ResetChart(void)
{
   uint32 u32_CounterItem;

   for (u32_CounterItem = 0;
        u32_CounterItem < this->mc_DataPoolElementsDataSeries.size();
        ++u32_CounterItem)
   {
      this->mc_DataPoolElementsDataSeries[u32_CounterItem]->clear();
   }

   // Reset the range
   this->mf64_MaxValue = 1.0;
   this->mf64_MinValue = 0.0;
   this->mf64_MaxTime = this->mf64_DefaultTimeSlot;

   this->mpc_AxisValue->setMax(this->mf64_MaxValue);
   this->mpc_AxisValue->setMin(this->mf64_MinValue);
   this->mpc_AxisValueInvisible->setMax(this->mf64_MaxValue);
   this->mpc_AxisValueInvisible->setMin(this->mf64_MinValue);
   this->mpc_AxisTime->setMax(this->mf64_MaxTime);
   this->mpc_AxisTime->setMin(0.0);
}

//----------------------------------------------------------------------------------------------------------------------
QColor C_SyvDaItChartWidget::m_GetColor(void)
{
   uint32 u32_Counter;
   QColor c_Color;

   // Search the next free Color
   for (u32_Counter = 0U; u32_Counter < this->mu32_MaximumDataElements; ++u32_Counter)
   {
      if (this->mc_DataColorsUsed[u32_Counter] == false)
      {
         std::vector<bool>::reference c_Value = this->mc_DataColorsUsed[u32_Counter];
         c_Value = true;
         c_Color = C_SyvDaItChartWidget::mhac_DataColors[u32_Counter];

         // Save the association between datapool element and the color
         this->mc_DataPoolElementsDataColorIndexes.push_back(u32_Counter);
         break;
      }
   }

   return c_Color;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_DataItemToggled(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                                             const bool oq_Checked)
{
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataSeries.size())
   {
      this->mc_DataPoolElementsDataSeries[ou32_DataPoolElementConfigIndex]->setVisible(oq_Checked);
   }

   if (ou32_DataPoolElementConfigIndex < this->mc_Data.c_DataPoolElementsActive.size())
   {
      std::vector<bool>::reference c_Value = this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex];
      c_Value = oq_Checked;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_DataItemSelected(const uint32 ou32_DataPoolElementConfigIndex)
{
   this->SelectDataSeriesAxis(ou32_DataPoolElementConfigIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the changed zoom mode

   \param[in]       oe_SettingZoomMode     Current used zoom mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_SettingZoomModeChanged(const C_PuiSvDbChart::E_SettingZoomMode oe_SettingZoomMode)
{
   switch (oe_SettingZoomMode)
   {
   case C_PuiSvDbChart::eSETTING_ZM_XY:
      this->mpc_ChartView->SetZoomMode(C_OgeChaViewBase::eSETTING_ZM_XY);
      break;
   case C_PuiSvDbChart::eSETTING_ZM_X:
      this->mpc_ChartView->SetZoomMode(C_OgeChaViewBase::eSETTING_ZM_X);
      break;
   case C_PuiSvDbChart::eSETTING_ZM_Y:
      this->mpc_ChartView->SetZoomMode(C_OgeChaViewBase::eSETTING_ZM_Y);
      break;
   default:
      this->mpc_ChartView->SetZoomMode(C_OgeChaViewBase::eSETTING_ZM_XY);
      break;
   }

   this->mc_Data.e_SettingZoomMode = oe_SettingZoomMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adaption of the Y axis values for the new zoom factor

   \param[in]       of64_Factor     Factor for zooming the Y axis
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_YZoomed(const float64 of64_Factor)
{
   float64 f64_DeltaRangeZoom;
   float64 f64_DeltaZoom;

   if (this->mq_ValueZoomed == false)
   {
      this->mq_ValueZoomed = true;

      // Save the current values as reference values. Further zooming shall be calculated based on this values
      // and not on further changed values
      this->mf64_ReferenceMaxValueZoomed = this->mf64_MaxValue;
      this->mf64_ReferenceMinValueZoomed = this->mf64_MinValue;
      this->mf64_ReferenceRangeZoomed = this->mf64_MaxValue - this->mf64_MinValue;
   }

   this->mf64_ZoomFactor *= of64_Factor;

   f64_DeltaRangeZoom = this->mf64_ReferenceRangeZoomed * this->mf64_ZoomFactor;
   f64_DeltaZoom = (this->mf64_ReferenceRangeZoomed - f64_DeltaRangeZoom) / 2.0;

   this->mf64_MaxValueZoomed = this->mf64_ReferenceMaxValueZoomed - f64_DeltaZoom;
   this->mf64_MinValueZoomed = this->mf64_ReferenceMinValueZoomed + f64_DeltaZoom;

   this->UpdateValueAxis();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scrolls the view of the chart

   \param[in]       of64_X     Scrolling distance for X axis
   \param[in]       of64_Y     Scrolling distance for Y axis
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_Scrolled(const float64 of64_X, const float64 of64_Y)
{
   if (this->mq_ValueZoomed == false)
   {
      // Custom scrolling needs the activation of the custom zooming with its reference values and
      // deactivating of the auto scaling
      this->m_YZoomed(1.0);
   }

   this->mf64_ScrollOffsetX += of64_X;
   this->mf64_ScrollOffsetY += of64_Y;

   this->UpdateValueAxis();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Zoom was reseted
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_ZoomReseted(void)
{
   this->mq_ValueZoomed = false;
   this->mf64_ZoomFactor = 1.0;
   this->mf64_ScrollOffsetX = 0.0;
   this->mf64_ScrollOffsetY = 0.0;
}
