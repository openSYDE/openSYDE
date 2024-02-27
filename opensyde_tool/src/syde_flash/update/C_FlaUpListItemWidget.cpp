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

#include "C_FlaUpListItemWidget.hpp"
#include "ui_C_FlaUpListItemWidget.h"
#include "C_FlaUpListWidget.hpp"
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

   \param[in,out] opc_Parent        Optional pointer to parent
   \param[in,out] opc_HexFileInfo   Current hex file Info
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpListItemWidget::C_FlaUpListItemWidget(C_FlaUpListWidget * const opc_Parent,
                                             C_FlaUpHexFileInfo * const opc_HexFileInfo) :
   C_OgeWiOnlyBackground(opc_Parent),
   pc_HexFileInfo(opc_HexFileInfo),
   mpc_Ui(new Ui::C_FlaUpListItemWidget),
   mpc_ContextMenu(NULL),
   ms32_ApplicationBlockIndex(0),
   mpc_FileStatusChecktimer(NULL)
{
   QSizePolicy c_SizePolicy;
   const QSize c_SIZE(20, 20);

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

   this->GetHexFileInformation(opc_HexFileInfo);
   this->m_SetupContextMenu();

   //retain icon labe size when hidden
   c_SizePolicy = this->mpc_Ui->pc_StatusIcon->sizePolicy();
   c_SizePolicy.setRetainSizeWhenHidden(true);
   this->mpc_Ui->pc_StatusIcon->setSizePolicy(c_SizePolicy);
   this->mpc_Ui->pc_StatusIcon->setPixmap(QIcon("://images/IconUpdateWaiting.svg").pixmap(c_SIZE));

   connect(this->mpc_Ui->pc_DeleteButton, &QPushButton::clicked, this, [opc_Parent, opc_HexFileInfo] ()
   {
      opc_Parent->DeleteItem(opc_HexFileInfo->s32_CurrentHexFileIndex, true);
   }
           );

   connect(this, &C_FlaUpListItemWidget::SigDeleteItemWidget, this, [opc_Parent, opc_HexFileInfo] ()
   {
      opc_Parent->DeleteItem(opc_HexFileInfo->s32_CurrentHexFileIndex, true);
   }
           );

   connect(this->mpc_Ui->pc_BrowseButton, &QPushButton::clicked, this, [opc_Parent, opc_HexFileInfo] ()
   {
      opc_Parent->SelectFile(opc_HexFileInfo->s32_CurrentHexFileIndex);
   }
           );

   connect(this, &C_FlaUpListItemWidget::SigSelectFile, this, [opc_Parent, opc_HexFileInfo] ()
   {
      opc_Parent->SelectFile(opc_HexFileInfo->s32_CurrentHexFileIndex);
   }
           );

   connect(this, &C_FlaUpListItemWidget::SigReloadCurrentFile, this, [opc_Parent, opc_HexFileInfo] ()
   {
      opc_Parent->ReloadCurrentFile(opc_HexFileInfo->c_HexFileInfo.c_FilePath,
                                    opc_HexFileInfo->s32_CurrentHexFileIndex);
   }
           );

   connect(mpc_FileStatusChecktimer, &QTimer::timeout, this, &C_FlaUpListItemWidget::m_FileStatusCheck);
   mpc_FileStatusChecktimer->start(100);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpListItemWidget::~C_FlaUpListItemWidget()
{
   delete this->mpc_FileStatusChecktimer;
   delete pc_HexFileInfo;
   delete this->mpc_ContextMenu;
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get Hex File Information

   \param[in]  opc_HexFileInfo   Current hex file info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::GetHexFileInformation(C_FlaUpHexFileInfo * const opc_HexFileInfo)
{
   const QFileInfo & rc_FileInfo(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath);

   if (rc_FileInfo.exists())
   {
      this->mpc_Ui->pc_FilePathLabel->setText(C_Uti::h_AdaptStringToSize(opc_HexFileInfo->c_HexFileInfo.c_FilePath,
                                                                         this->mpc_Ui->pc_FilePathLabel->fontMetrics(),
                                                                         this->width() - mhs32_TITLE_OFFSET));
      if (opc_HexFileInfo->c_HexFileInfo.c_TimeStamp != "No information available")
      {
         for (int32_t s32_It = 0; s32_It < static_cast<int32_t>(opc_HexFileInfo->c_BlockInfo.size()); s32_It++)
         {
            if (opc_HexFileInfo->c_BlockInfo.at(s32_It).c_BlockType == "Application")
            {
               this->ms32_ApplicationBlockIndex = s32_It;
               this->mpc_Ui->pc_DeviceIDLabel->setText(opc_HexFileInfo->c_BlockInfo.at(
                                                          this->ms32_ApplicationBlockIndex).c_DeviceId);
               this->mpc_Ui->pc_ProjectNameLabel->setText(opc_HexFileInfo->c_BlockInfo.at(
                                                             this->ms32_ApplicationBlockIndex).c_ProjectName);
               this->mpc_Ui->pc_ProjectVersionLabel->setText(opc_HexFileInfo->c_BlockInfo.at(this->
                                                                                             ms32_ApplicationBlockIndex).c_ProjectVersion);
               const QString c_Content = this->m_UpdateToolTipContent(this->ms32_ApplicationBlockIndex);
               this->SetToolTipInformation(opc_HexFileInfo->c_BlockInfo.at(
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
   this->mpc_Ui->pc_DeleteButton->setDisabled(!oq_Enabled);
   this->mpc_Ui->pc_BrowseButton->setDisabled(!oq_Enabled);
   this->mpc_ContextMenu->setDisabled(!oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creating context menu with required actions

   \param[in]  opc_Parent  parent widget of current widget
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

      c_Content += this->pc_HexFileInfo->c_HexFileInfo.c_Size;
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
   Q_EMIT this->SigDeleteItemWidget();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  emit signal to show file location in explorer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_SelectFile()
{
   Q_EMIT this->SigSelectFile();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checking file status for every 100 ms
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpListItemWidget::m_FileStatusCheck()
{
   if (this->pc_HexFileInfo != NULL)
   {
      const QFileInfo & rc_FileInfo(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath);

      if (!rc_FileInfo.exists())
      {
         if (this->mpc_Ui->pc_DeviceIDLabel->text() != "Missing")
         {
            Q_EMIT this->SigReloadCurrentFile();
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
               Q_EMIT this->SigReloadCurrentFile();
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
               Q_EMIT this->SigReloadCurrentFile();
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
   this->resize(this->parentWidget()->width(), 68);
   int32_t s32_Width = this->width();
   if (this->pc_HexFileInfo != NULL)
   {
      this->mpc_Ui->pc_FilePathLabel->setText(C_Uti::h_MinimizePath(this->pc_HexFileInfo->c_HexFileInfo.c_FilePath,
                                                                    this->mpc_Ui->pc_FilePathLabel->font(),
                                                                    static_cast<uint32_t>(s32_Width) -
                                                                    mhs32_TITLE_OFFSET,
                                                                    0U));
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
