//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for data elements (implementation)

   Tree view for data elements

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QKeyEvent>
#include <QScrollBar>
#include "C_TblTreDataElementView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
QMap<C_TblTreDataElementModel::E_Mode,
     std::vector<std::vector<uint32> > > C_TblTreDataElementView::mhc_LastKnownExpandedIndices;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreDataElementView::C_TblTreDataElementView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent),
   mq_UseInternalExpandedItems(true),
   mu32_ViewIndex(0),
   me_Mode(C_TblTreDataElementModel::eDATAPOOL_ELEMENT)
{
   this->mc_SortModel.setSourceModel(&this->mc_Model);
   this->C_TblTreDataElementView::setModel(&this->mc_SortModel);

   this->setHeaderHidden(true);
   //Configure filter
   this->mc_SortModel.setFilterCaseSensitivity(Qt::CaseInsensitive);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreDataElementView::~C_TblTreDataElementView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set flag to use internal expanded items

   \param[in] oq_Use Flag to use internal expanded items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::SetUseInternalExpandedItems(const bool oq_Use)
{
   this->mq_UseInternalExpandedItems = oq_Use;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the active node index

   \param[in] ou32_NodeIndex               Active node index
   \param[in] os32_SkipApplicationIndex    Application index to not display as used
   \param[in] orc_UsedDataPoolIndicesIndex Data pools to always display as used
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::InitSD(const uint32 ou32_NodeIndex, const sint32 os32_SkipApplicationIndex,
                                     const std::vector<uint32> & orc_UsedDataPoolIndicesIndex)
{
   this->me_Mode = C_TblTreDataElementModel::eDATAPOOLS;
   this->mc_Model.InitSD(ou32_NodeIndex, os32_SkipApplicationIndex, orc_UsedDataPoolIndicesIndex);
   //Some defaults
   this->mu32_ViewIndex = 0UL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize tree structure

   \param[in] ou32_ViewIndex            View index
   \param[in] oq_ShowOnlyWriteElements  Optional flag to show only writable elements
   \param[in] oq_ShowArrayElements      Optional flag to hide all array elements (if false)
   \param[in] oq_ShowArrayIndexElements Optional flag to hide all array index elements (if false)
   \param[in] oq_Show64BitValues        Optional flag to hide all 64 bit elements (if false)
   \param[in] oq_ShowNVMLists           Optional flag to only show NVM LISTs
   \param[in] opc_AlreasyUsedElements   Optional pointer to vector with already used elements. All added elements
                                        will be marked as used an will be disabled. Usable for Datapool element mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::InitSV(const stw_types::uint32 ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                                     const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements,
                                     const bool oq_Show64BitValues, const bool oq_ShowNVMLists,
                                     const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
   if (oq_ShowNVMLists == true)
   {
      this->me_Mode = C_TblTreDataElementModel::eNVM_LIST;
      this->mc_Model.InitSV(this->mu32_ViewIndex, C_TblTreDataElementModel::eNVM_LIST, oq_ShowOnlyWriteElements,
                            oq_ShowArrayElements, oq_ShowArrayIndexElements, oq_Show64BitValues,
                            opc_AlreasyUsedElements);
   }
   else
   {
      this->me_Mode = C_TblTreDataElementModel::eDATAPOOL_ELEMENT;
      this->mc_Model.InitSV(this->mu32_ViewIndex, C_TblTreDataElementModel::eDATAPOOL_ELEMENT, oq_ShowOnlyWriteElements,
                            oq_ShowArrayElements, oq_ShowArrayIndexElements, oq_Show64BitValues,
                            opc_AlreasyUsedElements);
   }
   m_RestoreExpandedIndices();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Filter for string

   \param[in] orc_Text String
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::Search(const QString & orc_Text)
{
   const bool q_StartIsEmpty = this->mc_SortModel.filterRegExp().isEmpty();

   //If it was empty and will soon not be empty: remember the expanded indices before filtering anything
   if (q_StartIsEmpty)
   {
      this->SaveExpandedIndices();
   }
   this->mc_SortModel.SetFilter(orc_Text);
   //If it is empty after previously not being empty restore the last known state
   if (this->mc_SortModel.filterRegExp().isEmpty())
   {
      if (q_StartIsEmpty == false)
      {
         this->m_RestoreExpandedIndices();
      }
   }
   else
   {
      //While filtering always expand all
      this->expandAll();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Setting of view index without initialization

   \param[in] ou32_ViewIndex View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::SetViewIndex(const uint32 ou32_ViewIndex)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Switch displayed content

   \param[in] ore_Mode                  New mode
   \param[in] oq_ShowOnlyWriteElements  Optional flag to show only writable elements
   \param[in] oq_ShowArrayElements      Optional flag to hide all array elements (if false)
   \param[in] oq_ShowArrayIndexElements Show array index elements
   \param[in] oq_Show64BitValues        Optional flag to hide all 64 bit elements (if false)
   \param[in] opc_AlreasyUsedElements   Optional pointer to vector with already used elements. All added elements
                                        will be marked as used an will be disabled.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::SwitchMode(const C_TblTreDataElementModel::E_Mode & ore_Mode,
                                         const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                         const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                         const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   this->me_Mode = ore_Mode;
   this->mc_Model.InitSV(this->mu32_ViewIndex, ore_Mode, oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                         oq_ShowArrayIndexElements,
                         oq_Show64BitValues, opc_AlreasyUsedElements);
   m_RestoreExpandedIndices();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected data elements

   \return
   Current selected data elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_TblTreDataElementView::GetSelectedDataElements(void) const
{
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   const QModelIndexList c_List = this->selectedIndexes();

   for (QModelIndexList::const_iterator c_It = c_List.begin(); c_It != c_List.end(); ++c_It)
   {
      const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_SubContent = this->mc_Model.GetDataElements(this->mc_SortModel.mapToSource(
                                                                                                             *c_It));
      for (std::vector<C_PuiSvDbNodeDataPoolListElementId>::const_iterator c_ItSubContent = c_SubContent.begin();
           c_ItSubContent != c_SubContent.end(); ++c_ItSubContent)
      {
         c_Retval.push_back(*c_ItSubContent);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if view empty

   \return
   True  Empty
   False Not empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreDataElementView::IsEmpty(void) const
{
   return this->mc_SortModel.rowCount() == 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle expanded index save
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::SaveExpandedIndices(void)
{
   if (this->mq_UseInternalExpandedItems == true)
   {
      const sintn sn_Column = 0;

      std::vector<std::vector<stw_types::uint32> > c_FoundItems;
      //THIS ONE HAS TO BE INVALID
      const QModelIndex c_Index;
      //Handle children
      m_AppendExpandedIndices(c_FoundItems, c_Index, sn_Column);

      C_TblTreDataElementView::mhc_LastKnownExpandedIndices.remove(this->me_Mode);
      C_TblTreDataElementView::mhc_LastKnownExpandedIndices.insert(this->me_Mode, c_FoundItems);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cleans up the last model configuration as preparation for the entire deletion
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::CleanUpLastView(void)
{
   this->mc_Model.CleanUpLastModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse double click event slot

   Here: Add dialog exit if valid selection

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(opc_Event->globalPos()));

   QTreeView::mouseDoubleClickEvent(opc_Event);
   if ((c_Index.isValid() == true) && (this->selectedIndexes().size() > 0))
   {
      if (this->mc_Model.GetDataElements(this->mc_SortModel.mapToSource(c_Index)).size() == 1UL)
      {
         Q_EMIT this->SigAccept();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten selection changed event slot

   Here: Emit signal with new number of selected items

   \param[in] orc_Selected   Selected
   \param[in] orc_Deselected Deselected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::selectionChanged(const QItemSelection & orc_Selected,
                                               const QItemSelection & orc_Deselected)
{
   QTreeView::selectionChanged(orc_Selected, orc_Deselected);
   Q_EMIT this->SigSelectionChanged(this->selectedIndexes().size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Append all expanded indices for this parent

   \param[in,out] orc_FoundItems All expanded items
   \param[in]     orc_CurParent  Current parent to analyze
   \param[in]     osn_Column     Column to use for index access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::m_AppendExpandedIndices(std::vector<std::vector<uint32> > & orc_FoundItems,
                                                      const QModelIndex & orc_CurParent, const sintn osn_Column)
{
   for (sintn sn_ItChild = 0; sn_ItChild < this->mc_Model.rowCount(orc_CurParent); ++sn_ItChild)
   {
      const QModelIndex c_CurItem = this->mc_Model.index(sn_ItChild, osn_Column, orc_CurParent);
      m_AppendExpandedIndices(orc_FoundItems, c_CurItem, osn_Column);
      if (this->isExpanded(m_ManualMapFromSource(c_CurItem)) == true)
      {
         orc_FoundItems.push_back(this->mc_Model.GetGenericRepresentationForIndex(c_CurItem));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle expanded index restoration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::m_RestoreExpandedIndices(void)
{
   if (this->mq_UseInternalExpandedItems == true)
   {
      const QMap<stw_opensyde_gui_logic::C_TblTreDataElementModel::E_Mode,
                 std::vector<std::vector<stw_types::uint32> > >::const_iterator c_ItMap =
         C_TblTreDataElementView::mhc_LastKnownExpandedIndices.find(this->me_Mode);

      if (c_ItMap != C_TblTreDataElementView::mhc_LastKnownExpandedIndices.end())
      {
         this->collapseAll();
         QModelIndexList c_List;
         for (uint32 u32_ItExpandedItem = 0UL; u32_ItExpandedItem < c_ItMap.value().size(); ++u32_ItExpandedItem)
         {
            const QModelIndex c_ModelIndex = this->mc_Model.GetIndexForItem(c_ItMap.value()[u32_ItExpandedItem]);
            const QModelIndex c_SortModelIndex = m_ManualMapFromSource(c_ModelIndex);
            this->expand(c_SortModelIndex);
            c_List.append(c_SortModelIndex);
         }
      }
      else
      {
         this->m_ExpandInitial();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map source model index to sort model index

   Hint: This function is probably only necessary because the mapFromSource seems to not work in some cases
   -> Problematic case: if there is no parent this functions seems to return an invalid internalPointer

   \param[in] orc_Index Source model index

   \return
   Sort model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_TblTreDataElementView::m_ManualMapFromSource(const QModelIndex & orc_Index) const
{
   QModelIndex c_Retval;

   if ((orc_Index.parent().isValid() == true) || (this->mc_SortModel.filterRegExp().isEmpty() == false))
   {
      c_Retval = this->mc_SortModel.mapFromSource(orc_Index);
   }
   else
   {
      c_Retval = this->mc_SortModel.index(orc_Index.row(), orc_Index.column());
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle initial expand
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::m_ExpandInitial(void)
{
   for (sintn sn_ItChild = 0; sn_ItChild < this->mc_SortModel.rowCount(); ++sn_ItChild)
   {
      const QModelIndex c_ModelIndex = this->mc_SortModel.index(sn_ItChild, 0);
      this->expand(c_ModelIndex);
      m_ExpandAllChildren(c_ModelIndex, 0UL, 3UL);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Expand all children

   \param[in] orc_Index         Index
   \param[in] ou32_LayerCounter Layer counter
   \param[in] ou32_MaxLayer     Max layer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementView::m_ExpandAllChildren(const QModelIndex & orc_Index, const uint32 ou32_LayerCounter,
                                                  const uint32 ou32_MaxLayer)
{
   if (ou32_LayerCounter < ou32_MaxLayer)
   {
      for (sintn sn_ItChild = 0; sn_ItChild < this->mc_SortModel.rowCount(orc_Index); ++sn_ItChild)
      {
         const QModelIndex c_ModelIndex = this->mc_SortModel.index(sn_ItChild, 0, orc_Index);
         this->expand(c_ModelIndex);
         m_ExpandAllChildren(c_ModelIndex, ou32_LayerCounter + 1UL, ou32_MaxLayer);
      }
   }
}
