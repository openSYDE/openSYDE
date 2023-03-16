//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for database message selection (implementation)

   View for database message selection

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>
#include <QMouseEvent>

#include "stwtypes.hpp"
#include "C_CamMosDatabaseSelectionView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
std::vector<QString> C_CamMosDatabaseSelectionView::mhc_LastKnownExpandedTopLevelItems;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseSelectionView::C_CamMosDatabaseSelectionView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent)
{
   this->mc_SortModel.setSourceModel(&this->mc_Model);
   this->C_CamMosDatabaseSelectionView::setModel(&this->mc_SortModel);

   this->setHeaderHidden(true);
   //Configure filter
   this->mc_SortModel.setFilterCaseSensitivity(Qt::CaseInsensitive);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseSelectionView::~C_CamMosDatabaseSelectionView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize tree structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionView::Init(void)
{
   this->mc_Model.Init();

   m_RestoreExpandedIndices();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Filter for string

   \param[in] orc_Text String
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionView::Search(const QString & orc_Text)
{
   this->mc_SortModel.SetFilter(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected data elements

   array:
   0: database name
   1: message name

   \return
   Current selected data elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::array<QString, 2> > C_CamMosDatabaseSelectionView::GetSelectedDataElements(void) const
{
   std::vector< std::array<QString, 2> > c_Retval;
   const QModelIndexList c_List = this->selectedIndexes();

   for (QModelIndexList::const_iterator c_It = c_List.begin(); c_It != c_List.end(); ++c_It)
   {
      const std::vector<std::array<QString, 2> > c_SubContent = this->mc_Model.GetDataElements(this->mc_SortModel.mapToSource(
                                                                                                  *c_It));
      for (std::vector<std::array<QString, 2> >::const_iterator c_ItSubContent = c_SubContent.begin();
           c_ItSubContent != c_SubContent.end(); ++c_ItSubContent)
      {
         c_Retval.push_back(*c_ItSubContent);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if view empty

   \return
   True  Empty
   False Not empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMosDatabaseSelectionView::IsEmpty(void) const
{
   return this->mc_SortModel.rowCount() == 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle expanded index save
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionView::SaveExpandedIndices(void)
{
   std::vector<QString> c_FoundItems;

   m_AppendExpandedIndices(c_FoundItems);

   //Save for next time
   C_CamMosDatabaseSelectionView::mhc_LastKnownExpandedTopLevelItems = c_FoundItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   Here: Add dialog exit if valid selection

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionView::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
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
/*! \brief   Overwritten selection changed event slot

   Here: Emit signal with new number of selected items

   \param[in]  orc_Selected      new selected items
   \param[in]  orc_Deselected    previous selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionView::selectionChanged(const QItemSelection & orc_Selected,
                                                     const QItemSelection & orc_Deselected)
{
   QTreeView::selectionChanged(orc_Selected, orc_Deselected);
   Q_EMIT this->SigSelectionChanged(this->selectedIndexes().size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append all expanded indices

   \param[in,out] orc_FoundItems All expanded items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionView::m_AppendExpandedIndices(std::vector<QString> & orc_FoundItems)
{
   orc_FoundItems.clear();
   for (int32_t s32_ItChild = 0; s32_ItChild < this->mc_Model.rowCount(); ++s32_ItChild)
   {
      const QModelIndex c_CurItem = this->mc_Model.index(s32_ItChild, 0);
      if (this->isExpanded(m_ManualMapFromSource(c_CurItem)) == true)
      {
         orc_FoundItems.push_back(this->mc_Model.data(c_CurItem).toString());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle expanded index restoration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionView::m_RestoreExpandedIndices(void)
{
   if (C_CamMosDatabaseSelectionView::mhc_LastKnownExpandedTopLevelItems.size() > 0UL)
   {
      this->collapseAll();
      QModelIndexList c_List;
      for (uint32_t u32_ItExpandedItem = 0UL;
           u32_ItExpandedItem < C_CamMosDatabaseSelectionView::mhc_LastKnownExpandedTopLevelItems.size();
           ++u32_ItExpandedItem)
      {
         const QModelIndex c_ModelIndex = this->mc_Model.GetIndexForItem(
            C_CamMosDatabaseSelectionView::mhc_LastKnownExpandedTopLevelItems[u32_ItExpandedItem]);
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Map source model index to sort model index

   Hint: This function is probably only necessary because the mapFromSource seems to not work in some cases
   -> Problematic case: if there is no parent this functions seems to return an invalid internalPointer

   \param[in] orc_Index Source model index

   \return
   Sort model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_CamMosDatabaseSelectionView::m_ManualMapFromSource(const QModelIndex & orc_Index) const
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
