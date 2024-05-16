//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for update properties

   Widget for update properties

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <QFileDialog>

#include "C_GtGetText.hpp"
#include "TglUtils.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiUtil.hpp"
#include "TglFile.hpp"
#include "C_UsHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_FlaUpProperties.hpp"
#include "C_FlaUpListItemWidget.hpp"

#include "ui_C_FlaUpProperties.h"

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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpProperties::C_FlaUpProperties(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_FlaUpProperties)
{
   this->mpc_Ui->setupUi(this);
   this->mpc_ScrollLayout = new QVBoxLayout(this->mpc_Ui->pc_ScrollAreaWidget);
   this->mpc_ScrollLayout->setContentsMargins(0, 0, 0, 0);
   this->mpc_ListWidget = new C_FlaUpListWidget(this->mpc_Ui->pc_ScrollAreaWidget);

   connect(this->mpc_ListWidget, &C_FlaUpListWidget::SigFileAdded, this, &C_FlaUpProperties::m_FileAdded);
   connect(this->mpc_ListWidget, &C_FlaUpListWidget::SigFileRemoved, this, &C_FlaUpProperties::m_FileAdded);

   this->mpc_ScrollLayout->addWidget(this->mpc_ListWidget);

   this->mpc_Ui->pc_WiTitle->SetTitle(C_GtGetText::h_GetText("Update"));
   this->mpc_Ui->pc_WiTitle->SetIconType(C_CamOgeWiSectionHeader::E_ButtonType::eNOBUTTON);
   this->mpc_Ui->pc_LabelTitle->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelTitle->SetFontPixel(12, true, false);
   this->mpc_Ui->pc_LabelTitle->setText(
      static_cast<QString>(C_GtGetText::h_GetText("<b>Hex-Files (%1)<\b>")).arg(QString::number(this->mpc_ListWidget->
                                                                                                count())));
   if (this->mpc_ListWidget->count() <= 0)
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(true);
      this->mpc_Ui->pc_Seperator->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(false);
      this->mpc_Ui->pc_Seperator->setVisible(true);
   }
   this->mpc_Ui->pc_AddButton->SetSvg("://images/IconAddEnabled.svg", "://images/IconAddDisabled.svg",
                                      "://images/IconAddEnabledHover.svg", "", "",
                                      "",
                                      "://images/IconAddEnabledPressed.svg");
   this->mpc_Ui->pc_AddButton->SetToolTipInformation(C_GtGetText::h_GetText("Add file"),
                                                     C_GtGetText::h_GetText(
                                                        "Add HEX file (.*hex)"));
   this->mpc_Ui->pc_FileIconButton->SetSvg("://images/IconApp.svg");

   stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator,
                                                                   "HasColor9Background", true);
   this->mpc_Ui->pc_ScrollAreaWidget->SetBackgroundColor(0);
   this->mpc_Ui->pc_LabelNoMessages->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelNoMessages->SetFontPixel(12, false, false);
   this->mpc_Ui->pc_LabelNoMessages->setText("No files, add any via the '+' button.");

   connect(this->mpc_Ui->pc_AddButton, &QPushButton::clicked, this, [this] ()
   {
      this->mpc_ListWidget->AddFileAction(false, this->mpc_ListWidget->count(), false);
      this->UpdateDataTransfer();
   }
           );

   connect(this->mpc_ListWidget, &C_FlaUpListWidget::SigUpdateFileCounter, this,
           &C_FlaUpProperties::m_UpdateLabelTitleWithFileCounter);

   this->setAcceptDrops(true);
   this->ResetSummary();
   this->UpdateDataTransfer();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpProperties::~C_FlaUpProperties()
{
   delete this->mpc_ScrollLayout;
   delete this->mpc_ListWidget;
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::SaveUserSettings() const
{
   if (this->mpc_ListWidget != NULL)
   {
      this->mpc_ListWidget->SetLastKnownHexFilePaths();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list of all HEX file paths

   \return
   HEX file paths
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_FlaUpProperties::GetHexFilePaths() const
{
   QStringList c_HexFilePaths;

   if (this->mpc_ListWidget != NULL)
   {
      c_HexFilePaths =  this->mpc_ListWidget->GetHexFilePaths();
   }
   return c_HexFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list of all devices in all HEX files

   \return
   Device names
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_FlaUpProperties::GetHexFileDeviceNames() const
{
   QStringList c_HexFilePaths;

   if (this->mpc_ListWidget != NULL)
   {
      c_HexFilePaths = this->mpc_ListWidget->GetHexFileDeviceNames();
   }
   return c_HexFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get information if all files are valid and existing

   \retval   True    all files are existing and valid
   \retval   False   at least one file is missing or invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_FlaUpProperties::AreAllFilesValid(void) const
{
   bool q_Result = false;

   if (this->mpc_ListWidget != NULL)
   {
      q_Result = this->mpc_ListWidget->AreAllFilesValid();
   }
   return q_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enable/disable settings widgets

   \param[in]  oq_Enabled  true: enable, false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::EnableSettings(const bool oq_Enabled)
{
   this->mpc_Ui->pc_AddButton->setEnabled(oq_Enabled);
   this->mpc_ListWidget->EnableSettings(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset the summary widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::ResetSummary(void)
{
   const uint8_t u8_HEADING_ICON_STATE = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_DEFAULT);

   const QString c_TEXT = "---";
   const QString c_ELAPSED_TIME_TEXT = "00:00:00";

   SetHeadingIcon(u8_HEADING_ICON_STATE);
   SetHeadingText(c_TEXT, u8_HEADING_ICON_STATE);
   SetElapsedTime(c_ELAPSED_TIME_TEXT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show progress bar text

   \param[in]  orc_Text               Text that will be presented in header
   \param[in]  oru8_ProgressState     State of the progress (hu32_STATE_DEFAULT - black,
                                                             hu32_STATE_FINISHED - green,
                                                             hu32_STATE_ERROR - red)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::SetHeadingText(const QString & orc_Text, const uint8_t & oru8_ProgressState) const
{
   this->mpc_Ui->pc_BottomBar->SetHeadingText(orc_Text, oru8_ProgressState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set Heading Icon

   \param[in]  oru8_ProgressState     Progress state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::SetHeadingIcon(const uint8_t & oru8_ProgressState) const
{
   this->mpc_Ui->pc_BottomBar->SetHeadingIcon(oru8_ProgressState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set progress bar color

   \param[in]  orq_Success    If oq_Success bar is green else red
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::SetProgressBarColor(const bool & orq_Success) const
{
   this->mpc_Ui->pc_BottomBar->SetProgressBarColor(orq_Success);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update progress bar

   \param[in]       oru32_ProgressInPercentage     Percentage (0% - 100%)
   \param[in]       oq_Finished                    Bool to define if it is finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::UpdateProgressBar(const uint32_t & oru32_ProgressInPercentage, const bool oq_Finished) const
{
   const uint16_t u16_ProgressInPercentage = static_cast<uint16_t>(oru32_ProgressInPercentage);

   this->mpc_Ui->pc_BottomBar->SetProgress(u16_ProgressInPercentage, oq_Finished);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update elapsed time label.

   \param[in]       orc_Text   Time string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::SetElapsedTime(const QString & orc_Text) const
{
   this->mpc_Ui->pc_BottomBar->SetElapsedTime(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update transfer data count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::UpdateDataTransfer(void) const
{
   QString c_Complete;
   QString c_Unit;
   QString c_DataTransfer;

   const float64_t f64_BYTE = 1024.0;
   const float64_t f64_OverallFilesSize = static_cast<float64_t>(this->mpc_ListWidget->GetTotalHexFileSize());

   if (f64_OverallFilesSize > (pow(f64_BYTE, 3.)))
   {
      //GB
      c_Unit = "GB";
      c_Complete = QString::number(f64_OverallFilesSize / (pow(f64_BYTE, 3.)), 'f', 1);
   }
   else if (f64_OverallFilesSize > (pow(f64_BYTE, 2.)))
   {
      //MB
      c_Unit = "MB";
      c_Complete = QString::number(f64_OverallFilesSize / (pow(f64_BYTE, 2.)), 'f', 1);
   }
   else if (f64_OverallFilesSize > f64_BYTE)
   {
      //kB
      c_Unit = "kB";
      c_Complete = QString::number(f64_OverallFilesSize / static_cast<float32_t>(f64_BYTE), 'f', 1);
   }
   else
   {
      //Bytes
      c_Unit = "B";
      if (f64_OverallFilesSize <= 0.0)
      {
         const uint8_t u8_OverallFilesSizeInteger = static_cast<uint8_t>(f64_OverallFilesSize);
         c_Complete = QString::number(u8_OverallFilesSizeInteger);
      }
      else
      {
         c_Complete = QString::number(f64_OverallFilesSize, 'n', 1);
      }
   }
   //Translation: 1:Flashed bytes, 2:File size, 3: File size unit
   c_DataTransfer = static_cast<QString>("0 %2/%1 %2").arg(c_Complete).arg(c_Unit);

   this->mpc_Ui->pc_BottomBar->SetDataTransfer(c_DataTransfer);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update transfer data count

   \param[in]    oru64_FlashedBytes    total number of written bytes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::UpdateDataTransfer(const uint64_t & oru64_FlashedBytes) const
{
   QString c_Progress;
   QString c_Complete;
   QString c_Unit;
   QString c_DataTransfer;

   const float64_t f64_BYTE = 1024.0;
   const float64_t f64_OverallFlashedBytes = static_cast<float64_t>(oru64_FlashedBytes);
   const float64_t f64_OverallFilesSize = static_cast<float64_t>(this->mpc_ListWidget->GetTotalHexFileSize());

   if (f64_OverallFilesSize > (pow(f64_BYTE, 3.)))
   {
      const float64_t f64_BytePowThree = pow(f64_BYTE, 3.);
      //GB
      c_Unit = "GB";
      c_Progress = QString::number(f64_OverallFlashedBytes / (f64_BytePowThree), 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize / (f64_BytePowThree), 'f', 1);
   }
   else if (f64_OverallFilesSize > (pow(f64_BYTE, 2.)))
   {
      const float64_t f64_BytePowTwo = pow(f64_BYTE, 2.);

      //MB
      c_Unit = "MB";
      c_Progress = QString::number(f64_OverallFlashedBytes / (f64_BytePowTwo), 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize / (f64_BytePowTwo), 'f', 1);
   }
   else if (f64_OverallFilesSize > f64_BYTE)
   {
      //kB
      c_Unit = "kB";
      c_Progress = QString::number(f64_OverallFlashedBytes / f64_BYTE, 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize / f64_BYTE, 'f', 1);
   }
   else
   {
      //Bytes
      c_Unit = "B";
      c_Progress = QString::number(f64_OverallFlashedBytes, 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize, 'f', 1);
   }
   //Translation: 1:Flashed bytes, 2:File size, 3: File size unit
   c_DataTransfer = static_cast<QString>("%1 %3/%2 %3").arg(c_Progress).arg(c_Complete).arg(c_Unit);
   this->mpc_Ui->pc_BottomBar->SetDataTransfer(c_DataTransfer);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set  file status update

   \param[in]       oru32_FlashedFiles     Flashed file index
   \param[in]       oru8_State             State
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::SetStatusIcon(const uint32_t & oru32_FlashedFiles, const uint8_t & oru8_State)
{
   mpc_ListWidget->SetStatusIcon(oru32_FlashedFiles, oru8_State);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set status icons
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::ResetStatusIcons(void)
{
   mpc_ListWidget->ResetStatusIcons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get total size of all the files that will be flashed

   \return
   int32_t
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaUpProperties::GetHexFileSize(void) const
{
   return mpc_ListWidget->GetTotalHexFileSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  get hex file size

   \param[in]       oru32_HexFileIndex     hey file index

   \return
   int32_t
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_FlaUpProperties::GetHexFileSize(const uint32_t & oru32_HexFileIndex) const
{
   return mpc_ListWidget->GetHexFileSize(oru32_HexFileIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden dragEnterEvent slot
 *
 *  Passes opc_Event to mpc_ListWidget DragEnterEvent

   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   this->mpc_ListWidget->DragEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden dragMoveEvent slot
 *
 *  Passes opc_Event to mpc_ListWidget DragMoveEvent

   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   this->mpc_ListWidget->DragMoveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden dropEvent slot
 *
 *  Passes opc_Event to mpc_ListWidget DropEvent

   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::dropEvent(QDropEvent * const opc_Event)
{
   this->mpc_ListWidget->DropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update Title with Counter value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::m_UpdateLabelTitleWithFileCounter(void)
{
   if (this->mpc_ListWidget != NULL)
   {
      this->mpc_Ui->pc_LabelTitle->setText(
         static_cast<QString>(C_GtGetText::h_GetText("<b>Hex-Files (%1)<\b>")).arg(this->mpc_ListWidget->count()));
   }

   if (this->mpc_ListWidget->count() <= 0)
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(true);
      this->mpc_Ui->pc_Seperator->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(false);
      this->mpc_Ui->pc_Seperator->setVisible(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot to update the total file size if someone adds a file or several files with drag and drop
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::m_FileAdded(void)
{
   const uint32_t u32_PROGRESS_IN_PERCENTAGE = 0;

   UpdateDataTransfer();
   ResetSummary();
   UpdateProgressBar(u32_PROGRESS_IN_PERCENTAGE);
}
