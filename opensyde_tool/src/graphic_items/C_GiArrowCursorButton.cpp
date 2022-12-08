//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A button displaying an arrow cursor (implementation)

   A button displaying an arrow cursor

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPixmap>

#include "stwtypes.hpp"
#include "gitypes.hpp"
#include "C_GtGetText.hpp"
#include "C_GiArrowCursorButton.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiArrowCursorButton::~C_GiArrowCursorButton()
{
   mpc_ConnectedNode = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stop listening to node position changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiArrowCursorButton::DetachNode(void)
{
   if (mpc_ConnectedNode != NULL)
   {
      disconnect(mpc_ConnectedNode, &C_GiNode::SigChangedGraphic, this, &C_GiArrowCursorButton::m_OnNodeChange);
      mpc_ConnectedNode = NULL;
   }
   this->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start listening to node position changes

   Implicit: Detach last node

   \param[in] opc_Node New node to connect to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiArrowCursorButton::AttachToNode(const C_GiNode * const opc_Node)
{
   if (opc_Node != NULL)
   {
      DetachNode();
      if (opc_Node->GetValid() == true)
      {
         mpc_ConnectedNode = opc_Node;
         connect(mpc_ConnectedNode, &C_GiNode::SigChangedGraphic, this, &C_GiArrowCursorButton::m_OnNodeChange);
         m_OnNodeChange();
      }
   }
   this->setVisible(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this itme

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiArrowCursorButton::type() const
{
   return ms32_GRAPHICS_ITEM_ARROWCURSORBTN;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle tool tip generation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiArrowCursorButton::GenerateHint(void)
{
   //Tool tip
   this->SetDefaultToolTipHeading(C_GtGetText::h_GetText("Link Use"));
   this->SetDefaultToolTipContent(C_GtGetText::h_GetText(
                                     "Connect existing elements: Click on the link icon and drag and drop it "
                                     "either to an existing node or to a bus element."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: Trigger connector

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiArrowCursorButton::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (mpc_ConnectedNode !=  NULL)
   {
      //Attach to node center
      Q_EMIT (this->SigStartConnector(mpc_ConnectedNode->mapToScene(
                                         mpc_ConnectedNode->boundingRect().center()), opc_Event->scenePos()));
   }
   else
   {
      Q_EMIT (this->SigStartConnector(opc_Event->scenePos(), opc_Event->scenePos()));
   }
   QGraphicsSvgItem::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle position change of node
*/
//----------------------------------------------------------------------------------------------------------------------
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
