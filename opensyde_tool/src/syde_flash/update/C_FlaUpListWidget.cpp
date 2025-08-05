//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for showing all hex files in update widget section

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>
#include <cmath>

#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_FlaUpListItemWidget.hpp"
#include "C_FlaUpListWidget.hpp"
#include "C_UsHandler.hpp"

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

  \param[in,out]  opc_Parent    parent widget
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpListWidget::C_FlaUpListWidget(QWidget * const opc_Parent) :
   QListWidget(opc_Parent),
   mpc_ContextMenu(NULL),
   mq_EnableSettings(true)
{
   this->setFrameShape(QFrame::NoFrame);
   this->m_LoadLastKnownHexFilePaths();
   this->setSelectionMode(QAbstractItemView::MultiSelection);
   this->setDragDropMode(QAbstractItemView::DropOnly);
   this->setAcceptDrops(true);
   this->setMouseTracking(true);

   this->m_SetupContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpListWidget::~C_FlaUpListWidget()
{
   delete this->mpc_ContextMenu;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add File action dialog heading and filter

   \param[in]  oq_IsActionForExistingFile    Is action for existing file
   \param[in]  os32_CurrentHexFileIndex      Current hex file widget index
   \param[in]  oq_IsFileToReplace      Is file to replace
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::AddFileAction(const bool oq_IsActionForExistingFile, const int32_t os32_CurrentHexFileIndex,
                                      const bool oq_IsFileToReplace)
{
   QString c_Folder;

   if (!oq_IsActionForExistingFile)
   {
      c_Folder = C_UsHandler::h_GetInstance()->GetLastKnownUpdateHexFileLocation();

      if (c_Folder.isEmpty())
      {
         c_Folder = C_Uti::h_GetExePath();
      }
   }

   else
   {
      QListWidgetItem * const pc_CurrentItem = this->item(os32_CurrentHexFileIndex);
      if (pc_CurrentItem != NULL)
      {
         C_FlaUpListItemWidget * const pc_ItemWidget =
            dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));

         if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
         {
            const QFileInfo c_FileInfo(pc_ItemWidget->pc_HexFileInfo->c_HexFileInfo.c_FilePath);

            if (c_FileInfo.exists())
            {
               c_Folder = pc_ItemWidget->pc_HexFileInfo->c_HexFileInfo.c_FilePath;
            }
            else
            {
               c_Folder = C_Uti::h_GetExePath();
            }
         }
      }
   }

   this->m_AddNewHexFile(C_GtGetText::h_GetText("Select HEX file"),
                         static_cast<QString>(C_GtGetText::h_GetText("Hex file")) +
                         " (*.hex)", c_Folder, oq_IsActionForExistingFile, os32_CurrentHexFileIndex,
                         oq_IsFileToReplace);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known hex files to user settings.ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::SetLastKnownHexFilePaths()
{
   C_UsHandler::h_GetInstance()->SetLastKnownUpdateHexFilePaths(this->GetHexFilePaths());
   C_UsHandler::h_GetInstance()->SetHexFilePathsAsRelativeOrAbsolute(this->m_GetHexFilePathsAsRelativeOrAbsolute());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list of all HEX file paths

   \return
   HEX file paths
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_FlaUpListWidget::GetHexFilePaths(void) const
{
   QStringList c_HexFilePaths;

   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         C_FlaUpListItemWidget * const pc_ItemWidget =
            dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
         if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
         {
            c_HexFilePaths.append(pc_ItemWidget->pc_HexFileInfo->c_HexFileInfo.c_FilePath);
         }
      }
   }

   return c_HexFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get hex file device names

   \return
   Hex file device names
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_FlaUpListWidget::GetHexFileDeviceNames(void) const
{
   QStringList c_DeviceNames;

   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         C_FlaUpListItemWidget * const pc_ItemWidget =
            dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
         if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL) &&
             (pc_ItemWidget->GetApplicationBlockIndex() <
              static_cast<int32_t>(pc_ItemWidget->pc_HexFileInfo->c_BlockInfo.size())))
         {
            c_DeviceNames.append(
               pc_ItemWidget->pc_HexFileInfo->c_BlockInfo[pc_ItemWidget->GetApplicationBlockIndex()].c_DeviceId);
         }
      }
   }

   return c_DeviceNames;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get information if all files are valid and existing

   \retval   True    all files are existing and valid
   \retval   False   at least one file is missing or invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_FlaUpListWidget::AreAllFilesValid(void) const
{
   bool q_Return = true;

   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         const C_FlaUpListItemWidget * const pc_ItemWidget =
            dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
         if (pc_ItemWidget != NULL)
         {
            if (pc_ItemWidget->IsFileExistingAndValid() == false)
            {
               q_Return = false;
               break;
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enable/disable settings widgets

   \param[in]  oq_Enabled  true: enable, false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::EnableSettings(const bool oq_Enabled)
{
   this->mq_EnableSettings = oq_Enabled;
   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         C_FlaUpListItemWidget * const pc_ItemWidget =
            dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
         if (pc_ItemWidget != NULL)
         {
            pc_ItemWidget->EnableSettings(oq_Enabled);
         }
      }
   }
   this->mpc_ContextMenu->setEnabled(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden drop event slot, here accepts MimeData has hasUrls

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::DropEvent(QDropEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   if (pc_MimeData->hasUrls())
   {
      QPoint c_DropPos = opc_Event->pos();
      int32_t s32_DroppedItemAtIndex = -1;
      const QList<QUrl>  c_Urls = pc_MimeData->urls();
      const int32_t s32_UrlsCount = c_Urls.size();
      QListWidgetItem * pc_DropItem;
      c_DropPos.setY(opc_Event->pos().y() - 56);
      pc_DropItem = this->itemAt(c_DropPos);
      if (pc_DropItem != NULL)
      {
         C_FlaUpListItemWidget * const pc_ItemWidget =
            dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_DropItem));
         if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
         {
            s32_DroppedItemAtIndex = pc_ItemWidget->pc_HexFileInfo->s32_CurrentHexFileIndex;
         }
      }

      for (int32_t s32_It = 0; s32_It < s32_UrlsCount; ++s32_It)
      {
         const QUrl & rc_Url = c_Urls.at(s32_It);
         const QString c_LocalFilePath = rc_Url.toLocalFile();

         if (rc_Url.isLocalFile() && rc_Url.toString().endsWith(".hex"))
         {
            opc_Event->setAccepted(true);
            if ((s32_DroppedItemAtIndex >= 0) && (s32_DroppedItemAtIndex < this->count()))
            {
               QListWidgetItem * const pc_CurrentItem = this->item(s32_DroppedItemAtIndex);
               if (pc_CurrentItem != NULL)
               {
                  C_FlaUpListItemWidget * const pc_ItemWidget =
                     dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
                  if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
                  {
                     if ((s32_UrlsCount > 1) && (s32_It == (s32_UrlsCount - 1))) //Show warning message box only
                                                                                 // on last file when try to drop
                                                                                 // multiple
                                                                                 // files on single file
                     {
                        C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eWARNING);

                        c_MessageBox.SetHeading(C_GtGetText::h_GetText("Multiple files"));
                        c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                                       "Multiple files cannot be replaced by single file ") +
                                                    pc_ItemWidget->pc_HexFileInfo->c_HexFileInfo.c_FileName);
                        c_MessageBox.SetOkButtonText("OK");
                        c_MessageBox.SetCustomMinHeight(180, 180);
                        c_MessageBox.Execute();
                     }
                     else if (s32_UrlsCount == 1)
                     {
                        C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);

                        c_MessageBox.SetHeading(C_GtGetText::h_GetText("Replace HEX file"));
                        c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                                       "Do you really want to replace ") + pc_ItemWidget->pc_HexFileInfo->c_HexFileInfo.c_FileName +
                                                    C_GtGetText::h_GetText(" with ") + rc_Url.fileName());
                        c_MessageBox.SetOkButtonText("Replace");
                        c_MessageBox.SetNoButtonText("Cancel");
                        c_MessageBox.SetCustomMinHeight(180, 180);
                        if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
                        {
                           this->m_AddFile(c_LocalFilePath, true, s32_DroppedItemAtIndex, this->m_AskUserToSaveRelativePath(
                                              c_LocalFilePath,
                                              C_Uti::h_GetExePath()));
                        }
                     }
                     else
                     {
                        //Do nothing here
                     }
                  }
               }
            }
            else
            {
               this->m_AddFile(c_LocalFilePath, false, this->count(),
                               this->m_AskUserToSaveRelativePath(c_LocalFilePath, C_Uti::h_GetExePath()));
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden drag enter event slot, here accepts MimeData has hasUrls

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::DragEnterEvent(QDragEnterEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   if ((this->mq_EnableSettings == true) && pc_MimeData->hasUrls())
   {
      const QList<QUrl> c_Urls = pc_MimeData->urls();
      for (int32_t s32_It = 0; s32_It < c_Urls.size(); ++s32_It)
      {
         const QUrl & rc_Url = c_Urls.at(s32_It);
         if (rc_Url.isLocalFile() && rc_Url.toString().endsWith(".hex"))
         {
            opc_Event->setAccepted(true);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden drag move event slot, here accepts MimeData has hasUrls

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::DragMoveEvent(QDragMoveEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   if (pc_MimeData->hasUrls())
   {
      const QList<QUrl> c_Urls = pc_MimeData->urls();
      for (int32_t s32_It = 0; s32_It < c_Urls.size(); ++s32_It)
      {
         const QUrl & rc_Url = c_Urls.at(s32_It);
         if (rc_Url.isLocalFile() && rc_Url.toString().endsWith(".hex"))
         {
            opc_Event->setAccepted(true);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of Bytes of all files

   \return
   uint32_t
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_FlaUpListWidget::GetTotalHexFileSize(void) const
{
   uint32_t u32_SizeInBytes = 0;

   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      const uint32_t u32_TempSizeInBytes = GetHexFileSize(s32_It);
      u32_SizeInBytes += u32_TempSizeInBytes;
   }

   return u32_SizeInBytes;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of Bytes

   \param[in]       oru32_HexFileIndex     Index of the hex file in list

   \return
   int32_t
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_FlaUpListWidget::GetHexFileSize(const uint32_t & oru32_HexFileIndex) const
{
   QListWidgetItem * const pc_CurrentItem = this->item(oru32_HexFileIndex);
   uint32_t u32_SizeInBytes = 0;

   if (pc_CurrentItem != NULL)
   {
      C_FlaUpListItemWidget * const pc_ItemWidget =
         dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
      if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
      {
         const QString c_BYTES_STR = " bytes";
         const QString c_NumberOfBytesStr = pc_ItemWidget->pc_HexFileInfo->c_HexFileInfo.c_Size;
         const QStringList c_NumberBytes = c_NumberOfBytesStr.split(c_BYTES_STR);
         if (c_NumberBytes.size() > 0)
         {
            u32_SizeInBytes = c_NumberBytes[0].toInt();
         }
      }
   }
   return u32_SizeInBytes;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set file status update

   \param[in]       ors32_FileIndex     Flashed file index
   \param[in]       oru8_State         State
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::SetStatusIcon(const int32_t & ors32_FileIndex, const uint8_t & oru8_State)
{
   QListWidgetItem * const pc_Item = this->item(ors32_FileIndex);

   if (pc_Item != NULL)
   {
      C_FlaUpListItemWidget * const pc_ItemWidget =
         dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_Item));

      if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
      {
         pc_ItemWidget->SetState(oru8_State);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set file status update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::ResetStatusIcons(void)
{
   const uint8_t u8_DISABLE_ICON = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_DISABLED);

   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         const C_FlaUpListItemWidget * const pc_ItemWidget =
            dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
         if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
         {
            SetStatusIcon(s32_It, u8_DISABLE_ICON);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds a new file for the file based file list, a hex file

   \param[in]  orc_File                      New path
   \param[in]  oq_HexFile                    Is hex file
   \param[in]  oq_IsActionForExistingFile    Is action for existing file
   \param[in]  os32_CurrentHexFileIndex      Current hex file index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_AddNewFile(const QString & orc_File, const bool oq_HexFile,
                                     const bool oq_IsActionForExistingFile, const int32_t os32_CurrentHexFileIndex)
{
   if ((oq_HexFile == true) && (!orc_File.isEmpty()))
   {
      this->m_AddFile(orc_File, oq_IsActionForExistingFile, os32_CurrentHexFileIndex,
                      this->m_AskUserToSaveRelativePath(orc_File, C_Uti::h_GetExePath()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds a new file for the file container

   Only possible for file based devices

   \param[in]  orc_File                      New path
   \param[in]  oq_IsActionForExistingFile    ActionForExistingFile
   \param[in]  os32_CurrentHexFileIndex      Current hex file index
   \param[in]  oq_IsRelativePath             Information whether file path is relative or absolute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_AddFile(const QString & orc_File, const bool oq_IsActionForExistingFile,
                                  const int32_t os32_CurrentHexFileIndex, const bool oq_IsRelativePath)
{
   bool q_HexFile = false;

   if (!orc_File.isEmpty())
   {
      q_HexFile = m_IsFileHexFile(orc_File);
   }

   if (q_HexFile)
   {
      bool q_FileAlreadyExists = false;
      for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
      {
         QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
         if (pc_CurrentItem != NULL)
         {
            C_FlaUpListItemWidget * const pc_ItemWidget =
               dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
            if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
            {
               const QString c_FilePath = pc_ItemWidget->pc_HexFileInfo->c_HexFileInfo.c_FilePath;
               if (c_FilePath == orc_File)
               {
                  q_FileAlreadyExists = true;
               }
            }
         }
      }
      // Add the file
      if (q_FileAlreadyExists == false)
      {
         C_FlaUpHexFileInfo * const pc_HexFileInfo = new C_FlaUpHexFileInfo(orc_File);
         QListWidgetItem * const pc_Item = new QListWidgetItem();
         C_FlaUpListItemWidget * pc_FileWidget;
         pc_HexFileInfo->c_HexFileInfo.c_FilePath = orc_File;
         pc_HexFileInfo->s32_CurrentHexFileIndex = os32_CurrentHexFileIndex;
         pc_FileWidget = new C_FlaUpListItemWidget(this, pc_HexFileInfo, oq_IsRelativePath);
         pc_Item->setSizeHint(QSize(pc_FileWidget->width(), pc_FileWidget->height()));
         if (!oq_IsActionForExistingFile)
         {
            this->addItem(pc_Item);
            Q_EMIT (this->SigUpdateFileCounter());
         }
         else
         {
            this->m_DeleteItem(os32_CurrentHexFileIndex, false);
            this->insertItem(os32_CurrentHexFileIndex, pc_Item);
         }
         this->setItemWidget(pc_Item, pc_FileWidget);
         this->show();

         connect(pc_FileWidget, &C_FlaUpListItemWidget::SigDeleteItemWidget, this, &C_FlaUpListWidget::m_DeleteItem);

         connect(pc_FileWidget, &C_FlaUpListItemWidget::SigSelectFile, this, &C_FlaUpListWidget::m_SelectFile);

         connect(pc_FileWidget, &C_FlaUpListItemWidget::SigDeleteAllItems, this, &C_FlaUpListWidget::m_DeleteAllItem);

         connect(pc_FileWidget, &C_FlaUpListItemWidget::SigReloadCurrentFile, this,
                 &C_FlaUpListWidget::m_ReloadCurrentFile);
         Q_EMIT (this->SigFileAdded());
      } //lint !e429  //no memory leak of pc_HexFileInfo, pc_FileWidget, pc_Item because of handling in
        // "this->DeleteItem()"
      else
      {
         C_OgeWiCustomMessage c_Message(this);
         c_Message.SetHeading(C_GtGetText::h_GetText("Add HEX file"));
         c_Message.SetDescription(
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "The file is already contained in the HEX file list "
                                    "and therefore not added again.")));
         c_Message.SetDetails(
            static_cast<QString>(C_GtGetText::h_GetText("%1")).arg(orc_File));
         c_Message.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if the file is an hex file

   The check decides based on the file extension.

   \param[in]  orc_File    File to check

   \retval   true    The file is a hex file
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_FlaUpListWidget::m_IsFileHexFile(const QString & orc_File)
{
   bool q_HexFile = false;

   if (!orc_File.isEmpty())
   {
      const QString c_FileExtension = "hex";

      if (m_LastSuffixOfFileName(orc_File).toLower() == c_FileExtension)
      {
         q_HexFile = true;
      }
   }

   return q_HexFile;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load known hex file into update window
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_LoadLastKnownHexFilePaths()
{
   QStringList c_HexFilePaths = C_UsHandler::h_GetInstance()->GetLastKnownUpdateHexFilePaths();
   QStringList c_HexFilePathsAsRelativeOrAbsolute =
      C_UsHandler::h_GetInstance()->GetHexFilePathsAsRelativeOrAbsolute();

   for (int32_t s32_It = 0; s32_It < static_cast<int32_t>(c_HexFilePaths.size()); s32_It++)
   {
      if (c_HexFilePathsAsRelativeOrAbsolute[s32_It] == "Relative")
      {
         this->m_AddFile(c_HexFilePaths[s32_It], false, s32_It, true);
      }
      else
      {
         this->m_AddFile(c_HexFilePaths[s32_It], false, s32_It, false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updating index for Hex file widget

   \param[in]  os32_CurrentHexFileIndex      Current hex file widget index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_UpdateHexFileWidgetIndex(const int32_t os32_CurrentHexFileIndex) const
{
   for (int32_t s32_It = os32_CurrentHexFileIndex; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         C_FlaUpListItemWidget * const pc_ItemWidget =
            dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
         if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
         {
            if (pc_ItemWidget->pc_HexFileInfo->s32_CurrentHexFileIndex <= 0)
            {
               pc_ItemWidget->pc_HexFileInfo->s32_CurrentHexFileIndex = 0;
            }
            else
            {
               --pc_ItemWidget->pc_HexFileInfo->s32_CurrentHexFileIndex;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new Hex files from context menu
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_AddFilesFromContextMenu()
{
   this->AddFileAction(false, this->count(), false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creating context menu with required actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_SetupContextMenu()
{
   mpc_ContextMenu = new stw::opensyde_gui_elements::C_OgeContextMenu(this);
   this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Add file(s)"), this,  &C_FlaUpListWidget::m_AddFilesFromContextMenu);
   this->mpc_ContextMenu->addSeparator();
   this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Remove all Files"), this, &C_FlaUpListWidget::m_DeleteAllItem);
   this->setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this, &C_FlaUpListWidget::customContextMenuRequested, this,
           &C_FlaUpListWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  how custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   if (this->mpc_ContextMenu != NULL)
   {
      this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Ask user if they wants to save the path as relative or absolute.

   \param[in]  orc_Path                   relative or absolute path of file or directory

   \retval   true     If path to be shown as relative
   \retval   false    If path to be shown as absolute
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_FlaUpListWidget::m_AskUserToSaveRelativePath(const QString & orc_Path, const QString & orc_AbsoluteReferenceDir)
{
   bool c_Return;
   QString c_PathRelative;
   QString c_PathAbsolute;
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);

   C_Uti::h_IsPathRelativeToDir(orc_Path, orc_AbsoluteReferenceDir, c_PathAbsolute, c_PathRelative);

   c_Message.SetHeading(C_GtGetText::h_GetText("Relative Path"));
   c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                    "Do you want to save the selected path (%1) relative or absolute?")).arg(
                               c_PathAbsolute));
   c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("Relative path: %1 \nAbsolute path: %2")).
                        arg(c_PathRelative).arg(c_PathAbsolute));
   c_Message.SetOkButtonText(C_GtGetText::h_GetText("Relative"));
   c_Message.SetNoButtonText(C_GtGetText::h_GetText("Absolute"));
   c_Message.SetCustomMinHeight(230, 250);

   if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
   {
      c_Return = true;
   }
   else
   {
      c_Return = false;
   }
   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adding a new hex file

   \param[in]      orc_DialogCaption                  Caption for file dialog
   \param[in]      orc_DialogFilter                   Filter configuration for file dialog
   \param[in]      oc_Folder                          Folder of hex file
   \param[in]      oq_IsActionForExistingFile         Is Action for existing files
   \param[in,out]  os32_CurrentHexFileIndex           Current hex file widget index
   \param[in]      oq_IsFileToReplace                 Is file to replace
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_AddNewHexFile(const QString & orc_DialogCaption, const QString & orc_DialogFilter,
                                        const QString & orc_Folder, const bool oq_IsActionForExistingFile,
                                        int32_t os32_CurrentHexFileIndex, const bool oq_IsFileToReplace)
{
   QStringList c_Files;

   if (!oq_IsFileToReplace)
   {
      c_Files = QFileDialog::getOpenFileNames(this, orc_DialogCaption, orc_Folder, orc_DialogFilter);
   }
   else
   {
      const QString c_File = C_OgeWiUtil::h_GetOpenFileName(this, orc_DialogCaption, orc_Folder, orc_DialogFilter,
                                                            "*.hex");
      if (!c_File.isEmpty())
      {
         c_Files.append(c_File);
      }
   }

   for (const QString & rc_File : c_Files)
   {
      const bool q_HexFile = (m_LastSuffixOfFileName(rc_File) == "hex");

      if (!q_HexFile)
      {
         const QString c_Details = static_cast<QString>("File path: %1").arg(rc_File);
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Update Package Configuration"));
         c_Message.SetDescription(C_GtGetText::h_GetText("Invalid file: only *.hex files are allowed."));
         c_Message.SetDetails(c_Details);
         c_Message.SetCustomMinHeight(180, 250);
         c_Message.Execute();
      }
      else
      {
         if (!oq_IsActionForExistingFile)
         {
            os32_CurrentHexFileIndex = this->count();
         }
         // add file
         this->m_AddNewFile(rc_File, q_HexFile, oq_IsActionForExistingFile, os32_CurrentHexFileIndex);
         C_UsHandler::h_GetInstance()->SetLastKnownUpdateHexFileLocation(rc_File);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get hex file paths as relative or absolute

   \return
   Hex file paths as relative or absolute
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_FlaUpListWidget::m_GetHexFilePathsAsRelativeOrAbsolute() const
{
   QStringList c_HexFilePathsAsRelativeOrAbsolute;

   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         const C_FlaUpListItemWidget * const pc_ItemWidget =
            dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
         if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
         {
            if (pc_ItemWidget->q_IsRelativePathToAdd == true)
            {
               c_HexFilePathsAsRelativeOrAbsolute.append("Relative");
            }
            else
            {
               c_HexFilePathsAsRelativeOrAbsolute.append("Absolute");
            }
         }
      }
   }

   return c_HexFilePathsAsRelativeOrAbsolute;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get suffix of file

   \param[in]  oc_FilePath    File to check

   \return   suffix of file
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_FlaUpListWidget::m_LastSuffixOfFileName(const QString & orc_File) const
{
   QString c_Result = "";
   const QFileInfo c_FileInfo(orc_File);
   const QString c_FileName = c_FileInfo.fileName();
   const int32_t s32_LastFoundIndex = c_FileName.lastIndexOf('.');

   if (s32_LastFoundIndex != -1)
   {
      c_Result = c_FileName.mid(s32_LastFoundIndex + 1).toLower();
   }
   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete item on delete button click

   \param[in,out]      os32_CurrentHexFileIndex           Current hex file widget index
   \param[in]          oq_NeedToUpdateListIndex           Need to update list indexes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_DeleteItem(const int32_t os32_CurrentHexFileIndex, const bool oq_NeedToUpdateListIndex)
{
   QListWidgetItem * const pc_CurrentItem = this->item(os32_CurrentHexFileIndex);

   if (pc_CurrentItem != NULL)
   {
      C_FlaUpListItemWidget * pc_ItemWidget =
         dynamic_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));

      if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
      {
         if (oq_NeedToUpdateListIndex)
         {
            C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);

            c_MessageBox.SetHeading(C_GtGetText::h_GetText("Remove file"));
            c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to remove ") +
                                        pc_ItemWidget->pc_HexFileInfo->c_HexFileInfo.c_FileName +
                                        C_GtGetText::h_GetText(" from the Hex-Files list?"));
            c_MessageBox.SetOkButtonText("Remove");
            c_MessageBox.SetNoButtonText("Keep");
            c_MessageBox.SetCustomMinHeight(180, 180);
            if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
            {
               pc_ItemWidget = dynamic_cast<C_FlaUpListItemWidget *>(this->takeItem(os32_CurrentHexFileIndex));
               delete pc_ItemWidget;
               Q_EMIT (this->SigUpdateFileCounter());
               m_UpdateHexFileWidgetIndex(os32_CurrentHexFileIndex);
            }
         }
         else
         {
            pc_ItemWidget = dynamic_cast<C_FlaUpListItemWidget *>(this->takeItem(os32_CurrentHexFileIndex));
            delete pc_ItemWidget;
         }
         Q_EMIT (this->SigFileRemoved());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete  all item from the list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_DeleteAllItem()
{
   C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);

   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Remove all File"));
   c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to remove all files from the Hex-Files list?"));
   c_MessageBox.SetOkButtonText("Remove All");
   c_MessageBox.SetNoButtonText("Keep");
   c_MessageBox.SetCustomMinHeight(180, 180);
   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
   {
      this->clear();
      Q_EMIT (this->SigUpdateFileCounter());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open file explorer with file location as default

   \param[in]  os32_CurrentHexFileIndex      Current hex file widget index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_SelectFile(const int32_t os32_CurrentHexFileIndex)
{
   this->AddFileAction(true, os32_CurrentHexFileIndex, true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload current hex file on status change

   \param[in]  orc_File                      Path of hex file
   \param[in]  opc_HexFileInfo               hex file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_ReloadCurrentFile(const QString & orc_File, C_FlaUpHexFileInfo * const opc_HexFileInfo)
{
   bool q_HexFile = false;

   if (!orc_File.isEmpty())
   {
      q_HexFile = m_IsFileHexFile(orc_File);
   }

   if (q_HexFile)
   {
      opc_HexFileInfo->SetHexFileInfo(orc_File);
   }
}
