//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dashboard pie chart item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIDASHBOARDPIECHART_H
#define C_OGEWIDASHBOARDPIECHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiDashboardPieChart :
   public QWidget
{
public:
   explicit C_OgeWiDashboardPieChart(QWidget * const opc_Parent = NULL);

   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode,
                        const bool oq_ShowUnit, const bool oq_ShowValue);
   void ReInitSize(void);
   void SetValue(const QString & orc_Value, const int32_t os32_Progress2000000);
   void SetUnit(const QString & orc_Value);
   void SetTransparency(const int32_t os32_Value);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   static const QColor mhc_TRANSPARENT;
   QColor mc_ValueStart;
   QColor mc_ValueEnd;
   QColor mc_InnerBorderTop;
   QColor mc_InnerBorderBottom;
   QColor mc_InnerCircleTop;
   QColor mc_InnerCircleBottom;
   QColor mc_StringColor;
   bool mq_ValueCircle;
   int32_t ms32_Value;
   const int32_t ms32_Min;
   const int32_t ms32_Max;
   QString mc_Unit;
   QString mc_Value;
   int32_t ms32_ValueWidth;
   int32_t ms32_BorderWidth;
   bool mq_ShowUnit;
   bool mq_ShowValue;
   QFont mc_Font;
   int32_t ms32_Transparency;
   bool mq_TransparentBackground;
   stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style me_Style;
   bool mq_DarkMode;
   void m_SetPie(QPainter & orc_Painter, const QBrush oc_PieBrush, const Qt::PenCapStyle oe_CapStyle,
                 const QRect & orc_PieCircleRect, const float32_t of32_PieWidth, const float32_t of32_TotalPixels,
                 const float32_t of32_Progress) const;
   void m_SetPieDot(QPainter & orc_Painter, const QColor & orc_PieDotColor, const QRect & orc_PieRect,
                    const float32_t of32_DotSize, const float32_t of32_TotalPixels,
                    const float32_t of32_Progress) const;
   void m_SetPieRim(QPainter & orc_Painter, const QBrush oc_RimBrush, const QRect & orc_RimCircleRect,
                    const float32_t of32_RimSize, const float32_t of32_TotalPixels,
                    const float32_t of32_ReductionFactor =  static_cast<float32_t>(0.0)) const;
   void m_SetInnerCircle(QPainter & orc_Painter, const QBrush oc_InnerCircleBrush,
                         const QRect & orc_InnerCircleRect) const;
   void m_SetPieText(QPainter & orc_Painter, const QColor & orc_TextColor, const QRect & orc_InnerCircleRect,
                     const QString & orc_DisplayString) const;
   void m_SetAllPieRects(QRect & orc_InnerCircleRect, QRect & orc_RimCircleRect, QRect & orc_PieCircleRect,
                         const float32_t of32_PieGap, const float32_t of32_PieSize, const float32_t of32_RimSize,
                         const float32_t of32_TotalPixels,
                         const float32_t of32_AdditionalFactor = static_cast<float32_t>(0.0)) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
