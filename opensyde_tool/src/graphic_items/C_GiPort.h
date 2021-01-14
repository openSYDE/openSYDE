//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Visualization and functionality of a port of a node (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIPORT_H
#define C_GILIPORT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsDropShadowEffect>

#include "stwtypes.h"
#include "C_GiBiConnectableItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiPort :
   public C_GiBiConnectableItem,
   public QGraphicsItem
{
public:
   C_GiPort(QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiPort(void);

   virtual QRectF boundingRect() const;

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget);

   virtual void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const;
   void AddConnectorToRegistry(void);
   void RemoveConnectorFromRegistry(void);
   void TemporaryUnregister(void);
   void AbortTemporaryUnregister(void);
   void StretchPort(const stw_types::float64 of64_Difference);
   void ResizePortToDefault(void);
   void ResetConnectorCount(void);
   void SetDrawBorder(const bool oq_Active);
   void SetDrawWhiteFilter(const bool oq_Active);
   void SetShadow();

private:
   //Avoid call
   C_GiPort(const C_GiPort &);
   C_GiPort & operator =(const C_GiPort &); //lint !e1511 //we want to hide the base func.

   QGraphicsDropShadowEffect * mpc_Shadow;
   QVector<QPointF> mc_Points;
   QRectF mc_BoundingRect;
   stw_types::sint32 ms32_RegisteredConnectors;
   bool mq_DrawBoder;
   bool mq_DrawWhiteFilter;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
