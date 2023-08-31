//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Max performance view (implementation)

   Max performance view

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>
#include <QHeaderView>
#include <QApplication>
#include <algorithm> //for std::sort
#include <QDrag>

#include "stwerrors.hpp"
#include "constants.hpp"
#include "cam_constants.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_UsHandler.hpp"
#include "C_CamProHandler.hpp"
#include "C_CamMetTreeView.hpp"
#include "C_CamMetClipBoardHelper.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
//lint -emacro(*,Q_DECLARE_METATYPE)   //macro provided by Qt library; nothing we can do about messages
Q_DECLARE_METATYPE(C_CamMetTreeLoggerData)

const int32_t C_CamMetTreeView::mhs32_COL_WIDTH_TIME_STAMP = 150;
const int32_t C_CamMetTreeView::mhs32_COL_WIDTH_CAN_ID = 81;
const int32_t C_CamMetTreeView::mhs32_COL_WIDTH_CAN_NAME = 317;
const int32_t C_CamMetTreeView::mhs32_COL_WIDTH_CAN_DIR = 44;
const int32_t C_CamMetTreeView::mhs32_COL_WIDTH_CAN_DLC = 44;
const int32_t C_CamMetTreeView::mhs32_COL_WIDTH_CAN_DATA = 247;
const int32_t C_CamMetTreeView::mhs32_COL_WIDTH_CAN_COUNTER = 97;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
//PC-Lint suppressions: No problem with having the mutex stored as member instead of a static member
//lint -save -e2751
C_CamMetTreeView::C_CamMetTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent),
   C_SyvComMessageMonitor(),
   mq_UniqueMessageMode(false),
   mq_IsRunning(false),
   mq_AllowSorting(false)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(ms32_USER_ROLE_SORT);
   this->C_CamMetTreeView::setModel(&mc_SortProxyModel);
   //Delete last selection model, see Qt documentation for setModel
   delete pc_LastSelectionModel;

   // Scrollbar
   this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

   this->header()->setStretchLastSection(false);

   this->setSelectionMode(QAbstractItemView::ExtendedSelection);

   //Buttons
   this->mpc_PushButtonScrollTop = new C_OgePubIconOnly(this->verticalScrollBar());
   this->mpc_PushButtonScrollBottom = new C_OgePubIconOnly(this->verticalScrollBar());

   this->header()->setFixedHeight(27);

   // Size is necessary for correct drawing of the images
   this->mpc_PushButtonScrollTop->setGeometry(0, 0, 17, 46);
   this->mpc_PushButtonScrollBottom->setGeometry(0, 0, 17, 17);

   this->mpc_PushButtonScrollTop->SetCustomIcons("://images/ScrollUp.png", "", "", "");
   this->mpc_PushButtonScrollBottom->SetCustomIcons("://images/ScrollDown.png", "", "", "");

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //Deactivate trace sorting as there are some known issues (not allowed while active, signals cut off)
   if (mq_AllowSorting)
   {
      this->setSortingEnabled(true);
      this->mc_SortProxyModel.setDynamicSortFilter(true);
   }
   else
   {
      this->setSortingEnabled(false);
      this->mc_SortProxyModel.setDynamicSortFilter(false);
   }

   this->m_SetupContextMenu();

   //Model
   this->mc_TimerHandleMessages.setInterval(90);
   this->setItemDelegate(&this->mc_Delegate);
   this->mc_GuiBuffer.moveToThread(&mc_ThreadGuiBuffer);
   this->mc_ThreadGuiBuffer.start();
   //lint -e{1938}  static const is guaranteed preinitialized before main
   qRegisterMetaType<std::list<C_CamMetTreeLoggerData> >();
   connect(&this->mc_GuiBuffer, &C_CamMetTreeGuiBuffer::SigUpdateUi, this, &C_CamMetTreeView::m_UpdateUi);
   connect(&this->mc_Delegate, &C_CamMetTreeDelegate::SigStartAccept, &this->mc_Model, &C_CamMetTreeModel::UnlockData);
   connect(&this->mc_Delegate, &C_CamMetTreeDelegate::SigEndAccept, &this->mc_Model, &C_CamMetTreeModel::LockData);

   // Connect for getting CAN messages
   connect(&this->mc_TimerHandleMessages, &QTimer::timeout, this, &C_CamMetTreeView::m_HandleMessages);
   //Button connects
   connect(this->mpc_PushButtonScrollTop, &QPushButton::clicked, this, &C_CamMetTreeView::m_DoScrollTop);
   connect(this->mpc_PushButtonScrollBottom, &QPushButton::clicked, this, &C_CamMetTreeView::m_DoScrollBottom);
   connect(this, &C_CamMetTreeView::collapsed, this, &C_CamMetTreeView::m_OnCollapse);
   //User settings
   connect(
      C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration, this,
      &C_CamMetTreeView::m_RestoreUserSettings);

   connect(this, &C_CamMetTreeView::SigEmitAddFilter, this, &C_CamMetTreeView::m_AddFilter);
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} never took ownership of any context menu item or button
C_CamMetTreeView::~C_CamMetTreeView() noexcept
{
   try
   {
      mc_ThreadGuiBuffer.quit();
      mc_ThreadGuiBuffer.wait();
   }
   catch (...)
   {
      //not much we can do here ...
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Continues the paused the logging
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::Continue(void)
{
   this->mq_IsRunning = true;
   //Sorting
   m_HandleSorting();

   this->mc_Model.Continue();

   C_SyvComMessageMonitor::Continue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Pauses the logging.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::Pause(void)
{
   this->mq_IsRunning = false;
   m_HandleSorting();

   this->mc_Model.Pause();

   C_SyvComMessageMonitor::Pause();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to react on the stop of the communication
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::Stop(void)
{
   this->mq_IsRunning = false;
   m_HandleSorting();

   this->mc_Model.Stop();

   C_SyvComMessageMonitor::Stop();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to react on the stop of the communication
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::Start(void)
{
   this->mq_IsRunning = true;
   m_HandleSorting();
   //While running no child nodes are available (only selection causes issues)
   this->clearSelection();

   this->mc_Model.Start();

   C_SyvComMessageMonitor::Start();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle action: clear data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::ActionClearData(void)
{
   this->m_HandleMessages();
   this->mc_GuiBuffer.ClearBuffer();
   this->mc_Model.ActionClearData();
   this->ResetCounter();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the protocol for interpreting

   \param[in]  oe_Protocol    Set protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SetProtocol(const stw::cmon_protocol::e_CanMonL7Protocols oe_Protocol)
{
   std::vector<C_CamMetTreeLoggerData *> c_Messages;
   C_SyvComMessageMonitor::SetProtocol(oe_Protocol);
   //Change all existing messages
   c_Messages = this->mc_Model.GetAllMessagesForProtocolChange();
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < c_Messages.size(); ++u32_ItMessage)
   {
      this->m_UpdateProtocolString(*c_Messages[u32_ItMessage]);
   }
   //Update all related columns
   this->mc_Model.SignalProtocolChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display mode: display tree

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SetDisplayTree(const bool oq_Value)
{
   //While tree is initialized no new data should be accepted
   this->mc_MutexUpdate.lock();
   this->mc_Model.SetDisplayTree(oq_Value);
   this->mc_MutexUpdate.unlock();
   m_SetAllChildren();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display mode: display unique messages

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SetDisplayUniqueMessages(const bool oq_Value)
{
   this->mq_UniqueMessageMode = oq_Value;
   this->mc_Model.SetDisplayUniqueMessages(oq_Value);
   m_SetAllChildren();
   m_HandleSorting();

   if (oq_Value == false)
   {
      // Change to continuous mode, scroll to the end
      this->scrollTo(this->mc_Model.index(this->mc_Model.rowCount() - 1, 0));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display style for ID

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SetDisplayAsHex(const bool oq_Value)
{
   this->mc_Model.SetDisplayAsHex(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display style for timestamp

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SetDisplayTimestampRelative(const bool oq_Value)
{
   this->mc_Model.SetDisplayTimestampRelative(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display style for absolute timestamp

   If relative timestamp is active, this mode does not change the trace

   \param[in]  oq_Value    New value
                           true: Timestamp with time of day
                           false: Timestamp beginning at start of measurement
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SetDisplayTimestampAbsoluteTimeOfDay(const bool oq_Value)
{
   this->mc_Model.SetDisplayTimestampAbsoluteTimeOfDay(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets a new size of the trace buffer

   This will affect the maximum shown messages in the trace view

   \param[in]  ou32_Value  New size of trace buffer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SetTraceBufferSize(const uint32_t ou32_Value)
{
   this->mc_Model.SetTraceBufferSize(ou32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns display style for CAN ID and CAN data

   \return
   true     Displaying hex
   false    Displaying decimal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeView::GetDisplayAsHex() const
{
   return this->mc_Model.GetDisplayAsHex();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns display style for timestamp.

   \return
   true     Displaying relative timestamp
   false    Displaying absolute timestamp
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeView::GetDisplayTimestampRelative() const
{
   return this->mc_Model.GetDisplayTimestampRelative();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search a matching item

   \param[in]  orc_SearchString  String to search in model
   \param[in]  oq_Next           Flag for search direction
                                 true:  Search the next entry, forward
                                 false: Search the previous entry, backward
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SearchTrace(const QString & orc_SearchString, const bool oq_Next)
{
   QItemSelection c_Selection;
   int32_t s32_RowSignal;
   int32_t s32_RowMultiplexedSignal;
   const int32_t s32_Row = this->mc_Model.SearchMessageData(orc_SearchString, oq_Next, s32_RowSignal,
                                                            s32_RowMultiplexedSignal);

   if (s32_Row >= 0)
   {
      // Select the result
      if (s32_RowSignal < 0)
      {
         // Message only
         c_Selection.select(this->mc_Model.index(s32_Row, 0),
                            this->mc_Model.index(s32_Row, this->mc_Model.columnCount() - 1));
         this->scrollTo(this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(s32_Row, 0)));
      }
      else
      {
         // Signal of message
         const QModelIndex c_ParentIndex =
            this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(s32_Row, 0));
         const QModelIndex c_ChildIndex =
            this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(s32_RowSignal, 0,
                                                                       this->mc_SortProxyModel.mapToSource(
                                                                          c_ParentIndex)));
         QModelIndex c_SelectIndex = c_ChildIndex;

         // Expand the parent message
         this->expand(c_ParentIndex);

         // Check for a multiplexed signal as result
         if (s32_RowMultiplexedSignal >= 0)
         {
            c_SelectIndex =
               this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(s32_RowMultiplexedSignal, 0,
                                                                          this->mc_SortProxyModel.mapToSource(
                                                                             c_ChildIndex)));
            this->expand(c_ChildIndex);
         }

         c_Selection.select(c_SelectIndex, c_SelectIndex);
         this->scrollTo(c_SelectIndex);
      }

      // Select the element
      this->selectionModel()->select(c_Selection,
                                     QItemSelectionModel::ClearAndSelect);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SaveUserSettings(void) const
{
   C_UsHandler::h_GetInstance()->SetTraceColWidths(this->GetCurrentColumnWidths());
   C_UsHandler::h_GetInstance()->SetTraceColPositions(this->m_GetCurrentColumnPositionIndices());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore column widths

   \param[in]  orc_ColumnWidths  Stored column widths (Restores default values if empty)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::SetCurrentColumnWidths(const std::vector<int32_t> & orc_ColumnWidths)
{
   if (orc_ColumnWidths.size() > 0)
   {
      for (uint32_t u32_ItCol = 0; u32_ItCol < orc_ColumnWidths.size(); ++u32_ItCol)
      {
         this->setColumnWidth(static_cast<int32_t>(u32_ItCol), orc_ColumnWidths[u32_ItCol]);
      }
   }
   else
   {
      //Default
      this->setColumnWidth(C_CamMetTreeModel::h_EnumToColumn(C_CamMetTreeModel::eTIME_STAMP),
                           mhs32_COL_WIDTH_TIME_STAMP);
      this->setColumnWidth(C_CamMetTreeModel::h_EnumToColumn(C_CamMetTreeModel::eCAN_ID), mhs32_COL_WIDTH_CAN_ID);
      this->setColumnWidth(C_CamMetTreeModel::h_EnumToColumn(C_CamMetTreeModel::eCAN_NAME), mhs32_COL_WIDTH_CAN_NAME);
      this->setColumnWidth(C_CamMetTreeModel::h_EnumToColumn(C_CamMetTreeModel::eCAN_DIR), mhs32_COL_WIDTH_CAN_DIR);
      this->setColumnWidth(C_CamMetTreeModel::h_EnumToColumn(C_CamMetTreeModel::eCAN_DLC), mhs32_COL_WIDTH_CAN_DLC);
      this->setColumnWidth(C_CamMetTreeModel::h_EnumToColumn(C_CamMetTreeModel::eCAN_DATA), mhs32_COL_WIDTH_CAN_DATA);
      this->setColumnWidth(C_CamMetTreeModel::h_EnumToColumn(C_CamMetTreeModel::eCAN_COUNTER),
                           mhs32_COL_WIDTH_CAN_COUNTER);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current column widths

   \return
   Current column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<int32_t> C_CamMetTreeView::GetCurrentColumnWidths() const
{
   std::vector<int32_t> c_Retval;
   c_Retval.reserve(this->model()->columnCount());
   for (int32_t s32_ItCol = 0; s32_ItCol < this->model()->columnCount(); ++s32_ItCol)
   {
      c_Retval.push_back(this->columnWidth(s32_ItCol));
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function call of connect mechanism from parent widget constructor. Throwing Filter popup on dragged item
 *  by calling the function "m_AddFilter"

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::CanFilterMsgDropped()
{
   C_CamMetTreeView::m_AddFilter();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten show event slot

   Here: move scroll bar buttons, start delayed timer

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::showEvent(QShowEvent * const opc_Event)
{
   QTreeView::showEvent(opc_Event);
   m_RepositionButtons();
   this->mc_TimerHandleMessages.start();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten resize event slot

   Here: move scroll bar buttons

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::resizeEvent(QResizeEvent * const opc_Event)
{
   QTreeView::resizeEvent(opc_Event);
   m_RepositionButtons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten selection changed event slot

   Here: update model with current selection

   \param[in]  orc_Selected      New selected items
   \param[in]  orc_Deselected    Last selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected)
{
   QModelIndexList c_SelectedItems;

   //Update selected items
   QTreeView::selectionChanged(orc_Selected, orc_Deselected);
   //Get selected items
   c_SelectedItems = this->selectedIndexes();
   //Special handling possible for single selection -> signal
   if (c_SelectedItems.size() == 1L)
   {
      for (QModelIndexList::ConstIterator c_It = c_SelectedItems.begin(); c_It != c_SelectedItems.end(); ++c_It)
      {
         const QModelIndex c_CurIndex = this->mc_SortProxyModel.mapToSource(*c_It);
         if (c_CurIndex.parent().isValid() == true)
         {
            this->mc_Model.SetSelection(
               c_CurIndex.parent().parent().isValid() ?
               static_cast<int32_t>(c_CurIndex.parent().parent().row()) : static_cast<int32_t>(c_CurIndex.parent().row()),
               static_cast<int32_t>(this->mc_Model.TranslateTreeRowsToSignalIndex(c_CurIndex)));
         }
         else
         {
            this->mc_Model.SetSelection(-1, -1);
         }
      }
   }
   else
   {
      this->mc_Model.SetSelection(-1, -1);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden key press event

   Here: Handle copy CAN message

   \param[in,out]  opc_Event  Pointer to key event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::keyPressEvent(QKeyEvent * const opc_Event)
{
   if ((opc_Event->key() == static_cast<int32_t>(Qt::Key_C)) &&
       (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      this->m_CopySelection();
   }
   else
   {
      QTreeView::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Draw points for visualizing branches

   \param[in,out]  opc_Painter   Current painter
   \param[in]      orc_Rect      Rectangle area for drawing
   \param[in]      orc_Index     Index of item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::drawBranches(QPainter * const opc_Painter, const QRect & orc_Rect,
                                    const QModelIndex & orc_Index) const
{
   // Adapt the color
   QPen c_Pen = opc_Painter->pen();
   const QPoint c_TopLeft = orc_Rect.topLeft();
   const int32_t s32_OFFSET_L1 = 10;
   const int32_t s32_OFFSET_L2 = 30;
   const int32_t s32_OFFSET_L3 = 50;

   c_Pen.setColor(mc_STYLE_GUIDE_COLOR_10);
   c_Pen.setWidth(1);
   opc_Painter->save();
   opc_Painter->setPen(c_Pen);

   //Message
   if (orc_Index.parent().isValid() == false)
   {
      const QModelIndex c_MessageIndex = orc_Index;
      if (this->model()->rowCount(c_MessageIndex) > 0)
      {
         //Don't draw with arrow
      }
      else
      {
         if (c_MessageIndex.sibling(c_MessageIndex.row() + 1, 0).isValid())
         {
            //Draw +
            C_CamMetTreeView::mh_DrawPlus(opc_Painter, c_TopLeft, s32_OFFSET_L1);
         }
         else
         {
            //Draw L
            C_CamMetTreeView::mh_DrawEl(opc_Painter, c_TopLeft, s32_OFFSET_L1);
         }
      }
   }
   else if (orc_Index.parent().parent().isValid() == false)
   {
      const QModelIndex c_MessageIndex = orc_Index.parent();
      //Draw |
      if (c_MessageIndex.sibling(c_MessageIndex.row() + 1, 0).isValid())
      {
         C_CamMetTreeView::mh_DrawVerticalLine(opc_Painter, c_TopLeft, s32_OFFSET_L1);
      }
   }
   else
   {
      const QModelIndex c_MessageIndex = orc_Index.parent().parent();
      //Draw |
      if (c_MessageIndex.sibling(c_MessageIndex.row() + 1, 0).isValid())
      {
         C_CamMetTreeView::mh_DrawVerticalLine(opc_Painter, c_TopLeft, s32_OFFSET_L1);
      }
   }
   //Signal
   if (orc_Index.parent().parent().isValid())
   {
      const QModelIndex c_SignalIndex = orc_Index.parent();
      //Draw |
      if (c_SignalIndex.sibling(c_SignalIndex.row() + 1, 0).isValid())
      {
         C_CamMetTreeView::mh_DrawVerticalLine(opc_Painter, c_TopLeft, s32_OFFSET_L2);
      }
   }
   else if (orc_Index.parent().isValid())
   {
      const QModelIndex c_SignalIndex = orc_Index;
      if (this->model()->rowCount(c_SignalIndex) > 0)
      {
         //Don't draw with arrow
      }
      else
      {
         if (c_SignalIndex.sibling(c_SignalIndex.row() + 1, 0).isValid())
         {
            //Draw +
            C_CamMetTreeView::mh_DrawPlus(opc_Painter, c_TopLeft, s32_OFFSET_L2);
         }
         else
         {
            //Draw L
            C_CamMetTreeView::mh_DrawEl(opc_Painter, c_TopLeft, s32_OFFSET_L2);
         }
      }
   }
   else
   {
      //No signal
   }
   if (orc_Index.parent().parent().isValid())
   {
      //Mux
      const QModelIndex c_MuxIndex = orc_Index;
      if (this->model()->rowCount(c_MuxIndex) > 0)
      {
         //Don't draw with arrow
      }
      else
      {
         if (c_MuxIndex.sibling(c_MuxIndex.row() + 1, 0).isValid())
         {
            //Draw +
            C_CamMetTreeView::mh_DrawPlus(opc_Painter, c_TopLeft, s32_OFFSET_L3);
         }
         else
         {
            //Draw L
            C_CamMetTreeView::mh_DrawEl(opc_Painter, c_TopLeft, s32_OFFSET_L3);
         }
      }
   }
   opc_Painter->restore();

   QTreeView::drawBranches(opc_Painter, orc_Rect, orc_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the hint for size column

   Will be used when resizeColumnToContents is used for the specific column.
   resizeColumnToContents is used by double click on the separator in the header too.

   \param[in]  os32_Column  Index of column

   \return
   Width of column in pixel
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamMetTreeView::sizeHintForColumn(const int32_t os32_Column) const
{
   int32_t s32_Size;
   static const int32_t has32_SIZES[static_cast<int32_t>(C_CamMetTreeModel::eCAN_COUNTER) + 1] =
   {
      mhs32_COL_WIDTH_TIME_STAMP,
      mhs32_COL_WIDTH_CAN_ID,
      mhs32_COL_WIDTH_CAN_NAME,
      mhs32_COL_WIDTH_CAN_DIR,
      mhs32_COL_WIDTH_CAN_DLC,
      mhs32_COL_WIDTH_CAN_DATA,
      mhs32_COL_WIDTH_CAN_COUNTER
   };

   if ((os32_Column <= static_cast<int32_t>(C_CamMetTreeModel::eCAN_COUNTER)) && (os32_Column >= 0))
   {
      s32_Size = has32_SIZES[os32_Column];
   }
   else
   {
      s32_Size = C_OgeTreeViewToolTipBase::sizeHintForColumn(os32_Column);
   }

   return s32_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten start drag event

   \param[in,out] oc_SupportedActions Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::startDrag(const Qt::DropActions oc_SupportedActions)
{
   if (this->selectedIndexes().size() > 0)
   {
      QModelIndexList pc_Item = this->selectedIndexes();

      QPixmap c_Pix;
      QDrag * const pc_Drag = new QDrag(this);
      QMimeData * const pc_MimeData = new QMimeData();

      pc_MimeData->setData("application/x-qabstractitemmodeldatalist", this->model()->data(pc_Item[0]).toByteArray());

      const QIcon c_Icon("://images/IconMessageSelected.svg");
      c_Pix = c_Icon.pixmap(25, 25);
      pc_Drag->setPixmap(c_Pix);
      pc_Drag->setHotSpot(c_Pix.rect().center());
      pc_Drag->setMimeData(pc_MimeData);

      pc_Drag->exec(oc_SupportedActions);
      this->clearSelection();
   } //lint !e429  //no memory leak because of the parent and the Qt memory management
     //              and QDrag does take ownership on setMimeData
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   this->mpc_ActionCopy = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                              "Copy as Text"), this, &C_CamMetTreeView::m_CopySelection,
                                                           static_cast<int32_t>(Qt::CTRL) +
                                                           static_cast<int32_t>(Qt::Key_C));

   this->mpc_AddFilter = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Add to Receive Filter"),
                                                          this, &C_CamMetTreeView::m_OnAddFilterClicked);

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionExpandAll = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Expand all"),
                                                                this, &C_CamMetTreeView::m_ExpandAll);

   this->mpc_ActionCollapseAll = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Collapse all"),
                                                                  this, &C_CamMetTreeView::m_CollapseAll);

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_CamMetTreeView::customContextMenuRequested, this,
           &C_CamMetTreeView::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const bool q_Selected = (this->selectedIndexes().size() > 0L);

   if ((q_Selected == true) ||
       (this->mc_SortProxyModel.rowCount() > 0))
   {
      this->mpc_ActionCopy->setVisible(q_Selected);

      this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy selected items to clipboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_CopySelection(void)
{
   QModelIndexList c_SelectedItems;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   c_SelectedItems = this->selectedIndexes();

   //selectedIndexes returns a list of all selected indexes, but the indexes are NOT sorted
   //we need to sort them due to we want same row order as displayed in the trace
   std::sort(c_SelectedItems.begin(), c_SelectedItems.end(),
             [] (const QModelIndex & orc_It1, const QModelIndex & orc_It2)
   {
      return orc_It1.row() < orc_It2.row();
   }
             );

   if (c_SelectedItems.size() > 0L)
   {
      std::vector<int32_t> c_AddedTopLevelRows;
      std::vector<C_CamMetClipBoardHelperCanMessageData> c_CanMessagesData;

      for (QModelIndexList::ConstIterator c_It = c_SelectedItems.begin(); c_It != c_SelectedItems.end(); ++c_It)
      {
         // Get the correct top layer of the tree
         QModelIndex c_TopLevelIndex = *c_It;

         // Each column is selected, but we only want one entry for each row
         if (c_TopLevelIndex.column() == 0)
         {
            C_CamMetClipBoardHelperCanMessageData c_CanMessageData;
            if (c_TopLevelIndex.parent().isValid() == true)
            {
               // In case of a selected signal
               //Add to expanded children if multiplexer child
               if (c_TopLevelIndex.parent().parent().isValid())
               {
                  //Multiplexer child
                  c_CanMessageData.c_ExpandedIndices.push_back(c_TopLevelIndex.parent().row());

                  c_TopLevelIndex = c_TopLevelIndex.parent().parent();
               }
               else
               {
                  //Simple signal
                  c_TopLevelIndex = c_TopLevelIndex.parent();
               }
               c_CanMessageData.q_Extended = true;
            }
            else
            {
               c_CanMessageData.q_Extended = this->isExpanded(c_TopLevelIndex);
            }

            {
               bool q_Add = true;
               //Look up message already added
               const int32_t s32_NewRow = this->mc_SortProxyModel.mapToSource(c_TopLevelIndex).row();
               for (uint32_t u32_AlreadyAdded = 0UL; (u32_AlreadyAdded < c_AddedTopLevelRows.size()) && (q_Add);
                    ++u32_AlreadyAdded)
               {
                  if (s32_NewRow == c_AddedTopLevelRows[u32_AlreadyAdded])
                  {
                     q_Add = false;
                  }
               }
               if (q_Add)
               {
                  //Handle expanded children
                  for (int32_t s32_ItChild = 0; s32_ItChild < this->mc_SortProxyModel.rowCount(c_TopLevelIndex);
                       ++s32_ItChild)
                  {
                     const QModelIndex c_Index = this->mc_SortProxyModel.index(s32_ItChild, 0, c_TopLevelIndex);
                     if (this->isExpanded(c_Index))
                     {
                        c_CanMessageData.c_ExpandedIndices.push_back(s32_ItChild);
                     }
                  }

                  // Save the info for this message data
                  c_CanMessageData.pc_MessageData =
                     this->mc_Model.GetMessageData(s32_NewRow);

                  c_CanMessagesData.push_back(c_CanMessageData);

                  //We always will remember
                  c_AddedTopLevelRows.push_back(s32_NewRow);
               }
            }
         }
      }

      if (c_CanMessagesData.size() > 0)
      {
         C_CamMetClipBoardHelper::h_StoreCanMessages(this->mc_Model.GetDisplayAsHex(),
                                                     this->mc_Model.GetDisplayTimestampRelative(),
                                                     this->mc_Model.GetDisplayTimestampAbsoluteTimeOfDay(),
                                                     c_CanMessagesData);
      }
   }

   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets messages and bring them to the model
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_HandleMessages(void)
{
   C_CamMetTreeLoggerData c_Msg;

   while (this->m_GetCanMessage(c_Msg) == C_NO_ERR)
   {
      this->mc_GuiBuffer.HandleData(c_Msg);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add data to UI

   \param[in]  orc_Data    New data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_UpdateUi(const std::list<C_CamMetTreeLoggerData> & orc_Data)
{
   std::vector<int32_t> c_Rows;
   // Get the maximum of the scroll bar before adding new data
   const int32_t s32_ScrollBarValMax = this->verticalScrollBar()->maximum();

   //Don't allow multiple concurrent add row steps
   this->mc_MutexUpdate.lock();
   c_Rows = this->mc_Model.AddRows(orc_Data);
   if (this->mq_UniqueMessageMode == false)
   {
      // In case of continuous mode, adapt the new messages for showing the signals in correct column size
      m_SetChildColumns(c_Rows);

      // Scroll only in case of continuous mode and when the user has not scrolled up manually
      if (this->verticalScrollBar()->value() == s32_ScrollBarValMax)
      {
         this->scrollTo(this->mc_SortProxyModel.index(this->mc_SortProxyModel.rowCount() - 1, 0));
      }
   }
   else
   {
      // In case of unique mode, adapt all messages for showing the signals in correct column size
      // It is possible that already existing messages got a signal interpretation
      // TODO: Is there a more efficient way?
      this->m_SetAllChildren();
      //update style (necessary so stylesheet sees changes -> necessary for expand and collapse icon update)
      this->style()->unpolish(this);
      this->style()->polish(this);
   }
   this->mc_MutexUpdate.unlock();
   //lint -restore
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set child item stretch property for these rows

   \param[in]  orc_Indices    Rows to stretch all child items for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_SetChildColumns(const std::vector<int32_t> & orc_Indices)
{
   for (uint32_t u32_It = 0; u32_It < orc_Indices.size(); ++u32_It)
   {
      const int32_t s32_ParentRow = orc_Indices[u32_It];
      const QModelIndex c_ParentIndex = this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(s32_ParentRow, 0));
      m_SetChildColumns(c_ParentIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set child item stretch property for this index

   \param[in]  orc_ModelIndex    Stretch all child items for this index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_SetChildColumns(const QModelIndex & orc_ModelIndex)
{
   if (orc_ModelIndex.isValid() == true)
   {
      for (int32_t s32_ItRow = 0; s32_ItRow < this->mc_SortProxyModel.rowCount(orc_ModelIndex); ++s32_ItRow)
      {
         const QModelIndex c_Child = this->mc_SortProxyModel.index(s32_ItRow, 0, orc_ModelIndex);
         if (this->mc_SortProxyModel.rowCount(c_Child) > 0)
         {
            this->m_SetChildColumns(c_Child);
         }
         //Every child needs an expanded row
         this->setFirstColumnSpanned(s32_ItRow, orc_ModelIndex, true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set child item stretch property for all rows
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_SetAllChildren(void)
{
   for (int32_t s32_It = 0; s32_It < this->mc_SortProxyModel.rowCount(); ++s32_It)
   {
      const QModelIndex c_ParentIndex = this->mc_SortProxyModel.index(s32_It, 0);
      m_SetChildColumns(c_ParentIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reposition the buttons as necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_RepositionButtons(void)
{
   const QSize c_BUTTON_SIZE_UP(17, 46);
   const QSize c_BUTTON_SIZE_DOWN(17, 17);

   //Position
   this->mpc_PushButtonScrollTop->setGeometry(QRect(QPoint(0, 0), c_BUTTON_SIZE_UP));
   this->mpc_PushButtonScrollBottom->setGeometry(QRect(QPoint(0,
                                                              this->verticalScrollBar()->height() -
                                                              c_BUTTON_SIZE_DOWN.height()), c_BUTTON_SIZE_DOWN));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle action: scroll to top
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_DoScrollTop(void)
{
   this->scrollTo(this->mc_SortProxyModel.index(0, 0));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle action: scroll to bottom
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_DoScrollBottom(void)
{
   this->scrollTo(this->mc_SortProxyModel.index(this->mc_SortProxyModel.rowCount() - 1, 0));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_RestoreUserSettings(void)
{
   this->SetCurrentColumnWidths(C_UsHandler::h_GetInstance()->GetTraceColWidths());
   this->m_SetColumnPositionIndices(C_UsHandler::h_GetInstance()->GetTraceColPositions());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Collapse all messages and update geometry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_CollapseAll(void)
{
   this->collapseAll();
   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  For adding Filter data from context menu and by drag and drop action, getting the selected row index
 *  complete data. Sending QList of selected messages Can Id's  and IsCanMsgExtended to parent widget for further process
 *  Here the SelectedRowIndexs = (number of selected rows X number of columns in a row) so we filtering
 *  extact number of rows selected by using (SelectedRowIndexs.size() % columns in a row)

   \return
   Emit signal, which will be caught in parent widget (C_CamMetWidget) constructor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_AddFilter()
{
   if (this->selectionModel()->hasSelection())
   {
      QList<int32_t>  c_SelectedRowIndexs;
      QList<int32_t>  c_SelectedRowListMessagesCanId;
      QList<uint8_t>  c_SelectedRowListMessagesXtd;
      const QModelIndexList c_Indices = this->selectedIndexes();
      for (QModelIndexList::const_iterator c_It = c_Indices.begin(); c_It != c_Indices.end(); ++c_It)
      {
         const QModelIndex & rc_Index = *c_It;
         int32_t s32_Row = 0;
         if (this->currentIndex().parent().row() == -1)
         {
            s32_Row = this->mc_SortProxyModel.mapToSource(rc_Index).row();
         }
         else
         {
            s32_Row = this->mc_SortProxyModel.mapToSource(this->currentIndex().parent()).row();
         }
         c_SelectedRowIndexs << s32_Row;
         if (((c_SelectedRowIndexs.size() % 7) == 1) || (c_SelectedRowIndexs.size() == 1))
         {
            if (this->mc_Model.GetMessageData(s32_Row)->c_CanMsg.u8_XTD == 0)
            {
               c_SelectedRowListMessagesCanId << this->mc_Model.GetMessageData(s32_Row)->c_CanIdDec.ToInt();
            }
            else
            {
               c_SelectedRowListMessagesCanId << this->mc_Model.GetMessageData(s32_Row)->c_CanMsg.u32_ID;
            }
            c_SelectedRowListMessagesXtd << this->mc_Model.GetMessageData(s32_Row)->c_CanMsg.u8_XTD;
         }
      }

      Q_EMIT C_CamMetTreeView::SigEmitAddFilterToParentWidget(c_SelectedRowListMessagesCanId,
                                                              c_SelectedRowListMessagesXtd);
      this->clearSelection();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function call on context menu "Add filter" action click. Emit an signal which can be used in connect
 *  mechanism from constructor

   \return
   Emit an signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_OnAddFilterClicked()
{
   Q_EMIT C_CamMetTreeView::SigEmitAddFilter();
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Expand all messages and update geometry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_ExpandAll(void)
{
   this->expandAll();
   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle collapse event

   Here: select collapsed item if child was selected before

   \param[in]  orc_Index   Collapsed item -> should be top level item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_OnCollapse(const QModelIndex & orc_Index)
{
   bool q_ChildSelected = false;

   for (int32_t s32_ItChild = 0L; s32_ItChild < this->mc_SortProxyModel.rowCount(orc_Index); ++s32_ItChild)
   {
      const QModelIndex c_Index = this->mc_SortProxyModel.index(s32_ItChild, 0, orc_Index);
      //Search for selected items and deselect them if necessary
      if (this->selectionModel()->isSelected(c_Index))
      {
         //Complete child row
         const QItemSelection c_SelectionChild(this->mc_SortProxyModel.index(s32_ItChild, 0,
                                                                             orc_Index), this->mc_SortProxyModel.index(
                                                  s32_ItChild, this->mc_SortProxyModel.columnCount(
                                                     orc_Index) - 1, orc_Index));
         //Remember at least one was selected
         q_ChildSelected = true;
         //Unselect all children
         this->selectionModel()->select(c_SelectionChild, QItemSelectionModel::Deselect);
         //No break as more than one might have to be unselected
      }
   }
   if (q_ChildSelected)
   {
      //Complete parent row
      const QItemSelection c_SelectionParent(this->mc_SortProxyModel.index(
                                                orc_Index.row(), 0), this->mc_SortProxyModel.index(
                                                orc_Index.row(), this->mc_SortProxyModel.columnCount() - 1));
      //Just select the new one
      this->selectionModel()->select(c_SelectionParent, QItemSelectionModel::Select);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current column position indices

   \return
   Current column position indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<int32_t> C_CamMetTreeView::m_GetCurrentColumnPositionIndices(void) const
{
   std::vector<int32_t> c_Retval;
   c_Retval.reserve(this->mc_Model.columnCount());
   for (int32_t s32_ItCol = 0L; s32_ItCol < this->mc_Model.columnCount(); ++s32_ItCol)
   {
      c_Retval.push_back(this->header()->visualIndex(s32_ItCol));
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set column position indices

   \param[in]  orc_NewColPositionIndices  New column position indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_SetColumnPositionIndices(const std::vector<int32_t> & orc_NewColPositionIndices)
{
   if (orc_NewColPositionIndices.size() == static_cast<uint32_t>(this->mc_Model.columnCount()))
   {
      //Use new spec as sorting specification
      while (this->m_ColumnsSortedAsExpected(orc_NewColPositionIndices) == false)
      {
         //If not sorted
         for (int32_t s32_ItCol = 0L; s32_ItCol < this->mc_Model.columnCount(); ++s32_ItCol)
         {
            //Find first invalid element
            if (this->header()->visualIndex(s32_ItCol) != orc_NewColPositionIndices[static_cast<uint32_t>(s32_ItCol)])
            {
               //Find element which should be at this position
               for (int32_t s32_ItCol2 = 0L; s32_ItCol2 < this->mc_Model.columnCount(); ++s32_ItCol2)
               {
                  if (this->header()->visualIndex(s32_ItCol2) ==
                      orc_NewColPositionIndices[static_cast<uint32_t>(s32_ItCol)])
                  {
                     //Swap expected index to current position
                     this->header()->moveSection(this->header()->visualIndex(s32_ItCol2),
                                                 this->header()->visualIndex(s32_ItCol));
                     //Trigger sorted check
                     break;
                  }
               }
               //Trigger sorted check
               break;
            }
         }
      }
   }
   else
   {
      //Leave positions as is
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if columns in expected sorting order

   \param[in]  orc_NewColPositionIndices  Expected sorting order

   \return
   True  Sorted or error
   False Unsorted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeView::m_ColumnsSortedAsExpected(const std::vector<int32_t> & orc_NewColPositionIndices) const
{
   bool q_Retval = true;

   if (orc_NewColPositionIndices.size() == static_cast<uint32_t>(this->mc_Model.columnCount()))
   {
      for (int32_t s32_ItCol = 0L; s32_ItCol < this->mc_Model.columnCount(); ++s32_ItCol)
      {
         if (this->header()->visualIndex(s32_ItCol) != orc_NewColPositionIndices[static_cast<uint32_t>(s32_ItCol)])
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle sorting allowed state depending on current state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::m_HandleSorting(void)
{
   //Deactivate trace sorting as there are some known issues (not allowed while active, signals cut off)
   if (mq_AllowSorting)
   {
      //Don't allow sorting while in unique mode as long as this bug is not resolved:
      // https://bugreports.qt.io/browse/QTBUG-27289 (can happen if items were inserted and sorting was active)
      if (this->mq_IsRunning == true)
      {
         //Required sort to allow handling as before (append at bottom) -> before internal deactivate!
         //Overwrite last user sort behavior to avoid jumping back to last sorting behavior on pausing
         this->header()->setSortIndicator(0, Qt::AscendingOrder);
         //User
         this->setSortingEnabled(false);
         //Internal
         this->mc_SortProxyModel.setDynamicSortFilter(false);
      }
      else
      {
         //User
         this->setSortingEnabled(true);
         //Internal
         this->mc_SortProxyModel.setDynamicSortFilter(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Draw L part of branch

   \param[in,out]  opc_Painter   Painter to draw with
   \param[in]      orc_TopLeft   Top left point
   \param[in]      os32_Offset   Offset to paint at
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::mh_DrawEl(QPainter * const opc_Painter, const QPoint & orc_TopLeft, const int32_t os32_Offset)
{
   //Down
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y()));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 2));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 4));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 6));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 8));
   //Right
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset + 2, orc_TopLeft.y() + 8));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset + 4, orc_TopLeft.y() + 8));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset + 6, orc_TopLeft.y() + 8));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset + 8, orc_TopLeft.y() + 8));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Draw "|-" part of branch

   \param[in,out]  opc_Painter   Painter to draw with
   \param[in]      orc_TopLeft   Top left point
   \param[in]      os32_Offset   Offset to paint at
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::mh_DrawPlus(QPainter * const opc_Painter, const QPoint & orc_TopLeft, const int32_t os32_Offset)
{
   //Down
   mh_DrawVerticalLine(opc_Painter, orc_TopLeft, os32_Offset);
   //Right
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset + 2, orc_TopLeft.y() + 8));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset + 4, orc_TopLeft.y() + 8));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset + 6, orc_TopLeft.y() + 8));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset + 8, orc_TopLeft.y() + 8));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Draw vertical line of branch

   \param[in,out]  opc_Painter   Painter to draw with
   \param[in]      orc_TopLeft   Top left point
   \param[in]      os32_Offset   Offset to paint at
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeView::mh_DrawVerticalLine(QPainter * const opc_Painter, const QPoint & orc_TopLeft,
                                           const int32_t os32_Offset)
{
   //Down
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y()));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 2));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 4));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 6));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 8));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 10));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 12));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 14));
   opc_Painter->drawPoint(QPoint(orc_TopLeft.x() + os32_Offset, orc_TopLeft.y() + 16));
}
