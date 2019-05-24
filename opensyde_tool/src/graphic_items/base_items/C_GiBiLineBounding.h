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

#include "stwtypes.h"
#include "constants.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiLineBounding
{
public:
   C_GiBiLineBounding(const QVector<QPointF> & orc_Points, const stw_types::float64 & orf64_Width, const stw_types::float64 & orf64_InteractionPointWidth =
                         stw_opensyde_gui::mf64_INTERACTION_POINT_WIDTH);
   C_GiBiLineBounding(const QLineF & orc_Line, const stw_types::float64 & orf64_Width, const stw_types::float64 & orf64_InteractionPointWidth =
                         stw_opensyde_gui::mf64_INTERACTION_POINT_WIDTH);
   QPainterPath GetShape(void);
   static QPointF GetNorm(const QPointF & orc_P);
   static QPointF AdaptVecToWidth(const QPointF & orc_P, const stw_types::float64 & orf64_Length);
   static QPointF GetPerpendicular(const QPointF & orc_P);
   static stw_types::float64 h_CrossProduct(const QPointF & orc_P1, const QPointF & orc_P2);

private:
   C_GiBiLineBounding();
   void AppendLineBoundingPointsTop(const stw_types::sint32 & ors32_IndexStart,
                                    const stw_types::sint32 & ors32_IndexEnd);
   QPointF AdaptVecToWidth(const QPointF & orc_P) const;
   static stw_types::float64 GetLength(const QPointF & orc_P);
   void m_ConsolidateLine(const stw_types::sint32 & ors32_RangeIndexStart,
                          const stw_types::sint32 & ors32_RangeIndexEnd);
   static bool m_CalcIntersection(const QPointF & orc_Start1, const QPointF & orc_End1, const QPointF & orc_Start2,
                                  const QPointF & orc_End2, QPointF & orc_Intersection);
   static bool mq_Within(const stw_types::float64 & orf64_Eval, const stw_types::float64 & orf64_Start,
                         const stw_types::float64 & orf64_End);

   const QVector<QPointF> mc_Points;
   const stw_types::float64 mf64_Width;
   const stw_types::float64 mf64_InteractionPointWidth;
   QPolygonF mc_Bounding;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
