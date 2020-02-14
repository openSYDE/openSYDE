//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget for bus messages (implementation)

   Tree widget for bus messages

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

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

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "TGLUtils.h"
#include "C_SdUtil.h"
#include "C_GtGetText.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdBueMessageSelectorTreeWidget.h"
#include "C_PuiSdHandler.h"
#include "C_SdClipBoardHelper.h"
#include "C_SdBueSortHelper.h"
#include "C_SdBusMessageSelectorTreeWidgetItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

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
   stw_opensyde_gui_elements::C_OgeTreeWidgetToolTipBase(opc_Parent),
   mpc_UndoManager(NULL),
   mpc_MessageSyncManager(NULL),
   mq_StopSigSelectionChanged(false),
   mq_ModeSingleNode(false),
   mu32_NodeIndex(0),
   mu32_InterfaceIndex(0),
   mu32_BusIndex(0),
   me_ProtocolType(C_OSCCanProtocol::eLAYER2),
   mq_NoSelectionUpdate(false)
{
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   //Drag & drop
   this->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(true);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this,
           &C_SdBueMessageSelectorTreeWidget::m_SelectionChanged);
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
C_SdBueMessageSelectorTreeWidget::~C_SdBueMessageSelectorTreeWidget(void)
{
   //lint -e{1540}  no memory leak because mpc_UndoManager and mpc_MessageSyncManager are not managed in this module
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reimplement default size hint

   \return
   Current required size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SdBueMessageSelectorTreeWidget::sizeHint(void) const
{
   sintn sn_CounterTopLevelItem;
   sintn sn_CounterChild;
   sintn sn_Height = 0;
   QRect c_Rect;

   for (sn_CounterTopLevelItem = 0; sn_CounterTopLevelItem < this->topLevelItemCount(); ++sn_CounterTopLevelItem)
   {
      QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(sn_CounterTopLevelItem);
      c_Rect = this->visualItemRect(pc_TopLevelItem);

      // add the height of the top level item
      sn_Height += c_Rect.height();

      // add the height of the children
      for (sn_CounterChild = 0; sn_CounterChild < pc_TopLevelItem->childCount(); ++sn_CounterChild)
      {
         c_Rect = this->visualItemRect(pc_TopLevelItem->child(sn_CounterChild));
         sn_Height += c_Rect.height();
      }
   }

   // add an offset for the line drawing
   sn_Height += 2;

   return QSize(QTreeWidget::sizeHint().width(), sn_Height);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active node id

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  orc_DatapoolIndexes  All Datapool indexes associated to the same protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SetNodeId(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                 const std::vector<uint32> & orc_DatapoolIndexes)
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
void C_SdBueMessageSelectorTreeWidget::SetBusId(const uint32 ou32_BusIndex)
{
   this->mu32_BusIndex = ou32_BusIndex;
   this->mq_ModeSingleNode = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current com protocol

   \param[in]  ore_Value   Com protocol value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SetProtocolType(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value)
{
   this->me_ProtocolType = ore_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Deselects all items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::DeselectAllItems(void)
{
   sintn sn_CounterTopLevelItem;

   this->mq_StopSigSelectionChanged = true;

   for (sn_CounterTopLevelItem = 0; sn_CounterTopLevelItem < this->topLevelItemCount(); ++sn_CounterTopLevelItem)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdBusMessageSelectorTreeWidgetItem * const pc_TopLevelItem =
         dynamic_cast<C_SdBusMessageSelectorTreeWidgetItem *>(this->topLevelItem(sn_CounterTopLevelItem));
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
void C_SdBueMessageSelectorTreeWidget::SetUndoManager(stw_opensyde_gui_logic::C_SdBueUnoManager * const opc_Value)
{
   this->mpc_UndoManager = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out]  opc_Value  Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SetMessageSyncManager(
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
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
         //Message
         C_OSCCanMessageIdentificationIndices c_MessageId;

         if (this->GetMessageIdForAdd(c_MessageId) == C_NO_ERR)
         {
            //Core
            this->mpc_UndoManager->DoAddMessage(c_MessageId, this->mpc_MessageSyncManager, this);

            this->SelectMessage(c_MessageId, false);
         }
      }
      else
      {
         //Signal
         const uint32 u32_MessageIndex = c_Selection.parent().row();
         if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
         {
            const C_OSCCanMessageIdentificationIndices c_MessageId = this->mc_UniqueMessageIds[u32_MessageIndex];
            const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
            if (pc_Message != NULL)
            {
               const uint32 u32_ItSignal = pc_Message->c_Signals.size();
               const uint16 u16_StartBit = C_SdBueMessageSelectorTreeWidget::mh_GetStartBit(c_MessageId);

               //Core
               this->mpc_UndoManager->DoAddSignal(c_MessageId, u32_ItSignal, u16_StartBit,
                                                  C_OSCCanSignal::eMUX_DEFAULT, 0,
                                                  this->mpc_MessageSyncManager,
                                                  this);

               this->SelectSignal(c_MessageId, u32_ItSignal, false);
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
      C_OSCCanMessageIdentificationIndices c_MessageId;

      if (this->GetMessageIdForAdd(c_MessageId) == C_NO_ERR)
      {
         //Core
         this->mpc_UndoManager->DoAddMessage(c_MessageId, this->mpc_MessageSyncManager, this);

         this->SelectMessage(c_MessageId, false);
         Q_EMIT (this->SigSelectName());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action add signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::AddSignal(void)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      QModelIndex c_Selection;
      const bool q_Result = this->m_GetHighestSelected(c_Selection);
      if (q_Result == true)
      {
         uint32 u32_MessageIndex;
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
            const C_OSCCanMessageIdentificationIndices c_MessageId = this->mc_UniqueMessageIds[u32_MessageIndex];
            const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
            if (pc_Message != NULL)
            {
               const uint32 u32_ItSignal = pc_Message->c_Signals.size();

               const uint16 u16_StartBit = C_SdBueMessageSelectorTreeWidget::mh_GetStartBit(c_MessageId);

               //Core
               this->mpc_UndoManager->DoAddSignal(c_MessageId, u32_ItSignal, u16_StartBit,
                                                  C_OSCCanSignal::eMUX_DEFAULT, 0,
                                                  this->mpc_MessageSyncManager,
                                                  this);
               this->SelectSignal(c_MessageId, u32_ItSignal, false);
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
void C_SdBueMessageSelectorTreeWidget::AddSignalWithStartBit(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                             const uint16 ou16_StartBit,
                                                             const bool oq_MultiplexedSignal,
                                                             const uint16 ou16_MultiplexValue)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
      if (pc_Message != NULL)
      {
         const uint32 u32_ItSignal = pc_Message->c_Signals.size();

         //Core
         if (oq_MultiplexedSignal == true)
         {
            this->mpc_UndoManager->DoAddSignal(orc_MessageId, u32_ItSignal, ou16_StartBit,
                                               C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL,
                                               ou16_MultiplexValue, this->mpc_MessageSyncManager,
                                               this);
         }
         else
         {
            this->mpc_UndoManager->DoAddSignal(orc_MessageId, u32_ItSignal, ou16_StartBit, C_OSCCanSignal::eMUX_DEFAULT,
                                               0, this->mpc_MessageSyncManager,
                                               this);
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
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      const QModelIndexList c_IndexList = this->selectedIndexes();
      std::vector<C_OSCCanMessageIdentificationIndices> c_SelectedMessageIds;
      std::vector<C_OSCCanMessageIdentificationIndices> c_SelectedSignalMessageIds;
      std::vector<uint32> c_SelectedSignals;
      std::vector<uint32> c_Dummy;
      for (QModelIndexList::const_iterator c_ItIndex = c_IndexList.begin(); c_ItIndex != c_IndexList.end(); ++c_ItIndex)
      {
         if (c_ItIndex->parent().isValid() == false)
         {
            //Message
            if (c_ItIndex->row() >= 0)
            {
               const uint32 u32_MessageIndex = c_ItIndex->row();

               tgl_assert(u32_MessageIndex < this->mc_UniqueMessageIds.size());
               if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
               {
                  c_SelectedMessageIds.push_back(this->mc_UniqueMessageIds[u32_MessageIndex]);
               }
            }
         }
         else
         {
            //Signal
            if (c_ItIndex->parent().row() >= 0)
            {
               const uint32 u32_MessageIndex = c_ItIndex->parent().row();
               if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
               {
                  if (c_ItIndex->row() >= 0)
                  {
                     const uint32 u32_SignalIndex = c_ItIndex->row();
                     uint32 u32_SignalDataIndex;
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
         std::vector<C_OSCCanMessageIdentificationIndices> c_Groups;
         std::vector<std::vector<uint32> > c_GroupMessages;
         c_Groups.push_back(c_SelectedMessageIds[0]);
         c_GroupMessages.resize(1);
         //First element
         c_GroupMessages[0].push_back(c_SelectedMessageIds[0].u32_MessageIndex);
         //Group by node & com protocol & interface & transmit flag
         for (uint32 u32_ItMessage = 1; u32_ItMessage < c_SelectedMessageIds.size(); ++u32_ItMessage)
         {
            uint32 u32_ItGroup;
            const C_OSCCanMessageIdentificationIndices & rc_CurMessage = c_SelectedMessageIds[u32_ItMessage];
            //Search for matching group
            for (u32_ItGroup = 0; u32_ItGroup < c_Groups.size(); ++u32_ItGroup)
            {
               const C_OSCCanMessageIdentificationIndices & rc_CurGroup = c_Groups[u32_ItGroup];
               std::vector<uint32> & rc_CurGroupMessages = c_GroupMessages[u32_ItGroup];

               //Compare everything except message id
               if ((((rc_CurMessage.u32_NodeIndex == rc_CurGroup.u32_NodeIndex) &&
                     (rc_CurMessage.e_ComProtocol == rc_CurGroup.e_ComProtocol)) &&
                    (rc_CurMessage.u32_InterfaceIndex == rc_CurGroup.u32_InterfaceIndex)) &&
                   (rc_CurMessage.q_MessageIsTx == rc_CurGroup.q_MessageIsTx))
               {
                  rc_CurGroupMessages.push_back(rc_CurMessage.u32_MessageIndex);
                  break;
               }
            }
            //If no match add new group
            if (u32_ItGroup == c_Groups.size())
            {
               c_Groups.push_back(rc_CurMessage);
               c_GroupMessages.resize(c_Groups.size());
               //Add first matching element
               c_GroupMessages[static_cast<std::vector< std::vector< uint32> >::size_type >
                               (c_Groups.size() - 1UL)].push_back(rc_CurMessage.u32_MessageIndex);
            }
         }
         //Sort per group
         for (uint32 u32_ItGroup = 0; u32_ItGroup < c_GroupMessages.size(); ++u32_ItGroup)
         {
            C_SdUtil::h_SortIndicesAscending(c_GroupMessages[u32_ItGroup]);
         }
         //Delete messages per group (Descending)
         tgl_assert(c_Groups.size() == c_GroupMessages.size());
         for (uint32 u32_ItGroup = 0; u32_ItGroup < c_Groups.size(); ++u32_ItGroup)
         {
            const C_OSCCanMessageIdentificationIndices & rc_CurGroup = c_Groups[u32_ItGroup];
            for (uint32 u32_ItGroupMessage = c_GroupMessages[u32_ItGroup].size(); u32_ItGroupMessage > 0;
                 --u32_ItGroupMessage)
            {
               const uint32 u32_GroupMessageIndex =
                  c_GroupMessages[u32_ItGroup][static_cast<std::vector< uint32>::size_type >
                                               (u32_ItGroupMessage - 1UL)];
               const C_OSCCanMessageIdentificationIndices c_MessageId(rc_CurGroup.u32_NodeIndex,
                                                                      rc_CurGroup.e_ComProtocol,
                                                                      rc_CurGroup.u32_InterfaceIndex,
                                                                      rc_CurGroup.u32_DatapoolIndex,
                                                                      rc_CurGroup.q_MessageIsTx,
                                                                      u32_GroupMessageIndex);
               this->mpc_UndoManager->DoDeleteMessage(c_MessageId, this->mpc_MessageSyncManager,
                                                      this);
            }
         }
      }
      //Sort signals
      c_Dummy.resize(c_SelectedSignals.size());
      C_SdUtil::h_SortIndicesDescendingAndSync<C_OSCCanMessageIdentificationIndices, uint32>(c_SelectedSignals,
                                                                                             c_SelectedSignalMessageIds,
                                                                                             c_Dummy);
      //Delete signals
      for (uint32 u32_ItSignal = 0; u32_ItSignal < c_SelectedSignals.size(); ++u32_ItSignal)
      {
         this->mpc_UndoManager->DoDeleteSignal(c_SelectedSignalMessageIds[u32_ItSignal],
                                               c_SelectedSignals[u32_ItSignal], this->mpc_MessageSyncManager, this);
      }
      //Merge all delete commands
      this->mpc_UndoManager->CommitDelete();
      //Ui update trigger
      this->updateGeometry();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::DeleteSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                    const uint32 ou32_SignalIndex)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      //Core
      this->mpc_UndoManager->DoDeleteSignal(orc_MessageId, ou32_SignalIndex,
                                            this->mpc_MessageSyncManager,
                                            this);

      //Merge all delete commands
      this->mpc_UndoManager->CommitDelete();
      //Ui update trigger
      this->updateGeometry();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action copy
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::Copy(void)
{
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
         uint32 u32_ItVec = 0;
         std::vector<C_OSCCanMessage> c_Messages;
         std::vector<std::vector<C_OSCNodeDataPoolListElement> > c_OSCSignalCommons;
         std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > c_UISignalCommons;
         std::vector<std::vector<C_PuiSdNodeCanSignal> > c_UISignals;
         std::vector<std::vector<QString> > c_OwnerNodeName;
         std::vector<std::vector<uint32> > c_OwnerNodeInterfaceIndex;
         std::vector<std::vector<uint32> > c_OwnerNodeDatapoolIndex;
         std::vector<std::vector<bool> > c_OwnerIsTxFlag;

         //Reserve
         c_Messages.resize(c_IndexList.size());
         c_OSCSignalCommons.resize(c_IndexList.size());
         c_UISignalCommons.resize(c_IndexList.size());
         c_UISignals.resize(c_IndexList.size());

         for (QModelIndexList::const_iterator c_ItIndex = c_IndexList.begin(); c_ItIndex != c_IndexList.end();
              ++c_ItIndex)
         {
            tgl_assert(c_ItIndex->row() >= 0);
            if (c_ItIndex->row() >= 0)
            {
               const uint32 u32_MessageIndex = c_ItIndex->row();
               tgl_assert(u32_MessageIndex < this->mc_UniqueMessageIds.size());
               if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
               {
                  std::vector<QString> c_OwnerNodeNamePerMessage;
                  std::vector<uint32> c_OwnerNodeInterfaceIndexPerMessage;
                  std::vector<uint32> c_OwnerNodeDatapoolIndexPerMessage;

                  std::vector<bool> c_OwnerIsTxFlagPerMessage;
                  const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessages =
                     this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_UniqueMessageIds[
                                                                               u32_MessageIndex]);
                  tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanMessageComplete(this->mc_UniqueMessageIds[
                                                                                       u32_MessageIndex],
                                                                                    c_Messages[u32_ItVec],
                                                                                    c_OSCSignalCommons[u32_ItVec],
                                                                                    c_UISignalCommons[u32_ItVec],
                                                                                    c_UISignals[u32_ItVec]) ==
                             C_NO_ERR);

                  //Get owner data
                  c_OwnerNodeNamePerMessage.reserve(c_MatchingMessages.size());
                  c_OwnerNodeInterfaceIndexPerMessage.reserve(c_MatchingMessages.size());
                  c_OwnerNodeDatapoolIndexPerMessage.reserve(c_MatchingMessages.size());
                  c_OwnerIsTxFlagPerMessage.reserve(c_MatchingMessages.size());
                  for (uint32 u32_ItMatchingMessage = 0; u32_ItMatchingMessage < c_MatchingMessages.size();
                       ++u32_ItMatchingMessage)
                  {
                     const C_OSCCanMessageIdentificationIndices & rc_CurMessageId =
                        c_MatchingMessages[u32_ItMatchingMessage];
                     const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                        rc_CurMessageId.u32_NodeIndex);
                     tgl_assert(pc_Node != NULL);
                     if (pc_Node != NULL)
                     {
                        c_OwnerNodeNamePerMessage.push_back(pc_Node->c_Properties.c_Name.c_str());
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
         C_SdClipBoardHelper::h_StoreMessages(c_Messages, c_OSCSignalCommons, c_UISignalCommons, c_UISignals,
                                              c_OwnerNodeName, c_OwnerNodeInterfaceIndex, c_OwnerNodeDatapoolIndex,
                                              c_OwnerIsTxFlag);
      }
      if (q_SignalsOnly == true)
      {
         uint32 u32_ItVec = 0;
         std::vector<C_OSCCanSignal> c_Signals;
         std::vector<C_OSCNodeDataPoolListElement> c_OSCSignalCommons;
         std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommons;
         std::vector<C_PuiSdNodeCanSignal> c_UISignals;

         //Reserve
         c_Signals.resize(c_IndexList.size());
         c_OSCSignalCommons.resize(c_IndexList.size());
         c_UISignalCommons.resize(c_IndexList.size());
         c_UISignals.resize(c_IndexList.size());

         for (QModelIndexList::const_iterator c_ItIndex = c_IndexList.begin(); c_ItIndex != c_IndexList.end();
              ++c_ItIndex)
         {
            tgl_assert(c_ItIndex->parent().row() >= 0);
            if (c_ItIndex->parent().row() >= 0)
            {
               const uint32 u32_MessageIndex = c_ItIndex->parent().row();
               tgl_assert(u32_MessageIndex < this->mc_UniqueMessageIds.size());
               if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
               {
                  tgl_assert(c_ItIndex->row() >= 0);
                  if (c_ItIndex->row() >= 0)
                  {
                     const uint32 u32_SignalIndex = c_ItIndex->row();
                     uint32 u32_SignalDataIndex;
                     if (this->m_MapSignalInternalIndexToDataIndex(u32_MessageIndex, u32_SignalIndex,
                                                                   u32_SignalDataIndex) == C_NO_ERR)
                     {
                        tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanSignalComplete(this->mc_UniqueMessageIds[
                                                                                            u32_MessageIndex],
                                                                                         u32_SignalDataIndex,
                                                                                         c_Signals[u32_ItVec],
                                                                                         c_OSCSignalCommons[u32_ItVec],
                                                                                         c_UISignalCommons[u32_ItVec],
                                                                                         c_UISignals[u32_ItVec]) ==
                                   C_NO_ERR);
                     }
                  }
               }
            }
            ++u32_ItVec;
         }
         C_SdClipBoardHelper::h_StoreSignalsToClipboard(c_Signals, c_OSCSignalCommons, c_UISignalCommons, c_UISignals);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::CopySignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                  const uint32 ou32_SignalIndex) const
{
   const uint32 u32_ItVec = 0;

   std::vector<C_OSCCanSignal> c_Signals;
   std::vector<C_OSCNodeDataPoolListElement> c_OSCSignalCommons;
   std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommons;
   std::vector<C_PuiSdNodeCanSignal> c_UISignals;

   //Reserve
   c_Signals.resize(1);
   c_OSCSignalCommons.resize(1);
   c_UISignalCommons.resize(1);
   c_UISignals.resize(1);

   tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanSignalComplete(orc_MessageId,
                                                                    ou32_SignalIndex,
                                                                    c_Signals[u32_ItVec],
                                                                    c_OSCSignalCommons[u32_ItVec],
                                                                    c_UISignalCommons[u32_ItVec],
                                                                    c_UISignals[u32_ItVec]) ==
              C_NO_ERR);

   C_SdClipBoardHelper::h_StoreSignalsToClipboard(c_Signals, c_OSCSignalCommons, c_UISignalCommons, c_UISignals);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action cut
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::Cut(void)
{
   Copy();

   Delete();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cut specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::CutSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                 const uint32 ou32_SignalIndex)
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
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      std::vector<C_OSCCanSignal> c_Signals;
      std::vector<C_OSCNodeDataPoolListElement> c_OSCSignalCommons;
      std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommons;
      std::vector<C_PuiSdNodeCanSignal> c_UISignals;
      if (C_SdClipBoardHelper::h_LoadSignalsFromClipboard(c_Signals, c_OSCSignalCommons, c_UISignalCommons,
                                                          c_UISignals) == C_NO_ERR)
      {
         //Valid signals
         QModelIndex c_Selection;
         const bool q_Result = this->m_GetHighestSelected(c_Selection);
         if (q_Result == true)
         {
            bool q_Continue = false;
            bool q_IsOtherMessage = false;
            uint32 u32_InternalMessageIndex = 0UL;
            if (c_Selection.parent().isValid() == true)
            {
               //Paste Signals in the same message
               if (c_Selection.parent().row() >= 0)
               {
                  u32_InternalMessageIndex = static_cast<uint32>(c_Selection.parent().row());
                  q_Continue = true;
               }
            }
            else
            {
               // Paste Signals in an other Message
               if (c_Selection.row() >= 0)
               {
                  u32_InternalMessageIndex = static_cast<uint32>(c_Selection.row());
                  q_IsOtherMessage = true;
                  q_Continue = true;
               }
            }
            if ((q_Continue) && (u32_InternalMessageIndex < this->mc_UniqueMessageIds.size()))
            {
               const C_OSCCanMessage * const pc_Message =
                  C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_UniqueMessageIds[
                                                                    u32_InternalMessageIndex]);
               if (pc_Message != NULL)
               {
                  const bool q_ContainsMultiplexer = C_OSCCanMessage::h_ContainsMultiplexer(c_Signals);
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
                     const uint32 u32_SignalIndex = pc_Message->c_Signals.size();

                     //For other message the current signal colors might not work
                     if (q_IsOtherMessage)
                     {
                        //Reset colors
                        c_UISignals.clear();
                        c_UISignals.resize(c_Signals.size());
                     }

                     //Auto fix signal restrictions of safety protocols
                     for (uint32 u32_ItSignal = 0; u32_ItSignal < c_Signals.size(); ++u32_ItSignal)
                     {
                        C_SdUtil::h_AdaptSignalToProtocolType(c_Signals[u32_ItSignal], this->me_ProtocolType, NULL);
                     }

                     this->mpc_UndoManager->DoPasteSignals(this->mc_UniqueMessageIds[u32_InternalMessageIndex],
                                                           u32_SignalIndex, c_Signals, c_OSCSignalCommons,
                                                           c_UISignalCommons, c_UISignals,
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
                        if (pc_Message->e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_CHANGE)
                        {
                           C_OSCCanMessage c_Copy = *pc_Message;
                           c_Copy.u32_CycleTimeMs = 0U;
                           c_Copy.e_TxMethod = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
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
         std::vector<C_OSCCanMessage> c_Messages;
         std::vector<std::vector<C_OSCNodeDataPoolListElement> > c_OSCMsgSignalCommons;
         std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > c_UIMsgSignalCommons;
         std::vector<std::vector<C_PuiSdNodeCanSignal> > c_UIMsgSignals;
         std::vector<std::vector<QString> > c_OwnerNodeName;
         std::vector<std::vector<uint32> > c_OwnerNodeInterfaceIndex;
         std::vector<std::vector<uint32> > c_OwnerNodeDatapoolIndex;
         std::vector<std::vector<bool> > c_OwnerIsTxFlag;
         if (C_SdClipBoardHelper::h_LoadMessages(c_Messages, c_OSCMsgSignalCommons, c_UIMsgSignalCommons,
                                                 c_UIMsgSignals, c_OwnerNodeName, c_OwnerNodeInterfaceIndex,
                                                 c_OwnerNodeDatapoolIndex, c_OwnerIsTxFlag) == C_NO_ERR)
         {
            C_OSCCanMessageIdentificationIndices c_MessageId;
            //Auto fix messages
            for (uint32 u32_ItMessage = 0; u32_ItMessage < c_Messages.size(); ++u32_ItMessage)
            {
               C_SdUtil::h_AdaptMessageToProtocolType(c_Messages[u32_ItMessage], this->me_ProtocolType, NULL);
            }
            //Valid messages
            if (GetMessageIdForAdd(c_MessageId) == C_NO_ERR)
            {
               std::vector<C_OSCCanMessageIdentificationIndices> c_NewIds;
               this->mpc_UndoManager->DoPasteMessages(c_MessageId, c_Messages, c_OSCMsgSignalCommons,
                                                      c_UIMsgSignalCommons,
                                                      c_UIMsgSignals, c_OwnerNodeName, c_OwnerNodeInterfaceIndex,
                                                      c_OwnerNodeDatapoolIndex,
                                                      c_OwnerIsTxFlag, this->mpc_MessageSyncManager, this, c_NewIds);
               //Selection
               this->SelectMessages(c_NewIds, false);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paste copied signal

   \param[in]  orc_MessageId  Message identification indices
   \param[in]  ou16_StartBit  Start bit for new signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::PasteSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                   const uint16 ou16_StartBit)
{
   tgl_assert(this->mpc_UndoManager != NULL);
   if (this->mpc_UndoManager != NULL)
   {
      std::vector<C_OSCCanSignal> c_Signals;
      std::vector<C_OSCNodeDataPoolListElement> c_OSCSignalCommons;
      std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommons;
      std::vector<C_PuiSdNodeCanSignal> c_UISignals;
      if (C_SdClipBoardHelper::h_LoadSignalsFromClipboard(c_Signals, c_OSCSignalCommons, c_UISignalCommons,
                                                          c_UISignals) == C_NO_ERR)
      {
         // pasting of only one concrete signal allowed with a concrete start bit
         if (c_Signals.size() == 1)
         {
            const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
            if (pc_Message != NULL)
            {
               const uint32 u32_SignalIndex = pc_Message->c_Signals.size();

               //Reset colors
               c_UISignals.clear();
               c_UISignals.resize(c_Signals.size());
               c_Signals[0].u16_ComBitStart = ou16_StartBit;
               this->mpc_UndoManager->DoPasteSignals(orc_MessageId,
                                                     u32_SignalIndex, c_Signals, c_OSCSignalCommons,
                                                     c_UISignalCommons, c_UISignals, this->mpc_MessageSyncManager,
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
/*! \brief   Clean up class after internal add message

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InternalAddMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   uint32 u32_InternalMessageIndex = 0;

   //Internal (BEFORE ui)
   m_UpdateUniqueMessageIds();
   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      //Ui
      m_InsertMessage(u32_InternalMessageIndex);
      //Ui update trigger
      this->updateGeometry();
      //Error handling
      RecheckErrorGlobal(false);
      //Signal
      Q_EMIT this->SigMessageCountChanged();
      //Otherwise there is no error update
      Q_EMIT this->SigErrorChanged();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class after internal delete message

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InternalDeleteMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   uint32 u32_InternalMessageIndex = 0;

   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      sint32 s32_NewSelection;
      //Internal (BEFORE ui)
      m_UpdateUniqueMessageIds();
      //Ui
      static_cast<void>(this->takeTopLevelItem(u32_InternalMessageIndex));
      //Ui update trigger
      this->updateGeometry();
      //Error handling
      RecheckErrorGlobal(false);
      //Signal
      Q_EMIT this->SigMessageCountChanged();
      //Otherwise there is no error update
      Q_EMIT this->SigErrorChanged();
      //Select near u32_InternalMessageIndex
      if (u32_InternalMessageIndex < static_cast<uint32>(this->topLevelItemCount()))
      {
         s32_NewSelection = static_cast<sint32>(u32_InternalMessageIndex);
      }
      else if (this->topLevelItemCount() > 0)
      {
         s32_NewSelection = this->topLevelItemCount() - 1L;
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class after internal add signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InternalAddSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                         const uint32 & oru32_SignalIndex)
{
   uint32 u32_InternalMessageIndex = 0;

   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      uint32 u32_SignalIndex;
      m_UpdateUniqueMessageIdsSignals(u32_InternalMessageIndex);
      if (m_MapSignalDataIndexToInternalIndex(u32_InternalMessageIndex, oru32_SignalIndex, u32_SignalIndex) == C_NO_ERR)
      {
         //Ui
         m_InsertSignal(this->topLevelItem(u32_InternalMessageIndex), u32_SignalIndex,
                        C_PuiSdHandler::h_GetInstance()->GetCanSignalDisplayName(orc_MessageId, oru32_SignalIndex));
         //Ui update trigger
         this->updateGeometry();
         //Error handling
         RecheckErrorGlobal(false);
         //Signal
         Q_EMIT this->SigSignalCountOfMessageChanged(orc_MessageId);
         Q_EMIT this->SigErrorChanged();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class after internal delete signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::InternalDeleteSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                            const uint32 & oru32_SignalIndex)
{
   uint32 u32_InternalMessageIndex = 0;

   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      uint32 u32_SignalIndex;
      if (m_MapSignalDataIndexToInternalIndex(u32_InternalMessageIndex, oru32_SignalIndex, u32_SignalIndex) == C_NO_ERR)
      {
         QTreeWidgetItem * const pc_Parent = this->topLevelItem(u32_InternalMessageIndex);

         //Ui
         tgl_assert(pc_Parent != NULL);
         if (pc_Parent != NULL)
         {
            pc_Parent->removeChild(pc_Parent->child(static_cast<sint32>(u32_SignalIndex)));
            //Ui update trigger
            this->updateGeometry();
            //Error handling
            RecheckErrorGlobal(false);
            //Signal
            Q_EMIT this->SigSignalCountOfMessageChanged(orc_MessageId);
            Q_EMIT this->SigErrorChanged();
            //Handle selection
            if (pc_Parent->childCount() > 0)
            {
               QTreeWidgetItem * pc_Child = NULL;
               if (static_cast<sint32>(u32_SignalIndex) >= pc_Parent->childCount())
               {
                  pc_Child = pc_Parent->child(pc_Parent->childCount() - 1);
               }
               else
               {
                  pc_Child = pc_Parent->child(static_cast<sint32>(u32_SignalIndex));
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
   m_RestoreSelection();
   m_RestoreExpand();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The signal name has changed

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::OnSignalNameChange(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   uint32 u32_InternalMessageIndex = 0;

   m_SaveSelection();
   if (m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalMessageIndex) == C_NO_ERR)
   {
      QTreeWidgetItem * const pc_Item = this->topLevelItem(u32_InternalMessageIndex);
      if (pc_Item != NULL)
      {
         uint32 u32_SignalIndex;
         for (uint32 u32_ItChild = 0; u32_ItChild < static_cast<uint32>(pc_Item->childCount()); ++u32_ItChild)
         {
            QTreeWidgetItem * const pc_ChildItem = pc_Item->child(u32_ItChild);

            if (m_MapSignalInternalIndexToDataIndex(u32_InternalMessageIndex, u32_ItChild, u32_SignalIndex) == C_NO_ERR)
            {
               if (pc_ChildItem != NULL)
               {
                  pc_ChildItem->setText(0,
                                        C_PuiSdHandler::h_GetInstance()->GetCanSignalDisplayName(this->
                                                                                                 mc_UniqueMessageIds[
                                                                                                    u32_InternalMessageIndex
                                                                                                 ],
                                                                                                 u32_SignalIndex));
               }
            }
         }
      }
   }
   m_RestoreSelection();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   In case of a disconnected node update of the unique message ids

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::OnNodeDisconnected(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      uint32 u32_UniqueMsgCounter;

      for (u32_UniqueMsgCounter = 0U; u32_UniqueMsgCounter < this->mc_UniqueMessageIds.size();
           ++u32_UniqueMsgCounter)
      {
         C_OSCCanMessageIdentificationIndices & rc_UniqueMsgId = this->mc_UniqueMessageIds[u32_UniqueMsgCounter];

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
   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < this->mc_UniqueMessageIds.size(); ++u32_ItMessageId)
   {
      this->RecheckError(this->mc_UniqueMessageIds[u32_ItMessageId], false);
   }
   if (orq_HandleSelection == true)
   {
      m_RestoreSelection(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger recheck of error values for tree

   \param[in]  orc_MessageId              Message identification indices
   \param[in]  orq_AllowMessageIdUpdate   Flag to allow message ID changes using when using this function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::RecheckError(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                    const bool & orq_AllowMessageIdUpdate)
{
   if (orq_AllowMessageIdUpdate == true)
   {
      m_SaveSelection();
      m_UpdateUniqueMessageIds();
   }
   if (this->mpc_MessageSyncManager != NULL)
   {
      stw_types::uint32 u32_InternalId;
      if (this->m_MapMessageIdToInternalMessageIndex(orc_MessageId, u32_InternalId) == C_NO_ERR)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SdBusMessageSelectorTreeWidgetItem * const pc_TopLevelItem =
            dynamic_cast<C_SdBusMessageSelectorTreeWidgetItem * const>(this->topLevelItem(u32_InternalId));
         const C_OSCCanMessage * pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
         const C_OSCCanMessageContainer * pc_MessageContainer =
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
            bool q_SignalsValid = true;
            this->mpc_MessageSyncManager->CheckMessageIdBus(pc_Message->u32_CanId, q_IdValid, &orc_MessageId);
            this->mpc_MessageSyncManager->CheckMessageNameBus(pc_Message->c_Name, q_NameValid, &orc_MessageId);
            this->mpc_MessageSyncManager->CheckMessageHasTx(q_HasTx, orc_MessageId);
            pc_MessageContainer->CheckMessageLocalError(NULL, orc_MessageId.u32_MessageIndex,
                                                        orc_MessageId.q_MessageIsTx, NULL, NULL, &q_DelayInvalid, NULL,
                                                        NULL, NULL, C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
                                                           orc_MessageId.e_ComProtocol));
            if (pc_Message->c_Signals.size() > 0)
            {
               const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
                  orc_MessageId.u32_NodeIndex, orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                  orc_MessageId.u32_DatapoolIndex, orc_MessageId.q_MessageIsTx);
               if (pc_List != NULL)
               {
                  uint32 u32_SignalInternalIndex;
                  for (uint32 u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
                  {
                     if (m_MapSignalDataIndexToInternalIndex(u32_InternalId, u32_ItSignal,
                                                             u32_SignalInternalIndex) == C_NO_ERR)
                     {
                        //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                        C_SdBusMessageSelectorTreeWidgetItem * const pc_ChildItem =
                           dynamic_cast<C_SdBusMessageSelectorTreeWidgetItem * const>(pc_TopLevelItem->child(
                                                                                         u32_SignalInternalIndex));
                        if (pc_ChildItem != NULL)
                        {
                           if (pc_Message->CheckErrorSignal(pc_List, u32_ItSignal,
                                                            C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
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
            if (((((q_IdValid == true) && (q_NameValid == true)) && (q_SignalsValid == true)) &&
                 (q_DelayInvalid == false)) && (q_HasTx == true))
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
      m_RestoreSelection();
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
         std::vector<uint32> c_NodeIndexes;
         std::vector<uint32> c_InterfaceIndexes;
         C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_BusIndex,
                                                                                            c_NodeIndexes,
                                                                                            c_InterfaceIndexes);
         if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
         {
            for (uint32 u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
            {
               std::vector<const C_OSCCanMessageContainer *> c_MessageContainers =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainers(c_NodeIndexes[u32_ItNode],
                                                                                   this->me_ProtocolType,
                                                                                   c_InterfaceIndexes[u32_ItNode]);
               uint32 u32_Counter;

               for (u32_Counter = 0U; u32_Counter < c_MessageContainers.size(); ++u32_Counter)
               {
                  const C_OSCCanMessageContainer * const pc_MessageContainer = c_MessageContainers[u32_Counter];
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
void C_SdBueMessageSelectorTreeWidget::SelectMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                     const bool oq_BlockSignal)
{
   std::vector<stw_types::uint32> c_Signal;

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
   this->mc_SelectedMessageIds.push_back(orc_MessageId);
   this->mc_SelectedSignals.push_back(c_Signal);

   //Do not trigger selection
   if (oq_BlockSignal == true)
   {
      m_DisconnectSelection();
   }
   m_RestoreSelection(true);

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle multi message selection change with all aspects

   \param[in]  orc_MessageIds    Multiple message identification indices
   \param[in]  oq_BlockSignal    Optional flag for blocking the signal for changed selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SelectMessages(
   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageIds, const bool oq_BlockSignal)
{
   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < orc_MessageIds.size(); ++u32_ItMessage)
   {
      const std::vector<stw_types::uint32> c_Signal;
      this->mc_SelectedMessageIds.push_back(orc_MessageIds[u32_ItMessage]);
      this->mc_SelectedSignals.push_back(c_Signal);
   }

   //Do not trigger selection
   if (oq_BlockSignal == true)
   {
      m_DisconnectSelection();
   }
   m_RestoreSelection(true);

   this->mc_SelectedMessageIds.clear();
   this->mc_SelectedSignals.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change with all aspects

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  oq_BlockSignal       Optional flag for blocking the signal for changed selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::SelectSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                    const uint32 & oru32_SignalIndex, const bool oq_BlockSignal)
{
   std::vector<stw_types::uint32> c_Signal;

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
   m_RestoreSelection(true);

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
sint32 C_SdBueMessageSelectorTreeWidget::GetLevelOfPos(const QPoint & orc_Pos) const
{
   sint32 s32_Level = 0;
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
                  QTreeWidgetItem * pc_ItemLastSignal = pc_Parent->child(pc_Parent->childCount() - 1);

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
               std::vector<C_OSCCanMessageIdentificationIndices> c_SourceMessageIds;
               std::vector<uint32> c_SourceSignalIndices;
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
                        sint32 s32_MessageIndex;
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
                           const uint32 u32_MessageIndex = static_cast<uint32>(s32_MessageIndex);
                           if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
                           {
                              const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
                                 this->mc_UniqueMessageIds[u32_MessageIndex]);
                              if (pc_Message != NULL)
                              {
                                 if (c_SourceMessageIds.size() == c_SourceSignalIndices.size())
                                 {
                                    std::vector<C_OSCCanMessageIdentificationIndices> c_TargetMessageIds;
                                    std::vector<uint32> c_TargetSignalIndices;
                                    uint32 u32_SignalCounter = pc_Message->c_Signals.size();
                                    c_TargetMessageIds.reserve(c_SourceMessageIds.size());
                                    c_TargetSignalIndices.reserve(c_SourceMessageIds.size());
                                    for (uint32 u32_ItMessage = 0; u32_ItMessage < c_SourceMessageIds.size();
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
                                       const C_OSCCanMessage * const pc_TargetMsg =
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
      std::vector<C_OSCCanMessageIdentificationIndices> c_MessageIds;
      std::vector<uint32> c_SignalIndices;
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
               const sint32 s32_InternalSignalIndex = pc_Parent->indexOfChild(pc_Child);
               const sint32 s32_MessageIndex = this->indexOfTopLevelItem(pc_Parent);
               if ((s32_InternalSignalIndex >= 0) && (s32_MessageIndex >= 0))
               {
                  const uint32 u32_MessageIndex = static_cast<uint32>(s32_MessageIndex);
                  if (u32_MessageIndex < this->mc_UniqueMessageIds.size())
                  {
                     const uint32 u32_InternalSignalIndex = static_cast<uint32>(s32_InternalSignalIndex);
                     uint32 u32_SignalDataIndex;
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
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < this->mc_UniqueMessageIds.size(); ++u32_ItMessage)
   {
      uint32 u32_InternalIndex;
      if (m_MapMessageIdToInternalMessageIndex(this->mc_UniqueMessageIds[u32_ItMessage], u32_InternalIndex) == C_NO_ERR)
      {
         QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(u32_InternalIndex);
         if (pc_TopLevelItem != NULL)
         {
            //Message
            pc_TopLevelItem->setData(0, msn_USER_ROLE_TOOL_TIP_HEADING, pc_TopLevelItem->text(0));
            pc_TopLevelItem->setData(0, msn_USER_ROLE_TOOL_TIP_CONTENT,
                                     C_SdUtil::h_GetToolTipContentMessage(this->mc_UniqueMessageIds[u32_ItMessage]));
            //Signals
            for (sint32 s32_ItSignalInternal = 0; s32_ItSignalInternal < pc_TopLevelItem->childCount();
                 ++s32_ItSignalInternal)
            {
               QTreeWidgetItem * const pc_SignalItem = pc_TopLevelItem->child(s32_ItSignalInternal);
               uint32 u32_SignalDataIndex;
               if (this->m_MapSignalInternalIndexToDataIndex(u32_InternalIndex, s32_ItSignalInternal,
                                                             u32_SignalDataIndex) == C_NO_ERR)
               {
                  //Signal
                  pc_SignalItem->setData(0, msn_USER_ROLE_TOOL_TIP_HEADING, pc_SignalItem->text(0));
                  pc_SignalItem->setData(0, msn_USER_ROLE_TOOL_TIP_CONTENT,
                                         C_SdUtil::h_GetToolTipContentSignal(this->mc_UniqueMessageIds[u32_ItMessage],
                                                                             u32_SignalDataIndex));
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
      for (uint32 u32_ItMessageId = 0; u32_ItMessageId < this->mc_UniqueMessageIds.size(); ++u32_ItMessageId)
      {
         this->m_InsertMessage(u32_ItMessageId);
      }
      RecheckErrorGlobal(false);
      if (orq_HandleSelection == true)
      {
         m_RestoreSelection();
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
void C_SdBueMessageSelectorTreeWidget::m_InsertMessage(const uint32 & oru32_MessageIdIndex)
{
   if (oru32_MessageIdIndex < this->mc_UniqueMessageIds.size())
   {
      const C_OSCCanMessageIdentificationIndices & rc_MessageId = this->mc_UniqueMessageIds[oru32_MessageIdIndex];
      const C_OSCCanMessage * const pc_MessageData = C_PuiSdHandler::h_GetInstance()->GetCanMessage(rc_MessageId);
      if (pc_MessageData != NULL)
      {
         QTreeWidgetItem * const pc_Message = new C_SdBusMessageSelectorTreeWidgetItem(true);

         uint32 u32_Counter;
         uint32 u32_SignalDataIndex;
         const QString c_Text = QString("%1 (0x%2)").arg(pc_MessageData->c_Name.c_str()).arg(
            QString::number(pc_MessageData->u32_CanId, 16).toUpper());

         this->insertTopLevelItem(oru32_MessageIdIndex, pc_Message);

         pc_Message->setText(0, c_Text);
         pc_Message->setForeground(0, mc_STYLE_GUIDE_COLOR_8);

         // Signals
         for (u32_Counter = 0; u32_Counter < pc_MessageData->c_Signals.size(); ++u32_Counter)
         {
            if (m_MapSignalInternalIndexToDataIndex(oru32_MessageIdIndex, u32_Counter, u32_SignalDataIndex) == C_NO_ERR)
            {
               this->m_InsertSignal(pc_Message, u32_Counter,
                                    C_PuiSdHandler::h_GetInstance()->GetCanSignalDisplayName(rc_MessageId,
                                                                                             u32_SignalDataIndex));
            }
         }
         //lint -e593 Qt parent handling will take care of it
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_InsertSignal(QTreeWidgetItem * const pc_MessageItem,
                                                      const uint32 u32_SignalIndex,
                                                      const QString & orc_SignalName) const
{
   QTreeWidgetItem * const pc_Signal = new C_SdBusMessageSelectorTreeWidgetItem(false);

   pc_Signal->setText(0, orc_SignalName);
   pc_Signal->setForeground(0, mc_STYLE_GUIDE_COLOR_8);

   pc_MessageItem->insertChild(u32_SignalIndex, pc_Signal);

   //lint -e{429}  no memory leak because of the parent of pc_Signal by insertChild and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_SelectionChanged(const QItemSelection & orc_Selected,
                                                          const QItemSelection & orc_Deselected)
{
   Q_UNUSED(orc_Selected)
   Q_UNUSED(orc_Deselected)

   sintn sn_CounterTopLevelItem;
   C_SdBusMessageSelectorTreeWidgetItem * pc_TopLevelItem;
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
      for (sn_CounterTopLevelItem = 0; sn_CounterTopLevelItem < this->topLevelItemCount(); ++sn_CounterTopLevelItem)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_TopLevelItem =
            dynamic_cast<C_SdBusMessageSelectorTreeWidgetItem *>(this->topLevelItem(sn_CounterTopLevelItem));
         if (pc_TopLevelItem != NULL)
         {
            if (pc_Parent == pc_TopLevelItem)
            {
               const sint32 s32_SignalInternalIndex = pc_Parent->indexOfChild(pc_Current);
               if (s32_SignalInternalIndex >= 0)
               {
                  const uint32 u32_ItMessageId = static_cast<uint32>(sn_CounterTopLevelItem);
                  const uint32 u32_SignalInternalIndex = s32_SignalInternalIndex;
                  uint32 u32_SignalDataIndex;
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
         const sint32 s32_ItMessageId = this->indexOfTopLevelItem(pc_Current);
         if (s32_ItMessageId >= 0)
         {
            const uint32 u32_ItMessageId = static_cast<uint32>(s32_ItMessageId);
            if (u32_ItMessageId < this->mc_UniqueMessageIds.size())
            {
               m_SendMessageSelectionUpdate(this->mc_UniqueMessageIds[u32_ItMessageId]);
            }
         }
      }
      for (sn_CounterTopLevelItem = 0; sn_CounterTopLevelItem < this->topLevelItemCount(); ++sn_CounterTopLevelItem)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_TopLevelItem =
            dynamic_cast<C_SdBusMessageSelectorTreeWidgetItem *>(this->topLevelItem(sn_CounterTopLevelItem));
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
void C_SdBueMessageSelectorTreeWidget::m_DeselectChildren(const QTreeWidgetItem * const opc_Parent) const
{
   sintn sn_CounterChild;

   // deselect all children of the parent
   for (sn_CounterChild = 0; sn_CounterChild < opc_Parent->childCount(); ++sn_CounterChild)
   {
      QTreeWidgetItem * const pc_ChildItem = opc_Parent->child(sn_CounterChild);
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
void C_SdBueMessageSelectorTreeWidget::m_ScrollBarRangeChanged(const sintn osn_Min, const sintn osn_Max) const
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
sint32 C_SdBueMessageSelectorTreeWidget::m_GetFirstConnectedNodeAndInterface(uint32 & oru32_NodeIndex,
                                                                             uint32 & oru32_InterfaceIndex,
                                                                             uint32 & oru32_DatapoolIndex) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mq_ModeSingleNode == true)
   {
      tgl_assert(this->mc_DatapoolIndexes.size() > 0);

      oru32_NodeIndex = this->mu32_NodeIndex;
      oru32_InterfaceIndex = this->mu32_InterfaceIndex;
      oru32_DatapoolIndex = this->mc_DatapoolIndexes[0];
   }
   else
   {
      std::vector<uint32> c_NodeIndexes;
      std::vector<uint32> c_InterfaceIndexes;
      std::vector<uint32> c_DatapoolIndexes;

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
sint32 C_SdBueMessageSelectorTreeWidget::m_MapMessageIdToInternalMessageIndex(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, uint32 & oru32_InternalMessageIndex) const
{
   sint32 s32_Retval = C_RANGE;

   if (this->mpc_MessageSyncManager != NULL)
   {
      const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingIds =
         this->mpc_MessageSyncManager->GetMatchingMessageVector(orc_MessageId);

      for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingIds.size(); ++u32_ItMessageId)
      {
         for (uint32 u32_ItInternalMessage = 0; u32_ItInternalMessage < this->mc_UniqueMessageIds.size();
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
sint32 C_SdBueMessageSelectorTreeWidget::GetMessageIdForAdd(C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   uint32 u32_NodeIndex;
   uint32 u32_InterfaceIndex;
   uint32 u32_DatapoolIndex;
   sint32 s32_Retval = C_NO_ERR;

   if (this->m_GetFirstConnectedNodeAndInterface(u32_NodeIndex, u32_InterfaceIndex, u32_DatapoolIndex) == C_NO_ERR)
   {
      const C_OSCCanMessageContainer * const pc_MessageContainer =
         C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(u32_NodeIndex, this->me_ProtocolType,
                                                                         u32_InterfaceIndex, u32_DatapoolIndex);
      if (pc_MessageContainer != NULL)
      {
         orc_MessageId = C_OSCCanMessageIdentificationIndices(u32_NodeIndex,
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
      for (uint32 u32_ItMessage = 0; u32_ItMessage < this->mc_UniqueMessageIds.size(); ++u32_ItMessage)
      {
         m_UpdateUniqueMessageIdsSignals(u32_ItMessage);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal message id signals vector

   \param[in]  oru32_InternalMessageIndex    Internal message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_UpdateUniqueMessageIdsSignals(const uint32 & oru32_InternalMessageIndex)
{
   if (oru32_InternalMessageIndex < this->mc_UniqueMessageIds.size())
   {
      const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
         this->mc_UniqueMessageIds[oru32_InternalMessageIndex]);
      if ((pc_Message != NULL) && (oru32_InternalMessageIndex < this->mc_UniqueMessageIdsSignalsOrder.size()))
      {
         std::vector<uint32> & orc_CurrentSignals = this->mc_UniqueMessageIdsSignalsOrder[oru32_InternalMessageIndex];
         std::vector<uint32>::iterator c_Begin;
         orc_CurrentSignals.clear();
         for (uint32 u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
         {
            orc_CurrentSignals.push_back(u32_ItSignal);
         }
         //Sort
         c_Begin = orc_CurrentSignals.begin();
         std::sort(c_Begin, orc_CurrentSignals.end(),
                   C_SdBueSortHelperSignal(this->mc_UniqueMessageIds[oru32_InternalMessageIndex]));
      }
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
   const C_OSCCanMessageIdentificationIndices & orc_MessageId)
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
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const uint32 & oru32_SignalIndex)
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

   std::vector<uint32> c_Signals;

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
               const uint32 u32_Message = c_ItSelection->parent().row();
               if (u32_Message < this->mc_UniqueMessageIds.size())
               {
                  const sint32 s32_SignalInternalIndex = c_ItSelection->row();
                  if (s32_SignalInternalIndex >= 0)
                  {
                     const uint32 u32_SignalInternalIndex = s32_SignalInternalIndex;
                     uint32 u32_DataIndex;
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
               const uint32 u32_Message = c_ItSelection->row();
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

   \param[in]  oq_AlsoSetCurrent    Flag to set current
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_RestoreSelection(const bool oq_AlsoSetCurrent)
{
   if (oq_AlsoSetCurrent)
   {
      //Wait for all events -> otherwise widget size is invalid so scrolling does not work properly
      QApplication::processEvents();
   }
   this->clearSelection();
   tgl_assert(this->mc_SelectedMessageIds.size() == this->mc_SelectedSignals.size());
   if (this->mc_SelectedMessageIds.size() == this->mc_SelectedSignals.size())
   {
      uint32 u32_InternalIndex;
      for (uint32 u32_ItPrevSelection = 0; u32_ItPrevSelection < this->mc_SelectedMessageIds.size();
           ++u32_ItPrevSelection)
      {
         const C_OSCCanMessageIdentificationIndices & rc_Message = this->mc_SelectedMessageIds[u32_ItPrevSelection];

         //Check if still in tree
         if (m_MapMessageIdToInternalMessageIndex(rc_Message, u32_InternalIndex) == C_NO_ERR)
         {
            if (u32_InternalIndex < this->mc_UniqueMessageIds.size())
            {
               QTreeWidgetItem * const pc_TopLevelItem = this->topLevelItem(u32_InternalIndex);
               if (pc_TopLevelItem != NULL)
               {
                  const std::vector<stw_types::uint32> & rc_Signals = this->mc_SelectedSignals[u32_ItPrevSelection];

                  if (rc_Signals.size() == 0)
                  {
                     //Message
                     pc_TopLevelItem->setSelected(true);
                     if (oq_AlsoSetCurrent == true)
                     {
                        this->scrollToItem(pc_TopLevelItem);
                     }
                  }
                  else
                  {
                     //Always expand for signal selection
                     this->expandItem(pc_TopLevelItem);

                     //Signal(s)
                     for (uint32 u32_ItSignal = 0; u32_ItSignal < rc_Signals.size(); ++u32_ItSignal)
                     {
                        uint32 u32_SignalInternalIndex;
                        if (this->m_MapSignalDataIndexToInternalIndex(u32_InternalIndex, rc_Signals[u32_ItSignal],
                                                                      u32_SignalInternalIndex) == C_NO_ERR)
                        {
                           QTreeWidgetItem * const pc_ChildItem = pc_TopLevelItem->child(u32_SignalInternalIndex);
                           if (pc_ChildItem != NULL)
                           {
                              pc_ChildItem->setSelected(true);
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
   m_ReconnectSelection();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save current expanded items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidget::m_SaveExpand(void)
{
   this->mc_ExpandedMessageIds.clear();
   for (sint32 s32_ItTopLevel = 0; s32_ItTopLevel < this->topLevelItemCount(); ++s32_ItTopLevel)
   {
      QTreeWidgetItem * const pc_Item = this->topLevelItem(s32_ItTopLevel);
      if (pc_Item != NULL)
      {
         if (pc_Item->isExpanded() == true)
         {
            const uint32 u32_Message = static_cast<uint32>(s32_ItTopLevel);
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
   uint32 u32_InternalIndex;
   for (uint32 u32_ItPrevExpansion = 0; u32_ItPrevExpansion < this->mc_ExpandedMessageIds.size();
        ++u32_ItPrevExpansion)
   {
      const C_OSCCanMessageIdentificationIndices & rc_Message = this->mc_ExpandedMessageIds[u32_ItPrevExpansion];

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
sint32 C_SdBueMessageSelectorTreeWidget::m_MapSignalInternalIndexToDataIndex(const uint32 & oru32_InternalMessageIndex,
                                                                             const uint32 & oru32_InternalSignalIndex,
                                                                             uint32 & oru32_SignalDataIndex) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (oru32_InternalMessageIndex < this->mc_UniqueMessageIdsSignalsOrder.size())
   {
      const std::vector<uint32> & rc_Signals = this->mc_UniqueMessageIdsSignalsOrder[oru32_InternalMessageIndex];
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
sint32 C_SdBueMessageSelectorTreeWidget::m_MapSignalDataIndexToInternalIndex(const uint32 & oru32_InternalMessageIndex,
                                                                             const uint32 & oru32_DataSignalIndex,
                                                                             uint32 & oru32_SignalInternalIndex) const
{
   sint32 s32_Retval = C_RANGE;

   if (oru32_InternalMessageIndex < this->mc_UniqueMessageIdsSignalsOrder.size())
   {
      const std::vector<uint32> & rc_Signals = this->mc_UniqueMessageIdsSignalsOrder[oru32_InternalMessageIndex];
      for (uint32 u32_ItSignal = 0; u32_ItSignal < rc_Signals.size(); ++u32_ItSignal)
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
uint16 C_SdBueMessageSelectorTreeWidget::mh_GetStartBit(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);

   return C_SdBueMessageSelectorTreeWidget::mh_GetStartBit(pc_Message, mu16_BUS_DEFAULT_SIGNAL_LENGTH,
                                                           stw_opensyde_core::C_OSCCanSignal::eBYTE_ORDER_INTEL);
}

//----------------------------------------------------------------------------------------------------------------------
uint16 C_SdBueMessageSelectorTreeWidget::mh_GetStartBit(const C_OSCCanMessage * const opc_Message,
                                                        const uint16 ou16_SignalSize,
                                                        const C_OSCCanSignal::E_ByteOrderType oe_Type)
{
   uint16 u16_StartBit = 0U;

   if (opc_Message != NULL)
   {
      std::set<uint16> c_SetUsedBits;
      uint32 u32_SignalCounter;
      uint16 u16_Counter;
      uint16 u16_BitCounter;
      uint16 u16_GapSizeCounter = 0U;
      sint16 s16_FirstUnusedBit = -1;
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
         if (oe_Type == C_OSCCanSignal::eBYTE_ORDER_MOTOROLA)
         {
            // check the bits from "left" when it is motorola format
            u16_BitCounter = static_cast<uint16>((7U - (static_cast<uint32>(u16_Counter) % 8U)) +
                                                 ((static_cast<uint32>(u16_Counter) / 8U) * 8U));
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
               s16_FirstUnusedBit = static_cast<sint16>(u16_BitCounter);
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
         u16_StartBit = static_cast<uint16>(s16_FirstUnusedBit);
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
      sint32 s32_CurrentRow = -1;
      bool q_MessageFound = false;
      bool q_SignalFound = false;
      q_Retval = true;
      for (sint32 s32_ItSelectedItem = 0; s32_ItSelectedItem < c_Selection.count(); ++s32_ItSelectedItem)
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
            for (sint32 s32_ItSelectedItem = 0; s32_ItSelectedItem < c_Selection.count(); ++s32_ItSelectedItem)
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
            for (sint32 s32_ItSelectedItem = 0; s32_ItSelectedItem < c_Selection.count(); ++s32_ItSelectedItem)
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
