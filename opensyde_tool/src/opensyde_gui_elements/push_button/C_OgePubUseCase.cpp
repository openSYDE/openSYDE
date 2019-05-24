//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design of main view for the main use cases

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgePubUseCase.h"

#include <QStyle>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QBitmap>

#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

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
C_OgePubUseCase::C_OgePubUseCase(QWidget * const opc_Parent) :
   C_OgePubStandard(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the element

   \param[in,out] opc_event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubUseCase::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);
   QBrush c_Brush;
   //QGraphicsDropShadowEffect *pc_shadow;
   QPixmap c_Pixmap;

   Q_UNUSED(opc_Event)

   c_Brush.setStyle(Qt::SolidPattern);

   if (this->isEnabled() == true)
   {
      // paint the main rectangle
      c_Brush.setColor(mc_STYLE_GUIDE_COLOR_2);
      c_Painter.setPen(mc_STYLE_GUIDE_COLOR_2);
      c_Painter.setBrush(c_Brush);
      c_Painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

      // mouse over element
      if (this->underMouse() == true)
      {
         // change background if hovered but not pressed
         if (this->isDown() == false)
         {
            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_3);
            c_Painter.setPen(mc_STYLE_GUIDE_COLOR_3);
            c_Painter.setBrush(c_Brush);
            c_Painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
         }

         // green line at bottom
         c_Brush.setColor(mc_STYLE_GUIDE_COLOR_21);
         c_Painter.setPen(mc_STYLE_GUIDE_COLOR_21);
         c_Painter.setBrush(c_Brush);
         c_Painter.drawRect(0, this->height() - 6, this->width(), 5); //height of line = 5px
      }

      // text
      c_Painter.setPen(mc_STYLE_GUIDE_COLOR_12);

      // icon
      c_Pixmap = this->icon().pixmap(50, 50);
   }
   else
   {
      c_Brush.setColor(mc_STYLE_GUIDE_COLOR_6);
      c_Painter.setPen(mc_STYLE_GUIDE_COLOR_6);
      c_Painter.setBrush(c_Brush);
      c_Painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

      // text
      c_Painter.setPen(mc_STYLE_GUIDE_COLOR_8);

      // icon
      c_Pixmap.load(mc_DisabledIconPath);
   }

   c_Painter.drawText(QPoint(125, 61), this->text());

   // draw the icon
   c_Painter.drawPixmap(40, 30, c_Pixmap);
}

void C_OgePubUseCase::SetDisabledIcon(const QString & orc_Path)
{
   this->mc_DisabledIconPath = orc_Path;
}
