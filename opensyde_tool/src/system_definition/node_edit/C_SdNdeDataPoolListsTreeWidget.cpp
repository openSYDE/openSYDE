//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget to store and manage multiple data pool list entries (implementation)

   Widget to store and manage multiple data pool list entries

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QDrag>
#include <QDragMoveEvent>
#include <QScrollBar>

#include "C_SdNdeDataPoolListsTreeWidget.h"
#include "stwerrors.h"
#include "C_SdClipBoardHelper.h"
#include "C_SdNdeDataPoolListTableView.h"
#include "C_PuiSdHandler.h"
#include "C_GtGetText.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_Uti.h"
#include "C_OgeWiUtil.h"
#include "constants.h"
#include "TGLUtils.h"
#include "C_SdUtil.h"
#include "C_UsHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace stw_errors;
using namespace stw_tgl;

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

   \created     17.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListsTreeWidget::C_SdNdeDataPoolListsTreeWidget(QWidget * const opc_Parent) :
   QTreeWidget(opc_Parent),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mpc_ContextMenu(NULL),
   mpc_ActionAdd(NULL),
   mpc_ActionMoveUp(NULL),
   mpc_ActionMoveDown(NULL),
   mq_AllowAdd(true),
   mq_AllowMoveUp(true),
   mq_AllowMoveDown(true),
   mq_InitialUserSettings(true)
{
   //UI Settings
   this->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
   this->setDragEnabled(true);
   this->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
   this->setDefaultDropAction(Qt::TargetMoveAction);
   this->setAlternatingRowColors(true);
   this->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
   this->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
   this->setAutoExpandDelay(500);
   this->setIndentation(0);
   this->setRootIsDecorated(false);
   this->setAnimated(true);
   this->setHeaderHidden(true);
   this->setAcceptDrops(true);
   this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

   this->setItemDelegate(&this->mc_Delegate);

   //Custom context menu
   m_SetupContextMenu();

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   //Avoid styling table inside
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->verticalScrollBar(), "C_SdNdeDataPoolListsTreeWidget", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->horizontalScrollBar(), "C_SdNdeDataPoolListsTreeWidget", true);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //Connect
   connect(this, &C_SdNdeDataPoolListsTreeWidget::collapsed, this, &C_SdNdeDataPoolListsTreeWidget::m_OnCollapse);
   connect(this, &C_SdNdeDataPoolListsTreeWidget::expanded, this, &C_SdNdeDataPoolListsTreeWidget::m_OnExpand);
   connect(&this->mc_UndoManager, &C_SdNdeUnoDataPoolManager::SigChanged, this,
           &C_SdNdeDataPoolListsTreeWidget::m_HandleChanged);
   connect(&this->mc_ModelViewManager, &C_SdNdeDataPoolListModelViewManager::SigDataSetCountChange, this,
           &C_SdNdeDataPoolListsTreeWidget::m_UpdateDataSetCount);
   connect(&this->mc_ModelViewManager, &C_SdNdeDataPoolListModelViewManager::SigDataSetErrorChange, this,
           &C_SdNdeDataPoolListsTreeWidget::m_HandleDataSetErrorChange);

   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdNdeDataPoolListsTreeWidget::m_ScrollBarRangeChanged);
   //Error handling
   connect(&this->mc_ErrorManager, &C_SdNdeDataPoolListErrorManager::SigErrorChange, this,
           &C_SdNdeDataPoolListsTreeWidget::m_OnErrorChangePossible);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     22.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListsTreeWidget::~C_SdNdeDataPoolListsTreeWidget(void)
{
   m_StoreUserSettings();
   //lint -e{1540,1579}  no memory leak because of the parent of items and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new data pool

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::SetDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex)
{
   if (this->mq_InitialUserSettings == true)
   {
      this->mq_InitialUserSettings = false;
   }
   else
   {
      //Might be problematic if deleting/adding new datapools
      m_StoreUserSettings();
   }
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->clear();
   this->mc_UndoManager.clear();
   this->mc_ModelViewManager.Clear();
   this->mc_ErrorManager.Init(this->mu32_NodeIndex, this->mu32_DataPoolIndex);
   this->mc_Delegate.SetDataPool(this->mu32_NodeIndex, this->mu32_DataPoolIndex);
   m_InitFromData();
   m_RestoreUserSettings();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new list

   \created     17.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::AddEntry(void)
{
   //Top level
   const sint32 s32_Index = this->topLevelItemCount();
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(static_cast<sintn>(QTreeWidgetItem::ItemType::UserType));

   this->addTopLevelItem(pc_Item);

   m_InitialItemConfigure(pc_Item, s32_Index);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update widgets from data

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::UpdateUI(void)
{
   const C_OSCNodeDataPool * const pc_OSCDataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex);

   if (pc_OSCDataPool != NULL)
   {
      if (static_cast<uint32>(this->topLevelItemCount()) == pc_OSCDataPool->c_Lists.size())
      {
         for (sint32 s32_It = 0; s32_It < this->topLevelItemCount(); ++s32_It)
         {
            QTreeWidgetItem * const pc_HeaderItem = this->topLevelItem(s32_It);

            if (pc_HeaderItem != NULL)
            {
               //Header
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               C_SdNdeDataPoolListHeaderWidget * pc_HeaderWidget =
                  dynamic_cast<C_SdNdeDataPoolListHeaderWidget *>(this->itemWidget(pc_HeaderItem, 0));

               //Update content
               if (pc_HeaderWidget == NULL)
               {
                  m_InitialItemConfigure(pc_HeaderItem, static_cast<uint32>(s32_It));
               }
               else
               {
                  pc_HeaderWidget->SetIndex(static_cast<uint32>(s32_It));
               }
               //Table
               if (pc_HeaderItem->childCount() > 0)
               {
                  QTreeWidgetItem * const pc_TableItem = pc_HeaderItem->child(0);
                  if (pc_TableItem != NULL)
                  {
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     C_SdNdeDataPoolListTableWidget * const pc_TableWidget =
                        dynamic_cast<C_SdNdeDataPoolListTableWidget * const>(this->itemWidget(pc_TableItem, 0));

                     //Update content
                     if (pc_TableWidget != NULL)
                     {
                        pc_TableWidget->SetList(this->mu32_NodeIndex, this->mu32_DataPoolIndex, s32_It);
                     }
                  }
               }
            }
         }
      }
   }
   //Do item layout to update the item sizes (Size hint!)
   this->doItemsLayout();
   //Signals
   Q_EMIT this->SigSizeChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert new row at specified row index

   \param[in] oru32_TargetRow Row index

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::InsertRowWithoutData(const uint32 & oru32_TargetRow)
{
   QTreeWidgetItem * const pc_Item = new QTreeWidgetItem(static_cast<sintn>(QTreeWidgetItem::ItemType::UserType));

   this->insertTopLevelItem(oru32_TargetRow, pc_Item);
   m_InitialItemConfigure(pc_Item, oru32_TargetRow);
}
//-----------------------------------------------------------------------------
/*!
   \brief   Copy tree item to clipboard

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::Copy(void) const
{
   C_SdNdeDataPoolListTableWidget * const pc_Table = m_GetActiveTable();

   if (pc_Table != NULL)
   {
      pc_Table->Copy();
   }
   else
   {
      const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
         this->mu32_NodeIndex,
         this->mu32_DataPoolIndex);
      if (pc_DataPool != NULL)
      {
         std::vector<C_OSCNodeDataPoolList> c_OSCContentVec;
         std::vector<C_PuiSdNodeDataPoolList> c_UIContentVec;
         C_OSCNodeDataPoolList c_OSCContent;
         C_PuiSdNodeDataPoolList c_UIContent;
         std::vector<uint32> c_SelectedIndices = m_GetSelectedIndices();

         //Sort to have "correct" copy order
         C_SdUtil::h_SortIndicesAscending(c_SelectedIndices);
         for (uint32 u32_ItIndex = 0; u32_ItIndex < c_SelectedIndices.size(); ++u32_ItIndex)
         {
            if (C_PuiSdHandler::h_GetInstance()->GetDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 c_SelectedIndices[u32_ItIndex],
                                                                 c_OSCContent,
                                                                 c_UIContent) == C_NO_ERR)
            {
               c_OSCContentVec.push_back(c_OSCContent);
               c_UIContentVec.push_back(c_UIContent);
            }
         }
         C_SdClipBoardHelper::h_StoreDataPoolLists(c_OSCContentVec, c_UIContentVec, pc_DataPool->e_Type);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy tree item to clipboard and delete it afterwards

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::Cut(void)
{
   C_SdNdeDataPoolListTableWidget * const pc_Table = m_GetActiveTable();

   if (pc_Table != NULL)
   {
      pc_Table->Cut();
   }
   else
   {
      Copy();
      Delete();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paste tree item

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::Paste(void)
{
   C_SdNdeDataPoolListTableWidget * const pc_Table = m_GetActiveTable();

   if (pc_Table != NULL)
   {
      pc_Table->Paste();
   }
   else
   {
      //Last selected index
      const uint32 u32_LastIndex = m_GetOneAfterHighestSelected();
      this->mc_UndoManager.DoPaste(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this, u32_LastIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete tree item

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::Delete(void)
{
   C_SdNdeDataPoolListTableWidget * const pc_Table = m_GetActiveTable();

   if (pc_Table != NULL)
   {
      pc_Table->Delete();
   }
   else
   {
      std::vector<uint32> c_Indices = m_GetSelectedIndices();
      this->mc_UndoManager.DoDeleteList(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this,
                                        c_Indices);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert triggered via action

   \created     08.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::InsertAction(void)
{
   Insert();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert tree item

   \param[in] orq_SetFocus Optional flag if inital focus should be set

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::Insert(const bool & orq_SetFocus)
{
   C_SdNdeDataPoolListTableWidget * const pc_Table = m_GetActiveTable();

   if (pc_Table != NULL)
   {
      pc_Table->Insert(orq_SetFocus);
   }
   else
   {
      if (this->mq_AllowAdd == true)
      {
         std::vector<uint32> c_Indices;
         const uint32 u32_TargetIndex = m_GetOneAfterHighestSelected();
         c_Indices.push_back(u32_TargetIndex);

         this->mc_UndoManager.DoAddList(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this,
                                        c_Indices);
         //Set initial focus
         if (orq_SetFocus == true)
         {
            QTreeWidgetItem * const pc_TreeWidgetItem =
               this->itemFromIndex(this->model()->index(static_cast<sintn>(u32_TargetIndex), 0));
            tgl_assert(pc_TreeWidgetItem != NULL);
            if (pc_TreeWidgetItem != NULL)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               C_SdNdeDataPoolListHeaderWidget * const pc_HeaderWidget =
                  dynamic_cast<C_SdNdeDataPoolListHeaderWidget * >(this->itemWidget(pc_TreeWidgetItem, 0));
               tgl_assert((pc_HeaderWidget != NULL) && (pc_TreeWidgetItem->isSelected() == true));
               if (pc_HeaderWidget != NULL)
               {
                  pc_HeaderWidget->SetEditFocus();
               }
            }
         }
      }
      else
      {
         C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText("Only %1 lists allowed per Datapool.")).arg(
                                        mu32_NODE_DATA_POOL_LIST_MAX));
         c_MessageBox.Execute();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move selected item up by one slot

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::DoMoveUp(void)
{
   C_SdNdeDataPoolListTableWidget * const pc_Table = m_GetActiveTable();

   if (pc_Table != NULL)
   {
      pc_Table->DoMoveUp();
   }
   else
   {
      if (this->mq_AllowMoveUp == true)
      {
         bool q_AllowMove = true;

         std::vector<uint32> c_TargetIndices;
         std::vector<uint32> c_SourceIndices = m_GetSelectedIndices();
         c_TargetIndices.resize(c_SourceIndices.size());

         //Sort to have "correct" move order
         C_SdUtil::h_SortIndicesAscending(c_SourceIndices);
         for (uint32 u32_ItItem = 0; u32_ItItem < c_TargetIndices.size(); ++u32_ItItem)
         {
            //Check if not first
            if (c_SourceIndices[u32_ItItem] > 0)
            {
               c_TargetIndices[u32_ItItem] = c_SourceIndices[u32_ItItem] - 1;
            }
            else
            {
               //No move
               q_AllowMove = false;
            }
         }
         if (q_AllowMove == true)
         {
            m_Move(c_SourceIndices, c_TargetIndices);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move selected item down by one slot

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::DoMoveDown(void)
{
   C_SdNdeDataPoolListTableWidget * const pc_Table = m_GetActiveTable();

   if (pc_Table != NULL)
   {
      pc_Table->DoMoveDown();
   }
   else
   {
      if (this->mq_AllowMoveDown == true)
      {
         bool q_AllowMove = true;

         std::vector<uint32> c_TargetIndices;
         std::vector<uint32> c_SourceIndices = m_GetSelectedIndices();
         const uint32 u32_LastIndex = static_cast<uint32>(this->topLevelItemCount() - 1);
         c_TargetIndices.resize(c_SourceIndices.size());

         //Sort to have "correct" move order
         C_SdUtil::h_SortIndicesAscending(c_SourceIndices);
         for (uint32 u32_ItItem = 0; u32_ItItem < c_TargetIndices.size(); ++u32_ItItem)
         {
            //Check if not last
            if (c_SourceIndices[u32_ItItem] < u32_LastIndex)
            {
               c_TargetIndices[u32_ItItem] = c_SourceIndices[u32_ItItem] + 1;
            }
            else
            {
               //No move
               q_AllowMove = false;
            }
         }
         if (q_AllowMove == true)
         {
            m_Move(c_SourceIndices, c_TargetIndices);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Enter name edit

   \created     06.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::Edit(void) const
{
   C_SdNdeDataPoolListTableWidget * const pc_Table = m_GetActiveTable();

   if (pc_Table != NULL)
   {
      pc_Table->Edit();
   }
   else
   {
      const std::vector<uint32> c_Selection = m_GetSelectedIndices();
      if (c_Selection.size() == 1)
      {
         const QModelIndex c_Index = this->model()->index(c_Selection[0], 0);
         QTreeWidgetItem * const pc_ItemWidget = this->itemFromIndex(c_Index);
         if (pc_ItemWidget != NULL)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SdNdeDataPoolListHeaderWidget * const pc_HeaderWidget =
               dynamic_cast<C_SdNdeDataPoolListHeaderWidget * >(this->itemWidget(pc_ItemWidget, 0));
            if (pc_HeaderWidget != NULL)
            {
               pc_HeaderWidget->SetEditFocus();
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle pop up request

   \created     13.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::PopUp(void) const
{
   QTreeWidgetItem * const pc_Table = m_GetActiveTableTreeWidget();

   if (pc_Table != NULL)
   {
      //Pop up header
      const QModelIndex c_Index = this->indexFromItem(pc_Table);
      if (c_Index.parent().isValid() == true)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SdNdeDataPoolListHeaderWidget * const pc_Widget =
            dynamic_cast<C_SdNdeDataPoolListHeaderWidget * const>(this->indexWidget(c_Index.parent()));

         if (pc_Widget != NULL)
         {
            pc_Widget->PopUp();
         }
      }
   }
   else
   {
      const std::vector<uint32> c_Selection = m_GetSelectedIndices();
      if (c_Selection.size() == 1)
      {
         const QModelIndex c_Index = this->model()->index(c_Selection[0], 0);
         QTreeWidgetItem * const pc_ItemWidget = this->itemFromIndex(c_Index);
         if (pc_ItemWidget != NULL)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SdNdeDataPoolListHeaderWidget * const pc_HeaderWidget =
               dynamic_cast<C_SdNdeDataPoolListHeaderWidget * >(this->itemWidget(pc_ItemWidget, 0));
            if (pc_HeaderWidget != NULL)
            {
               pc_HeaderWidget->PopUp();
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check node data pool full

   \return
   true  Data pool full
   false else

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolListsTreeWidget::CheckDataPoolFull(void) const
{
   bool q_Retval = false;
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(this->mu32_NodeIndex,
                                                                                                 this->mu32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      if (pc_DataPool->c_Lists.size() >= mu32_NODE_DATA_POOL_LIST_MAX)
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to open a concrete datapool list or dataelement

   \param[in] os32_ListIndex          Optional list index (if not used set to -1)
   \param[in] os32_DataElementIndex   Optional data element index (if not used set to -1)

   \created     16.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::OpenDetail(const sint32 os32_ListIndex, const sint32 os32_DataElementIndex)
{
   QTreeWidgetItem * pc_Item = this->topLevelItem(os32_ListIndex);
   const QList<QTreeWidgetItem *> & rc_SelectedItems = this->selectedItems();

   QList<QTreeWidgetItem *>::const_iterator c_ItItem;

   // deselect and collapse all previous lists
   for (c_ItItem = rc_SelectedItems.begin(); c_ItItem != rc_SelectedItems.end(); ++c_ItItem)
   {
      (*c_ItItem)->setSelected(false);
   }

   if (pc_Item != NULL)
   {
      if (os32_DataElementIndex < 0)
      {
         pc_Item->setSelected(true);
      }
      this->m_OnExpandRequested(pc_Item, true);

      // scroll to the end first to see the expanded list always too
      this->scrollToBottom();
      this->scrollToItem(pc_Item);

      if (os32_DataElementIndex >= 0)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SdNdeDataPoolListTableWidget * const pc_TableWidget =
            dynamic_cast<C_SdNdeDataPoolListTableWidget * const>(this->itemWidget(pc_Item->child(0), 0));

         if (pc_TableWidget != NULL)
         {
            pc_TableWidget->SelectDataElement(os32_DataElementIndex);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register model change

   \created     18.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::UpdateModels(void)
{
   this->mc_ModelViewManager.UpdateModels();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register size change

   \created     24.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::RegisterSizeChange(void)
{
   this->UpdateUI();
   m_HandleSizeChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle error change signal of any list item

   \created     24.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::HandleErrorChange(void)
{
   this->mc_ErrorManager.OnErrorChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drop event slot

   Here: Skip old item, find new item and add widget (widget is not copied)

   \param[in,out] opc_Event Event identification and information

   \created     17.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::dropEvent(QDropEvent * const opc_Event)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_SdNdeDataPoolListsTreeWidget * const pc_Tree =
      dynamic_cast<const C_SdNdeDataPoolListsTreeWidget * const>(opc_Event->source());

   if (pc_Tree != NULL)
   {
      bool q_AllowedMoveAction = false;
      const DropIndicatorPosition e_DropIndicator = dropIndicatorPosition();

      //Only allow drop between items
      switch (e_DropIndicator)
      {
      case QAbstractItemView::AboveItem:
         q_AllowedMoveAction = true;
         break;
      case QAbstractItemView::BelowItem:
         q_AllowedMoveAction = true;
         break;
      case QAbstractItemView::OnItem:
         q_AllowedMoveAction = true;
         break;
      default:
         break;
      }

      if (q_AllowedMoveAction == true)
      {
         const std::vector<uint32> c_SelectedIndices = this->m_GetSelectedIndices();
         std::vector<uint32> c_TargetIndices;
         sint32 s32_TargetRow = static_cast<sint32>(this->indexAt(opc_Event->pos()).row());
         mh_AdaptDropTargetIndex(c_SelectedIndices, e_DropIndicator, s32_TargetRow);
         c_TargetIndices.resize(c_SelectedIndices.size());
         for (uint32 u32_ItItem = 0; u32_ItItem < c_TargetIndices.size(); ++u32_ItItem)
         {
            c_TargetIndices[u32_ItItem] = static_cast<uint32>(s32_TargetRow) + u32_ItItem;
         }
         QTreeWidget::dropEvent(opc_Event);
         this->mc_UndoManager.DoMoveList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                         this,
                                         c_SelectedIndices,
                                         c_TargetIndices);
      }
      else
      {
         setDropIndicatorShown(false); // Manually hide the drop indicator
      }
   }
   else
   {
      opc_Event->ignore();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drag enter event slot

   Here: Accept table item type

   \param[in,out] opc_Event Event identification and information

   \created     03.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_SdNdeDataPoolListsTreeWidget * const pc_Tree =
      dynamic_cast<const C_SdNdeDataPoolListsTreeWidget * const>(opc_Event->source());

   if (pc_Tree != NULL)
   {
      //Ignore list drag
      const sint32 s32_Delay = this->autoExpandDelay();
      this->setAutoExpandDelay(-1);
      QTreeWidget::dragEnterEvent(opc_Event);
      opc_Event->accept();
      this->setAutoExpandDelay(s32_Delay);
   }
   else
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SdNdeDataPoolListTableView * const pc_Table =
         dynamic_cast<const C_SdNdeDataPoolListTableView * const>(opc_Event->source());

      if (pc_Table != NULL)
      {
         QTreeWidget::dragEnterEvent(opc_Event);
         opc_Event->accept();
      }
      else
      {
         opc_Event->ignore();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drag move event slot

   Here: selective deactivation of autoexpand feature

   \param[in,out] opc_Event Event identification and information

   \created     03.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_SdNdeDataPoolListsTreeWidget * const pc_Tree =
      dynamic_cast<const C_SdNdeDataPoolListsTreeWidget * const>(opc_Event->source());

   if (pc_Tree != NULL)
   {
      //Ignore list drag
      const sint32 s32_Delay = this->autoExpandDelay();
      this->setAutoExpandDelay(-1);
      QTreeWidget::dragMoveEvent(opc_Event);
      opc_Event->accept();
      this->setAutoExpandDelay(s32_Delay);
   }
   else
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SdNdeDataPoolListTableView * const pc_Table =
         dynamic_cast<const C_SdNdeDataPoolListTableView * const>(opc_Event->source());

      if (pc_Table != NULL)
      {
         QTreeWidget::dragMoveEvent(opc_Event);
         opc_Event->ignore();
      }
      else
      {
         opc_Event->ignore();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten selection changed event slot

   Here: minimize last selected item & signal selection change

   \param[in] orc_Selected   New selected items
   \param[in] orc_Deselected Last selected items

   \created     20.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::selectionChanged(const QItemSelection & orc_Selected,
                                                      const QItemSelection & orc_Deselected)
{
   bool q_IsTopLevel = true;
   const QModelIndexList c_DeselectedIndexList = orc_Deselected.indexes();
   const QModelIndexList c_SelectedIndexList = orc_Selected.indexes();

   QTreeWidget::selectionChanged(orc_Selected, orc_Deselected);

   //Check if top level
   for (QModelIndexList::const_iterator c_ItIndex = c_SelectedIndexList.begin();
        c_ItIndex != c_SelectedIndexList.end();
        ++c_ItIndex)
   {
      if (c_ItIndex->parent().isValid() == true)
      {
         q_IsTopLevel = false;
      }
   }

   //If top level collapse
   if ((q_IsTopLevel == true) && (c_SelectedIndexList.size() > 0))
   {
      m_ClearTableSelection();
   }
   m_CheckActions();
   //Notify selection on top level
   for (QModelIndexList::const_iterator c_ItIndex = c_DeselectedIndexList.begin();
        c_ItIndex != c_DeselectedIndexList.end();
        ++c_ItIndex)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdNdeDataPoolListHeaderWidget * const pc_HeaderWidget =
         dynamic_cast<C_SdNdeDataPoolListHeaderWidget * >(this->itemWidget(this->itemFromIndex(*c_ItIndex), 0));
      if (pc_HeaderWidget != NULL)
      {
         pc_HeaderWidget->NotifySelection(false);
      }
   }
   for (QModelIndexList::const_iterator c_ItIndex = c_SelectedIndexList.begin();
        c_ItIndex != c_SelectedIndexList.end();
        ++c_ItIndex)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdNdeDataPoolListHeaderWidget * const pc_HeaderWidget =
         dynamic_cast<C_SdNdeDataPoolListHeaderWidget * >(this->itemWidget(this->itemFromIndex(*c_ItIndex), 0));
      if (pc_HeaderWidget != NULL)
      {
         pc_HeaderWidget->NotifySelection(true);
      }
   }

   //Notify parent
   Q_EMIT this->SigSelectionChanged(m_GetSelectedIndices().size(), true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: handle list paste

   \param[in,out] opc_Event Event identification and information

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   bool q_CallOrig = true;

   //Debug commands
   if (((opc_Event->modifiers().testFlag(Qt::ShiftModifier) == true) &&
        (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)) &&
       (opc_Event->modifiers().testFlag(Qt::AltModifier) == true))
   {
      switch (opc_Event->key())
      {
      case Qt::Key_O:
         //Undo redo command view
         this->mc_UndoManager.ToggleCommandDisplay();
         break;
      default:
         break;
      }
   }
   //Standard commands
   switch (opc_Event->key())
   {
   case Qt::Key_C:
      if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
      {
         Copy();
      }
      break;
   case Qt::Key_X:
      if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
      {
         Cut();
      }
      break;
   case Qt::Key_V:
      if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
      {
         Paste();
      }
      break;
   case Qt::Key_Z:
      if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
      {
         this->mc_UndoManager.DoUndo();
      }
      break;
   case Qt::Key_Y:
      if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
      {
         this->mc_UndoManager.DoRedo();
      }
      break;
   case Qt::Key_Delete:
      Delete();
      break;
   case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
   case Qt::Key_Plus:
      if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         Insert(false);
      }
      break;
   case Qt::Key_Up:
      if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         DoMoveUp();
      }
      break;
   case Qt::Key_Down:
      if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         DoMoveDown();
      }
      break;
   case Qt::Key_F2:
      Edit();
      q_CallOrig = false;
      break;
   case Qt::Key_F11:
      PopUp();
      break;
   default:
      //Nothing to do
      break;
   }

   if (q_CallOrig == true)
   {
      QTreeWidget::keyPressEvent(opc_Event);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten start drag event

   Here: start drag manually (for custom preview)

   \param[in] oc_SupportedActions Supported actions

   \created     14.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::startDrag(const Qt::DropActions oc_SupportedActions)
{
   const QList<QTreeWidgetItem *> c_SelectedItems = this->selectedItems();

   if (c_SelectedItems.size() > 0)
   {
      //Manual drag
      QDrag * const pc_Drag = new QDrag(this);

      pc_Drag->setMimeData(this->mimeData(c_SelectedItems));
      pc_Drag->exec(oc_SupportedActions, this->defaultDropAction());
      //lint -e{429}  no memory leak because of the parent of pc_Drag and the Qt memory management
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle expand request

   \param[in] opc_Item  Affected tree item widget
   \param[in] oq_Expand true: expand
                        false collapse

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_OnExpandRequested(QTreeWidgetItem * const opc_Item, const bool oq_Expand)
{
   if (oq_Expand == true)
   {
      this->expand(this->indexFromItem(opc_Item));
   }
   else
   {
      this->collapse(this->indexFromItem(opc_Item));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set up new tree item widget

   \param[in,out] opc_Item   Tree item widget to set up
   \param[in]     os32_Index According index

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_InitialItemConfigure(QTreeWidgetItem * const opc_Item, const sint32 os32_Index)
{
   C_SdNdeDataPoolListHeaderWidget * const pc_ListItem = new C_SdNdeDataPoolListHeaderWidget(NULL, opc_Item,
                                                                                             &this->mc_UndoManager,
                                                                                             &this->mc_ModelViewManager,
                                                                                             this->mu32_NodeIndex,
                                                                                             this->mu32_DataPoolIndex,
                                                                                             os32_Index);

   connect(pc_ListItem, &C_SdNdeDataPoolListHeaderWidget::SigSave, this,
           &C_SdNdeDataPoolListsTreeWidget::SigSave);
   connect(pc_ListItem, &C_SdNdeDataPoolListHeaderWidget::SigSaveAs, this,
           &C_SdNdeDataPoolListsTreeWidget::SigSaveAs);
   connect(pc_ListItem, &C_SdNdeDataPoolListHeaderWidget::SigExpand, this,
           &C_SdNdeDataPoolListsTreeWidget::m_OnExpandRequested);
   connect(pc_ListItem, &C_SdNdeDataPoolListHeaderWidget::SigUpdateAddress, this,
           &C_SdNdeDataPoolListsTreeWidget::m_UpdateAddress);
   //Connect error change
   connect(pc_ListItem, &C_SdNdeDataPoolListHeaderWidget::SigErrorChange, this,
           &C_SdNdeDataPoolListsTreeWidget::HandleErrorChange);
   //Connect size change to top level
   connect(pc_ListItem, &C_SdNdeDataPoolListHeaderWidget::SigUpdateAddress, this,
           &C_SdNdeDataPoolListsTreeWidget::m_HandleSizeChange);
   //Selection handling
   connect(pc_ListItem, &C_SdNdeDataPoolListHeaderWidget::SigExclusiveSelection, this,
           &C_SdNdeDataPoolListsTreeWidget::m_HandleExclusiveListSelection);

   //Drag
   //Skip drop enabled
   opc_Item->setFlags(opc_Item->flags() ^ Qt::ItemIsDropEnabled);

   this->setItemWidget(opc_Item, 0, pc_ListItem);
   {
      //Table
      QTreeWidgetItem * const pc_Table =
         new QTreeWidgetItem(opc_Item, static_cast<sintn>(QTreeWidgetItem::ItemType::UserType));
      C_SdNdeDataPoolListTableWidget * const pc_TableWidget = new C_SdNdeDataPoolListTableWidget(NULL, pc_Table,
                                                                                                 &this->mc_UndoManager);

      //Init list index first!
      pc_TableWidget->SetList(this->mu32_NodeIndex, this->mu32_DataPoolIndex, static_cast<uint32>(os32_Index));
      //Init table manager
      pc_TableWidget->SetModelViewManager(&this->mc_ModelViewManager);

      //Connect error change
      connect(pc_TableWidget, &C_SdNdeDataPoolListTableWidget::SigErrorChangePossible, pc_ListItem,
              &C_SdNdeDataPoolListHeaderWidget::CheckError);
      //Update header on internal usage change
      connect(pc_TableWidget, &C_SdNdeDataPoolListTableWidget::SigSizeChangePossible, this,
              &C_SdNdeDataPoolListsTreeWidget::RegisterSizeChange);
      //Connect size change to header
      connect(pc_TableWidget, &C_SdNdeDataPoolListTableWidget::SigErrorChangePossible, pc_ListItem,
              &C_SdNdeDataPoolListHeaderWidget::VariablesSizeChange);
      //Table selection
      connect(pc_TableWidget, &C_SdNdeDataPoolListTableWidget::SigSelectionChanged, this,
              &C_SdNdeDataPoolListsTreeWidget::m_HandleTableSelection);
      //Table button status
      //lint -e{64,918,1025,1703} Qt interface
      connect(pc_TableWidget, &C_SdNdeDataPoolListTableWidget::SigButtonChange, this,
              &C_SdNdeDataPoolListsTreeWidget::m_OnButtonChange);
      //Reset after pop up
      connect(pc_ListItem, &C_SdNdeDataPoolListHeaderWidget::SigUpdateTable, pc_TableWidget,
              &C_SdNdeDataPoolListTableWidget::Reset);

      //Disable drag & drop
      pc_Table->setFlags(pc_Table->flags() ^ Qt::ItemIsDragEnabled);
      pc_Table->setFlags(pc_Table->flags() ^ Qt::ItemIsDropEnabled);

      this->setItemWidget(pc_Table, 0, pc_TableWidget);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move item in tree

   \param[in] oru32_SourceIndices Source index
   \param[in] oru32_TargetIndices Target index

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_Move(const std::vector<uint32> & oru32_SourceIndices,
                                            const std::vector<uint32> & oru32_TargetIndices)
{
   this->mc_UndoManager.DoMoveList(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this,
                                   oru32_SourceIndices, oru32_TargetIndices);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init from data pool data

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_InitFromData(void)
{
   C_OSCNodeDataPool c_OSCDataPool;
   C_PuiSdNodeDataPool c_UIDataPool;

   if (C_PuiSdHandler::h_GetInstance()->GetDataPool(this->mu32_NodeIndex, this->mu32_DataPoolIndex, c_OSCDataPool,
                                                    c_UIDataPool) == C_NO_ERR)
   {
      for (uint32 u32_It = 0; u32_It < c_OSCDataPool.c_Lists.size(); ++u32_It)
      {
         this->AddEntry();
         {
            QTreeWidgetItem * const pc_HeaderItem = this->topLevelItem(u32_It);

            if (pc_HeaderItem != NULL)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               C_SdNdeDataPoolListHeaderWidget * const pc_HeaderWidget =
                  dynamic_cast<C_SdNdeDataPoolListHeaderWidget * const>(this->itemWidget(pc_HeaderItem, 0));

               if (pc_HeaderWidget != NULL)
               {
                  //Init
                  pc_HeaderWidget->SetIndex(u32_It);
               }
               //Table
               if (pc_HeaderItem->childCount() > 0)
               {
                  QTreeWidgetItem * const pc_TableItem = pc_HeaderItem->child(0);
                  if (pc_TableItem != NULL)
                  {
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     C_SdNdeDataPoolListTableWidget * const pc_TableWidget =
                        dynamic_cast<C_SdNdeDataPoolListTableWidget * const>(this->itemWidget(pc_TableItem, 0));

                     if (pc_TableWidget != NULL)
                     {
                        pc_TableWidget->SetList(this->mu32_NodeIndex, this->mu32_DataPoolIndex, u32_It);
                     }
                  }
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update address values

   \created     30.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_UpdateAddress(void)
{
   this->UpdateUI();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position

   \created     03.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QModelIndex c_Current = this->indexAt(orc_Pos);

   if ((c_Current.isValid() == true) && (c_Current.parent().isValid() == false))
   {
      this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init context menu entries

   \created     03.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   this->mpc_ActionAdd = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                             "Add new List"), this, &C_SdNdeDataPoolListsTreeWidget::InsertAction,
                                                          static_cast<sintn>(Qt::CTRL) +
                                                          static_cast<sintn>(Qt::Key_Plus));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Cut"), this, &C_SdNdeDataPoolListsTreeWidget::Cut,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Copy"), this, &C_SdNdeDataPoolListsTreeWidget::Copy,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_C));
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Paste"), this, &C_SdNdeDataPoolListsTreeWidget::Paste,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_V));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionMoveUp = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                "Move Up"), this,
                                                             &C_SdNdeDataPoolListsTreeWidget::DoMoveUp,
                                                             static_cast<sintn>(Qt::CTRL) +
                                                             static_cast<sintn>(Qt::Key_Up));
   this->mpc_ActionMoveDown = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                  "Move Down"), this,
                                                               &C_SdNdeDataPoolListsTreeWidget::DoMoveDown,
                                                               static_cast<sintn>(Qt::CTRL) +
                                                               static_cast<sintn>(Qt::Key_Down));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Delete"), this, &C_SdNdeDataPoolListsTreeWidget::Delete,
                                    static_cast<sintn>(Qt::Key_Delete));

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdNdeDataPoolListsTreeWidget::customContextMenuRequested, this,
           &C_SdNdeDataPoolListsTreeWidget::m_OnCustomContextMenuRequested);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle collapse event

   \param[in] orc_Index Collapsed item

   \created     07.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_OnCollapse(const QModelIndex & orc_Index) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdNdeDataPoolListHeaderWidget * const pc_Widget =
      dynamic_cast<C_SdNdeDataPoolListHeaderWidget * const>(this->indexWidget(orc_Index));

   if (pc_Widget != NULL)
   {
      pc_Widget->RegisterExpandOrCollapse(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle expand event

   \param[in] orc_Index Expanded item

   \created     07.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_OnExpand(const QModelIndex & orc_Index) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdNdeDataPoolListHeaderWidget * const pc_Widget =
      dynamic_cast<C_SdNdeDataPoolListHeaderWidget * const>(this->indexWidget(orc_Index));

   if (pc_Widget != NULL)
   {
      pc_Widget->RegisterExpandOrCollapse(true);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle error change signal of any data set item

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index
   \param[in] oru32_ListIndex     List index

   \created     24.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_HandleDataSetErrorChange(const uint32 & oru32_NodeIndex,
                                                                const uint32 & oru32_DataPoolIndex,
                                                                const uint32 & oru32_ListIndex) const
{
   QTreeWidgetItem * const pc_HeaderItem = this->topLevelItem(oru32_ListIndex);

   tgl_assert((oru32_NodeIndex == this->mu32_NodeIndex) && (oru32_DataPoolIndex == this->mu32_DataPoolIndex));

   if (pc_HeaderItem != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdNdeDataPoolListHeaderWidget * const pc_HeaderWidget =
         dynamic_cast<C_SdNdeDataPoolListHeaderWidget * const>(this->itemWidget(pc_HeaderItem, 0));
      if (pc_HeaderWidget != NULL)
      {
         pc_HeaderWidget->CheckError();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle size change signal of any list item

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_HandleSizeChange(void)
{
   Q_EMIT this->SigSizeChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle table selction change

   \param[in] oru32_ListIndex Source list
   \param[in] oru32_Count     Number of selected items

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_HandleTableSelection(const uint32 & oru32_ListIndex, const uint32 & oru32_Count)
{
   if (oru32_Count > 0)
   {
      this->clearSelection();
      m_ClearTableSelection(static_cast<sint32>(oru32_ListIndex));
   }
   Q_EMIT this->SigSelectionChanged(oru32_Count, false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get selected indices

   \return
   Selected indices

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<uint32> C_SdNdeDataPoolListsTreeWidget::m_GetSelectedIndices(void) const
{
   std::vector<uint32> c_Retval;
   const QList<QTreeWidgetItem *> c_SelectedItems = this->selectedItems();

   for (QList<QTreeWidgetItem *>::const_iterator c_ItSelectedItem = c_SelectedItems.begin();
        c_ItSelectedItem != c_SelectedItems.end(); ++c_ItSelectedItem)
   {
      QTreeWidgetItem * const pc_Item = *c_ItSelectedItem;
      if (pc_Item->parent() == NULL)
      {
         c_Retval.push_back(this->indexOfTopLevelItem(pc_Item));
      }
   }
   C_Uti::h_Uniqueify(c_Retval);
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \param[in] orq_AddActive      Push button add active
   \param[in] orq_CutActive      Push button cut active
   \param[in] orq_CopyActive     Push button copy active
   \param[in] orq_PasteActive    Push button paste active
   \param[in] orq_DeleteActive   Push button delete active
   \param[in] orq_MoveDownActive Push button move down active
   \param[in] orq_MoveUpActive   Push button move up active

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_OnButtonChange(const bool & orq_AddActive, const bool & orq_CutActive,
                                                      const bool & orq_CopyActive, const bool & orq_PasteActive,
                                                      const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                                                      const bool & orq_MoveUpActive)
{
   Q_EMIT this->SigButtonChange(orq_AddActive, orq_CutActive, orq_CopyActive, orq_PasteActive, orq_DeleteActive,
                                orq_MoveDownActive, orq_MoveUpActive);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Recheck if actions statuses are still up to date

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_CheckActions(void)
{
   const std::vector<uint32> c_SelectedIndices = m_GetSelectedIndices();

   if (c_SelectedIndices.size() > 0)
   {
      bool q_AllowAdd = true;
      bool q_AllowMoveUp;
      bool q_AllowMoveDown;

      //Add
      if (CheckDataPoolFull() == true)
      {
         q_AllowAdd = false;
      }
      this->mq_AllowAdd = q_AllowAdd;
      //Move down & up
      q_AllowMoveUp = true;
      q_AllowMoveDown = true;
      for (uint32 u32_ItSelectedIndex = 0UL; u32_ItSelectedIndex < c_SelectedIndices.size(); ++u32_ItSelectedIndex)
      {
         if (c_SelectedIndices[u32_ItSelectedIndex] == 0UL)
         {
            q_AllowMoveUp = false;
         }
         if (c_SelectedIndices[u32_ItSelectedIndex] == static_cast<uint32>(this->topLevelItemCount() - 1))
         {
            q_AllowMoveDown = false;
         }
      }
      //Actions
      this->mq_AllowMoveDown = q_AllowMoveDown;
      if (this->mpc_ActionMoveDown != NULL)
      {
         this->mpc_ActionMoveDown->setEnabled(q_AllowMoveDown);
      }
      this->mq_AllowMoveUp = q_AllowMoveUp;
      if (this->mpc_ActionMoveUp != NULL)
      {
         this->mpc_ActionMoveUp->setEnabled(q_AllowMoveUp);
      }
      //Buttons
      Q_EMIT this->SigButtonChange(true, true, true, true, true, q_AllowMoveDown, q_AllowMoveUp);
   }
   else
   {
      //Buttons
      Q_EMIT this->SigButtonChange(true, false, false, false, false, false, false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get active table

   \param[in] orq_IgnoreSelectedItems Optional indicator if selected item size should be ignored

   \return
   NULL No active table
   else Active table

   \created     01.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListTableWidget * C_SdNdeDataPoolListsTreeWidget::m_GetActiveTable(const bool & orq_IgnoreSelectedItems)
const
{
   C_SdNdeDataPoolListTableWidget * pc_Retval = NULL;
   bool q_Continue = true;

   //If requested by user ignore list size
   if (orq_IgnoreSelectedItems == false)
   {
      if (m_GetSelectedIndices().size() > 0)
      {
         q_Continue = false;
      }
   }
   if (q_Continue == true)
   {
      //Search for collapsed list
      for (sint32 s32_ItList = 0; s32_ItList < static_cast<sint32>(this->topLevelItemCount()); ++s32_ItList)
      {
         const QTreeWidgetItem * const pc_ListItem = this->topLevelItem(static_cast<sintn>(s32_ItList));
         if (pc_ListItem != NULL)
         {
            if (pc_ListItem->isExpanded())
            {
               QTreeWidgetItem * const pc_TableWidgetItem = pc_ListItem->child(0);
               if (pc_TableWidgetItem != NULL)
               {
                  //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                  C_SdNdeDataPoolListTableWidget * const pc_Table =
                     dynamic_cast<C_SdNdeDataPoolListTableWidget *>(this->itemWidget(pc_TableWidgetItem, 0));
                  if ((pc_Table != NULL) && (pc_Table->IsSelected() == true))
                  {
                     pc_Retval = pc_Table;
                  }
               }
            }
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get active table tree widget item

   \param[in] orq_IgnoreSelectedItems Optional indicator if selected item size should be ignored

   \return
   NULL No active table tree widget item
   Else Active table tree widget item

   \created     13.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QTreeWidgetItem * C_SdNdeDataPoolListsTreeWidget::m_GetActiveTableTreeWidget(const bool & orq_IgnoreSelectedItems) const
{
   QTreeWidgetItem * pc_Retval = NULL;
   bool q_Continue = true;

   //If requested by user ignore list size
   if (orq_IgnoreSelectedItems == false)
   {
      if (m_GetSelectedIndices().size() > 0)
      {
         q_Continue = false;
      }
   }
   if (q_Continue == true)
   {
      //Search for collapsed list
      for (sint32 s32_ItList = 0; s32_ItList < static_cast<sint32>(this->topLevelItemCount()); ++s32_ItList)
      {
         const QTreeWidgetItem * const pc_ListItem = this->topLevelItem(static_cast<sintn>(s32_ItList));
         if (pc_ListItem != NULL)
         {
            if (pc_ListItem->isExpanded())
            {
               QTreeWidgetItem * const pc_TableWidgetItem = pc_ListItem->child(0);
               if (pc_TableWidgetItem != NULL)
               {
                  //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                  C_SdNdeDataPoolListTableWidget * const pc_Table =
                     dynamic_cast<C_SdNdeDataPoolListTableWidget *>(this->itemWidget(pc_TableWidgetItem, 0));
                  if ((pc_Table != NULL) && (pc_Table->IsSelected() == true))
                  {
                     pc_Retval = pc_TableWidgetItem;
                  }
               }
            }
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt target position of drop action

   \param[in]     orc_SelectedIndices       Selected indices
   \param[in]     ore_DropIndicatorPosition Drop indicator position
   \param[in,out] ors32_TargetPosition      Target position (Requirement: valid initialization)

   \created     14.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::mh_AdaptDropTargetIndex(const std::vector<uint32> & orc_SelectedIndices,
                                                             const QAbstractItemView::DropIndicatorPosition & ore_DropIndicatorPosition,
                                                             sint32 & ors32_TargetPosition)
{
   sint32 s32_Offset = 0;
   bool q_FirstSelected = false;

   //If drag below inserted adapt target row
   for (uint32 u32_ItSelected = 0; u32_ItSelected < orc_SelectedIndices.size(); ++u32_ItSelected)
   {
      if (static_cast<sint32>(orc_SelectedIndices[u32_ItSelected]) <= ors32_TargetPosition)
      {
         ++s32_Offset;
      }
      if (orc_SelectedIndices[u32_ItSelected] == 0)
      {
         q_FirstSelected = true;
      }
   }
   //Check if first item is dropped on first item
   //  -> no index adaptations necessary
   if ((q_FirstSelected == true) && (ors32_TargetPosition == 0))
   {
      //No adaptation necessary
   }
   else
   {
      ors32_TargetPosition -= s32_Offset;
      //If indicator below item insert at next index
      if (ore_DropIndicatorPosition != QAbstractItemView::AboveItem)
      {
         ++ors32_TargetPosition;
      }
   }
   //Always check if it is positive
   ors32_TargetPosition = std::max(ors32_TargetPosition, 0L);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle change detected

   \created     14.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_HandleChanged(void)
{
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle data set size change

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex List index

   \created     20.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_UpdateDataSetCount(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_ListIndex) const
{
   if ((this->mu32_NodeIndex == oru32_NodeIndex) && (this->mu32_DataPoolIndex == oru32_DataPoolIndex))
   {
      QTreeWidgetItem * const pc_TreeWidgetItem = this->topLevelItem(static_cast<uint32>(oru32_ListIndex));
      if (pc_TreeWidgetItem != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SdNdeDataPoolListHeaderWidget * const pc_HeaderWidget =
            dynamic_cast<C_SdNdeDataPoolListHeaderWidget * const>(this->indexWidget(this->indexFromItem(
                                                                                       pc_TreeWidgetItem)));
         if (pc_HeaderWidget != NULL)
         {
            pc_HeaderWidget->UpdateDataSetCount();
         }
      }
   }
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_ScrollBarRangeChanged(const sintn osn_Min, const sintn osn_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((osn_Min == 0) && (osn_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get index of element after highest selected element

   \return
   Index of element after highest selected element

   \created     23.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_SdNdeDataPoolListsTreeWidget::m_GetOneAfterHighestSelected(void) const
{
   uint32 u32_Retval = 0;
   const std::vector<uint32> c_SelectedItems = this->m_GetSelectedIndices();

   if (c_SelectedItems.size() > 0)
   {
      for (uint32 u32_ItSelected = 0; u32_ItSelected < c_SelectedItems.size(); ++u32_ItSelected)
      {
         const uint32 u32_Cur = c_SelectedItems[u32_ItSelected];
         u32_Retval = std::max(u32_Retval, u32_Cur);
      }
      ++u32_Retval;
   }
   else
   {
      if (this->topLevelItemCount() >= 0)
      {
         u32_Retval = static_cast<uint32>(this->topLevelItemCount());
      }
   }
   return u32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \param[in] orq_Error Changed error status

   \created     24.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_OnErrorChangePossible(const bool & orq_Error)
{
   Q_EMIT this->SigErrorChange(orq_Error);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Disable all table selections

   \param[in] ors32_Exception Optional parameter to allow one table to not have its selection cleared

   \created     31.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_ClearTableSelection(const sint32 & ors32_Exception) const
{
   for (sint32 s32_It = 0; s32_It < this->topLevelItemCount(); ++s32_It)
   {
      if (ors32_Exception != s32_It)
      {
         QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(s32_It);
         if (pc_TopLevelItem != NULL)
         {
            QTreeWidgetItem * const pc_TableWidgetItem = pc_TopLevelItem->child(0);
            if (pc_TableWidgetItem != NULL)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               C_SdNdeDataPoolListTableWidget * const pc_Table =
                  dynamic_cast<C_SdNdeDataPoolListTableWidget * const>(this->itemWidget(pc_TableWidgetItem, 0));

               //Clear table selection
               if (pc_Table != NULL)
               {
                  pc_Table->ClearSelection();
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle exclusive list selection

   \param[in] oru32_ListIndex List index

   \created     08.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_HandleExclusiveListSelection(const uint32 & oru32_ListIndex) const
{
   for (sint32 s32_It = 0; s32_It < this->topLevelItemCount(); ++s32_It)
   {
      QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(s32_It);
      if (pc_TopLevelItem != NULL)
      {
         if (static_cast<sint32>(oru32_ListIndex) != s32_It)
         {
            pc_TopLevelItem->setSelected(false);
         }
         else
         {
            pc_TopLevelItem->setSelected(true);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Store user settings

   \created     23.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_StoreUserSettings(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   const C_OSCNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(this->mu32_NodeIndex,
                                                                                                 this->mu32_DataPoolIndex);

   if ((pc_Node != NULL) && (pc_Datapool != NULL))
   {
      std::vector<QString> c_ExpandedListNames;
      std::vector<QString> c_SelectedListNames;
      std::vector<QString> c_SelectedVariableNames;

      //Expanded items
      for (sint32 s32_It = 0; s32_It < this->topLevelItemCount(); ++s32_It)
      {
         QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(s32_It);
         if ((pc_TopLevelItem != NULL) && (pc_TopLevelItem->isExpanded() == true))
         {
            const uint32 u32_ListIndex = static_cast<uint32>(s32_It);
            if (u32_ListIndex < pc_Datapool->c_Lists.size())
            {
               const C_OSCNodeDataPoolList & rc_List = pc_Datapool->c_Lists[u32_ListIndex];
               c_ExpandedListNames.push_back(rc_List.c_Name.c_str());
            }
         }
      }
      C_UsHandler::h_GetInstance()->SetProjSdNodeDatapoolOpenListNames(pc_Node->c_Properties.c_Name.c_str(),
                                                                       pc_Datapool->c_Name.c_str(),
                                                                       c_ExpandedListNames);

      //Selected items
      for (sint32 s32_It = 0; s32_It < this->topLevelItemCount(); ++s32_It)
      {
         QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(s32_It);
         if (pc_TopLevelItem != NULL)
         {
            if (pc_TopLevelItem->isSelected() == true)
            {
               const uint32 u32_ListIndex = static_cast<uint32>(s32_It);
               //Lists
               if (u32_ListIndex < pc_Datapool->c_Lists.size())
               {
                  const C_OSCNodeDataPoolList & rc_List = pc_Datapool->c_Lists[u32_ListIndex];
                  c_SelectedListNames.push_back(rc_List.c_Name.c_str());
               }
            }
            else
            {
               QTreeWidgetItem * const pc_TableWidgetItem = pc_TopLevelItem->child(0);
               //Variables
               if (pc_TableWidgetItem != NULL)
               {
                  //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                  C_SdNdeDataPoolListTableWidget * const pc_Table =
                     dynamic_cast<C_SdNdeDataPoolListTableWidget * const>(this->itemWidget(pc_TableWidgetItem, 0));

                  //Check table selection
                  if (pc_Table != NULL)
                  {
                     const std::vector<QString> c_CurSelectedVariableNames = pc_Table->GetSelectedVariableNames();
                     if (c_CurSelectedVariableNames.size() > 0)
                     {
                        const uint32 u32_ListIndex = static_cast<uint32>(s32_It);
                        c_SelectedVariableNames = c_CurSelectedVariableNames;
                        if (u32_ListIndex < pc_Datapool->c_Lists.size())
                        {
                           const C_OSCNodeDataPoolList & rc_List = pc_Datapool->c_Lists[u32_ListIndex];
                           c_SelectedListNames.push_back(rc_List.c_Name.c_str());
                        }
                     }
                  }
               }
            }
         }
      }
      C_UsHandler::h_GetInstance()->SetProjSdNodeDatapoolSelectedListNames(pc_Node->c_Properties.c_Name.c_str(),
                                                                           pc_Datapool->c_Name.c_str(),
                                                                           c_SelectedListNames);
      C_UsHandler::h_GetInstance()->SetProjSdNodeDatapoolSelectedVariableNames(pc_Node->c_Properties.c_Name.c_str(),
                                                                               pc_Datapool->c_Name.c_str(),
                                                                               c_SelectedVariableNames);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Restore user settings

   \created     23.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeWidget::m_RestoreUserSettings(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   const C_OSCNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(this->mu32_NodeIndex,
                                                                                                 this->mu32_DataPoolIndex);

   if ((pc_Node != NULL) && (pc_Datapool != NULL))
   {
      const C_UsNode c_Node = C_UsHandler::h_GetInstance()->GetProjSdNode(pc_Node->c_Properties.c_Name.c_str());
      const C_UsNodeDatapool c_Datapool = c_Node.GetDatapool(pc_Datapool->c_Name.c_str());
      const std::vector<QString> & rc_ExpandedListNames = c_Datapool.GetExpandedListNames();
      const std::vector<QString> & rc_SelectedListNames = c_Datapool.GetSelectedListNames();
      const std::vector<QString> & rc_SelectedVariableNames = c_Datapool.GetSelectedVariableNames();

      //Expanded items
      for (sint32 s32_It = 0; s32_It < this->topLevelItemCount(); ++s32_It)
      {
         QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(s32_It);
         if (pc_TopLevelItem != NULL)
         {
            const uint32 u32_ListIndex = static_cast<uint32>(s32_It);
            if (u32_ListIndex < pc_Datapool->c_Lists.size())
            {
               const C_OSCNodeDataPoolList & rc_List = pc_Datapool->c_Lists[u32_ListIndex];
               const QString c_ListName = rc_List.c_Name.c_str();
               bool q_Expand = false;
               for (uint32 u32_ItName = 0; u32_ItName < rc_ExpandedListNames.size(); ++u32_ItName)
               {
                  if (c_ListName.compare(rc_ExpandedListNames[u32_ItName]) == 0)
                  {
                     q_Expand = true;
                     break;
                  }
               }
               pc_TopLevelItem->setExpanded(q_Expand);
            }
         }
      }

      //Selected items
      if ((rc_SelectedVariableNames.size() > 0) && (rc_SelectedListNames.size() == 1))
      {
         //Variables
         for (sint32 s32_It = 0; s32_It < this->topLevelItemCount(); ++s32_It)
         {
            QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(s32_It);
            if (pc_TopLevelItem != NULL)
            {
               const uint32 u32_ListIndex = static_cast<uint32>(s32_It);
               if (u32_ListIndex < pc_Datapool->c_Lists.size())
               {
                  const C_OSCNodeDataPoolList & rc_List = pc_Datapool->c_Lists[u32_ListIndex];
                  const QString c_ListName = rc_List.c_Name.c_str();
                  bool q_Selection = false;
                  for (uint32 u32_ItName = 0; u32_ItName < rc_SelectedListNames.size(); ++u32_ItName)
                  {
                     if (c_ListName.compare(rc_SelectedListNames[u32_ItName]) == 0)
                     {
                        q_Selection = true;
                        break;
                     }
                  }
                  if (q_Selection == true)
                  {
                     QTreeWidgetItem * const pc_TableWidgetItem = pc_TopLevelItem->child(0);
                     if (pc_TableWidgetItem != NULL)
                     {
                        //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                        C_SdNdeDataPoolListTableWidget * const pc_Table =
                           dynamic_cast<C_SdNdeDataPoolListTableWidget * const>(this->itemWidget(pc_TableWidgetItem,
                                                                                                 0));

                        //Check table selection
                        if (pc_Table != NULL)
                        {
                           pc_Table->SetSelectedVariableNames(rc_SelectedVariableNames);
                        }
                     }
                  }
               }
            }
         }
      }
      else
      {
         //Lists
         for (sint32 s32_It = 0; s32_It < this->topLevelItemCount(); ++s32_It)
         {
            QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(s32_It);
            if (pc_TopLevelItem != NULL)
            {
               const uint32 u32_ListIndex = static_cast<uint32>(s32_It);
               if (u32_ListIndex < pc_Datapool->c_Lists.size())
               {
                  const C_OSCNodeDataPoolList & rc_List = pc_Datapool->c_Lists[u32_ListIndex];
                  const QString c_ListName = rc_List.c_Name.c_str();
                  bool q_Selection = false;
                  for (uint32 u32_ItName = 0; u32_ItName < rc_SelectedListNames.size(); ++u32_ItName)
                  {
                     if (c_ListName.compare(rc_SelectedListNames[u32_ItName]) == 0)
                     {
                        q_Selection = true;
                        //If selected, scroll there
                        this->scrollToItem(pc_TopLevelItem);
                        break;
                     }
                  }
                  pc_TopLevelItem->setSelected(q_Selection);
               }
            }
         }
      }
   }
}
