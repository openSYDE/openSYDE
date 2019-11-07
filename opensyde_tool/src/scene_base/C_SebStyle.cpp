//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Style for scene rubber band (implementation)

   Style for scene rubber band

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include <QStyleOption>
#include "constants.h"
#include "C_SebStyle.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Style Base style
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebStyle::C_SebStyle(QStyle * const opc_Style) :
   QProxyStyle(opc_Style)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten draw control

   Here: Draw custom rubber band

   \param[in]     oe_Element  Control type
   \param[in]     opc_Option  Drawing options
   \param[in,out] opc_Painter Used painter
   \param[in,out] opc_Widget  Widget to draw in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebStyle::drawControl(const QStyle::ControlElement oe_Element, const QStyleOption * const opc_Option,
                             QPainter * const opc_Painter, const QWidget * const opc_Widget) const
{
   if ((oe_Element == QStyle::CE_RubberBand) && (opc_Option->rect.isNull() == false))
   {
      QColor c_BackgroundColor = mc_STYLE_GUIDE_COLOR_21;

      opc_Painter->save();
      //Background
      //20% transparency
      c_BackgroundColor.setAlpha(255 / 5);
      opc_Painter->setBrush(c_BackgroundColor);
      opc_Painter->setPen(Qt::NoPen);
      //One step inside because otherwise there were unclean paint areas
      opc_Painter->drawRect(opc_Option->rect.adjusted(0, 0, 0, 0));
      //Border
      opc_Painter->setBrush(Qt::NoBrush);
      opc_Painter->setPen(QPen(mc_STYLE_GUIDE_COLOR_21, 2.0, Qt::SolidLine));
      //Only draw border if adaption does not invert the rectangle
      if ((opc_Option->rect.width() > 1) && (opc_Option->rect.height() > 1))
      {
         //Move rect so that complete drawn line is inside / on top of rect (otherwise we draw outside!)
         opc_Painter->drawRect(opc_Option->rect.adjusted(1, 1, -1, -1));
      }
      opc_Painter->restore();
   }
   else
   {
      QProxyStyle::drawControl(oe_Element, opc_Option, opc_Painter, opc_Widget);
   }
}
