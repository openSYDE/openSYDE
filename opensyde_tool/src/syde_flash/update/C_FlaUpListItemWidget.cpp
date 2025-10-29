//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for list item

   Widget for list item

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QProcess>
#include <QFileDialog>
#include <cmath>

#include "C_FlaUpListItemWidget.hpp"
#include "ui_C_FlaUpListItemWidget.h"
#include "C_Uti.hpp"
#include "C_GtGetText.hpp"
#include "C_FlaUpHexFileView.hpp"
#include "C_FlaUpHexFileInfo.hpp"
#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent          Optional pointer to parent
   \param[in,out] opc_HexFileInfo     Current hex file Info
   \param[in,out] oq_IsRelativePath   Is to add path as relative or absolute
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpListItemWidget::C_FlaUpListItemWidget(QListWidget * const opc_Parent, C_FlaUpHexFileInfo * const opc_HexFileInfo,
                                             const bool oq_IsRelativePath) :
   C_OgeWiOnlyBackground(opc_Parent),
   pc_HexFileInfo(opc_HexFileInfo),
   q_IsRelativePathToAdd(oq_IsRelativePath),
   mpc_Ui(new Ui::C_FlaUpListItemWidget),
   mpc_ContextMenu(nullptr),
   ms32_ApplicationBlockIndex(0),
   mpc_FileStatusChecktimer(nullptr),
   mpc_Movie(new QMovie(":/images/UpdateAnimationInProgress.gif", QByteArray(), this))
{
   const QSize c_DEFAULT_SIZE = QSize(20, 20);
   const QString c_StatusIconPath = ":/images/IconUpdateWaiting.svg";

   mpc_FileStatusChecktimer = new QTimer(this);

   this->mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_DeleteButton->SetSvg("://images/IconClear.svg", "://images/IconClearGrayDisabled.svg",
                                         "://images/IconClearHover.svg", "", "", "",
                                         "://images/IconClearPressed.svg");
   this->mpc_Ui->pc_DeleteButton->SetToolTipInformation(C_GtGetText::h_GetText("Delete file"),
                                                        C_GtGetText::h_GetText(
                                                           "Remove HEX file from list"));
   this->mpc_Ui->pc_BrowseButton->SetSvg("://images/IconBrowse.svg", "://images/IconBrowseDisabled.svg",
                                         "://images/IconBrowseHovered.svg", "", "", "",
                                         "://images/IconBrowsePressed.svg");
   this->mpc_Ui->pc_BrowseButton->SetToolTipInformation(C_GtGetText::h_GetText("Browse file"),
                                                        C_GtGetText::h_GetText(
                                                           "Select HEX file (.*hex)"));
   this->mpc_Ui->pc_FilePathLabel->SetFontPixel(14, true, false);
   this->mpc_Ui->pc_FilePathLabel->SetForegroundColor(6);
   this->mpc_Ui->pc_FilepathWidget->SetBackgroundColor(11);
   this->mpc_Ui->pc_DeviceIdWidget->SetBackgroundColor(0);
   this->mpc_Ui->pc_DeviceIDLabel->SetFontPixel(10);
   this->mpc_Ui->pc_DeviceIDLabel->SetForegroundColor(0);
   this->mpc_Ui->pc_DeviceIDLabel->SetBackgroundColor(9);

   this->mpc_Ui->pc_ProjectNameLabel->SetFontPixel(10);
   this->mpc_Ui->pc_ProjectNameLabel->SetForegroundColor(0);
   this->mpc_Ui->pc_ProjectNameLabel->SetBackgroundColor(9);

   this->mpc_Ui->pc_ProjectVersionLabel->SetFontPixel(10);
   this->mpc_Ui->pc_ProjectVersionLabel->SetForegroundColor(0);
   this->mpc_Ui->pc_ProjectVersionLabel->SetBackgroundColor(9);

   stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Separator,
                                                                   "HasColor9Background", true);

   this->m_GetHexFileInformation();
   this->m_SetupContextMenu();

   this->m_GetAbsoluteAndRelativePaths(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath, C_Uti::h_GetExePath());

   connect(this->mpc_Ui->pc_DeleteButton, &QPushButton::clicked, this, &C_FlaUpListItemWidget::m_DeleteItem);
   connect(this->mpc_Ui->pc_BrowseButton, &QPushButton::clicked, this, &C_FlaUpListItemWidget::m_SelectFile);

   connect(mpc_FileStatusChecktimer, &QTimer::timeout, this, &C_FlaUpListItemWidget::m_FileStatusCheck);
   mpc_FileStatusChecktimer->start(10);

   m_SetSizePolicy();

   this->mpc_Movie->setScaledSize(c_DEFAULT_SIZE);
   this->mpc_Ui->pc_StatusIcon->setPixmap(QIcon(c_StatusIconPath).pixmap(c_DEFAULT_SIZE));
} //lint !e818

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Movie and the Qt memory management
C_FlaUpListItemWidget::~C_FlaUpListItemWidget()
{
   delete this->mpc_FileStatusChecktimer;
   delete pc_HexFileInfo;
   delete this->mpc_ContextMenu;
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get Hex File Information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_GetHexFileInformation(void)
{
   const QFileInfo c_FileInfo(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath);

   if (c_FileInfo.exists() == true)
   {
      if (this->q_IsRelativePathToAdd)
      {
         this->mpc_Ui->pc_FilePathLabel->setText(C_Uti::h_MinimizePath(this->mc_CurrentRelativePath,
                                                                       this->mpc_Ui->pc_FilePathLabel->font(),
                                                                       static_cast<uint32_t>(this->width()) -
                                                                       mhs32_TITLE_OFFSET, 0U));
      }
      else
      {
         this->mpc_Ui->pc_FilePathLabel->setText(C_Uti::h_MinimizePath(this->mc_CurrentAbsolutePath,
                                                                       this->mpc_Ui->pc_FilePathLabel->font(),
                                                                       static_cast<uint32_t>(this->width()) -
                                                                       mhs32_TITLE_OFFSET, 0U));
      }
      if (pc_HexFileInfo->c_HexFileInfo.c_TimeStamp != "No information available")
      {
         for (int32_t s32_It = 0; s32_It < static_cast<int32_t>(pc_HexFileInfo->c_BlockInfo.size()); s32_It++)
         {
            if (pc_HexFileInfo->c_BlockInfo.at(s32_It).c_BlockType == "Application")
            {
               QString c_Content;

               this->ms32_ApplicationBlockIndex = s32_It;
               c_Content = this->m_UpdateToolTipContent(this->ms32_ApplicationBlockIndex);

               this->mpc_Ui->pc_DeviceIDLabel->setText(pc_HexFileInfo->c_BlockInfo.at(
                                                          this->ms32_ApplicationBlockIndex).c_DeviceId);
               this->mpc_Ui->pc_ProjectNameLabel->setText(pc_HexFileInfo->c_BlockInfo.at(
                                                             this->ms32_ApplicationBlockIndex).c_ProjectName);
               this->mpc_Ui->pc_ProjectVersionLabel->setText(pc_HexFileInfo->c_BlockInfo.at(this->
                                                                                            ms32_ApplicationBlockIndex).c_ProjectVersion);
               this->SetToolTipInformation(pc_HexFileInfo->c_BlockInfo.at(
                                              this->ms32_ApplicationBlockIndex).c_BlockType, c_Content);
            }
         }
      }
      else
      {
         this->mpc_Ui->pc_DeviceIDLabel->SetBackgroundColor(18);
         this->mpc_Ui->pc_DeviceIDLabel->setText("Invalid");
         this->mpc_Ui->pc_ProjectNameLabel->setVisible(false);
         this->mpc_Ui->pc_ProjectVersionLabel->setVisible(false);

         this->SetToolTipInformation("Invalid", "No information available");
      }
   }
   else
   {
      this->mpc_Ui->pc_DeviceIDLabel->SetBackgroundColor(18);
      this->mpc_Ui->pc_DeviceIDLabel->setText("Missing");
      this->mpc_Ui->pc_ProjectNameLabel->setVisible(false);
      this->mpc_Ui->pc_ProjectVersionLabel->setVisible(false);

      this->mpc_Ui->pc_DeviceIdWidget->SetToolTipInformation("Missing",
                                                             "Cannot able to find file");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get index of application block of HEX file kept by this item

   \return
   Application block index
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaUpListItemWidget::GetApplicationBlockIndex(void) const
{
   return ms32_ApplicationBlockIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get information if the file is existing and valid

   \retval   True    file exists and is valid
   \retval   False   file is missing or invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_FlaUpListItemWidget::IsFileExistingAndValid() const
{
   bool q_Return = true;

   if (this->pc_HexFileInfo != NULL)
   {
      if ((QFile::exists(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath) == false) ||
          (pc_HexFileInfo->c_BlockInfo.size() == 0))
      {
         q_Return = false;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enable/disable settings widgets

   \param[in]  oq_Enabled  true: enable, false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::EnableSettings(const bool oq_Enabled)
{
   this->mpc_Ui->pc_DeleteButton->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_BrowseButton->setEnabled(oq_Enabled);
   this->mpc_ContextMenu->setEnabled(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the state of the application

   The icon will be adapted

   \param[in]  ou32_State  State of application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::SetState(const uint32_t ou32_State)
{
   const QSize c_Size = this->mpc_Ui->pc_StatusIcon->size();

   // Stop the GIF
   disconnect(this->mpc_Movie, &QMovie::finished, this, &C_FlaUpListItemWidget::m_RestartMovie);
   this->mpc_Movie->stop();

   switch (ou32_State)
   {
   case C_FlaUpListItemWidget::hu32_STATE_DEFAULT:
      this->mpc_Ui->pc_StatusIcon->setPixmap(QIcon(":/images/IconUpdateWaiting.svg").pixmap(c_Size));
      break;
   case C_FlaUpListItemWidget::hu32_STATE_ERROR:
      this->mpc_Ui->pc_StatusIcon->setPixmap(QIcon(":/images/IconUpdateError.svg").pixmap(c_Size));
      break;
   case C_FlaUpListItemWidget::hu32_STATE_FINISHED:
      this->mpc_Ui->pc_StatusIcon->setPixmap(QIcon(":/images/IconUpdateSuccess.svg").pixmap(c_Size));
      break;
   case C_FlaUpListItemWidget::hu32_STATE_IN_PROGRESS:
      this->mpc_Ui->pc_StatusIcon->setMovie(this->mpc_Movie);
      connect(this->mpc_Movie, &QMovie::finished, this, &C_FlaUpListItemWidget::m_RestartMovie);
      this->mpc_Movie->start();
      break;
   case C_FlaUpListItemWidget::hu32_STATE_TO_DO:
      this->mpc_Ui->pc_StatusIcon->setPixmap(QIcon(":/images/IconUpdateWaiting.svg").pixmap(c_Size));
      break;
   default:
      this->mpc_Ui->pc_StatusIcon->setPixmap(QIcon(":/images/IconUpdateWaiting.svg").pixmap(c_Size));
      break;
   }
   this->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creating context menu with required actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_SetupContextMenu()
{
   mpc_ContextMenu = new stw::opensyde_gui_elements::C_OgeContextMenu(this);
   this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Select File"), this,  &C_FlaUpListItemWidget::m_SelectFile);
   this->mpc_ContextMenu->addSeparator();
   this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("View File Information"), this, &C_FlaUpListItemWidget::m_ViewFileInfo);
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Show in Explorer"), this, &C_FlaUpListItemWidget::m_ShowInExplorer);
   this->mpc_ContextMenu->addSeparator();
   this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Remove File"), this, &C_FlaUpListItemWidget::m_DeleteItem);
   this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Remove all Files"), this, &C_FlaUpListItemWidget::m_DeleteAllItems);
   this->setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this, &C_FlaUpListItemWidget::customContextMenuRequested, this,
           &C_FlaUpListItemWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  how custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   if (this->mpc_ContextMenu != NULL)
   {
      this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show file information of selected file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_ViewFileInfo()
{
   if (this->pc_HexFileInfo != NULL)
   {
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);

      c_New->SetSize(QSize(1000, 761));

      const C_FlaUpHexFileView * const pc_HexViewFileDialog = new C_FlaUpHexFileView(*c_New, *pc_HexFileInfo);

      Q_UNUSED(pc_HexViewFileDialog)
      if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         // do nothing
      }

      if (c_New != NULL)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
      delete pc_HexViewFileDialog;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show file in explorer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_ShowInExplorer() const
{
   if (this->pc_HexFileInfo != NULL)
   {
      const QFileInfo c_FileInfo(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath);

      if (c_FileInfo.exists() == true)
      {
         QStringList c_Param;

         if (c_FileInfo.isDir() == false)
         {
            c_Param += QLatin1String("/select,");
         }
         c_Param += QDir::toNativeSeparators(c_FileInfo.canonicalFilePath());
         QProcess::startDetached("explorer.exe", c_Param);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updating tooltip content

   \param[in]       oc_Index     Index of application block in hex file

   \retval   c_Content   contect for tooltip
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_FlaUpListItemWidget::m_UpdateToolTipContent(const int32_t os32_Index)
{
   QString c_Content = "";

   if (this->pc_HexFileInfo != NULL)
   {
      const uint32_t u32_FileSize =
         static_cast<uint32_t>(this->pc_HexFileInfo->c_HexFileInfo.c_Size.split(' ').at(0).toUInt());
      const float64_t f64_FileSize = std::ceil(static_cast<float64_t>(u32_FileSize) / 1024.0);

      c_Content += C_GtGetText::h_GetText("Project Name: ");
      c_Content += this->pc_HexFileInfo->c_BlockInfo.at(os32_Index).c_ProjectName;

      c_Content += C_GtGetText::h_GetText("\nTimestamp: ");
      c_Content += this->pc_HexFileInfo->c_BlockInfo.at(os32_Index).c_Date + " " + this->pc_HexFileInfo->c_BlockInfo.at(
         os32_Index).c_Time;

      c_Content += C_GtGetText::h_GetText("\nVersion: ");
      c_Content += this->pc_HexFileInfo->c_BlockInfo.at(os32_Index).c_ProjectVersion;

      c_Content += C_GtGetText::h_GetText("\nFile path origin: Defined by User");

      c_Content += C_GtGetText::h_GetText("\nFile path: ");
      c_Content += this->pc_HexFileInfo->c_HexFileInfo.c_FileName;

      c_Content += C_GtGetText::h_GetText("\nFile path absolute: ");
      c_Content += this->pc_HexFileInfo->c_HexFileInfo.c_FilePath;

      c_Content += C_GtGetText::h_GetText("\nFile size: ");

      c_Content += QString::number(f64_FileSize);
      c_Content += C_GtGetText::h_GetText(" kB");
   }

   return c_Content;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  emit signal to delete item widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_DeleteItem()
{
   Q_EMIT this->SigDeleteItemWidget(pc_HexFileInfo->s32_CurrentHexFileIndex, true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  emit signal to delete all items from widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_DeleteAllItems()
{
   Q_EMIT this->SigDeleteAllItems();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  emit signal to show file location in explorer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_SelectFile()
{
   Q_EMIT this->SigSelectFile(pc_HexFileInfo->s32_CurrentHexFileIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checking file status for every 100 ms
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_FileStatusCheck()
{
   if (this->pc_HexFileInfo != NULL)
   {
      const QFileInfo c_FileInfo(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath);

      if (c_FileInfo.exists() == false)
      {
         if (this->mpc_Ui->pc_DeviceIDLabel->text() != "Missing")
         {
            Q_EMIT this->SigReloadCurrentFile(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath, this->pc_HexFileInfo);
            this->mpc_Ui->pc_DeviceIDLabel->SetBackgroundColor(18);
            this->mpc_Ui->pc_DeviceIDLabel->setText("Missing");
            this->mpc_Ui->pc_ProjectNameLabel->setVisible(false);
            this->mpc_Ui->pc_ProjectVersionLabel->setVisible(false);
         }
      }
      else
      {
         if (this->pc_HexFileInfo->c_HexFileInfo.s32_NumberOfBlocks > 0)
         {
            if (this->mpc_Ui->pc_DeviceIDLabel->text() != this->pc_HexFileInfo->c_BlockInfo.at(
                   this->ms32_ApplicationBlockIndex).c_DeviceId)
            {
               Q_EMIT this->SigReloadCurrentFile(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath, this->pc_HexFileInfo);
               this->mpc_Ui->pc_DeviceIDLabel->SetBackgroundColor(9);
               this->mpc_Ui->pc_ProjectNameLabel->setVisible(true);
               this->mpc_Ui->pc_ProjectVersionLabel->setVisible(true);
               this->mpc_Ui->pc_DeviceIDLabel->setText(this->pc_HexFileInfo->c_BlockInfo.at(
                                                          this->ms32_ApplicationBlockIndex).c_DeviceId);
               this->mpc_Ui->pc_ProjectNameLabel->setText(this->pc_HexFileInfo->c_BlockInfo.at(
                                                             this->ms32_ApplicationBlockIndex).c_ProjectName);
               this->mpc_Ui->pc_ProjectVersionLabel->setText(this->pc_HexFileInfo->c_BlockInfo.at(this->
                                                                                                  ms32_ApplicationBlockIndex).c_ProjectVersion);
            }
         }
         else
         {
            if (this->mpc_Ui->pc_DeviceIDLabel->text() != "Invalid")
            {
               Q_EMIT this->SigReloadCurrentFile(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath, this->pc_HexFileInfo);
               this->mpc_Ui->pc_DeviceIDLabel->SetBackgroundColor(18);
               this->mpc_Ui->pc_DeviceIDLabel->setText("Invalid");
               this->mpc_Ui->pc_ProjectNameLabel->setVisible(false);
               this->mpc_Ui->pc_ProjectVersionLabel->setVisible(false);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   int32_t s32_Width = this->width();

   if (this->pc_HexFileInfo != NULL)
   {
      if (this->q_IsRelativePathToAdd)
      {
         this->mpc_Ui->pc_FilePathLabel->setText(C_Uti::h_MinimizePath(this->mc_CurrentRelativePath,
                                                                       this->mpc_Ui->pc_FilePathLabel->font(),
                                                                       static_cast<uint32_t>(s32_Width) -
                                                                       mhs32_TITLE_OFFSET,
                                                                       0U));
      }
      else
      {
         this->mpc_Ui->pc_FilePathLabel->setText(C_Uti::h_MinimizePath(this->mc_CurrentAbsolutePath,
                                                                       this->mpc_Ui->pc_FilePathLabel->font(),
                                                                       static_cast<uint32_t>(s32_Width) -
                                                                       mhs32_TITLE_OFFSET,
                                                                       0U));
      }
   }

   if (this->mpc_Ui->pc_DeviceIDLabel->isVisible() == true)
   {
      s32_Width -= (this->mpc_Ui->pc_DeviceIDLabel->width());
   }
   if (this->mpc_Ui->pc_ProjectNameLabel->isVisible() == true)
   {
      s32_Width -= (this->mpc_Ui->pc_ProjectNameLabel->width());
   }
   if (this->mpc_Ui->pc_ProjectVersionLabel->isVisible() == true)
   {
      s32_Width -= (this->mpc_Ui->pc_ProjectVersionLabel->width());
   }

   if (this->pc_HexFileInfo != NULL)
   {
      if ((this->pc_HexFileInfo->c_HexFileInfo.s32_NumberOfBlocks > 0) &&
          (this->mpc_Ui->pc_ProjectNameLabel->isVisible() == true))
      {
         this->mpc_Ui->pc_ProjectNameLabel->setText(C_Uti::h_MinimizePath(this->pc_HexFileInfo->c_BlockInfo.at(this->
                                                                                                               ms32_ApplicationBlockIndex)
                                                                          .c_ProjectName,
                                                                          this->mpc_Ui->pc_ProjectNameLabel->font(),
                                                                          static_cast<uint32_t>(s32_Width), 0U));
      }
   }

   QWidget::resizeEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set icon size policy
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_SetSizePolicy(void)
{
   QSizePolicy c_SizePolicy = this->mpc_Ui->pc_StatusIcon->sizePolicy();

   c_SizePolicy.setRetainSizeWhenHidden(true);

   this->mpc_Ui->pc_StatusIcon->setSizePolicy(c_SizePolicy);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restart animation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_RestartMovie(void)
{
   this->mpc_Movie->start();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get absolute or relative paths for hex files

   \param[in]       orc_Path                  file path
   \param[in]      orc_AbsoluteReferenceDir   Detailed output parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_GetAbsoluteAndRelativePaths(const QString & orc_Path,
                                                          const QString & orc_AbsoluteReferenceDir)
{
   C_Uti::h_IsPathRelativeToDir(orc_Path, orc_AbsoluteReferenceDir, this->mc_CurrentAbsolutePath,
                                this->mc_CurrentRelativePath);
}
