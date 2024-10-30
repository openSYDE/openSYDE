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
#ifndef C_GIPROGRESSBARUTIL_H
#define C_GIPROGRESSBARUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QHBoxLayout>
#include <QPen>
#include <QPainter>
#include <array>

#include "stwtypes.hpp"
#include "C_OgeLabDashboardDefault.hpp"
#include "C_PuiSvDbProgressBar.hpp"
#include "C_OgeDashboardProgressBar.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiProgressBarUtil
{
public:
   static void h_DrawArrow(QPainter * const opc_Painter, std::array<QPointF, 3> & orc_ArrowCornerPoints,
                           const QPen & orc_Pen);

   //-------Define progress bar type 1----------------------------------------------------------------------------------
   static void h_DefineType1Margins(const QRect & orc_Rect, QHBoxLayout * const opc_Layout, const bool & orq_ShowMinMax,
                                    const QString & orc_MaxValue, const QString & orc_MinValue,
                                    const QString & orc_Unit,
                                    const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                    const float32_t & orf32_MinimumFontSize, const float32_t & orf32_ArrowOffset);
   static float32_t h_GetType1FontSizeValue(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize);
   static void h_SetType1MinimumValue(
      const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar,
      const bool & orq_ShowMinMax, const QString & orc_MinValue, const QString & orc_Unit,
      const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment, QPainter & orc_Painter);
   static void h_SetType1MaximumValue(
      const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar,
      const bool & orq_ShowMinMax, const QString & orc_MaxValue, const QString & orc_Unit,
      const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment, QPainter & orc_Painter);
   static QRect h_SetType1Value(const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar,
                                const QString & orc_MaxValue, const QString & orc_MinValue,
                                const QString & orc_CurrentValue, const QString & orc_Unit,
                                const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                const QString & orc_ArrowSpacerString, QPainter & orc_Painter);

   static void h_SetType1Arrow(const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar,
                               const QRect & orc_ValueRect,
                               const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                               const QString & orc_ArrowSpacerString, QPainter & orc_Painter);

   static QMarginsF h_GetType1Margins(const QRect & orc_Rect, const bool & orq_ShowMinMax, const QString & orc_MaxValue,
                                      const QString & orc_MinValue, const QString & orc_Unit,
                                      const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                      const float32_t & orf32_MinimumFontSize, const float32_t & orf32_ArrowOffset);

   //-------Define progress bar type 2----------------------------------------------------------------------------------
   static void h_DefineType2Margins(const QRect & orc_Rect, QHBoxLayout * const opc_Layout, const bool & orq_ShowMinMax,
                                    const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                    const float32_t & orf32_MinimumFontSize);
   static float32_t h_GetType2FontSizeValue(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize);
   static QRect h_SetType2Value(const QRect & orc_Rect,
                                const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar,
                                const QString & orc_MaxValue, const QString & orc_MinValue,
                                const QString & orc_CurrentValue, const QString & orc_Unit,
                                const QString & orc_ArrowSpacerString,
                                const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                const int32_t & ors32_BottomMargin, QPainter & orc_Painter);
   static void h_SetType2Arrow(const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar,
                               const QRect & orc_ValueRect,
                               const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                               const QString & orc_ArrowSpacerString, QPainter & orc_Painter);

   static QMarginsF h_GetType2Margins(const QRect & orc_Rect, const bool & orq_ShowMinMax,
                                      const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                      const float32_t & orf32_MinimumFontSize);

   //-------Define progress bar type 3----------------------------------------------------------------------------------
   static void h_DefineType3Margins(const QRect & orc_Rect, QHBoxLayout * const opc_Layout, const bool & orq_ShowMinMax,
                                    const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                    const float32_t & orf32_MinimumFontSize);
   static float32_t h_GetType3FontSizeValue(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize);
   static float32_t h_GetType3FontSizeMaxMin(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize);
   static void h_SetType3Value(const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar,
                               const QString & orc_CurrentValue, const QString & orc_Unit,
                               const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                               const float32_t orf32_FontSize, QPainter & orc_Painter);
   static QMarginsF h_GetType3Margins(const QRect & orc_Rect, const bool & orq_ShowMinMax,
                                      const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
                                      const float32_t & orf32_MinimumFontSize);

   //-------Define progress bar type 2 and 3 Function-----------------------------------------------------------
   static void h_SetType23MinimumMaximumValue(const QRect & orc_Rect,
                                              const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar, const bool & orq_ShowMinMax, const QString & orc_MinValue, const QString & orc_MaxValue, const QString & orc_Unit, const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment, QPainter & orc_Painter);

private:
   //-------Functions---------------------------------------------------------------------------------------------------
   static QPolygonF mh_GetOuterRect(const QPointF & orc_P1, const QPointF & orc_P2, const QPointF & orc_PointOpposite,
                                    const float64_t of64_Width);
   static QRectF mh_GetCircle(const QPointF & orc_P1, const float64_t of64_Width);
   static void mh_SetArc(QPainterPath & orc_Path, const QPointF & orc_PointCenter, const QPointF & orc_P2,
                         const QPointF & orc_P3, const float64_t of64_Radius);
   static float64_t mh_GetArc(const QPointF & orc_V1, const QPointF & orc_V2);

   // Calculate arrow points for Type 1 and 2
   static std::array<QPointF, 3> mh_GetType1ArrowCornerPoints(
      const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment,
      const QString & orc_ArrowSpacerString, const QPainter & orc_Painter,
      const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar,
      const QRect & orc_ValueRect);

   static std::array<QPointF, 3> mh_GetType23ArrowCornerPoints(const QString & orc_ArrowSpacerString,
                                                               const QRect & orc_ValueRect,
                                                               const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar, const QFontMetrics & orc_FontMetrics, const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment);

   // Progress bar type 1 calculation
   static void mh_GetMarginType1TopBottom(float32_t * const opf32_MarginTop, float32_t * const opf32_MarginBottom,
                                          const float32_t orf32_ArrowOffset,
                                          const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment, const QFontMetrics & orc_TextFontMetric);

   static void mh_GetMarginType1LeftRight(float32_t * const opf32_MarginLeft, float32_t * const opf32_MarginRight,
                                          const QString orc_MaxValue, const QString orc_MinValue,
                                          const QString orc_Unit, const bool & orq_ShowMinMax,
                                          const QFontMetrics & orc_TextFontMetric);

   // Progress bar type 2 calculation
   static void mh_GetMarginType23LeftRight(float32_t * const opf32_MarginLeft, float32_t * const opf32_MarginRight,
                                           const QRect & orc_Rect,
                                           const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment, const float32_t & orf32_ReservedCurrentValueSpace);

   static void mh_GetMarginTyp23TopBottom(float32_t * const opf32_MarginTop, float32_t * const opf32_MarginBottom,
                                          const bool & orq_ShowMinMax, const QFontMetrics & orc_TextFontMetric);

   // Calculate height position
   static float64_t mh_GetCurrentValueHeightPosition(const QRect & orc_Rect,
                                                     const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar, const QString & orc_MaxValue, const QString & orc_MinValue, const QString & orc_CurrentValue, const int32_t & ors32_BottomMargin);
   static float64_t mh_GetCurrentValueWidthPosition(
      const stw::opensyde_gui_elements::C_OgeDashboardProgressBar * const opc_ProgressBar,
      const QString & orc_MaxValue, const QString & orc_MinValue, const QString & orc_CurrentValue);
   static float32_t mh_GetValueInPercentageDecimal(const QString & orc_MaxValue, const QString & orc_MinValueString,
                                                   const QString & orc_CurrentValue);

   //Set progress bar type 2 and type 3 minimum and maximum value
   static void mh_SetType23MinimumValue(const QRect & orc_Rect, const QString & orc_MinValue, const QString & orc_Unit,
                                        const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment, QPainter & orc_Painter, const uint32_t & oru32_FontWidthMinValue, const uint32_t & oru32_FontHeight, const uint32_t & oru32_ProgressBarXposition, const uint32_t & oru32_ProgressBarWidth);
   static void mh_SetType23MaximumValue(const QString & orc_MaxValue, const QString & orc_Unit,
                                        const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment & ore_Alignment, QPainter & orc_Painter, const uint32_t & oru32_FontWidthMaxValue, const uint32_t & oru32_FontHeight, const uint32_t & oru32_ProgressBarXposition, const uint32_t & oru32_ProgressBarWidth);

   // General
   static float32_t mh_ShrinkCornerPointsDistance(const float32_t & orf32_ProgressBarPointDistance);
   static QFontMetrics mh_GetType1FontMetrics(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize);
   static QFontMetrics mh_GetType3FontMetrics(const QRect & orc_Rect, const float32_t & orf32_MinimumFontSize);

   //-------Constants---------------------------------------------------------------------------------------------------
   // General constants
   static constexpr float32_t mhf32_DEFAULT_MARGIN = 0.0F;
   static constexpr float32_t mhf32_HALF_MODIFIER = 2.0F;
   static constexpr uint32_t mhu32_PAINTER_FLAG = 0; // No alignment flag
   static constexpr float32_t mhf32_ARROW_BAR_SPACER = 1.0F;

   // Arrow constants
   static constexpr float32_t mhf32_ARROW_SIZE_H = 3.0F;
   static constexpr float32_t mhf32_ARROW_SIZE_V = 3.0F;
   static constexpr float32_t mhf32_ARROW_MODIFIER = 2.0F;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
