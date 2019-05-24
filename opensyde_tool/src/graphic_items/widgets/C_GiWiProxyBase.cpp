//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Proxy widget with cumstom functions for graphics scenes to use widgets (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsSceneMouseEvent>

#include "C_GiWiProxyBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiWiProxyBase::C_GiWiProxyBase(const stw_types::float64 of64_InitWidth, const stw_types::float64 of64_InitHeight,
                                 QGraphicsItem * const opc_Parent) :
   QGraphicsProxyWidget(opc_Parent),
   C_GiBiSizeableItem(of64_InitWidth, of64_InitHeight),
   mq_GrabbedByWidget(false)
{
   this->setFlag(QGraphicsItem::ItemIsMovable);
   this->setFlag(QGraphicsItem::ItemIsSelectable);
   this->setFlag(QGraphicsItem::ItemIsFocusable);
   this->resize(of64_InitWidth, of64_InitHeight);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redraw item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::Redraw(void)
{
   this->setGeometry(this->GetSizeRect());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger mouse press event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerMousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   QGraphicsProxyWidget::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger mouse move event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerMouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   QGraphicsProxyWidget::mouseMoveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger mouse release event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerMouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   QGraphicsProxyWidget::mouseReleaseEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger key press event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerKeyPressEvent(QKeyEvent * const opc_Event)
{
   QGraphicsProxyWidget::keyPressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger key release event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerKeyReleaseEvent(QKeyEvent * const opc_Event)
{
   QGraphicsProxyWidget::keyReleaseEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger focus out event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerFocusOutEvent(QFocusEvent * const opc_Event)
{
   QGraphicsProxyWidget::focusOutEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger wheel event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerWheelEvent(QGraphicsSceneWheelEvent * const opc_Event)
{
   QGraphicsProxyWidget::wheelEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger hover enter event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerHoverEnterEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   QGraphicsProxyWidget::hoverEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger hover move event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerHoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   QGraphicsProxyWidget::hoverMoveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger hover leave event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiWiProxyBase::TriggerHoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   QGraphicsProxyWidget::hoverLeaveEvent(opc_Event);
}
