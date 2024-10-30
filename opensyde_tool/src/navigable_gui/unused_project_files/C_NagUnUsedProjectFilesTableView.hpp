//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for unused files and empty folders
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGUNUSEDPROJECTFILESTABLEVIEW_H
#define C_NAGUNUSEDPROJECTFILESTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>

#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_TblTreMultiColumnLeafSortFilter.hpp"
#include "C_NagUnUsedProjectFilesTableModel.hpp"
#include "C_TblViewScroll.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_NagUnUsedProjectFilesTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   explicit C_NagUnUsedProjectFilesTableView(QWidget * const opc_Parent = NULL);
   void UpdateData(const QStringList & orc_UnUsedFilesList, const QStringList & orc_EmptyFoldersList);
   bool IsEmpty(void) const;
   void SelectAllFiles();
   void DeleteSelectedFiles(void);
   void DeleteAllFiles();
   bool HasVisibleFiles(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSelectionChanged(const uint32_t ou32_Count) const;
   void SigUpdateTable(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;

private:
   static std::map<opensyde_gui_logic::C_NagUnUsedProjectFilesTableModel::E_Columns,
                   uint32_t> mh_GetDefaultColumnWidths(void);
   void m_InitColumns(void);
   void m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;
   void m_ShowHideHorizontalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);

   stw::opensyde_gui_logic::C_TblTreMultiColumnLeafSortFilter mc_SortProxyModel;
   stw::opensyde_gui_logic::C_NagUnUsedProjectFilesTableModel mc_Model;
   QStyledItemDelegate mc_Delegate;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
