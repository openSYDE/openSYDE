//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main window for project SYDEflash

   Main window for project SYDEflash

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include <QWinTaskbarButton>

#include "stwerrors.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_HeHandler.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_UsHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_FlaConNodeConfigPopup.hpp"
#include "C_FlaSenDcBasicSequences.hpp"
#include "C_FlaSenSearchNodePopup.hpp"
#include "C_Uti.hpp"
#include "C_FlaUpListItemWidget.hpp"
#include "C_FlaMainWindow.hpp"

#include "ui_C_FlaMainWindow.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_FlaMainWindow::mhs32_SETTINGS_SPLITTER_MAX = 350;

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
C_FlaMainWindow::C_FlaMainWindow(QWidget * const opc_Parent) :
   QMainWindow(opc_Parent),
   mpc_Ui(new Ui::C_FlaMainWindow),
   mpc_UpSequences(NULL),
   ms32_NextHexFile(0),
   mq_ContinueUpdate(false),
   mu32_FlashedFilesCounter(0),
   mu64_FlashedBytes(0),
   mu32_FlashedBytesTmp(0),
   mq_NewFile(true)
{
   const uint16_t u16_COUTER_TIMEOUT_IN_MILLIS = 1000;

   this->mpc_Ui->setupUi(this);

   // set stretch factor to have the splitter at real user settings position on tool start
   this->mpc_Ui->pc_SplitterSettings->setStretchFactor(0, 1);
   // initialize settings splitter position
   // (real position is set on first move, so we avoid calculation and use 0 for simplicity)
   this->ms32_SettingsSplitterPosition = 0;

   // window title
   this->setWindowTitle("SYDEflash");

   // style background white
   this->mpc_Ui->pc_PropertiesAndUpdateWidget->SetBackgroundColor(0);

   // set help file path (openSYDE SYDEflash specific)
   stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().SetHelpFileRelPath("/../../tool/Help/openSYDE PC Tool.chm");

   // Connections
   // Settings splitter: individual collapse behavior
   connect(this->mpc_Ui->pc_SettingsWidget, &C_FlaSetWidget::SigExpandSettings,
           this, &C_FlaMainWindow::m_OnExpandSettings);
   connect(this->mpc_Ui->pc_SplitterSettings, &C_CamOgeSpi::splitterMoved,
           this, &C_FlaMainWindow::m_OnSettingsSplitterMoved);
   connect(this->mpc_Ui->pc_SplitterSettings, &C_CamOgeSpi::SigDoubleClick,
           this, &C_FlaMainWindow::m_OnSettingsSplitterHandleDoubleClick);
   // Title bar
   connect(this->mpc_Ui->pc_TitleBarWidget, &C_FlaTitleBarWidget::SigUpdateNode, this,
           &C_FlaMainWindow::m_OnUpdateNode);
   connect(this->mpc_Ui->pc_TitleBarWidget, &C_FlaTitleBarWidget::SigSearchNode, this,
           &C_FlaMainWindow::m_OnSearchNode);
   connect(this->mpc_Ui->pc_TitleBarWidget, &C_FlaTitleBarWidget::SigConfigureNode, this,
           &C_FlaMainWindow::m_OnConfigureNode);

   // Timers
   connect(&this->mc_TimerUpdate, &QTimer::timeout, this, &C_FlaMainWindow::m_TimerUpdate);
   this->mc_TimerUpdate.setInterval(25);

   connect(&this->mc_SecTimer, &QTimer::timeout, this, &C_FlaMainWindow::m_CountTime);
   this->mc_SecTimer.setInterval(u16_COUTER_TIMEOUT_IN_MILLIS);

   // User settings
   this->m_LoadUserSettings();
} //lint !e429  //no memory leak because of the parent of pc_Button and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaMainWindow::~C_FlaMainWindow()
{
   delete this->mpc_Ui;
   delete this->mpc_Progress;
   delete this->mpc_UpSequences;
   this->mpc_UpSequences = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten show event slot

   Init progress in Windows taskbar

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::showEvent(QShowEvent * const opc_Event)
{
   //handle task bar button
   m_InitWinTaskbar();

   opc_Event->accept();
   QMainWindow::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten close event slot

   Save user settings

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::closeEvent(QCloseEvent * const opc_Event)
{
   // Call all functions for saving user settings
   this->m_SaveUserSettings();

   //Clean up user settings
   C_UsHandler::h_Destroy();

   QMainWindow::closeEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten key press event slot

   Here: Trigger send-by-keypress, help key handling and save options.

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if (stw::opensyde_gui_logic::C_HeHandler::h_CheckHelpKey(opc_KeyEvent) == true)
   {
      stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
         this->metaObject()->className());
   }
   else
   {
      //Default handling
      QMainWindow::keyPressEvent(opc_KeyEvent);
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_LoadUserSettings(void)
{
   // adapt start position
   QPoint c_Position = C_UsHandler::h_GetInstance()->GetScreenPos();
   QSize c_Size = C_UsHandler::h_GetInstance()->GetAppSize();

   C_OgeWiUtil::h_CheckAndFixDialogPositionAndSize(c_Position, c_Size, QSize(1000, 700));
   this->setGeometry(c_Position.x(), c_Position.y(), c_Size.width(), c_Size.height());

   if (C_UsHandler::h_GetInstance()->GetAppMaximized() == true)
   {
      this->showMaximized();
   }

   // adapt splitter widget states
   this->m_OnExpandSettings(C_UsHandler::h_GetInstance()->GetSettingsAreExpanded());

   // Modules with own user settings
   this->mpc_Ui->pc_SettingsWidget->LoadUserSettings();
   this->mpc_Ui->pc_GeneralPropertiesWidget->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_SaveUserSettings(void)
{
   // Modules with own user settings
   this->mpc_Ui->pc_GeneralPropertiesWidget->SaveUserSettings();
   this->mpc_Ui->pc_UpdateWidget->SaveUserSettings();
   this->mpc_Ui->pc_SettingsWidget->SaveUserSettings();

   // screen
   C_UsHandler::h_GetInstance()->SetScreenPos(this->normalGeometry().topLeft());
   C_UsHandler::h_GetInstance()->SetAppSize(this->normalGeometry().size());
   C_UsHandler::h_GetInstance()->SetAppMaximized(this->isMaximized());

   // splitter
   // update to actual value if settings are not collapsed
   if (C_UsHandler::h_GetInstance()->GetSettingsAreExpanded() == true)
   {
      const QList<int32_t> c_Sizes = this->mpc_Ui->pc_SplitterSettings->sizes();
      if (c_Sizes.size() > 1)
      {
         C_UsHandler::h_GetInstance()->SetSplitterSettingsHorizontal(c_Sizes.at(1));
      }
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Expand or collapse settings.

   \param[in]  oq_Expand   true: expand settings subsections
                           false: collapse settings subsections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_OnExpandSettings(const bool oq_Expand)
{
   if (oq_Expand == true)
   {
      // expand
      this->mpc_Ui->pc_SplitterSettings->SetSecondSegment(
         std::max(mhs32_SETTINGS_SPLITTER_MAX, C_UsHandler::h_GetInstance()->GetSplitterSettingsHorizontal()));
   }
   else
   {
      // remember segment size (only when they were expanded, i.e. do not overwrite on tool start)
      if (C_UsHandler::h_GetInstance()->GetSettingsAreExpanded() == true)
      {
         const QList<int32_t> c_Sizes = this->mpc_Ui->pc_SplitterSettings->sizes();

         if (c_Sizes.size() > 1)
         {
            C_UsHandler::h_GetInstance()->SetSplitterSettingsHorizontal(c_Sizes.at(1));
         }
      }
      //collapse (because childrenCollapsible == false the following results in minimum size)
      this->mpc_Ui->pc_SplitterSettings->SetSecondSegment(0);
   }

   // save state (after expanding!)
   C_UsHandler::h_GetInstance()->SetSettingsAreExpanded(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle double click of settings splitter.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_OnSettingsSplitterHandleDoubleClick(void) const
{
   // toggle splitter expanded state
   this->mpc_Ui->pc_SettingsWidget->ExpandSettings(!C_UsHandler::h_GetInstance()->GetSettingsAreExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for splitter move.

   If settings section gets collapsed, do not really collapse but minimize it.

   \param[in]  ors32_Pos      New position of splitter
   \param[in]  ors32_Index    Index of splitter widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_OnSettingsSplitterMoved(const int32_t & ors32_Pos, const int32_t & ors32_Index)
{
   const QList<int32_t> & rc_Sizes = this->mpc_Ui->pc_SplitterSettings->sizes();

   if (ors32_Index == 1)
   {
      if (rc_Sizes.size() > 1)
      {
         // is actual size between boundaries?
         if ((rc_Sizes.at(1) <= C_FlaMainWindow::mhs32_SETTINGS_SPLITTER_MAX) && (rc_Sizes.at(1) > 35))
         {
            if (ors32_Pos > this->ms32_SettingsSplitterPosition)
            {
               // splitter moved from left to right -> check for collapse
               if (C_UsHandler::h_GetInstance()->GetSettingsAreExpanded() == true)
               {
                  this->mpc_Ui->pc_SettingsWidget->ExpandSettings(false);
               }
            }
            else if (ors32_Pos < this->ms32_SettingsSplitterPosition)
            {
               // splitter moved from right to left -> check for expand
               if (C_UsHandler::h_GetInstance()->GetSettingsAreExpanded() == false)
               {
                  this->mpc_Ui->pc_SettingsWidget->ExpandSettings(true);
               }
            }
            else
            {
               // do nothing
            }
         }
      }

      // remember position for next round
      this->ms32_SettingsSplitterPosition = ors32_Pos;

      // set splitter position
      if (C_UsHandler::h_GetInstance()->GetSettingsAreExpanded() == false)
      {
         this->mpc_Ui->pc_SplitterSettings->SetSecondSegment(0);
      }
      else
      {
         this->mpc_Ui->pc_SplitterSettings->SetSecondSegment(
            std::max(mhs32_SETTINGS_SPLITTER_MAX,
                     static_cast<int32_t>(this->mpc_Ui->pc_SplitterSettings->width()) - ors32_Pos));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle trigger update node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_OnUpdateNode()
{
   bool q_ProgressError = false;
   bool q_ProgressUpdate = false;
   QString orc_Text = "";
   const bool q_ENABLE_ACTION = true;

   m_ResetProgressBar();
   m_ResetFileIcons();
   m_SetProgressBarColor(q_ENABLE_ACTION);

   if (this->mpc_Ui->pc_UpdateWidget->GetHexFilePaths().isEmpty() == true)
   {
      q_ProgressError = true;

      C_OgeWiCustomMessage c_Message(this->mpc_Ui->pc_TitleBarWidget, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Hex-Files missing"));
      c_Message.SetDescription(
         C_GtGetText::h_GetText("No HEX file(s) found for flashing."));
      c_Message.Execute();

      this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_ENABLE_ACTION);
   }
   else if (this->mpc_Ui->pc_UpdateWidget->AreAllFilesValid() == false)
   {
      q_ProgressError = true;

      C_OgeWiCustomMessage c_Message(this->mpc_Ui->pc_TitleBarWidget, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Hex-Files invalid"));
      c_Message.SetDescription(C_GtGetText::h_GetText("At least one HEX file is missing or invalid."));
      c_Message.Execute();

      this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_ENABLE_ACTION);
   }
   else
   {
      if (this->m_InitUpdateSequence() == C_NO_ERR)
      {
         QApplication::setOverrideCursor(Qt::WaitCursor);
         orc_Text = "Entering update mode ...";

         this->me_UpdateStep = C_FlaUpSequences::eACTIVATEFLASHLOADER;
         if (this->mpc_UpSequences->StartActivateFlashLoader(
                this->mpc_Ui->pc_SettingsWidget->GetFlashloaderResetWaitTime()) == C_NO_ERR)
         {
            q_ProgressUpdate = true;

            this->mc_TimerUpdate.start();
            this->mc_SecTimer.start();
            this->m_StartElapsedTimer();
         }
         else
         {
            tgl_assert(false);
         }
      }
   }

   if (q_ProgressError || q_ProgressUpdate)
   {
      uint8_t u8_ProgressState = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_DEFAULT);
      if (q_ProgressError)
      {
         u8_ProgressState = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_ERROR);
      }

      m_SetHeadingText(orc_Text, u8_ProgressState);
      m_SetHeadingIcon(u8_ProgressState);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle trigger search node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_OnSearchNode()
{
   bool q_EnableAction = false;
   const QSize c_SIZE(550, 510);

   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_FlaSenSearchNodePopup * const pc_Dialog = new C_FlaSenSearchNodePopup(*c_New);

   m_ResetProgressBar();
   m_ResetFileIcons();

   this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_EnableAction);

   c_New->SetSize(c_SIZE);

   this->mpc_Ui->pc_SettingsWidget->ClearProgress();
   connect(pc_Dialog, &C_FlaSenSearchNodePopup::SigReportProgress, this, &C_FlaMainWindow::m_ShowProgress);

   if (pc_Dialog->StartSearch(this->mpc_Ui->pc_SettingsWidget->GetCanDllPath(),
                              this->mpc_Ui->pc_GeneralPropertiesWidget->GetBitrate(),
                              this->mpc_Ui->pc_SettingsWidget->GetFlashloaderResetWaitTime()) == C_NO_ERR)
   {
      if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         const uint8_t u8_NodeId = pc_Dialog->GetSelectedNodeId();
         this->mpc_Ui->pc_GeneralPropertiesWidget->SetNodeId(u8_NodeId);
      }
   }

   disconnect(pc_Dialog, &C_FlaSenSearchNodePopup::SigReportProgress, this, &C_FlaMainWindow::m_ShowProgress);

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
   q_EnableAction = true;
   this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_EnableAction);
} //lint !e429  no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle trigger configure node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_OnConfigureNode()
{
   m_ResetProgressBar();
   m_ResetFileIcons();

   bool q_EnalbeAction = false;

   this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_EnalbeAction);

   //get current values for dialog generation
   const uint8_t u8_NodeId = this->mpc_Ui->pc_GeneralPropertiesWidget->GetNodeId();
   const uint32_t u32_Bitrate = this->mpc_Ui->pc_GeneralPropertiesWidget->GetBitrate();
   const QString & rc_CanDllPath = this->mpc_Ui->pc_SettingsWidget->GetCanDllPath();

   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);

   C_FlaConNodeConfigPopup * const pc_Dialog =
      new C_FlaConNodeConfigPopup(*c_New, u8_NodeId, u32_Bitrate,
                                  this->mpc_Ui->pc_SettingsWidget->GetFlashloaderResetWaitTime(),
                                  rc_CanDllPath);

   //Resize
   const QSize c_SIZE(550, 400);

   c_New->SetSize(c_SIZE);

   this->mpc_Ui->pc_SettingsWidget->ClearProgress();

   connect(pc_Dialog, &C_FlaConNodeConfigPopup::SigReportProgress, this, &C_FlaMainWindow::m_ShowProgress);

   c_New->exec();

   disconnect(pc_Dialog, &C_FlaConNodeConfigPopup::SigReportProgress, this, &C_FlaMainWindow::m_ShowProgress);

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
   q_EnalbeAction = true;

   this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_EnalbeAction);
} //lint !e429  no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show progress

   \param[in]  orc_Text    Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_ShowProgress(const QString & orc_Text)
{
   this->mpc_Ui->pc_SettingsWidget->ShowProgress(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show progress bar text

   \param[in]  orc_Text             Text that will be presented
   \param[in]  u8_ProgressState     State of the progress (hu32_STATE_DEFAULT - black,
                                                           hu32_STATE_FINISHED - green,
                                                           hu32_STATE_ERROR - red)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_SetHeadingText(const QString & orc_Text, const uint8_t & oru8_ProgressState) const
{
   this->mpc_Ui->pc_UpdateWidget->SetHeadingText(orc_Text, oru8_ProgressState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set Heading Icon

   \param[in]  u8_ProgressState     Progress state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_SetHeadingIcon(const uint8_t & oru8_State) const
{
   this->mpc_Ui->pc_UpdateWidget->SetHeadingIcon(oru8_State);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set progress bar color

   \param[in]  oq_Success    If oq_Success bar is green else red
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_SetProgressBarColor(const bool & orq_Success) const
{
   this->mpc_Ui->pc_UpdateWidget->SetProgressBarColor(orq_Success);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calcualte how many bytes of a specific file have been flashed

   \param[in]       s32_FlashableBytesPerFile     Amount of flashable bytes
   \param[in]       oru8_ProgressInPercentage     Progress in percentage

   \return
   uint32_t
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t stw::opensyde_gui::C_FlaMainWindow::m_CalculateFileBytesFlashed(const uint8_t & oru8_ProgressInPercentage)
{
   const uint8_t u8_MAX_PERCENTAGE = 100;

   const uint32_t u32_FlashableBytesPerFile = this->mpc_Ui->pc_UpdateWidget->GetHexFileSize(mu32_FlashedFilesCounter);
   const float32_t f32_PercentageDecimal = static_cast<float32_t>(oru8_ProgressInPercentage) /
                                           static_cast<float32_t>(u8_MAX_PERCENTAGE);
   const float32_t f32_FlashedBytes = f32_PercentageDecimal * static_cast<float32_t>(u32_FlashableBytesPerFile);
   const uint32_t u32_FlashedBytes = static_cast<uint32_t>(f32_FlashedBytes);

   return u32_FlashedBytes;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init windows taskbar progress bar
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_InitWinTaskbar(void)
{
   const uint8_t u8_MINIMUM_PERCENTAGE = 0;
   const uint8_t u8_MAXIMUM_PERCENTAGE = 100;

   QWidget * const pc_Top = C_OgeWiUtil::h_GetWidgetUnderNextPopUp(this);
   QWinTaskbarButton * const pc_Button = new QWinTaskbarButton(pc_Top);

   pc_Button->setWindow(pc_Top->windowHandle());

   this->mpc_Progress = pc_Button->progress();
   this->mpc_Progress->setMinimum(u8_MINIMUM_PERCENTAGE);
   this->mpc_Progress->setMaximum(u8_MAXIMUM_PERCENTAGE);
} //lint !e429  //no memory leak because of the parent of pc_Button and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Error has been detected

   Reset progress bar, taskbar progress, necessary values and show a popup window if wanted

   \param[in]       orq_ShowErrorMessage     Show error message popup window

   \return
   true        reset
   false       no reset
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_FlaMainWindow::m_ErrorDetected(const bool & orq_ShowErrorMessage)
{
   bool q_CallReset = true;

   const uint8_t u8_STATE = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_ERROR);
   const QString c_Text = "Update failed!";
   const uint8_t u8_PROGRESS_STATE = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_ERROR);
   const bool q_ENABLEACTION = true;
   const bool q_SUCCESSFULL_UPDATE = false;

   m_UpdateFileIcon(mu32_FlashedFilesCounter, u8_STATE);

   this->mc_SecTimer.stop();
   this->m_RestartElapsedTimer();
   this->mpc_Ui->pc_UpdateWidget->ResetSummary();

   m_SetHeadingText(c_Text, u8_PROGRESS_STATE);
   m_SetHeadingIcon(u8_PROGRESS_STATE);
   m_SetProgressBarColor(q_SUCCESSFULL_UPDATE);

   if (orq_ShowErrorMessage)
   {
      q_CallReset = m_ShowErrorMessage();
   }

   C_OscLoggingHandler::h_Flush();
   this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_ENABLEACTION);

   mq_NewFile = q_ENABLEACTION;

   return q_CallReset;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update bottom bar

   Update progress bar and the data transfer

   \param[in]       oru8_ProgressInPercentage     Progress in percentage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_UpdateBottomBar(const uint8_t & oru8_ProgressInPercentage)
{
   const uint8_t u8_MAX_PERCENTAGE = 100;
   const bool q_VISIBLE_WIN_PROGRESS = true;

   if ((oru8_ProgressInPercentage == 0) && mq_NewFile)
   {
      const uint8_t u8_STATE_PROGRESS = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_IN_PROGRESS);
      const QString orc_Text = "Updating Node ...";
      m_SetHeadingText(orc_Text, u8_STATE_PROGRESS);
      m_UpdateFileIcon(mu32_FlashedFilesCounter, u8_STATE_PROGRESS);

      mu32_FlashedBytesTmp = 0;
      mq_NewFile = false;
   }

   const uint32_t u32_CalculateFileBytesFlashed = m_CalculateFileBytesFlashed(oru8_ProgressInPercentage);
   if ((u32_CalculateFileBytesFlashed > 0) && (u32_CalculateFileBytesFlashed > mu32_FlashedBytesTmp))
   {
      uint8_t u8_TotalProgressInPercentage;

      const uint32_t u32_Differences = u32_CalculateFileBytesFlashed - mu32_FlashedBytesTmp;

      mu32_FlashedBytesTmp += u32_Differences;
      mu64_FlashedBytes += u32_Differences;

      u8_TotalProgressInPercentage = m_CalcualteTotalProgressInPercentage();

      this->mpc_Ui->pc_UpdateWidget->UpdateProgressBar(u8_TotalProgressInPercentage);
      this->m_UpdateWinProgress(q_VISIBLE_WIN_PROGRESS, u8_TotalProgressInPercentage);
      this->mpc_Ui->pc_UpdateWidget->UpdateDataTransfer(mu64_FlashedBytes);
   }

   //Calculate the total file bytes percentage
   if (oru8_ProgressInPercentage == u8_MAX_PERCENTAGE)
   {
      const uint8_t u8_STATE_FINISHED = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_FINISHED);
      m_UpdateFileIcon(mu32_FlashedFilesCounter, u8_STATE_FINISHED);
      mu32_FlashedFilesCounter += 1;
      mq_NewFile = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the already flashed file by updating its icon

   \param[in]      oru32_FileIndex    File index
   \param[in]      u8_ProgressState   State of the file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_UpdateFileIcon(const uint32_t & oru32_FileIndex, const uint8_t & oru8_ProgressState) const
{
   this->mpc_Ui->pc_UpdateWidget->SetStatusIcon(oru32_FileIndex, oru8_ProgressState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set task bar progress

   \param[in]  orq_Visible  Progress is visible flag
   \param[in]  oru8_Value  Progress value (0-100)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_UpdateWinProgress(const bool & orq_Visible, const uint8_t & oru8_Value)
{
   this->mpc_Progress->setVisible(orq_Visible);
   this->mpc_Progress->setValue(oru8_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calcualte total progress in percentage

   \return
   uint8_t
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_FlaMainWindow::m_CalcualteTotalProgressInPercentage(void) const
{
   const uint8_t u8_MINIMUM_PROGRESS_PERCENTAGE = 2; // defined in C_OscBuSequences

   const uint8_t u8_MAX_PERCENTAGE = 100;
   const uint32_t u32_TotalNumberOfFlashableSize = this->mpc_Ui->pc_UpdateWidget->GetHexFileSize();
   uint8_t u8_ProgressInPercentage =
      static_cast<uint8_t>((u8_MAX_PERCENTAGE * mu64_FlashedBytes) / u32_TotalNumberOfFlashableSize);

   if (u8_ProgressInPercentage < (u8_MAX_PERCENTAGE - u8_MINIMUM_PROGRESS_PERCENTAGE))
   {
      u8_ProgressInPercentage += u8_MINIMUM_PROGRESS_PERCENTAGE;
   }
   return u8_ProgressInPercentage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset the progress bar with default values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_ResetProgressBar()
{
   const bool q_VISIBLE_PROGRESSBAR = false;
   const uint8_t u8_PROGRESSBAR_VALUE = 0;
   const uint8_t u8_FLAHSED_FILES_COUNTER = 0;

   /*Reset progress bar*/
   this->mpc_Ui->pc_UpdateWidget->ResetSummary();
   this->mpc_Ui->pc_UpdateWidget->UpdateProgressBar(u8_PROGRESSBAR_VALUE);
   this->mpc_Ui->pc_UpdateWidget->UpdateDataTransfer(u8_PROGRESSBAR_VALUE);

   /*Reset taskbar progress*/
   m_UpdateWinProgress(q_VISIBLE_PROGRESSBAR, u8_PROGRESSBAR_VALUE);

   /*Reset local variables*/
   this->mc_SecTimer.stop();
   this->m_RestartElapsedTimer();
   mu32_FlashedFilesCounter = u8_FLAHSED_FILES_COUNTER;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset flashable file Icons
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_ResetFileIcons() const
{
   this->mpc_Ui->pc_UpdateWidget->ResetStatusIcons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize update sequence

   \return
   C_NO_ERR    everything ok
   else        error occurred, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaMainWindow::m_InitUpdateSequence(void)
{
   int32_t s32_Return;

   if (this->mpc_UpSequences == NULL)
   {
      this->mpc_UpSequences = new C_FlaUpSequences();

      // connect signals
      connect(mpc_UpSequences, &C_FlaUpSequences::SigReportProgress, this, &C_FlaMainWindow::m_ShowProgress);
      connect(mpc_UpSequences, &C_FlaUpSequences::SigReportFlashingProgress, this, &C_FlaMainWindow::m_UpdateBottomBar);
      connect(mpc_UpSequences, &C_FlaUpSequences::SigReportFlashloaderInformationText,
              this, &C_FlaMainWindow::m_ShowProgress);
      connect(mpc_UpSequences, &C_FlaUpSequences::SigReportDeviceName, this, &C_FlaMainWindow::m_CheckDeviceName);
   }

   // disable UI
   this->mpc_Ui->pc_TitleBarWidget->EnableActions(false);
   this->mpc_Ui->pc_GeneralPropertiesWidget->setEnabled(false);
   this->mpc_Ui->pc_UpdateWidget->EnableSettings(false);
   this->mpc_Ui->pc_SettingsWidget->EnableSettings(false);

   // initialize sequence
   s32_Return = this->mpc_UpSequences->Init(this->mpc_Ui->pc_SettingsWidget->GetCanDllPath().toStdString(),
                                            this->mpc_Ui->pc_GeneralPropertiesWidget->GetBitrate(),
                                            this->mpc_Ui->pc_GeneralPropertiesWidget->GetNodeId());

   if (s32_Return != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(this->mpc_Ui->pc_TitleBarWidget, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Initialization failed"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Failed to initialize CAN interface."));
      c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("For details see ")) +
                           C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLESHEET_GUIDE_COLOR_LINK,
                                            C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()) + ".");
      C_OscLoggingHandler::h_Flush();
      c_Message.Execute();

      // cleanup
      this->m_CleanupUpdateSequence();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Cleanup update sequence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_CleanupUpdateSequence(void)
{
   if (this->mpc_UpSequences != NULL)
   {
      disconnect(mpc_UpSequences, &C_FlaUpSequences::SigReportProgress, this, &C_FlaMainWindow::m_ShowProgress);
      disconnect(mpc_UpSequences, &C_FlaUpSequences::SigReportFlashloaderInformationText,
                 this, &C_FlaMainWindow::m_ShowProgress);
      disconnect(mpc_UpSequences, &C_FlaUpSequences::SigReportDeviceName, this, &C_FlaMainWindow::m_CheckDeviceName);
   }

   // enable UI (always)
   this->mpc_Ui->pc_TitleBarWidget->EnableActions(true);
   this->mpc_Ui->pc_GeneralPropertiesWidget->setEnabled(true);
   this->mpc_Ui->pc_UpdateWidget->EnableSettings(true);
   this->mpc_Ui->pc_SettingsWidget->EnableSettings(true);

   delete this->mpc_UpSequences;
   this->mpc_UpSequences = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Timer function for the Update sequence connected on timeout
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_TimerUpdate(void)
{
   int32_t s32_ThreadResult = C_CONFIG;
   int32_t s32_SequenceResult = C_UNKNOWN_ERR;

   if (this->mpc_UpSequences != NULL)
   {
      s32_ThreadResult = this->mpc_UpSequences->GetResults(s32_SequenceResult);
   }

   // Thread finished if result is C_NO_ERR
   if (s32_ThreadResult == C_NO_ERR)
   {
      QApplication::setOverrideCursor(Qt::ArrowCursor);

      bool q_CallReset = false;

      // In the event queue could be some emitted signals from thread left.
      // We need all information for showing the result
      QApplication::processEvents();

      this->mc_TimerUpdate.stop();

      if (s32_SequenceResult != C_NO_ERR)
      {
         const bool q_SHOW_ERROR_MESSAGE = true;
         q_CallReset = m_ErrorDetected(q_SHOW_ERROR_MESSAGE);
      }
      else
      {
         // note: me_UpdateStep is the current step that is already finished -> so we always do the "next step" stuff
         switch (this->me_UpdateStep)
         {
         case C_FlaUpSequences::eNOT_ACTIVE:
            // should never happen
            tgl_assert(false);
            break;
         case C_FlaUpSequences::eACTIVATEFLASHLOADER:
            // next step: read device information
            this->me_UpdateStep = C_FlaUpSequences::eREADDEVICEINFO;
            s32_ThreadResult = this->mpc_UpSequences->StartReadDeviceInformation();
            if (s32_ThreadResult == C_NO_ERR)
            {
               this->mc_TimerUpdate.start();
            }
            break;
         case C_FlaUpSequences::eREADDEVICEINFO:
            // nothing to do here, we go on after device name check that is triggered by signal
            break;
         case C_FlaUpSequences::eUPDATENODE:
            // next step: update node with next hex file if available (and if device name check is ok)
            if (mq_ContinueUpdate == true)
            {
               const QStringList & rc_HexFiles = this->mpc_Ui->pc_UpdateWidget->GetHexFilePaths();
               if (ms32_NextHexFile < rc_HexFiles.size())
               {
                  s32_ThreadResult =
                     this->mpc_UpSequences->StartUpdateNode(rc_HexFiles.at(ms32_NextHexFile),
                                                            this->mpc_Ui->pc_SettingsWidget->GetRequestDownloadTimeout(),
                                                            this->mpc_Ui->pc_SettingsWidget->GetTransferDataTimeout());
                  if (s32_ThreadResult == C_NO_ERR)
                  {
                     ms32_NextHexFile++;
                     this->mc_TimerUpdate.start();
                  }
               }
               else
               {
                  const QString c_Text = "Update successful!";
                  const uint8_t u8_PROGRESS_STATE = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_FINISHED);

                  C_OgeWiCustomMessage c_Message(this->mpc_Ui->pc_TitleBarWidget);

                  m_SetHeadingIcon(u8_PROGRESS_STATE);
                  m_SetHeadingText(c_Text, u8_PROGRESS_STATE);
                  this->mc_SecTimer.stop();

                  // no more steps to do (reset is done on success and failure)
                  c_Message.SetHeading(C_GtGetText::h_GetText("Update successful"));
                  c_Message.SetDescription(C_GtGetText::h_GetText("Successfully updated node with ID ") +
                                           QString::number(this->mpc_Ui->pc_GeneralPropertiesWidget->GetNodeId()) +
                                           ".");
                  c_Message.SetDetails("Flashed HEX file(s):\n" +
                                       this->mpc_Ui->pc_UpdateWidget->GetHexFilePaths().join("\n"));

                  if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
                  {
                     const uint8_t u8_STATE = static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_FINISHED);
                     const uint32_t u32_FlashedFileIndex = mu32_FlashedFilesCounter - 1;

                     this->m_RestartElapsedTimer();
                     m_UpdateFileIcon(u32_FlashedFileIndex, u8_STATE);

                     q_CallReset = true;
                  }
               }
            }
            else
            {
               const bool q_SHOW_ERROR_MESSAGE = false;
               m_ErrorDetected(q_SHOW_ERROR_MESSAGE);

               q_CallReset = true;
            }
            break;
         case C_FlaUpSequences::eRESETSYSTEM:
            // reset finished -> no more steps to do -> cleanup sequence
            this->m_CleanupUpdateSequence();
            break;
         default:
            break;
         }
      }

      if (q_CallReset == true)
      {
         mu32_FlashedFilesCounter = 0;
         mu64_FlashedBytes = 0;
         mu32_FlashedBytesTmp = 0;
         const uint8_t u8_WIN_PROGRESS_PERCENTAGE = 0;
         const bool q_VISIBLE_WIN_PROGRESS = false;

         this->m_UpdateWinProgress(q_VISIBLE_WIN_PROGRESS, u8_WIN_PROGRESS_PERCENTAGE);

         this->me_UpdateStep = C_FlaUpSequences::eRESETSYSTEM;
         this->ms32_NextHexFile = 0; // reset
         s32_ThreadResult = this->mpc_UpSequences->StartResetSystem();
         if (s32_ThreadResult == C_NO_ERR)
         {
            this->mc_TimerUpdate.start();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check device name read from flashloader against HEX files

   \param[in]  orc_ReadDeviceName   Read device name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_CheckDeviceName(const QString & orc_ReadDeviceName)
{
   bool q_DeviceNameCheck = true;

   this->mq_ContinueUpdate = true;

   // Get all device names from the hex files
   QStringList c_DeviceNames = this->mpc_Ui->pc_UpdateWidget->GetHexFileDeviceNames();

   // Compare device name with the device names from all HEX files
   for (int32_t s32_Index = 0; s32_Index < c_DeviceNames.size(); s32_Index++)
   {
      // Trim device names
      c_DeviceNames[s32_Index] = c_DeviceNames[s32_Index].trimmed();

      // Compare device name
      if (orc_ReadDeviceName.compare(c_DeviceNames[s32_Index], Qt::CaseInsensitive) != 0)
      {
         q_DeviceNameCheck = false;
      }
   }
   const QString c_DeviceNamesList = c_DeviceNames.join(", ");

   // If device name does not match
   if (q_DeviceNameCheck == false)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;
      const QString c_Details = static_cast<QString>(C_GtGetText::h_GetText("Device: %1 \nHEX file(s): %2")).arg(
         orc_ReadDeviceName).arg(c_DeviceNamesList);

      // Show message if user wants to continue with update
      c_Message.SetHeading(C_GtGetText::h_GetText("Device name check"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "Device name of at least one HEX file does not match the "
                                  "device name reported. Do you want to continue with the update anyway?"));
      c_Message.SetDetails(c_Details);
      c_Message.SetOkButtonText(C_GtGetText::h_GetText("Continue"));
      c_Message.SetNoButtonText(C_GtGetText::h_GetText("Cancel"));

      e_ReturnMessageBox = c_Message.Execute();

      // user does not want to update
      if (e_ReturnMessageBox != C_OgeWiCustomMessage::eYES)
      {
         this->mq_ContinueUpdate = false;
      }
   }

   // Start timer always to trigger reset anyway
   this->me_UpdateStep = C_FlaUpSequences::eUPDATENODE;
   this->mc_TimerUpdate.start();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start elapsed timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_CountTime(void) const
{
   const uint64_t u64_ELAPSED_MS = this->mc_ElapsedTimer.elapsed();
   const uint64_t u64_MS_TO_SEC = 1000ULL;
   const uint64_t u64_MS_TO_MIN = 60000ULL;
   const uint64_t u64_MS_TO_HOURS = 3600000ULL;
   const uint64_t u64_MODULOR_OPERATION_VALUE = 60ULL;

   const uint64_t u64_ElapsedSeconds = (u64_ELAPSED_MS / u64_MS_TO_SEC) % u64_MODULOR_OPERATION_VALUE;
   const uint64_t u64_ElapsedMin = (u64_ELAPSED_MS / u64_MS_TO_MIN) % u64_MODULOR_OPERATION_VALUE;
   const uint64_t u64_ElapsedHours = u64_ELAPSED_MS / u64_MS_TO_HOURS;

   const QString c_Time = static_cast<QString>("%1:%2:%3").arg(u64_ElapsedHours, 2, 10, QChar('0')).
                          arg(u64_ElapsedMin, 2, 10, QChar('0')).arg(u64_ElapsedSeconds, 2, 10, QChar('0'));

   this->mpc_Ui->pc_UpdateWidget->SetElapsedTime(c_Time);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start elapsed timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_StartElapsedTimer(void)
{
   this->mc_ElapsedTimer.start();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restart elapsed timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_RestartElapsedTimer(void)
{
   this->mc_ElapsedTimer.restart();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show error message popup window

   \param[in]       oq_ShowErrorMessage     Show error message (Id is wrong)

   \return
   true        reset
   false       no reset
*/
//----------------------------------------------------------------------------------------------------------------------
bool stw::opensyde_gui::C_FlaMainWindow::m_ShowErrorMessage(void)
{
   bool q_CallReset = false;

   C_OgeWiCustomMessage c_Message(this->mpc_Ui->pc_TitleBarWidget, C_OgeWiCustomMessage::eERROR);

   c_Message.SetHeading(C_GtGetText::h_GetText("Update failed"));
   c_Message.SetDescription(C_GtGetText::h_GetText("Failure when updating node with ID ") +
                            QString::number(this->mpc_Ui->pc_GeneralPropertiesWidget->GetNodeId()) + ".");
   c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("For details see ")) +
                        C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLESHEET_GUIDE_COLOR_LINK,
                                         C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()) + ".");

   if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
   {
      q_CallReset = true;
   }

   return q_CallReset;
}
