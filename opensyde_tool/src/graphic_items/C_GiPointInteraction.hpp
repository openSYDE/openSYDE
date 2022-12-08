//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom interaction point (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIPOINTINTERACTION_H
#define C_GILIPOINTINTERACTION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsEllipseItem>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiPointInteraction :
   public QGraphicsEllipseItem
{
public:
   C_GiPointInteraction(QGraphicsItem * const opc_Parent = NULL);
   ~C_GiPointInteraction() override;

   int32_t type() const override;
   bool IsPointInside(const QPointF & orc_Pos) const;
   void UpdateTransform(const QTransform & orc_Transform);

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget) override;

private:
   //Avoid call
   C_GiPointInteraction(const C_GiPointInteraction &);
   C_GiPointInteraction & operator =(const C_GiPointInteraction &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
