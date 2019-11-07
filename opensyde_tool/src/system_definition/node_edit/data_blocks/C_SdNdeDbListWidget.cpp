//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for node applications (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QApplication>
#include <QScrollBar>

#include "stwtypes.h"
#include "C_SdNdeDbListWidget.h"
#include "C_SdNdeDbWidget.h"
#include "C_PuiSdHandler.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbListWidget::C_SdNdeDbListWidget(QWidget * const opc_Parent) :
   QListWidget(opc_Parent),
   mu32_NodeIndex(0U),
   msn_DragItemIndex(-1)
{
   // configuration of list widget
   this->setFrameShape(QFrame::NoFrame);
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   this->setAutoScroll(true);
   this->setWrapping(true);
   this->setFlow(QListView::LeftToRight);
   this->setResizeMode(QListView::Adjust);
   //set no focus policy to hide focus rectangle of an selected item
   this->setFocusPolicy(Qt::NoFocus);

   // drag functionality for moving items of
   this->setDragEnabled(true);
   this->setDragDropMode(QAbstractItemView::InternalMove);
   this->setDefaultDropAction(Qt::MoveAction);
   this->setSelectionMode(QAbstractItemView::SingleSelection);

   this->setItemDelegate(&this->mc_Delegate);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   // scroll bar style
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "C_SdNdeDbWidget", true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbListWidget::~C_SdNdeDbListWidget()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node index

   \param[in] ou32_NodeIndex Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListWidget::SetIndex(const stw_types::uint32 ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new node application to the list

   \param[in] ou32_NodeIndex        Node index
   \param[in] ou32_ApplicationIndex Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListWidget::AddApplication(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex)
{
   QListWidgetItem * pc_Item = new QListWidgetItem(NULL, static_cast<sintn>(QListWidgetItem::ItemType::UserType));
   C_SdNdeDbWidget * pc_ItemWidget = new C_SdNdeDbWidget(ou32_NodeIndex, ou32_ApplicationIndex,
                                                         this);

   //signal to handle "there are no data blocks declared..." label
   Q_EMIT this->SigAppDisplay();

   pc_Item->setSizeHint(pc_ItemWidget->size());

   if (this->count() == 0)
   {
      // configure the list for the first usage
      // the offset of 20 pixel: 14(+/-1) pixel + 6 pixel margin of C_SdNdeDbWidget
      this->setGridSize(QSize(pc_ItemWidget->size().width() + 14, pc_ItemWidget->size().height() + 13));
      this->setMinimumWidth(pc_ItemWidget->size().width() + 60);
   }

   this->addItem(pc_Item);
   this->setItemWidget(pc_Item, pc_ItemWidget);

   //Connections
   connect(pc_ItemWidget, &C_SdNdeDbWidget::SigDelete, this, &C_SdNdeDbListWidget::SigDelete);
   connect(pc_ItemWidget, &C_SdNdeDbWidget::SigReloadDataPools, this,
           &C_SdNdeDbListWidget::SigReloadDataPools);
   connect(pc_ItemWidget, &C_SdNdeDbWidget::SigCheckNodeId, this,
           &C_SdNdeDbListWidget::m_CheckNodeId);
   connect(pc_ItemWidget, &C_SdNdeDbWidget::SigOpenDataPool, this,
           &C_SdNdeDbListWidget::SigOpenDataPool);
   //lint -e{429}  no memory leak because of the parent of pc_Item by calling addItem and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update applications
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListWidget::UpdateApplications(void) const
{
   // Update all application indexes of the concrete widgets
   for (sintn sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      QListWidgetItem * const pc_Item = this->item(sn_Counter);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdNdeDbWidget * const pc_WidgetItem =
         dynamic_cast<C_SdNdeDbWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->UpdateDataPools();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event

   Here: Synchronizing application data with widgets

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListWidget::dropEvent(QDropEvent * const opc_Event)
{
   // deactivate the painting of the drag widget
   this->mc_Delegate.StopPaint();

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

         // move only if changed
         QListWidget::dropEvent(opc_Event);

         this->m_MoveApplication(this->msn_DragItemIndex, s32_TargetRow);
         // the number must be updated
         this->m_UpdateApplicationIndexes();
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

   \param[in] oc_SupportedActions Supported actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListWidget::startDrag(const Qt::DropActions oc_SupportedActions)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdNdeDbWidget * pc_ItemWidget =
      dynamic_cast<C_SdNdeDbWidget *>(this->itemWidget(this->item(this->currentIndex().row())));

   this->mc_Delegate.StartPaint(this->currentIndex().row(), pc_ItemWidget);

   // save the actual index for the next drag and drop events
   this->msn_DragItemIndex = this->currentRow();

   QListWidget::startDrag(oc_SupportedActions);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListWidget::m_MoveApplication(const sintn osn_SourceIndex, const sintn osn_TargetIndex)
{
   QApplication::setOverrideCursor(Qt::WaitCursor);

   // check for index errors
   if ((osn_SourceIndex >= 0) &&
       (osn_TargetIndex >= 0))
   {
      C_PuiSdHandler::h_GetInstance()->MoveApplication(this->mu32_NodeIndex, osn_SourceIndex, osn_TargetIndex);

      this->setCurrentRow(osn_TargetIndex);
   }

   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListWidget::m_UpdateApplicationIndexes(void) const
{
   sintn sn_Counter;

   // Update all application indexes of the concrete widgets
   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      QListWidgetItem * const pc_Item = this->item(sn_Counter);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdNdeDbWidget * const pc_WidgetItem =
         dynamic_cast<C_SdNdeDbWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->UpdateApplicationIndex(static_cast<uint32>(sn_Counter));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger node ID recheck
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListWidget::m_CheckNodeId(void) const
{
   // Update all application indexes of the concrete widgets
   for (sintn sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      QListWidgetItem * const pc_Item = this->item(sn_Counter);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SdNdeDbWidget * const pc_WidgetItem =
         dynamic_cast<const C_SdNdeDbWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->CheckProcessIdError();
      }
   }
   Q_EMIT this->SigErrorChange();
}
