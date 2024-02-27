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
#include "C_FlaUpHexFileInfo.hpp"
#include "C_UsHandler.hpp"
#include "C_FlaUpHexFileInfo.hpp"

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
 *  \param[in,out]  opc_Parent    parent widget
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpListWidget::C_FlaUpListWidget(QWidget * const opc_Parent) :
   QListWidget(opc_Parent)
{
   this->setFrameShape(QFrame::NoFrame);
   this->m_LoadLastKnownHexFilePaths();
   this->setSelectionMode(QAbstractItemView::MultiSelection);
   this->setDragDropMode(QAbstractItemView::DropOnly);
   this->setAcceptDrops(true);
   this->setMouseTracking(true);
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add File action dialog heading and filter
 *
 * \param[in]  oq_IsActionForExistingFile    Is action for existing file
 * \param[in]  os32_CurrentHexFileIndex      Current hex file widget index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::AddFileAction(const bool oq_IsActionForExistingFile, const int32_t os32_CurrentHexFileIndex)
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
            qobject_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));

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

   this->AddNewFile(C_GtGetText::h_GetText("Select HEX file"),
                    static_cast<QString>(C_GtGetText::h_GetText("Hex file")) +
                    " (*.hex)", c_Folder, oq_IsActionForExistingFile, os32_CurrentHexFileIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adding a new hex file

   \param[in]  orc_DialogCaption                  Caption for file dialog
   \param[in]  orc_DialogFilter                   Filter configuration for file dialog
   \param[in]  oc_Folder                          Folder of hex file
   \param[in]  oq_IsActionForExistingFile         Is Action for existing files
   \param[in, out]  os32_CurrentHexFileIndex      Current hex file widget index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::AddNewFile(const QString & orc_DialogCaption, const QString & orc_DialogFilter,
                                   const QString oc_Folder, const bool oq_IsActionForExistingFile,
                                   int32_t os32_CurrentHexFileIndex)
{
   QStringList c_Files = QFileDialog::getOpenFileNames(this, orc_DialogCaption, oc_Folder, orc_DialogFilter);

   if (c_Files.isEmpty() == false)
   {
      for (int32_t s32_Pos = 0; s32_Pos < c_Files.size(); ++s32_Pos)
      {
         const QFileInfo c_File(c_Files[s32_Pos]);
         const bool q_HexFile = c_File.completeSuffix().toLower() == "hex";

         if (q_HexFile == false)
         {
            const QString c_Details =
               static_cast<QString>(C_GtGetText::h_GetText("File path: %1")).arg(c_Files[s32_Pos]);
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("Update Package Configuration"));
            c_Message.SetDescription(C_GtGetText::h_GetText(
                                        "Invalid file: only *.hex files are allowed."));
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
            this->m_AddNewFile(c_Files[s32_Pos], q_HexFile, oq_IsActionForExistingFile, os32_CurrentHexFileIndex);
            C_UsHandler::h_GetInstance()->SetLastKnownUpdateHexFileLocation(c_Files[s32_Pos]);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete item on delete button click
 *
 *  \param[in, out]  os32_CurrentHexFileIndex      Current hex file widget index
 *  \param[in]  oq_NeedToUpdateListIndex           Need to update list indexes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::DeleteItem(const int32_t os32_CurrentHexFileIndex, const bool oq_NeedToUpdateListIndex)
{
   QListWidgetItem * const pc_CurrentItem = this->item(os32_CurrentHexFileIndex);

   if (pc_CurrentItem != NULL)
   {
      C_FlaUpListItemWidget * const pc_ItemWidget =
         qobject_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));

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
               this->takeItem(os32_CurrentHexFileIndex);
               Q_EMIT (this->SigUpdateFileCounter());
               m_UpdateHexFileWidgetIndex(os32_CurrentHexFileIndex);
            }
         }
         else
         {
            this->takeItem(os32_CurrentHexFileIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open file explorer with file location as default
 *
 *  \param[in]  os32_CurrentHexFileIndex      Current hex file widget index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::SelectFile(const int32_t os32_CurrentHexFileIndex)
{
   this->AddFileAction(true, os32_CurrentHexFileIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known hex files to user settings.ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::SetLastKnownHexFilePaths()
{
   C_UsHandler::h_GetInstance()->SetLastKnownUpdateHexFilePaths(this->GetHexFilePaths());
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
            qobject_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
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
            qobject_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
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
         C_FlaUpListItemWidget * const pc_ItemWidget =
            qobject_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
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
   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         C_FlaUpListItemWidget * const pc_ItemWidget =
            qobject_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
         if (pc_ItemWidget != NULL)
         {
            pc_ItemWidget->EnableSettings(oq_Enabled);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload current hex file on status change

   \param[in]  orc_File                      Path of hex file
   \param[in]  os32_CurrentHexFileIndex      Current hex file index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::ReloadCurrentFile(const QString & orc_File, const int32_t os32_CurrentHexFileIndex)
{
   bool q_HexFile = false;

   if (!orc_File.isEmpty())
   {
      q_HexFile = mh_IsFileHexFile(orc_File);
   }

   if (q_HexFile)
   {
      C_FlaUpHexFileInfo * const pc_HexFileInfo = new C_FlaUpHexFileInfo(orc_File);
      pc_HexFileInfo->c_HexFileInfo.c_FilePath = orc_File;
      pc_HexFileInfo->s32_CurrentHexFileIndex = os32_CurrentHexFileIndex;
      C_FlaUpListItemWidget * const pc_FileWidget = new C_FlaUpListItemWidget(this, pc_HexFileInfo);
      QListWidgetItem * const pc_Item = new QListWidgetItem();
      pc_Item->setSizeHint(QSize(pc_FileWidget->width(), pc_FileWidget->height()));
      this->DeleteItem(os32_CurrentHexFileIndex, false);
      this->insertItem(os32_CurrentHexFileIndex, pc_Item);
      this->setItemWidget(pc_Item, pc_FileWidget);
      this->show();
   } //lint !e429  //no memory leak of pc_HexFileInfo, pc_FileWidget, pc_Item because of handling in
     // "this->DeleteItem()"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden drag enter event slot

   Here accepts MimeData has hasUrls

   \param[in,out]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::dragEnterEvent(QDragEnterEvent * const opc_Event)
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
/*! \brief  Overridden drag move event slot

   Here accepts MimeData has hasUrls

   \param[in,out]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::dragMoveEvent(QDragMoveEvent * const opc_Event)
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
/*! \brief  Overridden drop event slot

   Here accepts MimeData has hasUrls

   \param[in,out]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::dropEvent(QDropEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   if (pc_MimeData->hasUrls())
   {
      const QList<QUrl>  c_Urls = pc_MimeData->urls();
      int32_t s32_DroppedItemAtIndex =
         static_cast<int32_t>(std::ceil(static_cast<float64_t>(opc_Event->pos().y()) / 55.0));
      for (int32_t s32_It = 0; s32_It < c_Urls.size(); ++s32_It)
      {
         const QUrl & rc_Url = c_Urls.at(s32_It);
         if (rc_Url.isLocalFile() && rc_Url.toString().endsWith(".hex"))
         {
            opc_Event->setAccepted(true);
            if (s32_DroppedItemAtIndex <= this->count())
            {
               QListWidgetItem * const pc_CurrentItem = this->item(s32_DroppedItemAtIndex - 1);
               if (pc_CurrentItem != NULL)
               {
                  C_FlaUpListItemWidget * const pc_ItemWidget =
                     qobject_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
                  if ((pc_ItemWidget != NULL) && (pc_ItemWidget->pc_HexFileInfo != NULL))
                  {
                     if ((c_Urls.size() > 1) && (s32_It == (c_Urls.size() - 1)))
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
                     else if (c_Urls.size() <= 1)
                     {
                        C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);

                        c_MessageBox.SetHeading(C_GtGetText::h_GetText("Replace file"));
                        c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                                       "Do you really want to replace ") + pc_ItemWidget->pc_HexFileInfo->c_HexFileInfo.c_FileName +
                                                    C_GtGetText::h_GetText(" with ") + rc_Url.fileName());
                        c_MessageBox.SetOkButtonText("Replace");
                        c_MessageBox.SetNoButtonText("Cancel");
                        c_MessageBox.SetCustomMinHeight(180, 180);
                        if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
                        {
                           this->m_AddFile(rc_Url.toLocalFile(), true, s32_DroppedItemAtIndex - 1);
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
               this->m_AddFile(rc_Url.toLocalFile(), false, this->count());
               ++s32_DroppedItemAtIndex;
            }
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
      this->m_AddFile(orc_File, oq_IsActionForExistingFile, os32_CurrentHexFileIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare all files in sections data blocks and files with input file.

   If file is already contained in update package, an information message is shown.

   \param[in]  orc_File    Input file

   \retval   true    File is already contained
   \retval   false   File is new
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_FlaUpListWidget::m_CheckFileAlreadyContained(const QString & orc_File) const
{
   if (!orc_File.isEmpty())
   {
   }
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds a new file for the file container

   Only possible for file based devices

   \param[in]  orc_File    New path
   \param[in]  oq_IsActionForExistingFile    ActionForExistingFile
   \param[in]  os32_CurrentHexFileIndex      Current hex file index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListWidget::m_AddFile(const QString & orc_File, const bool oq_IsActionForExistingFile,
                                  const int32_t os32_CurrentHexFileIndex)
{
   bool q_HexFile = false;

   if (!orc_File.isEmpty())
   {
      q_HexFile = mh_IsFileHexFile(orc_File);
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
               qobject_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
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
         pc_HexFileInfo->c_HexFileInfo.c_FilePath = orc_File;
         pc_HexFileInfo->s32_CurrentHexFileIndex = os32_CurrentHexFileIndex;
         C_FlaUpListItemWidget * const pc_FileWidget = new C_FlaUpListItemWidget(this, pc_HexFileInfo);
         QListWidgetItem * const pc_Item = new QListWidgetItem();
         pc_Item->setSizeHint(QSize(pc_FileWidget->width(), pc_FileWidget->height()));
         if (!oq_IsActionForExistingFile)
         {
            this->addItem(pc_Item);
            Q_EMIT (this->SigUpdateFileCounter());
         }
         else
         {
            this->DeleteItem(os32_CurrentHexFileIndex, false);
            this->insertItem(os32_CurrentHexFileIndex, pc_Item);
         }
         this->setItemWidget(pc_Item, pc_FileWidget);
         this->show();
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
bool C_FlaUpListWidget::mh_IsFileHexFile(const QString & orc_File)
{
   bool q_HexFile = false;

   if (!orc_File.isEmpty())
   {
      const QFileInfo c_File(orc_File);

      const QString c_FileExtension = "hex";

      if (c_File.completeSuffix().toLower() == c_FileExtension)
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
   QStringList orc_HexFilePaths = C_UsHandler::h_GetInstance()->GetLastKnownUpdateHexFilePaths();

   for (int32_t s32_It = 0; s32_It < static_cast<int32_t>(orc_HexFilePaths.size()); s32_It++)
   {
      this->m_AddFile(orc_HexFilePaths[s32_It], false, s32_It);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updating index for Hex file widget
 *
 *  \param[in]  os32_CurrentHexFileIndex      Current hex file widget index
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
            qobject_cast<C_FlaUpListItemWidget *>(this->itemWidget(pc_CurrentItem));
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
