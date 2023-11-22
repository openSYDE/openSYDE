//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A sizeable variant of QGraphicsRectItem (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIRECT_HPP
#define C_GIRECT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGraphicsRectItem>

#include "C_GiBiSizeableItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiRect :
   public QGraphicsRectItem,
   public C_GiBiSizeableItem
{
public:
   C_GiRect(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiRect(void) override;

   void Redraw(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
