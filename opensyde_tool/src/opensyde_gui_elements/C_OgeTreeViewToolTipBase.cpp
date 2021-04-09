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
#include <QCheckBox>
#include "TGLUtils.h"
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
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTreeViewToolTipBaseDelegate::C_OgeTreeViewToolTipBaseDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Editor event function to handle the checkbox handling

   Used original Qt implementation as base from QStyledItemDelegate with an adaption
   to make the entire line with the label of the checkbox clickable.

   \param[in,out]   opc_Event   Received event
   \param[in,out]   opc_Model   Model of view
   \param[in]       orc_Option  Detailed Style options of item
   \param[in]       orc_Index   Index of affected item

   \retval   true    Event is handled
   \retval   false   Event is not handled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeViewToolTipBaseDelegate::editorEvent(QEvent * const opc_Event, QAbstractItemModel * const opc_Model,
                                                   const QStyleOptionViewItem & orc_Option,
                                                   const QModelIndex & orc_Index)
{
   bool q_Return = false;

   if ((opc_Model->flags(orc_Index).testFlag(Qt::ItemIsUserCheckable) == true) &&
       (opc_Model->flags(orc_Index).testFlag(Qt::ItemIsEnabled) == true))
   {
      // make sure that we have a check state
      const QVariant c_Value = orc_Index.data(static_cast<sintn>(Qt::CheckStateRole));
      if (c_Value.isValid() == true)
      {
         bool q_AdaptState = true;

         // make sure that we have the right event type
         if ((opc_Event->type() == QEvent::MouseButtonRelease) ||
             (opc_Event->type() == QEvent::MouseButtonDblClick) ||
             (opc_Event->type() == QEvent::MouseButtonPress))
         {
            QStyleOptionViewItem c_ViewOpt(orc_Option);
            const QRect c_CheckRect = orc_Option.widget->rect();

            QMouseEvent * const pc_MouseEvent = dynamic_cast<QMouseEvent *>(opc_Event);

            tgl_assert(pc_MouseEvent != NULL);
            if (pc_MouseEvent != NULL)
            {
               this->initStyleOption(&c_ViewOpt, orc_Index);

               if ((pc_MouseEvent->button() != Qt::LeftButton) ||
                   (c_CheckRect.contains(pc_MouseEvent->pos()) == false))
               {
                  q_AdaptState = false;
               }
               else if ((opc_Event->type() == QEvent::MouseButtonPress) ||
                        (opc_Event->type() == QEvent::MouseButtonDblClick))
               {
                  q_AdaptState = false;
                  q_Return = true;
               }
               else
               {
                  // Nothing to do
               }
            }
         }
         else if (opc_Event->type() == QEvent::KeyPress)
         {
            QKeyEvent * const pc_KeyEvent = dynamic_cast<QKeyEvent *>(opc_Event);

            tgl_assert(pc_KeyEvent != NULL);
            if (pc_KeyEvent != NULL)
            {
               if ((pc_KeyEvent->key() != static_cast<sintn>(Qt::Key_Space)) &&
                   (pc_KeyEvent->key() != static_cast<sintn>(Qt::Key_Select)))
               {
                  q_AdaptState = false;
               }
            }
         }
         else
         {
            q_AdaptState = false;
         }

         if (q_AdaptState == true)
         {
            Qt::CheckState e_State = static_cast<Qt::CheckState>(c_Value.toInt());
            if (opc_Model->flags(orc_Index).testFlag(Qt::ItemIsUserTristate) == true)
            {
               e_State = static_cast<Qt::CheckState>(((static_cast<sintn>(e_State) + 1) % 3));
            }
            else
            {
               e_State = (e_State == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
            }
            q_Return =
               opc_Model->setData(orc_Index, static_cast<sintn>(e_State), static_cast<sintn>(Qt::CheckStateRole));
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
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

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeViewToolTipBase::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QTreeView::mouseMoveEvent(opc_Event);
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
bool C_OgeTreeViewToolTipBase::event(QEvent * const opc_Event)
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
bool C_OgeTreeViewToolTipBase::eventFilter(QObject * const opc_Object, QEvent * const opc_Event)
{
   this->m_CallBeforeEventFilter(opc_Event);
   return QTreeView::eventFilter(opc_Object, opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call native "mapToGlobal"

   \param[in]  orc_LocalPos   native parameter

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

   \param[in,out]  opc_Event  Event identification and information

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

   \param[in]  oq_Active   native parameter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeViewToolTipBase::m_SetMouseTracking(const bool oq_Active)
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
QModelIndex C_OgeTreeViewToolTipBase::m_IndexAtGlobalPos(const QPoint & orc_GlobalPos) const
{
   return this->indexAt(this->viewport()->mapFromGlobal(orc_GlobalPos));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get column widths

   \return
   column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_OgeTreeViewToolTipBase::m_GetColumnWidths() const
{
   std::vector<sint32> c_ColumnWidths;
   c_ColumnWidths.reserve(this->model()->columnCount());
   for (sint32 s32_ItColumn = 0; s32_ItColumn < this->model()->columnCount(); ++s32_ItColumn)
   {
      c_ColumnWidths.push_back(this->columnWidth(s32_ItColumn));
   }
   return c_ColumnWidths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set column widths

   \param[in]  orc_ColumnWidths  Column widths

   \retval   true    column widths were set
   \retval   false   column widths invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeViewToolTipBase::m_SetColumnWidths(const std::vector<sint32> & orc_ColumnWidths)
{
   bool q_Retval = false;

   //Only apply user settings if number of expected columns, otherwise this could lead to unexpected behavior
   if ((orc_ColumnWidths.size() > 0UL) &&
       (static_cast<uint32>(this->model()->columnCount()) == orc_ColumnWidths.size()))
   {
      q_Retval = true;
      for (uint32 u32_ItCol = 0; u32_ItCol < orc_ColumnWidths.size(); ++u32_ItCol)
      {
         this->setColumnWidth(static_cast<sint32>(u32_ItCol), orc_ColumnWidths[u32_ItCol]);
      }
   }
   return q_Retval;
}
