//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Visualization and functionality of a port of a node (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.08.2016  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include <QLinearGradient>
#include "C_GiPort.h"
#include "constants.h"
#include "C_OSCUtils.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui;
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     22.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiPort::C_GiPort(QGraphicsItem * const opc_Parent) :
   QGraphicsItem(opc_Parent),
   mc_BoundingRect(QRectF(0.0, 0.0, 80.0, 15.0)),
   ms32_RegisteredConnectors(0),
   mq_DrawBoder(false),
   mq_DrawWhiteFilter(false)
{
   this->setFlag(ItemIsMovable);
   //   this->setFlag(ItemStacksBehindParent);

   // fill the vector
   this->mc_Points.append(QPointF(0.0, 15.0));
   this->mc_Points.append(QPointF(80.0, 15.0));
   this->mc_Points.append(QPointF(70.0, 2.0));
   this->mc_Points.append(QPointF(10.0, 2.0));

   //shadow
   this->SetShadow();
   //this->setZValue(100000.0);
   this->setVisible(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     30.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiPort::~C_GiPort(void)
{
   this->mpc_Shadow = NULL;
   //lint -e{1740}  no memory leak because of the parent of mpc_Shadow and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   boundingRect
            Returns an estimate of the area painted by the item

   \created     04.08.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
QRectF C_GiPort::boundingRect() const
{
   return this->mc_BoundingRect;
}

//-----------------------------------------------------------------------------
/*!
   \brief   boundingRect


   \created     04.08.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_GiPort::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                     QWidget * const opc_Widget)
{
   Q_UNUSED(opc_Option)
   Q_UNUSED(opc_Widget)

   QFont c_Font;
   QLinearGradient c_Gradient;
   QVector<QPointF> c_PointsSurrounding;
   QVector<QPointF> c_PointsInside;

   c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
   c_Font.setPixelSize(c_Font.pointSize());

   //rounded rect
   QRectF c_Rect = this->boundingRect();

   c_Gradient.setColorAt(0.0, QColor(97, 97, 147));
   c_Gradient.setColorAt(1.0, QColor(127, 127, 168));

   c_Gradient.setStart(0.0, 0.0);
   c_Gradient.setFinalStop(0.0, c_Rect.height());

   //port above
   opc_Painter->setPen(Qt::NoPen);

   if (this->mq_DrawBoder == true)
   {
      //Polygon underneath Port, locking like a sorrounding
      if (this->mq_DrawWhiteFilter == false)
      {
         opc_Painter->setBrush(QBrush(mc_STYLE_GUIDE_COLOR_3));
         c_PointsSurrounding.append(QPointF(this->mc_Points[0].x() + 0.6, this->mc_Points[0].y() - 1.0));
         c_PointsSurrounding.append(QPointF(this->mc_Points[1].x() - 0.6, this->mc_Points[1].y() - 1.0));
         c_PointsSurrounding.append(QPointF(this->mc_Points[2].x(), this->mc_Points[2].y()));
         c_PointsSurrounding.append(QPointF(this->mc_Points[3].x(), this->mc_Points[3].y()));
         opc_Painter->drawPolygon(QPolygonF(c_PointsSurrounding));

         c_PointsInside.append(QPointF(this->mc_Points[0].x() + 2.0, this->mc_Points[0].y()));
         c_PointsInside.append(QPointF(this->mc_Points[1].x() - 2.0, this->mc_Points[1].y()));
         c_PointsInside.append(QPointF(this->mc_Points[2].x() - 0.46, this->mc_Points[2].y() + 2.0));
         c_PointsInside.append(QPointF(this->mc_Points[3].x() + 0.46, this->mc_Points[3].y() + 2.0));
      }
      else
      {
         // fill the vector
         c_PointsInside.append(QPointF(this->mc_Points[0].x(), this->mc_Points[0].y()));
         c_PointsInside.append(QPointF(this->mc_Points[1].x(), this->mc_Points[1].y()));
         c_PointsInside.append(QPointF(this->mc_Points[2].x(), this->mc_Points[2].y()));
         c_PointsInside.append(QPointF(this->mc_Points[3].x(), this->mc_Points[3].y()));
      }
   }
   else
   {
      // fill the vector
      c_PointsInside.append(QPointF(this->mc_Points[0].x(), this->mc_Points[0].y()));
      c_PointsInside.append(QPointF(this->mc_Points[1].x(), this->mc_Points[1].y()));
      c_PointsInside.append(QPointF(this->mc_Points[2].x(), this->mc_Points[2].y()));
      c_PointsInside.append(QPointF(this->mc_Points[3].x(), this->mc_Points[3].y()));
   }
   opc_Painter->setBrush(c_Gradient);
   opc_Painter->drawPolygon(QPolygonF(c_PointsInside));

   if (this->mq_DrawWhiteFilter == true)
   {
      // draw the white filter
      QColor c_Color = mc_STYLE_GUIDE_COLOR_0;
      QBrush c_Brush;

      // half transparent
      c_Color.setAlpha(127);
      c_Brush.setColor(c_Color);
      c_Brush.setStyle(Qt::SolidPattern);

      opc_Painter->setBrush(c_Brush);
      opc_Painter->setPen(Qt::NoPen);

      opc_Painter->drawPolygon(QPolygonF(mc_Points));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Find closest point in shape to scene position

   \param[in]  orc_ScenePoint Scene position
   \param[out] orc_Closest    Closest point in shape

   \created     24.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------

void C_GiPort::FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const
{
   const QPointF c_Offset(0.0, (-1.0 * (this->boundingRect().topLeft().y() - this->boundingRect().center().y())) - 1.0);

   orc_Closest = orc_ScenePoint;
   if (mc_Points.size() == 4)
   {
      C_GiBiConnectableItem::h_DistToLine(this->mapToScene(mc_Points[2] + c_Offset), this->mapToScene(
                                             mc_Points[3] + c_Offset), orc_ScenePoint, NULL, &orc_Closest, NULL);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register connector

   \created     26.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------

void C_GiPort::AddConnectorToRegistry(void)
{
   ++ms32_RegisteredConnectors;
   if (ms32_RegisteredConnectors > 0)
   {
      this->setVisible(true);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove connector from registry

   \created     26.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------

void C_GiPort::RemoveConnectorFromRegistry(void)
{
   --ms32_RegisteredConnectors;
   if (ms32_RegisteredConnectors <= 0)
   {
      this->setVisible(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Prepare for unregister

   \created     26.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------

void C_GiPort::TemporaryUnregister(void)
{
   if (this->ms32_RegisteredConnectors <= 1)
   {
      this->setOpacity(0.5);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Abort prepare for unregister

   \created     26.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------

void C_GiPort::AbortTemporaryUnregister(void)
{
   this->setOpacity(1.0);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Stretches the port for a specific difference distance

   \param[in]   of64_Difference     Stretch distance

   \created     19.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiPort::StretchPort(const float64 of64_Difference)
{
   if ((this->mc_Points.size() == 4) &&
       (C_OSCUtils::h_IsFloat64NearlyEqual(of64_Difference, 0.0) == false))
   {
      float64 f64_DiffHalf = of64_Difference / 2.0;

      this->mc_Points[1].setX(this->mc_Points[1].x() + f64_DiffHalf);
      this->mc_Points[2].setX(this->mc_Points[2].x() + f64_DiffHalf);

      this->mc_Points[0].setX(this->mc_Points[0].x() - f64_DiffHalf);
      this->mc_Points[3].setX(this->mc_Points[3].x() - f64_DiffHalf);

      this->mc_BoundingRect.setX(this->mc_Points[0].x());
      this->mc_BoundingRect.setWidth(this->mc_Points[1].x() - this->mc_Points[0].x());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Resets the size of the port to default values

   \created     21.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiPort::ResizePortToDefault()
{
   if (this->mc_Points.size() == 4)
   {
      this->mc_Points[0] = QPointF(0.0, 15.0);
      this->mc_Points[1] = QPointF(80.0, 15.0);
      this->mc_Points[2] = QPointF(70.0, 2.0);
      this->mc_Points[3] = QPointF(10.0, 2.0);

      this->mc_BoundingRect = QRectF(0.0, 0.0, 80.0, 15.0);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reset port counter and port visibility to default

   \created     02.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiPort::ResetConnectorCount(void)
{
   this->ms32_RegisteredConnectors = 0;
   this->setVisible(false);
   this->setOpacity(1.0);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activates the drawing of the border line

   \param[in]     oq_Active       Flag for activating or deactivating the border

   \created     22.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiPort::SetDrawBorder(const bool oq_Active)
{
   this->mq_DrawBoder = oq_Active;
   this->SetShadow();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activates the drawing of the white filter

   \param[in]     oq_Active       Flag for activating or deactivating the white filter

   \created     22.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiPort::SetDrawWhiteFilter(const bool oq_Active)
{
   this->mq_DrawWhiteFilter = oq_Active;
   this->update();
}

//-----------------------------------------------------------------------------
/*!
   \brief   short description of function

   long description of function within several lines

   \param[in]     ou8_Aa         input parameter description
   \param[out]    opu32_Bb       output parameter description
   \param[in,out] opu16_Cc       input/output parameter description

   \return
   possible return value(s) and description

   \created     dd.mm.yyyy  STW/J.Doe
*/
//-----------------------------------------------------------------------------
void C_GiPort::SetShadow()
{
   if (this->mq_DrawBoder == false)
   {
      this->mpc_Shadow = new QGraphicsDropShadowEffect();
      this->mpc_Shadow->setBlurRadius(30.0);
      this->mpc_Shadow->setColor(mc_HOVER_SHADOW_COLOR);
      this->mpc_Shadow->setOffset(0.0);
      this->setGraphicsEffect(this->mpc_Shadow);
   }
   else
   {
      this->mpc_Shadow = new QGraphicsDropShadowEffect();
      this->mpc_Shadow->setBlurRadius(0.0);
      this->mpc_Shadow->setColor(mc_HOVER_SHADOW_COLOR);
      this->mpc_Shadow->setOffset(0.0);
      this->setGraphicsEffect(this->mpc_Shadow);
   }
}
