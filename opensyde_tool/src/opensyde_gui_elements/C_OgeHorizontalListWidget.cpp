//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for horizontal orientated list widgets (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QDropEvent>

#include "stwtypes.hpp"
#include "C_OgeHorizontalListWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_OgeHorizontalListWidget::mhs32_SCROLL_AREA = 70;
const int32_t C_OgeHorizontalListWidget::mhs32_DRAG_SCROLL_TIMER_INTERVAL_START = 1000;
const int32_t C_OgeHorizontalListWidget::mhs32_DRAG_SCROLL_TIMER_INTERVAL = 500;

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
   ms32_DragItemIndex(-1),
   ms32_ItemsPerLine(1),
   ms32_CountLines(1),
   ms32_ActualLine(0),
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
   this->mc_TimerDragMove.setInterval(mhs32_DRAG_SCROLL_TIMER_INTERVAL_START);
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
int32_t C_OgeHorizontalListWidget::GetCountLines(void) const
{
   return this->ms32_CountLines;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the actual shown row

   \return
   Actual row
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeHorizontalListWidget::GetActualLine(void) const
{
   return this->ms32_ActualLine;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the the new row number

   The native row of the list is each item.

   \param[in]  os32_Line       New row count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::SetActualLine(const int32_t os32_Line)
{
   const int32_t s32_Row = os32_Line * this->ms32_ItemsPerLine;

   this->ms32_ActualLine = os32_Line;
   this->scrollTo(this->indexFromItem(this->item(s32_Row)));
   Q_EMIT this->SigListChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the line to the position of the item

   \param[in]  os32_Index   Item index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeHorizontalListWidget::ScrollToItem(const int32_t os32_Index)
{
   int32_t s32_LineNumber;

   if (this->ms32_ItemsPerLine > 0)
   {
      s32_LineNumber = os32_Index / this->ms32_ItemsPerLine;
   }
   else
   {
      s32_LineNumber = 0;
   }

   this->SetActualLine(s32_LineNumber);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check valid move action

   \param[in]  os32_StartIndex   Start index
   \param[in]  os32_EndIndex     End index
   \param[in]  orc_Widget        Widget

   \return
   Flags

   \retval   True    Valid move action
   \retval   False   Invalid move action
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeHorizontalListWidget::h_CheckValidMoveAction(const int32_t os32_StartIndex, const int32_t os32_EndIndex,
                                                       const QListWidget & orc_Widget)
{
   const bool q_IndexChanged = (os32_StartIndex != os32_EndIndex);
   const bool q_MovingLastItemAfterEnd =
      ((os32_StartIndex >= (orc_Widget.count() - 1)) &&
       (static_cast<int32_t>(os32_EndIndex) >= (orc_Widget.count() - 1)));

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

   if (this->ms32_DragItemIndex >= 0)
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
         int32_t s32_TargetRow = this->indexAt(opc_Event->pos()).row();
         //If drag below inserted adapt target row
         if (this->ms32_DragItemIndex < s32_TargetRow)
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
         if (C_OgeHorizontalListWidget::h_CheckValidMoveAction(this->ms32_DragItemIndex, s32_TargetRow, *this))
         {
            // move only if changed
            QListWidget::dropEvent(opc_Event);

            // the number must be updated
            this->m_UpdateNumbers();

            this->m_MoveItem(this->ms32_DragItemIndex, s32_TargetRow);
         }
      }

      // reset the counter;
      this->ms32_DragItemIndex = -1;
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
      this->ms32_DragItemIndex = this->currentRow();

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
   c_Rect.setTopLeft(QPoint(0, mhs32_SCROLL_AREA));
   c_Rect.setBottomRight(QPoint(this->width() - mhs32_SCROLL_AREA, this->height()));

   if ((opc_Event->pos().x() < mhs32_SCROLL_AREA) &&
       (this->GetActualLine() > 0))
   {
      // start the timer if it is not already started
      if (this->mc_TimerDragMove.isActive() == false)
      {
         // reset interval
         this->mc_TimerDragMove.setInterval(mhs32_DRAG_SCROLL_TIMER_INTERVAL_START);
         this->mc_TimerDragMove.start();
         this->mq_DragTimeoutActiveLeft = true;
      }
   }
   else if ((opc_Event->pos().x() > (this->width() - mhs32_SCROLL_AREA)) &&
            (this->GetActualLine() < (this->GetCountLines() - 1)))
   {
      // start the timer if it is not already started
      if (this->mc_TimerDragMove.isActive() == false)
      {
         // reset interval
         this->mc_TimerDragMove.setInterval(mhs32_DRAG_SCROLL_TIMER_INTERVAL_START);
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
   this->mc_TimerDragMove.setInterval(mhs32_DRAG_SCROLL_TIMER_INTERVAL);

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
