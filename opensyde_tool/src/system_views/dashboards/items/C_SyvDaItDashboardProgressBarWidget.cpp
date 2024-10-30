//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard progress bar widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "constants.hpp"

#include <cmath>
#include <QPainter>
#include "stwtypes.hpp"
#include "C_SyvDaItUtil.hpp"
#include "C_GiBiLineBounding.hpp"
#include "C_GiBiConnectableItem.hpp"
#include "C_SyvDaItDashboardProgressBarWidget.hpp"
#include "C_GiProgressBarUtil.hpp"
#include "ui_C_SyvDaItDashboardProgressBarWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
C_SyvDaItDashboardProgressBarWidget::C_SyvDaItDashboardProgressBarWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItDashboardProgressBarWidget),
   me_Type(C_PuiSvDbProgressBar::eTYPE_1),
   me_Alignment(C_PuiSvDbProgressBar::eTOP),
   mc_MinValue("0"),
   mc_MaxValue("100"),
   me_Style(C_PuiSvDbWidgetBase::eOPENSYDE),
   mq_ShowMinMax(true),
   mq_DarkMode(false),
   ms32_Transparency(255)
{
   mpc_Ui->setupUi(this);

   this->SetDisplayStyle(C_PuiSvDbWidgetBase::eOPENSYDE, C_PuiSvDbProgressBar::eTYPE_1, C_PuiSvDbProgressBar::eTOP,
                         false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItDashboardProgressBarWidget::~C_SyvDaItDashboardProgressBarWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display style (horizontal or vertical progress bar)

   In addition it set the minimum bar size of the specific type, calculates the margin, and does an update (go to paint
   event)

   \param[in]       oe_Style     Defines the style
   \param[out]      oe_Type      Defines the type
   \param[in]   oe_Alignment     Defines the alignment type
   \param[in]   oq_DarkMode      Defines the displayed mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style,
                                                          const C_PuiSvDbProgressBar::E_Type oe_Type,
                                                          const C_PuiSvDbProgressBar::E_Alignment oe_Alignment,
                                                          const bool oq_DarkMode)
{
   this->me_Style = oe_Style;
   this->me_Type = oe_Type;
   this->me_Alignment = oe_Alignment;
   this->mq_DarkMode = oq_DarkMode;

   this->mpc_Ui->pc_ProgressBar->SetDisplayStyle(oe_Style, oq_DarkMode);

   switch (this->me_Type)
   {
   case C_PuiSvDbProgressBar::eTYPE_1:
      this->mpc_Ui->pc_ProgressBar->setOrientation(Qt::Horizontal);
      break;
   case C_PuiSvDbProgressBar::eTYPE_2:
      this->mpc_Ui->pc_ProgressBar->setOrientation(Qt::Vertical);
      break;
   case C_PuiSvDbProgressBar::eTYPE_3:
      this->mpc_Ui->pc_ProgressBar->setOrientation(Qt::Vertical);

      break;
   default:
      break;
   }

   m_SetMinimumProgressbarSize();

   m_SetBarMargins();
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size

    Therefore, it calculates the margin and does an update (go to paint event)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::AdjustFontToSize(void)
{
   m_SetBarMargins();
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   After setting the value it calls for recalculating the margin and updates the scene (paintEvent)

   \param[in]  os32_Value   New value
   \param[in]  orc_Value    New value string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetValue(const int32_t os32_Value, const QString & orc_Value)
{
   const int32_t s32_Min = this->mpc_Ui->pc_ProgressBar->minimum();

   if (os32_Value < s32_Min)
   // Necessary because os32_Value = -7905 when it should be 0
   // setValue() doesn't take negative values
   {
      // Setting the shown value
      const uint32_t u32_MINIMUM_VALUE = 0;
      this->mpc_Ui->pc_ProgressBar->setValue(u32_MINIMUM_VALUE);
   }
   else
   {
      // Setting the value
      this->mpc_Ui->pc_ProgressBar->setValue(os32_Value);
   }

   this->mc_Value = orc_Value;

   m_SetBarMargins();
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set minimum value

   After setting the minimum value it calls for recalculating the margin

   \param[in]  os32_Value   New min value
   \param[in]  orc_Value    New min string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetMin(const int32_t os32_Value, const QString & orc_Value)
{
   this->mc_MinValue = orc_Value;
   this->mpc_Ui->pc_ProgressBar->setMinimum(os32_Value);
   m_SetBarMargins();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set maximum value

   After setting the maximum value it calls for recalculating the margin

   \param[in]  os32_Value   New max value
   \param[in]  orc_Value    New max string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetMax(const int32_t os32_Value, const QString & orc_Value)
{
   this->mc_MaxValue = orc_Value;
   this->mpc_Ui->pc_ProgressBar->setMaximum(os32_Value);
   m_SetBarMargins();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flag for showing minimum and maximum value

   \param[in]  oq_Value    Flag for showing minimum and maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetShowMinMax(const bool oq_Value)
{
   this->mq_ShowMinMax = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set unit characters

   \param[in]  orc_Value   New unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetUnit(const QString & orc_Value)
{
   mc_Unit = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets of the color transparency value configured by the actual timeout state

   \param[in]  os32_Value   Value for transparency (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetTransparency(const int32_t os32_Value)
{
   this->ms32_Transparency = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get progress bar type

   The type can be:
   - C_PuiSvDbProgressBar::eTYPE_1: progress bar is orientated horizontally
   - C_PuiSvDbProgressBar::eTYPE_2: progress bar is orientated vertically
   - C_PuiSvDbProgressBar::eTYPE_3: progress bar is orientated vertically

   \return
   progress bar type (eTYPE_1, eTYPE_2, eTYPE_3)
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type C_SyvDaItDashboardProgressBarWidget::GetType() const
{
   return this->me_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Handle resize specific update

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   AdjustFontToSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here:
      - Font size is set
      - Minimum value rectangle is set
      - Maximum value rectangle is set
      - Value rectangle is set
      - Arrow is set for progress bar type 1 and type 2

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   // Variables
   QPainter c_Painter(this);
   QFont c_TextFont;
   QPen c_Pen;

   c_TextFont.setFamily("Segoe UI");
   m_SetStyle(c_Painter, c_Pen);

   if (this->me_Type == stw::opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_1)
   {
      QRect c_ValueRect;
      const float32_t f32_FontSize = C_GiProgressBarUtil::h_GetType1FontSizeValue(this->rect(),
                                                                                  this->mf32_MinimumFontSize);
      c_TextFont.setPixelSize(static_cast<int32_t>(f32_FontSize));
      c_Painter.setFont(c_TextFont);
      c_Painter.setPen(c_Pen);

      //Draw minimum, maximum, and current value
      C_GiProgressBarUtil::h_SetType1MinimumValue(this->mpc_Ui->pc_ProgressBar,
                                                  this->mq_ShowMinMax,
                                                  this->mc_MinValue,
                                                  this->mc_Unit,
                                                  this->me_Alignment,
                                                  c_Painter);

      C_GiProgressBarUtil::h_SetType1MaximumValue(this->mpc_Ui->pc_ProgressBar,
                                                  this->mq_ShowMinMax,
                                                  this->mc_MaxValue,
                                                  this->mc_Unit,
                                                  this->me_Alignment,
                                                  c_Painter);
      c_ValueRect = C_GiProgressBarUtil::h_SetType1Value(this->mpc_Ui->pc_ProgressBar,
                                                         this->mc_MaxValue,
                                                         this->mc_MinValue,
                                                         this->mc_Value,
                                                         this->mc_Unit,
                                                         this->me_Alignment,
                                                         this->mc_ArrowSpacerString,
                                                         c_Painter);

      //Draw arrow
      c_Painter.setPen(c_Pen);
      C_GiProgressBarUtil::h_SetType1Arrow(this->mpc_Ui->pc_ProgressBar,
                                           c_ValueRect,
                                           this->me_Alignment,
                                           this->mc_ArrowSpacerString,
                                           c_Painter);
   }
   else if (this->me_Type == stw::opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_2)
   {
      const QRect c_Rect = this->rect();
      const float32_t f32_MinMaxFontSize = C_GiProgressBarUtil::h_GetType2FontSizeValue(
         c_Rect, this->mf32_MinimumFontSize);

      QRect c_ValueRect;

      c_TextFont.setPixelSize(static_cast<int32_t>(f32_MinMaxFontSize));
      c_Painter.setFont(c_TextFont);
      c_Painter.setPen(c_Pen);

      C_GiProgressBarUtil::h_SetType23MinimumMaximumValue(c_Rect,
                                                          this->mpc_Ui->pc_ProgressBar,
                                                          this->mq_ShowMinMax,
                                                          this->mc_MinValue,
                                                          this->mc_MaxValue,
                                                          this->mc_Unit,
                                                          this->me_Alignment,
                                                          c_Painter);

      c_ValueRect = C_GiProgressBarUtil::h_SetType2Value(c_Rect,
                                                         this->mpc_Ui->pc_ProgressBar,
                                                         this->mc_MaxValue,
                                                         this->mc_MinValue,
                                                         this->mc_Value,
                                                         this->mc_Unit,
                                                         this->mc_ArrowSpacerString,
                                                         this->me_Alignment,
                                                         this->mpc_Ui->horizontalLayout->contentsMargins().bottom(),
                                                         c_Painter);

      //Draw arrow
      c_Painter.setPen(c_Pen);
      C_GiProgressBarUtil::h_SetType2Arrow(this->mpc_Ui->pc_ProgressBar,
                                           c_ValueRect,
                                           this->me_Alignment,
                                           this->mc_ArrowSpacerString,
                                           c_Painter);
   }
   else if (this->me_Type == stw::opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_3)
   {
      const QRect c_Rect = this->rect();
      const float32_t f32_FontValueSize = C_GiProgressBarUtil::h_GetType3FontSizeValue(c_Rect,
                                                                                       this->mf32_MinimumFontSize);

      const float32_t f32_FontMinMaxSize = C_GiProgressBarUtil::h_GetType3FontSizeMaxMin(
         c_Rect, this->mf32_MinimumFontSize);

      c_TextFont.setPixelSize(static_cast<int32_t>(f32_FontMinMaxSize));
      c_Painter.setFont(c_TextFont);
      c_Painter.setPen(c_Pen);

      //Draw minimum, maximum and current value
      C_GiProgressBarUtil::h_SetType23MinimumMaximumValue(c_Rect,
                                                          this->mpc_Ui->pc_ProgressBar,
                                                          this->mq_ShowMinMax,
                                                          this->mc_MinValue,
                                                          this->mc_MaxValue,
                                                          this->mc_Unit,
                                                          this->me_Alignment,
                                                          c_Painter);

      c_TextFont.setPixelSize(static_cast<int32_t>(f32_FontValueSize));

      C_GiProgressBarUtil::h_SetType3Value(this->mpc_Ui->pc_ProgressBar,
                                           this->mc_Value,
                                           this->mc_Unit,
                                           this->me_Alignment,
                                           f32_FontValueSize,
                                           c_Painter);
   }
   else
   {
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set label color, brush, and alpha for the progress bar elements for the specific displayed mode
            (dark/bright)

   \param[in]       orc_Painter          Painter needed for set the necessary options
   \param[in]       orc_Pen              Pen needed to set the color of the labels
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::m_SetStyle(QPainter & orc_Painter, QPen & orc_Pen) const
{
   if (mq_DarkMode)
   {
      this->m_SetDarkModeStyle(orc_Painter, orc_Pen);
   }
   else
   {
      this->m_SetBrightModeStyle(orc_Painter, orc_Pen);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the margin

   The margin is the space at the top, bottom, right, and left side of the progress bar.
   The margin of every progress bar type is a bit different, so the type will be checked before setting the margin for
   the type.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::m_SetBarMargins(void)
{
   if (this->me_Type == stw::opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_1)
   {
      C_GiProgressBarUtil::h_DefineType1Margins(this->rect(),
                                                this->mpc_Ui->horizontalLayout,
                                                this->mq_ShowMinMax,
                                                this->mc_MaxValue,
                                                this->mc_MinValue,
                                                this->mc_Unit,
                                                this->me_Alignment,
                                                this->mf32_MinimumFontSize,
                                                this->mf32_ArrowOffset);
   }
   else if (this->me_Type == stw::opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_2)
   {
      C_GiProgressBarUtil::h_DefineType2Margins(this->rect(),
                                                this->mpc_Ui->horizontalLayout,
                                                this->mq_ShowMinMax,
                                                this->me_Alignment,
                                                this->mf32_MinimumFontSize);
   }
   else if (this->me_Type == stw::opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_3)
   {
      C_GiProgressBarUtil::h_DefineType3Margins(this->rect(),
                                                this->mpc_Ui->horizontalLayout,
                                                this->mq_ShowMinMax,
                                                this->me_Alignment,
                                                this->mf32_MinimumFontSize);
   }
   else
   {
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set Dark mode style

   \param[in]       orc_Painter          Painter needed for set the necessary options
   \param[in]       orc_Pen              Pen to define the color for the labels
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::m_SetDarkModeStyle(QPainter & orc_Painter, QPen & orc_Pen) const
{
   QColor c_HelpingColorVariable;

   c_HelpingColorVariable = mc_STYLE_GUIDE_COLOR_0;
   c_HelpingColorVariable.setAlpha(ms32_Transparency); // Color for Value has to get more transparent some times

   //set brush color and transparency to have a color inside arrow
   orc_Painter.setBrush(static_cast<QBrush>(c_HelpingColorVariable));

   //set pen color
   orc_Pen.setColor(c_HelpingColorVariable);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set Dark mode style

   \param[in]       orc_Painter          Painter needed for set the necessary options
   \param[in]       orc_Pen              Pen to define the color for the labels
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::m_SetBrightModeStyle(QPainter & orc_Painter, QPen & orc_Pen) const
{
   QColor c_HelpingColorVariable = (me_Style == stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::eOPENSYDE_2) ?
                                   (mc_STYLE_GUIDE_COLOR_6) :
                                   (mc_STYLE_GUIDE_COLOR_34);

   c_HelpingColorVariable.setAlpha(ms32_Transparency);

   //set brush color and transparency to have a color inside arrow
   orc_Painter.setBrush(static_cast<QBrush>(c_HelpingColorVariable));

   //Set pen color
   orc_Pen.setColor(c_HelpingColorVariable);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set minimum progress bar size

   Dependent on the orientation of the progress bar that has to be set before,
   the progress bar minimum size is hard set to either
   Vertical bar: (10, 25)
   Horizontal bar: (25, 10)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::m_SetMinimumProgressbarSize(void)
{
   // Constants
   const uint32_t u32_MAXIMUM_OF_MINIMUM_SIZE = 35;
   const uint32_t u32_MINIMUM_OF_MINIIMUM_SZE = 20;

   const uint32_t u32_MinimumWidth = (this->mpc_Ui->pc_ProgressBar->orientation() == Qt::Vertical) ?
                                     u32_MINIMUM_OF_MINIIMUM_SZE :
                                     u32_MAXIMUM_OF_MINIMUM_SIZE;
   const uint32_t u32_MinimumHeight =
      (this->mpc_Ui->pc_ProgressBar->orientation() == Qt::Vertical) ?
      u32_MAXIMUM_OF_MINIMUM_SIZE :
      u32_MINIMUM_OF_MINIIMUM_SZE;

   this->mpc_Ui->pc_ProgressBar->setMinimumSize(u32_MinimumWidth, u32_MinimumHeight);
}
