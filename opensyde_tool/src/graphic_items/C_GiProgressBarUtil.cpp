//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Util class for Progress bar in System Commissioning

      - Is used to set:
         - minimum value label
         - maximum value label
         - current value label
         - arrow  value label

      - Used to calculate the location of the current value on the horizontal and vertical axes
      - Used to calculate the arrow position and look

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <QPen>
#include <QPainter>
#include <array>

#include "stwtypes.hpp"
#include "C_GiProgressBarUtil.hpp"
#include "C_GiBiLineBounding.hpp"
#include "C_GiBiConnectableItem.hpp"
#include "C_SyvDaItUtil.hpp"
#include "C_PuiSvDbProgressBar.hpp"
#include "C_OgeDashboardProgressBar.hpp"

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
/*! \brief   Arrow drawing function

   \param[in]  opc_Painter             Current painter
   \param[in]  orc_ArrowCornerPoints   Arrow corner points
   \param[in]  orc_Pen                 Arrow pen
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_DrawArrow(QPainter * const opc_Painter, std::array<QPointF, 3> & orc_ArrowCornerPoints,
                                      const QPen & orc_Pen)
{
   const QPointF c_ArrowCornerPoint1 = orc_ArrowCornerPoints[0];
   const QPointF c_ArrowCornerPoint2 = orc_ArrowCornerPoints[1];
   const QPointF c_ArrowCornerPoint3 = orc_ArrowCornerPoints[2];

   QPointF c_P2;
   QPointF c_P3;

   if (C_GiBiLineBounding::h_CrossProduct(c_ArrowCornerPoint1 - c_ArrowCornerPoint2,
                                          c_ArrowCornerPoint1 - c_ArrowCornerPoint3) > 0.0)
   {
      c_P2 = c_ArrowCornerPoint2;
      c_P3 = c_ArrowCornerPoint3;
   }
   else
   {
      c_P2 = c_ArrowCornerPoint3;
      c_P3 = c_ArrowCornerPoint2;
   }

   QPainterPath c_Path;
   const float64_t f64_HalfWidth = orc_Pen.widthF() / 2.0;
   //Rects
   const QPolygonF c_Poly1To3 = mh_GetOuterRect(c_ArrowCornerPoint1, c_P3, c_P2, f64_HalfWidth);
   const QPolygonF c_Poly3To2 = mh_GetOuterRect(c_P3, c_P2, c_ArrowCornerPoint1, f64_HalfWidth);
   const QPolygonF c_Poly2To1 = mh_GetOuterRect(c_P2, c_ArrowCornerPoint1, c_P3, f64_HalfWidth);

   c_Path.setFillRule(Qt::WindingFill);

   //P1 to P3
   c_Path.moveTo(c_Poly1To3.at(3));
   c_Path.lineTo(c_Poly1To3.at(2));
   //Circle P3
   mh_SetArc(c_Path, c_P3, c_ArrowCornerPoint1, c_P2, f64_HalfWidth);
   //P3 to P2
   c_Path.lineTo(c_Poly3To2.at(2));
   //Circle P2
   mh_SetArc(c_Path, c_P2, c_ArrowCornerPoint1, c_P3, f64_HalfWidth);
   //P2 to P1
   c_Path.lineTo(c_Poly2To1.at(2));
   //Circle P1
   mh_SetArc(c_Path, c_ArrowCornerPoint1, c_P3, c_P2, f64_HalfWidth);
   //Close
   c_Path.closeSubpath();

   opc_Painter->save();
   opc_Painter->setPen(Qt::NoPen);
   opc_Painter->setRenderHint(QPainter::Antialiasing, true);
   opc_Painter->fillPath(c_Path, opc_Painter->brush());
   opc_Painter->setRenderHint(QPainter::Antialiasing, false);
   opc_Painter->restore();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the margin for type 1 progress bar

   \param[in]       orc_Rect                 Rectangle of the progress bar
   \param[in]       opc_Layout               Layout of the progress bar widget
   \param[in]       orq_ShowMinMax           Is used to define if the the maximum and the minimum value should be
                                             visible
   \param[in]       orc_MaxValue             Maximum value of the progress bar
   \param[in]       orc_MinValue             Minimum value of the progress bar
   \param[in]       orc_Unit                 Unit string that is presented on scene
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current, maximum,
                                             and minimum labels
   \param[in]       orf32_ArrowOffset        Arrow offset to progress bar

*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_DefineType1Margins(const QRect & orc_Rect, QHBoxLayout * const opc_Layout,
                                               const bool & orq_ShowMinMax, const QString & orc_MaxValue,
                                               const QString & orc_MinValue, const QString & orc_Unit,
                                               const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                               const float32_t & orf32_MinimumFontSize,
                                               const float32_t & orf32_ArrowOffset)
{
   const QMarginsF c_Margins = h_GetType1Margins(orc_Rect, orq_ShowMinMax, orc_MaxValue, orc_MinValue, orc_Unit,
                                                 ore_Alignment,
                                                 orf32_MinimumFontSize, orf32_ArrowOffset);

   opc_Layout->setContentsMargins(static_cast<int32_t>(c_Margins.left()),
                                  static_cast<int32_t>(c_Margins.top()),
                                  static_cast<int32_t>(c_Margins.right()),
                                  static_cast<int32_t>(c_Margins.bottom()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get progress bar type 1 font size

   \param[in]       orc_Rect                 Rectangle of the progress bar
   \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current,
                                             maximum, and minimum labels

   \return
   Font size
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_GiProgressBarUtil::h_GetType1FontSizeValue(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize)
{
   //Constants
   const float32_t f32_MODIFIER = 0.35F;

   const float32_t f32_FontSize =
      ((static_cast<float32_t>(orc_Rect.height()) * f32_MODIFIER) < orf32_MinimumFontSize) ?
      (orf32_MinimumFontSize) :
      (static_cast<float32_t>(orc_Rect.height()) * f32_MODIFIER);

   return f32_FontSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set minimum value label for progress bar type 1 on scene if it is allowed

   \param[in]       opc_ProgressBar      Progress bar widget
   \param[in]       orq_ShowMinMax       Is used to define if the the maximum and the minimum value should be
                                         visible
   \param[in]       orc_MinValue         Minimum value string
   \param[in]       orc_Unit             Unit string that is presented on scene
   \param[in]       ore_Alignment        Alignment of the progress var e.g. top, left, right, or bottom.
                                         It defined the alignment of the current value but also for the minimum
                                         and maximum value
   \param[in]       orc_Painter          Painter needed for drawing in scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_SetType1MinimumValue(const C_OgeDashboardProgressBar * const opc_ProgressBar,
                                                 const bool & orq_ShowMinMax, const QString & orc_MinValue,
                                                 const QString & orc_Unit,
                                                 const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                 QPainter & orc_Painter)
{
   if (orq_ShowMinMax)
   {
      const QFontMetrics c_FONT_METRICS = orc_Painter.fontMetrics();

      // Define font height and width
      const uint32_t u32_FontWidth = static_cast<uint32_t>(c_FONT_METRICS.horizontalAdvance(orc_MinValue + orc_Unit));
      const uint32_t u32_FontHeight = static_cast<uint32_t>(c_FONT_METRICS.height());

      // Define specific progress bar variables
      const uint32_t u32_ProgressBarXposition = opc_ProgressBar->pos().x();
      const uint32_t u32_ProgressBarYposition = opc_ProgressBar->pos().y();
      const uint32_t u32_ProgressBarHeight = opc_ProgressBar->height();

      // Define x and y position of the rect that holds the minimum value
      const uint32_t u32_MinRectXposition = u32_ProgressBarXposition - u32_FontWidth;
      const uint32_t u32_MinRectYposition =
         (ore_Alignment == C_PuiSvDbProgressBar::eTOP) ?
         u32_ProgressBarYposition :
         ((u32_ProgressBarYposition + u32_ProgressBarHeight) - u32_FontHeight);

      QRect c_MinRect;
      c_MinRect.setX(u32_MinRectXposition);
      c_MinRect.setY(u32_MinRectYposition);
      c_MinRect.setWidth(u32_FontWidth);
      c_MinRect.setHeight(u32_FontHeight);

      orc_Painter.drawText(c_MinRect, mhu32_PAINTER_FLAG, orc_MinValue + orc_Unit);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set maximum value label for progress bar type 1 on scene

   \param[in]       opc_ProgressBar          Progress bar widget
   \param[in]       orq_ShowMinMax           Is used to define if the the maximum and the minimum value should be
                                             visible
   \param[in]       orc_MaxValue             Maximum value of the progress bar
   \param[in]       orc_Unit                 Unit string that is presented on scene
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orc_Painter              Painter needed for drawing in scene

*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_SetType1MaximumValue(const C_OgeDashboardProgressBar * const opc_ProgressBar,
                                                 const bool & orq_ShowMinMax, const QString & orc_MaxValue,
                                                 const QString & orc_Unit,
                                                 const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                 QPainter & orc_Painter)
{
   if (orq_ShowMinMax)
   {
      const QFontMetrics c_FONT_METRICS = orc_Painter.fontMetrics();

      // Define font height and width
      const uint32_t u32_FontWidth = static_cast<uint32_t>(c_FONT_METRICS.horizontalAdvance(orc_MaxValue + orc_Unit));
      const uint32_t u32_FontHeight = static_cast<uint32_t>(c_FONT_METRICS.height());

      // Define specific progress bar variables
      const uint32_t u32_ProgressBarXposition = opc_ProgressBar->pos().x();
      const uint32_t u32_ProgressBarYposition = opc_ProgressBar->pos().y();
      const uint32_t u32_ProgressBarHeight = opc_ProgressBar->height();
      const uint32_t u32_ProgressBarWidth = opc_ProgressBar->width();

      // Define x and y position of the rect that holds the minimum value
      const uint32_t u32_MaxRectXposition = u32_ProgressBarXposition + u32_ProgressBarWidth;

      const uint32_t u32_MaxRectYposition =
         (ore_Alignment == C_PuiSvDbProgressBar::eTOP) ?
         u32_ProgressBarYposition :
         ((u32_ProgressBarYposition + u32_ProgressBarHeight) - u32_FontHeight);

      QRect c_MaxRect;
      c_MaxRect.setX(u32_MaxRectXposition);
      c_MaxRect.setY(u32_MaxRectYposition);
      c_MaxRect.setWidth(u32_FontWidth);
      c_MaxRect.setHeight(u32_FontHeight);

      orc_Painter.drawText(c_MaxRect, mhu32_PAINTER_FLAG, orc_MaxValue + orc_Unit);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the current value label for progress bar type 1 on scene

   \param[in]       opc_ProgressBar          Progress bar widget
   \param[in]       orc_MaxValue             Represents the string that is visible on the scene
   \param[in]       orc_MinValue             Minimum value string
   \param[in]       orc_CurrentValue         Current value string
   \param[in]       orc_Unit                 Unit string that is presented on scene
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orc_ArrowSpacerString    Spacer string to calculate the position
   \param[in]       orc_Painter              Painter needed for drawing in scene

   \return
   value rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
QRect C_GiProgressBarUtil::h_SetType1Value(const C_OgeDashboardProgressBar * const opc_ProgressBar,
                                           const QString & orc_MaxValue, const QString & orc_MinValue,
                                           const QString & orc_CurrentValue, const QString & orc_Unit,
                                           const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                           const QString & orc_ArrowSpacerString, QPainter & orc_Painter)
{
   // Constant
   const QFontMetrics c_FONT_METRICS = orc_Painter.fontMetrics();
   const uint32_t u32_MODIFIER = 2;

   //Variables
   QRect c_ValueRect;
   float64_t f64_CurrentValueXposition = mh_GetCurrentValueWidthPosition(opc_ProgressBar,
                                                                         orc_MaxValue,
                                                                         orc_MinValue,
                                                                         orc_CurrentValue);

   f64_CurrentValueXposition -=
      static_cast<float64_t>(c_FONT_METRICS.horizontalAdvance(orc_CurrentValue + orc_Unit)) /
      static_cast<float64_t>(u32_MODIFIER); // Get center

   const float32_t f32_ArrowSpacer =
      static_cast<float32_t>(c_FONT_METRICS.horizontalAdvance(orc_ArrowSpacerString)) / mhf32_ARROW_SIZE_H;

   // Define font height and width
   const uint32_t u32_FontWidth = static_cast<uint32_t>(c_FONT_METRICS.horizontalAdvance(orc_CurrentValue + orc_Unit));
   const uint32_t u32_FontHeight = static_cast<uint32_t>(c_FONT_METRICS.height());

   // Define specific progress bar variables
   const uint32_t u32_ProgressBarYposition = opc_ProgressBar->pos().y();
   const uint32_t u32_ProgressBarHeight = static_cast<uint32_t>(opc_ProgressBar->height());

   // Define x and y position of the rect that holds the minimum value
   const uint32_t u32_CurrentValueRectYposition =
      (ore_Alignment == C_PuiSvDbProgressBar::eTOP) ?
      ((u32_ProgressBarYposition - u32_FontHeight) - (u32_MODIFIER * static_cast<uint32_t>(f32_ArrowSpacer))) :
      ((u32_ProgressBarYposition + u32_ProgressBarHeight) + (u32_ProgressBarYposition +
                                                             static_cast<uint32_t>(f32_ArrowSpacer)));

   c_ValueRect = QRect(static_cast<uint32_t>(f64_CurrentValueXposition),
                       u32_CurrentValueRectYposition,
                       u32_FontWidth,
                       u32_FontHeight);

   orc_Painter.drawText(c_ValueRect, mhu32_PAINTER_FLAG, orc_CurrentValue + orc_Unit);

   return c_ValueRect;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the arrows for type 1 progress bar according to the alignment of the progress bar

   \param[in]       opc_ProgressBar          Progress bar widgets
   \param[in]       orc_ValueRect            Value rectangle is needed to define the position of the arrow
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orc_ArrowSpacerString    Spacer string to calculate the position
   \param[in]       orc_Painter              Painter needed for drawing in scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_SetType1Arrow(const C_OgeDashboardProgressBar * const opc_ProgressBar,
                                          const QRect & orc_ValueRect,
                                          const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                          const QString & orc_ArrowSpacerString, QPainter & orc_Painter)
{
   // Constants
   const QPen c_ArrowPen;

   if (ore_Alignment == C_PuiSvDbProgressBar::eTOP)
   {
      std::array<QPointF, 3> ac_ArrowCornerPoints = mh_GetType1ArrowCornerPoints(ore_Alignment,
                                                                                 orc_ArrowSpacerString,
                                                                                 orc_Painter, opc_ProgressBar,
                                                                                 orc_ValueRect);

      C_GiProgressBarUtil::h_DrawArrow(&orc_Painter,
                                       ac_ArrowCornerPoints,
                                       c_ArrowPen);
   }
   else if (ore_Alignment == C_PuiSvDbProgressBar::eBOTTOM)
   {
      std::array<QPointF, 3> ac_ArrowCornerPoints = mh_GetType1ArrowCornerPoints(ore_Alignment,
                                                                                 orc_ArrowSpacerString,
                                                                                 orc_Painter, opc_ProgressBar,
                                                                                 orc_ValueRect);
      C_GiProgressBarUtil::h_DrawArrow(&orc_Painter,
                                       ac_ArrowCornerPoints,
                                       c_ArrowPen);
   }
   else
   {
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get margins for type 1 progress bar

   This function is only used to get the

   \param[in]       orc_Rect                 Rectangle of the progress bar
   \param[in]       orq_ShowMinMax           Is used to define if the the maximum and the minimum value should be
                                             visible
   \param[in]       orc_MaxValue             Represents the string that is visible on the scene
   \param[in]       orc_MinValue             Represents the string that is visible on the scene
   \param[in]       orc_Unit                 Unit string that is presented on scene
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current, maximum,
                                             and minimum labels
   \param[in]       orf32_ArrowOffset        Arrow offset to progress bar


   \return
   margin calculated by the rectangle, font size and so on
*/
//----------------------------------------------------------------------------------------------------------------------
QMarginsF C_GiProgressBarUtil::h_GetType1Margins(const QRect & orc_Rect, const bool & orq_ShowMinMax,
                                                 const QString & orc_MaxValue, const QString & orc_MinValue,
                                                 const QString & orc_Unit,
                                                 const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                 const float32_t & orf32_MinimumFontSize,
                                                 const float32_t & orf32_ArrowOffset)
{
   //Variables
   float32_t f32_BarMarginTop = .0F;
   float32_t f32_BarMarginBottom = .0F;
   float32_t f32_BarMarginLeft = .0F;
   float32_t f32_BarMarginRight = .0F;

   // Getting the font size is very important to set the correct margin
   // Otherwise, there is not enough or too much margin
   const QFontMetrics c_TextFontMetric = mh_GetType1FontMetrics(orc_Rect, orf32_MinimumFontSize);

   mh_GetMarginType1TopBottom(&f32_BarMarginTop, &f32_BarMarginBottom, orf32_ArrowOffset, ore_Alignment,
                              c_TextFontMetric);

   mh_GetMarginType1LeftRight(&f32_BarMarginLeft, &f32_BarMarginRight, orc_MaxValue, orc_MinValue, orc_Unit,
                              orq_ShowMinMax,
                              c_TextFontMetric);

   QMarginsF c_Margin;

   c_Margin.setTop(f32_BarMarginTop);
   c_Margin.setBottom(f32_BarMarginBottom);
   c_Margin.setRight(f32_BarMarginRight);
   c_Margin.setLeft(f32_BarMarginLeft);

   return c_Margin;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the margin for type 2 progress bar

   \param[in]       orc_Rect                 Rectangle of the progress bar
   \param[in]       opc_Layout               Layout of the progress bar widget
   \param[in]       orq_ShowMinMax           Is used to define if the the maximum and the minimum value should be
                                             visible
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current, maximum,
                                             and minimum labels

*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_DefineType2Margins(const QRect & orc_Rect, QHBoxLayout * const opc_Layout,
                                               const bool & orq_ShowMinMax,
                                               const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                               const float32_t & orf32_MinimumFontSize)
{
   const QMarginsF c_Margin = h_GetType2Margins(orc_Rect,
                                                orq_ShowMinMax, ore_Alignment,
                                                orf32_MinimumFontSize);

   opc_Layout->setContentsMargins(static_cast<int32_t>(c_Margin.left()),
                                  static_cast<int32_t>(c_Margin.top()),
                                  static_cast<int32_t>(c_Margin.right()),
                                  static_cast<int32_t>(c_Margin.bottom()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get progress bar type 2 font size

  Therefore, the height of the rectangle is checked. It is is too small, the font size is set to minimum 15

   \param[in]       orc_Rect                 Rectangle of the progress bar
   \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current, maximum,
                                             and minimum labels

   \return
   Font size
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_GiProgressBarUtil::h_GetType2FontSizeValue(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize)
{
   //Constants
   const float32_t f32_FONT_MODIFIERT = 5.0F;

   float32_t f32_FontSize = (static_cast<float32_t>(orc_Rect.width()) / f32_FONT_MODIFIERT);

   if (f32_FontSize < orf32_MinimumFontSize)
   {
      f32_FontSize = orf32_MinimumFontSize;
   }

   return f32_FontSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the current value label for progress bar type 2 on scene

    \param[in]       orc_Rect                 Rectangle of the progress bar
    \param[in]       opc_ProgressBar          Progress bar widget
    \param[in]       orc_MaxValue             Represents the string that is visible on the scene
    \param[in]       orc_MinValue             Progress bar
    \param[in]       orc_CurrentValue         TextFontMetric needed to get the spacing to the progress bar right
    \param[in]       orc_Unit                 Unit string that is presented on scene
    \param[in]       orc_ArrowSpacerString    Spacer string to calculate the position
    \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
    \param[in]       ors32_BottomMargin       Bottom margin
    \param[in]       orc_Painter              Painter needed for drawing in scene

    \return
    QRect - value rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
QRect C_GiProgressBarUtil::h_SetType2Value(const QRect & orc_Rect,
                                           const C_OgeDashboardProgressBar * const opc_ProgressBar,
                                           const QString & orc_MaxValue, const QString & orc_MinValue,
                                           const QString & orc_CurrentValue, const QString & orc_Unit,
                                           const QString & orc_ArrowSpacerString,
                                           const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                           const int32_t & ors32_BottomMargin, QPainter & orc_Painter)
{
   //Constants
   const QFontMetrics c_FONT_METRICS = orc_Painter.fontMetrics();

   const float32_t f32_ArrowSpacer =
      static_cast<float32_t>(c_FONT_METRICS.horizontalAdvance(orc_ArrowSpacerString)) / mhf32_ARROW_SIZE_V;

   //Define font height and width
   const uint32_t u32_FontWidth =
      static_cast<uint32_t>(c_FONT_METRICS.horizontalAdvance(orc_CurrentValue + orc_Unit));
   const uint32_t u32_FontHeight = static_cast<uint32_t>(c_FONT_METRICS.height());

   //Define specific progress bar variables
   const uint32_t u32_ProgressBarXposition = opc_ProgressBar->pos().x();
   const uint32_t u32_ProgressBarWidth = opc_ProgressBar->width();

   //Define x and y position of the rect that holds the minimum value
   const uint32_t u32_CurrentValueRectXposition =
      (ore_Alignment == C_PuiSvDbProgressBar::eLEFT) ?
      ((u32_ProgressBarXposition - u32_FontWidth) -
       ((static_cast<uint32_t>(mhf32_ARROW_MODIFIER) * static_cast<uint32_t>(f32_ArrowSpacer)))) :
      ((u32_ProgressBarXposition + u32_ProgressBarWidth) +
       ((static_cast<uint32_t>(mhf32_ARROW_MODIFIER) * static_cast<uint32_t>(f32_ArrowSpacer))));

   // Variables
   QRect c_ValueRect;
   float64_t f64_CurrentValueYposition = mh_GetCurrentValueHeightPosition(orc_Rect, opc_ProgressBar, orc_MaxValue,
                                                                          orc_MinValue, orc_CurrentValue,
                                                                          ors32_BottomMargin);

   f64_CurrentValueYposition -= (static_cast<float64_t>(c_FONT_METRICS.height()) /
                                 static_cast<float64_t>(mhf32_ARROW_MODIFIER));
   c_ValueRect = QRect(u32_CurrentValueRectXposition,
                       static_cast<uint32_t>(f64_CurrentValueYposition),
                       u32_FontWidth,
                       u32_FontHeight);

   orc_Painter.drawText(c_ValueRect, mhu32_PAINTER_FLAG, orc_CurrentValue + orc_Unit);

   return c_ValueRect;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the arrows for type 2 progress bar according to the alignment of the progress bar

   \param[in]       opc_ProgressBar          Progress bar
   \param[in]       orc_ValueRect            Value rectangle is needed to define the position of the arrow
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orc_ArrowSpacerString    Spacer string to calculate the position
   \param[in]       orc_Painter              Painter needed for drawing in scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_SetType2Arrow(const C_OgeDashboardProgressBar * const opc_ProgressBar,
                                          const QRect & orc_ValueRect,
                                          const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                          const QString & orc_ArrowSpacerString, QPainter & orc_Painter)
{
   // Constants
   const QPen c_ArrowPen;
   const QFontMetrics c_FONT_METRICS = orc_Painter.fontMetrics();

   //Variables
   std::array<QPointF, 3> ac_ArrowCornerPoints;

   if (ore_Alignment == C_PuiSvDbProgressBar::eLEFT)
   {
      ac_ArrowCornerPoints = mh_GetType23ArrowCornerPoints(orc_ArrowSpacerString,
                                                           orc_ValueRect,
                                                           opc_ProgressBar,
                                                           c_FONT_METRICS,
                                                           ore_Alignment);

      C_GiProgressBarUtil::h_DrawArrow(&orc_Painter,
                                       ac_ArrowCornerPoints,
                                       c_ArrowPen);
   }
   else if (ore_Alignment == C_PuiSvDbProgressBar::eRIGHT)
   {
      ac_ArrowCornerPoints = mh_GetType23ArrowCornerPoints(orc_ArrowSpacerString,
                                                           orc_ValueRect,
                                                           opc_ProgressBar,
                                                           c_FONT_METRICS,
                                                           ore_Alignment);

      C_GiProgressBarUtil::h_DrawArrow(&orc_Painter,
                                       ac_ArrowCornerPoints,
                                       c_ArrowPen);
   }
   else
   {
      //Nothing to do here
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get margins for type 2 progress bar

   This function is only used to get the

   \param[in]       orc_Rect                 Rectangle of the progress bar
   \param[in]       orq_ShowMinMax           Is used to define if the the maximum and the minimum value should be
                                             visible
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current, maximum,
                                             and minimum labels


   \return
   QMarginsF - margin calculated by the rectangle, font size and so on
*/
//----------------------------------------------------------------------------------------------------------------------
QMarginsF C_GiProgressBarUtil::h_GetType2Margins(const QRect & orc_Rect, const bool & orq_ShowMinMax,
                                                 const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                 const float32_t & orf32_MinimumFontSize)
{
   //Constants
   const float32_t f32_FontSize = h_GetType2FontSizeValue(orc_Rect, orf32_MinimumFontSize);
   // 35% of the rect can have the progress bar. The other 65% are reserved for the current value that is placed
   // left or right from the progress bar
   const float32_t f32_RESERVED_CURRENT_VALUE_SPACING = 0.65F;

   //Variables
   float32_t f32_BarMarginTop = .0F;
   float32_t f32_BarMarginBottom = .0F;
   float32_t f32_BarMarginLeft = .0F;
   float32_t f32_BarMarginRight = .0F;

   // Getting the font size is very important to set the correct margin
   // Otherwise, there is not enough or too much margin
   QFont c_TextFont;
   QFontMetrics c_TextFontMetric(c_TextFont);

   c_TextFont.setPixelSize(static_cast<int32_t>(f32_FontSize));
   c_TextFontMetric = QFontMetrics(c_TextFont);

   mh_GetMarginType23LeftRight(&f32_BarMarginLeft, &f32_BarMarginRight, orc_Rect, ore_Alignment,
                               f32_RESERVED_CURRENT_VALUE_SPACING);
   mh_GetMarginTyp23TopBottom(&f32_BarMarginTop, &f32_BarMarginBottom, orq_ShowMinMax, c_TextFontMetric);

   QMarginsF c_Margin;
   c_Margin.setTop(f32_BarMarginTop);
   c_Margin.setBottom(f32_BarMarginBottom);
   c_Margin.setRight(f32_BarMarginRight);
   c_Margin.setLeft(f32_BarMarginLeft);

   return c_Margin;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Define the margin for type 3 progress bar

   \param[in]       orc_Rect                 Rectangle of the progress bar
   \param[in]       opc_Layout               Layout of the progress bar widget
   \param[in]       orq_ShowMinMax           Is used to define if the the maximum and the minimum value should be
                                             visible
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current, maximum,
                                             and minimum labels

*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_DefineType3Margins(const QRect & orc_Rect, QHBoxLayout * const opc_Layout,
                                               const bool & orq_ShowMinMax,
                                               const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                               const float32_t & orf32_MinimumFontSize)
{
   const QMarginsF c_Margins = h_GetType3Margins(orc_Rect,
                                                 orq_ShowMinMax,
                                                 ore_Alignment,
                                                 orf32_MinimumFontSize);

   opc_Layout->setContentsMargins(static_cast<int32_t>(c_Margins.left()),
                                  static_cast<int32_t>(c_Margins.top()),
                                  static_cast<int32_t>(c_Margins.right()),
                                  static_cast<int32_t>(c_Margins.bottom()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get progress bar type 3 font size for the current value label

    \param[in]       orc_Rect                 Rectangle of the progress bar
    \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current, maximum, and minimum labels

   \return
   float32_t - Font size
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_GiProgressBarUtil::h_GetType3FontSizeValue(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize)
{
   const float32_t f32_FONT_WIDTH_MODIFIER = 3.0F;

   float32_t f32_FontSize = (static_cast<float32_t>(orc_Rect.width()) / f32_FONT_WIDTH_MODIFIER);

   if (f32_FontSize < orf32_MinimumFontSize)
   {
      f32_FontSize = orf32_MinimumFontSize;
   }

   return f32_FontSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get progress bar type 3 font size for the minimum and maximum value label

    \param[in]       orc_Rect                 Rectangle of the progress bar
    \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current, maximum,
                                              and minimum labels

   \return
   float32_t - Font size
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_GiProgressBarUtil::h_GetType3FontSizeMaxMin(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize)
{
   //Constants
   const float32_t f32_FONT_MODIFIER = 4.0F;

   float32_t f32_FontSize = (static_cast<float32_t>(orc_Rect.width()) / f32_FONT_MODIFIER);

   if (f32_FontSize <= orf32_MinimumFontSize)
   {
      f32_FontSize = orf32_MinimumFontSize;
   }

   return f32_FontSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the current value label for progress bar type 3 on scene

   \param[in]       opc_ProgressBar      Progress bar
   \param[in]       orc_CurrentValue     Current value
   \param[in]       orc_Unit             Unit string that is presented on scene
   \param[in]       ore_Alignment        Alignment of the progress var e.g. top, left, right, or bottom.
                                         It defined the alignment of the current value but also for the minimum
                                         and maximum value
   \param[in]       orf32_FontSize       FontSize that will be set
   \param[in]       orc_Painter          Painter needed for drawing in scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_SetType3Value(
   const opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar, const QString & orc_CurrentValue,
   const QString & orc_Unit, const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment, const float32_t orf32_FontSize,
   QPainter & orc_Painter)
{
   //Define specific progress bar variables
   const uint32_t u32_ProgressBarXposition = opc_ProgressBar->pos().x();
   const uint32_t u32_ProgressBarYposition = opc_ProgressBar->pos().y();
   const uint32_t u32_ProgressBarHeight = opc_ProgressBar->height();
   const uint32_t u32_ProgressBarWidth = opc_ProgressBar->width();

   const uint32_t u32_HALF_PROGRESS_BAR_HEIGHT = u32_ProgressBarHeight / static_cast<uint32_t>(mhf32_HALF_MODIFIER);

   //Variables
   QRect c_ValueRect;
   QFont orc_TextFont;
   QFontMetrics orc_TextFontMetric(orc_TextFont);
   uint32_t u32_FontWidth;
   uint32_t u32_FontHeight;
   uint32_t u32_HalfFontHeight;
   uint32_t u32_MinRectXposition;
   uint32_t u32_MinRectYposition;

   orc_TextFont.setPixelSize(static_cast<int32_t>(orf32_FontSize));
   orc_TextFontMetric = QFontMetrics(orc_TextFont);

   //Define font height and width
   u32_FontWidth = static_cast<uint32_t>(orc_TextFontMetric.horizontalAdvance(orc_CurrentValue + orc_Unit));
   u32_FontHeight = static_cast<uint32_t>(orc_TextFontMetric.height());

   //Define x and y position of the rect that holds the minimum value
   u32_HalfFontHeight = u32_FontHeight / static_cast<uint32_t>(mhf32_HALF_MODIFIER);
   u32_MinRectXposition = (ore_Alignment == C_PuiSvDbProgressBar::eLEFT) ?
                          (u32_ProgressBarXposition - u32_FontWidth) :
                          (u32_ProgressBarXposition + u32_ProgressBarWidth);

   // To set the max value on top of the progress bar and there also in the middle of the bar's height, we take the y
   // position of the bar and add it by half the height of the progress bar and subtract half the font size
   u32_MinRectYposition = (u32_ProgressBarYposition + u32_HALF_PROGRESS_BAR_HEIGHT) -
                          u32_HalfFontHeight;

   c_ValueRect.setX(u32_MinRectXposition);
   c_ValueRect.setY(u32_MinRectYposition);
   c_ValueRect.setWidth(u32_FontWidth);
   c_ValueRect.setHeight(u32_FontHeight);

   orc_Painter.setFont(orc_TextFont);
   orc_Painter.drawText(c_ValueRect, mhu32_PAINTER_FLAG, orc_CurrentValue + orc_Unit);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get margins for type 3 progress bar

   This function is only used to get the

   \param[in]       orc_Rect                 Rectangle of the progress bar
   \param[in]       orq_ShowMinMax           Is used to define if the the maximum and the minimum value should be
                                             visible
   \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                             It defined the alignment of the current value but also for the minimum
                                             and maximum value
   \param[in]       orf32_MinimumFontSize    Minimum font size defines the minimum font size of the current, maximum,
                                             and minimum labels

   \return
   QMarginsF - margin calculated by the rectangle, font size and so on
*/
//----------------------------------------------------------------------------------------------------------------------
QMarginsF C_GiProgressBarUtil::h_GetType3Margins(const QRect & orc_Rect, const bool & orq_ShowMinMax,
                                                 const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                 const float32_t & orf32_MinimumFontSize)
{
   //Constants
   // 20% of the rect can have the progress bar. the other 80% are reserved for the current value that is placed
   // left or right from the progress bar
   const float32_t f32_RESERVED_CURRENT_VALUE_SPACING = 0.80F;
   // MaxMin font size is used because only the bottom and top
   // values are interesting. The values there are minimum and maximum value

   //Variables
   float32_t f32_BarMarginTop = .0F;
   float32_t f32_BarMarginBottom = .0F;
   float32_t f32_BarMarginLeft = .0F;
   float32_t f32_BarMarginRight = .0F;

   // Getting the font size is very important to set the correct margin
   // Otherwise, there is not enough or too much margin
   const QFontMetrics c_TextFontMetric = mh_GetType3FontMetrics(orc_Rect, orf32_MinimumFontSize);

   mh_GetMarginType23LeftRight(&f32_BarMarginLeft, &f32_BarMarginRight, orc_Rect, ore_Alignment,
                               f32_RESERVED_CURRENT_VALUE_SPACING);

   mh_GetMarginTyp23TopBottom(&f32_BarMarginTop, &f32_BarMarginBottom,
                              orq_ShowMinMax, c_TextFontMetric);
   QMarginsF c_Margins;
   c_Margins.setTop(f32_BarMarginTop);
   c_Margins.setBottom(f32_BarMarginBottom);
   c_Margins.setRight(f32_BarMarginRight);
   c_Margins.setLeft(f32_BarMarginLeft);

   return c_Margins;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set minimum and maximum value for progress bar type 2 and 3


    \param[in]       orc_Rect                 Rectangle of the progress bar
    \param[in]       opc_ProgressBar          Progress bar
    \param[in]       orq_ShowMinMax           Is used to define if the the maximum and the minimum value should be
    \param[in]       orc_MaxValue             Represents the string that is visible on the scene
    \param[in]       orc_MinValue             Minimum value of the progress bar
    \param[in]       orc_Unit                 Unit string that is presented on scene
    \param[in]       ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                              It defined the alignment of the current value but also for the minimum
                                              and maximum value
    \param[in]       orc_Painter              Painter needed for drawing in scene
                                              visible
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::h_SetType23MinimumMaximumValue(const QRect & orc_Rect,
                                                         const C_OgeDashboardProgressBar * const opc_ProgressBar,
                                                         const bool & orq_ShowMinMax, const QString & orc_MinValue,
                                                         const QString & orc_MaxValue, const QString & orc_Unit,
                                                         const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                         QPainter & orc_Painter)
{
   if (orq_ShowMinMax)
   {
      //Constants
      const QFontMetrics c_FONT_METRICS = orc_Painter.fontMetrics();

      //Define font height and width
      const uint32_t u32_FontWidthMinValue =
         static_cast<uint32_t>(c_FONT_METRICS.horizontalAdvance(orc_MinValue + orc_Unit));
      const uint32_t u32_FontWidthMaxValue =
         static_cast<uint32_t>(c_FONT_METRICS.horizontalAdvance(orc_MaxValue + orc_Unit));

      const uint32_t u32_FontHeight = static_cast<uint32_t>(c_FONT_METRICS.height());

      //Define specific progress bar variables
      const uint32_t u32_ProgressBarXposition = opc_ProgressBar->pos().x();
      const uint32_t u32_ProgressBarWidth = opc_ProgressBar->width();

      mh_SetType23MaximumValue(orc_MaxValue, orc_Unit, ore_Alignment, orc_Painter,
                               u32_FontWidthMaxValue, u32_FontHeight, u32_ProgressBarXposition,
                               u32_ProgressBarWidth);
      mh_SetType23MinimumValue(orc_Rect, orc_MinValue, orc_Unit, ore_Alignment, orc_Painter,
                               u32_FontWidthMinValue, u32_FontHeight, u32_ProgressBarXposition,
                               u32_ProgressBarWidth);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get rectangle at border of current line

   \param[in]  orc_P1               First point
   \param[in]  orc_P2               Second point
   \param[in]  orc_PointOpposite    Point opposite of rectangle
   \param[in]  of64_Width           Rectangle width

   \return
   Rectangle at border of current line (output: P1,P2,P4,P3 -> look at diagram)
*/
//----------------------------------------------------------------------------------------------------------------------
QPolygonF C_GiProgressBarUtil::mh_GetOuterRect(const QPointF & orc_P1, const QPointF & orc_P2,
                                               const QPointF & orc_PointOpposite, const float64_t of64_Width)
{
   QPolygonF c_Retval;
   /*
   //                                   P4/ \.
   //                                    /    \.
   //                                   /       \.
   //                                  /          \.
   //                                 /        90° /\P2
   //                                /            /  \
   //                               /            /    \
   //                              /            /      \
   //                             /            /        \
   //                            /            /          \
   //                           /            /            \
   //                          /            /              \
   //                         /            /                \
   //                        /            /                  \
   //                       /            /                    \
   //                      /            /                      \
   //                     /            /                        \
   //                    /            /                          \
   //                   /            /                            \
   //                  /            /                              \
   //                 /            /                                \
   //                /            /                                  \
   //               /            /                                    \
   //              /            /                                      \
   //             /            /                                        \
   //            /            /                                          \
   //           /            /                                            \
   //          /            /                                              \
   //         /            /                                                \
   //        /            /                                                  \
   //     P3/            /                                                    \
   //       \.          /                                                      \
   //         \.       /                                                        \
   //           \.90° /                                                          \
   //             \. /P1                                                          \
   //               \--------------------------------------------------------------
   */
   const QPointF c_P1ToP2 = orc_P2 - orc_P1;
   const QPointF c_Perpendicular = C_GiBiLineBounding::h_GetPerpendicular(c_P1ToP2);
   const float64_t f64_LengthP1Perpendicular = sqrt(
      (c_Perpendicular.x() * c_Perpendicular.x()) + (c_Perpendicular.y() * c_Perpendicular.y()));
   const QPointF c_PerpendicularToNewPoints = (c_Perpendicular / f64_LengthP1Perpendicular) * of64_Width;
   const QPointF c_P3Plus = orc_PointOpposite + c_PerpendicularToNewPoints;
   const QPointF c_P3Minus = orc_PointOpposite - c_PerpendicularToNewPoints;
   float64_t f64_Dist1;
   float64_t f64_Dist2;

   C_GiBiConnectableItem::h_DistToPoint(orc_P1, c_P3Plus, f64_Dist1);
   C_GiBiConnectableItem::h_DistToPoint(orc_P1, c_P3Minus, f64_Dist2);

   c_Retval.push_back(orc_P1);
   c_Retval.push_back(orc_P2);
   if (f64_Dist1 < f64_Dist2)
   {
      const QPointF c_P4 = orc_P2 + c_PerpendicularToNewPoints;
      const QPointF c_P3 = orc_P1 + c_PerpendicularToNewPoints;
      c_Retval.push_back(c_P4);
      c_Retval.push_back(c_P3);
   }
   else
   {
      const QPointF c_P4 = orc_P2 - c_PerpendicularToNewPoints;
      const QPointF c_P3 = orc_P1 - c_PerpendicularToNewPoints;
      c_Retval.push_back(c_P4);
      c_Retval.push_back(c_P3);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get circle around current point

   \param[in]  orc_P1      Current point
   \param[in]  of64_Width  Circle radius

   \return
   Rectangle for circle
*/
//----------------------------------------------------------------------------------------------------------------------
QRectF C_GiProgressBarUtil::mh_GetCircle(const QPointF & orc_P1, const float64_t of64_Width)
{
   return QRectF(orc_P1.x() - of64_Width, orc_P1.y() - of64_Width, of64_Width * 2.0, of64_Width * 2.0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply arc

   \param[in,out]  orc_Path            Current path
   \param[in]      orc_PointCenter     Arc center
   \param[in]      orc_P2              Other triangle point 1
   \param[in]      orc_P3              Other triangle point 2
   \param[in]      of64_Radius         Circle radius
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::mh_SetArc(QPainterPath & orc_Path, const QPointF & orc_PointCenter, const QPointF & orc_P2,
                                    const QPointF & orc_P3, const float64_t of64_Radius)
{
   const float64_t f64_FULL_360 = 2.0 * M_PI;
   QPointF c_P1;
   QPointF c_P2;

   if (C_GiBiLineBounding::h_CrossProduct(orc_PointCenter - orc_P2, orc_PointCenter - orc_P3) > 0.0)
   {
      c_P1 = orc_P2;
      c_P2 = orc_P3;
   }
   else
   {
      c_P1 = orc_P3;
      c_P2 = orc_P2;
   }
   {
      /*
      //                       \.   sweep      ./
      //                         \.----------./\
      //                           \.      ./   \Start angle
      //                             \.  ./      \
      //                           90° /\ 90° ------------
      //                              /^ \
      //                             / |  \
      //                            /  |   \
      //                           /PCenter \
      //                          /          \
      //                         /            \
      //                        /              \
      //                       /                \
      //                      /                  \
      //                     /                    \
      //                    /                      \
      //                   /                        \
      //                  /                          \
      //                 /                            \
      //                /                              \
      //               /                                \
      //              /                                  \
      //             /                                    \
      //            /                                      \
      //           /                                        \
      //          /                                          \
      //         /Alpha                                       \Beta
      //        /-----------------------                       \----------------------------------
      //       /                                                \
      //      /                                                  \
      //     /                                                    \
      //    /                                                      \
      //   /                                                        \
      //  /                                                          \
      // /                                                            \
      // --------------------------------------------------------------
      // P2                                                           P1
      */
      const float64_t f64_Beta = mh_GetArc(orc_PointCenter - c_P1, QPointF(1.0, 0.0));
      const float64_t f64_Alpha = mh_GetArc(orc_PointCenter - c_P2, QPointF(1.0, 0.0));
      const float64_t f64_Start = f64_Beta - (f64_FULL_360 / 4.0);
      const float64_t f64_Sweep =  ((f64_FULL_360 / 2.0) + f64_Alpha) - f64_Beta;

      orc_Path.arcTo(mh_GetCircle(orc_PointCenter,
                                  of64_Radius), f64_Start * (360.0 / f64_FULL_360), f64_Sweep * (360.0 / f64_FULL_360));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get angle between two vectors

   \param[in]  orc_V1   First vector
   \param[in]  orc_V2   Second vector

   \return
   Angle between vectors
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_GiProgressBarUtil::mh_GetArc(const QPointF & orc_V1, const QPointF & orc_V2)
{
   const float64_t f64_DotProduct = (orc_V1.x() * orc_V2.x()) + (orc_V1.y() * orc_V2.y());
   const float64_t f64_Determinant = (orc_V1.x() * orc_V2.y()) - (orc_V1.y() * orc_V2.x());
   const float64_t f64_Angle = atan2(f64_Determinant, f64_DotProduct);

   return f64_Angle;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Get progress bar type 1 arrow corner points

   \param[in]   ore_Alignment             Alignment of the progress var e.g. top, left, right, or bottom.
                                          It defined the alignment of the current value but also for the minimum
                                          and maximum value
   \param[in]   orc_ArrowSpacerString     Arrow size string that is used to define the horizontal size of the
                                          arrow
   \param[in]   orc_Painter               Painter needed for drawing in scene
   \param[in]   opc_ProgressBar           Progress bar widget
   \param[in]   orc_ValueRect             Rectangle of the value

   \return
   std::array<QPointF, 3> - Arrow corner points
*/
//----------------------------------------------------------------------------------------------------------------------
std::array<QPointF, 3> C_GiProgressBarUtil::mh_GetType1ArrowCornerPoints(
   const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment, const QString & orc_ArrowSpacerString,
   const QPainter & orc_Painter, const C_OgeDashboardProgressBar * const opc_ProgressBar, const QRect & orc_ValueRect)
{
   std::array<QPointF, 3> ac_ArrowPoints;

   // Progress bar values
   const float64_t f64_ProgressBarYposition = static_cast<float64_t>(opc_ProgressBar->y());
   const float64_t f64_ProgressBarHeight = static_cast<float64_t>(opc_ProgressBar->height());
   const float64_t f64_ProgressBarBottomYcorner = f64_ProgressBarYposition + f64_ProgressBarHeight;

   // Rectangular values
   const QPoint c_ValueRectCenterPoint = orc_ValueRect.center();
   const float64_t f64_ValueRectYposition = static_cast<float64_t>(orc_ValueRect.y());
   const float64_t f64_ValueRectHeight = static_cast<float64_t>(orc_ValueRect.height());
   const float64_t f64_ValueRectBottomYcorner = f64_ValueRectYposition + f64_ValueRectHeight;

   float32_t f32_ProgressBarPointDistance = .0F;

   if (ore_Alignment == C_PuiSvDbProgressBar::eTOP)
   {
      f32_ProgressBarPointDistance =
         abs(static_cast<float32_t>(f64_ProgressBarYposition) - static_cast<float32_t>(f64_ValueRectBottomYcorner));
   }
   else if (ore_Alignment == C_PuiSvDbProgressBar::eBOTTOM)
   {
      f32_ProgressBarPointDistance =
         abs(static_cast<float32_t>(f64_ProgressBarBottomYcorner) - static_cast<float32_t>(f64_ValueRectYposition));
   }
   else
   {
      //nothing to do here
   }

   f32_ProgressBarPointDistance = mh_ShrinkCornerPointsDistance(f32_ProgressBarPointDistance);

   // Others
   const QFontMetrics c_FONT_METRICS = orc_Painter.fontMetrics();
   const float32_t f32_HorizonalFontMetrics =
      static_cast<float32_t>(c_FONT_METRICS.horizontalAdvance(orc_ArrowSpacerString));

   const float32_t f32_PossibleHorizonalSpace = f32_HorizonalFontMetrics / mhf32_ARROW_SIZE_H;

   const float64_t f64_Point1Xposition = static_cast<float64_t>(c_ValueRectCenterPoint.x()) +
                                         static_cast<float64_t>(f32_PossibleHorizonalSpace);
   float64_t f64_Point1Yposition = f64_ValueRectBottomYcorner + mhf32_ARROW_BAR_SPACER;

   if (ore_Alignment == C_PuiSvDbProgressBar::eTOP)
   {
      f64_Point1Yposition = f64_ValueRectBottomYcorner + mhf32_ARROW_BAR_SPACER;
   }
   else if (ore_Alignment == C_PuiSvDbProgressBar::eBOTTOM)
   {
      f64_Point1Yposition = f64_ValueRectYposition + mhf32_ARROW_BAR_SPACER;
   }
   else
   {
      //nothing to do here
   }

   ac_ArrowPoints[0] = QPointF(f64_Point1Xposition,
                               f64_Point1Yposition);

   const float64_t f64_Point2Xposition = static_cast<float64_t>(c_ValueRectCenterPoint.x()) -
                                         static_cast<float64_t>(f32_PossibleHorizonalSpace);
   const float64_t f64_Point2Yposition = f64_Point1Yposition;

   ac_ArrowPoints[1] = QPointF(f64_Point2Xposition,
                               f64_Point2Yposition);

   const float64_t f64_Point3Xposition = static_cast<float64_t>(c_ValueRectCenterPoint.x());
   float64_t f64_Point3Yposition = .0F;
   if (ore_Alignment == C_PuiSvDbProgressBar::eTOP)
   {
      f64_Point3Yposition = f64_ValueRectBottomYcorner + f32_ProgressBarPointDistance;
   }
   else if (ore_Alignment == C_PuiSvDbProgressBar::eBOTTOM)
   {
      f64_Point3Yposition = f64_ValueRectYposition - f32_ProgressBarPointDistance;
   }
   else
   {
      //nothing to do here
   }

   ac_ArrowPoints[2] = QPointF(f64_Point3Xposition,
                               f64_Point3Yposition);

   return ac_ArrowPoints;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get progress bar type 2 and 3 arrow corner points

     \param[in]   orc_ArrowSpacerString   Arrow size string that is used to define the horizontal size of the
                                          arrow
   \param[in]   orc_ValueRect             Rectangle of the value
   \param[in]   opc_ProgressBar           Progress bar widget
   \param[in]   orc_FontMetrics           Font metrics
   \param[in]   ore_Alignment             Alignment of the progress var e.g. top, left, right, or bottom.
                                          It defined the alignment of the current value but also for the minimum
                                          and maximum value

   \return
   std::array<QPointF, 3> - arrow corner points
*/
//----------------------------------------------------------------------------------------------------------------------
std::array<QPointF, 3> C_GiProgressBarUtil::mh_GetType23ArrowCornerPoints(const QString & orc_ArrowSpacerString,
                                                                          const QRect & orc_ValueRect,
                                                                          const C_OgeDashboardProgressBar * const opc_ProgressBar, const QFontMetrics & orc_FontMetrics,
                                                                          const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment)
{
   std::array<QPointF, 3> ac_ArrowPoints;

   // Constants
   // Progress bar values
   const float64_t f64_ProgressBarXposition = static_cast<float64_t>(opc_ProgressBar->x());
   const float64_t f64_ProgressBarWidth = static_cast<float64_t>(opc_ProgressBar->width());
   const float64_t f64_PROGRESS_BAR_RIGHT_X_CORNER = f64_ProgressBarXposition + f64_ProgressBarWidth;

   // Rectangular values
   const uint32_t u32_RectangularxPosition = orc_ValueRect.x();
   const uint32_t u32_RectangularWidth = orc_ValueRect.width();
   const float64_t f64_RECTANGULAR_RIGHT_X_CORNER = static_cast<float64_t>(u32_RectangularxPosition) +
                                                    static_cast<float64_t>(u32_RectangularWidth);

   const QPoint c_ValueRectCenterPoint = orc_ValueRect.center();
   float32_t f32_ProgressBarPointDistance = .0F;

   if (ore_Alignment == C_PuiSvDbProgressBar::eLEFT)
   {
      f32_ProgressBarPointDistance = abs(
         static_cast<float32_t>(f64_ProgressBarXposition) -
         static_cast<float32_t>(f64_RECTANGULAR_RIGHT_X_CORNER));
   }

   else if (ore_Alignment == C_PuiSvDbProgressBar::eRIGHT)
   {
      f32_ProgressBarPointDistance =
         abs(static_cast<float32_t>(f64_PROGRESS_BAR_RIGHT_X_CORNER) -
             static_cast<float32_t>(u32_RectangularxPosition));
   }
   else
   {
      //nothing to do
   }
   f32_ProgressBarPointDistance = mh_ShrinkCornerPointsDistance(f32_ProgressBarPointDistance);

   // Spacer
   const float32_t f32_PossibleSpaceVertical =
      static_cast<float32_t>(orc_FontMetrics.horizontalAdvance(orc_ArrowSpacerString)) / mhf32_ARROW_SIZE_V;

   // Calculates the corner point of the arrow located at the bottom
   //from the progress bar position a bit left

   float64_t f64_Point1Xposition = 0.F;
   if (ore_Alignment == C_PuiSvDbProgressBar::eRIGHT)
   {
      //from the progress bar x-position I have to add the width to be at the bottom of the bar and add the spacer
      f64_Point1Xposition = static_cast<float64_t>(orc_ValueRect.x()) -
                            static_cast<float64_t>(mhf32_ARROW_BAR_SPACER);
   }
   else if (ore_Alignment == C_PuiSvDbProgressBar::eLEFT)
   {
      f64_Point1Xposition = f64_RECTANGULAR_RIGHT_X_CORNER + mhf32_ARROW_BAR_SPACER;
   }
   else
   {
      //nothing to do here
   }
   //from the value rectangle center point y a bit to the bottom
   const float64_t f64_POINT_1_Y_POSTION = static_cast<float64_t>(c_ValueRectCenterPoint.y()) +
                                           static_cast<float64_t>(f32_PossibleSpaceVertical);

   ac_ArrowPoints[0] = QPointF(f64_Point1Xposition,
                               f64_POINT_1_Y_POSTION);

   // Calculates the corner point of the arrow located at the top
   //from the progress bar position a bit right
   const float64_t f64_Point2Xposition = f64_Point1Xposition;
   //from the value rectangle center point y a bit to the top
   const float64_t f64_POINT_2_Y_POSITION = static_cast<float64_t>(c_ValueRectCenterPoint.y()) -
                                            static_cast<float64_t>(f32_PossibleSpaceVertical);

   ac_ArrowPoints[1] = QPointF(f64_Point2Xposition,
                               f64_POINT_2_Y_POSITION);

   float64_t f64_Point3Xposition = 0.F;

   if (ore_Alignment == C_PuiSvDbProgressBar::eLEFT)
   {
      // Calculates the top that is looking at the progress bar
      f64_Point3Xposition = f64_RECTANGULAR_RIGHT_X_CORNER + f32_ProgressBarPointDistance;
   }
   else if (ore_Alignment == C_PuiSvDbProgressBar::eRIGHT)
   {
      f64_Point3Xposition = ((f64_Point1Xposition - f32_ProgressBarPointDistance) + (mhf32_ARROW_BAR_SPACER));
   }
   else
   {
      //nothing to do here
   }
   const float64_t f64_Point3Yposition = static_cast<float64_t>(c_ValueRectCenterPoint.y());
   ac_ArrowPoints[2] = QPointF(f64_Point3Xposition,
                               f64_Point3Yposition);

   return ac_ArrowPoints;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get margin type 1 top and bottom

   \param[in,out]      opf32_MarginTop          Pointer that will store the margin bottom value
   \param[in,out]      opf32_MarginBottom       Pointer that will store the margin bottom value
   \param[in]          orf32_ArrowOffset        Arrow offset to progress bar
   \param[in]          ore_Alignment            Alignment of the progress var e.g. top, left, right, or bottom.
                                                It defined the alignment of the current value but also for the minimum
                                                and maximum value
   \param[in]          orc_TextFontMetric       Provides font metrics information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::mh_GetMarginType1TopBottom(float32_t * const opf32_MarginTop,
                                                     float32_t * const opf32_MarginBottom,
                                                     const float32_t orf32_ArrowOffset,
                                                     const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                     const QFontMetrics & orc_TextFontMetric)
{
   //Define top and bottom margin
   if (ore_Alignment == C_PuiSvDbProgressBar::eTOP)
   {
      *opf32_MarginTop = static_cast<float32_t>(orc_TextFontMetric.height()) + orf32_ArrowOffset;
      *opf32_MarginBottom = mhf32_DEFAULT_MARGIN;
   }
   else
   {
      *opf32_MarginBottom = static_cast<float32_t>(orc_TextFontMetric.height()) + orf32_ArrowOffset;
      *opf32_MarginTop = mhf32_DEFAULT_MARGIN;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the margin for left and right space

   \param[int,out]       opf32_MarginLeft       Margin left
   \param[in,out]        opf32_MarginRight      Margin right
   \param[in]            orc_MaxValue           Maximum value of the progress bar
   \param[in]            orc_MinValue           Minimum value of the progress bar
   \param[in]            orc_Unit               Unit of the values
   \param[in]            orq_ShowMinMax         Show or hide the minimum and maximum value
   \param[in]            orc_TextFontMetric     Provides font metrics information

*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::mh_GetMarginType1LeftRight(float32_t * const opf32_MarginLeft,
                                                     float32_t * const opf32_MarginRight, const QString orc_MaxValue,
                                                     const QString orc_MinValue, const QString orc_Unit,
                                                     const bool & orq_ShowMinMax,
                                                     const QFontMetrics & orc_TextFontMetric)
{
   // Define left and right margin
   // Take the space that is needed to show minimum and maximum value with its unit
   if (orq_ShowMinMax)
   {
      *opf32_MarginLeft =
         static_cast<float32_t>(orc_TextFontMetric.horizontalAdvance(orc_MinValue + orc_Unit));
      *opf32_MarginRight =
         static_cast<float32_t>(orc_TextFontMetric.horizontalAdvance(orc_MaxValue + orc_Unit));
   }
   // If no minimum and maximum value should be visible
   else
   {
      *opf32_MarginLeft = static_cast<float32_t>(orc_TextFontMetric.horizontalAdvance(orc_MinValue + orc_Unit)) /
                          mhf32_HALF_MODIFIER;
      *opf32_MarginRight = static_cast<float32_t>(orc_TextFontMetric.horizontalAdvance(orc_MaxValue + orc_Unit)) /
                           mhf32_HALF_MODIFIER;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get margin left and right for progress bar type 2

   \param[int,out]  opf32_MarginLeft                  Margin left
   \param[in,out]   opf32_MarginRight                 Margin right
   \param[in]       orc_Rect                          Rectangle of the progress bar
   \param[in]       ore_Alignment                     Alignment of the progress var e.g. top, left, right, or bottom.
                                                      It defined the alignment of the current value but also for the
                                                      minimum and maximum value
   \param[in]       orf32_ReservedCurrentValueSpace   Space that is reserved for the value. So, if 0.2(20%)
   is used to define the space 0.80 (80%) is reserved for the progress bar
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::mh_GetMarginType23LeftRight(float32_t * const opf32_MarginLeft,
                                                      float32_t * const opf32_MarginRight, const QRect & orc_Rect,
                                                      const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                      const float32_t & orf32_ReservedCurrentValueSpace)
{
   // Define left and right margin
   if (ore_Alignment == C_PuiSvDbProgressBar::eLEFT)
   {
      *opf32_MarginLeft = static_cast<float32_t>(orc_Rect.width()) * orf32_ReservedCurrentValueSpace;
      *opf32_MarginRight = mhf32_DEFAULT_MARGIN;
   }
   else
   {
      *opf32_MarginRight = static_cast<float32_t>(orc_Rect.width()) * orf32_ReservedCurrentValueSpace;
      *opf32_MarginLeft = mhf32_DEFAULT_MARGIN;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the margin top and margin bottom

   \param[in,out]       opf32_MarginTop        Pointer that will store the margin top value
   \param[in,out]       opf32_MarginBottom     Pointer that will store the margin bottom value
   \param[in]           orq_ShowMinMax         Show min and max value at the progress bar
   \param[in]           orc_TextFontMetric     Provides font metrics information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::mh_GetMarginTyp23TopBottom(float32_t * const opf32_MarginTop,
                                                     float32_t * const opf32_MarginBottom, const bool & orq_ShowMinMax,
                                                     const QFontMetrics & orc_TextFontMetric)
{
   // Define top and bottom margin
   if (orq_ShowMinMax)
   {
      *opf32_MarginBottom = static_cast<float32_t>(orc_TextFontMetric.height());
      *opf32_MarginTop = static_cast<float32_t>(orc_TextFontMetric.height());
   }
   // If no minimum and maximum value should be visible
   else
   {
      *opf32_MarginBottom = static_cast<float32_t>(orc_TextFontMetric.height()) / mhf32_HALF_MODIFIER;
      *opf32_MarginTop = static_cast<float32_t>(orc_TextFontMetric.height()) / mhf32_HALF_MODIFIER;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current value x position according to its value

   The alignment has to be added to the value later if needed

   \param[in]   orc_Rect            Rectangle of the progress bar
   \param[in]   opc_ProgressBar     Progress bar widget
   \param[in]   orc_MaxValue        Maximum value
   \param[in]   orc_MinValue        Minimum value
   \param[in]   orc_CurrentValue    Current value
   \param[in]   ors32_BottomMargin  Bottom margin of the layout

   \return
   float64_t - x position without right/left alignment check
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_GiProgressBarUtil::mh_GetCurrentValueHeightPosition(const QRect & orc_Rect,
                                                                const C_OgeDashboardProgressBar * const opc_ProgressBar,
                                                                const QString & orc_MaxValue,
                                                                const QString & orc_MinValue,
                                                                const QString & orc_CurrentValue,
                                                                const int32_t & ors32_BottomMargin)
{
   // Constants
   const uint32_t u32_PROGRESS_BAR_HEIGHT = opc_ProgressBar->height();

   const float32_t f32_DECIMAL_PERCENTAGE = mh_GetValueInPercentageDecimal(orc_MaxValue,
                                                                           orc_MinValue,
                                                                           orc_CurrentValue); //e.g. 0.5

   // Percentage has to be multiplied with progress bar height to get current height position
   const float64_t f64_HEIGHT_PERCENTAGE_PROGRESSBAR = static_cast<float64_t>(u32_PROGRESS_BAR_HEIGHT) *
                                                       f32_DECIMAL_PERCENTAGE;

   const int64_t s64_TOTAL_HEIGHT = static_cast<int64_t>(orc_Rect.height()) -
                                    static_cast<int64_t>(ors32_BottomMargin);

   // Since the minimum value is placed at the bottom of the bar, 0% has to be define at the bottom of the progress bar
   // as well.
   const float64_t f64_CURRENT_HEIGHT = static_cast<float64_t>(s64_TOTAL_HEIGHT) - f64_HEIGHT_PERCENTAGE_PROGRESSBAR;

   return f64_CURRENT_HEIGHT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current value y position according to its value percentage

   The alignment has to be added to the value later if needed

   \param[in]   opc_ProgressBar    Progress bar widget
   \param[in]   orc_MaxValue       Maximum value
   \param[in]   orc_MinValue       Minimum value
   \param[in]   orc_CurrentValue   Current value

   \return
   float64_t - y position without left/ right alignment check
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_GiProgressBarUtil::mh_GetCurrentValueWidthPosition(const C_OgeDashboardProgressBar * const opc_ProgressBar,
                                                               const QString & orc_MaxValue,
                                                               const QString & orc_MinValue,
                                                               const QString & orc_CurrentValue)
{
   const uint32_t u32_PROGRESS_BAR_WIDTH = opc_ProgressBar->width();
   const uint32_t u32_PROGRESS_BAR_X_POSITION = opc_ProgressBar->pos().x();

   const float32_t f32_DECIMAL_PERCENTAGE = mh_GetValueInPercentageDecimal(orc_MaxValue,
                                                                           orc_MinValue,
                                                                           orc_CurrentValue); //e.g. 0.5

   // Percentage has to be multiplied with progress bar width to get current position
   const float64_t f64_WIDTH_PERCENTAGE_PROGRESSBAR = static_cast<float64_t>(u32_PROGRESS_BAR_WIDTH) *
                                                      (f32_DECIMAL_PERCENTAGE);

   // Since I want that the 0 position is at the beginning of the progress bar I have to add the x position of the
   // progress bar
   const float64_t f64_CURRENT_WIDTH = static_cast<float64_t>(u32_PROGRESS_BAR_X_POSITION) +
                                       f64_WIDTH_PERCENTAGE_PROGRESSBAR;

   return f64_CURRENT_WIDTH;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculate percentage of the current value according to minimum and maximum value

   \param[in]   orc_CurrentValue   Current value
   \param[in]   orc_MinValue       Minimum value
   \param[in]   orc_MaxValue       Maximum value

   \return
   percentage e.g 5%
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_GiProgressBarUtil::mh_GetValueInPercentageDecimal(const QString & orc_MaxValue,
                                                              const QString & orc_MinValueString,
                                                              const QString & orc_CurrentValue)
{
   const float64_t f64_VALUE = orc_CurrentValue.toDouble();
   const float64_t f64_MAXIMUM_VALUE = orc_MaxValue.toDouble();
   const float64_t f64_MINIMUM_VALUE = orc_MinValueString.toDouble();
   const float32_t f32_MAX_PERCENT = 100.0F;

   const float32_t f32_PERCENT =
      static_cast<float32_t>(((f64_VALUE - f64_MINIMUM_VALUE) / (f64_MAXIMUM_VALUE - f64_MINIMUM_VALUE)) *
                             f32_MAX_PERCENT);

   const float32_t f32_DECIMAL_PERCENTAGE = f32_PERCENT / f32_MAX_PERCENT;

   return f32_DECIMAL_PERCENTAGE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set minimum value label for progress bar type 2 and type 3 on scene

    \param[in]       orc_Rect                      Rectangle of the progress bar
    \param[in]       orc_MinValue                  Minimum value of the progress bar
    \param[in]       orc_Unit                      Unit string that is presented on scene
    \param[in]       ore_Alignment                 Alignment of the progress var e.g. top, left, right, or bottom.
                                                   It defined the alignment of the current value but also for the
                                                   minimum and maximum value
    \param[in]       orc_Painter                   Painter needed for drawing in scene visible
    \param[in]       oru32_FontWidthMinValue       Font width of the minimum value
    \param[in]       oru32_FontHeight              Font height
    \param[in]       oru32_ProgressBarXposition    Progress bar x position
    \param[in]       oru32_ProgressBarWidth        Progress bar width
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::mh_SetType23MinimumValue(const QRect & orc_Rect, const QString & orc_MinValue,
                                                   const QString & orc_Unit,
                                                   const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                   QPainter & orc_Painter, const uint32_t & oru32_FontWidthMinValue,
                                                   const uint32_t & oru32_FontHeight,
                                                   const uint32_t & oru32_ProgressBarXposition,
                                                   const uint32_t & oru32_ProgressBarWidth)
{
   QRect c_MinRect;
   uint32_t u32_MinRectYposition = 0;
   uint32_t u32_MinRectXposition = 0;

   // If the Minimum value plus its unit width is bigger than the progress bar width
   // Check the alignment
   // If alignment is left, it means that on the left side is still space, so move the label a bit to the left, so
   // that the end of the minimum/unit-label end where the progress bar ends
   // If the alignment is right, it means that on the right side is still space, the label should start where the
   // progress bar start as well
   if (oru32_ProgressBarWidth < oru32_FontWidthMinValue)
   {
      const uint32_t u32_RectHeight = orc_Rect.height();
      u32_MinRectYposition = u32_RectHeight + oru32_FontHeight;

      if (ore_Alignment == C_PuiSvDbProgressBar::eLEFT)
      {
         const uint32_t u32_NeededSpace = oru32_FontWidthMinValue - oru32_ProgressBarWidth;
         u32_MinRectXposition = oru32_ProgressBarXposition - u32_NeededSpace;
      }
      else if (ore_Alignment == C_PuiSvDbProgressBar::eRIGHT)
      {
         u32_MinRectXposition = oru32_ProgressBarXposition;
      }
      else
      {
         // nothing to do here
      }
   }
   // Else move label to the center of the progress bar (Center of the label and the progress bar)
   else
   {
      //Constants
      const uint32_t u32_RectHeight = orc_Rect.height();

      const float32_t f32_HALF_PROGRESS_BAR_WIDTH = static_cast<float32_t>(oru32_ProgressBarWidth) /
                                                    mhf32_HALF_MODIFIER;
      const float32_t f32_HALF_FONT_WIDTH = static_cast<float32_t>(oru32_FontWidthMinValue) / mhf32_HALF_MODIFIER;

      u32_MinRectXposition = (oru32_ProgressBarXposition +
                              static_cast<uint32_t>(f32_HALF_PROGRESS_BAR_WIDTH)) -
                             static_cast<uint32_t>(f32_HALF_FONT_WIDTH);
      u32_MinRectYposition = u32_RectHeight - oru32_FontHeight;
   }

   c_MinRect.setX(u32_MinRectXposition);
   c_MinRect.setY(u32_MinRectYposition);
   c_MinRect.setWidth(oru32_FontWidthMinValue);
   c_MinRect.setHeight(oru32_FontHeight);
   orc_Painter.drawText(c_MinRect, mhu32_PAINTER_FLAG, orc_MinValue + orc_Unit);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set maximum value label for progress bar type 2 and 3 on scene

  The maximum value is displayed on top of the progress bar. If the font width (spacing that is needed to display the
  whole maximum value plus the unit) it shall check which alignment the progress bar has.

   \param[in]       orc_MaxValue                   Represents the string that is visible on the scene
   \param[in]       orc_Unit                       Unit string that is presented on scene
   \param[in]       ore_Alignment                 Alignment of the progress var e.g. top, left, right, or bottom.
                                                  It defined the alignment of the current value but also for the
                                                  minimum and maximum value
   \param[in]       orc_Painter                   Painter needed for drawing in scene
   \param[in]       oru32_FontWidthMaxValue       Font width of the maximum value
   \param[in]       oru32_FontHeight              Font height
   \param[in]       oru32_ProgressBarXposition    Progress bar x position
   \param[in]       oru32_ProgressBarWidth        Progress bar width

*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiProgressBarUtil::mh_SetType23MaximumValue(const QString & orc_MaxValue, const QString & orc_Unit,
                                                   const C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                                   QPainter & orc_Painter, const uint32_t & oru32_FontWidthMaxValue,
                                                   const uint32_t & oru32_FontHeight,
                                                   const uint32_t & oru32_ProgressBarXposition,
                                                   const uint32_t & oru32_ProgressBarWidth)
{
   //Constants
   const uint32_t u32_MAX_RECT_Y_POSITION = 0;

   //Define rect that will be displayed
   QRect c_MaxRect;
   uint32_t u32_MaxRectXposition = 0;

   // if the font width (spacing that is needed to display the whole maximum value plus the unit)
   // it shall check which alignment the progress bar has
   if (oru32_ProgressBarWidth < oru32_FontWidthMaxValue)
   {
      //Left aligned progress bar -> display maximum value plus unit that start on the right side of
      //the progress bar
      if (ore_Alignment == C_PuiSvDbProgressBar::eLEFT)
      {
         const uint32_t u32_NeededSpace = (oru32_FontWidthMaxValue - oru32_ProgressBarWidth);
         u32_MaxRectXposition = oru32_ProgressBarXposition - u32_NeededSpace;
      }
      //Right aligned progress bar -> display maximum value plus unit that start on the left side of
      //the progress bar
      else if (ore_Alignment == C_PuiSvDbProgressBar::eRIGHT)
      {
         u32_MaxRectXposition = oru32_ProgressBarXposition;
      }
      else
      {
         // nothing to do here
      }
   }
   else
   {
      //Define x and y position of the rectangle that defines the rectangle center aligned to the progress bat
      u32_MaxRectXposition =
         (oru32_ProgressBarXposition + (oru32_ProgressBarWidth / static_cast<uint32_t>(mhf32_HALF_MODIFIER))) -
         (oru32_FontWidthMaxValue / static_cast<uint32_t>(mhf32_HALF_MODIFIER));
   }

   c_MaxRect.setX(u32_MaxRectXposition);
   c_MaxRect.setY(u32_MAX_RECT_Y_POSITION);
   c_MaxRect.setWidth(oru32_FontWidthMaxValue);
   c_MaxRect.setHeight(oru32_FontHeight);

   orc_Painter.drawText(c_MaxRect, mhu32_PAINTER_FLAG, orc_MaxValue + orc_Unit);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  If possible it shrinks the corner points distance

   This is only possible if the corner points distance not 0 obviously.

   \param[in]       orf32_ProgressBarPointDistance     Current calculated corner points distance

   \return
   float32_t
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_GiProgressBarUtil::mh_ShrinkCornerPointsDistance(const float32_t & orf32_ProgressBarPointDistance)
{
   const float32_t f32_ZERO_DISTANCE = 0.F;
   const float32_t f32_SPACER_MODIFIER = 2.0F;

   float32_t f32_ProgressBarPointDistance = orf32_ProgressBarPointDistance;

   if ((f32_ProgressBarPointDistance - (f32_SPACER_MODIFIER * mhf32_ARROW_BAR_SPACER)) > f32_ZERO_DISTANCE)
   {
      f32_ProgressBarPointDistance -= mhf32_ARROW_BAR_SPACER;
   }

   return f32_ProgressBarPointDistance;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get font metrics for progress bar type 1

   \param[in]       orc_Rect                  Rectangle of the progress bar
   \param[in]       orf32_MinimumFontSize     Minimum font size defines the minimum font size of the current, maximum,
                                              and minimum labels
   \return
   QFontMetrics
*/
//----------------------------------------------------------------------------------------------------------------------
QFontMetrics C_GiProgressBarUtil::mh_GetType1FontMetrics(const QRect & orc_Rect,
                                                         const float32_t & orf32_MinimumFontSize)
{
   QFont c_TextFont;
   const float32_t f32_FontSize = h_GetType1FontSizeValue(orc_Rect, orf32_MinimumFontSize);

   QFontMetrics c_TextFontMetric(c_TextFont);

   if (static_cast<int32_t>(f32_FontSize) != c_TextFont.pixelSize())
   {
      c_TextFont.setPixelSize(static_cast<int32_t>(f32_FontSize));
      c_TextFontMetric = QFontMetrics(c_TextFont);
   }

   return c_TextFontMetric;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get font metrics for progress bar type 3

   \param[in]       orc_Rect                  Rectangle of the progress bar
   \param[in]       orf32_MinimumFontSize     Minimum font size defines the minimum font size of the current, maximum,
                                              and minimum labels
   \return
   QFontMetrics
*/
//----------------------------------------------------------------------------------------------------------------------
QFontMetrics C_GiProgressBarUtil::mh_GetType3FontMetrics(const QRect & orc_Rect,
                                                         const float32_t & orf32_MinimumFontSize)
{
   const float32_t f32_FontSize = h_GetType3FontSizeMaxMin(orc_Rect, orf32_MinimumFontSize);

   QFont c_TextFont;
   QFontMetrics c_TextFontMetric(c_TextFont);

   if (static_cast<int32_t>(f32_FontSize) != c_TextFont.pixelSize())
   {
      c_TextFont.setPixelSize(static_cast<int32_t>(f32_FontSize));
      c_TextFontMetric = QFontMetrics(c_TextFont);
   }
   return c_TextFontMetric;
}
