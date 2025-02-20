//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget for bus messages (implementation)

   Tree widget for bus messages

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QModelIndex>
#include <QApplication>
#include <QItemSelectionModel>
#include <QStyleOptionViewItem>
#include <QTreeWidgetItemIterator>
#include <QScrollBar>
#include <QMimeData>
#include <QDrag>
#include <QDropEvent>

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "C_SdUtil.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdBueSortHelper.hpp"
#include "C_SdClipBoardHelper.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscCanUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdBueMessageSelectorTreeWidget.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SdBueCoAddSignalsDialog.hpp"
#include "C_SdBueJ1939AddMessagesFromCatalogDialog.hpp"
#include "C_CieImportDataAssignment.hpp"
#include "C_CieDataPoolListAdapter.hpp"
#include "C_CieImportReportWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
C_SdBueMessageSelectorTreeWidget::C_SdBueMessageSelectorTreeWidget(QWidget * const opc_Parent) :
   stw::opensyde_gui_elements::C_OgeTreeWidgetToolTipBase(opc_Parent),
   mpc_UndoManager(NULL),
   mpc_MessageSyncManager(NULL),
   mq_StopSigSelectionChanged(false),
   mq_ModeSingleNode(false),
   mu32_NodeIndex(0),
   mu32_InterfaceIndex(0),
   mu32_BusIndex(0),
   me_ProtocolType(C_OscCanProtocol::eLAYER2),
   mq_NoSelectionUpdate(false)
{
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);
   this->setItemDelegate(&this->mc_Delegate);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   //Drag & drop
   this->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(true);

   //Avoid styling widgets inside
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->verticalScrollBar(), "C_SdBueMessageSelectorTreeWidget", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->horizontalScrollBar(), "C_SdBueMessageSelectorTreeWidget", true);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   this->ReconnectSelectionHandling();
   connect(this, &C_SdBueMessageSelectorTreeWidget::expanded, this,
           &C_SdBueMessageSelectorTreeWidget::m_TreeSizeChanged);
   connect(this, &C_SdBueMessageSelectorTreeWidget::collapsed, this,
           &C_SdBueMessageSelectorTreeWidget::m_TreeSizeChanged);

   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdBueMessageSelectorTreeWidget::m_ScrollBarRangeChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because mpc_UndoManager and mpc_MessageSyncManager are not managed in this module
