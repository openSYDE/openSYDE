//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Visualization and functionality of a port of a node (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIPORT_HPP
#define C_GILIPORT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsDropShadowEffect>

#include "stwtypes.hpp"
#include "C_GiBiConnectableItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiPort :
   public C_GiBiConnectableItem,
   public QGraphicsItem
{
public:
   C_GiPort(QGraphicsItem * const opc_Parent = NULL);
   ~C_GiPort(void) override;

   QRectF boundingRect() const override;

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget) override;

   void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const override;
   void AddConnectorToRegistry(void);
   void RemoveConnectorFromRegistry(void);
   void TemporaryUnregister(void);
   void AbortTemporaryUnregister(void);
   void StretchPort(const float64_t of64_Difference);
   void ResizePortToDefault(void);
   void ResetConnectorCount(void);
   void SetDrawBorder(const bool oq_Active);
   void SetDrawWhiteFilter(const bool oq_Active);
   void SetShadow();

private:
   //Avoid call
   C_GiPort(const C_GiPort &);
   C_GiPort & operator =(const C_GiPort &) &; //lint !e1511 //we want to hide the base func.

   QGraphicsDropShadowEffect * mpc_Shadow;
   QVector<QPointF> mc_Points;
   QRectF mc_BoundingRect;
   int32_t ms32_RegisteredConnectors;
   bool mq_DrawBoder;
   bool mq_DrawWhiteFilter;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
