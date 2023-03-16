//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main button in material design of navigation bar for the main use cases

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgePubUseCaseNav.hpp"

#include <QPainter>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

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
C_OgePubUseCaseNav::C_OgePubUseCaseNav(QWidget * const opc_Parent) :
   C_OgePubStandard(opc_Parent),
   q_LastButton(false)
{
   this->setCheckable(true);
   this->setChecked(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Draws the element

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubUseCaseNav::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);
   QBrush c_Brush;
   const QPixmap c_Pixmap = this->icon().pixmap(44, 45);

   const QColor c_COL_BACKGROUND(71, 71, 99, 255);
   const QColor c_COL_BACKGROUND_DARK(19, 19, 45, 255);

   Q_UNUSED(opc_Event)

   // paint the main rectangle
   c_Brush.setColor(c_COL_BACKGROUND);
   c_Brush.setStyle(Qt::SolidPattern);
   c_Painter.setPen(c_COL_BACKGROUND);
   c_Painter.setBrush(c_Brush);
   c_Painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

   if (this->isChecked() == true)
   {
      c_Brush.setColor(QColor(94, 163, 67));
      c_Painter.setPen(QColor(94, 163, 67));
      c_Painter.setBrush(c_Brush);
      c_Painter.drawRect(0, 0, 5, this->height() - 1);
   }

   //draw line
   c_Painter.setPen(c_COL_BACKGROUND_DARK);
   c_Painter.drawLine(0, 0, this->width() - 1, 0);

   //buttom line
   if (q_LastButton == true)
   {
      c_Painter.drawLine(0, this->height() - 1, this->width() - 1, this->height() - 1);
   }

   c_Painter.setPen(QColor(255, 255, 255));
   c_Painter.drawText(QPoint(80, 43), this->text());

   // draw the icon
   c_Painter.drawPixmap(25, 22, c_Pixmap);
}
