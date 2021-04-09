//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dashboard pie chart item (implementation)

   Dashboard pie chart item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include <QPainter>
#include "constants.h"
#include "C_OgeWiDashboardPieChart.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QColor C_OgeWiDashboardPieChart::mhc_Transparent = QColor(Qt::transparent);
const float32 C_OgeWiDashboardPieChart::mhf32_Pi = static_cast<float32>(std::acos(-1));

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiDashboardPieChart::C_OgeWiDashboardPieChart(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mq_ValueCircle(false),
   ms32_Value(100),
   ms32_Min(0),
   ms32_Max(2000000),
   mc_Unit("%"),
   msn_ValueWidth(5),
   msn_BorderWidth(5),
   mq_ShowUnit(true),
   mq_ShowValue(true),
   msn_Transparence(msn_TRANSPARENCY_END),
   mq_TransparentBackground(false),
   me_Style(C_PuiSvDbWidgetBase::eOPENSYDE),
   mq_DarkMode(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style       New style type
   \param[in] oq_DarkMode    Flag if dark mode is active
   \param[in] oq_ShowUnit    Show unit flag
   \param[in] oq_ShowValue   Show value flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode,
                                               const bool oq_ShowUnit, const bool oq_ShowValue)
{
   this->me_Style = oe_Style;
   this->mq_DarkMode = oq_DarkMode;

   this->mq_ShowUnit = oq_ShowUnit;
   this->mq_ShowValue = oq_ShowValue;
   this->mq_TransparentBackground = false;

   ReInitSize();
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reinitialize size based parameters
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::ReInitSize(void)
{
   sintn sn_PixelSize;
   QFont c_Font;
   float32 f32_Temp;

   //Update font
   c_Font = this->mc_Font;
   sn_PixelSize =
      static_cast<sintn>(std::max((static_cast<float32>(this->rect().height()) / 3.0F) * (11.0F / 17.0F), 1.0F));

   c_Font.setFamily("Segoe UI"); //
   c_Font.setPointSize(sn_PixelSize);
   this->mc_Font = c_Font;
   //Value width
   f32_Temp = (static_cast<float32>(this->rect().height()) / 20.0F) * (10.0F / 13.0F);
   this->msn_ValueWidth = static_cast<sintn>(f32_Temp);
   //Boder width
   f32_Temp = (static_cast<float32>(this->rect().height()) / 20.0F) * (10.0F / 13.0F);
   this->msn_BorderWidth = static_cast<sintn>(f32_Temp);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update pie chart value

   \param[in] orc_Value            Displayed value
   \param[in] os32_Progress2000000 Value in percent (only 0-2000000 supported)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::SetValue(const QString & orc_Value, const sint32 os32_Progress2000000)
{
   this->mc_Value = orc_Value;
   this->ms32_Value = os32_Progress2000000;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update unit

   \param[in] orc_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::SetUnit(const QString & orc_Value)
{
   this->mc_Unit = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets of the color transparence value configured by the actual timeout state

   \param[in]     osn_Value                           Value for transparence (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::SetTransparence(const sintn osn_Value)
{
   this->msn_Transparence = osn_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Update font size

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);
   ReInitSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Draw custom widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::paintEvent(QPaintEvent * const opc_Event)
{
   //Progress of value
   const sint32 s32_Value = this->ms32_Value - this->ms32_Min;
   const sint32 s32_Range = this->ms32_Max - this->ms32_Min;
   const float32 f32_Progress = static_cast<float32>(s32_Value) / static_cast<float32>(s32_Range);
   QString c_DisplayString;
   QPainter c_Painter(this);
   QColor c_HelpingColor;

   // Variables for general Pie-----------------------------------------------
   stw_types::float32 f32_PieWidth = 8.0F; // Width of the pieCircle
   stw_types::float32 f32_RimWidth = 7.0F; // Width of the rim (to fill gaps: +1 later
   // on)
   stw_types::float32 f32_PieGap = (f32_PieWidth / 2.0F) +
                                   2.0F; // Addition factor for the spacing to
   // this->rect()
   stw_types::float32 f32_TotalPixels = 158.0F; // Size of the Pie, defined in the
   // template
   stw_types::float32 f32_DotSize = 8.0F; // Size of the Dot on the pieCircle
   QConicalGradient c_Gradient;           // For pieCircle with Gradation (STW_2)

   QRect c_InnerCircleRect; // Rectangele of the inner circuit
   QRect c_RimRect;         // rectangle of the rim
   QRect c_PieRect;         // rectangle of the Pies

   Q_UNUSED(opc_Event)
   c_Painter.setRenderHint(QPainter::Antialiasing, true);

   // Configure display string
   if ((this->mq_ShowUnit == true) &&
       (this->mq_ShowValue == true))
   {
      c_DisplayString = static_cast<QString>("%1%2").arg(this->mc_Value).arg(this->mc_Unit);
   }
   else if (this->mq_ShowValue == true)
   {
      c_DisplayString = this->mc_Value;
   }
   else if (this->mq_ShowUnit == true)
   {
      c_DisplayString = this->mc_Unit;
   }
   else
   {
      c_DisplayString = "";
   }

   switch (this->me_Style) // Different Dashboard-Styles
   {
   case C_PuiSvDbWidgetBase::eOPENSYDE: // Material (openSYDE)

      if (this->mq_DarkMode != 0) // openSYDE DARK
      {
         f32_PieWidth = 8.0F;
         f32_RimWidth = 8.0F;
         f32_PieGap = (f32_PieWidth / 2.0F) + 2.0F;
         f32_TotalPixels = 158.0F;

         // Sizing the three rectangles dependent on the set variables above------------------------
         m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth,
                          f32_TotalPixels, 2.0F);

         // Pie--------------------------------------------------------------------------------
         m_SetPie(c_Painter, static_cast<QBrush>(
                     mc_STYLE_GUIDE_COLOR_13), Qt::RoundCap, c_PieRect, f32_PieWidth, f32_TotalPixels, f32_Progress);

         // Rim--------------------------------------------------------------------------
         m_SetPieRim(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_2), c_RimRect, f32_RimWidth, f32_TotalPixels);

         // Inner circuit-------------------------------------------------------------------------
         if (mq_TransparentBackground == false) // Nicht transparent
         {
            m_SetInnerCircle(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_52), c_InnerCircleRect);
         }
         else // Transparent
         {
            m_SetInnerCircle(c_Painter, static_cast<QBrush>(QColor(Qt::transparent)), c_InnerCircleRect);
         }

         // Text-------------------------------------------------------------------------------
         c_HelpingColor = mc_STYLE_GUIDE_COLOR_0;
         c_HelpingColor.setAlpha(msn_Transparence);
         m_SetPieText(c_Painter, c_HelpingColor, c_InnerCircleRect, c_DisplayString);

         QWidget::paintEvent(opc_Event);
      }
      else // openSYDE BRIGHT
      {
         f32_PieWidth = 7.0F;
         f32_RimWidth = 6.0F;
         f32_PieGap = (f32_PieWidth / 2.0F) + 2.0F;
         f32_TotalPixels = 148.0F;

         m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth,
                          f32_TotalPixels);
         // Pie--------------------------------------------------------------------------------
         m_SetPie(c_Painter, static_cast<QBrush>(
                     mc_STYLE_GUIDE_COLOR_13), Qt::RoundCap, c_PieRect, f32_PieWidth, f32_TotalPixels, f32_Progress);

         // Rim--------------------------------------------------------------------------
         m_SetPieRim(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_38), c_RimRect, f32_RimWidth, f32_TotalPixels);

         // Inner circuit-------------------------------------------------------------------------
         if (mq_TransparentBackground == false)
         {
            m_SetInnerCircle(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_0), c_InnerCircleRect);
         }
         else
         {
            m_SetInnerCircle(c_Painter, static_cast<QBrush>(QColor(Qt::transparent)), c_InnerCircleRect);
         }

         // Text-------------------------------------------------------------------------------
         c_HelpingColor = mc_STYLE_GUIDE_COLOR_34;
         c_HelpingColor.setAlpha(msn_Transparence);
         m_SetPieText(c_Painter, c_HelpingColor, c_InnerCircleRect, c_DisplayString);
         //         m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_34, c_InnerCircleRect, c_DisplayString);

         QWidget::paintEvent(opc_Event);
      }
      break;

   case C_PuiSvDbWidgetBase::eFLAT: // Flat
      f32_PieWidth = 7.0F;
      f32_RimWidth = 5.0F;
      f32_PieGap = (f32_PieWidth / 2.0F) + 2.0F;
      f32_TotalPixels = 148.0F;

      m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth, f32_TotalPixels,
                       2.0F);
      // For smooth edges
      c_Painter.setRenderHint(QPainter::Antialiasing);

      if (this->mq_DarkMode != 0) // Flat DARK
      {
         // Pie--------------------------------------------------------------------------------
         m_SetPie(c_Painter, static_cast<QBrush>(
                     mc_STYLE_GUIDE_COLOR_26), Qt::FlatCap, c_PieRect, f32_PieWidth, f32_TotalPixels, f32_Progress);

         // Rim--------------------------------------------------------------------------
         m_SetPieRim(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_32), c_RimRect, f32_RimWidth, f32_TotalPixels);

         // Inner circuit-------------------------------------------------------------------------
         m_SetInnerCircle(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_32), c_InnerCircleRect);

         // Text-------------------------------------------------------------------------------
         c_HelpingColor = mc_STYLE_GUIDE_COLOR_0;
         c_HelpingColor.setAlpha(msn_Transparence);
         m_SetPieText(c_Painter, c_HelpingColor, c_InnerCircleRect, c_DisplayString);
         //         m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_0, c_InnerCircleRect, c_DisplayString);

         QWidget::paintEvent(opc_Event);
      }
      else // Flat BRIGHT
      {
         // Pie--------------------------------------------------------------------------------
         m_SetPie(c_Painter, static_cast<QBrush>(
                     mc_STYLE_GUIDE_COLOR_26), Qt::FlatCap, c_PieRect, f32_PieWidth, f32_TotalPixels, f32_Progress);

         // Rim--------------------------------------------------------------------------
         m_SetPieRim(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_10), c_RimRect, f32_RimWidth, f32_TotalPixels);

         // Inner circuit-------------------------------------------------------------------------
         m_SetInnerCircle(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_10), c_InnerCircleRect);

         // Text-------------------------------------------------------------------------------
         c_HelpingColor = mc_STYLE_GUIDE_COLOR_0;
         c_HelpingColor.setAlpha(msn_Transparence);
         m_SetPieText(c_Painter, c_HelpingColor, c_InnerCircleRect, c_DisplayString);
         //         m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_0, c_InnerCircleRect, c_DisplayString);

         QWidget::paintEvent(opc_Event);
      }
      break;

   case C_PuiSvDbWidgetBase::eSKEUOMORPH: // Skeuomorph
      f32_PieWidth = 4.0F;
      f32_RimWidth = 7.0F;
      f32_PieGap = (f32_PieWidth / 2.0F) + 2.0F;
      f32_TotalPixels = 142.0F;
      f32_DotSize = 8.0F;

      m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth, f32_TotalPixels,
                       2.0F);
      // For smooth edges
      c_Painter.setRenderHint(QPainter::Antialiasing);

      if (this->mq_DarkMode != 0) // Skeuomorph DARK
      {
         // Color Gradient of Pie
         QLinearGradient c_LinearGradient;
         QLinearGradient c_LinearGradient2;
         c_LinearGradient.setColorAt(0.0, mc_STYLE_GUIDE_COLOR_33); // Farbe: 33
         c_LinearGradient.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_34); // Farbe: 34
         c_LinearGradient.setStart(c_InnerCircleRect.topLeft());
         c_LinearGradient.setFinalStop(c_InnerCircleRect.bottomLeft());
         c_LinearGradient2.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_33); // Farbe: 33
         c_LinearGradient2.setColorAt(0.0, mc_STYLE_GUIDE_COLOR_34); // Farbe: 34
         c_LinearGradient2.setStart(c_RimRect.topLeft());
         c_LinearGradient2.setFinalStop(c_RimRect.bottomLeft());

         // Pie--------------------------------------------------------------------------------
         m_SetPie(c_Painter, static_cast<QBrush>(
                     mc_STYLE_GUIDE_COLOR_24), Qt::FlatCap, c_PieRect, f32_PieWidth, f32_TotalPixels, f32_Progress);

         // Optional Punkt auf Pie-------------------------------------------------------------
         m_SetPieDot(c_Painter, mc_STYLE_GUIDE_COLOR_24, c_PieRect, f32_DotSize, f32_TotalPixels, f32_Progress);

         // Rim--------------------------------------------------------------------------
         m_SetPieRim(c_Painter, c_LinearGradient2, c_RimRect, f32_RimWidth, f32_TotalPixels);

         // Inner circuit-------------------------------------------------------------------------
         m_SetInnerCircle(c_Painter, c_LinearGradient, c_InnerCircleRect);

         // Text-------------------------------------------------------------------------------
         c_HelpingColor = mc_STYLE_GUIDE_COLOR_0;
         c_HelpingColor.setAlpha(msn_Transparence);
         m_SetPieText(c_Painter, c_HelpingColor, c_InnerCircleRect, c_DisplayString);
         //         m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_0, c_InnerCircleRect, c_DisplayString);

         QWidget::paintEvent(opc_Event);
      }
      else // Skeuomorph BRIGHT
      {
         // Color Gradient of Pie
         QLinearGradient c_LinearGradient;
         QLinearGradient c_LinearGradient2;
         c_LinearGradient.setColorAt(0.0, mc_STYLE_GUIDE_COLOR_37); // Farbe: 37
         c_LinearGradient.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_11); // Farbe: 11
         c_LinearGradient.setStart(c_InnerCircleRect.topLeft());
         c_LinearGradient.setFinalStop(c_InnerCircleRect.bottomLeft());
         c_LinearGradient2.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_37); // Farbe: 37
         c_LinearGradient2.setColorAt(0.0, mc_STYLE_GUIDE_COLOR_11); // Farbe: 11
         c_LinearGradient2.setStart(c_RimRect.topLeft());
         c_LinearGradient2.setFinalStop(c_RimRect.bottomLeft());

         // Pie--------------------------------------------------------------------------------
         m_SetPie(c_Painter, static_cast<QBrush>(
                     mc_STYLE_GUIDE_COLOR_24), Qt::FlatCap, c_PieRect, f32_PieWidth, f32_TotalPixels, f32_Progress);

         // Optional Punkt auf Pie-------------------------------------------------------------
         m_SetPieDot(c_Painter, mc_STYLE_GUIDE_COLOR_24, c_PieRect, f32_DotSize, f32_TotalPixels, f32_Progress);

         // Rim--------------------------------------------------------------------------
         m_SetPieRim(c_Painter, c_LinearGradient2, c_RimRect, f32_RimWidth, f32_TotalPixels);

         // Inner circuit-------------------------------------------------------------------------
         m_SetInnerCircle(c_Painter, c_LinearGradient, c_InnerCircleRect);

         // Text-------------------------------------------------------------------------------
         c_HelpingColor = mc_STYLE_GUIDE_COLOR_34;
         c_HelpingColor.setAlpha(msn_Transparence);
         m_SetPieText(c_Painter, c_HelpingColor, c_InnerCircleRect, c_DisplayString);
         //         m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_34, c_InnerCircleRect, c_DisplayString);

         QWidget::paintEvent(opc_Event);
      }
      break;

   case C_PuiSvDbWidgetBase::eOPENSYDE_2: // Gradation (openSyde_2)
      if (this->mq_DarkMode != 0)         // Gradation DARK
      {
         f32_PieWidth = 7.0F;
         f32_RimWidth = 7.0F;
         f32_PieGap = (f32_PieWidth / 2.0F) + 2.0F;
         f32_TotalPixels = 150.0F;

         m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth,
                          f32_TotalPixels, 3.0F);

         // For smooth edges
         c_Painter.setRenderHint(QPainter::Antialiasing);

         // Pie--------------------------------------------------------------------------------
         c_Gradient.setCenter(c_InnerCircleRect.center());
         c_Gradient.setAngle(static_cast<qreal>(90));
         c_Gradient.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_33);                                        // Farbe: 33
         c_Gradient.setColorAt((1.0 - static_cast<float64>(f32_Progress)), mc_STYLE_GUIDE_COLOR_13); // Farbe: 13
         m_SetPie(c_Painter, c_Gradient, Qt::FlatCap, c_PieRect, f32_PieWidth, f32_TotalPixels, f32_Progress);

         // Rim--------------------------------------------------------------------------
         m_SetPieRim(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_32), c_RimRect, f32_RimWidth, f32_TotalPixels,
                     2.0F);

         // Inner circuit-------------------------------------------------------------------------
         if (mq_TransparentBackground == false)
         {
            m_SetInnerCircle(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_52), c_InnerCircleRect);
         }
         else
         {
            m_SetInnerCircle(c_Painter, static_cast<QBrush>(QColor(Qt::transparent)), c_InnerCircleRect);
         }

         // Text-------------------------------------------------------------------------------
         c_HelpingColor = mc_STYLE_GUIDE_COLOR_0;
         c_HelpingColor.setAlpha(msn_Transparence);
         m_SetPieText(c_Painter, c_HelpingColor, c_InnerCircleRect, c_DisplayString);
         //         m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_0, c_InnerCircleRect, c_DisplayString);

         QWidget::paintEvent(opc_Event);
      }
      else // Gradation BRIGHT
      {
         f32_PieWidth = 6.0F;
         f32_RimWidth = 6.0F;
         f32_PieGap = (f32_PieWidth / 2.0F) + 2.0F;
         f32_TotalPixels = 146.0F;

         m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth,
                          f32_TotalPixels, 2.0F);

         // For smooth edges
         c_Painter.setRenderHint(QPainter::Antialiasing);

         // Pie--------------------------------------------------------------------------------
         c_Gradient.setCenter(c_InnerCircleRect.center());
         c_Gradient.setAngle(static_cast<qreal>(90));
         c_Gradient.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_38);                                        // Farbe: 38
         c_Gradient.setColorAt((1.0 - static_cast<float64>(f32_Progress)), mc_STYLE_GUIDE_COLOR_13); // Farbe: 13
         m_SetPie(c_Painter, c_Gradient, Qt::FlatCap, c_PieRect, f32_PieWidth, f32_TotalPixels, f32_Progress);

         // Rim--------------------------------------------------------------------------
         m_SetPieRim(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_38), c_RimRect, f32_RimWidth, f32_TotalPixels);

         // Inner circuit-------------------------------------------------------------------------
         if (mq_TransparentBackground == false)
         {
            m_SetInnerCircle(c_Painter, static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_0), c_InnerCircleRect);
         }
         else
         {
            m_SetInnerCircle(c_Painter, static_cast<QBrush>(QColor(Qt::transparent)), c_InnerCircleRect);
         }

         // Text-------------------------------------------------------------------------------
         c_HelpingColor = mc_STYLE_GUIDE_COLOR_6;
         c_HelpingColor.setAlpha(msn_Transparence);
         m_SetPieText(c_Painter, c_HelpingColor, c_InnerCircleRect, c_DisplayString);
         //         m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_6, c_InnerCircleRect, c_DisplayString);

         QWidget::paintEvent(opc_Event);
      }
      break;

   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_OgeWiDashboardPieChart::m_SetPie(QPainter & orc_Painter, const QBrush oc_PieBrush,
                                        const Qt::PenCapStyle oe_CapStyle, const QRect & orc_PieCircleRect,
                                        const float32 of32_PieWidth, const float32 of32_TotalPixels,
                                        const float32 of32_Progress) const
{
   QPen c_PieArc;

   c_PieArc.setBrush(oc_PieBrush); // Color
   // Setting the with a little bigger to avoid gaps
   c_PieArc.setWidth(static_cast<sintn>(std::ceil((static_cast<float32>(orc_PieCircleRect.height())
                                                   *
                                                   (of32_PieWidth + 1.0F)) / of32_TotalPixels)));

   c_PieArc.setCapStyle(oe_CapStyle); //Qt::RoundCap causes  overlap & default not possible with Gradient
   orc_Painter.setPen(c_PieArc);
   const sintn sn_PieStartAngle = 90 * 16; // To start on top in the
   // middle
   const float32 f32_PieSpanAngle = (-1.0F * 360.0F * of32_Progress * 16.0F); // -1: clockwise, 360*16: full
   // turn

   // Paints the Arc in the rect form start with the length span
   orc_Painter.drawArc(orc_PieCircleRect, sn_PieStartAngle, static_cast<sintn>(f32_PieSpanAngle));
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::m_SetPieDot(QPainter & orc_Painter, const QColor & orc_PieDotColor,
                                           const QRect & orc_PieRect, const float32 of32_DotSize,
                                           const float32 of32_TotalPixels, const float32 of32_Progress) const
{
   float32 f32_DotPositionX; // Variable for X-Postition
   float32 f32_DotPositionY; // Variable for Y-Position
   float32 f32_DotSize;      // Size of the Dot (diameter)

   const float32 f32_DotPositionXCosInput = (of32_Progress * 2.0F * static_cast<float32>(M_PI)) -
                                            (static_cast<float32>(M_PI) / 2.0F);
   const float32 f32_DotPositionYSinInput = (of32_Progress * 2.0F * static_cast<float32>(M_PI)) +
                                            (static_cast<float32>(M_PI) / 2.0F);

   orc_Painter.setPen(Qt::NoPen);
   orc_Painter.setBrush(static_cast<QBrush>(orc_PieDotColor)); // Color of the Dot
   //f32_DotPositionX = static_cast<stw_types::sint16>(oc_PieRect.left()); // Initial value
   //f32_DotPositionY = static_cast<stw_types::sint16>(oc_PieRect.top());  // Initial value
   f32_DotSize = (static_cast<float32>(orc_PieRect.width()) * of32_DotSize) /
                 of32_TotalPixels; // Size dependent on the this->rect size

   // Kugel sitzt auf Außenrand des Keises
   f32_DotPositionX =
      (((static_cast<float32>(orc_PieRect.right()) + static_cast<float32>(orc_PieRect.left())) /
        2.0F) - (static_cast<float32>(f32_DotSize) / 2.0F)) +
      ((static_cast<float32>(cos(static_cast<float64>(f32_DotPositionXCosInput))) *
        static_cast<float32>(orc_PieRect.width())) /
       2.0F);
   f32_DotPositionY =
      (((static_cast<float32>(orc_PieRect.bottom()) + static_cast<float32>(orc_PieRect.top())) /
        2.0F) - (static_cast<float32>(f32_DotSize) / 2.0F)) -
      (static_cast<float32>(sin(static_cast<float64>(f32_DotPositionYSinInput))) *
       (static_cast<float32>(orc_PieRect.height()) / static_cast<float32>(2.0)));

   orc_Painter.drawEllipse(static_cast<sintn>(f32_DotPositionX), static_cast<sintn>(f32_DotPositionY),
                           static_cast<sintn>(f32_DotSize), static_cast<sintn>(f32_DotSize));
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::m_SetPieRim(QPainter & orc_Painter, const QBrush oc_RimBrush,
                                           const QRect & orc_RimCircleRect, const float32 of32_RimSize,
                                           const float32 of32_TotalPixels, const float32 of32_ReductionFactor) const
{
   const float32 f32_WidthHeightMin =
      static_cast<float32>(std::min(this->rect().width(), this->rect().height()));
   QPen c_RandArc;

   c_RandArc.setBrush(oc_RimBrush); // Color
   // ReductionFactor to close or open gaps between the circles
   c_RandArc.setWidth(static_cast<sintn>(std::ceil((f32_WidthHeightMin *
                                                    (of32_RimSize - of32_ReductionFactor)) /
                                                   of32_TotalPixels)));
   c_RandArc.setCapStyle(Qt::FlatCap);
   orc_Painter.setPen(c_RandArc);
   // Arc with complete turn
   orc_Painter.drawArc(orc_RimCircleRect, static_cast<sint16>(90 * 16),
                       static_cast<sintn>(-360)  * 16);
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::m_SetInnerCircle(QPainter & orc_Painter, const QBrush oc_InnerCircleBrush,
                                                const QRect & orc_InnerCircleRect) const
{
   // Painting the inner Circle
   orc_Painter.setPen(Qt::NoPen);
   orc_Painter.setBrush(oc_InnerCircleBrush);
   orc_Painter.drawEllipse(orc_InnerCircleRect);
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::m_SetPieText(QPainter & orc_Painter, const QColor & orc_TextColor,
                                            const QRect & orc_InnerCircleRect, const QString & orc_DisplayString) const
{
   const uint16 u16_FontPixelSize = static_cast<uint16>(orc_InnerCircleRect.height());
   QPen c_TextPen;
   QFont c_TextFont;
   //   QLabel c_TextLabel;
   const float32 f32_TextPixelSize = static_cast<float32>(u16_FontPixelSize) / 3.0F;

   c_TextPen.setColor(orc_TextColor);
   c_TextFont.setFamily("Segoe UI");
   c_TextFont.setPixelSize(static_cast<sintn>(f32_TextPixelSize));

   //   c_TextLabel.setText(oc_DisplayString);
   //   c_TextLabel.setFont(c_TextFont);

   orc_Painter.setPen(c_TextPen);
   orc_Painter.setFont(c_TextFont);
   orc_Painter.drawText(orc_InnerCircleRect, static_cast<sintn>(Qt::AlignCenter), orc_DisplayString);
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardPieChart::m_SetAllPieRects(QRect & orc_InnerCircleRect, QRect & orc_RimCircleRect,
                                                QRect & orc_PieCircleRect, const float32 of32_PieGap,
                                                const float32 of32_PieSize, const float32 of32_RimSize,
                                                const float32 of32_TotalPixels,
                                                const float32 of32_AdditionalFactor) const
{
   const float32 f32_WidthHeightMin =
      static_cast<float32>(std::min(this->rect().width(), this->rect().height()));
   // Defining Inner Circle Rect --------------------------------------------------------
   const float32 f32_InnerCircleRectX =
      (static_cast<float32>(this->rect().left()) +
       ((f32_WidthHeightMin * (of32_PieGap + (of32_PieSize / 2.0F) + of32_RimSize)) /
        of32_TotalPixels)) - of32_AdditionalFactor;
   const float32 f32_InnerCircleRectY = (static_cast<float32>(this->rect().top()) +
                                         ((f32_WidthHeightMin *
                                           (of32_PieGap + (of32_PieSize / 2.0F) + of32_RimSize)) /
                                          of32_TotalPixels)) - of32_AdditionalFactor;
   const float32 f32_InnerCircleRectW =
      ((f32_WidthHeightMin *
        (of32_TotalPixels - (2.0F * (of32_PieGap + (of32_PieSize / 2.0F) + of32_RimSize)))) / of32_TotalPixels) +
      (2.0F * of32_AdditionalFactor);
   const float32 f32_InnerCircleRectH =
      ((f32_WidthHeightMin *
        (of32_TotalPixels - (2.0F * (of32_PieGap + (of32_PieSize / 2.0F) + of32_RimSize)))) / of32_TotalPixels) +
      (2.0F * of32_AdditionalFactor);
   // ------------------------------------------------------------------------------------

   // Defining Inner Rim Rect ------------------------------------------------------------
   const float32 f32_RimCircleRectX = static_cast<float32>(this->rect().left()) +
                                      ((f32_WidthHeightMin *
                                        (of32_PieGap + (of32_PieSize / 2.0F) + (of32_RimSize / 2.0F))) /
                                       of32_TotalPixels);
   const float32 f32_RimCircleRectY = static_cast<float32>(this->rect().top()) +
                                      ((f32_WidthHeightMin *
                                        (of32_PieGap + (of32_PieSize / 2.0F) + (of32_RimSize / 2.0F))) /
                                       of32_TotalPixels);
   const float32 f32_RimCircleRectW =
      (f32_WidthHeightMin *
       (of32_TotalPixels - (2.0F * (of32_PieGap + (of32_PieSize / 2.0F) + (of32_RimSize / 2.0F))))) / of32_TotalPixels;
   const float32 f32_RimCircleRectH =
      (f32_WidthHeightMin *
       (of32_TotalPixels - (2.0F * (of32_PieGap + (of32_PieSize / 2.0F) + (of32_RimSize / 2.0F))))) / of32_TotalPixels;
   // ------------------------------------------------------------------------------------

   // Defining Pie Circle Rect -----------------------------------------------------------
   const float32 f32_PieCircleRectX = (static_cast<float32>(this->rect().left()) +
                                       ((f32_WidthHeightMin * of32_PieGap) /
                                        of32_TotalPixels));
   const float32 f32_PieCircleRectY = static_cast<float32>(this->rect().top()) +
                                      (((f32_WidthHeightMin) * of32_PieGap) /
                                       of32_TotalPixels);
   const float32 f32_PieCircleRectW = (f32_WidthHeightMin *
                                       (of32_TotalPixels - (2.0F * of32_PieGap))) / of32_TotalPixels;
   const float32 f32_PieCircleRectH = (f32_WidthHeightMin *
                                       (of32_TotalPixels - (2.0F * of32_PieGap))) / of32_TotalPixels;

   // ------------------------------------------------------------------------------------

   orc_InnerCircleRect.setRect(static_cast<sintn>(f32_InnerCircleRectX),
                               static_cast<sintn>(f32_InnerCircleRectY),
                               static_cast<sintn>(f32_InnerCircleRectW),
                               static_cast<sintn>(f32_InnerCircleRectH));

   orc_RimCircleRect.setRect(static_cast<sintn>(f32_RimCircleRectX),
                             static_cast<sintn>(f32_RimCircleRectY),
                             static_cast<sintn>(f32_RimCircleRectW),
                             static_cast<sintn>(f32_RimCircleRectH));
   orc_PieCircleRect.setRect(static_cast<sintn>(f32_PieCircleRectX),
                             static_cast<sintn>(f32_PieCircleRectY),
                             static_cast<sintn>(f32_PieCircleRectW),
                             static_cast<sintn>(f32_PieCircleRectH));
}
