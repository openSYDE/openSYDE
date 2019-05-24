//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Proxy widget with cumstom functions for graphics scenes to use widgets (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIWIPROXYBASE_H
#define C_GIWIPROXYBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsProxyWidget>
#include "C_GiBiSizeableItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiWiProxyBase :
   public QGraphicsProxyWidget,
   public C_GiBiSizeableItem
{
public:
   C_GiWiProxyBase(const stw_types::float64 of64_InitWidth, const stw_types::float64 of64_InitHeight,
                   QGraphicsItem * const opc_Parent = NULL);

   virtual void Redraw(void);
   void TriggerMousePressEvent(QGraphicsSceneMouseEvent * const opc_Event);
   void TriggerMouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event);
   void TriggerMouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event);
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
} //end of namespace

#endif
