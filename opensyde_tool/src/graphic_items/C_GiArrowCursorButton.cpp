//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       A button displaying an arrow cursor (implementation)

   A button displaying an arrow cursor

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.08.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPixmap>

#include "stwtypes.h"
#include "gitypes.h"
#include "C_GtGetText.h"
#include "C_GiArrowCursorButton.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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

   \created     29.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiArrowCursorButton::C_GiArrowCursorButton(QGraphicsItem * const opc_Parent) :
   QGraphicsSvgItem("://images/graphic_items/IconNodeConnect.svg", opc_Parent),
   C_GiBiCustomToolTip(),
   mpc_ConnectedNode(NULL)
{
   this->setFlag(ItemIsSelectable);
   this->setScale(1.0);
   // It has to be on the highest level. It shall be always be visible.
   this->setZValue(mf64_ZORDER_MAX);
   this->setCursor(Qt::ArrowCursor);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     29.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiArrowCursorButton::~C_GiArrowCursorButton()
{
   mpc_ConnectedNode = NULL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Stop listening to node position changes

   \created     30.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiArrowCursorButton::DetachNode(void)
{
   if (mpc_ConnectedNode != NULL)
   {
      //lint -e{64, 918, 1025, 1703}  false positive because of C++11 use of Qt
      disconnect(mpc_ConnectedNode, &C_GiNode::ChangedGraphic, this, &C_GiArrowCursorButton::m_OnNodeChange);
      mpc_ConnectedNode = NULL;
   }
   this->setVisible(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start listening to node position changes

   Implicit: Detach last node

   \param[in] opc_Node New node to connect to

   \created     30.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiArrowCursorButton::AttachToNode(const C_GiNode * const opc_Node)
{
   if (opc_Node != NULL)
   {
      DetachNode();
      if (opc_Node->GetValid() == true)
      {
         mpc_ConnectedNode = opc_Node;
         //lint -e{64, 918, 1025, 1703}  false positive because of C++11 use of Qt
         connect(mpc_ConnectedNode, &C_GiNode::ChangedGraphic, this, &C_GiArrowCursorButton::m_OnNodeChange);
         m_OnNodeChange();
      }
   }
   this->setVisible(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the type of this itme

   \return  ID

   \created     01.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sintn C_GiArrowCursorButton::type() const
{
   return msn_GRAPHICS_ITEM_ARROWCURSORBTN;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Signal for update of current scaling

   \param[in] orc_Transform Current scaling

   \created     16.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiArrowCursorButton::UpdateTransform(const QTransform & orc_Transform) const
{
   Q_UNUSED(orc_Transform)
   //Not necessary with new connect icon (also make function not const anymore
   //if (orc_Transform.isInvertible() == true)
   //{
   //   this->prepareGeometryChange();
   //   this->setTransform(orc_Transform.inverted());
   //}
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle tool tip generation

   \created     17.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiArrowCursorButton::GenerateHint(void)
{
   //Tool tip
   this->SetDefaultToolTipHeading(C_GtGetText::h_GetText("Link Use"));
   this->SetDefaultToolTipContent(C_GtGetText::h_GetText(
                                     "Connect existing elements easily. Click on link icon and while holding down the left mouse button,"
                                     "\ndrag to either an existing node or bus element."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse press event slot

   Here: Trigger connector

   \param[in,out] opc_Event Event identification and information

   \created     30.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiArrowCursorButton::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (mpc_ConnectedNode !=  NULL)
   {
      //Attach to node center
      Q_EMIT this->StartConnector(mpc_ConnectedNode->mapToScene(
                                     mpc_ConnectedNode->boundingRect().center()), opc_Event->scenePos());
   }
   else
   {
      Q_EMIT this->StartConnector(opc_Event->scenePos(), opc_Event->scenePos());
   }
   QGraphicsSvgItem::mousePressEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle position change of node

   \created     30.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiArrowCursorButton::m_OnNodeChange(void)
{
   if (this->mpc_ConnectedNode != NULL)
   {
      QPointF c_NewPos = this->mpc_ConnectedNode->mapToScene(
         this->mpc_ConnectedNode->GetVisibleBoundingRect().topRight());
      //Adapt x
      c_NewPos.setX(c_NewPos.x() + 20.0);
      this->setPos(c_NewPos);
   }
}
