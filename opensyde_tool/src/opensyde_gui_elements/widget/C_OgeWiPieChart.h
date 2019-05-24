//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for PieChart

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIPIECHART_H
#define C_OGEWIPIECHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.h"
#include "CSCLString.h"

#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_OgeWiPieChart;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OgeWiPieChart :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiPieChart(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeWiPieChart();

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   // Karsten Anfang
   /*
   void m_SetPie(QPainter & orc_Painter, QBrush oc_PieBrush, Qt::PenCapStyle oc_CapStyle, QRect & orc_PieCircleRect,
                 stw_types::float32 of32_PieWidth, stw_types::float32 of32_GesamtPixel);
   void m_SetPieDot(QPainter & orc_Painter, QColor oc_PieDotColor, QRect oc_PieRect,
                    stw_types::float32 of32_DotSize, stw_types::float32 of32_GesamtPixel);
   void m_SetPieRim(QPainter & orc_Painter, QBrush oc_RimBrush, QRect oc_RimCircleRect,
                    stw_types::float32 of32_RimSize, stw_types::float32 of32_GesamtPixel, stw_types::float32 of32_ReductionFactor = 0.0);
   void m_SetInnerCircle(QPainter & orc_Painter, QBrush oc_InnerCircleBrush, QRect oc_InnerCircleRect);
   void m_SetPieText(QPainter & orc_Painter, QColor oc_TextColor, QRect oc_InnerCircleRect);
   void m_SetAllPieRects(QRect &orc_InnerCircleRect, QRect &orc_RimCircleRect, QRect &orc_PieCircleRect,
                         stw_types::float32 of32_PieGap, stw_types::float32 of32_PieSize,
                         stw_types::float32 of32_RimSize, stw_types::float32 of32_GesamtPixel,
                         stw_types::float32 of32_AdditionalFactor = 0.0);
   int mc_Style;
   int mc_Mode;
   stw_types::uint16 mu16_BackgroundRed;           // Background Color Red Part
   stw_types::uint16 mu16_BackgroundGreen;         // Background Color Green Part
   stw_types::uint16 mu16_BackgroundBlue;          // Background Color Blue Part
   stw_types::uint16 mu16_BackgroundTransparnecy;  // Background Color Transparency
   */
   // Karstern Ende

   void m_OnSpinboxValuechanged(const stw_types::sintn osn_Value);
   //Avoid call
   C_OgeWiPieChart(const C_OgeWiPieChart &);
   C_OgeWiPieChart & operator =(const C_OgeWiPieChart &);

   Ui::C_OgeWiPieChart * mpc_Ui;

   stw_types::uint16 mu16_ValueInPercent;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace
#endif
