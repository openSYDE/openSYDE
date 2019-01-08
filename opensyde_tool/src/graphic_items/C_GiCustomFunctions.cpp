//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       A collection of useful functions for graphics items (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.08.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QGraphicsItem>

#include "stwtypes.h"
#include "C_GiCustomFunctions.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;
using namespace stw_types;

/* -- Module Global Constants ---------------------------------------------- */
const float64 C_GiCustomFunctions::hf64_SceneMinBorderSize = 6.0;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Standard function implementation of overloaded itemChange method

   Checks the new position of the item. It have to be bigger or equal than 0.

   \param[in,out] ope_Change        Indicator what changed
   \param[in,out] orc_Value         Value corresponding to change
   \param[in]     opc_GraphicsItem  Pointer to the actual graphics item
   \param[out]    opq_Changed       Optional flag if the function has the result adapted

   \return
   Adapted orc_Value

   \created     11.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QVariant C_GiCustomFunctions::h_ItemChange(const QGraphicsItem::GraphicsItemChange oe_Change,
                                           const QVariant & orc_Value, const QGraphicsItem * const opc_GraphicsItem,
                                           bool * const opq_Changed)
{
   QVariant c_Return = orc_Value;

   if (opq_Changed != NULL)
   {
      *opq_Changed = false;
   }

   if ((oe_Change == QGraphicsItem::ItemPositionChange) ||
       (oe_Change == QGraphicsItem::ItemPositionHasChanged))
   {
      QPointF c_NewPos = opc_GraphicsItem->mapToScene(orc_Value.toPointF());
      const QPointF c_SceneTopLeftOld = opc_GraphicsItem->mapToScene(opc_GraphicsItem->boundingRect().topLeft());
      const QPointF c_Diff = orc_Value.toPointF() - opc_GraphicsItem->pos();
      QPointF c_SceneTopLeftNew = c_SceneTopLeftOld + c_Diff;
      if (c_SceneTopLeftNew.x() < C_GiCustomFunctions::hf64_SceneMinBorderSize)
      {
         c_NewPos.setX(c_NewPos.x() + (C_GiCustomFunctions::hf64_SceneMinBorderSize - c_SceneTopLeftNew.x()));
         if (opq_Changed != NULL)
         {
            *opq_Changed = true;
         }
      }
      if (c_SceneTopLeftNew.y() < C_GiCustomFunctions::hf64_SceneMinBorderSize)
      {
         c_NewPos.setY(c_NewPos.y() + (C_GiCustomFunctions::hf64_SceneMinBorderSize - c_SceneTopLeftNew.y()));
         if (opq_Changed != NULL)
         {
            *opq_Changed = true;
         }
      }
      c_Return = opc_GraphicsItem->mapFromScene(c_NewPos);
   }

   return c_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks and adapt a position for the zero points

   \param[in,out] orc_Pos    Position which will be checked and adapted
   \param[in]     orc_Offset Optional parameter for offset

   \created     20.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiCustomFunctions::h_AdaptMouseRangePos(QPointF & orc_Pos, const QPointF & orc_Offset)
{
   const float64 f64_MinX = orc_Offset.x() + C_GiCustomFunctions::hf64_SceneMinBorderSize;
   const float64 f64_MinY = orc_Offset.y() + C_GiCustomFunctions::hf64_SceneMinBorderSize;

   if (orc_Pos.x() < f64_MinX)
   {
      orc_Pos.setX(f64_MinX);
   }
   if (orc_Pos.y() < f64_MinY)
   {
      orc_Pos.setY(f64_MinY);
   }
}
