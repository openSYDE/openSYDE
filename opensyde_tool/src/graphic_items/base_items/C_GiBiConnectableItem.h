//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interface for connectable item (header)

   Interface for connectable item (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIBICONNECTABLEITEM_H
#define C_GIBICONNECTABLEITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include <QPointF>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiConnectableItem :
   public QObject
{
   Q_OBJECT

public:
   C_GiBiConnectableItem(void);
   virtual void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const = 0;
   virtual void TriggerSigChangedGraphic(void);

   static void h_DistToLine(const QPointF & orc_LineStart, const QPointF & orc_LineEnd, const QPointF & orc_EvPoint,
                            stw_types::float64 * const opf64_Distance = NULL, QPointF * const opc_Projection = NULL,
                            stw_types::float64 * const opf64_RestrictPosition = NULL);
   static void h_DistToPoint(const QPointF & orc_Point1, const QPointF & orc_Point2,
                             stw_types::float64 & orf64_Distance);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangedGraphic(void);
   void SigChangedZOrder(void);

private:
   //Avoid call
   C_GiBiConnectableItem(const C_GiBiConnectableItem &);
   C_GiBiConnectableItem & operator =(const C_GiBiConnectableItem &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
