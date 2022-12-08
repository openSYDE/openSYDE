//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for PieChart

   Widget for PieChart

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiPieChart.hpp"
#include "C_SclString.hpp"
#include "ui_C_OgeWiPieChart.h"
#include "constants.hpp"

#include <QPainter>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
C_OgeWiPieChart::C_OgeWiPieChart(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiPieChart)
{
   this->mpc_Ui->setupUi(this);

   mu16_ValueInPercent = 0;

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBox, static_cast<void (QSpinBox::*)(
                                                    int32_t)>(&QSpinBox::valueChanged), this,
           &C_OgeWiPieChart::m_OnSpinboxValuechanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiPieChart::~C_OgeWiPieChart()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the element

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiPieChart::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   c_Painter.save();

   QBrush c_BackgroundBrush(QColor(212, 212, 212));
   QBrush c_ForegroundBrush(QColor(245, 247, 248));
   QBrush c_PieBrush(QColor(247, 147, 30));
   QPen c_Pen(QColor(91, 91, 91));
   QFont c_Font;
   QRect c_Rect(50, 50, 200, 200);
   int16_t s16_StartAngle;
   float32_t f32_SpanAngle;
   stw::scl::C_SclString c_String;

   c_BackgroundBrush.setColor(QColor(2, 155, 136));
   c_ForegroundBrush.setColor(QColor(40, 40, 40));
   c_PieBrush.setColor(QColor(230, 0, 0));
   c_Pen.setColor(QColor(176, 6, 176));
   c_Font.setFamily("Segoe UI");
   c_Font.setPointSize(44);

   c_Painter.setPen(Qt::NoPen);
   c_Painter.setRenderHint(QPainter::Antialiasing);

   //background cycle
   c_Painter.setBrush(c_BackgroundBrush);
   c_Painter.drawEllipse(50, 50, 200, 200);

   //pie
   c_Rect.setRect(50, 50, 200, 200);
   s16_StartAngle = 90 * 16;
   f32_SpanAngle = (-1.0F * (360.0F / 100.0F) * static_cast<float32_t>(mu16_ValueInPercent)  * 16.0F);

   c_Painter.setBrush(c_PieBrush);
   c_Painter.drawPie(c_Rect, s16_StartAngle, static_cast<int32_t>(f32_SpanAngle));

   //foreground cycle
   c_Painter.setBrush(c_ForegroundBrush);
   c_Painter.drawEllipse(60, 60, 180, 180);

   //text
   c_Painter.setPen(c_Pen);
   c_Painter.setFont(c_Font);
   c_String = stw::scl::C_SclString::IntToStr(mu16_ValueInPercent);
   c_String += "%";
   c_Painter.drawText(c_Rect, static_cast<int32_t>(Qt::AlignCenter), c_String.c_str());

   c_Painter.restore();
   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
void stw::opensyde_gui::C_OgeWiPieChart::m_OnSpinboxValuechanged(const int32_t os32_Value)
{
   mu16_ValueInPercent = static_cast<uint16_t>(os32_Value);
   this->repaint();
}
