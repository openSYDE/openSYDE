//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for adding messages from J1939 Catalog
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QHeaderView>
#include <QScrollBar>
#include <QMouseEvent>

#include "precomp_headers.hpp"
#include "constants.hpp"
#include "stwtypes.hpp"

#include "C_SdNdeSingleHeaderView.hpp"
#include "C_SdBueJ1939AddMessagesFromCatalogTableView.hpp"
#include "C_CieConverter.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
 *
 *   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueJ1939AddMessagesFromCatalogTableView::C_SdBueJ1939AddMessagesFromCatalogTableView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   //   this->mc_SortProxyModel.setSortRole(static_cast<int32_t>(Qt::DisplayRole));
   this->C_SdBueJ1939AddMessagesFromCatalogTableView::setModel(&mc_SortProxyModel);
   //Delete last selection model, see Qt documentation for setModel
   delete pc_LastSelectionModel;

   this->mc_SortProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);

   m_InitColumns();

   //UI Settings
   this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

   this->setSortingEnabled(true);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setTabKeyNavigation(false);

   //Icon
   //   make sure to resize to icon size
   this->horizontalHeader()->resizeSection(C_SdBueJ1939AddMessagesFromCatalogTableModel::h_EnumToColumn(
                                              C_SdBueJ1939AddMessagesFromCatalogTableModel::eICON),
                                           42);
   this->setIconSize(mc_ICON_SIZE_20);

   //Hide vertical header
   this->verticalHeader()->hide();

   //Hover event
   this->setMouseTracking(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTableView::m_InitColumns(void)
{
   const
   std::map<C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns,
            uint32_t> c_ColumnWidths = C_SdBueJ1939AddMessagesFromCatalogTableView::mh_GetDefaultColumnWidths();

   for (std::map<C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns,
                 uint32_t>::const_iterator c_ItEntry = c_ColumnWidths.cbegin(); c_ItEntry != c_ColumnWidths.cend();
        ++c_ItEntry)
   {
      this->setColumnWidth(C_SdBueJ1939AddMessagesFromCatalogTableModel::h_EnumToColumn(
                              c_ItEntry->first), c_ItEntry->second);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get default column widths

   \return
   Default column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns,
         uint32_t> C_SdBueJ1939AddMessagesFromCatalogTableView::mh_GetDefaultColumnWidths()
{
   std::map<C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns, uint32_t> c_ColumnWidths;
   c_ColumnWidths[C_SdBueJ1939AddMessagesFromCatalogTableModel::eICON] = 26;
   c_ColumnWidths[C_SdBueJ1939AddMessagesFromCatalogTableModel::eNAME] = 150;
   c_ColumnWidths[C_SdBueJ1939AddMessagesFromCatalogTableModel::eCOMMENT] = 150;
   c_ColumnWidths[C_SdBueJ1939AddMessagesFromCatalogTableModel::eJ1939_PGN] = 100;
   c_ColumnWidths[C_SdBueJ1939AddMessagesFromCatalogTableModel::eCAN_ID] = 100;

   return c_ColumnWidths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal data changes to table
 *
 *  \param[in]  orc_MessagesImported      Messages extracted from the DBC file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTableView::UpdateData(
   const std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_MessagesImported)
{
   this->sortByColumn(C_SdBueJ1939AddMessagesFromCatalogTableModel::h_EnumToColumn(
                         C_SdBueJ1939AddMessagesFromCatalogTableModel::eCAN_ID),
                      Qt::AscendingOrder);

   this->mc_Model.UpdateData(orc_MessagesImported);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the table contains any messages

   \return
   True  Empty
   False Not empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueJ1939AddMessagesFromCatalogTableView::IsEmpty() const
{
   return this->mc_Model.rowCount() == 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Filter for string

   \param[in]  orc_Text    String
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTableView::Search(const QString & orc_Text)
{
   this->mc_SortProxyModel.SetFilter(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the number of selected rows

   \return
   Number of rows
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdBueJ1939AddMessagesFromCatalogTableView::m_GetSelectedIndexCount() const
{
   std::vector<uint32_t> c_Rows;

   // "selectedIndexes()" returns index of each cell element.
   // number of selected indices = selected (number of columns x number of rows)
   QModelIndexList rc_IndexList = this->selectedIndexes();

   // Fetch only the rows of the selected indices
   for (uint32_t u32_IndexCounter = 0; u32_IndexCounter < static_cast<uint32_t>(rc_IndexList.size());
        u32_IndexCounter++)
   {
      const QModelIndex c_Index = rc_IndexList[u32_IndexCounter];

      if (c_Index.isValid())
      {
         c_Rows.push_back(c_Index.row());
      }
   }

   // Selecting unique elements gives the number of rows selected
   if (c_Rows.size() > 0)
   {
      C_Uti::h_Uniqueify(c_Rows);
   }

   return c_Rows.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten selection changed event slot

   Here: Emit signal with new number of selected items

   \param[in]  orc_Selected      Selected
   \param[in]  orc_Deselected    Deselected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTableView::selectionChanged(const QItemSelection & orc_Selected,
                                                                   const QItemSelection & orc_Deselected)
{
   QTableView::selectionChanged(orc_Selected, orc_Deselected);

   Q_EMIT this->SigSelectionChanged(m_GetSelectedIndexCount());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map model indices

   \param[in]  orc_SortModelIndices    Sort model indices

   \return
   Converted model indices
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndexList C_SdBueJ1939AddMessagesFromCatalogTableView::m_MapModelIndices(
   const QModelIndexList & orc_SortModelIndices) const
{
   QModelIndexList c_Retval;

   c_Retval.reserve(orc_SortModelIndices.size());
   for (QModelIndexList::ConstIterator c_ItSortModelIndex = orc_SortModelIndices.cbegin();
        c_ItSortModelIndex != orc_SortModelIndices.cend(); ++c_ItSortModelIndex)
   {
      c_Retval.push_back(this->mc_SortProxyModel.mapToSource(*c_ItSortModelIndex));
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the selected messages

   \return
   Selected messages

*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_CieConverter::C_CieNodeMessage> C_SdBueJ1939AddMessagesFromCatalogTableView::GetSelectedMessages(void)
const
{
   std::vector<C_CieConverter::C_CieNodeMessage> c_RetVal;

   // Get the original index of the selected messages mapped from the sorting order
   // This returns QModelIndex for every table cell, hence has duplicate entries ([selected]rows x columns)
   const QModelIndexList & rc_MappedIndexList = m_MapModelIndices(this->selectedIndexes());
   std::vector<uint32_t> c_Rows;

   for (uint32_t u32_IndexCounter = 0; u32_IndexCounter < static_cast<uint32_t>(rc_MappedIndexList.size());
        u32_IndexCounter++)
   {
      const QModelIndex c_Index = rc_MappedIndexList[u32_IndexCounter];

      if (c_Index.isValid())
      {
         c_Rows.push_back(c_Index.row());
      }
   }

   // Eliminate duplicate indexes (generated from "selectedIndexes()")
   C_Uti::h_Uniqueify(c_Rows);

   c_RetVal.reserve(c_Rows.size());

   for (uint32_t u32_MessageIndex = 0; u32_MessageIndex < static_cast<uint32_t>(c_Rows.size()); u32_MessageIndex++)
   {
      const C_CieConverter::C_CieNodeMessage * const pc_Message = this->mc_Model.GetDataForIndex(
         c_Rows[u32_MessageIndex]);

      if (pc_Message != NULL)
      {
         c_RetVal.push_back(*pc_Message);
      }
   }

   return c_RetVal;
}
