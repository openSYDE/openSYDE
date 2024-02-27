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
   mq_ContinueUpdate(false)
{
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

   // User settings
   this->m_LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaMainWindow::~C_FlaMainWindow()
{
   delete this->mpc_Ui;
   delete this->mpc_UpSequences;
   this->mpc_UpSequences = NULL;
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
   bool q_EnableAction = false;

   this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_EnableAction);

   if (this->mpc_Ui->pc_UpdateWidget->GetHexFilePaths().isEmpty() == true)
   {
      C_OgeWiCustomMessage c_Message(this->mpc_Ui->pc_TitleBarWidget, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Hex-Files missing"));
      c_Message.SetDescription(
         C_GtGetText::h_GetText("No HEX file(s) found for flashing."));
      c_Message.Execute();

      q_EnableAction = true;
      this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_EnableAction);
   }
   else if (this->mpc_Ui->pc_UpdateWidget->AreAllFilesValid() == false)
   {
      C_OgeWiCustomMessage c_Message(this->mpc_Ui->pc_TitleBarWidget, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Hex-Files invalid"));
      c_Message.SetDescription(C_GtGetText::h_GetText("At least one HEX file is missing or invalid."));
      c_Message.Execute();

      q_EnableAction = true;
      this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_EnableAction);
   }
   else
   {
      this->mpc_Ui->pc_SettingsWidget->ClearProgress();

      if (this->m_InitUpdateSequence() == C_NO_ERR)
      {
         this->me_UpdateStep = C_FlaUpSequences::eACTIVATEFLASHLOADER;
         if (this->mpc_UpSequences->StartActivateFlashLoader(
                this->mpc_Ui->pc_SettingsWidget->GetFlashloaderResetWaitTime()) == C_NO_ERR)
         {
            this->mc_TimerUpdate.start();
         }
         else
         {
            tgl_assert(false);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle trigger search node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaMainWindow::m_OnSearchNode()
{
   bool q_EnableAction = false;

   this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_EnableAction);

   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_FlaSenSearchNodePopup * const pc_Dialog = new C_FlaSenSearchNodePopup(*c_New);

   //Resize
   const QSize c_SIZE(550, 510);

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
   }

   // connect signals
   connect(mpc_UpSequences, &C_FlaUpSequences::SigReportProgress, this, &C_FlaMainWindow::m_ShowProgress);
   connect(mpc_UpSequences, &C_FlaUpSequences::SigReportFlashloaderInformationText,
           this, &C_FlaMainWindow::m_ShowProgress);
   connect(mpc_UpSequences, &C_FlaUpSequences::SigReportDeviceName, this, &C_FlaMainWindow::m_CheckDeviceName);

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
      bool q_CallReset = false;

      // In the event queue could be some emitted signals from thread left.
      // We need all information for showing the result
      QApplication::processEvents();

      this->mc_TimerUpdate.stop();

      if (s32_SequenceResult != C_NO_ERR)
      {
         C_OgeWiCustomMessage c_Message(this->mpc_Ui->pc_TitleBarWidget, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Update failed"));
         c_Message.SetDescription(C_GtGetText::h_GetText("Failure when updating node with ID ") +
                                  QString::number(this->mpc_Ui->pc_GeneralPropertiesWidget->GetNodeId()) + ".");
         c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("For details see ")) +
                              C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLESHEET_GUIDE_COLOR_LINK,
                                               C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()) + ".");
         C_OscLoggingHandler::h_Flush();
         c_Message.Execute();

         const bool q_ENABLEACTION = true;
         this->mpc_Ui->pc_TitleBarWidget->EnableActions(q_ENABLEACTION);

         q_CallReset = true;
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
                  // no more steps to do (reset is done on success and failure)
                  C_OgeWiCustomMessage c_Message(this->mpc_Ui->pc_TitleBarWidget);
                  c_Message.SetHeading(C_GtGetText::h_GetText("Update successful"));
                  c_Message.SetDescription(C_GtGetText::h_GetText("Successfully updated node with ID ") +
                                           QString::number(this->mpc_Ui->pc_GeneralPropertiesWidget->GetNodeId()) +
                                           ".");
                  c_Message.SetDetails("Flashed HEX file(s):\n" +
                                       this->mpc_Ui->pc_UpdateWidget->GetHexFilePaths().join("\n"));
                  c_Message.Execute();
                  q_CallReset = true;
               }
            }
            else
            {
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
         this->me_UpdateStep = C_FlaUpSequences::eRESETSYSTEM;
         this->ms32_NextHexFile = 0; // reset
         s32_ThreadResult = this->mpc_UpSequences->StartResetSystem();
         if (s32_ThreadResult == C_NO_ERR)
         {
            this->mc_TimerUpdate.start();
         }
      }

      // very unlikely that thread start failed (C_BUSY)
      tgl_assert(s32_ThreadResult == C_NO_ERR);
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
