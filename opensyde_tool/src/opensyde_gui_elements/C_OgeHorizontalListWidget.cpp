//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for horizontal orientated list widgets (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QDropEvent>

#include "stwtypes.h"
#include "C_OgeHorizontalListWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::sintn C_OgeHorizontalListWidget::mhsn_SCROLL_AREA = 70;
const stw_types::sintn C_OgeHorizontalListWidget::mhsn_DRAG_SCROLL_TIMER_INTERVAL_START = 1000;
const stw_types::sintn C_OgeHorizontalListWidget::mhsn_DRAG_SCROLL_TIMER_INTERVAL = 500;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeHorizontalListWidget::C_OgeHorizontalListWidget(QWidget * const opc_Parent) :
   QListWidget(opc_Parent),
   msn_DragItemIndex(-1),
   msn_ItemsPerLine(1),
   msn_CountLines(1),
   msn_ActualLine(0),
   mq_DragTimeoutActiveLeft(false),
   mq_DragTimeoutActiveRight(false)
{
   // configuration of list widget
   this->setFrameShape(QFrame::NoFrame);
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   this->setAutoScroll(false);
   this->setWrapping(true);
   this->setFlow(QListView::LeftToRight);
   this->setResizeMode(QListView::Adjust);

   // drag functionality for moving items of
   this->setDragEnabled(true);
   this->setDragDropMode(QAbstractItemView::InternalMove);
   this->setDefaultDropAction(Qt::MoveAction);

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mc_TimerDragMove.setInterval(mhsn_DRAG_SCROLL_TIMER_INTERVAL_START);
   this->mc_TimerDragMove.setSingleShot(false);
   //connect timer
   connect(&this->mc_TimerDragMove, &QTimer::timeout,
           this, &C_OgeHorizontalListWidget::m_DragTimeout);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeHorizontalListWidget::~C_OgeHorizontalListWidget()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the row count

   \return
   Actual row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_OgeHorizontalListWidget::GetCountLines(void) const
{
   return this->msn_CountLines;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the actual shown row

   \return
   Actual row
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_OgeHorizontalListWidget::GetActualLine(void) const
{
   return this->msn_ActualLine;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the the new row number

   The native row of the list is each item.

   \param[in]  osn_Line       New row count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::SetActualLine(const sintn osn_Line)
{
   const sintn sn_Row = osn_Line * this->msn_ItemsPerLine;

   this->msn_ActualLine = osn_Line;
   this->scrollTo(this->indexFromItem(this->item(sn_Row)));
   Q_EMIT this->SigListChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the line to the position of the item

   \param[in]  osn_Index   Item index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::ScrollToItem(const sintn osn_Index)
{
   sintn sn_LineNumber;

   if (this->msn_ItemsPerLine > 0)
   {
      sn_LineNumber = osn_Index / this->msn_ItemsPerLine;
   }
   else
   {
      sn_LineNumber = 0;
   }

   this->SetActualLine(sn_LineNumber);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check valid move action

   \param[in]  osn_StartIndex    Start index
   \param[in]  os32_EndIndex     End index
   \param[in]  orc_Widget        Widget

   \return
   Flags

   \retval   True    Valid move action
   \retval   False   Invalid move action
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeHorizontalListWidget::h_CheckValidMoveAction(const sintn osn_StartIndex, const sint32 os32_EndIndex,
                                                       const QListWidget & orc_Widget)
{
   const bool q_IndexChanged = (osn_StartIndex != os32_EndIndex);
   const bool q_MovingLastItemAfterEnd =
      ((osn_StartIndex >= (orc_Widget.count() - 1)) && (static_cast<sintn>(os32_EndIndex) >= (orc_Widget.count() - 1)));

   return q_IndexChanged && (q_MovingLastItemAfterEnd == false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event

   Here: Synchronizing datapool data with widgets

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::dropEvent(QDropEvent * const opc_Event)
{
   // deactivate the painting of the drag widget
   // TODO BAY Painting will not be stopped if the drop event will not be triggered here
   this->m_DelegateStopPaint();
   this->m_StopDragTimer();

   if (this->msn_DragItemIndex >= 0)
   {
      bool q_AllowedMoveAction = false;
      const QListWidget::DropIndicatorPosition e_DropIndicator = this->dropIndicatorPosition();

      //Only allow drop between items
      switch (e_DropIndicator)
      {
      case QAbstractItemView::AboveItem:  // above is allowed
      case QAbstractItemView::BelowItem:  // below is allowed
      case QAbstractItemView::OnViewport: // behind all items is allowed
         q_AllowedMoveAction = true;
         break;
      case QAbstractItemView::OnItem: // on item is not allowed
      default:
         break;
      }

      if (q_AllowedMoveAction == true)
      {
         sint32 s32_TargetRow = this->indexAt(opc_Event->pos()).row();
         //If drag below inserted adapt target row
         if (this->msn_DragItemIndex < s32_TargetRow)
         {
            --s32_TargetRow;
         }

         //If indicator below item insert at next index
         if (e_DropIndicator == QAbstractItemView::BelowItem)
         {
            ++s32_TargetRow;
         }

         if (e_DropIndicator == QAbstractItemView::OnViewport)
         {
            // special case: drop behind the last item in the free space
            s32_TargetRow = this->count();
         }
         if (C_OgeHorizontalListWidget::h_CheckValidMoveAction(this->msn_DragItemIndex, s32_TargetRow, *this))
         {
            // move only if changed
            QListWidget::dropEvent(opc_Event);

            // the number must be updated
            this->m_UpdateNumbers();

            this->m_MoveItem(this->msn_DragItemIndex, s32_TargetRow);
         }
      }

      // reset the counter;
      this->msn_DragItemIndex = -1;
   }
   else
   {
      opc_Event->ignore();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten start drag event

   Here: start custom paint

   \param[in]  oc_SupportedActions  Supported actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::startDrag(const Qt::DropActions oc_SupportedActions)
{
   if (this->count() > 1)
   {
      this->m_DelegateStartPaint();

      // save the actual index for the next drag and drop events
      this->msn_DragItemIndex = this->currentRow();

      QListWidget::startDrag(oc_SupportedActions);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag move event

   Here: Scrolling with auto wrap enabled from left to right instead of top to bottom

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   QRect c_Rect;

   // adapt rectangle
   c_Rect.setTopLeft(QPoint(0, mhsn_SCROLL_AREA));
   c_Rect.setBottomRight(QPoint(this->width() - mhsn_SCROLL_AREA, this->height()));

   if ((opc_Event->pos().x() < mhsn_SCROLL_AREA) &&
       (this->GetActualLine() > 0))
   {
      // start the timer if it is not already started
      if (this->mc_TimerDragMove.isActive() == false)
      {
         // reset interval
         this->mc_TimerDragMove.setInterval(mhsn_DRAG_SCROLL_TIMER_INTERVAL_START);
         this->mc_TimerDragMove.start();
         this->mq_DragTimeoutActiveLeft = true;
      }
   }
   else if ((opc_Event->pos().x() > (this->width() - mhsn_SCROLL_AREA)) &&
            (this->GetActualLine() < (this->GetCountLines() - 1)))
   {
      // start the timer if it is not already started
      if (this->mc_TimerDragMove.isActive() == false)
      {
         // reset interval
         this->mc_TimerDragMove.setInterval(mhsn_DRAG_SCROLL_TIMER_INTERVAL_START);
         this->mc_TimerDragMove.start();
         this->mq_DragTimeoutActiveRight = true;
      }
   }
   else
   {
      QListWidget::dragMoveEvent(opc_Event);
      this->m_StopDragTimer();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag leave event

   Here: Deactivate drag move scrolling timeout

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::dragLeaveEvent(QDragLeaveEvent * const opc_Event)
{
   this->m_StopDragTimer();

   QListWidget::dragLeaveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event

   Updates the information about the count of columns and items per row

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::wheelEvent(QWheelEvent * const opc_Event)
{
   QListWidget::wheelEvent(opc_Event);

   if ((opc_Event->angleDelta().y() > 0) &&
       (this->GetActualLine() > 0))
   {
      // wheel up
      this->SetActualLine(this->GetActualLine() - 1);
   }
   else if ((opc_Event->angleDelta().y() < 0) &&
            (this->GetActualLine() < (this->GetCountLines() - 1)))
   {
      // wheel down
      this->SetActualLine(this->GetActualLine() + 1);
   }
   else
   {
      // nothing todo
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::m_DragTimeout(void)
{
   // next call must be earlier
   this->mc_TimerDragMove.setInterval(mhsn_DRAG_SCROLL_TIMER_INTERVAL);

   if (this->mq_DragTimeoutActiveLeft == true)
   {
      // timer times out, start the move
      this->SetActualLine(this->GetActualLine() - 1);
   }
   else if (this->mq_DragTimeoutActiveRight == true)
   {
      // timer times out, start the move
      this->SetActualLine(this->GetActualLine() + 1);
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::m_StopDragTimer()
{
   this->mc_TimerDragMove.stop();
   this->mq_DragTimeoutActiveLeft = false;
   this->mq_DragTimeoutActiveRight = false;
}
