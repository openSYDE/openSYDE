//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UnUsed ProjectFiles Settings PopUp Dialog

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QDir>
#include <QDirIterator>

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_NagUnUsedProjectFilesPopUpDialog.hpp"
#include "ui_C_NagUnUsedProjectFilesPopUpDialog.h"
#include "constants.hpp"
#include "C_PuiProject.hpp"
#include "C_NagUnUsedProjectFilesTableView.hpp"
#include "C_Uti.hpp"

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
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagUnUsedProjectFilesPopUpDialog::C_NagUnUsedProjectFilesPopUpDialog(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_NagUnUsedProjectFilesPopUpDialog),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);
   // init icons
   this->mpc_Ui->pc_BtnRefresh->setIconSize(mc_ICON_SIZE_24);
   this->mpc_Ui->pc_BtnDeleteAll->setIconSize(mc_ICON_SIZE_24);
   this->mpc_Ui->pc_BtnRefresh->SetCustomIcons("://images/system_views/dashboards/icons/IconUpdateValueRead.svg", "",
                                               "", "://images/system_views/dashboards/icons/IconUpdateValueRead.svg");
   this->mpc_Ui->pc_BtnDeleteAll->SetCustomIcons("://images/system_views/IconClearAllEnabled.svg", "",
                                                 "", "://images/system_views/IconClearAllDisabled.svg");

   this->mpc_Ui->pc_TableViewUnUsedFiles->setSelectionMode(QAbstractItemView::ExtendedSelection);

   connect(this->mpc_Ui->pc_BtnCancel, &QPushButton::clicked, this,
           &C_NagUnUsedProjectFilesPopUpDialog::CancelDialog);
   connect(this->mpc_Ui->pc_BtnDeleteAll, &QPushButton::clicked, this->mpc_Ui->pc_TableViewUnUsedFiles,
           &C_NagUnUsedProjectFilesTableView::DeleteAllFiles);
   connect(this->mpc_Ui->pc_TableViewUnUsedFiles, &C_NagUnUsedProjectFilesTableView::SigSelectionChanged, this,
           &C_NagUnUsedProjectFilesPopUpDialog::m_UpdateSelection);
   connect(this->mpc_Ui->pc_TableViewUnUsedFiles, &C_NagUnUsedProjectFilesTableView::SigUpdateTable, this,
           &C_NagUnUsedProjectFilesPopUpDialog::ShowAllUnusedFiles);
   connect(this->mpc_Ui->pc_BtnRefresh, &QPushButton::clicked, this,
           &C_NagUnUsedProjectFilesPopUpDialog::ShowAllUnusedFiles);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ContextMenu and the Qt memory management