C_SdBueMessageSelectorTreeWidget::~C_SdBueMessageSelectorTreeWidget(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reimplement default size hint

   \return
   Current required size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SdBueMessageSelectorTreeWidget::sizeHint(void) const
{
   int32_t s32_CounterTopLevelItem;
   int32_t s32_CounterChild;
   int32_t s32_Height = 0;
   QRect c_Rect;

   for (s32_CounterTopLevelItem = 0; s32_CounterTopLevelItem < this->topLevelItemCount(); ++s32_CounterTopLevelItem)
   {
      QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(s32_CounterTopLevelItem);
      c_Rect = this->visualItemRect(pc_TopLevelItem);

      // add the height of the top level item
      s32_Height += c_Rect.height();

      // add the height of the children
      for (s32_CounterChild = 0; s32_CounterChild < pc_TopLevelItem->childCount(); ++s32_CounterChild)
      {
         c_Rect = this->visualItemRect(pc_TopLevelItem->child(s32_CounterChild));
         s32_Height += c_Rect.height();
      }
   }

   // add an offset for the line drawing
   s32_Height += 2;

   return QSize(QTreeWidget::sizeHint().width(), s32_Height);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active node id

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  orc_DatapoolIndexes  All Datapool indexes associated to the same protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SetNodeId(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                                 const std::vector<uint32_t> & orc_DatapoolIndexes)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_InterfaceIndex = ou32_InterfaceIndex;
   this->mc_DatapoolIndexes = orc_DatapoolIndexes;
   this->mq_ModeSingleNode = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active bus id

   \param[in]  ou32_BusIndex  Bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SetBusId(const uint32_t ou32_BusIndex)
{
   this->mu32_BusIndex = ou32_BusIndex;
   this->mq_ModeSingleNode = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current com protocol

   \param[in]  ore_Value   Com protocol value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SetProtocolType(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Value)
{
   this->me_ProtocolType = ore_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Deselects all items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::DeselectAllItems(void)
{
   int32_t s32_CounterTopLevelItem;

   this->mq_StopSigSelectionChanged = true;

   for (s32_CounterTopLevelItem = 0; s32_CounterTopLevelItem < this->topLevelItemCount(); ++s32_CounterTopLevelItem)
   {
      C_SdBueMessageSelectorTreeWidgetItem * const pc_TopLevelItem =
         dynamic_cast<C_SdBueMessageSelectorTreeWidgetItem *>(this->topLevelItem(s32_CounterTopLevelItem));
      if (pc_TopLevelItem != NULL)
      {
         this->m_DeselectChildren(pc_TopLevelItem);
         pc_TopLevelItem->setSelected(false);
         pc_TopLevelItem->setForeground(0, mc_STYLE_GUIDE_COLOR_8);
         pc_TopLevelItem->SetAlwaysActive(false);
      }
   }

   this->mq_StopSigSelectionChanged = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set undo manager

   \param[in,out]  opc_Value  Undo manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SetUndoManager(stw::opensyde_gui_logic::C_SdBueUnoManager * const opc_Value)
{
   this->mpc_UndoManager = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out]  opc_Value  Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SetMessageSyncManager(
   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   this->mpc_MessageSyncManager = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init from data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InitFromData(void)
{
   m_ReloadTree();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::Add(void)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      QModelIndex c_Selection;
      const bool q_Result = this->m_GetHighestSelected(c_Selection);

      //Check either no selection or top level selection
      if ((q_Result == false) || (c_Selection.parent().isValid() == false))
      {
         // Adding message for CANopen is not possible
         if (this->me_ProtocolType != C_OscCanProtocol::eCAN_OPEN)
         {
            //Message
            C_OscCanMessageIdentificationIndices c_MessageId;

            if (this->m_GetMessageIdForAdd(c_MessageId) == C_NO_ERR)
            {
               const bool q_SELECT_SIGNAL = false;

               //Core
               this->mpc_UndoManager->DoAddMessage(c_MessageId, this->mpc_MessageSyncManager, this);

               this->SelectMessage(c_MessageId, false);
               this->AddSignal(q_SELECT_SIGNAL);
            }
         }
      }
      else
      {
         //Signal
         const uint32_t u32_MessageIndex = c_Selection.parent().row();
         if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
         {
            const C_OscCanMessageIdentificationIndices c_MessageId = this->mc_UniqueMessageIds[u32_MessageIndex];
            const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
            if (pc_Message != NULL)
            {
               this->m_AddSignal(u32_MessageIndex);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action add message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::AddMessage(void)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      //Message
      C_OscCanMessageIdentificationIndices c_MessageId;

      if (this->m_GetMessageIdForAdd(c_MessageId) == C_NO_ERR)
      {
         const bool q_SELECT_SIGNAL = false;

         //Core
         this->mpc_UndoManager->DoAddMessage(c_MessageId, this->mpc_MessageSyncManager, this);

         this->SelectMessage(c_MessageId, false);
         this->AddSignal(q_SELECT_SIGNAL);

         Q_EMIT (this->SigSelectName());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Action add message from catalog (J1939 specific)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::AddMessageFromCatalog(void)
{
   const QPointer<C_OgePopUpDialog> c_PopUpCatalog = new C_OgePopUpDialog(this, this);

   C_SdBueJ1939AddMessagesFromCatalogDialog * const pc_AddMessageFromCatalogDialog =
      new C_SdBueJ1939AddMessagesFromCatalogDialog(*c_PopUpCatalog);

   //Resize
   c_PopUpCatalog->SetSize(QSize(1000, 900));

   Q_UNUSED(pc_AddMessageFromCatalogDialog)

   if (c_PopUpCatalog->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      // Assign all imported messages to one node as Tx messages
      C_CieConverter::C_CieNode c_NodeForCatalogMessages;
      c_NodeForCatalogMessages.c_TxMessages =  pc_AddMessageFromCatalogDialog->GetMessagesImportedFromCatalog();

      if (c_NodeForCatalogMessages.c_TxMessages.size() > 0)
      {
         const std::vector<C_CieImportDataAssignment> c_NodeAssignments = m_GetJ1939DefaultNodeAssignment(
            c_NodeForCatalogMessages);

         const std::vector<C_CieImportDataAssignment> c_SkippedImportDataAssigned;

         // Create message report for user
         const QPointer<C_OgePopUpDialog> c_PopUpDialogReportDialog =
            new C_OgePopUpDialog(this, this);
         C_CieImportReportWidget * const pc_DialogImportReport =
            new C_CieImportReportWidget(*c_PopUpDialogReportDialog,
                                        pc_AddMessageFromCatalogDialog->GetCatalogFilePath(), this->mu32_BusIndex,
                                        C_OscCanProtocol::eJ1939, c_NodeAssignments, c_SkippedImportDataAssigned,
                                        NULL, false, true);

         Q_UNUSED(pc_DialogImportReport)

         //Hide previous overlay before showing the next one
         c_PopUpCatalog->HideOverlay();

         // resize
         c_PopUpDialogReportDialog->SetSize(mc_POPUP_REPORT_SIZE);

         // display message report
         if (c_PopUpDialogReportDialog->exec() == static_cast<int32_t>(QDialog::Accepted))
         {
            Q_EMIT (this->SigReload());
            Q_EMIT (this->SigErrorChanged());
         }

         if (c_PopUpDialogReportDialog != NULL)
         {
            c_PopUpDialogReportDialog->HideOverlay();
            c_PopUpDialogReportDialog->deleteLater();
         }
      } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
   }

   if (c_PopUpCatalog != NULL)
   {
      c_PopUpCatalog->HideOverlay();
      c_PopUpCatalog->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_AddMessageFromCatalogDialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add signal from Menu
 *
 *  It is working as a container; otherwise, the parameter that comes from the signal is wrong and so is the slot
 *  signal then
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::AddSignalFromMenu(void)
{
   AddSignal();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action add signal

   \param[in]       oq_SelectSignal     Select signal if true else keep the selection you have
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::AddSignal(const bool oq_SelectSignal)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      QModelIndex c_Selection;
      const bool q_Result = this->m_GetHighestSelected(c_Selection);
      if (q_Result == true)
      {
         uint32_t u32_MessageIndex;
         //Check either no selection or top level selection
         if (c_Selection.parent().isValid() == false)
         {
            //Message
            u32_MessageIndex = c_Selection.row();
         }
         else
         {
            //Signal
            u32_MessageIndex = c_Selection.parent().row();
         }
         if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
         {
            this->m_AddSignal(u32_MessageIndex, oq_SelectSignal);

            if (oq_SelectSignal)
            {
               Q_EMIT (this->SigSelectName());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new signal

   \param[in]  orc_MessageId           Message identification indices
   \param[in]  ou16_StartBit           Start bit for new signal
   \param[in]  oq_MultiplexedSignal    Flag if signal shall be a multiplexed signal
   \param[in]  ou16_MultiplexValue     Concrete multiplexed value if oq_MultiplexedSignal is true
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::AddSignalWithStartBit(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                             const uint16_t ou16_StartBit,
                                                             const bool oq_MultiplexedSignal,
                                                             const uint16_t ou16_MultiplexValue)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
      if (pc_Message != NULL)
      {
         const uint32_t u32_ItSignal = pc_Message->c_Signals.size();

         //Core
         if (oq_MultiplexedSignal == true)
         {
            this->mpc_UndoManager->DoAddSignal(orc_MessageId, u32_ItSignal, ou16_StartBit,
                                               C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL,
                                               ou16_MultiplexValue, this->mpc_MessageSyncManager,
                                               this);
         }
         else
         {
            // Multiplexed CANopen signals are not supported by openSYDE
            if (this->me_ProtocolType != C_OscCanProtocol::eCAN_OPEN)
            {
               this->mpc_UndoManager->DoAddSignal(orc_MessageId, u32_ItSignal, ou16_StartBit,
                                                  C_OscCanSignal::eMUX_DEFAULT,
                                                  0, this->mpc_MessageSyncManager,
                                                  this);
            }
            else
            {
               this->m_AddCoSignal(orc_MessageId, u32_ItSignal, ou16_StartBit);
            }
         }
         this->SelectSignal(orc_MessageId, u32_ItSignal, false);
         Q_EMIT (this->SigSelectName());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::Delete(void)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();

   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      const QModelIndexList c_IndexList = this->selectedIndexes();
      std::vector<C_OscCanMessageIdentificationIndices> c_SelectedMessageIds;
      std::vector<C_OscCanMessageIdentificationIndices> c_SelectedSignalMessageIds;
      std::vector<uint32_t> c_SelectedSignals;
      std::vector<uint32_t> c_Dummy;
      for (QModelIndexList::const_iterator c_ItIndex = c_IndexList.begin(); c_ItIndex != c_IndexList.end(); ++c_ItIndex)
      {
         if (c_ItIndex->parent().isValid() == false)
         {
            //Message
            if (this->me_ProtocolType != C_OscCanProtocol::eCAN_OPEN)
            {
               // Deleting of CANopen PDO messages is not allowed. Deleting is only possible for the other protocols
               if (c_ItIndex->row() >= 0)
               {
                  const uint32_t u32_MessageIndex = c_ItIndex->row();

                  tgl_assert(u32_MessageIndex < this->mc_UniqueMessageIds.size());
                  if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
                  {
                     c_SelectedMessageIds.push_back(this->mc_UniqueMessageIds[u32_MessageIndex]);
                  }
               }
            }
         }
         else
         {
            //Signal
            if (c_ItIndex->parent().row() >= 0)
            {
               const uint32_t u32_MessageIndex = c_ItIndex->parent().row();
               if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
               {
                  if (c_ItIndex->row() >= 0)
                  {
                     const uint32_t u32_SignalIndex = c_ItIndex->row();
                     uint32_t u32_SignalDataIndex;
                     if (this->m_MapSignalInternalIndexToDataIndex(u32_MessageIndex, u32_SignalIndex,
                                                                   u32_SignalDataIndex) == C_NO_ERR)
                     {
                        c_SelectedSignalMessageIds.push_back(this->mc_UniqueMessageIds[u32_MessageIndex]);
                        c_SelectedSignals.push_back(u32_SignalDataIndex);
                     }
                  }
               }
            }
         }
      }
      //Sort messages
      if (c_SelectedMessageIds.size() > 0)
      {
         std::vector<C_OscCanMessageIdentificationIndices> c_Groups;
         std::vector<std::vector<uint32_t> > c_GroupMessages;
         std::vector<std::vector<C_OscCanMessageIdentificationIndices> > c_SortedAscendingMessageGroups;
         c_Groups.push_back(c_SelectedMessageIds[0]);
         c_GroupMessages.resize(1);
         //First element
         c_GroupMessages[0].push_back(c_SelectedMessageIds[0].u32_MessageIndex);
         //Group by node & com protocol & interface & transmit flag
         for (uint32_t u32_ItMessage = 1; u32_ItMessage < c_SelectedMessageIds.size(); ++u32_ItMessage)
         {
            uint32_t u32_ItGroup;
            const C_OscCanMessageIdentificationIndices & rc_CurMessage = c_SelectedMessageIds[u32_ItMessage];
            //Search for matching group
            for (u32_ItGroup = 0; u32_ItGroup < c_Groups.size(); ++u32_ItGroup)
            {
               const C_OscCanMessageIdentificationIndices & rc_CurGroup = c_Groups[u32_ItGroup];

               //Compare everything except message id
               if (((((rc_CurMessage.u32_NodeIndex == rc_CurGroup.u32_NodeIndex) &&
                      (rc_CurMessage.e_ComProtocol == rc_CurGroup.e_ComProtocol)) &&
                     (rc_CurMessage.u32_InterfaceIndex == rc_CurGroup.u32_InterfaceIndex)) &&
                    (rc_CurMessage.u32_DatapoolIndex == rc_CurGroup.u32_DatapoolIndex)) &&
                   (rc_CurMessage.q_MessageIsTx == rc_CurGroup.q_MessageIsTx))
               {
                  c_GroupMessages[u32_ItGroup].push_back(rc_CurMessage.u32_MessageIndex);
                  break;
               }
            }
            //If no match add new group
            if (u32_ItGroup == c_Groups.size())
            {
               c_Groups.push_back(rc_CurMessage);
               c_GroupMessages.resize(c_Groups.size());
               //Add first matching element
               c_GroupMessages[static_cast<std::vector< std::vector< uint32_t> >::size_type >
                               (c_Groups.size() - 1UL)].push_back(rc_CurMessage.u32_MessageIndex);
            }
         }
         //Sort per group
         for (uint32_t u32_ItGroup = 0; u32_ItGroup < c_GroupMessages.size(); ++u32_ItGroup)
         {
            C_SdUtil::h_SortIndicesAscending(c_GroupMessages[u32_ItGroup]);
         }
         //Delete messages per group (Ascending)
         tgl_assert(c_Groups.size() == c_GroupMessages.size());
         c_SortedAscendingMessageGroups.reserve(c_Groups.size());
         for (uint32_t u32_ItGroup = 0; u32_ItGroup < c_Groups.size(); ++u32_ItGroup)
         {
            std::vector<C_OscCanMessageIdentificationIndices> c_Tmp;
            const C_OscCanMessageIdentificationIndices & rc_CurGroup = c_Groups[u32_ItGroup];
            c_Tmp.reserve(c_GroupMessages[u32_ItGroup].size());
            for (uint32_t u32_ItGroupMessage = 0UL; u32_ItGroupMessage < c_GroupMessages[u32_ItGroup].size();
                 ++u32_ItGroupMessage)
            {
               const uint32_t u32_GroupMessageIndex =
                  c_GroupMessages[u32_ItGroup][static_cast<std::vector< uint32_t>::size_type >
                                               (u32_ItGroupMessage)];
               const C_OscCanMessageIdentificationIndices c_MessageId(rc_CurGroup.u32_NodeIndex,
                                                                      rc_CurGroup.e_ComProtocol,
                                                                      rc_CurGroup.u32_InterfaceIndex,
                                                                      rc_CurGroup.u32_DatapoolIndex,
                                                                      rc_CurGroup.q_MessageIsTx,
                                                                      u32_GroupMessageIndex);
               c_Tmp.push_back(c_MessageId);
            }
            c_SortedAscendingMessageGroups.push_back(c_Tmp);
         }
         this->mpc_UndoManager->DoDeleteMessages(c_SortedAscendingMessageGroups, this->mpc_MessageSyncManager,
                                                 this);
      }
      //Sort signals
      c_Dummy.resize(c_SelectedSignals.size());
      C_SdUtil::h_SortIndicesAscendingAndSync<C_OscCanMessageIdentificationIndices, uint32_t>(c_SelectedSignals,
                                                                                              c_SelectedSignalMessageIds,
                                                                                              c_Dummy);
      //Delete signals
      this->mpc_UndoManager->DoDeleteSignals(c_SelectedSignalMessageIds,
                                             c_SelectedSignals, this->mpc_MessageSyncManager, this);
      //Merge all delete commands
      this->mpc_UndoManager->CommitDelete();
   }
   osc_write_log_performance_stop(u16_TimerId, "Bus-Messages-Delete");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::DeleteSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                    const uint32_t ou32_SignalIndex)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      std::vector<C_OscCanMessageIdentificationIndices> c_MessageIds;
      std::vector<uint32_t> c_Signals;
      c_MessageIds.push_back(orc_MessageId);
      c_Signals.push_back(ou32_SignalIndex);
      //Core
      this->mpc_UndoManager->DoDeleteSignals(c_MessageIds, c_Signals,
                                             this->mpc_MessageSyncManager,
                                             this);

      //Merge all delete commands
      this->mpc_UndoManager->CommitDelete();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action copy
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::Copy(void)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();
   const QModelIndexList c_IndexList = this->selectedIndexes();

   if (c_IndexList.size() > 0)
   {
      bool q_MessagesOnly = true;
      bool q_SignalsOnly = true;

      //Check first
      for (QModelIndexList::const_iterator c_ItIndex = c_IndexList.begin(); c_ItIndex != c_IndexList.end(); ++c_ItIndex)
      {
         if (c_ItIndex->parent().isValid() == true)
         {
            q_MessagesOnly = false;
         }
         else
         {
            q_SignalsOnly = false;
         }
      }

      if ((q_MessagesOnly == true) && (this->mpc_MessageSyncManager != NULL))
      {
         uint32_t u32_ItVec = 0;
         std::vector<C_OscCanMessage> c_Messages;
         std::vector<std::vector<C_OscNodeDataPoolListElement> > c_OscSignalCommons;
         std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > c_UiSignalCommons;
         std::vector<C_PuiSdNodeCanMessage> c_UiMessages;
         std::vector<std::vector<QString> > c_OwnerNodeName;
         std::vector<std::vector<uint32_t> > c_OwnerNodeInterfaceIndex;
         std::vector<std::vector<uint32_t> > c_OwnerNodeDatapoolIndex;
         std::vector<std::vector<bool> > c_OwnerIsTxFlag;

         //Reserve
         c_Messages.resize(c_IndexList.size());
         c_OscSignalCommons.resize(c_IndexList.size());
         c_UiSignalCommons.resize(c_IndexList.size());
         c_UiMessages.resize(c_IndexList.size());

         for (QModelIndexList::const_iterator c_ItIndex = c_IndexList.begin(); c_ItIndex != c_IndexList.end();
              ++c_ItIndex)
         {
            tgl_assert(c_ItIndex->row() >= 0);
            if (c_ItIndex->row() >= 0)
            {
               const uint32_t u32_MessageIndex = c_ItIndex->row();
               tgl_assert(u32_MessageIndex < this->mc_UniqueMessageIds.size());
               if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
               {
                  std::vector<QString> c_OwnerNodeNamePerMessage;
                  std::vector<uint32_t> c_OwnerNodeInterfaceIndexPerMessage;
                  std::vector<uint32_t> c_OwnerNodeDatapoolIndexPerMessage;

                  std::vector<bool> c_OwnerIsTxFlagPerMessage;
                  const std::vector<C_OscCanMessageIdentificationIndices> c_MatchingMessages =
                     this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_UniqueMessageIds[
                                                                               u32_MessageIndex]);
                  tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanMessageComplete(this->mc_UniqueMessageIds[
                                                                                       u32_MessageIndex],
                                                                                    c_Messages[u32_ItVec],
                                                                                    c_OscSignalCommons[u32_ItVec],
                                                                                    c_UiSignalCommons[u32_ItVec],
                                                                                    c_UiMessages[u32_ItVec], true) ==
                             C_NO_ERR);

                  //Get owner data
                  c_OwnerNodeNamePerMessage.reserve(c_MatchingMessages.size());
                  c_OwnerNodeInterfaceIndexPerMessage.reserve(c_MatchingMessages.size());
                  c_OwnerNodeDatapoolIndexPerMessage.reserve(c_MatchingMessages.size());
                  c_OwnerIsTxFlagPerMessage.reserve(c_MatchingMessages.size());
                  for (uint32_t u32_ItMatchingMessage = 0; u32_ItMatchingMessage < c_MatchingMessages.size();
                       ++u32_ItMatchingMessage)
                  {
                     const C_OscCanMessageIdentificationIndices & rc_CurMessageId =
                        c_MatchingMessages[u32_ItMatchingMessage];
                     const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
                        rc_CurMessageId.u32_NodeIndex);
                     tgl_assert(pc_Node != NULL);
                     if (pc_Node != NULL)
                     {
                        c_OwnerNodeNamePerMessage.emplace_back(pc_Node->c_Properties.c_Name.c_str());
                     }
                     c_OwnerNodeInterfaceIndexPerMessage.push_back(rc_CurMessageId.u32_InterfaceIndex);
                     c_OwnerNodeDatapoolIndexPerMessage.push_back(rc_CurMessageId.u32_DatapoolIndex);
                     c_OwnerIsTxFlagPerMessage.push_back(rc_CurMessageId.q_MessageIsTx);
                  }
                  c_OwnerNodeName.push_back(c_OwnerNodeNamePerMessage);
                  c_OwnerNodeInterfaceIndex.push_back(c_OwnerNodeInterfaceIndexPerMessage);
                  c_OwnerNodeDatapoolIndex.push_back(c_OwnerNodeDatapoolIndexPerMessage);
                  c_OwnerIsTxFlag.push_back(c_OwnerIsTxFlagPerMessage);
               }
            }
            ++u32_ItVec;
         }
         C_SdClipBoardHelper::h_StoreMessages(c_Messages, c_OscSignalCommons, c_UiSignalCommons, c_UiMessages,
                                              c_OwnerNodeName, c_OwnerNodeInterfaceIndex, c_OwnerNodeDatapoolIndex,
                                              c_OwnerIsTxFlag, this->me_ProtocolType);
      }
      if (q_SignalsOnly == true)
      {
         uint32_t u32_ItVec = 0;
         std::vector<C_OscCanSignal> c_Signals;
         std::vector<C_OscNodeDataPoolListElement> c_OscSignalCommons;
         std::vector<C_PuiSdNodeDataPoolListElement> c_UiSignalCommons;
         std::vector<C_PuiSdNodeCanSignal> c_UiSignals;

         //Reserve
         c_Signals.resize(c_IndexList.size());
         c_OscSignalCommons.resize(c_IndexList.size());
         c_UiSignalCommons.resize(c_IndexList.size());
         c_UiSignals.resize(c_IndexList.size());

         for (QModelIndexList::const_iterator c_ItIndex = c_IndexList.begin(); c_ItIndex != c_IndexList.end();
              ++c_ItIndex)
         {
            tgl_assert(c_ItIndex->parent().row() >= 0);
            if (c_ItIndex->parent().row() >= 0)
            {
               const uint32_t u32_MessageIndex = c_ItIndex->parent().row();
               tgl_assert(u32_MessageIndex < this->mc_UniqueMessageIds.size());
               if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
               {
                  tgl_assert(c_ItIndex->row() >= 0);
                  if (c_ItIndex->row() >= 0)
                  {
                     const uint32_t u32_SignalIndex = c_ItIndex->row();
                     uint32_t u32_SignalDataIndex;
                     if (this->m_MapSignalInternalIndexToDataIndex(u32_MessageIndex, u32_SignalIndex,
                                                                   u32_SignalDataIndex) == C_NO_ERR)
                     {
                        tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanSignalComplete(this->mc_UniqueMessageIds[
                                                                                            u32_MessageIndex],
                                                                                         u32_SignalDataIndex,
                                                                                         c_Signals[u32_ItVec],
                                                                                         c_OscSignalCommons[u32_ItVec],
                                                                                         c_UiSignalCommons[u32_ItVec],
                                                                                         c_UiSignals[u32_ItVec]) ==
                                   C_NO_ERR);
                     }
                  }
               }
            }
            ++u32_ItVec;
         }
         C_SdClipBoardHelper::h_StoreSignalsToClipboard(c_Signals, c_OscSignalCommons, c_UiSignalCommons, c_UiSignals,
                                                        this->me_ProtocolType);
      }
   }
   osc_write_log_performance_stop(u16_TimerId, "Bus-Messages-Copy");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::CopySignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                  const uint32_t ou32_SignalIndex) const
{
   const uint32_t u32_IT_VEC = 0U;

   std::vector<C_OscCanSignal> c_Signals;
   std::vector<C_OscNodeDataPoolListElement> c_OscSignalCommons;
   std::vector<C_PuiSdNodeDataPoolListElement> c_UiSignalCommons;
   std::vector<C_PuiSdNodeCanSignal> c_UiSignals;

   //Reserve
   c_Signals.resize(1);
   c_OscSignalCommons.resize(1);
   c_UiSignalCommons.resize(1);
   c_UiSignals.resize(1);

   tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanSignalComplete(orc_MessageId,
                                                                    ou32_SignalIndex,
                                                                    c_Signals[u32_IT_VEC],
                                                                    c_OscSignalCommons[u32_IT_VEC],
                                                                    c_UiSignalCommons[u32_IT_VEC],
                                                                    c_UiSignals[u32_IT_VEC]) ==
              C_NO_ERR);

   C_SdClipBoardHelper::h_StoreSignalsToClipboard(c_Signals, c_OscSignalCommons, c_UiSignalCommons, c_UiSignals,
                                                  this->me_ProtocolType);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action cut
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::Cut(void)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();

   Copy();

   Delete();
   osc_write_log_performance_stop(u16_TimerId, "Bus-Messages-Cut");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cut specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::CutSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                 const uint32_t ou32_SignalIndex)
{
   this->CopySignal(orc_MessageId, ou32_SignalIndex);
   this->DeleteSignal(orc_MessageId, ou32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action paste
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::Paste(void)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();

   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      std::vector<C_OscCanSignal> c_Signals;
      std::vector<C_OscNodeDataPoolListElement> c_OscSignalCommons;
      std::vector<C_PuiSdNodeDataPoolListElement> c_UiSignalCommons;
      std::vector<C_PuiSdNodeCanSignal> c_UiSignals;
      if (C_SdClipBoardHelper::h_LoadSignalsFromClipboard(c_Signals, c_OscSignalCommons, c_UiSignalCommons,
                                                          c_UiSignals) == C_NO_ERR)
      {
         //Valid signals
         QModelIndex c_Selection;
         const bool q_Result = this->m_GetHighestSelected(c_Selection);
         if (q_Result == true)
         {
            bool q_Continue = false;
            bool q_IsOtherMessage = false;
            uint32_t u32_InternalMessageIndex = 0UL;
            if (c_Selection.parent().isValid() == true)
            {
               //Paste Signals in the same message
               if (c_Selection.parent().row() >= 0)
               {
                  u32_InternalMessageIndex = static_cast<uint32_t>(c_Selection.parent().row());
                  q_Continue = true;
               }
            }
            else
            {
               // Paste Signals in an other Message
               if (c_Selection.row() >= 0)
               {
                  u32_InternalMessageIndex = static_cast<uint32_t>(c_Selection.row());
                  q_IsOtherMessage = true;
                  q_Continue = true;
               }
            }
            if ((q_Continue) && (u32_InternalMessageIndex < this->mc_UniqueMessageIds.size()))
            {
               const C_OscCanMessage * const pc_Message =
                  C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_UniqueMessageIds[
                                                                    u32_InternalMessageIndex]);
               if (pc_Message != NULL)
               {
                  const bool q_ContainsMultiplexer = C_OscCanMessage::h_ContainsMultiplexer(c_Signals);
                  if (pc_Message->IsMultiplexed() && (q_ContainsMultiplexer))
                  {
                     C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
                     c_Message.SetHeading(C_GtGetText::h_GetText("Paste signal"));
                     c_Message.SetDescription(C_GtGetText::h_GetText("Could not paste the copied signals.\n"
                                                                     "At least one of the copied signals is a multiplexer signal,"
                                                                     " the message already contains one multiplexer signal"
                                                                     " and only one multiplexer signal per message is allowed."));
                     c_Message.SetCustomMinHeight(230, 230);
                     c_Message.Execute();
                  }
                  else
                  {
                     const uint32_t u32_SignalIndex = pc_Message->c_Signals.size();

                     //For other message the current signal colors might not work
                     if (q_IsOtherMessage)
                     {
                        //Reset colors
                        c_UiSignals.clear();
                        c_UiSignals.resize(c_Signals.size());
                     }

                     //Auto fix signal restrictions of safety protocols
                     for (uint32_t u32_ItSignal = 0; u32_ItSignal < c_Signals.size(); ++u32_ItSignal)
                     {
                        if ((c_Signals[u32_ItSignal].u32_ComDataElementIndex < c_OscSignalCommons.size()) &&
                            (c_Signals[u32_ItSignal].u32_ComDataElementIndex < c_UiSignalCommons.size()))
                        {
                           C_SdUtil::h_AdaptSignalToProtocolType(
                              c_Signals[u32_ItSignal],
                              c_OscSignalCommons[c_Signals[u32_ItSignal].u32_ComDataElementIndex],
                              c_UiSignalCommons[c_Signals[u32_ItSignal].u32_ComDataElementIndex],
                              this->me_ProtocolType, NULL);
                        }
                     }

                     this->mpc_UndoManager->DoPasteSignals(this->mc_UniqueMessageIds[u32_InternalMessageIndex],
                                                           u32_SignalIndex, c_Signals, c_OscSignalCommons,
                                                           c_UiSignalCommons, c_UiSignals,
                                                           this->mpc_MessageSyncManager,
                                                           this, this->me_ProtocolType);

                     if (this->mc_UniqueMessageIds.size() > 0)
                     {
                        this->SelectSignal(this->mc_UniqueMessageIds[u32_InternalMessageIndex], u32_SignalIndex,
                                           false);
                     }
                     if ((q_ContainsMultiplexer) && (this->mpc_MessageSyncManager != NULL))
                     {
                        //Adapt parent
                        if (pc_Message->e_TxMethod == C_OscCanMessage::eTX_METHOD_ON_CHANGE)
                        {
                           C_OscCanMessage c_Copy = *pc_Message;
                           c_Copy.u32_CycleTimeMs = 0U;
                           c_Copy.e_TxMethod = C_OscCanMessage::eTX_METHOD_ON_EVENT;
                           this->mpc_MessageSyncManager->
                           SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
                              this->mc_UniqueMessageIds[u32_InternalMessageIndex], c_Copy);
                        }
                     }
                  }
               }
            }
         }
      }
      else
      {
         std::vector<C_OscCanMessage> c_Messages;
         std::vector<std::vector<C_OscNodeDataPoolListElement> > c_OscMsgSignalCommons;
         std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > c_UiMsgSignalCommons;
         std::vector<C_PuiSdNodeCanMessage> c_UiMessages;
         std::vector<std::vector<QString> > c_OwnerNodeName;
         std::vector<std::vector<uint32_t> > c_OwnerNodeInterfaceIndex;
         std::vector<std::vector<uint32_t> > c_OwnerNodeDatapoolIndex;
         std::vector<std::vector<bool> > c_OwnerIsTxFlag;
         if (C_SdClipBoardHelper::h_LoadMessages(c_Messages, c_OscMsgSignalCommons, c_UiMsgSignalCommons,
                                                 c_UiMessages, c_OwnerNodeName, c_OwnerNodeInterfaceIndex,
                                                 c_OwnerNodeDatapoolIndex, c_OwnerIsTxFlag) == C_NO_ERR)
         {
            C_OscCanMessageIdentificationIndices c_MessageId;
            //Auto fix messages
            for (uint32_t u32_ItMessage = 0; u32_ItMessage < c_Messages.size(); ++u32_ItMessage)
            {
               if ((u32_ItMessage < c_OscMsgSignalCommons.size()) && (u32_ItMessage < c_UiMsgSignalCommons.size()))
               {
                  C_SdUtil::h_AdaptMessageToProtocolType(c_Messages[u32_ItMessage],
                                                         c_UiMessages[u32_ItMessage],
                                                         c_OscMsgSignalCommons[u32_ItMessage],
                                                         c_UiMsgSignalCommons[u32_ItMessage],
                                                         this->me_ProtocolType, NULL);
               }
            }
            //Valid messages
            if (m_GetMessageIdForAdd(c_MessageId) == C_NO_ERR)
            {
               std::vector<C_OscCanMessageIdentificationIndices> c_NewIds;
               this->mpc_UndoManager->DoPasteMessages(c_MessageId, c_Messages, c_OscMsgSignalCommons,
                                                      c_UiMsgSignalCommons,
                                                      c_UiMessages, c_OwnerNodeName, c_OwnerNodeInterfaceIndex,
                                                      c_OwnerNodeDatapoolIndex,
                                                      c_OwnerIsTxFlag, this->mpc_MessageSyncManager, this, c_NewIds);
               //Selection
               this->SelectMessages(c_NewIds);
            }
         }
      }
   }
   osc_write_log_performance_stop(u16_TimerId, "Bus-Messages-Paste");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paste copied signal

   \param[in]  orc_MessageId  Message identification indices
   \param[in]  ou16_StartBit  Start bit for new signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::PasteSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                   const uint16_t ou16_StartBit)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      std::vector<C_OscCanSignal> c_Signals;
      std::vector<C_OscNodeDataPoolListElement> c_OscSignalCommons;
      std::vector<C_PuiSdNodeDataPoolListElement> c_UiSignalCommons;
      std::vector<C_PuiSdNodeCanSignal> c_UiSignals;
      if (C_SdClipBoardHelper::h_LoadSignalsFromClipboard(c_Signals, c_OscSignalCommons, c_UiSignalCommons,
                                                          c_UiSignals) == C_NO_ERR)
      {
         // pasting of only one concrete signal allowed with a concrete start bit
         if (c_Signals.size() == 1)
         {
            const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
            if (pc_Message != NULL)
            {
               const uint32_t u32_SignalIndex = pc_Message->c_Signals.size();

               //Reset colors
               c_UiSignals.clear();
               c_UiSignals.resize(c_Signals.size());
               c_Signals[0].u16_ComBitStart = ou16_StartBit;
               this->mpc_UndoManager->DoPasteSignals(orc_MessageId,
                                                     u32_SignalIndex, c_Signals, c_OscSignalCommons,
                                                     c_UiSignalCommons, c_UiSignals, this->mpc_MessageSyncManager,
                                                     this, this->me_ProtocolType);
               this->SelectSignal(orc_MessageId, u32_SignalIndex, false);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Collapse all messages and update geometry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::CollapseAll(void)
{
   this->collapseAll();
   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Expand all messages and update geometry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::ExpandAll(void)
{
   this->expandAll();
   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Triggers the editing the name of the actual item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::EditName(void)
{
   const QList<QTreeWidgetItem *> & rc_SelectedItems = this->selectedItems();

   if (rc_SelectedItems.size() == 1)
   {
      Q_EMIT this->SigSelectName();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Disconnect selection handling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::DisconnectSelectionHandling() const
{
   disconnect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this,
              &C_SdBueMessageSelectorTreeWidget::m_SelectionChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reconnect selection handling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::ReconnectSelectionHandling() const
{
   connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this,
           &C_SdBueMessageSelectorTreeWidget::m_SelectionChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class after internal add message

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InternalAddMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   uint32_t u32_InternalMessageIndex;

   //Internal (BEFORE ui)
   m_UpdateUniqueMessageIds();
   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      //Ui
      m_InsertMessage(u32_InternalMessageIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Internal add commit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InternalAddMessageCommit(void)
{
   //Ui update trigger
   this->updateGeometry();
   //Error handling
   RecheckErrorGlobal(false);
   //Signal
   Q_EMIT this->SigMessageCountChanged();
   //Otherwise there is no error update
   Q_EMIT this->SigErrorChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class after internal delete message

   \param[in]  orc_MessageId  Message identification indices

   \return
   Internal message index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdBueMessageSelectorTreeWidget::InternalDeleteMessage(
   const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   uint32_t u32_InternalMessageIndex = 0;

   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      //Internal (BEFORE ui)
      m_UpdateUniqueMessageIds();
      //Ui
      static_cast<void>(this->takeTopLevelItem(u32_InternalMessageIndex));
   }

   return u32_InternalMessageIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Internal delete message commit

   \param[in]  ou32_LastInternalMessageIndex    Last internal message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InternalDeleteMessageCommit(const uint32_t ou32_LastInternalMessageIndex)
{
   int32_t s32_NewSelection;

   //Ui update trigger
   this->updateGeometry();
   //Error handling
   RecheckErrorGlobal(false);
   //Signal
   Q_EMIT this->SigMessageCountChanged();
   //Otherwise there is no error update
   Q_EMIT this->SigErrorChanged();
   //Select near ou32_LastInternalMessageIndex
   if (ou32_LastInternalMessageIndex < static_cast<uint32_t>(this->topLevelItemCount()))
   {
      s32_NewSelection = static_cast<int32_t>(ou32_LastInternalMessageIndex);
   }
   else if (this->topLevelItemCount() > 0)
   {
      s32_NewSelection = static_cast<int32_t>(this->topLevelItemCount()) - 1L;
   }
   else
   {
      //Nothing to select
      s32_NewSelection = -1;
      Q_EMIT this->SigZeroMessages();
   }
   if (s32_NewSelection >= 0)
   {
      QTreeWidgetItem * const pc_TreeWidgetItem = this->topLevelItem(s32_NewSelection);
      if (pc_TreeWidgetItem != NULL)
      {
         pc_TreeWidgetItem->setSelected(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class after internal add signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InternalAddSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                         const uint32_t & oru32_SignalIndex)
{
   uint32_t u32_InternalMessageIndex = 0;

   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      uint32_t u32_SignalIndex = 0;
      m_UpdateUniqueMessageIdsSignals(u32_InternalMessageIndex);
      if (m_MapSignalDataIndexToInternalIndex(u32_InternalMessageIndex, oru32_SignalIndex, u32_SignalIndex) == C_NO_ERR)
      {
         //Ui
         m_InsertSignal(this->topLevelItem(u32_InternalMessageIndex), u32_SignalIndex,
                        C_PuiSdHandler::h_GetInstance()->GetCanSignalDisplayName(orc_MessageId, oru32_SignalIndex,
                                                                                 false));
         //Ui update trigger
         this->updateGeometry();

         if (this->me_ProtocolType == C_OscCanProtocol::eCAN_OPEN)
         {
            // Adapt the DLC of the message in case of CANopen
            this->m_AutoAdaptCoDlc(orc_MessageId);
         }

         //Error handling
         RecheckErrorGlobal(false);
         //Signal
         Q_EMIT (this->SigSignalCountOfMessageChanged(orc_MessageId));
         Q_EMIT (this->SigErrorChanged());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class after internal delete signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InternalDeleteSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                            const uint32_t & oru32_SignalIndex)
{
   uint32_t u32_InternalMessageIndex = 0;

   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      uint32_t u32_SignalIndex = 0;
      if (m_MapSignalDataIndexToInternalIndex(u32_InternalMessageIndex, oru32_SignalIndex, u32_SignalIndex) == C_NO_ERR)
      {
         QTreeWidgetItem * const pc_Parent = this->topLevelItem(u32_InternalMessageIndex);

         //Ui
         tgl_assert(pc_Parent != NULL);
         if (pc_Parent != NULL)
         {
            pc_Parent->removeChild(pc_Parent->child(static_cast<int32_t>(u32_SignalIndex)));
            //Ui update trigger
            this->updateGeometry();
            //Error handling
            RecheckErrorGlobal(false);

            if (this->me_ProtocolType == C_OscCanProtocol::eCAN_OPEN)
            {
               // Adapt the DLC of the message in case of CANopen
               this->m_AutoAdaptCoDlc(orc_MessageId);
            }

            //Signal
            Q_EMIT (this->SigSignalCountOfMessageChanged(orc_MessageId));
            Q_EMIT (this->SigErrorChanged());

            //Handle selection
            if (pc_Parent->childCount() > 0)
            {
               QTreeWidgetItem * pc_Child = NULL;
               if (static_cast<int32_t>(u32_SignalIndex) >= pc_Parent->childCount())
               {
                  pc_Child = pc_Parent->child(pc_Parent->childCount() - 1);
               }
               else
               {
                  pc_Child = pc_Parent->child(static_cast<int32_t>(u32_SignalIndex));
               }
               if (pc_Child != NULL)
               {
                  pc_Child->setSelected(true);
               }
            }
            else
            {
               pc_Parent->setSelected(true);
            }
         }
      }
      m_UpdateUniqueMessageIdsSignals(u32_InternalMessageIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Any message id is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::OnMessageIdChange(void)
{
   m_UpdateUniqueMessageIds();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The specified message data was changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::OnMessageNameChange(void)
{
   m_SaveExpand();
   m_SaveSelection();
   //Reload strings
   m_ReloadTree(false);
   m_RestoreSelection(false, false);
   m_RestoreExpand();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The signal name has changed

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::OnSignalNameChange(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   uint32_t u32_InternalMessageIndex = 0;

   m_SaveSelection();
   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      QTreeWidgetItem * const pc_Item = this->topLevelItem(u32_InternalMessageIndex);
      if (pc_Item != NULL)
      {
         uint32_t u32_SignalIndex;
         for (uint32_t u32_ItChild = 0; u32_ItChild < static_cast<uint32_t>(pc_Item->childCount()); ++u32_ItChild)
         {
            QTreeWidgetItem * const pc_ChildItem = pc_Item->child(u32_ItChild);

            if (m_MapSignalInternalIndexToDataIndex(u32_InternalMessageIndex, u32_ItChild, u32_SignalIndex) == C_NO_ERR)
            {
               if (pc_ChildItem != NULL)
               {
                  pc_ChildItem->setText(0, C_PuiSdHandler::h_GetInstance()->GetCanSignalDisplayName
                                           (this->mc_UniqueMessageIds[u32_InternalMessageIndex], u32_SignalIndex,
                                           false));
               }
            }
         }
      }
   }
   m_RestoreSelection(false, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The signal position has changed

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::OnSignalPositionChange(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   // A change of the position (start bit or length can cause a change for the auto DLC mechanism)
   this->m_AutoAdaptCoDlc(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   In case of a disconnected node update of the unique message ids

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::OnNodeDisconnected(const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_InterfaceIndex)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      uint32_t u32_UniqueMsgCounter;

      for (u32_UniqueMsgCounter = 0U; u32_UniqueMsgCounter < this->mc_UniqueMessageIds.size();
           ++u32_UniqueMsgCounter)
      {
         C_OscCanMessageIdentificationIndices & rc_UniqueMsgId = this->mc_UniqueMessageIds[u32_UniqueMsgCounter];

         if ((rc_UniqueMsgId.u32_NodeIndex == ou32_NodeIndex) &&
             (rc_UniqueMsgId.u32_InterfaceIndex == ou32_InterfaceIndex))
         {
            // This message id is affected of the change -> Check if it has matching message ids with an other
            // node/interface combination
            this->mpc_MessageSyncManager->ReplaceMessageIdWithMatchingId(rc_UniqueMsgId);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger global error check (icons only)

   \param[in]  orq_HandleSelection  Flag to change selection
                                    (use-case: message name change -> new position of selected item!)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::RecheckErrorGlobal(const bool & orq_HandleSelection)
{
   if (orq_HandleSelection == true)
   {
      m_SaveSelection();
   }
   m_UpdateUniqueMessageIds();
   for (uint32_t u32_ItMessageId = 0; u32_ItMessageId < this->mc_UniqueMessageIds.size(); ++u32_ItMessageId)
   {
      this->RecheckError(this->mc_UniqueMessageIds[u32_ItMessageId], false);
   }
   if (orq_HandleSelection == true)
   {
      m_RestoreSelection(true, false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger recheck of error values for tree

   \param[in]  orc_MessageId              Message identification indices
   \param[in]  orq_AllowMessageIdUpdate   Flag to allow message ID changes using when using this function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::RecheckError(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                    const bool & orq_AllowMessageIdUpdate)
{
   if (orq_AllowMessageIdUpdate == true)
   {
      m_SaveSelection();
      m_UpdateUniqueMessageIds();
   }
   if (this->mpc_MessageSyncManager != NULL)
   {
      uint32_t u32_InternalId = 0;
      if (this->m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalId) == C_NO_ERR)
      {
         C_SdBueMessageSelectorTreeWidgetItem * const pc_TopLevelItem =
            dynamic_cast<C_SdBueMessageSelectorTreeWidgetItem * const>(this->topLevelItem(u32_InternalId));
         const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
         const C_OscCanMessageContainer * const pc_MessageContainer =
            C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_MessageId.u32_NodeIndex,
                                                                            orc_MessageId.e_ComProtocol,
                                                                            orc_MessageId.u32_InterfaceIndex,
                                                                            orc_MessageId.u32_DatapoolIndex);
         if (((pc_TopLevelItem != NULL) && (pc_Message != NULL)) && (pc_MessageContainer != NULL))
         {
            bool q_IdValid = true;
            bool q_NameValid = true;
            bool q_HasTx = true;
            bool q_DelayInvalid;
            bool q_NoSignalsInvalid;
            bool q_TxMethodInvalid;
            bool q_SignalsValid = true;

            // Parameter oq_CanOpenPdoSyncValid is true in case of no CANopen protocol
            // to avoid a check for this scenario
            bool q_CanOpenPdoSyncValid = true;

            if (orc_MessageId.e_ComProtocol == C_OscCanProtocol::eCAN_OPEN)
            {
               // Special case CANopen
               // Get the CANopen Manager
               const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
                  orc_MessageId.u32_NodeIndex);
               tgl_assert(pc_Node != NULL);
               if (pc_Node != NULL)
               {
                  const std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItCoManager =
                     pc_Node->c_CanOpenManagers.find(
                        pc_Node->c_Properties.c_ComInterfaces[orc_MessageId.u32_InterfaceIndex].u8_InterfaceNumber);

                  // Check if it is the CANopen Manager of this bus
                  if (c_ItCoManager != pc_Node->c_CanOpenManagers.end())
                  {
                     // When PDO SYNC message is produces, a configured TX method with PDO SYNC
                     // is valid
                     q_CanOpenPdoSyncValid = c_ItCoManager->second.q_ProduceSyncMessage;
                  }
               }
            }

            this->mpc_MessageSyncManager->CheckMessageIdBus(C_OscCanMessageUniqueId(pc_Message->u32_CanId,
                                                                                    pc_Message->q_IsExtended), q_IdValid,
                                                            &orc_MessageId);
            this->mpc_MessageSyncManager->CheckMessageNameBus(pc_Message->c_Name, q_NameValid, &orc_MessageId);
            this->mpc_MessageSyncManager->CheckMessageHasTx(q_HasTx, orc_MessageId);
            pc_MessageContainer->CheckMessageLocalError(NULL, orc_MessageId.u32_MessageIndex,
                                                        orc_MessageId.q_MessageIsTx, NULL, NULL, &q_DelayInvalid, NULL,
                                                        NULL, NULL, &q_NoSignalsInvalid, &q_TxMethodInvalid,
                                                        C_OscCanProtocol::h_GetCanMessageValidSignalsDlcOffset(
                                                           orc_MessageId.e_ComProtocol),
                                                        C_OscCanProtocol::h_GetCanMessageSignalGapsValid(
                                                           orc_MessageId.e_ComProtocol),
                                                        C_OscCanProtocol::h_GetCanMessageSignalByteAlignmentRequired(
                                                           orc_MessageId.e_ComProtocol),
                                                        C_OscCanProtocol::h_GetCanMessageSignalsRequired(
                                                           orc_MessageId.e_ComProtocol),
                                                        q_CanOpenPdoSyncValid);
            if (pc_Message->c_Signals.size() > 0)
            {
               const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolList(
                  orc_MessageId.u32_NodeIndex, orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                  orc_MessageId.u32_DatapoolIndex, orc_MessageId.q_MessageIsTx);
               if (pc_List != NULL)
               {
                  uint32_t u32_SignalInternalIndex = 0;
                  for (uint32_t u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
                  {
                     if (m_MapSignalDataIndexToInternalIndex(u32_InternalId, u32_ItSignal,
                                                             u32_SignalInternalIndex) == C_NO_ERR)
                     {
                        C_SdBueMessageSelectorTreeWidgetItem * const pc_ChildItem =
                           dynamic_cast<C_SdBueMessageSelectorTreeWidgetItem * const>(pc_TopLevelItem->child(
                                                                                         u32_SignalInternalIndex));
                        if (pc_ChildItem != NULL)
                        {
                           if (pc_Message->CheckErrorSignal(
                                  pc_List, u32_ItSignal,
                                  C_OscCanProtocol::h_GetCanMessageValidSignalsDlcOffset(orc_MessageId.e_ComProtocol),
                                  C_OscCanProtocol::h_GetCanMessageSignalGapsValid(orc_MessageId.e_ComProtocol),
                                  C_OscCanProtocol::h_GetCanMessageSignalByteAlignmentRequired(
                                     orc_MessageId.e_ComProtocol)))
                           {
                              //Error
                              pc_ChildItem->SetError(true);
                              q_SignalsValid = false;
                           }
                           else
                           {
                              //Valid
                              pc_ChildItem->SetError(false);
                           }
                        }
                     }
                  }
               }
            }
            //Always active
            if ((q_IdValid == true) && (q_NameValid == true) && (q_SignalsValid == true) &&
                (q_DelayInvalid == false) && (q_NoSignalsInvalid == false) && (q_TxMethodInvalid == false) &&
                (q_HasTx == true))
            {
               //Valid
               pc_TopLevelItem->SetError(false);
            }
            else
            {
               //Error
               pc_TopLevelItem->SetError(true);
            }
         }
      }
   }
   if (orq_AllowMessageIdUpdate == true)
   {
      m_RestoreSelection(false, false);
   }
   //Update icons
   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if interaction possible

   \return
   true  At least one node connected
   false No node connected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMessageSelectorTreeWidget::CheckIfAnyNodeConnected(void) const
{
   bool q_Retval = false;

   if (this->mpc_MessageSyncManager != NULL)
   {
      if (this->mq_ModeSingleNode == true)
      {
         q_Retval = true;
      }
      else
      {
         std::vector<uint32_t> c_NodeIndexes;
         std::vector<uint32_t> c_InterfaceIndexes;
         C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_BusIndex,
                                                                                            c_NodeIndexes,
                                                                                            c_InterfaceIndexes);
         if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
         {
            for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
            {
               std::vector<const C_OscCanMessageContainer *> c_MessageContainers =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainers(c_NodeIndexes[u32_ItNode],
                                                                                   this->me_ProtocolType,
                                                                                   c_InterfaceIndexes[u32_ItNode]);
               uint32_t u32_Counter;

               for (u32_Counter = 0U; u32_Counter < c_MessageContainers.size(); ++u32_Counter)
               {
                  const C_OscCanMessageContainer * const pc_MessageContainer = c_MessageContainers[u32_Counter];
                  if (pc_MessageContainer->q_IsComProtocolUsedByInterface == true)
                  {
                     q_Retval = true;
                     break;
                  }
               }

               if (q_Retval == true)
               {
                  break;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle single message selection change with all aspects

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  oq_BlockSignal    Optional flag for blocking the signal for changed selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SelectMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                     const bool oq_BlockSignal)
{
   const std::vector<uint32_t> c_Signal;

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
   this->mc_SelectedMessageIds.push_back(orc_MessageId);
   this->mc_SelectedSignals.push_back(c_Signal);

   //Do not trigger selection
   if (oq_BlockSignal == true)
   {
      m_DisconnectSelection();
   }
   m_RestoreSelection(true, false);

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle multi message selection change with all aspects

   \param[in]  orc_MessageIds    Multiple message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SelectMessages(
   const std::vector<C_OscCanMessageIdentificationIndices> & orc_MessageIds)
{
   const uint16_t u16_Timer = osc_write_log_performance_start();

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_MessageIds.size(); ++u32_ItMessage)
   {
      const std::vector<uint32_t> c_Signal;
      this->mc_SelectedMessageIds.push_back(orc_MessageIds[u32_ItMessage]);
      this->mc_SelectedSignals.push_back(c_Signal);
   }

   m_RestoreSelection(true, true);

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
   osc_write_log_performance_stop(u16_Timer, "System Defintion Bus Edit Select Messages");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change with all aspects

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  oq_BlockSignal       Optional flag for blocking the signal for changed selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SelectSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                    const uint32_t & oru32_SignalIndex, const bool oq_BlockSignal)
{
   std::vector<uint32_t> c_Signal;

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
   this->mc_SelectedMessageIds.push_back(orc_MessageId);
   c_Signal.push_back(oru32_SignalIndex);
   this->mc_SelectedSignals.push_back(c_Signal);

   //Do not trigger selection
   if (oq_BlockSignal == true)
   {
      m_DisconnectSelection();
   }
   m_RestoreSelection(true, false);

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the level of the item at the position

   \param[in]  orc_Pos  Position of item

   \return
   0: No level
   1: Message level
   2: Signal level
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageSelectorTreeWidget::GetLevelOfPos(const QPoint & orc_Pos) const
{
   int32_t s32_Level = 0;
   const QTreeWidgetItem * const pc_Item = this->itemAt(this->mapFromGlobal(orc_Pos));

   if (pc_Item != NULL)
   {
      // check if the element is a top level item or a child
      const QTreeWidgetItem * const pc_Parent = pc_Item->parent();
      if (pc_Parent == NULL)
      {
         // top level item
         s32_Level = 1;
      }
      else
      {
         s32_Level = 2;
      }
   }

   return s32_Level;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the current selected signals message parent is read only

   \retval   true    The message of the selected signal has the read only flag
   \retval   false   The message of the selected signal has no read only flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMessageSelectorTreeWidget::IsSelectedMessageContentReadOnly(void) const
{
   bool q_Return = false;

   // If empty, this information is not available and can not be checked
   if (this->mc_CoUniqueMessagesPdoMappingRo.empty() == false)
   {
      const QModelIndexList c_IndexList = this->selectedIndexes();

      if (c_IndexList.isEmpty() == false)
      {
         uint32_t u32_MessageIndex;
         bool q_MsgIndexValid = false;
         // Only one relevant to check for signal.
         const QModelIndexList::const_iterator c_ItIndex = c_IndexList.begin();

         if (c_ItIndex->parent().isValid() == true)
         {
            //Signal
            if (c_ItIndex->parent().row() >= 0)
            {
               q_MsgIndexValid = true;
               u32_MessageIndex = c_ItIndex->parent().row();
            }
         }
         else
         {
            //Message
            if (c_ItIndex->row() >= 0)
            {
               q_MsgIndexValid = true;
               u32_MessageIndex = c_ItIndex->row();
            }
         }

         if ((q_MsgIndexValid == true) &&
             (u32_MessageIndex < this->mc_CoUniqueMessagesPdoMappingRo.size()))
         {
            q_Return = (this->mc_CoUniqueMessagesPdoMappingRo[u32_MessageIndex] == 1U) ? true : false;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the seperator lines

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::paintEvent(QPaintEvent * const opc_Event)
{
   // On a QTreeWidget can not painted directly
   if (this->viewport() != NULL)
   {
      QRect c_Rect;
      QPainter c_Painter(this->viewport());
      QBrush c_Brush = c_Painter.brush();
      QPen c_Pen = c_Painter.pen();
      const QList<QTreeWidgetItem *> & rc_SelectedItems = this->selectedItems();
      QList<QTreeWidgetItem *>::const_iterator c_ItSelectedItem;
      QTreeWidgetItem * pc_Parent;

      // prepare the brush and pen
      c_Brush.setStyle(Qt::SolidPattern);
      c_Pen.setColor(Qt::transparent);
      c_Painter.setPen(c_Pen);

      for (c_ItSelectedItem = rc_SelectedItems.begin(); c_ItSelectedItem != rc_SelectedItems.end(); ++c_ItSelectedItem)
      {
         // draw background selection
         c_Rect = this->visualItemRect(*c_ItSelectedItem);
         c_Rect.setX(0);

         c_Brush.setColor(mc_STYLE_GUIDE_COLOR_10);
         c_Painter.setBrush(c_Brush);
         c_Painter.drawRect(c_Rect);

         // check if the element is a top level item or a child
         pc_Parent = (*c_ItSelectedItem)->parent();
         if (pc_Parent == NULL)
         {
            // top level item
            if (rc_SelectedItems.count() == 1)
            {
               // to mark the viewing are, only one top level item may be selected
               pc_Parent = *c_ItSelectedItem;
            }
         }

         if (pc_Parent != NULL)
         {
            // draw the special line borders
            if (pc_Parent->isExpanded() == true)
            {
               c_Brush.setColor(mc_STYLE_GUIDE_COLOR_7);
               c_Painter.setBrush(c_Brush);

               if (pc_Parent->childCount() > 0)
               {
                  QTreeWidgetItem * const pc_ItemLastSignal = pc_Parent->child(pc_Parent->childCount() - 1);

                  // draw the upper line
                  c_Rect = this->visualItemRect(pc_Parent);
                  c_Painter.drawRect(0, c_Rect.y(), this->width(), 2);

                  // draw the bottom line
                  c_Rect = this->visualItemRect(pc_ItemLastSignal);
                  c_Painter.drawRect(0, c_Rect.y() + c_Rect.height(), this->width(), 2);
               }
            }
         }
      }
   }

   QTreeWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event slot

   Here: Move elements

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::dropEvent(QDropEvent * const opc_Event)
{
   if (this->mpc_UndoManager != NULL)
   {
      const QMimeData * const pc_MimeData = opc_Event->mimeData();

      if (pc_MimeData != NULL)
      {
         if (this->mimeTypes().size() > 2)
         {
            if ((pc_MimeData->hasFormat(this->mimeTypes().at(1)) == true) &&
                (pc_MimeData->hasFormat(this->mimeTypes().at(2)) == true))
            {
               std::vector<C_OscCanMessageIdentificationIndices> c_SourceMessageIds;
               std::vector<uint32_t> c_SourceSignalIndices;
               const QString c_MessageIndicesString = pc_MimeData->data(this->mimeTypes().at(1));
               const QString c_SignalIndicesString = pc_MimeData->data(this->mimeTypes().at(2));
               if (C_SdClipBoardHelper::h_LoadMessageIndexFromString(c_MessageIndicesString,
                                                                     c_SourceMessageIds) == C_NO_ERR)
               {
                  if (C_SdClipBoardHelper::h_LoadIndicesFromString(c_SourceSignalIndices,
                                                                   c_SignalIndicesString) == C_NO_ERR)
                  {
                     QTreeWidgetItem * const pc_Child = this->itemAt(opc_Event->pos());
                     if (pc_Child != NULL)
                     {
                        QTreeWidgetItem * const pc_Parent = pc_Child->parent();
                        int32_t s32_MessageIndex;
                        if (pc_Parent != NULL)
                        {
                           //Specific signal
                           s32_MessageIndex = this->indexOfTopLevelItem(pc_Parent);
                        }
                        else
                        {
                           //Message only
                           s32_MessageIndex = this->indexOfTopLevelItem(pc_Child);
                        }
                        if (s32_MessageIndex >= 0)
                        {
                           const uint32_t u32_MessageIndex = static_cast<uint32_t>(s32_MessageIndex);
                           if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
                           {
                              const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
                                 this->mc_UniqueMessageIds[u32_MessageIndex]);
                              if (pc_Message != NULL)
                              {
                                 if (c_SourceMessageIds.size() == c_SourceSignalIndices.size())
                                 {
                                    std::vector<C_OscCanMessageIdentificationIndices> c_TargetMessageIds;
                                    std::vector<uint32_t> c_TargetSignalIndices;
                                    uint32_t u32_SignalCounter = pc_Message->c_Signals.size();
                                    c_TargetMessageIds.reserve(c_SourceMessageIds.size());
                                    c_TargetSignalIndices.reserve(c_SourceMessageIds.size());
                                    for (uint32_t u32_ItMessage = 0; u32_ItMessage < c_SourceMessageIds.size();
                                         ++u32_ItMessage)
                                    {
                                       //Skip move in same message
                                       if ((c_SourceMessageIds[u32_ItMessage] ==
                                            this->mc_UniqueMessageIds[u32_MessageIndex]) == false)
                                       {
                                          c_TargetMessageIds.push_back(this->mc_UniqueMessageIds[u32_MessageIndex]);
                                          c_TargetSignalIndices.push_back(u32_SignalCounter);
                                          ++u32_SignalCounter;
                                       }
                                    }
                                    if ((c_TargetSignalIndices.size() > 0) && (c_TargetMessageIds.size() > 0))
                                    {
                                       const C_OscCanMessage * const pc_TargetMsg =
                                          C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_TargetMessageIds[0]);

                                       if (pc_TargetMsg != NULL)
                                       {
                                          this->mpc_UndoManager->DoMoveSignal(c_SourceMessageIds, c_SourceSignalIndices,
                                                                              c_TargetMessageIds,
                                                                              c_TargetSignalIndices,
                                                                              this->mpc_MessageSyncManager,
                                                                              this);
                                       }
                                    }
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten start drag event

   Here: start drag manually (for custom preview)

   \param[in]  oc_SupportedActions  Supported actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::startDrag(const Qt::DropActions oc_SupportedActions)
{
   if (this->me_ProtocolType != C_OscCanProtocol::eCAN_OPEN)
   {
      const QList<QTreeWidgetItem *> c_SelectedItems = this->selectedItems();

      if ((c_SelectedItems.size() > 0) && (this->mimeTypes().size() > 0))
      {
         QMimeData * const pc_Mime = this->mimeData(c_SelectedItems);
         if (pc_Mime != NULL)
         {
            //Manual drag
            QDrag * const pc_Drag = new QDrag(this);

            //Store in mime

            pc_Drag->setMimeData(pc_Mime);
            pc_Drag->exec(oc_SupportedActions, this->defaultDropAction());
            //lint -e429 Qt parent handling will take care of it
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overloaded function for stored mime data

   \param[in]  oc_Items    Current selected items

   \return
   Mime data for drag event
*/
//----------------------------------------------------------------------------------------------------------------------
QMimeData * C_SdBueMessageSelectorTreeWidget::mimeData(const QList<QTreeWidgetItem *> oc_Items) const
{
   QMimeData * pc_Retval = NULL;

   if ((oc_Items.size() > 0) && (this->mimeTypes().size() > 2))
   {
      std::vector<C_OscCanMessageIdentificationIndices> c_MessageIds;
      std::vector<uint32_t> c_SignalIndices;
      bool q_SignalsOnly = true;

      //Get selected indices
      for (QList<QTreeWidgetItem *>::const_iterator c_ItItem = oc_Items.begin(); c_ItItem != oc_Items.end();
           ++c_ItItem)
      {
         QTreeWidgetItem * const pc_Child = *c_ItItem;
         if (pc_Child != NULL)
         {
            QTreeWidgetItem * const pc_Parent = pc_Child->parent();
            if (pc_Parent != NULL)
            {
               const int32_t s32_InternalSignalIndex = pc_Parent->indexOfChild(pc_Child);
               const int32_t s32_MessageIndex = this->indexOfTopLevelItem(pc_Parent);
               if ((s32_InternalSignalIndex >= 0) && (s32_MessageIndex >= 0))
               {
                  const uint32_t u32_MessageIndex = static_cast<uint32_t>(s32_MessageIndex);
                  if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
                  {
                     const uint32_t u32_InternalSignalIndex = static_cast<uint32_t>(s32_InternalSignalIndex);
                     uint32_t u32_SignalDataIndex;
                     if (this->m_MapSignalInternalIndexToDataIndex(u32_MessageIndex, u32_InternalSignalIndex,
                                                                   u32_SignalDataIndex) == C_NO_ERR)
                     {
                        c_MessageIds.push_back(this->mc_UniqueMessageIds[u32_MessageIndex]);
                        c_SignalIndices.push_back(u32_SignalDataIndex);
                     }
                  }
               }
            }
            else
            {
               q_SignalsOnly = false;
            }
         }
      }

      if (q_SignalsOnly == true)
      {
         QString c_TmpString;

         pc_Retval = new QMimeData();
         //Store message index
         C_SdClipBoardHelper::h_StoreMessageIndexToString(c_MessageIds, c_TmpString);
         pc_Retval->setData(this->mimeTypes().at(1), c_TmpString.toStdString().c_str());
         //Store signal indices
         C_SdClipBoardHelper::h_StoreIndicesToString(c_SignalIndices, c_TmpString);
         pc_Retval->setData(this->mimeTypes().at(2), c_TmpString.toStdString().c_str());
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overloaded function for stored mime types

   \return
   Mime types for drag event
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_SdBueMessageSelectorTreeWidget::mimeTypes(void) const
{
   QStringList c_Retval = QTreeWidget::mimeTypes();

   c_Retval.append("application/stw/opensyde/system/bus/message");
   c_Retval.append("application/stw/opensyde/system/bus/message/signals");
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do any potential last minute tooltip update actions

  Last minute update necessary as there is no trigger for any change yet (might cost performance)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_LastMinuteToolTipUpdate(void)
{
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < this->mc_UniqueMessageIds.size(); ++u32_ItMessage)
   {
      uint32_t u32_InternalIndex;
      const C_OscCanMessageIdentificationIndices & rc_MessageId = this->mc_UniqueMessageIds[u32_ItMessage];

      if (m_MapMessageIdToInternalMessageIndex(rc_MessageId, u32_InternalIndex) == C_NO_ERR)
      {
         QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(u32_InternalIndex);
         if (pc_TopLevelItem != NULL)
         {
            //Message
            pc_TopLevelItem->setData(0, ms32_USER_ROLE_TOOL_TIP_HEADING,
                                     C_PuiSdHandler::h_GetInstance()->GetCanMessageDisplayName(rc_MessageId, true));
            pc_TopLevelItem->setData(0, ms32_USER_ROLE_TOOL_TIP_CONTENT,
                                     C_SdUtil::h_GetToolTipContentMessage(rc_MessageId));
            //Signals
            for (int32_t s32_ItSignalInternal = 0; s32_ItSignalInternal < pc_TopLevelItem->childCount();
                 ++s32_ItSignalInternal)
            {
               QTreeWidgetItem * const pc_SignalItem = pc_TopLevelItem->child(s32_ItSignalInternal);
               uint32_t u32_SignalDataIndex;
               if (this->m_MapSignalInternalIndexToDataIndex(u32_InternalIndex, s32_ItSignalInternal,
                                                             u32_SignalDataIndex) == C_NO_ERR)
               {
                  //Signal
                  pc_SignalItem->setData(0, ms32_USER_ROLE_TOOL_TIP_HEADING,
                                         C_PuiSdHandler::h_GetInstance()->GetCanSignalDisplayName(
                                            rc_MessageId, u32_SignalDataIndex, true));
                  pc_SignalItem->setData(0, ms32_USER_ROLE_TOOL_TIP_CONTENT,
                                         C_SdUtil::h_GetToolTipContentSignal(rc_MessageId, u32_SignalDataIndex));
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_ReloadTree(const bool & orq_HandleSelection)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      if (orq_HandleSelection == true)
      {
         m_SaveSelection();
      }
      m_UpdateUniqueMessageIds();
      //Clear tree
      this->clear();
      for (uint32_t u32_ItMessageId = 0; u32_ItMessageId < this->mc_UniqueMessageIds.size(); ++u32_ItMessageId)
      {
         this->m_InsertMessage(u32_ItMessageId);
      }
      RecheckErrorGlobal(false);
      if (orq_HandleSelection == true)
      {
         m_RestoreSelection(false, false);
         //Handle no selection
         if (this->selectedItems().size() == 0)
         {
            Q_EMIT this->SigZeroMessages();
         }
      }
   }
   //Ui update trigger
   this->adjustSize();
   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new signal

   \param[in]  ou32_MessageIndex Index of message which will get the new signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_AddSignal(const uint32_t ou32_MessageIndex, const bool oq_SelectSignal)
{
   const C_OscCanMessageIdentificationIndices c_MessageId = this->mc_UniqueMessageIds[ou32_MessageIndex];
   const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);

   if (pc_Message != NULL)
   {
      const uint32_t u32_ItSignal = pc_Message->c_Signals.size();
      const uint16_t u16_StartBit = C_SdBueMessageSelectorTreeWidget::mh_GetStartBit(c_MessageId);

      if (this->me_ProtocolType != C_OscCanProtocol::eCAN_OPEN)
      {
         //Core
         this->mpc_UndoManager->DoAddSignal(c_MessageId, u32_ItSignal, u16_StartBit,
                                            C_OscCanSignal::eMUX_DEFAULT, 0,
                                            this->mpc_MessageSyncManager,
                                            this);
      }
      else
      {
         this->m_AddCoSignal(c_MessageId, u32_ItSignal, u16_StartBit);
      }

      if (oq_SelectSignal)
      {
         this->SelectSignal(c_MessageId, u32_ItSignal, false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new CANopen specific signal

   \param[in]      orc_MessageId             Message identification indices
   \param[in]      ou32_SignalIndex          Signal index
   \param[in]      ou16_StartBit             Start bit of signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_AddCoSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                     const uint32_t ou32_SignalIndex, const uint16_t ou16_StartBit)
{
   const QPointer<C_OgePopUpDialog> c_PopUp = new C_OgePopUpDialog(this, this);
   C_SdBueCoAddSignalsDialog * const pc_AddDialog = new C_SdBueCoAddSignalsDialog(*c_PopUp,
                                                                                  orc_MessageId);

   //Resize
   c_PopUp->SetSize(QSize(800, 800));

   Q_UNUSED(pc_AddDialog)

   if (c_PopUp->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      const std::vector<C_OscCanOpenManagerMappableSignal> c_Signals = pc_AddDialog->GetSelectedSignals();
      if (c_Signals.size() > 0UL)
      {
         //Core
         this->mpc_UndoManager->DoAddCoSignal(orc_MessageId, ou32_SignalIndex, ou16_StartBit, c_Signals,
                                              this->mpc_MessageSyncManager,
                                              this);
      }
   }

   if (c_PopUp != NULL)
   {
      pc_AddDialog->PrepareCleanUp();
      c_PopUp->HideOverlay();
      c_PopUp->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_AddDialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt the DLC of the CANopen message in case of changed signals

   \param[in]      orc_MessageId             Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_AutoAdaptCoDlc(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   if ((this->mpc_MessageSyncManager != NULL) &&
       (this->me_ProtocolType == C_OscCanProtocol::eCAN_OPEN))
   {
      const C_OscCanMessage * const pc_CanMessage =
         C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);

      if (pc_CanMessage != NULL)
      {
         //copy current message
         C_OscCanMessage c_MessageData = *pc_CanMessage;
         uint32_t u32_SignalCounter;

         c_MessageData.u16_Dlc = 0U;

         // Check the size of all signals
         for (u32_SignalCounter = 0U; u32_SignalCounter < c_MessageData.c_Signals.size(); ++u32_SignalCounter)
         {
            // In case of CANopen the DLC is adapted automatically. Adapt to the added signals
            const C_OscCanSignal & rc_CurSignal = c_MessageData.c_Signals[u32_SignalCounter];
            const uint16_t u16_LastBit = rc_CurSignal.u16_ComBitStart +
                                         rc_CurSignal.u16_ComBitLength;
            uint16_t u16_NeededBytes = u16_LastBit / 8U;

            // Check for not byte aligned signals
            if ((u16_LastBit % 8U) != 0U)
            {
               ++u16_NeededBytes;
            }

            if (u16_NeededBytes > c_MessageData.u16_Dlc)
            {
               c_MessageData.u16_Dlc = u16_NeededBytes;

               if (c_MessageData.u16_Dlc >= 8U)
               {
                  // 8 is maximum
                  c_MessageData.u16_Dlc = 8U;
                  break;
               }
            }
         }

         tgl_assert(this->mpc_MessageSyncManager->SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
                       orc_MessageId, c_MessageData) == C_NO_ERR);

         // In case of a selected message, the DLC spin box must be updated
         Q_EMIT (this->SigRefreshSelection());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_InsertMessage(const uint32_t & oru32_MessageIdIndex)
{
   if (oru32_MessageIdIndex < this->mc_UniqueMessageIds.size())
   {
      const C_OscCanMessageIdentificationIndices & rc_MessageId = this->mc_UniqueMessageIds[oru32_MessageIdIndex];
      const C_OscCanMessage * const pc_MessageData = C_PuiSdHandler::h_GetInstance()->GetCanMessage(rc_MessageId);
      if (pc_MessageData != NULL)
      {
         C_SdBueMessageSelectorTreeWidgetItem * const pc_Message = new C_SdBueMessageSelectorTreeWidgetItem(true);

         uint32_t u32_Counter;
         uint32_t u32_SignalDataIndex;

         if ((this->mpc_MessageSyncManager != NULL) &&
             (this->mpc_MessageSyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eCAN_OPEN))
         {
            // Adopt PDO settings (rw/ro and activated/deactivated) from EDS
            // Hint: the case read-only as well as deactivated PDO at the same time is valid and accepted here,
            //       even if it is probably not really useful for the user, but nevertheless this case would be
            //       rather an EDS file user issue.

            // first, check if PDO is read-only
            const C_OscCanOpenManagerDeviceInfo * const pc_Manager =
               C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerDevice(rc_MessageId);

            const uint16_t u16_PdoIndex = pc_MessageData->u16_CanOpenManagerPdoIndex;
            const bool q_MessageIsTx = !(rc_MessageId.q_MessageIsTx); // the message is from CANopen manager view and
                                                                      // therefore this flag must be used inverse for
                                                                      // IsCobIdRo method
            bool q_IsPdoRo;
            pc_Manager->c_EdsFileContent.IsCobIdRo(u16_PdoIndex, q_MessageIsTx, q_IsPdoRo);

            if (q_IsPdoRo == true)
            {
               // PDO is read only
               Qt::ItemFlags c_Flags = pc_Message->flags();
               c_Flags.setFlag(Qt::ItemIsUserCheckable, false);
               pc_Message->setFlags(c_Flags);
            }
            else
            {
               pc_Message->setFlags(pc_Message->flags() | Qt::ItemIsUserCheckable);
            }

            // second, special case: CANopen PDO messages can be activated/deactivated
            pc_Message->setCheckState(0, ((pc_MessageData->q_CanOpenManagerMessageActive == true) ?
                                          Qt::Checked : Qt::Unchecked));
            connect(pc_Message, &C_SdBueMessageSelectorTreeWidgetItem::SigCheckedStateChanged,
                    this, &C_SdBueMessageSelectorTreeWidget::m_CoMessageCheckedStateChanged);
         }

         this->insertTopLevelItem(oru32_MessageIdIndex, pc_Message);

         pc_Message->setText(0, C_PuiSdHandler::h_GetInstance()->GetCanMessageDisplayName(rc_MessageId, false));
         pc_Message->setForeground(0, mc_STYLE_GUIDE_COLOR_8);

         // Signals
         for (u32_Counter = 0; u32_Counter < pc_MessageData->c_Signals.size(); ++u32_Counter)
         {
            if (m_MapSignalInternalIndexToDataIndex(oru32_MessageIdIndex, u32_Counter, u32_SignalDataIndex) == C_NO_ERR)
            {
               this->m_InsertSignal(pc_Message, u32_Counter,
                                    C_PuiSdHandler::h_GetInstance()->GetCanSignalDisplayName(
                                       rc_MessageId, u32_SignalDataIndex, false));
            }
         }
         //lint -e593 Qt parent handling will take care of it
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_InsertSignal(QTreeWidgetItem * const opc_MessageItem,
                                                      const uint32_t ou32_SignalIndex,
                                                      const QString & orc_SignalName) const
{
   QTreeWidgetItem * const pc_Signal = new C_SdBueMessageSelectorTreeWidgetItem(false);

   pc_Signal->setText(0, orc_SignalName);
   pc_Signal->setForeground(0, mc_STYLE_GUIDE_COLOR_8);

   opc_MessageItem->insertChild(ou32_SignalIndex, pc_Signal);
} //lint !e429  //no memory leak because of the parent of pc_Signal by insertChild and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_SelectionChanged(const QItemSelection & orc_Selected,
                                                          const QItemSelection & orc_Deselected)
{
   Q_UNUSED(orc_Selected)
   Q_UNUSED(orc_Deselected)

   int32_t s32_CounterTopLevelItem;
   C_SdBueMessageSelectorTreeWidgetItem * pc_TopLevelItem;
   // last selected parent item
   QTreeWidgetItem * pc_Parent = NULL;
   QTreeWidgetItem * pc_Current = NULL;
   QModelIndex c_HighestSelection;
   const bool q_Result = this->m_GetHighestSelected(c_HighestSelection);

   //Check current selection
   if (q_Result == true)
   {
      if (c_HighestSelection.isValid() == true)
      {
         const QModelIndex & rc_ParentIndex = c_HighestSelection.parent();
         if (rc_ParentIndex.isValid() == false)
         {
            //New message selection
            pc_Current = this->topLevelItem(c_HighestSelection.row());
         }
         else
         {
            //New signal selection
            pc_Parent = this->topLevelItem(rc_ParentIndex.row());
            if (pc_Parent != NULL)
            {
               pc_Current = pc_Parent->child(c_HighestSelection.row());
            }
         }
      }
   }

   if (pc_Parent != NULL)
   {
      // actual selection is a child
      for (s32_CounterTopLevelItem = 0; s32_CounterTopLevelItem < this->topLevelItemCount(); ++s32_CounterTopLevelItem)
      {
         pc_TopLevelItem =
            dynamic_cast<C_SdBueMessageSelectorTreeWidgetItem *>(this->topLevelItem(s32_CounterTopLevelItem));
         if (pc_TopLevelItem != NULL)
         {
            if (pc_Parent == pc_TopLevelItem)
            {
               const int32_t s32_SignalInternalIndex = pc_Parent->indexOfChild(pc_Current);
               if (s32_SignalInternalIndex >= 0)
               {
                  const uint32_t u32_ItMessageId = static_cast<uint32_t>(s32_CounterTopLevelItem);
                  const uint32_t u32_SignalInternalIndex = s32_SignalInternalIndex;
                  uint32_t u32_SignalDataIndex;
                  // child was selected with this parent
                  // parent shall not be selected any more, but marked as active and open
                  // the selection color of the children is handled by the stylesheets
                  pc_TopLevelItem->setSelected(false);
                  pc_TopLevelItem->setForeground(0, mc_STYLE_GUIDE_COLOR_4);
                  pc_TopLevelItem->SetAlwaysActive(true);
                  if (this->m_MapSignalInternalIndexToDataIndex(u32_ItMessageId, u32_SignalInternalIndex,
                                                                u32_SignalDataIndex) == C_NO_ERR)
                  {
                     //Signal
                     if (u32_ItMessageId < this->mc_UniqueMessageIds.size())
                     {
                        m_SendSignalSelectionUpdate(this->mc_UniqueMessageIds[u32_ItMessageId],
                                                    u32_SignalDataIndex);
                     }
                  }
               }
            }
            else
            {
               // deselect all other items
               pc_TopLevelItem->setSelected(false);
               pc_TopLevelItem->setForeground(0, mc_STYLE_GUIDE_COLOR_8);
               pc_TopLevelItem->SetAlwaysActive(false);
               this->m_DeselectChildren(pc_TopLevelItem);
            }
         }
      }
   }
   else
   {
      // actual selection is a top level item
      if (pc_Current != NULL)
      {
         //Signal
         const int32_t s32_ItMessageId = this->indexOfTopLevelItem(pc_Current);
         if (s32_ItMessageId >= 0)
         {
            const uint32_t u32_ItMessageId = static_cast<uint32_t>(s32_ItMessageId);
            if (u32_ItMessageId < this->mc_UniqueMessageIds.size())
            {
               m_SendMessageSelectionUpdate(this->mc_UniqueMessageIds[u32_ItMessageId]);
            }
         }
      }
      for (s32_CounterTopLevelItem = 0; s32_CounterTopLevelItem < this->topLevelItemCount(); ++s32_CounterTopLevelItem)
      {
         pc_TopLevelItem =
            dynamic_cast<C_SdBueMessageSelectorTreeWidgetItem *>(this->topLevelItem(s32_CounterTopLevelItem));
         if (pc_TopLevelItem != NULL)
         {
            // default case: not selected
            pc_TopLevelItem->SetAlwaysActive(false);
            if (pc_TopLevelItem->isSelected() == true)
            {
               pc_TopLevelItem->setForeground(0, mc_STYLE_GUIDE_COLOR_4);
            }
            else
            {
               pc_TopLevelItem->setForeground(0, mc_STYLE_GUIDE_COLOR_8);
            }

            // no children shall be selected if a top level item is selected
            this->m_DeselectChildren(pc_TopLevelItem);
         }
      }
   }

   // update the drawing
   this->updateGeometry();

   Q_EMIT this->SigSelectionChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for changed items of the tree widget

   \param[in]  opc_Item   Changed item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_CoMessageCheckedStateChanged(
   C_SdBueMessageSelectorTreeWidgetItem * const opc_Item)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      const int32_t s32_MessageIndex = this->indexOfTopLevelItem(opc_Item);

      if (s32_MessageIndex >= 0)
      {
         const uint32_t u32_MessageIndex = static_cast<uint32_t>(s32_MessageIndex);
         if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
         {
            const C_OscCanMessageIdentificationIndices & rc_MessageId = this->mc_UniqueMessageIds[u32_MessageIndex];
            // Get the current version of the message
            const C_OscCanMessage * const pc_CanMessage =
               C_PuiSdHandler::h_GetInstance()->GetCanMessage(rc_MessageId);
            if (pc_CanMessage != NULL)
            {
               // Adapt the message
               const bool q_Enabled = (opc_Item->checkState(0) == Qt::Checked) ? true : false;
               C_OscCanMessage c_MessageData = *pc_CanMessage;

               c_MessageData.q_CanOpenManagerMessageActive = q_Enabled;

               //save new message data
               tgl_assert(this->mpc_MessageSyncManager->
                          SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
                             rc_MessageId,
                             c_MessageData) == C_NO_ERR);

               this->RecheckErrorGlobal(false);

               // Change can cause a changed error state
               Q_EMIT (this->SigMessageCountChanged());
               Q_EMIT (this->SigErrorChanged());
               Q_EMIT (this->SigRefreshSelection());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_DeselectChildren(const QTreeWidgetItem * const opc_Parent) const
{
   int32_t s32_CounterChild;

   // deselect all children of the parent
   for (s32_CounterChild = 0; s32_CounterChild < opc_Parent->childCount(); ++s32_CounterChild)
   {
      QTreeWidgetItem * const pc_ChildItem = opc_Parent->child(s32_CounterChild);
      pc_ChildItem->setSelected(false);
      pc_ChildItem->setForeground(0, mc_STYLE_GUIDE_COLOR_8);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_TreeSizeChanged(void)
{
   //Added proper fix for size change handling
   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_ScrollBarRangeChanged(const int32_t os32_Min, const int32_t os32_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to get current, first connected node

   \param[out]  oru32_NodeIndex        Node index
   \param[out]  oru32_InterfaceIndex   Interface index
   \param[out]  oru32_DatapoolIndex    Datapool index

   \return
   C_NO_ERR Valid node
   C_NOACT  No node connected
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageSelectorTreeWidget::m_GetFirstConnectedNodeAndInterface(uint32_t & oru32_NodeIndex,
                                                                              uint32_t & oru32_InterfaceIndex,
                                                                              uint32_t & oru32_DatapoolIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->mq_ModeSingleNode == true)
   {
      if (this->mc_DatapoolIndexes.size() == 0)
      {
         // No Datapool exists, add a new one
         C_PuiSdHandler::h_GetInstance()->AddAutoGenCommDataPool(this->mu32_NodeIndex, this->me_ProtocolType);
         Q_EMIT (this->SigReload());
      }

      oru32_NodeIndex = this->mu32_NodeIndex;
      oru32_InterfaceIndex = this->mu32_InterfaceIndex;
      oru32_DatapoolIndex = this->mc_DatapoolIndexes[0];
   }
   else
   {
      std::vector<uint32_t> c_NodeIndexes;
      std::vector<uint32_t> c_InterfaceIndexes;
      std::vector<uint32_t> c_DatapoolIndexes;

      C_PuiSdNodeCanMessageSyncManager::h_GetConnectedAndActiveInterfaces(this->mu32_BusIndex,
                                                                          this->me_ProtocolType,
                                                                          c_NodeIndexes,
                                                                          c_InterfaceIndexes,
                                                                          c_DatapoolIndexes);
      if ((c_NodeIndexes.size() > 0) &&
          (c_NodeIndexes.size() == c_InterfaceIndexes.size()) &&
          (c_NodeIndexes.size() == c_DatapoolIndexes.size()))
      {
         oru32_NodeIndex = c_NodeIndexes[0];
         oru32_InterfaceIndex = c_InterfaceIndexes[0];
         oru32_DatapoolIndex = c_DatapoolIndexes[0];
      }
      else
      {
         s32_Retval = C_NOACT;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Map global message indices to internal message index

   \param[in]   orc_MessageId                Message identification indices
   \param[out]  oru32_InternalMessageIndex   Internal message index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageSelectorTreeWidget::m_MapMessageIdToInternalMessageIndex(
   const C_OscCanMessageIdentificationIndices & orc_MessageId, uint32_t & oru32_InternalMessageIndex) const
{
   int32_t s32_Retval = C_RANGE;

   if (this->mpc_MessageSyncManager != NULL)
   {
      const std::vector<C_OscCanMessageIdentificationIndices> c_MatchingIds =
         this->mpc_MessageSyncManager->GetMatchingMessageVector(orc_MessageId);

      for (uint32_t u32_ItMessageId = 0; (u32_ItMessageId < c_MatchingIds.size()) && (s32_Retval != C_NO_ERR);
           ++u32_ItMessageId)
      {
         for (uint32_t u32_ItInternalMessage = 0;
              (u32_ItInternalMessage < this->mc_UniqueMessageIds.size()) && (s32_Retval != C_NO_ERR);
              ++u32_ItInternalMessage)
         {
            if (c_MatchingIds[u32_ItMessageId] == this->mc_UniqueMessageIds[u32_ItInternalMessage])
            {
               s32_Retval = C_NO_ERR;
               oru32_InternalMessageIndex = u32_ItInternalMessage;
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get any valid message id for add operation

   \param[out]  orc_MessageId    Message identification indices

   \retval  C_NO_ERR    at least on valid message id found
   \retval  C_NO_ACT    no valid message id found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageSelectorTreeWidget::m_GetMessageIdForAdd(C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   uint32_t u32_NodeIndex;
   uint32_t u32_InterfaceIndex;
   uint32_t u32_DatapoolIndex;
   int32_t s32_Retval = C_NO_ERR;

   if (this->m_GetFirstConnectedNodeAndInterface(u32_NodeIndex, u32_InterfaceIndex, u32_DatapoolIndex) == C_NO_ERR)
   {
      const C_OscCanMessageContainer * const pc_MessageContainer =
         C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(u32_NodeIndex, this->me_ProtocolType,
                                                                         u32_InterfaceIndex, u32_DatapoolIndex);
      if (pc_MessageContainer != NULL)
      {
         orc_MessageId = C_OscCanMessageIdentificationIndices(u32_NodeIndex,
                                                              this->me_ProtocolType,
                                                              u32_InterfaceIndex, u32_DatapoolIndex, true,
                                                              pc_MessageContainer->c_TxMessages.size());
      }
      else
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_NOACT;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal message id vector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_UpdateUniqueMessageIds(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      this->mc_UniqueMessageIds = this->mpc_MessageSyncManager->GetUniqueMessages();
      //Sort
      std::sort(this->mc_UniqueMessageIds.begin(), this->mc_UniqueMessageIds.end(), C_SdBueSortHelper());
      //Sort signals
      this->mc_UniqueMessageIdsSignalsOrder.resize(this->mc_UniqueMessageIds.size());
      for (uint32_t u32_ItMessage = 0; u32_ItMessage < this->mc_UniqueMessageIds.size(); ++u32_ItMessage)
      {
         m_UpdateUniqueMessageIdsSignals(u32_ItMessage);
      }

      this->m_CoLoadEdsRestricitions();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal message id signals vector

   \param[in]  oru32_InternalMessageIndex    Internal message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_UpdateUniqueMessageIdsSignals(const uint32_t & oru32_InternalMessageIndex)
{
   if (oru32_InternalMessageIndex < this->mc_UniqueMessageIds.size())
   {
      const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
         this->mc_UniqueMessageIds[oru32_InternalMessageIndex]);
      if ((pc_Message != NULL) && (oru32_InternalMessageIndex < this->mc_UniqueMessageIdsSignalsOrder.size()))
      {
         std::vector<uint32_t> & orc_CurrentSignals = this->mc_UniqueMessageIdsSignalsOrder[oru32_InternalMessageIndex];
         std::vector<uint32_t>::iterator c_Begin;
         orc_CurrentSignals.clear();
         for (uint32_t u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
         {
            orc_CurrentSignals.push_back(u32_ItSignal);
         }
         //Sort
         c_Begin = orc_CurrentSignals.begin();
         std::sort(c_Begin, orc_CurrentSignals.end(),
                   static_cast<C_SdBueSortHelperSignal>(this->mc_UniqueMessageIds[oru32_InternalMessageIndex]));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load the EDS file restrictions

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_CoLoadEdsRestricitions(void)
{
   this->mc_CoUniqueMessagesPdoMappingRo.clear();

   if (this->me_ProtocolType == C_OscCanProtocol::eCAN_OPEN)
   {
      uint32_t u32_MessageCounter;

      for (u32_MessageCounter = 0U; u32_MessageCounter < this->mc_UniqueMessageIds.size(); ++u32_MessageCounter)
      {
         const C_OscCanMessageIdentificationIndices & rc_MsgId = this->mc_UniqueMessageIds[u32_MessageCounter];
         // The manager must be the only node associated by this message
         const C_OscCanOpenManagerDeviceInfo * const pc_Manager =
            C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerDevice(rc_MsgId);

         tgl_assert(pc_Manager != NULL);
         if (pc_Manager != NULL)
         {
            const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(rc_MsgId);
            tgl_assert(pc_Message != NULL);
            if (pc_Message != NULL)
            {
               bool q_RoFlag = false;

               // Message Tx flag is relative to the device, not the manager when using the EDS file content
               // PDO Mapping
               pc_Manager->c_EdsFileContent.IsPdoMappingRo(pc_Message->u16_CanOpenManagerPdoIndex,
                                                           !rc_MsgId.q_MessageIsTx, q_RoFlag);

               this->mc_CoUniqueMessagesPdoMappingRo.push_back(static_cast<uint8_t>(q_RoFlag));
            }
         }
      }
      tgl_assert(this->mc_UniqueMessageIds.size() == this->mc_CoUniqueMessagesPdoMappingRo.size());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Temporarily disable selection updates (same selection different position)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_DisconnectSelection(void)
{
   this->mq_NoSelectionUpdate = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enable selection update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_ReconnectSelection(void)
{
   this->mq_NoSelectionUpdate = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal message selection

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_SendMessageSelectionUpdate(
   const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   if (this->mq_NoSelectionUpdate == false)
   {
      Q_EMIT this->SigMessageSelected(orc_MessageId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal signal selection

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_SendSignalSelectionUpdate(
   const C_OscCanMessageIdentificationIndices & orc_MessageId, const uint32_t & oru32_SignalIndex)
{
   if (this->mq_NoSelectionUpdate == false)
   {
      Q_EMIT this->SigSignalSelected(orc_MessageId, oru32_SignalIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save current selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_SaveSelection(void)
{
   const QModelIndexList c_Selection = this->selectedIndexes();

   std::vector<uint32_t> c_Signals;

   m_DisconnectSelection();

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
   for (QModelIndexList::const_iterator c_ItSelection = c_Selection.begin(); c_ItSelection != c_Selection.end();
        ++c_ItSelection)
   {
      c_Signals.clear();
      if (c_ItSelection->isValid() == true)
      {
         if (c_ItSelection->parent().isValid() == true)
         {
            //Signal
            if ((c_ItSelection->parent().row() >= 0) && (c_ItSelection->row() >= 0))
            {
               const uint32_t u32_Message = c_ItSelection->parent().row();
               if (u32_Message < this->mc_UniqueMessageIds.size())
               {
                  const int32_t s32_SignalInternalIndex = c_ItSelection->row();
                  if (s32_SignalInternalIndex >= 0)
                  {
                     const uint32_t u32_SignalInternalIndex = s32_SignalInternalIndex;
                     uint32_t u32_DataIndex;
                     if (this->m_MapSignalInternalIndexToDataIndex(u32_Message, u32_SignalInternalIndex,
                                                                   u32_DataIndex) == C_NO_ERR)
                     {
                        this->mc_SelectedMessageIds.push_back(this->mc_UniqueMessageIds[u32_Message]);
                        c_Signals.push_back(u32_DataIndex);
                        this->mc_SelectedSignals.push_back(c_Signals);
                     }
                  }
               }
            }
         }
         else
         {
            //Message
            if (c_ItSelection->row() >= 0)
            {
               const uint32_t u32_Message = c_ItSelection->row();
               if (u32_Message < this->mc_UniqueMessageIds.size())
               {
                  this->mc_SelectedMessageIds.push_back(this->mc_UniqueMessageIds[u32_Message]);
                  this->mc_SelectedSignals.push_back(c_Signals);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore last known selection

   \param[in]  oq_AlsoSetCurrent             Flag to set current
   \param[in]  oq_SendSignalForLastSelected  Send signal for last selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_RestoreSelection(const bool oq_AlsoSetCurrent,
                                                          const bool oq_SendSignalForLastSelected)
{
   if (oq_SendSignalForLastSelected == true)
   {
      m_DisconnectSelection();
   }
   if (oq_AlsoSetCurrent)
   {
      //Wait for all events -> otherwise widget size is invalid so scrolling does not work properly
      QApplication::processEvents();
   }
   this->clearSelection();
   tgl_assert(this->mc_SelectedMessageIds.size() == this->mc_SelectedSignals.size());
   if (this->mc_SelectedMessageIds.size() == this->mc_SelectedSignals.size())
   {
      uint32_t u32_InternalIndex;
      for (uint32_t u32_ItPrevSelection = 0; u32_ItPrevSelection < this->mc_SelectedMessageIds.size();
           ++u32_ItPrevSelection)
      {
         const C_OscCanMessageIdentificationIndices & rc_Message = this->mc_SelectedMessageIds[u32_ItPrevSelection];

         //Check if still in tree
         if (m_MapMessageIdToInternalMessageIndex(rc_Message, u32_InternalIndex) == C_NO_ERR)
         {
            if ((u32_ItPrevSelection == (static_cast<uint32_t>(this->mc_SelectedMessageIds.size()) - 1UL)) &&
                (oq_SendSignalForLastSelected == true))
            {
               m_ReconnectSelection();
            }
            if (u32_InternalIndex < this->mc_UniqueMessageIds.size())
            {
               QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(u32_InternalIndex);
               if (pc_TopLevelItem != NULL)
               {
                  const std::vector<uint32_t> & rc_Signals = this->mc_SelectedSignals[u32_ItPrevSelection];

                  if (rc_Signals.size() == 0)
                  {
                     //Message
                     pc_TopLevelItem->setSelected(true);
                     //Only scroll to last item
                     if (u32_ItPrevSelection == (static_cast<uint32_t>(this->mc_SelectedMessageIds.size()) - 1UL))
                     {
                        if (oq_AlsoSetCurrent == true)
                        {
                           this->scrollToItem(pc_TopLevelItem);
                        }
                     }
                  }
                  else
                  {
                     //Always expand for signal selection

                     this->expandItem(pc_TopLevelItem);

                     //Signal(s)
                     for (uint32_t u32_ItSignal = 0; u32_ItSignal < rc_Signals.size(); ++u32_ItSignal)
                     {
                        uint32_t u32_SignalInternalIndex = 0;
                        if (this->m_MapSignalDataIndexToInternalIndex(u32_InternalIndex, rc_Signals[u32_ItSignal],
                                                                      u32_SignalInternalIndex) == C_NO_ERR)
                        {
                           QTreeWidgetItem * const pc_ChildItem = pc_TopLevelItem->child(u32_SignalInternalIndex);
                           if (pc_ChildItem != NULL)
                           {
                              pc_ChildItem->setSelected(true);
                              //Only scroll to last item
                              if (u32_ItPrevSelection ==
                                  (static_cast<uint32_t>(this->mc_SelectedMessageIds.size()) - 1UL))
                              {
                                 if (oq_AlsoSetCurrent == true)
                                 {
                                    this->scrollToItem(pc_ChildItem);
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   m_ReconnectSelection();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save current expanded items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_SaveExpand(void)
{
   this->mc_ExpandedMessageIds.clear();
   for (int32_t s32_ItTopLevel = 0; s32_ItTopLevel < this->topLevelItemCount(); ++s32_ItTopLevel)
   {
      QTreeWidgetItem * const pc_Item = this->topLevelItem(s32_ItTopLevel);
      if (pc_Item != NULL)
      {
         if (pc_Item->isExpanded() == true)
         {
            const uint32_t u32_Message = static_cast<uint32_t>(s32_ItTopLevel);
            if (u32_Message < this->mc_UniqueMessageIds.size())
            {
               this->mc_ExpandedMessageIds.push_back(this->mc_UniqueMessageIds[u32_Message]);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore last known expanded items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_RestoreExpand(void)
{
   this->collapseAll();
   uint32_t u32_InternalIndex;
   for (uint32_t u32_ItPrevExpansion = 0; u32_ItPrevExpansion < this->mc_ExpandedMessageIds.size();
        ++u32_ItPrevExpansion)
   {
      const C_OscCanMessageIdentificationIndices & rc_Message = this->mc_ExpandedMessageIds[u32_ItPrevExpansion];

      //Check if still in tree
      if (m_MapMessageIdToInternalMessageIndex(rc_Message, u32_InternalIndex) == C_NO_ERR)
      {
         if (u32_InternalIndex < this->mc_UniqueMessageIds.size())
         {
            QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(u32_InternalIndex);
            if (pc_TopLevelItem != NULL)
            {
               pc_TopLevelItem->setExpanded(true);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Map internal sorted signal index to global signal index

   \param[in]   oru32_InternalMessageIndex   Internal message index
   \param[in]   oru32_InternalSignalIndex    Internal signal index
   \param[out]  oru32_SignalDataIndex        Data signal index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageSelectorTreeWidget::m_MapSignalInternalIndexToDataIndex(
   const uint32_t & oru32_InternalMessageIndex, const uint32_t & oru32_InternalSignalIndex,
   uint32_t & oru32_SignalDataIndex) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (oru32_InternalMessageIndex < this->mc_UniqueMessageIdsSignalsOrder.size())
   {
      const std::vector<uint32_t> & rc_Signals = this->mc_UniqueMessageIdsSignalsOrder[oru32_InternalMessageIndex];
      if (oru32_InternalSignalIndex < rc_Signals.size())
      {
         oru32_SignalDataIndex = rc_Signals[oru32_InternalSignalIndex];
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Map global signal index to internal signal index

   \param[in]   oru32_InternalMessageIndex   Internal message index
   \param[in]   oru32_DataSignalIndex        Internal signal index
   \param[out]  oru32_SignalInternalIndex    Data signal index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageSelectorTreeWidget::m_MapSignalDataIndexToInternalIndex(
   const uint32_t & oru32_InternalMessageIndex, const uint32_t & oru32_DataSignalIndex,
   uint32_t & oru32_SignalInternalIndex) const
{
   int32_t s32_Retval = C_RANGE;

   if (oru32_InternalMessageIndex < this->mc_UniqueMessageIdsSignalsOrder.size())
   {
      const std::vector<uint32_t> & rc_Signals = this->mc_UniqueMessageIdsSignalsOrder[oru32_InternalMessageIndex];
      for (uint32_t u32_ItSignal = 0; u32_ItSignal < rc_Signals.size(); ++u32_ItSignal)
      {
         if (rc_Signals[u32_ItSignal] == oru32_DataSignalIndex)
         {
            oru32_SignalInternalIndex = u32_ItSignal;
            s32_Retval = C_NO_ERR;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
uint16_t C_SdBueMessageSelectorTreeWidget::mh_GetStartBit(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);

   return C_SdBueMessageSelectorTreeWidget::mh_GetStartBit(pc_Message, mu16_BUS_DEFAULT_SIGNAL_LENGTH,
                                                           stw::opensyde_core::C_OscCanSignal::eBYTE_ORDER_INTEL);
}

//----------------------------------------------------------------------------------------------------------------------
uint16_t C_SdBueMessageSelectorTreeWidget::mh_GetStartBit(const C_OscCanMessage * const opc_Message,
                                                          const uint16_t ou16_SignalSize,
                                                          const C_OscCanSignal::E_ByteOrderType oe_Type)
{
   uint16_t u16_StartBit = 0U;

   if (opc_Message != NULL)
   {
      std::set<uint16_t> c_SetUsedBits;
      uint32_t u32_SignalCounter;
      uint16_t u16_Counter;
      uint16_t u16_BitCounter;
      uint16_t u16_GapSizeCounter = 0U;
      int16_t s16_FirstUnusedBit = -1;
      bool q_GapFound = false;

      // get all used bits
      for (u32_SignalCounter = 0U; u32_SignalCounter < opc_Message->c_Signals.size(); ++u32_SignalCounter)
      {
         // the set will not be cleared in the function
         opc_Message->c_Signals[u32_SignalCounter].GetDataBytesBitPositionsOfSignal(c_SetUsedBits);
      }

      // search the gap and a free start bit
      for (u16_Counter = 0U; u16_Counter < 64U; ++u16_Counter)
      {
         if (oe_Type == C_OscCanSignal::eBYTE_ORDER_MOTOROLA)
         {
            // check the bits from "left" when it is motorola format
            u16_BitCounter = static_cast<uint16_t>((7U - (static_cast<uint32_t>(u16_Counter) % 8U)) +
                                                   ((static_cast<uint32_t>(u16_Counter) / 8U) * 8U));
         }
         else
         {
            u16_BitCounter = u16_Counter;
         }

         if (c_SetUsedBits.find(u16_BitCounter) == c_SetUsedBits.end())
         {
            // it is a free bit
            if (u16_GapSizeCounter == 0U)
            {
               // the first bit of a potential gap
               u16_StartBit = u16_BitCounter;
            }

            ++u16_GapSizeCounter;

            if (u16_GapSizeCounter >= ou16_SignalSize)
            {
               // actual gap is big enough
               q_GapFound = true;
               break;
            }

            if (s16_FirstUnusedBit == -1)
            {
               // the first free bit
               // save it for the fallback if no big enough gap can be found
               s16_FirstUnusedBit = static_cast<int16_t>(u16_BitCounter);
            }
         }
         else
         {
            // not free, reset the counter
            u16_GapSizeCounter = 0U;
         }
      }

      if ((q_GapFound == false) && (s16_FirstUnusedBit >= 0))
      {
         // no gap found but a free start bit
         u16_StartBit = static_cast<uint16_t>(s16_FirstUnusedBit);
      }
      else if (q_GapFound == false)
      {
         // no gap and no free start bit found. Use default
         u16_StartBit = 0U;
      }
      else
      {
         // a free gap for 1 byte found, nothing to do, u16_StartBit was already set
      }
   }

   return u16_StartBit;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get highest selected model index

   \param[in,out]  orc_Index  Highest selected, if any

   \return
   true  Success / output valid
   false No selection
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMessageSelectorTreeWidget::m_GetHighestSelected(QModelIndex & orc_Index) const
{
   bool q_Retval;
   const QModelIndexList c_Selection = this->selectedIndexes();

   if (c_Selection.size() > 0)
   {
      int32_t s32_CurrentRow = -1;
      bool q_MessageFound = false;
      bool q_SignalFound = false;
      q_Retval = true;
      for (int32_t s32_ItSelectedItem = 0; s32_ItSelectedItem < c_Selection.count(); ++s32_ItSelectedItem)
      {
         const QModelIndex & rc_CurrentItem = c_Selection[s32_ItSelectedItem];
         if (rc_CurrentItem.row() > s32_CurrentRow)
         {
            s32_CurrentRow = rc_CurrentItem.row();
            orc_Index = rc_CurrentItem;
         }
         if (rc_CurrentItem.parent().isValid() == true)
         {
            q_SignalFound = true;
         }
         else
         {
            q_MessageFound = true;
         }
      }
      //If both selected do special handling
      if ((q_SignalFound == true) && (q_MessageFound == true))
      {
         //Use current as tie breaker
         const QModelIndex c_Current = this->currentIndex();
         s32_CurrentRow = -1;
         if ((c_Current.isValid() == true) && (c_Current.parent().isValid() == true))
         {
            //Signals
            for (int32_t s32_ItSelectedItem = 0; s32_ItSelectedItem < c_Selection.count(); ++s32_ItSelectedItem)
            {
               const QModelIndex & rc_CurrentItem = c_Selection[s32_ItSelectedItem];
               if ((rc_CurrentItem.parent().isValid() == true) &&
                   (c_Current.parent().row() == rc_CurrentItem.parent().row()))
               {
                  if (rc_CurrentItem.row() > s32_CurrentRow)
                  {
                     s32_CurrentRow = rc_CurrentItem.row();
                     orc_Index = rc_CurrentItem;
                  }
               }
            }
         }
         else
         {
            //Messages
            for (int32_t s32_ItSelectedItem = 0; s32_ItSelectedItem < c_Selection.count(); ++s32_ItSelectedItem)
            {
               const QModelIndex & rc_CurrentItem = c_Selection[s32_ItSelectedItem];
               if (rc_CurrentItem.parent().isValid() == false)
               {
                  if (rc_CurrentItem.row() > s32_CurrentRow)
                  {
                     s32_CurrentRow = rc_CurrentItem.row();
                     orc_Index = rc_CurrentItem;
                  }
               }
            }
         }
      }
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get j1939 default node assignment

   \param[in]  orc_NodeInfo   Node info

   \return
   J1939 default node assignment
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_CieImportDataAssignment> C_SdBueMessageSelectorTreeWidget::m_GetJ1939DefaultNodeAssignment(
   const C_CieConverter::C_CieNode & orc_NodeInfo) const
{
   // Assign the imported messages to the first active node on the current bus
   std::vector<C_CieImportDataAssignment> c_NodeAssignments;
   C_CieImportDataAssignment c_NodeAssignmentConverted;
   c_NodeAssignmentConverted.c_ImportData = C_CieDataPoolListAdapter::h_GetStructureFromDbcFileImport(orc_NodeInfo);
   if (this->mq_ModeSingleNode)
   {
      c_NodeAssignmentConverted.u32_OsyNodeIndex = this->mu32_NodeIndex;
      c_NodeAssignmentConverted.u32_OsyInterfaceIndex = this->mu32_InterfaceIndex;
   }
   else
   {
      // Fetch nodes and interfaces of the current bus
      std::vector<uint32_t> c_NodeIndexes;
      std::vector<uint32_t> c_InterfaceIndexes;

      C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_BusIndex,
                                                                                         c_NodeIndexes,
                                                                                         c_InterfaceIndexes);

      // Find an active node to which the messages imported from the catalog will be added
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         bool q_NodeFound = false;
         for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
         {
            std::vector<const C_OscCanMessageContainer *> c_MessageContainers =
               C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainers(c_NodeIndexes[u32_ItNode],
                                                                                this->me_ProtocolType,
                                                                                c_InterfaceIndexes[u32_ItNode]);
            uint32_t u32_Counter;
            for (u32_Counter = 0U; u32_Counter < c_MessageContainers.size(); ++u32_Counter)
            {
               const C_OscCanMessageContainer * const pc_MessageContainer = c_MessageContainers[u32_Counter];
               if (pc_MessageContainer->q_IsComProtocolUsedByInterface == true)
               {
                  c_NodeAssignmentConverted.u32_OsyNodeIndex = c_NodeIndexes[u32_ItNode];
                  c_NodeAssignmentConverted.u32_OsyInterfaceIndex = c_InterfaceIndexes[u32_ItNode];
                  q_NodeFound = true;
                  break;
               }
            }

            if (q_NodeFound == true)
            {
               break;
            }
         }
      }
   }
   c_NodeAssignments.push_back(c_NodeAssignmentConverted);
   return c_NodeAssignments;
}
