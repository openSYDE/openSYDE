//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QScrollBar>
#include <QStandardItemModel>
#include <QList>

#include "precomp_headers.hpp"
#include "stwtypes.hpp"
#include "C_SdBueJ1939AddMessagesFromCatalogTreeView.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
//using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

/*! \brief  Default constructor/destructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueJ1939AddMessagesFromCatalogTreeDelegate::C_SdBueJ1939AddMessagesFromCatalogTreeDelegate(
   QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden paint event

   Inserts space only for child items. In this case child items are signals

   \param[in,out]  opc_Painter   Painter
   \param[in]      orc_Option    Style option
   \param[in]      orc_Index     Index of the current item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeDelegate::paint(QPainter * const opc_Painter,
                                                           const QStyleOptionViewItem & orc_Option,
                                                           const QModelIndex & orc_Index) const
{
   // paint parent of current item in selected style
   QStyleOptionViewItem c_Option = orc_Option;

   // Add space only for child items
   // That means, orc_Index refers to an an item having a valid parent and having no further child items
   if ((orc_Index.parent().isValid()) && ((orc_Index.model() == NULL) || (orc_Index.model()->rowCount(orc_Index) == 0)))
   {
      c_Option.rect.adjust(20, 0, 0, 0);
   }

   // draw original
   QStyledItemDelegate::paint(opc_Painter, c_Option, orc_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
 *
 *   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueJ1939AddMessagesFromCatalogTreeView::C_SdBueJ1939AddMessagesFromCatalogTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(static_cast<int32_t>(Qt::DisplayRole));
   this->C_SdBueJ1939AddMessagesFromCatalogTreeView::setModel(&mc_SortProxyModel);
   //Delete last selection model, see Qt documentation for setModel
   delete pc_LastSelectionModel;

   this->mc_SortProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
   this->setItemDelegateForColumn(0, &this->mc_Delegate);

   m_InitColumns();

   // Hide CAN Id column (In case CAN Id is used in future)
   this->setColumnHidden(C_SdBueJ1939AddMessagesFromCatalogTreeModel::h_EnumToColumn(
                            C_SdBueJ1939AddMessagesFromCatalogTreeModel::eCAN_ID), true);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->horizontalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   this->setSortingEnabled(true);

   connect(this, &QTreeView::clicked, this,
           &C_SdBueJ1939AddMessagesFromCatalogTreeView::m_OnItemChecked);
   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdBueJ1939AddMessagesFromCatalogTreeView::m_ShowHideVerticalScrollBar);
   connect(this->horizontalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdBueJ1939AddMessagesFromCatalogTreeView::m_ShowHideHorizontalScrollBar);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if any messages present

   \return
   True  Empty
   False Not empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueJ1939AddMessagesFromCatalogTreeView::IsEmpty() const
{
   return (this->mc_Model.rowCount() == 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convey data changes to model
*
*  \param[in]  orc_MessagesImported      Messages extracted from the DBC file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeView::UpdateData(
   const std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_MessagesImported)
{
   this->sortByColumn(C_SdBueJ1939AddMessagesFromCatalogTreeModel::h_EnumToColumn(
                         C_SdBueJ1939AddMessagesFromCatalogTreeModel::ePGN_SPN),
                      Qt::AscendingOrder);

   this->mc_Model.UpdateData(orc_MessagesImported);

   // Resize the "Comment" column (Since it may have longer text)
   this->resizeColumnToContents(C_SdBueJ1939AddMessagesFromCatalogTreeModel::h_EnumToColumn(
                                   C_SdBueJ1939AddMessagesFromCatalogTreeModel::eCOMMENT));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select all messages
 *
  */
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeView::SelectAllMessages()
{
   const uint32_t u32_FilteredRowCount =  static_cast<uint32_t>(this->mc_SortProxyModel.rowCount());
   const uint32_t u32_TotalRowCount = static_cast<uint32_t>(this->mc_Model.rowCount());

   // No filtered messages exist, therefore all the messages to be selected.
   if (u32_FilteredRowCount == u32_TotalRowCount)
   {
      // data gives PGN of filtered messages. Fetch these messages and select them.
      this->mc_Model.SelectAllParentItems();
   }
   // Filtered messages exist, hence select only these (that are visible).
   else
   {
      if ((u32_FilteredRowCount != 0) && (u32_FilteredRowCount < u32_TotalRowCount))
      {
         QModelIndexList c_IndexList;

         // Save the indexes of the filtered messages
         for (uint32_t u32_RowIndex = 0; u32_RowIndex < u32_FilteredRowCount; u32_RowIndex++)
         {
            // Map the sort index to the actual index. The tree item at the actual index contains the message vector
            // index.
            const QModelIndex c_SortFilterIndex = this->mc_SortProxyModel.index(u32_RowIndex, 2);
            c_IndexList.append(this->mc_SortProxyModel.mapToSource(c_SortFilterIndex));
         }

         // Select only those messages that have been filtered and are visible
         this->mc_Model.SelectFilteredParentItems(c_IndexList);
      }
   }

   Q_EMIT (this->SigSelectionChanged(this->mc_Model.GetCheckedItemCount()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Unselect all messages

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeView::UnSelectAllMessages(void)
{
   this->mc_Model.UnselectAllParentItems();
   Q_EMIT (this->SigSelectionChanged(0));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeView::m_InitColumns()
{
   const
   std::map<C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns,
            uint32_t> c_ColumnWidths = C_SdBueJ1939AddMessagesFromCatalogTreeView::mh_GetDefaultColumnWidths();

   for (std::map<C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns,
                 uint32_t>::const_iterator c_ItEntry = c_ColumnWidths.cbegin(); c_ItEntry != c_ColumnWidths.cend();
        ++c_ItEntry)
   {
      this->setColumnWidth(C_SdBueJ1939AddMessagesFromCatalogTreeModel::h_EnumToColumn(
                              c_ItEntry->first), c_ItEntry->second);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  \brief  Show hide vertical scroll bar

   \param[in]  os32_Min  Minimum range
   \param[in]  os32_Max  Maximum range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeView::m_ShowHideVerticalScrollBar(const int32_t os32_Min,
                                                                             const int32_t os32_Max) const
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
/*! \brief  \brief  Show hide horizontal scroll bar

   \param[in]  os32_Min  Minimum range
   \param[in]  os32_Max  Maximum range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeView::m_ShowHideHorizontalScrollBar(const int32_t os32_Min,
                                                                               const int32_t os32_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->horizontalScrollBar()->hide();
   }
   else
   {
      this->horizontalScrollBar()->show();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get default column widths

   \return
   Default column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns,
         uint32_t> C_SdBueJ1939AddMessagesFromCatalogTreeView::mh_GetDefaultColumnWidths()
{
   std::map<C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns, uint32_t> c_ColumnWidths;
   c_ColumnWidths[C_SdBueJ1939AddMessagesFromCatalogTreeModel::eNAME] = 200;
   c_ColumnWidths[C_SdBueJ1939AddMessagesFromCatalogTreeModel::eCOMMENT] = 250;
   c_ColumnWidths[C_SdBueJ1939AddMessagesFromCatalogTreeModel::ePGN_SPN] = 150;
   c_ColumnWidths[C_SdBueJ1939AddMessagesFromCatalogTreeModel::eCAN_ID] = 100;

   return c_ColumnWidths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Called when a tree item is clicked i.e item checkbox is checked

   \param[in]       orc_Index     Index of the tree item clicked

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeView::m_OnItemChecked()
{
   const uint32_t u32_ItemCount = this->mc_Model.GetCheckedItemCount();

   Q_EMIT (this->SigSelectionChanged(u32_ItemCount));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Filter for string

   \param[in]  orc_Text    String
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeView::Search(const QString & orc_Text)
{
   this->mc_SortProxyModel.SetFilter(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the selected messages

   \return
   Selected messages

*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_CieConverter::C_CieNodeMessage> C_SdBueJ1939AddMessagesFromCatalogTreeView::GetSelectedMessages(void)
const
{
   std::vector<C_CieConverter::C_CieNodeMessage> c_RetVal;

   const uint32_t u32_SelectedMessageCount = this->mc_Model.GetCheckedItemCount();

   c_RetVal.reserve(u32_SelectedMessageCount);
   this->mc_Model.GetSelectedMessages(c_RetVal);

   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if any messages are visible

   \return
   True  visible
   False none visible
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueJ1939AddMessagesFromCatalogTreeView::HasVisibleData() const
{
   return (this->mc_SortProxyModel.rowCount() != 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeView::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   // Shortcut (Ctrl + A) to select all messages
   if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_A))
   {
      if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
      {
         this->SelectAllMessages();
      }
   }
}
