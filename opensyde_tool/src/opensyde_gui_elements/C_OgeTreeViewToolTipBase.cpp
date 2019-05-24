//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view with tool tip (implementation)

   Tree view with tool tip

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>
#include <QHeaderView>
#include "constants.h"
#include "C_OgeTreeViewToolTipBase.h"

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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTreeViewToolTipBase::C_OgeTreeViewToolTipBase(QWidget * const opc_Parent) :
   QTreeView(opc_Parent)
{
   this->setMouseTracking(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Handle tooltip if necessary

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeViewToolTipBase::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QTreeView::mouseMoveEvent(opc_Event);
   this->CallAfterMouseMove(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeViewToolTipBase::event(QEvent * const opc_Event)
{
   return this->CallForEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten event filter slot

   Here: Handle tool tip move in special occasion

   \param[in,out] opc_Object Event sender object
   \param[in,out] opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeViewToolTipBase::eventFilter(QObject * const opc_Object, QEvent * const opc_Event)
{
   this->CallBeforeEventFilter(opc_Event);
   return QTreeView::eventFilter(opc_Object, opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call native "mapToGlobal"

   \param[in] orc_LocalPos native parameter

   \return
   Native return value
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_OgeTreeViewToolTipBase::m_MapToGlobal(const QPoint & orc_LocalPos) const
{
   return this->mapToGlobal(orc_LocalPos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call native "header"

   \return
   Native return value
*/
//----------------------------------------------------------------------------------------------------------------------
const QHeaderView * C_OgeTreeViewToolTipBase::m_Header(void) const
{
   return this->header();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call native "model"

   \return
   Native return value
*/
//----------------------------------------------------------------------------------------------------------------------
const QAbstractItemModel * C_OgeTreeViewToolTipBase::m_Model(void) const
{
   return this->model();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call base "event"

   \param[in] orc_LocalPos Base parameter

   \return
   Base return value
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeViewToolTipBase::m_BaseEvent(QEvent * const opc_Event)
{
   return QTreeView::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call native "setMouseTracking"

   \param[in] oq_Active native parameter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeViewToolTipBase::m_SetMouseTracking(const bool oq_Active)
{
   this->setMouseTracking(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get index at global position

   \param[in] orc_GlobalPos Global pos

   \return
   Found index if any
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_OgeTreeViewToolTipBase::m_IndexAtGlobalPos(const QPoint & orc_GlobalPos) const
{
   return this->indexAt(this->viewport()->mapFromGlobal(orc_GlobalPos));
}
