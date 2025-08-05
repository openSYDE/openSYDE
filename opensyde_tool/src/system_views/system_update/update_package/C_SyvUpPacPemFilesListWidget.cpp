//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for showing all pem files in secure authentication section

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QDropEvent>
#include <QMimeData>
#include <QScrollBar>

#include "stwtypes.hpp"
#include "C_SyvUpPacPemFilesListWidget.hpp"
#include "C_UsHandler.hpp"
#include "C_Uti.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SyvUpPacPemFileEntry.hpp"
#include "C_ImpUtil.hpp"

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

   Set up GUI with all elements.

   \param[in,out] opc_Parent          Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacPemFilesListWidget::C_SyvUpPacPemFilesListWidget(QWidget * const opc_Parent) :
   QListWidget(opc_Parent)
{
   this->setFrameShape(QFrame::NoFrame);
   this->setSelectionMode(QAbstractItemView::MultiSelection);
   this->setDragDropMode(QAbstractItemView::DropOnly);
   this->setMouseTracking(true);

   this->m_LoadLastKnownPemFilePaths();

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SyvUpPacPemFilesListWidget::m_ShowHideVerticalScrollBar);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add File action dialog heading and filter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::AddFileAction(void)
{
   QString c_Folder;

   c_Folder = C_UsHandler::h_GetInstance()->GetLastKnownAddPemFilePath();

   if (c_Folder.isEmpty())
   {
      c_Folder = C_Uti::h_GetExePath();
   }

   this->m_AddNewPemFile(C_GtGetText::h_GetText("Select PEM file"),
                         static_cast<QString>(C_GtGetText::h_GetText(
                                                 "PEM file")) + " (*.pem)", c_Folder);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known pem files to user settings.ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::SaveUserSettings()
{
   C_UsHandler::h_GetInstance()->SetLastKnownUpdatePemFilePaths(this->GetPemFilePaths());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete  all item from the list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::DeleteAllItems()
{
   C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);

   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Remove all Files"));
   c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to remove all files from the list?"));
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
/*! \brief  Get list of all PEM file paths

   \return
   PEM file paths
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_SyvUpPacPemFilesListWidget::GetPemFilePaths() const
{
   QStringList c_PemFilePaths;

   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         C_SyvUpPacPemFileEntry * const pc_ItemWidget =
            dynamic_cast<C_SyvUpPacPemFileEntry *>(this->itemWidget(pc_CurrentItem));
         if ((pc_ItemWidget != NULL) && (!pc_ItemWidget->c_FilePath.isEmpty()))
         {
            c_PemFilePaths.append(pc_ItemWidget->c_FilePath);
         }
      }
   }

   return c_PemFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   DropEvent slot
 *
   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::DropEvent(QDropEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   if (pc_MimeData->hasUrls())
   {
      const QList<QUrl>  c_Urls = pc_MimeData->urls();
      const int32_t s32_UrlsCount = c_Urls.size();
      for (int32_t s32_It = 0; s32_It < s32_UrlsCount; ++s32_It)
      {
         const QUrl & rc_Url = c_Urls.at(s32_It);

         if (rc_Url.isLocalFile() && rc_Url.toString().endsWith(".pem"))
         {
            opc_Event->setAccepted(true);
            const QString c_LocalFilePath = rc_Url.toLocalFile();
            this->m_AddFile(c_LocalFilePath, this->count());
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  DragEnterEvent slot
 *
   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::DragEnterEvent(QDragEnterEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   if (pc_MimeData->hasUrls())
   {
      const QList<QUrl> c_Urls = pc_MimeData->urls();
      for (int32_t s32_It = 0; s32_It < c_Urls.size(); ++s32_It)
      {
         const QUrl & rc_Url = c_Urls.at(s32_It);
         if (rc_Url.isLocalFile() && rc_Url.toString().endsWith(".pem"))
         {
            opc_Event->setAccepted(true);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  DragMoveEvent slot
 *
   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::DragMoveEvent(QDragMoveEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   if (pc_MimeData->hasUrls())
   {
      const QList<QUrl> c_Urls = pc_MimeData->urls();
      for (int32_t s32_It = 0; s32_It < c_Urls.size(); ++s32_It)
      {
         const QUrl & rc_Url = c_Urls.at(s32_It);
         if (rc_Url.isLocalFile() && rc_Url.toString().endsWith(".pem"))
         {
            opc_Event->setAccepted(true);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adding a new Pem files

   \param[in]      orc_DialogCaption                  Caption for file dialog
   \param[in]      orc_DialogFilter                   Filter configuration for file dialog
   \param[in]      orc_Folder                         Folder of pem file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::m_AddNewPemFile(const QString & orc_DialogCaption, const QString & orc_DialogFilter,
                                                   const QString & orc_Folder)
{
   QStringList c_InvalidFiles;
   const QStringList c_Files = QFileDialog::getOpenFileNames(this, orc_DialogCaption, orc_Folder, orc_DialogFilter);

   for (const QString & rc_File : c_Files)
   {
      const bool q_PemFile = (m_LastSuffixOfFileName(rc_File) == "pem");
      if (!q_PemFile)
      {
         c_InvalidFiles.append(static_cast<QString>("File path: %1").arg(rc_File));
      }
      else
      {
         this->m_AddFile(rc_File, this->count());
         C_UsHandler::h_GetInstance()->SetLastKnownAddPemFilePath(rc_File);
      }
   }
   if (!c_InvalidFiles.isEmpty())
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Update Package Configuration"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Invalid files: only *.pem files are allowed."));
      c_Message.SetDetails(c_InvalidFiles.join("\n"));
      c_Message.SetCustomMinHeight(180, 250);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get suffix of file

   \param[in]  orc_File    File to check

   \return   suffix of file
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacPemFilesListWidget::m_LastSuffixOfFileName(const QString & orc_File) const
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
/*! \brief  Adds a new file for the file container

   Only possible for file based devices

   \param[in]  orc_File                      New path
   \param[in]  os32_CurrentPemFileIndex      Current pem file index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::m_AddFile(const QString & orc_File, const int32_t os32_CurrentPemFileIndex)
{
   bool q_FileAlreadyExists = false;
   const QFileInfo c_NewFileInfo(orc_File);
   const QString c_AbsoluteNewFilePath = c_NewFileInfo.absoluteFilePath();

   for (int32_t s32_It = 0; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         const C_SyvUpPacPemFileEntry * const pc_ItemWidget =
            dynamic_cast<C_SyvUpPacPemFileEntry *>(this->itemWidget(pc_CurrentItem));
         if ((pc_ItemWidget != NULL) && (pc_ItemWidget->c_FilePath != NULL))
         {
            const QFileInfo c_ExistingFileInfo(pc_ItemWidget->c_FilePath);
            const QString c_AbsoluteExistingFilePath = c_ExistingFileInfo.absoluteFilePath();
            if (c_AbsoluteNewFilePath == c_AbsoluteExistingFilePath)
            {
               q_FileAlreadyExists = true;
            }
         }
      }
   }

   if (q_FileAlreadyExists == false)
   {
      QListWidgetItem * const pc_Item = new QListWidgetItem();
      C_SyvUpPacPemFileEntry * const pc_ItemWidget =
         new C_SyvUpPacPemFileEntry(this, orc_File, os32_CurrentPemFileIndex);

      pc_Item->setSizeHint(QSize(pc_ItemWidget->width(), pc_ItemWidget->height()));
      this->addItem(pc_Item);
      this->setItemWidget(pc_Item, pc_ItemWidget);
      this->show();
      Q_EMIT (this->SigUpdateFileCounter());

      connect(pc_ItemWidget, &C_SyvUpPacPemFileEntry::SigDeleteItemWidget, this,
              &C_SyvUpPacPemFilesListWidget::m_DeleteItem);
   } //lint !e429  //no memory leak of pc_ItemWidget, pc_Item because of handling in
   // "this->m_DeleteItem()"
   else
   {
      C_OgeWiCustomMessage c_Message(this);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add PEM file"));
      c_Message.SetDescription(
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "The file is already contained in the PEM file list "
                                 "and therefore not added again.")));
      c_Message.SetDetails(
         static_cast<QString>(C_GtGetText::h_GetText("%1")).arg(orc_File));
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load known pem file into update window
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::m_LoadLastKnownPemFilePaths()
{
   QStringList c_PemFilePaths = C_UsHandler::h_GetInstance()->GetLastKnownUpdatePemFilePaths();

   for (int32_t s32_It = 0; s32_It < static_cast<int32_t>(c_PemFilePaths.size()); s32_It++)
   {
      this->m_AddFile(c_PemFilePaths[s32_It], s32_It);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete item on delete button click

   \param[in]      os32_CurrentPemFileIndex           Current pem file widget index
   \param[in]          oq_NeedToUpdateListIndex           Need to update list indexes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::m_DeleteItem(const int32_t os32_CurrentPemFileIndex,
                                                const bool oq_NeedToUpdateListIndex)
{
   QListWidgetItem * const pc_CurrentItem = this->item(os32_CurrentPemFileIndex);

   if (pc_CurrentItem != NULL)
   {
      C_SyvUpPacPemFileEntry * pc_ItemWidget =
         dynamic_cast<C_SyvUpPacPemFileEntry *>(this->itemWidget(pc_CurrentItem));
      if (pc_ItemWidget != NULL)
      {
         if (oq_NeedToUpdateListIndex)
         {
            C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);

            c_MessageBox.SetHeading(C_GtGetText::h_GetText("Remove file"));
            c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to remove ") +
                                        pc_ItemWidget->c_FilePath +
                                        C_GtGetText::h_GetText(" from the PEM files list?"));
            c_MessageBox.SetOkButtonText("Remove");
            c_MessageBox.SetNoButtonText("Keep");
            c_MessageBox.SetCustomMinHeight(200, 200);
            if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
            {
               pc_ItemWidget = dynamic_cast<C_SyvUpPacPemFileEntry *>(this->takeItem(os32_CurrentPemFileIndex));
               delete pc_ItemWidget;
               Q_EMIT (this->SigUpdateFileCounter());
               m_UpdatePemFileWidgetIndex(os32_CurrentPemFileIndex);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updating index for PEM file widget

   \param[in]  os32_CurrentPemFileIndex      Current pem file widget index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::m_UpdatePemFileWidgetIndex(const int32_t os32_CurrentPemFileIndex) const
{
   for (int32_t s32_It = os32_CurrentPemFileIndex; s32_It < this->count(); ++s32_It)
   {
      QListWidgetItem * const pc_CurrentItem = this->item(s32_It);
      if (pc_CurrentItem != NULL)
      {
         C_SyvUpPacPemFileEntry * const pc_ItemWidget =
            dynamic_cast<C_SyvUpPacPemFileEntry *>(this->itemWidget(pc_CurrentItem));
         if (pc_ItemWidget != NULL)
         {
            if (pc_ItemWidget->s32_CurrentPemFileIndex <= 0)
            {
               pc_ItemWidget->s32_CurrentPemFileIndex = 0;
            }
            else
            {
               --pc_ItemWidget->s32_CurrentPemFileIndex;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  \brief  Show hide vertical scroll bar

   \param[in]  os32_Min  Minimum range
   \param[in]  os32_Max  Maximum range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesListWidget::m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const
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
