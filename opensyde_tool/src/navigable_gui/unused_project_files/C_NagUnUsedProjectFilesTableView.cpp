//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for unused files and empty folders table (implementation)

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>

#include "stwtypes.hpp"
#include "C_NagUnUsedProjectFilesTableView.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_GtGetText.hpp"
#include "C_SdNdeDpUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

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
C_NagUnUsedProjectFilesTableView::C_NagUnUsedProjectFilesTableView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent),
   mpc_ContextMenu(NULL)
{
   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(static_cast<int32_t>(Qt::DisplayRole));
   this->C_NagUnUsedProjectFilesTableView::setModel(&mc_SortProxyModel);
   this->mc_SortProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
   this->setItemDelegateForColumn(0, &this->mc_Delegate);
   this->m_InitColumns();
   this->m_SetupContextMenu();

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->horizontalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->setSortingEnabled(true);
   this->verticalHeader()->setVisible(false);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_NagUnUsedProjectFilesTableView::m_ShowHideVerticalScrollBar);
   connect(this->horizontalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_NagUnUsedProjectFilesTableView::m_ShowHideHorizontalScrollBar);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data

   \param[in]       orc_UnUsedFilesList     List of Unused files
   \param[in]       orc_EmptyFoldersList    List of Empty folders
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::UpdateData(const QStringList & orc_UnUsedFilesList,
                                                  const QStringList & orc_EmptyFoldersList)
{
   this->sortByColumn(C_NagUnUsedProjectFilesTableModel::h_EnumToColumn(
                         C_NagUnUsedProjectFilesTableModel::E_Columns::ePATH),
                      Qt::AscendingOrder);

   this->mc_Model.UpdateData(orc_UnUsedFilesList, orc_EmptyFoldersList);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if any messages present

   \return
   True  Empty
   False Not empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagUnUsedProjectFilesTableView::IsEmpty() const
{
   return (this->mc_Model.rowCount() == 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select all messages
 *
  */
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::SelectAllFiles()
{
   const QModelIndex c_TopLeft = this->mc_Model.index(0, 0);
   const QModelIndex c_BottomRight = this->mc_Model.index(this->mc_Model.rowCount() - 1,
                                                          this->mc_Model.columnCount() - 1);
   const QItemSelection c_Selection(c_TopLeft, c_BottomRight);

   this->selectionModel()->select(c_Selection, QItemSelectionModel::Select);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete selected files
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::DeleteSelectedFiles()
{
   C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);
   const QItemSelection c_Selection = this->selectionModel()->selection();

   QItemSelection c_SourceSelection;

   for (const QModelIndex & rc_Index : c_Selection.indexes())
   {
      const QModelIndex c_SourceIndex = mc_SortProxyModel.mapToSource(rc_Index);
      if (c_SourceIndex.isValid())
      {
         c_SourceSelection.select(c_SourceIndex, c_SourceIndex);
      }
   }
   const QString c_Details = this->mc_Model.DetailsOfFilesToDelete(c_SourceSelection);

   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Delete Files"));
   c_MessageBox.SetDescription(
      "Are you sure about deleting following files?");
   c_MessageBox.SetDetails(c_Details);
   c_MessageBox.SetOkButtonText("Yes");
   c_MessageBox.SetNoButtonText("Cancel");
   c_MessageBox.SetCustomMinHeight(180, 550);
   c_MessageBox.SetCustomMinWidth(800);

   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
   {
      this->mc_Model.DeleteSelectedFiles(c_Selection);
      Q_EMIT (this->SigUpdateTable());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete all files
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::DeleteAllFiles()
{
   if (!this->IsEmpty())
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Delete All Files"));
      c_MessageBox.SetDescription(
         "Are you sure about deleting all files?");
      c_MessageBox.SetOkButtonText("Yes");
      c_MessageBox.SetNoButtonText("Cancel");
      c_MessageBox.SetCustomMinHeight(180, 550);
      c_MessageBox.SetCustomMinWidth(800);

      if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
      {
         this->mc_Model.DeleteAllFiles();
         Q_EMIT (this->SigUpdateTable());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if any messages are visible

   \return
   True  visible
   False none visible
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagUnUsedProjectFilesTableView::HasVisibleFiles() const
{
   return (this->mc_SortProxyModel.rowCount() != 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_A))
   {
      if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
      {
         this->SelectAllFiles();
      }
   }

   if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Delete))
   {
      this->DeleteSelectedFiles();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten selection changed event slot

   Here: Emit signal with new number of selected items

   \param[in]  orc_Selected      Selected
   \param[in]  orc_Deselected    Deselected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::selectionChanged(const QItemSelection & orc_Selected,
                                                        const QItemSelection & orc_Deselected)
{
   // This method call ensures correct item row selection
   C_TblViewScroll::selectionChanged(orc_Selected, orc_Deselected);

   std::vector<uint32_t> c_SelectedIndices;
   c_SelectedIndices = C_SdNdeDpUtil::h_ConvertVector(this->selectedIndexes());
   C_Uti::h_Uniqueify(c_SelectedIndices);
   Q_EMIT this->SigSelectionChanged(c_SelectedIndices.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get default column widths

   \return
   Default column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_NagUnUsedProjectFilesTableModel::E_Columns,
         uint32_t> C_NagUnUsedProjectFilesTableView::mh_GetDefaultColumnWidths()
{
   std::map<C_NagUnUsedProjectFilesTableModel::E_Columns, uint32_t> c_ColumnWidths;
   c_ColumnWidths[C_NagUnUsedProjectFilesTableModel::ePATH] = 660;
   c_ColumnWidths[C_NagUnUsedProjectFilesTableModel::eEXTENSION] = 153;

   return c_ColumnWidths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::m_InitColumns()
{
   const
   std::map<C_NagUnUsedProjectFilesTableModel::E_Columns,
            uint32_t> c_ColumnWidths = C_NagUnUsedProjectFilesTableView::mh_GetDefaultColumnWidths();

   for (std::map<C_NagUnUsedProjectFilesTableModel::E_Columns,
                 uint32_t>::const_iterator c_ItEntry = c_ColumnWidths.cbegin(); c_ItEntry != c_ColumnWidths.cend();
        ++c_ItEntry)
   {
      this->setColumnWidth(C_NagUnUsedProjectFilesTableModel::h_EnumToColumn(
                              c_ItEntry->first), c_ItEntry->second);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  \brief  Show hide vertical scroll bar

   \param[in]  os32_Min  Minimum range
   \param[in]  os32_Max  Maximum range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const
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
void C_NagUnUsedProjectFilesTableView::m_ShowHideHorizontalScrollBar(const int32_t os32_Min,
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
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::m_SetupContextMenu()
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select all"),
                                    this,
                                    &C_NagUnUsedProjectFilesTableView::SelectAllFiles,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_A));
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete selected"),
                                    this,
                                    &C_NagUnUsedProjectFilesTableView::DeleteSelectedFiles,
                                    static_cast<int32_t>(Qt::Key_Delete));

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_NagUnUsedProjectFilesTableView::customContextMenuRequested, this,
           &C_NagUnUsedProjectFilesTableView::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesTableView::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

   this->mpc_ContextMenu->popup(c_PosGlobal);
}
