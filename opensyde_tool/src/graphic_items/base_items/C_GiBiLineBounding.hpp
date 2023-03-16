//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper class to create line bounding shape (header)

   Helper class to create line bounding shape (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIBILINEBOUNDING_H
#define C_GIBILINEBOUNDING_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVector>
#include <QPointF>
#include <QLineF>
#include <QPolygonF>
#include <QPainterPath>

#include "stwtypes.hpp"
#include "constants.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiLineBounding
{
public:
   C_GiBiLineBounding(const QVector<QPointF> & orc_Points, const float64_t & orf64_Width, const float64_t & orf64_InteractionPointWidth =
                         stw::opensyde_gui::mf64_INTERACTION_POINT_WIDTH);
   C_GiBiLineBounding(const QLineF & orc_Line, const float64_t & orf64_Width, const float64_t & orf64_InteractionPointWidth =
                         stw::opensyde_gui::mf64_INTERACTION_POINT_WIDTH);
   QPainterPath GetShape(void);
   static QPointF h_GetNorm(const QPointF & orc_Point);
   static QPointF h_AdaptVecToWidth(const QPointF & orc_Point, const float64_t & orf64_Length);
   static QPointF h_GetPerpendicular(const QPointF & orc_Point);
   static float64_t h_CrossProduct(const QPointF & orc_P1, const QPointF & orc_P2);

private:
   C_GiBiLineBounding();
   void m_AppendLineBoundingPointsTop(const int32_t & ors32_IndexStart, const int32_t & ors32_IndexEnd);
   QPointF m_AdaptVecToWidth(const QPointF & orc_Point) const;
   static float64_t mh_GetLength(const QPointF & orc_Point);
   void m_ConsolidateLine(const int32_t & ors32_RangeIndexStart, const int32_t & ors32_RangeIndexEnd);
   static bool mh_CalcIntersection(const QPointF & orc_Start1, const QPointF & orc_End1, const QPointF & orc_Start2,
                                   const QPointF & orc_End2, QPointF & orc_Intersection);
   static bool mh_Within(const float64_t & orf64_Eval, const float64_t & orf64_Start, const float64_t & orf64_End);

   const QVector<QPointF> mc_Points;
   const float64_t mf64_Width;
   const float64_t mf64_InteractionPointWidth;
   QPolygonF mc_Bounding;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
