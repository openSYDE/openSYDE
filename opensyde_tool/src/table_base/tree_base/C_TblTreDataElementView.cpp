//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Tree view for data elements (implementation)

   Tree view for data elements

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QKeyEvent>
#include <QScrollBar>
#include "C_TblTreDataElementView.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
QMap<C_TblTreDataElementModel::E_Mode,
     std::vector<std::vector<uint32> > > C_TblTreDataElementView::mhc_LastKnownExpandedIndices;

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

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_TblTreDataElementView::C_TblTreDataElementView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent),
   mq_UseInternalExpandedItems(true),
   mq_ShowOnlyWriteElements(false),
   mq_ShowArrayElements(false),
   mq_Show64BitValues(false),
   mu32_ViewIndex(0),
   me_Mode(C_TblTreDataElementModel::eDATAPOOL_ELEMENT)
{
   this->mc_SortModel.setSourceModel(&this->mc_Model);
   this->setModel(&this->mc_SortModel);

   this->setHeaderHidden(true);
   //Configure filter
   this->mc_SortModel.setFilterCaseSensitivity(Qt::CaseInsensitive);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     19.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_TblTreDataElementView::~C_TblTreDataElementView(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set flag to use internal expanded items

   \param[in] oq_Use Flag to use internal expanded items

   \created     01.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementView::SetUseInternalExpandedItems(const bool oq_Use)
{
   this->mq_UseInternalExpandedItems = oq_Use;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set the active node index

   \param[in] ou32_NodeIndex               Active node index
   \param[in] os32_SkipApplicationIndex    Application index to not display as used
   \param[in] orc_UsedDataPoolIndicesIndex Data pools to always display as used

   \created     01.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementView::InitSD(const uint32 ou32_NodeIndex, const sint32 os32_SkipApplicationIndex,
                                     const std::vector<uint32> & orc_UsedDataPoolIndicesIndex)
{
   this->me_Mode = C_TblTreDataElementModel::eDATAPOOLS;
   this->mc_Model.InitSD(ou32_NodeIndex, os32_SkipApplicationIndex, orc_UsedDataPoolIndicesIndex);
   //Some defaults
   this->mq_ShowOnlyWriteElements = false;
   this->mq_ShowArrayElements = false;
   this->mq_Show64BitValues = false;
   this->mu32_ViewIndex = 0UL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize tree structure

   \param[in] ou32_ViewIndex           View index
   \param[in] oq_ShowOnlyWriteElements Optional flag to show only writable elements
   \param[in] oq_ShowArrayElements     Optional flag to hide all array elements (if false)
   \param[in] oq_Show64BitValues       Optional flag to hide all 64 bit elements (if false)
   \param[in] oq_ShowNVMLists          Optional flag to only show NVM LISTs

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementView::InitSV(const stw_types::uint32 ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                                     const bool oq_ShowArrayElements, const bool oq_Show64BitValues,
                                     const bool oq_ShowNVMLists)
{
   this->mq_ShowOnlyWriteElements = oq_ShowOnlyWriteElements;
   this->mq_ShowArrayElements = oq_ShowArrayElements;
   this->mq_Show64BitValues = oq_Show64BitValues;
   this->mu32_ViewIndex = ou32_ViewIndex;
   if (oq_ShowNVMLists == true)
   {
      this->me_Mode = C_TblTreDataElementModel::eNVM_LIST;
      this->mc_Model.InitSV(this->mu32_ViewIndex, C_TblTreDataElementModel::eNVM_LIST, oq_ShowOnlyWriteElements,
                            oq_ShowArrayElements, oq_Show64BitValues);
   }
   else
   {
      this->me_Mode = C_TblTreDataElementModel::eDATAPOOL_ELEMENT;
      this->mc_Model.InitSV(this->mu32_ViewIndex, C_TblTreDataElementModel::eDATAPOOL_ELEMENT, oq_ShowOnlyWriteElements,
                            oq_ShowArrayElements, oq_Show64BitValues);
   }
   m_RestoreExpandedIndices();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Filter for string

   \param[in] orc_Text String

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementView::Search(const QString & orc_Text)
{
   this->mc_SortModel.SetFilter(orc_Text);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Switch displayed content

   \param[in] ore_Mode New mode

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementView::SwitchMode(const C_TblTreDataElementModel::E_Mode & ore_Mode)
{
   this->me_Mode = ore_Mode;
   this->mc_Model.InitSV(this->mu32_ViewIndex, ore_Mode, this->mq_ShowOnlyWriteElements, this->mq_ShowArrayElements,
                         this->mq_Show64BitValues);
   m_RestoreExpandedIndices();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get selected data elements

   \return
   Current selected data elements

   \created     06.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check if view empty

   \return
   True  Empty
   False Not empty

   \created     19.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_TblTreDataElementView::IsEmpty(void) const
{
   return this->mc_SortModel.rowCount() == 0;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle expanded index save

   \created     18.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementView::SaveExpandedIndices(void)
{
   if (this->mq_UseInternalExpandedItems == true)
   {
      const sintn sn_Column = 0;

      std::vector<std::vector<stw_types::uint32> > c_FoundItems;
      //THIS ONE HAS TO BE INVALID
      QModelIndex c_Index;
      //Handle children
      m_AppendExpandedIndices(c_FoundItems, c_Index, sn_Column);

      C_TblTreDataElementView::mhc_LastKnownExpandedIndices.remove(this->me_Mode);
      C_TblTreDataElementView::mhc_LastKnownExpandedIndices.insert(this->me_Mode, c_FoundItems);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse double click event slot

   Here: Add dialog exit if valid selection

   \param[in,out] opc_Event Event identification and information

   \created     19.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten selection changed event slot

   Here: Emit signal with new number of selected items

   \param[in,out] opc_Event Event identification and information

   \created     26.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreDataElementView::selectionChanged(const QItemSelection & orc_Selected,
                                               const QItemSelection & orc_Deselected)
{
   QTreeView::selectionChanged(orc_Selected, orc_Deselected);
   Q_EMIT this->SigSelectionChanged(this->selectedIndexes().size());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Append all expanded indices for this parent

   \param[in,out] orc_FoundItems All expanded items
   \param[in]     orc_CurParent  Current parent to analyze
   \param[in]     osn_Column     Column to use for index access

   \created     19.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Handle expanded index restoration

   \created     18.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
         this->expandAll();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Map source model index to sort model index

   Hint: This function is probably only necessary because the mapFromSource seems to not work in some cases
   -> Problematic case: if there is no parent this functions seems to return an invalid internalPointer

   \param[in] orc_Index Source model index

   \return
   Sort model index

   \created     19.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
