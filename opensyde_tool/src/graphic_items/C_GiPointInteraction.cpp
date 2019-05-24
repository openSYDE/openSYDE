//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom interaction point (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFlags>
#include <QTransform>
#include <QStyleOptionGraphicsItem>

#include "C_SdTopologyScene.h"
#include "C_GiPointInteraction.h"
#include "constants.h"
#include "gitypes.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent   Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiPointInteraction::C_GiPointInteraction(QGraphicsItem * const opc_Parent) :
   QGraphicsEllipseItem(opc_Parent)
{
   this->setFlag(ItemSendsGeometryChanges);

   this->setPen(Qt::NoPen);
   this->setBrush(QBrush(mc_STYLE_GUIDE_COLOR_21));
   this->setRect(-(mf64_INTERACTION_POINT_WIDTH / 2.0), -(mf64_INTERACTION_POINT_WIDTH / 2.0),
                 mf64_INTERACTION_POINT_WIDTH, mf64_INTERACTION_POINT_WIDTH);

   // It has to be on the highest level. It shall be always be visible.
   this->setZValue(mf64_ZORDER_MAX);

   this->setCursor(QCursor(Qt::SizeBDiagCursor));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiPointInteraction::~C_GiPointInteraction()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiPointInteraction::type() const
{
   return msn_GRAPHICS_ITEM_POINTINTERACTION;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the parameter position is inside the bounding rect

   The original function 'contains' use the local coordinates

   \param[in]  orc_Pos     Position to check

   \return  true if the poiont is inside the rectangle, false if not
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiPointInteraction::IsPointInside(const QPointF & orc_Pos) const
{
   return this->sceneBoundingRect().contains(orc_Pos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal for update of current scaling

   \param[in] orc_Transform Current scaling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiPointInteraction::UpdateTransform(const QTransform & orc_Transform)
{
   if (orc_Transform.isInvertible() == true)
   {
      this->prepareGeometryChange();
      this->setTransform(orc_Transform.inverted());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Draw interaction point

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiPointInteraction::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                                 QWidget * const opc_Widget)
{
   //Deactivate selection
   QStyleOptionGraphicsItem c_Option = (*opc_Option);

   //lint -e{64,1013,1058,1514} Qt feature
   c_Option.state.setFlag(QStyle::State_Selected, false);
   QGraphicsEllipseItem::paint(opc_Painter, &c_Option, opc_Widget);
}
