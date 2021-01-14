//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget with tool tip (implementation)

   Tree widget with tool tip

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>
#include <QHeaderView>
#include "C_OgeTreeWidgetToolTipBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTreeWidgetToolTipBase::C_OgeTreeWidgetToolTipBase(QWidget * const opc_Parent) :
   QTreeWidget(opc_Parent)
{
   this->setMouseTracking(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Handle tooltip if necessary

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeWidgetToolTipBase::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QTreeWidget::mouseMoveEvent(opc_Event);
   this->m_CallAfterMouseMove(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeWidgetToolTipBase::event(QEvent * const opc_Event)
{
   return this->m_CallForEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten event filter slot

   Here: Handle tool tip move in special occasion

   \param[in,out]  opc_Object    Event sender object
   \param[in,out]  opc_Event     Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeWidgetToolTipBase::eventFilter(QObject * const opc_Object, QEvent * const opc_Event)
{
   this->m_CallBeforeEventFilter(opc_Event);
   return QTreeWidget::eventFilter(opc_Object, opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call native "mapToGlobal"

   \param[in]  orc_LocalPos   native parameter

   \return
   Native return value
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_OgeTreeWidgetToolTipBase::m_MapToGlobal(const QPoint & orc_LocalPos) const
{
   return this->mapToGlobal(orc_LocalPos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call native "header"

   \return
   Native return value
*/
//----------------------------------------------------------------------------------------------------------------------
const QHeaderView * C_OgeTreeWidgetToolTipBase::m_Header(void) const
{
   return this->header();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call native "model"

   \return
   Native return value
*/
//----------------------------------------------------------------------------------------------------------------------
const QAbstractItemModel * C_OgeTreeWidgetToolTipBase::m_Model(void) const
{
   return this->model();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call base "event"

   \param[in,out]  opc_Event  Event identification and information

   \return
   Base return value
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeWidgetToolTipBase::m_BaseEvent(QEvent * const opc_Event)
{
   return QTreeWidget::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call native "setMouseTracking"

   \param[in]  oq_Active   native parameter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeWidgetToolTipBase::m_SetMouseTracking(const bool oq_Active)
{
   this->setMouseTracking(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get index at global position

   \param[in]  orc_GlobalPos  Global pos

   \return
   Found index if any
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_OgeTreeWidgetToolTipBase::m_IndexAtGlobalPos(const QPoint & orc_GlobalPos) const
{
   return this->indexAt(this->viewport()->mapFromGlobal(orc_GlobalPos));
}
