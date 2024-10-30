//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UnUsed ProjectFiles Settings PopUp Dialog
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGUNUSEDPROJECTFILESPOPUPDIALOG_H
#define C_NAGUNUSEDPROJECTFILESPOPUPDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_NagUnUsedProjectFilesPopUpDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagUnUsedProjectFilesPopUpDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagUnUsedProjectFilesPopUpDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_NagUnUsedProjectFilesPopUpDialog(void) override;

   void InitStaticNames(void) const;
   void ShowAllUnusedFiles(void);
   void CancelDialog(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_NagUnUsedProjectFilesPopUpDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   QStringList mc_UnUsedFilesList;
   QStringList mc_EmptyFolders;

   QStringList m_GetAllFilesInDirectory(const QString & orc_DirectoryPath);
   QStringList m_GetAllUnusedFiles(const QStringList & orc_OriginalFilesList, const QStringList & orc_TempFilesList);
   QSet<QString> m_GetAllEmptyFoldersInDirectory(const QString & orc_FolderLocation);
   QStringList m_GetAllUnUsedEmptyFolders(const QSet<QString> & orc_OriginalFolderLocation,
                                          const QSet<QString> & orc_TempFolderLocation);
   void m_UpdateSelection(const uint32_t ou32_SelectionCount) const;
   bool m_IsCurrentFolderEmpty(const QString & orc_FolderPath);
   void m_CheckAndAddEmptyFolder(const QString & orc_FolderPath, QSet<QString> & orc_EmptyFolders);
   QString m_UpdatePathToCompare(const QString & orc_Path) const;

   //Avoid call
   C_NagUnUsedProjectFilesPopUpDialog(const C_NagUnUsedProjectFilesPopUpDialog &);
   C_NagUnUsedProjectFilesPopUpDialog & operator =(const C_NagUnUsedProjectFilesPopUpDialog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
