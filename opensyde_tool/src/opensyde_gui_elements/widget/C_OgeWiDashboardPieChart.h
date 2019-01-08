//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Dashboard pie chart item (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWIDASHBOARDPIECHART_H
#define C_OGEWIDASHBOARDPIECHART_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeWiDashboardPieChart :
   public QWidget
{
public:
   explicit C_OgeWiDashboardPieChart(QWidget * const opc_Parent = NULL);

   void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode,
                        const bool oq_ShowUnit, const bool oq_ShowValue);
   void ReInitSize(void);
   void SetValue(const QString & orc_Value, const stw_types::sint32 os32_Progress2000000);
   void SetUnit(const QString & orc_Value);
   void SetTransparence(const stw_types::sintn osn_Value);

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   static const QColor mhc_Transparent;
   static const stw_types::float32 mhf32_Pi;
   QColor mc_ValueStart;
   QColor mc_ValueEnd;
   QColor mc_InnerBorderTop;
   QColor mc_InnerBorderBottom;
   QColor mc_InnerCircleTop;
   QColor mc_InnerCircleBottom;
   QColor mc_StringColor;
   bool mq_ValueCircle;
   stw_types::sint32 ms32_Value;
   const stw_types::sint32 ms32_Min;
   const stw_types::sint32 ms32_Max;
   QString mc_Unit;
   QString mc_Value;
   stw_types::sintn msn_ValueWidth;
   stw_types::sintn msn_BorderWidth;
   bool mq_ShowUnit;
   bool mq_ShowValue;
   QFont mc_Font;
   stw_types::sintn msn_Transparence;
   // Karsten Angang
   bool mq_TransparentBackground;
   stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style e_Style;
   bool mq_DarkMode;
   void m_SetPie(QPainter & orc_Painter, QBrush oc_PieBrush, const Qt::PenCapStyle oe_CapStyle,
                 const QRect & orc_PieCircleRect, const stw_types::float32 of32_PieWidth,
                 const stw_types::float32 of32_GesamtPixel, const stw_types::float32 of32_Progress) const;
   void m_SetPieDot(QPainter & orc_Painter, const QColor & orc_PieDotColor, const QRect & orc_PieRect,
                    const stw_types::float32 of32_DotSize, const stw_types::float32 of32_GesamtPixel,
                    const stw_types::float32 of32_Progress) const;
   void m_SetPieRim(QPainter & orc_Painter, QBrush oc_RimBrush, const QRect & orc_RimCircleRect,
                    const stw_types::float32 of32_RimSize, const stw_types::float32 of32_GesamtPixel,
                    const stw_types::float32 of32_ReductionFactor =  static_cast<stw_types::float32>(0.0)) const;
   void m_SetInnerCircle(QPainter & orc_Painter, QBrush oc_InnerCircleBrush, const QRect & orc_InnerCircleRect) const;
   void m_SetPieText(QPainter & orc_Painter, const QColor & orc_TextColor, const QRect & orc_InnerCircleRect,
                     QString oc_DisplayString) const;
   void m_SetAllPieRects(QRect & orc_InnerCircleRect, QRect & orc_RimCircleRect, QRect & orc_PieCircleRect,
                         const stw_types::float32 of32_PieGap, const stw_types::float32 of32_PieSize,
                         const stw_types::float32 of32_RimSize, const stw_types::float32 of32_GesamtPixel,
                         const stw_types::float32 of32_AdditionalFactor = static_cast<stw_types::float32>(0.0)) const;
   // Karsten Ende
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
