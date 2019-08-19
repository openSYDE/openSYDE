//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A collection of useful functions for graphics items (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILICUSTOMFUNCTIONS_H
#define C_GILICUSTOMFUNCTIONS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVariant>
#include <QGraphicsItem>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiCustomFunctions
{
public:
   enum E_AspectRatioMovement
   {
      eARM_TOP_LEFT,
      eARM_TOP_RIGHT,
      eARM_BOTTOM_LEFT,
      eARM_BOTTOM_RIGHT
   };

   static QVariant h_ItemChange(const QGraphicsItem::GraphicsItemChange oe_Change, const QVariant & orc_Value,
                                const QGraphicsItem * const opc_GraphicsItem, bool * const opq_Changed = NULL,
                                const QPointF & orc_Offset = QPointF(0.0, 0.0));
   static void h_AdaptMouseRangePos(QPointF & orc_Pos, const QPointF & orc_Offset = QPointF(0.0, 0.0),
                                    bool * const opq_Changed = NULL);
   static QPointF h_AdaptDeltaForAspectRatio(const stw_types::float64 of64_AspectRatio,
                                             const E_AspectRatioMovement oe_AspectRatioMovement,
                                             const QPointF & orc_Delta);

   static const stw_types::float64 hf64_SceneMinBorderSize;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
