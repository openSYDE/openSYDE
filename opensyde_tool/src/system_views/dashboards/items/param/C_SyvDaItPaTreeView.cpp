//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter tree view (implementation)

   Parameter tree view

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>
#include <QHeaderView>
#include "stwtypes.h"
#include "C_GtGetText.h"
#include "C_GiSvDaParam.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvDaItPaArWidget.h"
#include "C_SyvDaItPaTreeView.h"
#include "C_SyvDaItTableHeaderView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaTreeView::C_SyvDaItPaTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent),
   mpc_DataWidget(NULL),
   mq_IgnoreChanges(false)
{
   //Header
   QHeaderView * const pc_Tmp = new C_SyvDaItTableHeaderView(Qt::Horizontal, this);

   this->setHeader(pc_Tmp);
   //Make sure to install event filter to have handling for mouse move events
   pc_Tmp->installEventFilter(this);

   this->setModel(&this->mc_Model);
   this->setItemDelegate(&mc_Delegate);

   //Configure
   this->header()->setSectionsMovable(true);
   this->header()->setFixedHeight(27);
   this->setAlternatingRowColors(true);
   this->header()->setMouseTracking(true);
   this->header()->setStretchLastSection(false);
   this->setSelectionMode(QAbstractItemView::NoSelection);
   this->setEditTriggers(
      QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked |
      QAbstractItemView::EditKeyPressed);

   // track hover events for different cursors
   this->setAttribute(Qt::WA_Hover);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->horizontalScrollBar()->hide();
   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   connect(&this->mc_Delegate, &C_SyvDaItPaTreeDelegate::SigLinkClicked, this,
           &C_SyvDaItPaTreeView::m_HandleLinkClicked);

   //Register changes
   connect(this->header(), &QHeaderView::sectionMoved, this, &C_SyvDaItPaTreeView::m_HandleChange);
   connect(this->header(), &QHeaderView::sectionResized, this, &C_SyvDaItPaTreeView::m_HandleChange);
   connect(this, &C_SyvDaItPaTreeView::expanded, this, &C_SyvDaItPaTreeView::m_HandleChange);
   connect(this, &C_SyvDaItPaTreeView::collapsed, this, &C_SyvDaItPaTreeView::m_HandleChange);
   //Edit signals
   connect(&this->mc_Delegate, &C_SyvDaItPaTreeDelegate::SigActionRead, this, &C_SyvDaItPaTreeView::m_HandleActionRead);
   connect(&this->mc_Delegate, &C_SyvDaItPaTreeDelegate::SigActionWrite, this,
           &C_SyvDaItPaTreeView::m_HandleActionWrite);
   connect(&this->mc_Delegate, &C_SyvDaItPaTreeDelegate::SigActionApply, this,
           &C_SyvDaItPaTreeView::m_HandleActionApply);
   connect(&this->mc_Delegate, &C_SyvDaItPaTreeDelegate::SigActionLoad, this,
           &C_SyvDaItPaTreeView::m_HandleActionLoad);
   connect(&this->mc_Delegate, &C_SyvDaItPaTreeDelegate::SigActionSave, this,
           &C_SyvDaItPaTreeView::m_HandleActionSave);
   connect(&this->mc_Delegate, &C_SyvDaItPaTreeDelegate::SigActionRecord, this,
           &C_SyvDaItPaTreeView::m_HandleActionRecord);
   connect(&this->mc_Delegate, &C_SyvDaItPaTreeDelegate::SigActionRemove, this,
           &C_SyvDaItPaTreeView::m_HandleActionRemove);
   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SyvDaItPaTreeView::m_ScrollBarRangeChangedVer);
   connect(this->horizontalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SyvDaItPaTreeView::m_ScrollBarRangeChangedHor);
   //lint -e429 Qt parent handling will take care of it
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} Never took ownership
C_SyvDaItPaTreeView::~C_SyvDaItPaTreeView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if tree empty

   \return
   True  Empty
   False Not empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeView::IsEmpty(void) const
{
   return this->mc_Model.IsEmpty();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all ECU values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::ClearECUValues(void)
{
   this->mc_Model.ClearECUValues();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reload set data column
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::ReloadSetValues(void)
{
   this->mc_Model.ReloadSetValues();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check all Set values in range

   \param[in]  orc_ListIds    Subset of lists to check range for
   \param[in]  orc_ListIds2   Additional subset of lists to check range for

   \return
   True  In range
   False Not in range
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeView::CheckRange(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds,
                                     const std::vector<C_OSCNodeDataPoolListId> & orc_ListIds2) const
{
   return this->mc_Model.CheckRange(orc_ListIds, orc_ListIds2);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check all lists have read values

   \return
   True  Read
   False Not read
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeView::CheckAllListsRead(void) const
{
   return this->mc_Model.CheckAllListsRead();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle value preparation

   \param[in]  orc_ListIds    List IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::PrepareChangedValues(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds) const
{
   this->mc_Model.PrepareChangedValues(orc_ListIds);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the NVM changed flag for all relevant elements

   \param[in]  orc_ListIds    List IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::RemoveValuesChangedFlag(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds) const
{
   this->mc_Model.RemoveValuesChangedFlag(orc_ListIds);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark flag value

   \param[in]  oq_Value    New dark flag value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::SetDark(const bool oq_Value)
{
   this->mc_Model.SetDark(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set edit mode status

   \param[in]  oq_EditMode    Edit mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::SetEditMode(const bool oq_EditMode)
{
   this->mc_Model.SetEditMode(oq_EditMode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connection status

   \param[in]  oq_Connected   Connection active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::SetConnected(const bool oq_Connected)
{
   this->mc_Model.SetConnected(oq_Connected);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set action status

   \param[in]  oq_Active   Action active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::SetActionActive(const bool oq_Active)
{
   this->mc_Model.SetActionActive(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get template with the type description for the current selection

   \return
   Template with the type description for the current selection
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaTreeView::GetSelectedItemTypeTemplate(void) const
{
   QString c_Retval;
   const QModelIndexList c_IndexList = this->selectedIndexes();

   if (0L < c_IndexList.size())
   {
      c_Retval = C_SyvDaItPaTreeModel::h_GetSelectedItemTypeTemplate(c_IndexList[0L]);
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of selected items

   \return
   Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvDaItPaTreeView::GetSelectedItemCount(void) const
{
   return this->selectedIndexes().size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize

   \param[in]  opc_DataWidget    Data storage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::Init(stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget)
{
   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   this->mpc_DataWidget = opc_DataWidget;
   if (opc_DataWidget != NULL)
   {
      this->mc_Model.Init(opc_DataWidget);
   }
   //Initially set column widths to last stored value
   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
      if (pc_ParamData != NULL)
      {
         this->SetAllExpandedTreeItems(pc_ParamData->c_ExpandedItems);
         if (pc_ParamData->c_ColWidth.size() > 0UL)
         {
            this->SetColumnWidth(pc_ParamData->c_ColWidth[0]);
         }
      }
      else
      {
         //Initialize with default values
         const std::vector<stw_types::sint32> c_Empty;
         this->SetColumnWidth(c_Empty);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update ECU values for specified ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::UpdateECUValues(void)
{
   this->mc_Model.UpdateECUValues();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current column widths

   \return
   Current column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_SyvDaItPaTreeView::GetCurrentColumnWidths(void) const
{
   std::vector<sint32> c_Retval;
   c_Retval.reserve(this->mc_Model.columnCount());
   for (sint32 s32_ItCol = 0L; s32_ItCol < this->mc_Model.columnCount(); ++s32_ItCol)
   {
      c_Retval.push_back(this->columnWidth(s32_ItCol));
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current column position indices

   \return
   Current column position indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_SyvDaItPaTreeView::GetCurrentColumnPositionIndices(void) const
{
   std::vector<sint32> c_Retval;
   c_Retval.reserve(this->mc_Model.columnCount());
   for (sint32 s32_ItCol = 0L; s32_ItCol < this->mc_Model.columnCount(); ++s32_ItCol)
   {
      c_Retval.push_back(this->header()->visualIndex(s32_ItCol));
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete the specified list IDs

   \param[in]  orc_ListIds    List IDs to delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::DeleteSpecified(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds)
{
   this->mc_Model.DeleteSpecified(orc_ListIds);
   //Reload
   this->Init(this->mpc_DataWidget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set column position indices

   \param[in]  orc_NewColPositionIndices  New column position indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::SetColumnPositionIndices(const std::vector<sint32> & orc_NewColPositionIndices)
{
   this->mq_IgnoreChanges = true;
   if (orc_NewColPositionIndices.size() == static_cast<uint32>(this->mc_Model.columnCount()))
   {
      //Use new spec as sorting specification
      while (this->m_ColumnsSortedAsExpected(orc_NewColPositionIndices) == false)
      {
         //If not sorted
         for (sint32 s32_ItCol = 0L; s32_ItCol < this->mc_Model.columnCount(); ++s32_ItCol)
         {
            //Find first invalid element
            if (this->header()->visualIndex(s32_ItCol) != orc_NewColPositionIndices[static_cast<uint32>(s32_ItCol)])
            {
               //Find element which should be at this position
               for (sint32 s32_ItCol2 = 0L; s32_ItCol2 < this->mc_Model.columnCount(); ++s32_ItCol2)
               {
                  if (this->header()->visualIndex(s32_ItCol2) ==
                      orc_NewColPositionIndices[static_cast<uint32>(s32_ItCol)])
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
   this->mq_IgnoreChanges = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new column widths

   \param[in]  orc_NewColWidths  New column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::SetColumnWidth(const std::vector<sint32> & orc_NewColWidths)
{
   std::vector<sint32> c_Copy = orc_NewColWidths;

   this->mq_IgnoreChanges = true;

   if (c_Copy.size() != static_cast<uint32>(this->mc_Model.columnCount()))
   {
      c_Copy.clear();
      //Init cols
      //Tree
      c_Copy.push_back(250);
      //Comment
      c_Copy.push_back(147);
      //Device value
      c_Copy.push_back(117);
      //Set
      c_Copy.push_back(110);
      //Unit
      c_Copy.push_back(40);
      //Read
      c_Copy.push_back(40);
      //Write
      c_Copy.push_back(40);
      //Apply
      c_Copy.push_back(40);
      //Load
      c_Copy.push_back(40);
      //Save
      c_Copy.push_back(40);
      //Record
      c_Copy.push_back(40);
      //Remove
      c_Copy.push_back(40);
   }
   for (uint32 u32_ItCol = 0UL;
        (u32_ItCol < c_Copy.size()) && (static_cast<sintn>(u32_ItCol) < this->mc_Model.columnCount());
        ++u32_ItCol)
   {
      this->setColumnWidth(static_cast<sintn>(u32_ItCol), c_Copy[u32_ItCol]);
   }
   this->mq_IgnoreChanges = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if all specified lists are read

   \param[in]  orc_ListIds    List IDs to check

   \return
   True  Read
   False Not read
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeView::CheckListsRead(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds) const
{
   return this->mc_Model.CheckListsRead(orc_ListIds);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all expanded tree items

   \return
   All expanded tree items
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_PuiSvDbExpandedTreeIndex> C_SyvDaItPaTreeView::GetAllExpandedTreeItems(void) const
{
   std::vector<C_PuiSvDbExpandedTreeIndex> c_Retval;
   const QModelIndexList c_Indixes = this->mc_Model.GetAllAvailableIndixesForOneColumn();
   for (QModelIndexList::const_iterator c_ItList = c_Indixes.begin(); c_ItList != c_Indixes.end(); ++c_ItList)
   {
      if (this->isExpanded(*c_ItList) == true)
      {
         C_PuiSvDbExpandedTreeIndex c_Elm;
         C_SyvDaItPaTreeModel::h_DecodeIndex(*c_ItList, c_Elm.c_ExpandedId, c_Elm.u32_Layer);
         c_Retval.push_back(c_Elm);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set all items as expanded

   \param[in]  orc_Items   Items to expand
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::SetAllExpandedTreeItems(const std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items)
{
   this->mq_IgnoreChanges = true;
   this->collapseAll();
   for (uint32 u32_ItItem = 0UL; u32_ItItem < orc_Items.size(); ++u32_ItItem)
   {
      const C_PuiSvDbExpandedTreeIndex & rc_Item = orc_Items[u32_ItItem];
      const QModelIndex c_Index = this->mc_Model.GetIndexForItem(rc_Item.c_ExpandedId, rc_Item.u32_Layer);
      if (c_Index.isValid() == true)
      {
         this->setExpanded(c_Index, true);
      }
   }
   this->mq_IgnoreChanges = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all list IDs

   \return
   All list IDs
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> C_SyvDaItPaTreeView::GetAllListIds(void) const
{
   return this->mc_Model.GetAllListIds();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all changed list IDs

   \return
   All list IDs
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> C_SyvDaItPaTreeView::GetChangedListElementIds(void) const
{
   return this->mc_Model.GetChangedListElementIds();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all invalid list IDs

   \return
   All invalid list IDs
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCNodeDataPoolListId> C_SyvDaItPaTreeView::GetInvalidListIds() const
{
   return this->mc_Model.GetInvalidListIds();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update CRC status

   \param[in]  orc_ListId  LIST ID
   \param[in]  oq_Status   New CRC status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::SetCRCStatus(const C_OSCNodeDataPoolListId & orc_ListId, const bool oq_Status)
{
   this->mc_Model.SetCRCStatus(orc_ListId, oq_Status);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get set values for specified list

   \param[in]   orc_ListId       List ID
   \param[out]  orc_ListValues   Set values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::GetListSetValues(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_ListId,
                                           std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> & orc_ListValues)
const
{
   this->mc_Model.GetListSetValues(orc_ListId, orc_ListValues);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle escape manually

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (opc_Event->key() == static_cast<sintn>(Qt::Key_Escape))
   {
      QWidget * const pc_Editor = this->mc_Delegate.GetEditor();
      if (pc_Editor != NULL)
      {
         this->closeEditor(pc_Editor, QAbstractItemDelegate::NoHint);
         opc_Event->accept();
      }
      else
      {
         C_OgeTreeViewToolTipBase::keyPressEvent(opc_Event);
      }
   }
   C_OgeTreeViewToolTipBase::keyPressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle hover for cursor adaption on links

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeView::event(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::HoverMove)
   {
      // check if an ip address was hovered

      QHoverEvent * const pc_HoverEvent = dynamic_cast<QHoverEvent * const>(opc_Event);
      bool q_IpAddressHovered = false;
      if (pc_HoverEvent != NULL)
      {
         QModelIndex c_Index;
         c_Index = this->indexAt(this->viewport()->mapFromGlobal(this->mapToGlobal(pc_HoverEvent->pos())));
         if (c_Index.isValid() == true)
         {
            const QVariant c_Data = this->mc_Model.data(c_Index, static_cast<sintn>(Qt::EditRole));
            if (c_Data.type() == QVariant::Point)
            {
               q_IpAddressHovered = true;
            }
         }
      }

      // change the cursor if a link was hovered
      if (q_IpAddressHovered == true)
      {
         this->setCursor(Qt::PointingHandCursor);
      }
      else
      {
         this->setCursor(Qt::ArrowCursor);
      }
   }

   return C_OgeTreeViewToolTipBase::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle data change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_HandleChange(void)
{
   if ((this->mpc_DataWidget != NULL) && (this->mq_IgnoreChanges == false))
   {
      this->mpc_DataWidget->UpdateData();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if columns in expected sorting order

   \param[in]  orc_NewColPositionIndices  Expected sorting order

   \return
   True  Sorted or error
   False Unsorted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeView::m_ColumnsSortedAsExpected(const std::vector<sint32> & orc_NewColPositionIndices) const
{
   bool q_Retval = true;

   if (orc_NewColPositionIndices.size() == static_cast<uint32>(this->mc_Model.columnCount()))
   {
      for (sint32 s32_ItCol = 0L; s32_ItCol < this->mc_Model.columnCount(); ++s32_ItCol)
      {
         if (this->header()->visualIndex(s32_ItCol) != orc_NewColPositionIndices[static_cast<uint32>(s32_ItCol)])
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle link clicked signal

   \param[in]  orc_Index   Index to edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_HandleLinkClicked(const QModelIndex & orc_Index)
{
   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_ParamItem = pc_ParamWidget->GetParamItem();
      C_OSCNodeDataPoolListElementId c_Id;
      uint32 u32_ValidLayers;

      C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
      if ((u32_ValidLayers == 4UL) && (pc_ParamItem != NULL))
      {
         const C_PuiSvDbParam c_Copy = *pc_ParamItem;
         bool q_ECU;
         QPointer<C_OgePopUpDialog> const c_Dialog = new C_OgePopUpDialog(
            pc_ParamWidget->GetPopUpParent(), pc_ParamWidget->GetPopUpParent());
         C_SyvDaItPaArWidget * pc_ArrayEditWidget;
         const uint32 u32_ItParamIndex = this->mc_Model.GetParamIndexId(c_Id);

         const C_SyvDaItPaTreeModel::E_Columns e_Col = C_SyvDaItPaTreeModel::h_ColumnToEnum(orc_Index.column());

         if (e_Col == C_SyvDaItPaTreeModel::eDEVICE_VALUE)
         {
            q_ECU = true;
         }
         else
         {
            q_ECU = false;
         }
         pc_ArrayEditWidget = new C_SyvDaItPaArWidget(*c_Dialog, u32_ItParamIndex, this->mpc_DataWidget, q_ECU);
         Q_UNUSED(pc_ArrayEditWidget)
         //Resize
         c_Dialog->SetSize(QSize(871, 318));
         if (c_Dialog->exec() != static_cast<sintn>(QDialog::Accepted))
         {
            //Revert to last known state if not accepted
            pc_ParamWidget->SetParamItem(c_Copy);
         }

         //TODO?
         //OnErrorChangePossible();
         if (c_Dialog != NULL)
         {
            c_Dialog->HideOverlay();
         }
      }
   }
   //Force change of focus
   this->setCurrentIndex(this->mc_Model.index(0, 0));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare read action

   \param[in]  orc_Id            Current ID
   \param[in]  ou32_ValidLayers  Number of valid Layers of ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_HandleActionRead(const C_OSCNodeDataPoolListElementId & orc_Id,
                                             const uint32 ou32_ValidLayers)
{
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_ListIds = this->mc_Model.GetListIdsForId(
      orc_Id, ou32_ValidLayers);

   //Force change of focus
   this->setCurrentIndex(this->mc_Model.index(0, 0));

   Q_EMIT this->SigActionRead(c_ListIds);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare write action

   \param[in]  orc_Id            Current ID
   \param[in]  ou32_ValidLayers  Number of valid Layers of ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_HandleActionWrite(const C_OSCNodeDataPoolListElementId & orc_Id,
                                              const uint32 ou32_ValidLayers)
{
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_ListIds = this->mc_Model.GetListIdsForId(
      orc_Id, ou32_ValidLayers);

   //Force change of focus
   this->setCurrentIndex(this->mc_Model.index(0, 0));

   Q_EMIT this->SigActionWrite(c_ListIds);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare apply action

   \param[in]  orc_Id            Current ID
   \param[in]  ou32_ValidLayers  Number of valid Layers of ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_HandleActionApply(const C_OSCNodeDataPoolListElementId & orc_Id,
                                              const uint32 ou32_ValidLayers)
{
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_ListIds = this->mc_Model.GetListIdsForId(
      orc_Id, ou32_ValidLayers);

   std::vector<C_OSCNodeDataPoolListElementId> c_InvalidValueIds;
   std::vector<QString> c_InvalidValues;
   std::vector<QString> c_NewValues;

   //Force change of focus
   this->setCurrentIndex(this->mc_Model.index(0, 0));

   this->mc_Model.ApplyEcuValues(c_ListIds, c_InvalidValueIds, c_InvalidValues, c_NewValues);

   Q_EMIT (this->SigInformUserFloatRangeCheck(c_InvalidValueIds, c_InvalidValues, c_NewValues));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare load action

   \param[in]  orc_Id            Current ID
   \param[in]  ou32_ValidLayers  Number of valid Layers of ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_HandleActionLoad(const C_OSCNodeDataPoolListElementId & orc_Id,
                                             const uint32 ou32_ValidLayers)
{
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_ElementIds =
      this->mc_Model.GetElementIdsForId(
         orc_Id, ou32_ValidLayers);

   //Force change of focus
   this->setCurrentIndex(this->mc_Model.index(0, 0));

   Q_EMIT this->SigActionLoad(c_ElementIds, orc_Id, ou32_ValidLayers);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare save action

   \param[in]  orc_Id            Current ID
   \param[in]  ou32_ValidLayers  Number of valid Layers of ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_HandleActionSave(const C_OSCNodeDataPoolListElementId & orc_Id,
                                             const uint32 ou32_ValidLayers)
{
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_ElementIds =
      this->mc_Model.GetListIdsForId(
         orc_Id, ou32_ValidLayers);

   //Force change of focus
   this->setCurrentIndex(this->mc_Model.index(0, 0));

   Q_EMIT this->SigActionSave(c_ElementIds, orc_Id, ou32_ValidLayers);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare record action

   \param[in]  orc_Id            Current ID
   \param[in]  ou32_ValidLayers  Number of valid Layers of ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_HandleActionRecord(const C_OSCNodeDataPoolListElementId & orc_Id,
                                               const uint32 ou32_ValidLayers)
{
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_ListIds = this->mc_Model.GetListIdsForId(
      orc_Id, ou32_ValidLayers);

   //Force change of focus
   this->setCurrentIndex(this->mc_Model.index(0, 0));

   Q_EMIT this->SigActionRecord(c_ListIds);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare remove action

   \param[in]  orc_Id            Current ID
   \param[in]  ou32_ValidLayers  Number of valid Layers of ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_HandleActionRemove(const C_OSCNodeDataPoolListElementId & orc_Id,
                                               const uint32 ou32_ValidLayers)
{
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_ListIds = this->mc_Model.GetListIdsForId(
      orc_Id, ou32_ValidLayers);

   //Force change of focus
   this->setCurrentIndex(this->mc_Model.index(0, 0));

   Q_EMIT this->SigActionRemove(c_ListIds);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeView::m_ScrollBarRangeChangedVer(const sintn osn_Min, const sintn osn_Max) const
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
void C_SyvDaItPaTreeView::m_ScrollBarRangeChangedHor(const sintn osn_Min, const sintn osn_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((osn_Min == 0) && (osn_Max == 0))
   {
      this->horizontalScrollBar()->hide();
   }
   else
   {
      this->horizontalScrollBar()->show();
   }
}
