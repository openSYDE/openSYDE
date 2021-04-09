//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Temporary dashed line for connector creation (implementation)

   Temporary dashed line for connector creation

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPen>
#include <QCursor>

#include "C_GiLiTemporaryLine.h"

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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiTemporaryLine::C_GiLiTemporaryLine(QGraphicsItem * const opc_Parent) :
   QGraphicsLineItem(
      opc_Parent)
{
   QPen c_Pen = this->pen();

   c_Pen.setStyle(Qt::DashLine);
   this->setPen(c_Pen);
   this->setLine(QLineF(0., 0., 0., 0.));
   this->setCursor(static_cast<QCursor>(Qt::UpArrowCursor));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiTemporaryLine::~C_GiLiTemporaryLine()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle mouse position change

   \param[in] orc_Pos New mouse position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiTemporaryLine::UpdateP2(const QPointF & orc_Pos)
{
   QLineF c_Line = this->line();

   c_Line.setP2(this->mapFromScene(orc_Pos));
   this->setLine(c_Line);
}
