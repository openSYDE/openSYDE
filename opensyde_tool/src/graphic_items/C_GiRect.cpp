//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A sizeable variant of QGraphicsRectItem (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPen>
#include "C_GiRect.hpp"

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

   \param[in]     orc_Rect     Initial size of the item
   \param[in,out] opc_Parent   Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiRect::C_GiRect(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent) :
   QGraphicsRectItem(orc_Rect, opc_Parent),
   C_GiBiSizeableItem(orc_Rect.width(), orc_Rect.height())
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiRect::~C_GiRect()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the drawing of the rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiRect::Redraw(void)
{
   QRectF c_Rect = this->GetSizeRect();
   const QPen c_Pen = this->pen();
   const float64_t f64_PenWidth = static_cast<float64_t>(c_Pen.width());

   //Resize
   //Move top left and bottom right inside by the pen width
   c_Rect.setTopLeft(c_Rect.topLeft() + QPointF(f64_PenWidth / 2.0, f64_PenWidth / 2.0));
   c_Rect.setBottomRight(c_Rect.bottomRight() - QPointF(f64_PenWidth / 2.0, f64_PenWidth / 2.0));
   this->setRect(c_Rect);
}
