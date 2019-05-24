//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a progress status as point with configurable colors. (implementation)

   Width of widget must be match to height

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPen>
#include <QPainter>

#include "constants.h"

#include "C_OgeWiProgressBopperle.h"

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
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiProgressBopperle::C_OgeWiProgressBopperle(QWidget * const opc_Parent) :
   QWidget(opc_Parent)
{
   this->mc_BrushOuterCircle.setStyle(Qt::NoBrush);
   this->mc_BrushMainCircle.setStyle(Qt::SolidPattern);

   this->mc_PenOuterCircle.setColor(mc_STYLE_GUIDE_COLOR_38);
   this->HideMainBopperle();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the color of the outer circle of the progress bopperle

   \param[in]     orc_OuterColer         Color of the pen
   \param[in]     orc_InnerColor         Color of the brush
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBopperle::SetOuterBopperleColor(const QColor & orc_BorderColer, const QColor & orc_InnerColor)
{
   this->mc_BrushOuterCircle.setColor(orc_InnerColor);
   this->mc_PenOuterCircle.setColor(orc_BorderColer);

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the color of the inner circle of the progress bopperle

   \param[in]     orc_OuterColer         Color of the pen
   \param[in]     orc_InnerColor         Color of the brush
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBopperle::SetMainBopperleColor(const QColor & orc_BorderColer, const QColor & orc_InnerColor)
{
   this->mc_BrushMainCircle.setColor(orc_InnerColor);
   this->mc_PenMainCircle.setColor(orc_BorderColer);

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hides the inner circle of the bopperle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBopperle::HideMainBopperle(void)
{
   this->mc_BrushMainCircle.setColor(Qt::transparent);
   this->mc_PenMainCircle.setColor(Qt::transparent);

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the usage bar

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBopperle::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   QWidget::paintEvent(opc_Event);

   // make the bopperle smooth
   c_Painter.setRenderHint(QPainter::Antialiasing);

   // draw the outer circle
   c_Painter.setPen(this->mc_PenOuterCircle);
   c_Painter.setBrush(this->mc_BrushOuterCircle);
   c_Painter.drawEllipse(0, 0, this->width() - 1, this->height() - 1);

   // draw the main circle
   c_Painter.setPen(this->mc_PenMainCircle);
   c_Painter.setBrush(this->mc_BrushMainCircle);
   c_Painter.drawEllipse(4, 4, this->width() - 9, this->height() - 9);
}
