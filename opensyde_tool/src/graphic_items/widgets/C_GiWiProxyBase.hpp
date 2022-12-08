//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Proxy widget with cumstom functions for graphics scenes to use widgets (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIWIPROXYBASE_HPP
#define C_GIWIPROXYBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsProxyWidget>
#include "C_GiBiSizeableItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiWiProxyBase :
   public QGraphicsProxyWidget,
   public C_GiBiSizeableItem
{
public:
   C_GiWiProxyBase(const float64_t of64_InitWidth, const float64_t of64_InitHeight,
                   QGraphicsItem * const opc_Parent = NULL);

   void Redraw(void) override;
   void TriggerMousePressEvent(QGraphicsSceneMouseEvent * const opc_Event);
   void TriggerMouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event);
   void TriggerMouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event);
   void TriggerMouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event);
   void TriggerKeyPressEvent(QKeyEvent * const opc_Event);
   void TriggerKeyReleaseEvent(QKeyEvent * const opc_Event);
   void TriggerFocusOutEvent(QFocusEvent * const opc_Event);
   void TriggerWheelEvent(QGraphicsSceneWheelEvent * const opc_Event);
   void TriggerHoverEnterEvent(QGraphicsSceneHoverEvent * const opc_Event);
   void TriggerHoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event);
   void TriggerHoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event);

private:
   bool mq_GrabbedByWidget;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
