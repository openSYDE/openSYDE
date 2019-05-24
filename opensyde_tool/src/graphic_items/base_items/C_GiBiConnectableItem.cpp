//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interface for connectable item (implementation)

   Interface for connectable item

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>

#include "C_OSCUtils.h"
#include "C_GiBiConnectableItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_types;

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
C_GiBiConnectableItem::C_GiBiConnectableItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Helper function to trigger changed graphic manually to avoid signals on every possible change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiConnectableItem::TriggerSigChangedGraphic()
{
   Q_EMIT this->ChangedGraphic();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate distance to segment of line

   Look at http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
   Abstract:
   1. Use Form f(t) = orc_LineStart + t * (orc_LineEnd - orc_LineStart)
      t = Variable indicating progress on line, e.g 0 = Start, 1 = End
   2. Now project point on line and restrict u to [0,1]
   3. Then calculate the resulting point f(t)
   4. Calculate distance of resulting point to orc_EvPoint

   \param[in]  orc_LineStart          Start of line segment
   \param[in]  orc_LineEnd            End of line segment
   \param[in]  orc_EvPoint            Point to claculate distance to
   \param[out] opf64_Distance         Optional pointer for distance as result
   \param[out] opc_Projection         Optional pointer for projected point on line
   \param[out] opf64_RestrictPosition Optional pointer for relative position of projected point on line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiConnectableItem::h_DistToLine(const QPointF & orc_LineStart, const QPointF & orc_LineEnd,
                                         const QPointF & orc_EvPoint, float64 * const opf64_Distance,
                                         QPointF * const opc_Projection, float64 * const opf64_RestrictPosition)
{
   const QPointF c_Diff = orc_LineEnd - orc_LineStart;
   const float64 f64_LengthSquared = (c_Diff.x() * c_Diff.x()) + (c_Diff.y() * c_Diff.y());

   //Check if line has length
   if (C_OSCUtils::h_IsFloat64NearlyEqual(f64_LengthSquared, 0.0) == true)
   {
      if (opf64_Distance != NULL)
      {
         h_DistToPoint(orc_LineStart, orc_EvPoint, *opf64_Distance);
      }
      if (opc_Projection != NULL)
      {
         *opc_Projection = orc_LineStart;
      }
   }
   else
   {
      float64 f64_Tmp1;
      float64 f64_RestrictPosition;
      QPointF c_Projection;
      //Projected t position on line in range of 0 to 1
      f64_Tmp1 = QPointF::dotProduct(orc_EvPoint - orc_LineStart, c_Diff);
      f64_Tmp1 = f64_Tmp1 / f64_LengthSquared;
      f64_RestrictPosition = std::max(0., std::min(1., f64_Tmp1));
      if (opf64_RestrictPosition != NULL)
      {
         *opf64_RestrictPosition = f64_RestrictPosition;
      }
      //Calculate position of relative t value
      c_Projection = orc_LineStart + (f64_RestrictPosition * c_Diff);
      if (opc_Projection != NULL)
      {
         *opc_Projection = c_Projection;
      }
      //Calculate distance to projection
      if (opf64_Distance != NULL)
      {
         h_DistToPoint(orc_EvPoint, c_Projection, *opf64_Distance);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate distance point to point

   \param[in]  orc_Point1     Start
   \param[in]  orc_Point2     End
   \param[out] orf64_Distance Distance as result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiConnectableItem::h_DistToPoint(const QPointF & orc_Point1, const QPointF & orc_Point2,
                                          stw_types::float64 & orf64_Distance)
{
   const QPointF c_Diff = orc_Point1 - orc_Point2;

   orf64_Distance = std::sqrt((c_Diff.x() * c_Diff.x()) + (c_Diff.y() * c_Diff.y()));
}
