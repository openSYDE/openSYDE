//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A collection of useful functions for graphics items (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QGraphicsItem>

#include "stwtypes.hpp"
#include "C_OscUtils.hpp"
#include "C_GiCustomFunctions.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64_t C_GiCustomFunctions::hf64_SCENE_MIN_BORDER_SIZE = 6.0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Standard function implementation of overloaded itemChange method

   Checks the new position of the item. It have to be bigger or equal than 0.

   \param[in,out] oe_Change        Indicator what changed
   \param[in,out] orc_Value        Value corresponding to change
   \param[in]     opc_GraphicsItem Pointer to the actual graphics item
   \param[out]    opq_Changed      Optional flag if the function has adapted the result
   \param[in]     orc_Offset       Optional parameter for offset

   \return
   Adapted orc_Value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_GiCustomFunctions::h_ItemChange(const QGraphicsItem::GraphicsItemChange oe_Change,
                                           const QVariant & orc_Value, const QGraphicsItem * const opc_GraphicsItem,
                                           bool * const opq_Changed, const QPointF & orc_Offset)
{
   QVariant c_Return = orc_Value;

   if (opq_Changed != NULL)
   {
      *opq_Changed = false;
   }

   //Return value for ItemPositionHasChanged has no effect
   if (oe_Change == QGraphicsItem::ItemPositionChange)
   {
      const float64_t f64_MinHorizontal = orc_Offset.x() + C_GiCustomFunctions::hf64_SCENE_MIN_BORDER_SIZE;
      const float64_t f64_MinVertical = orc_Offset.y() + C_GiCustomFunctions::hf64_SCENE_MIN_BORDER_SIZE;
      //Scene boundaries check only makes sense in scene coordinates
      QPointF c_NewPos = opc_GraphicsItem->mapToScene(orc_Value.toPointF());
      const QPointF c_SceneTopLeftOld = opc_GraphicsItem->mapToScene(opc_GraphicsItem->boundingRect().topLeft());
      const QPointF c_DiffPrev = orc_Value.toPointF() - opc_GraphicsItem->pos();
      const QPointF c_SceneTopLeftNew = c_SceneTopLeftOld + c_DiffPrev;
      if (c_SceneTopLeftNew.x() < f64_MinHorizontal)
      {
         //Adapt current x by offset of top left corner to minimum position
         c_NewPos.setX(c_NewPos.x() + (f64_MinHorizontal - c_SceneTopLeftNew.x()));
         if (opq_Changed != NULL)
         {
            *opq_Changed = true;
         }
      }
      if (c_SceneTopLeftNew.y() < f64_MinVertical)
      {
         //Adapt current y by offset of top left corner to minimum position
         c_NewPos.setY(c_NewPos.y() + (f64_MinVertical - c_SceneTopLeftNew.y()));
         if (opq_Changed != NULL)
         {
            *opq_Changed = true;
         }
      }
      //Go back to expected item coordinates
      c_Return = opc_GraphicsItem->mapFromScene(c_NewPos);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks and adapt a position for the zero points

   \param[in,out] orc_Pos     Position which will be checked and adapted (scene coordinates)
   \param[in]     orc_Offset  Optional parameter for offset
   \param[out]    opq_Changed Optional flag if the function has adapted the result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiCustomFunctions::h_AdaptMouseRangePos(QPointF & orc_Pos, const QPointF & orc_Offset, bool * const opq_Changed)
{
   const float64_t f64_MinHorizontal = orc_Offset.x() + C_GiCustomFunctions::hf64_SCENE_MIN_BORDER_SIZE;
   const float64_t f64_MinVertical = orc_Offset.y() + C_GiCustomFunctions::hf64_SCENE_MIN_BORDER_SIZE;

   if (opq_Changed != NULL)
   {
      *opq_Changed = false;
   }

   if (orc_Pos.x() < f64_MinHorizontal)
   {
      orc_Pos.setX(f64_MinHorizontal);
      if (opq_Changed != NULL)
      {
         *opq_Changed = true;
      }
   }
   if (orc_Pos.y() < f64_MinVertical)
   {
      orc_Pos.setY(f64_MinVertical);
      if (opq_Changed != NULL)
      {
         *opq_Changed = true;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt size delta to keep aspect ratio

   \param[in] of64_AspectRatio       Aspect ratio to keep (Width/Height)
   \param[in] oe_AspectRatioMovement Movement direction
   \param[in] orc_Delta              Original delta

   \return
   Adapted delta
*/
//----------------------------------------------------------------------------------------------------------------------
QPointF C_GiCustomFunctions::h_AdaptDeltaForAspectRatio(const float64_t of64_AspectRatio,
                                                        const C_GiCustomFunctions::E_AspectRatioMovement oe_AspectRatioMovement,
                                                        const QPointF & orc_Delta)
{
   QPointF c_Delta = orc_Delta;

   switch (oe_AspectRatioMovement)
   {
   case eARM_BOTTOM_LEFT:
      // adapt the difference to the aspect ratio
      if ((c_Delta.x() * -1.0) > c_Delta.y())
      {
         if (C_OscUtils::h_IsFloat64NearlyEqual(of64_AspectRatio, 0.0) == false)
         {
            c_Delta.setY((c_Delta.x() * -1.0) / of64_AspectRatio);
         }
      }
      else
      {
         c_Delta.setX((c_Delta.y() * -1.0) * of64_AspectRatio);
      }
      break;
   case eARM_TOP_RIGHT:
      // adapt the difference to the aspect ratio
      if (c_Delta.x() > (c_Delta.y() * -1.0))
      {
         if (C_OscUtils::h_IsFloat64NearlyEqual(of64_AspectRatio, 0.0) == false)
         {
            c_Delta.setY((c_Delta.x() * -1.0) / of64_AspectRatio);
         }
      }
      else
      {
         c_Delta.setX((c_Delta.y() * -1.0) * of64_AspectRatio);
      }
      break;
   case eARM_TOP_LEFT:
      // adapt the difference to the aspect ratio
      if (c_Delta.x() < c_Delta.y())
      {
         if (C_OscUtils::h_IsFloat64NearlyEqual(of64_AspectRatio, 0.0) == false)
         {
            c_Delta.setY(c_Delta.x() / of64_AspectRatio);
         }
      }
      else
      {
         c_Delta.setX(c_Delta.y() * of64_AspectRatio);
      }
      break;
   case eARM_BOTTOM_RIGHT:
      // adapt the difference to the aspect ratio
      if (c_Delta.x() > c_Delta.y())
      {
         if (C_OscUtils::h_IsFloat64NearlyEqual(of64_AspectRatio, 0.0) == false)
         {
            c_Delta.setY(c_Delta.x() / of64_AspectRatio);
         }
      }
      else
      {
         c_Delta.setX(c_Delta.y() * of64_AspectRatio);
      }
      break;
   default:
      break;
   }

   return c_Delta;
}