C_NagUnUsedProjectFilesPopUpDialog::~C_NagUnUsedProjectFilesPopUpDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesPopUpDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Project Settings"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Un-used project files"));
   this->mpc_Ui->pc_BtnCancel->setText(C_GtGetText::h_GetText("Cancel"));

   this->mpc_Ui->pc_BtnRefresh->SetToolTipInformation(C_GtGetText::h_GetText("Refresh"),
                                                      C_GtGetText::h_GetText("Refresh list of un-used files."));
   this->mpc_Ui->pc_BtnDeleteAll->SetToolTipInformation(C_GtGetText::h_GetText("Delete All"),
                                                        C_GtGetText::h_GetText("Delete all shown files."));

   this->mpc_Ui->pc_LabelEmptyFiles->setText(C_GtGetText::h_GetText("No un-used files found."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Getting files to new local folder which are currently In use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesPopUpDialog::ShowAllUnusedFiles()
{
   const QString c_FilePath = C_PuiProject::h_GetInstance()->GetPath();
   const QFileInfo c_File(c_FilePath);

   const QDir c_TempDir(C_Uti::h_GetExePath());

   const QString c_TempFolderPath = C_Uti::h_GetExePath() + "/_temp_unused_files_/" + c_File.fileName();

   const QDir c_DeleteTempDir(c_TempDir.absolutePath() + "/_temp_unused_files_/");
   QStringList c_OriginalProjectfolderFiles;
   QStringList c_TempProjectfolderFiles;

   QSet<QString> c_OriginalProjectEmptyFolders;
   QSet<QString> c_TempProjectEmptyFolders;

   if (!c_DeleteTempDir.exists())
   {
      c_DeleteTempDir.mkpath(".");
      C_PuiProject::h_GetInstance()->SaveAsWithoutInternalChange(c_TempFolderPath, false);
   }
   c_OriginalProjectfolderFiles = m_GetAllFilesInDirectory(
      C_PuiProject::h_GetInstance()->GetFolderPath());
   c_TempProjectfolderFiles = m_GetAllFilesInDirectory(
      c_TempDir.absolutePath() + "/_temp_unused_files_/");
   c_OriginalProjectEmptyFolders = this->m_GetAllEmptyFoldersInDirectory(
      C_PuiProject::h_GetInstance()->GetFolderPath());
   c_TempProjectEmptyFolders = this->m_GetAllEmptyFoldersInDirectory(
      c_TempDir.absolutePath() + "/_temp_unused_files_/");
   this->mc_EmptyFolders = this->m_GetAllUnUsedEmptyFolders(c_OriginalProjectEmptyFolders, c_TempProjectEmptyFolders);
   this->mc_UnUsedFilesList = this->m_GetAllUnusedFiles(c_OriginalProjectfolderFiles, c_TempProjectfolderFiles);
   this->mpc_Ui->pc_LabelHeadingPreview->setText(static_cast<QString>("Un-used Project Files (%1)").arg(this->
                                                                                                        mc_UnUsedFilesList
                                                                                                        .size() + this->
                                                                                                        mc_EmptyFolders.
                                                                                                        size()));
   this->mpc_Ui->pc_TableViewUnUsedFiles->UpdateData(this->mc_UnUsedFilesList, this->mc_EmptyFolders);

   if ((this->mc_UnUsedFilesList.size() + this->mc_EmptyFolders.size()) > 0)
   {
      this->mpc_Ui->pc_LabelEmptyFiles->setVisible(false);
      this->mpc_Ui->pc_TableViewUnUsedFiles->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_TableViewUnUsedFiles->setVisible(false);
      this->mpc_Ui->pc_LabelEmptyFiles->setVisible(true);
   }

   if (this->mpc_Ui->pc_TableViewUnUsedFiles->IsEmpty())
   {
      this->mpc_Ui->pc_LabelNumOfSelectedMessages->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_LabelNumOfSelectedMessages->setVisible(true);
      this->mpc_Ui->pc_LabelNumOfSelectedMessages->setText(C_GtGetText::h_GetText("No selected files"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesPopUpDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_ParentDialog.accept();
      }
      else
      {
         q_CallOrg = false;
      }
   }

   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get All Files from the Directory

   \param[in]       orc_DirectoryPath     Directory path to get list of files

   \return
   QStringList    returns list of files in given directory
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_NagUnUsedProjectFilesPopUpDialog::m_GetAllFilesInDirectory(const QString & orc_DirectoryPath)
{
   QStringList c_FilesList;

   QDirIterator c_It(orc_DirectoryPath, QDirIterator::Subdirectories);

   while (c_It.hasNext())
   {
      const QString c_FilePath = c_It.next();
      const QFileInfo c_File(c_FilePath);
      if (c_File.isFile())
      {
         c_FilesList.append(c_FilePath);
      }
   }

   return c_FilesList;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get Unused files in project folder

   \param[in]       c_OriginalFilesList     Files list in Original project folder
   \param[in]       c_TempFilesList          Files list in Temp project folder

   \return
   QStringList     UnUsed files list
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_NagUnUsedProjectFilesPopUpDialog::m_GetAllUnusedFiles(const QStringList & orc_OriginalFilesList,
                                                                    const QStringList & orc_TempFilesList)
{
   QStringList c_OriginalList = orc_OriginalFilesList;
   QStringList c_TempList = orc_TempFilesList;
   QStringList c_UnUsedFilesList;

   QStringList c_UpdatePathOriginalList;
   QStringList c_UpdatePathTempList;

   for (const QString & rc_Item : c_OriginalList)
   {
      c_UpdatePathOriginalList << this->m_UpdatePathToCompare(rc_Item);
   }

   for (const QString & rc_Item : c_TempList)
   {
      c_UpdatePathTempList << this->m_UpdatePathToCompare(rc_Item);
   }
   for (uint32_t u32_It = 0; u32_It < static_cast<uint32_t>(c_UpdatePathOriginalList.size()); ++u32_It)
   {
      const QDir c_ProjectDir(C_PuiProject::h_GetInstance()->GetFolderPath());
      if ((!c_UpdatePathTempList.contains(c_UpdatePathOriginalList[u32_It])) &&
          (c_ProjectDir.relativeFilePath(c_OriginalList[u32_It]) !=
           (C_PuiProject::h_GetInstance()->GetName() + ".syde")))
      {
         c_UnUsedFilesList << c_OriginalList[u32_It];
      }
   }

   return c_UnUsedFilesList;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all empty folders from location

   \param[in]   orc_FolderLocation    folder location

   \return
   QSet<QString>    returns set of empty folders
*/
//----------------------------------------------------------------------------------------------------------------------
QSet<QString> C_NagUnUsedProjectFilesPopUpDialog::m_GetAllEmptyFoldersInDirectory(const QString & orc_FolderLocation)
{
   QSet<QString> c_EmptyFolders;
   this->m_CheckAndAddEmptyFolder(orc_FolderLocation, c_EmptyFolders);
   return c_EmptyFolders;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all empty folders in project location

   \param[in]       orc_OriginalFolderLocation     Original project location
   \param[in]       orc_TempFolderLocation        Temp project location

   \return
   QStringList     Empty folders list  list
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_NagUnUsedProjectFilesPopUpDialog::m_GetAllUnUsedEmptyFolders(
   const QSet<QString> & orc_OriginalFolderLocation, const QSet<QString> & orc_TempFolderLocation)
{
   QSet<QString> c_UniqueEmptyFolders = orc_OriginalFolderLocation;
   c_UniqueEmptyFolders.subtract(orc_TempFolderLocation);
   QStringList c_UniqueEmptyFoldersList;
   for (const QString & rc_Folder : c_UniqueEmptyFolders)
   {
      c_UniqueEmptyFoldersList.append(rc_Folder);
   }
   return c_UniqueEmptyFoldersList;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesPopUpDialog::CancelDialog(void)
{
   QDir c_DeleteTempDir(C_Uti::h_GetExePath() + "/_temp_unused_files_/");

   if (c_DeleteTempDir.exists())
   {
      c_DeleteTempDir.removeRecursively();
   }
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update number of selected items

   \param[in]  os32_SelectionCount   Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesPopUpDialog::m_UpdateSelection(const uint32_t ou32_SelectionCount) const
{
   if (this->mpc_Ui->pc_TableViewUnUsedFiles->IsEmpty())
   {
      this->mpc_Ui->pc_LabelNumOfSelectedMessages->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_LabelNumOfSelectedMessages->setVisible(true);
      if (ou32_SelectionCount > 0)
      {
         this->mpc_Ui->pc_LabelNumOfSelectedMessages->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                                      "%1 selected file(s)")).
                                                              arg(ou32_SelectionCount));
         this->mpc_Ui->pc_TableViewUnUsedFiles->update();
      }
      else
      {
         this->mpc_Ui->pc_LabelNumOfSelectedMessages->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                                      "No selected files")));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks whether current folder is empty or not

   \param[in]   orc_FolderPath    folder path

   \return
   bool   Is current folder empty

   \retval   true   when folder or sub-folders doesn't contains any files
   \retval   false   when folder or sub-folders contains any file/s
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagUnUsedProjectFilesPopUpDialog::m_IsCurrentFolderEmpty(const QString & orc_FolderPath)
{
   const QDir c_Dir(orc_FolderPath);
   QStringList c_SubDirs = c_Dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
   const QStringList c_Files = c_Dir.entryList(QDir::Files);

   bool b_IsEmpty = true;

   if (!c_Files.isEmpty())
   {
      b_IsEmpty = false;
   }
   else
   {
      for (const QString & rc_SubDirName : c_SubDirs)
      {
         const QString c_SubDirPath = orc_FolderPath + "/" + rc_SubDirName;
         if (!m_IsCurrentFolderEmpty(c_SubDirPath))
         {
            b_IsEmpty = false;
            break;
         }
      }
   }

   return b_IsEmpty;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks all folders and sub-folders and get all empty folders

   \param[in]   orc_FolderPath    folder path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUnUsedProjectFilesPopUpDialog::m_CheckAndAddEmptyFolder(const QString & orc_FolderPath,
                                                                  QSet<QString> & orc_EmptyFolders)
{
   if (m_IsCurrentFolderEmpty(orc_FolderPath))
   {
      const QDir c_Dir(orc_FolderPath);
      orc_EmptyFolders.insert(c_Dir.absolutePath());
   }

   const QDir c_Dir(orc_FolderPath);
   QStringList c_SubDirs = c_Dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
   for (QString & rc_SubDirName : c_SubDirs)
   {
      const QString c_SubDirPath = orc_FolderPath + "/" + rc_SubDirName;
      m_CheckAndAddEmptyFolder(c_SubDirPath, orc_EmptyFolders);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update path for comparing

   \param[in]       orc_Path     File path

   \return
   QString   Path for comparing
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_NagUnUsedProjectFilesPopUpDialog::m_UpdatePathToCompare(const QString & orc_Path) const
{
   const QFileInfo c_FileInfo(orc_Path);
   const QString c_ParentDir = c_FileInfo.dir().dirName();
   const QString c_FileName = c_FileInfo.fileName();

   return c_ParentDir + "/" + c_FileName;
}
