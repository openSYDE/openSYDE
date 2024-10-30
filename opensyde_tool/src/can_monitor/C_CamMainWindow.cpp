//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main window for project openSYDE CAN Monitor (implementation)

   Main window for project openSYDE CAN Monitor

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileInfo>

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "C_CamMainWindow.hpp"
#include "ui_C_CamMainWindow.h"

#include "TglUtils.hpp"
#include "TglTime.hpp"
#include "C_OscUtils.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_CamUti.hpp"
#include "C_UsHandler.hpp"
#include "C_CamProHandler.hpp"
#include "C_CamDbHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_GtGetText.hpp"
#include "C_HeHandler.hpp"

#include <QDebug>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_CamMainWindow::mhs32_SETTINGS_SPLITTER_MAX = 350;
const int32_t C_CamMainWindow::mhs32_MESSAGE_GEN_SPLITTER_MAX = 120;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMainWindow::C_CamMainWindow(QWidget * const opc_Parent) :
   QMainWindow(opc_Parent),
   mpc_Ui(new Ui::C_CamMainWindow),
   mpc_CanDllDispatcher(NULL),
   mq_LoggingStarted(false)
{
   this->mpc_Ui->setupUi(this);

   // set stretch factor to have the splitter at real user settings position on tool start
   this->mpc_Ui->pc_SplitterSettings->setStretchFactor(0, 1);
   this->mpc_Ui->pc_SplitterMessageGen->setStretchFactor(0, 1);
   // initialize settings splitter position
   // (real position is set on first move, so we avoid calculation and use 0 for simplicity)
   this->ms32_SettingsSplitterPosition = 0;
   this->ms32_MessageGenSplitterPosition = 0;

   //Drag & drop of *.syde_cam and database files
   this->setAcceptDrops(true);

   // window title
   this->setWindowTitle("openSYDE CAN Monitor");

   // set help file path (openSYDE CAN Monitor specific)
   stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().SetHelpFileRelPath("/../Help/openSYDE PC Tool.chm");

   // Connections
   // State handling
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigStartLogging,
           this, &C_CamMainWindow::m_StartLogging);
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigPauseLogging,
           this, &C_CamMainWindow::m_PauseLogging);
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigContinueLogging,
           this, &C_CamMainWindow::m_ContinueLogging);
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigStopLogging,
           this, &C_CamMainWindow::m_StopLogging);
   // Settings splitter: individual collapse behavior
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigExpandSettings,
           this, &C_CamMainWindow::m_OnExpandSettings);
   connect(this->mpc_Ui->pc_SplitterSettings, &C_CamOgeSpi::splitterMoved,
           this, &C_CamMainWindow::m_OnSettingsSplitterMoved);
   connect(this->mpc_Ui->pc_SplitterSettings, &C_CamOgeSpi::SigDoubleClick,
           this, &C_CamMainWindow::m_OnSettingsSplitterHandleDoubleClick);
   // Message generator splitter: individual collapse behavior
   connect(this->mpc_Ui->pc_GeneratorWidget, &C_CamGenWidget::SigExpandMessageGen,
           this, &C_CamMainWindow::m_OnExpandMessageGen);
   connect(this->mpc_Ui->pc_SplitterMessageGen, &C_CamOgeSpi::splitterMoved,
           this, &C_CamMainWindow::m_OnMessageGenSplitterMoved);
   connect(this->mpc_Ui->pc_SplitterMessageGen, &C_CamOgeSpi::SigDoubleClick,
           this, &C_CamMainWindow::m_OnMessageGenSplitterHandleDoubleClick);
   // Project operations (window title)
   connect(this->mpc_Ui->pc_TitleBarWidget, &C_CamTitleBarWidget::windowTitleChanged,
           this, &C_CamMainWindow::setWindowTitle);
   // Filter configuration
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigFilterNumberChanged,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SetActiveFilters);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigAddFilterItems,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::AddFilterItems);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigRemoveFilterItems,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::RemoveFilterItems);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigRemoveAllFilters,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::RemoveAllFilters);
   // Connect before database handling -> needs to be called first before next load is started
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigAddDatabaseOsySysDef,
           this, &C_CamMainWindow::m_OnDatabaseLoadStarted);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigAddDatabaseDbc,
           this, &C_CamMainWindow::m_OnDatabaseLoadStarted);
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigDatabaseLoadResultDbc,
           this, &C_CamMainWindow::m_OnDatabaseLoadFinished);
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigDatabaseLoadResultOsySysDef,
           this, &C_CamMainWindow::m_OnDatabaseLoadFinished);
   // Database handling
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigAddDatabaseOsySysDef,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::AddDatabaseOsySysDef);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigAddDatabaseDbc,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::AddDatabaseDbc);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigRemoveDatabase,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::RemoveDatabase);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigNotifyMissingDataBase,
           this, &C_CamMainWindow::m_CheckMessagesForLoadedDatabase);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigActivateDatabase,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::ActivateDatabase);
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigDatabaseLoadResultDbc,
           this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::OnLoadFinishedDbc);
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigDatabaseLoadResultOsySysDef,
           this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::OnLoadFinishedOsySysDef);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigRemoveDatabase,
           this, &C_CamMainWindow::m_OnDatabaseRemove);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigSetDatabaseOsySysDefBus,
           this, &C_CamMainWindow::m_OnOsyChangeBus);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigActivateDatabase,
           this, &C_CamMainWindow::m_OnActivateDatabase);
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration,
           this, &C_CamMainWindow::m_OnNewConfiguration);
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigClearOldConfiguration,
           this, &C_CamMainWindow::m_OnClearOldConfiguration);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigSetDatabaseOsySysDefBus,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SetDatabaseOsySysDefBus);
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigDatabaseSetOsySysDefBusResult,
           this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::OnSigOsySysDefBusResult);
   connect(this->mpc_Ui->pc_TitleBarWidget, &C_CamTitleBarWidget::SigSavedAsNew,
           this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::OnSigSavedAsNew);
   // Logging configuration
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigAddLogFileAsc,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::AddLogFileAsc);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigAddLogFileBlf,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::AddLogFileBlf);
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigRemoveAllLogFiles,
           this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::RemoveAllLogFiles);
   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigLogFileAddResult,
           this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::OnSigLogFileAddResult);
   // Saving user settings
   connect(this->mpc_Ui->pc_TitleBarWidget, &C_CamTitleBarWidget::SigPrepareForSave,
           this, &C_CamMainWindow::m_SaveUserSettings);
   // CAN DLL configuration
   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigCanDllConfigured, this,
           &C_CamMainWindow::m_OnCanDllConfigChange);

   mpc_CanThread = new stw::opensyde_gui_logic::C_SyvComDriverThread(&C_CamMainWindow::mh_ThreadFunc, this);

   // Starting the real communication
   mpc_CanDllDispatcher = new stw::can::C_Can();

   // Prepare the COM driver for CAN message handling
   this->mc_ComDriver.InitBase(this->mpc_CanDllDispatcher);
   this->mc_ComDriver.RegisterLogger(this->mpc_Ui->pc_TraceWidget->GetMessageMonitor());

   // Load initial project
   this->m_LoadInitialProject();

   // Message generator
   connect(this->mpc_Ui->pc_GeneratorWidget, &C_CamGenWidget::SigSendMessage, this, &C_CamMainWindow::m_SendMessage);
   connect(this->mpc_Ui->pc_GeneratorWidget, &C_CamGenWidget::SigRegisterCyclicMessage, this,
           &C_CamMainWindow::m_RegisterCyclicMessage);
   connect(this->mpc_Ui->pc_GeneratorWidget, &C_CamGenWidget::SigRemoveAllCyclicMessages, this,
           &C_CamMainWindow::m_RemoveAllCyclicMessages);

   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigEmitAddFilterToParentWidget, this,
           &C_CamMainWindow::m_AddFilterData);
   connect(this, &C_CamMainWindow::SigEmitAddFilterToChildWidget, this->mpc_Ui->pc_SettingsWidget,
           &C_CamMosWidget::AddFilterData);

   connect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigSendCanMsgDroppedToParentWidget, this,
           &C_CamMainWindow::CanFilterMsgDropped);
   connect(this, &C_CamMainWindow::SigSendCanMsgDroppedToChildrenWidget, this->mpc_Ui->pc_TraceWidget,
           &C_CamMetWidget::CanFilterMsgDropped);

   connect(this->mpc_Ui->pc_TraceWidget, &C_CamMetWidget::SigClearData, this, &C_CamMainWindow::m_ClearData);
   connect(this->mpc_Ui->pc_GeneratorWidget, &C_CamGenWidget::SigAutoProtocolSupport, this,
           &C_CamMainWindow::m_UpdateAutoSupportProtocol);

   this->m_LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMainWindow::~C_CamMainWindow()
{
   if (mpc_CanThread->isRunning() == true)
   {
      // TODO
      this->mpc_CanThread->requestInterruption();
      this->mpc_CanThread->wait(2000U);
   }

   //Disconnect database remove (otherwise the signal will be handled by a destroyed main window
   disconnect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigRemoveDatabase,
              this, &C_CamMainWindow::m_OnDatabaseRemove);
   disconnect(this->mpc_Ui->pc_SettingsWidget, &C_CamMosWidget::SigNotifyMissingDataBase,
              this, &C_CamMainWindow::m_CheckMessagesForLoadedDatabase);

   delete mpc_CanThread;
   mpc_CanThread = NULL;

   if (mpc_CanDllDispatcher != NULL)
   {
      this->m_CloseCan();

      delete mpc_CanDllDispatcher;
      mpc_CanDllDispatcher = NULL;
   }

   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function call of connect mechanism from child widget constructor. Used to pass CanMsgDropped Information to child widget

   \return
   Emit a signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::CanFilterMsgDropped()
{
   Q_EMIT SigSendCanMsgDroppedToChildrenWidget();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function call of connect mechanism from constructor. Received QList of selected messages
 *   Can Id's  and IsCanMsgExtended data from child widget

   \param[in]       oc_CanMsgId     List of selected message CanId's
   \param[in]       oc_CanMsgXtd     List of selected CanMessage has extended format

   \return
   Emit an signal which will be caught in constructor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_AddFilterData(const QList<int32_t> oc_CanMsgId, const QList<uint8_t> oc_CanMsgXtd)
{
   Q_EMIT C_CamMainWindow::SigEmitAddFilterToChildWidget(oc_CanMsgId, oc_CanMsgXtd);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On Update of AutoSupportProtocol to send information for stop/start Message counter

   \param[in]       ou32_MessageIndex     Current message index
   \param[in]      oq_Active             Is AutoSupport activated
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_UpdateAutoSupportProtocol(const uint32_t ou32_MessageIndex, const bool oq_Active)
{
   const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(ou32_MessageIndex);

   if (pc_Message != NULL)
   {
      C_OscCanProtocol::E_Type e_ProtocolType = C_OscCanProtocol::eCAN_OPEN;
      C_CamDbHandler::h_GetInstance()->GetOscMessage(pc_Message->c_DataBaseFilePath.c_str(),
                                                     pc_Message->c_Name.c_str(),
                                                     pc_Message->q_ContainsValidHash,
                                                     pc_Message->u32_Hash,
                                                     &e_ProtocolType);

      this->mc_ComDriver.UpdateAutoSupportProtocol(pc_Message->ToCanMessage().u32_ID, oq_Active, e_ProtocolType);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten close event slot

   Save user settings

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::closeEvent(QCloseEvent * const opc_Event)
{
   if (this->mpc_Ui->pc_TitleBarWidget->HandleProjectComparison())
   {
      // Call all functions for saving user settings
      this->m_SaveUserSettings();

      //Clean up user settings
      C_UsHandler::h_Destroy();

      QMainWindow::closeEvent(opc_Event);
   }
   else
   {
      //User aborted quit action
      opc_Event->ignore();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten key press event slot

   Here: Trigger send-by-keypress, help key handling and save options.

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if (stw::opensyde_gui_logic::C_HeHandler::h_CheckHelpKey(opc_KeyEvent) == true)
   {
      stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
         this->metaObject()->className());
   }
   else if (this->mpc_Ui->pc_GeneratorWidget->CheckAndHandleKey(opc_KeyEvent->text()) == true)
   {
      //Accepted
   }
   // Save on Ctrl+S
   else if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_S)) &&
            (opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      this->mpc_Ui->pc_TitleBarWidget->SaveConfig();
   }
   // Save as on F12
   else if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_F12))
   {
      this->mpc_Ui->pc_TitleBarWidget->SaveAsConfig();
   }
   // Search on F3
   else if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_F3))
   {
      if (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false)
      {
         // Search forward on F3
         this->mpc_Ui->pc_TraceWidget->SearchNext();
      }
      else
      {
         // Search backward on Shift F3
         this->mpc_Ui->pc_TraceWidget->SearchPrev();
      }
   }
   // Set focus to SearchBar on Ctrl + F
   else if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_F)) &&
            (opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      this->mpc_Ui->pc_TraceWidget->SearchBarFocus();
   }
   else
   {
      //Default handling
      QMainWindow::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden drag enter event slot

   Here: Accept external *.syde_cam file. Adopted from openSYDE GUI.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   QMainWindow::dragEnterEvent(opc_Event);

   if (mh_CheckMime(pc_MimeData) == true)
   {
      opc_Event->accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden drag move event slot

   Here: Accept external *.syde_cam file. Adopted from openSYDE GUI.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   QMainWindow::dragMoveEvent(opc_Event);

   if (mh_CheckMime(pc_MimeData) == true)
   {
      opc_Event->accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden drop event slot

   Here: Handle dropped *.syde_cam file. Adopted from openSYDE GUI.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::dropEvent(QDropEvent * const opc_Event)
{
   QString c_FilePath;
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   if (mh_CheckMime(pc_MimeData, &c_FilePath) == true)
   {
      // Check if path is a valid path with no irregular characters
      if (C_OscUtils::h_CheckValidFilePath(c_FilePath.toStdString().c_str()) == false)
      {
         C_OgeWiUtil::h_ShowPathInvalidError(this, c_FilePath);
      }
      else
      {
         const QFileInfo c_FileInfo(c_FilePath);

         // drop of project file
         if (c_FileInfo.suffix().compare("syde_cam", Qt::CaseInsensitive) == 0)
         {
            if (this->mpc_Ui->pc_TitleBarWidget->HandleProjectComparison() == true)
            {
               // Let all modules save their specific user settings before saving to file
               this->m_SaveUserSettings();

               // save user settings to not lose project dependent user settings
               C_UsHandler::h_GetInstance()->Save();

               // load configuration
               this->mpc_Ui->pc_TitleBarWidget->LoadConfig(c_FilePath);
            }
         }
         // drop of database file
         else
         {
            this->mpc_Ui->pc_SettingsWidget->OnDatabaseDropped(c_FilePath);
         }
      }
   }
   QMainWindow::dropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start the logging
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_StartLogging(void)
{
   int32_t s32_Bitrate;
   int32_t s32_Return;

   s32_Return = this->m_InitCan(s32_Bitrate);

   if (s32_Return != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR);
      QString c_Text;

      switch (s32_Return)
      {
      case C_RD_WR:
         c_Text = C_GtGetText::h_GetText("No valid CAN DLL found.");
         break;
      case C_CONFIG:
         c_Text = C_GtGetText::h_GetText("CAN DLL loading not successful.");
         break;
      case C_COM:
         c_Text = C_GtGetText::h_GetText("CAN DLL initialization not successful.");
         break;
      case C_WARN:
         c_MessageBox.SetType(C_OgeWiCustomMessage::eWARNING);
         c_Text = C_GtGetText::h_GetText("Used bitrate could not used for bus load calculation."
                                         " Bus load will not work.");
         break;
      default:
         break;
      }

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Starting CAN monitoring"));
      c_MessageBox.SetDescription(c_Text);
      c_MessageBox.Execute();
   }

   if ((s32_Return == C_NO_ERR) ||
       (s32_Return == C_WARN))
   {
      // Clear before start
      this->mc_ComDriver.ClearRxMessages();
      // Use the current configured bitrate of the CAN DLL for the bus load calculation
      this->mc_ComDriver.StartLogging(s32_Bitrate);
      this->mpc_Ui->pc_TraceWidget->SetCanBitrate(s32_Bitrate);
      this->mpc_CanThread->start();
      this->mpc_Ui->pc_GeneratorWidget->SetCommunicationStarted(true);
      this->mpc_Ui->pc_SettingsWidget->OnCommunicationStarted(true);
      this->mq_LoggingStarted = true;
   }
   else
   {
      this->mpc_Ui->pc_TraceWidget->StopLogging();
      this->mpc_Ui->pc_GeneratorWidget->SetCommunicationStarted(false);
      this->mpc_Ui->pc_SettingsWidget->OnCommunicationStarted(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Pause an active logging
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_PauseLogging(void)
{
   this->mc_ComDriver.PauseLogging();
   this->mpc_Ui->pc_GeneratorWidget->SetCommunicationStarted(false);
   this->mc_ComDriver.RemoveAllCyclicCanMessages();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Continue of a paused logging
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_ContinueLogging(void)
{
   this->mc_ComDriver.ContinueLogging();
   this->mpc_Ui->pc_GeneratorWidget->SetCommunicationStarted(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Stop the current logging
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_StopLogging(void)
{
   //Stop playing
   this->mq_LoggingStarted = false;
   this->mpc_CanThread->requestInterruption();
   this->mpc_CanThread->wait();
   this->mc_ComDriver.StopLogging();
   this->m_CloseCan();
   this->mpc_Ui->pc_GeneratorWidget->SetCommunicationStarted(false);
   this->mc_ComDriver.RemoveAllCyclicCanMessages();
   this->mpc_Ui->pc_SettingsWidget->OnCommunicationStarted(false);
   //Clear bitrate
   this->mpc_Ui->pc_TraceWidget->SetCanBitrate(0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Short function description

   Detailed function description (optional). The function shall be described detailed if it is not described completely
   by short function description and parameter description.

   \param[in]       ou8_Aa     Detailed input parameter description
   \param[out]      opu32_Bb   Detailed output parameter description
   \param[in,out]   opu16_Cc   Detailed input/output parameter description

   \return
   Type of return values, e.g. STW error codes

   \retval   Return value 1   Detailed description of 1st return value
   \retval   Return value 2   Detailed description of 2nd return value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_ClearData()
{
   this->mc_ComDriver.ClearData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of com driver and CAN DLL

   \param[out]  ors32_Bitrate    Current set bitrate of CAN DLL

   \return
   C_NO_ERR    CAN initialized
   C_RD_WR     CAN DLL not found
   C_CONFIG    Could not open DLL
   C_COM       Could not initialize CAN
   C_WARN      Bitrate could not be read
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamMainWindow::m_InitCan(int32_t & ors32_Bitrate)
{
   QString c_DllPath;
   int32_t s32_Return = C_RD_WR;
   QFileInfo c_File;

   // Initialize
   ors32_Bitrate = 0;

   // Get absolute DLL path (resolve variables and make absolute if it is relative ant not empty)
   c_DllPath = C_CamProHandler::h_GetInstance()->GetCanDllPath();
   if (c_DllPath.isEmpty() == false)
   {
      c_DllPath = C_CamUti::h_GetResolvedAbsolutePathFromExe(c_DllPath);
   }
   c_File.setFile(c_DllPath);

   if ((c_File.exists() == true) &&
       (c_File.isFile() == true))
   {
      // Open the DLL
      c_DllPath = c_DllPath.replace("/", "\\");
      s32_Return = this->mpc_CanDllDispatcher->DLL_Open(c_DllPath.toStdString().c_str());

      if (s32_Return == C_NO_ERR)
      {
         // Init the CAN with the current configured bitrate
         s32_Return = this->mpc_CanDllDispatcher->CAN_Init();

         if (s32_Return == C_NO_ERR)
         {
            stw::can::T_STWCAN_Status c_Status;

            // Get the bitrate of the CAN DLL
            s32_Return = this->mpc_CanDllDispatcher->CAN_Status(c_Status);
            if (s32_Return == C_NO_ERR)
            {
               ors32_Bitrate = c_Status.iActBitrate;
            }
            else
            {
               s32_Return = C_WARN;
               ors32_Bitrate = 0;
            }
         }
         else
         {
            // Error
            s32_Return = C_COM;
         }
      }
      else
      {
         // Error
         s32_Return = C_CONFIG;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Stops communication and close DLL
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_CloseCan(void)
{
   this->mpc_CanDllDispatcher->CAN_Exit();
   this->mpc_CanDllDispatcher->DLL_Close();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle initial project loading
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_LoadInitialProject(void)
{
   const QStringList c_Projects = C_UsHandler::h_GetInstance()->GetRecentProjects();
   bool q_ProjectLoadSuccess = false;

   // Initial project from command line
   if (QApplication::arguments().size() > 1)
   {
      // use title bar load configuration to inform user if it failed
      if (this->mpc_Ui->pc_TitleBarWidget->LoadConfig(QApplication::arguments().at(1).toStdString().c_str()) ==
          C_NO_ERR)
      {
         q_ProjectLoadSuccess = true;
      }
   }

   // if it failed: check all recent projects
   if (q_ProjectLoadSuccess == false)
   {
      int32_t s32_Pos;

      // try to load projects from recent project list (stop on success)
      for (s32_Pos = 0; s32_Pos < c_Projects.size(); ++s32_Pos)
      {
         // directly use project handler because we do not want an error message for every fail
         if (C_CamProHandler::h_GetInstance()->LoadFromFile(c_Projects.at(s32_Pos).toStdString().c_str()) == C_NO_ERR)
         {
            q_ProjectLoadSuccess = true;
            break;
         }
      }

      // if no recent project could be loaded use default new project
      if (q_ProjectLoadSuccess == false)
      {
         C_CamProHandler::h_GetInstance()->Clear();
      }

      // update recent projects menu to remove files that could not be loaded (already got called in cmd case)
      this->mpc_Ui->pc_TitleBarWidget->UpdateRecentProjectsAndWindowTitle();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_LoadUserSettings(void)
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
   this->m_OnExpandMessageGen(C_UsHandler::h_GetInstance()->GetMessageGenIsExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_SaveUserSettings(void)
{
   // Modules with own user settings
   this->mpc_Ui->pc_TraceWidget->SaveUserSettings();
   this->mpc_Ui->pc_GeneratorWidget->SaveUserSettings();
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
   // update to actual value if message generator is not collapsed
   if (C_UsHandler::h_GetInstance()->GetMessageGenIsExpanded() == true)
   {
      const QList<int32_t> c_Sizes = this->mpc_Ui->pc_SplitterMessageGen->sizes();
      if (c_Sizes.size() > 1)
      {
         C_UsHandler::h_GetInstance()->SetSplitterMessageGenVertical(c_Sizes.at(1));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if mime valid

   Adopted from openSYDE GUI main window implementation.

   \param[in]      opc_Mime      Mime to check
   \param[in,out]  opc_FilePath  Optional parameter for file path output if valid

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMainWindow::mh_CheckMime(const QMimeData * const opc_Mime, QString * const opc_FilePath)
{
   bool q_Retval = false;

   if (opc_Mime != NULL)
   {
      if (opc_Mime->hasUrls() == true)
      {
         QStringList c_PathList;
         const QList<QUrl> c_UrlList = opc_Mime->urls();

         // extract the local paths of the files
         for (int32_t s32_It = 0; s32_It < c_UrlList.size(); ++s32_It)
         {
            c_PathList.append(c_UrlList.at(s32_It).toLocalFile());
         }
         if (c_PathList.size() == 1)
         {
            QFileInfo c_File;
            c_File.setFile(c_PathList[0]);
            if (c_File.exists() == true)
            {
               if ((c_File.suffix().compare("syde_cam", Qt::CaseInsensitive) == 0) ||
                   (c_File.suffix().compare("syde_sysdef", Qt::CaseInsensitive) == 0) ||
                   (c_File.suffix().compare("syde", Qt::CaseInsensitive) == 0) ||
                   (c_File.suffix().compare("dbc", Qt::CaseInsensitive) == 0))
               {
                  q_Retval = true;
                  if (opc_FilePath != NULL)
                  {
                     *opc_FilePath = c_PathList[0];
                  }
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function for continuous calling by thread.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::mh_ThreadFunc(void * const opv_Instance)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_CamMainWindow * const pc_Instance = reinterpret_cast<C_CamMainWindow *>(opv_Instance);

   tgl_assert(pc_Instance != NULL);
   if (pc_Instance != NULL)
   {
      pc_Instance->m_ThreadFunc();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function for continuous calling by thread.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_ThreadFunc(void)
{
   // Get and spread the CAN messages to all registered loggers
   this->mc_ComDriver.DistributeMessages();

   //rescind CPU time to other threads ...
   stw::tgl::TglSleep(1);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Expand or collapse settings.

   \param[in]  oq_Expand   true: expand settings subsections
                           false: collapse settings subsections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnExpandSettings(const bool oq_Expand)
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
void C_CamMainWindow::m_OnSettingsSplitterHandleDoubleClick(void) const
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
void C_CamMainWindow::m_OnSettingsSplitterMoved(const int32_t & ors32_Pos, const int32_t & ors32_Index)
{
   const QList<int32_t> & rc_Sizes = this->mpc_Ui->pc_SplitterSettings->sizes();

   if (ors32_Index == 1)
   {
      if (rc_Sizes.size() > 1)
      {
         // is actual size between boundaries?
         if ((rc_Sizes.at(1) <= C_CamMainWindow::mhs32_SETTINGS_SPLITTER_MAX) && (rc_Sizes.at(1) > 35))
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
/*! \brief  Expand or collapse message generator widget.

   \param[in]  oq_Expand   true: expand message generator subsections
                           false: message generator subsections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnExpandMessageGen(const bool oq_Expand)
{
   if (oq_Expand == true)
   {
      // expand
      this->mpc_Ui->pc_SplitterMessageGen
      ->SetSecondSegment(std::max(mhs32_MESSAGE_GEN_SPLITTER_MAX,
                                  C_UsHandler::h_GetInstance()->GetSplitterMessageGenVertical()));
   }
   else
   {
      // remember segment size (only when they were expanded, i.e. do not overwrite on tool start)
      if (C_UsHandler::h_GetInstance()->GetMessageGenIsExpanded() == true)
      {
         {
            const QList<int32_t> c_Sizes = this->mpc_Ui->pc_SplitterMessageGen->sizes();

            if (c_Sizes.size() > 1)
            {
               C_UsHandler::h_GetInstance()->SetSplitterMessageGenVertical(c_Sizes.at(1));
            }
         }
      }
      //collapse (because childrenCollapsible == false the following results in minimum size)
      this->mpc_Ui->pc_SplitterMessageGen->SetSecondSegment(0);
   }

   // save state (after expanding!)
   C_UsHandler::h_GetInstance()->SetMessageGenIsExpanded(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle double click of message generator splitter.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnMessageGenSplitterHandleDoubleClick(void) const
{
   // toggle splitter expanded state
   this->mpc_Ui->pc_GeneratorWidget->ExpandMessageGen(!C_UsHandler::h_GetInstance()->GetMessageGenIsExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for splitter move.

   If message generator section gets collapsed, do not really collapse but minimize it.

   \param[in]  ors32_Pos      New position of splitter (distance to left border of widget)
   \param[in]  ors32_Index    Index of splitter widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnMessageGenSplitterMoved(const int32_t & ors32_Pos, const int32_t & ors32_Index)
{
   const QList<int32_t> & rc_Sizes = this->mpc_Ui->pc_SplitterMessageGen->sizes();

   if (ors32_Index == 1)
   {
      if (rc_Sizes.size() > 1)
      {
         // is actual size between boundaries?
         if ((rc_Sizes.at(1) <= mhs32_MESSAGE_GEN_SPLITTER_MAX) && (rc_Sizes.at(1) > 18))
         {
            if (ors32_Pos > this->ms32_MessageGenSplitterPosition)
            {
               // splitter moved from top to bottom -> check for collapse
               if (C_UsHandler::h_GetInstance()->GetMessageGenIsExpanded() == true)
               {
                  this->mpc_Ui->pc_GeneratorWidget->ExpandMessageGen(false);
               }
            }
            else if (ors32_Pos < this->ms32_MessageGenSplitterPosition)
            {
               // splitter moved from bottom to top -> check for expand
               if (C_UsHandler::h_GetInstance()->GetMessageGenIsExpanded() == false)
               {
                  this->mpc_Ui->pc_GeneratorWidget->ExpandMessageGen(true);
               }
            }
            else
            {
               // do nothing
            }
         }
      }

      // remember position for next round
      this->ms32_MessageGenSplitterPosition = ors32_Pos;

      // set splitter position
      if (C_UsHandler::h_GetInstance()->GetMessageGenIsExpanded() == false)
      {
         this->mpc_Ui->pc_SplitterMessageGen->SetSecondSegment(0);
      }
      else
      {
         this->mpc_Ui->pc_SplitterMessageGen->SetSecondSegment(
            std::max(mhs32_MESSAGE_GEN_SPLITTER_MAX,
                     static_cast<int32_t>(this->mpc_Ui->pc_SplitterMessageGen->height()) - ors32_Pos));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger send of cyclic message

   \param[in]  ou32_MessageIndex    Message index
   \param[in]  oq_Active            Flag if cyclic message is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_RegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active)
{
   const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(ou32_MessageIndex);

   if (pc_Message != NULL)
   {
      C_OscCanProtocol::E_Type e_ProtocolType = C_OscCanProtocol::eCAN_OPEN;
      C_CamDbHandler::h_GetInstance()->GetOscMessage(pc_Message->c_DataBaseFilePath.c_str(),
                                                     pc_Message->c_Name.c_str(),
                                                     pc_Message->q_ContainsValidHash,
                                                     pc_Message->u32_Hash,
                                                     &e_ProtocolType);

      //Prepare necessary parameter
      stw::opensyde_core::C_OscComDriverBaseCanMessage c_Message;
      c_Message.c_Msg = pc_Message->ToCanMessage();
      c_Message.u32_Interval = pc_Message->u32_CyclicTriggerTime;
      c_Message.u32_TimeToSend = 0UL;
      //Send
      if (oq_Active)
      {
         this->mc_ComDriver.AddCyclicCanMessage(c_Message, pc_Message->q_SetAutoSupportMode, e_ProtocolType);
      }
      else
      {
         this->mc_ComDriver.RemoveCyclicCanMessage(c_Message, pc_Message->q_SetAutoSupportMode, e_ProtocolType);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger send of specified message

   \param[in]  ou32_MessageIndex    Message index
   \param[in]  ou32_TimeToSend      Time to send this message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_SendMessage(const uint32_t ou32_MessageIndex, const uint32_t ou32_TimeToSend)
{
   const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(ou32_MessageIndex);

   if (pc_Message != NULL)
   {
      C_OscCanProtocol::E_Type e_ProtocolType = C_OscCanProtocol::eCAN_OPEN;
      C_CamDbHandler::h_GetInstance()->GetOscMessage(pc_Message->c_DataBaseFilePath.c_str(),
                                                     pc_Message->c_Name.c_str(),
                                                     pc_Message->q_ContainsValidHash,
                                                     pc_Message->u32_Hash,
                                                     &e_ProtocolType);

      //Prepare necessary parameter
      stw::opensyde_core::C_OscComDriverBaseCanMessage c_Message;
      c_Message.c_Msg = pc_Message->ToCanMessage();
      c_Message.u32_Interval = 0UL;
      c_Message.u32_TimeToSend = ou32_TimeToSend;
      //Send
      this->mc_ComDriver.SendCanMessage(c_Message, pc_Message->q_SetAutoSupportMode, e_ProtocolType);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for removing all registered cyclic messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_RemoveAllCyclicMessages(void)
{
   this->mc_ComDriver.RemoveAllCyclicCanMessages();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle remove the old configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnClearOldConfiguration(void) const
{
   this->mpc_Ui->pc_SettingsWidget->ClearConfiguration();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle loading the new configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnNewConfiguration(void)
{
   // Clear and stop trace on project load
   this->mpc_Ui->pc_TraceWidget->StopLogging();
   this->mpc_Ui->pc_TraceWidget->ClearData();

   //If that signal is connected directly this class has to derive of QObject
   C_CamDbHandler::h_GetInstance()->RemoveAllFiles();

   // Call all functions for loading user settings
   // Must be after constructor calls because of connect dependencies on the other modules
   // For example: The push buttons with states must send their toggle signal on setting the loaded user settings
   this->mpc_Ui->pc_TraceWidget->LoadUserSettings();
   this->mpc_Ui->pc_SettingsWidget->LoadUserSettings();
   this->mpc_Ui->pc_GeneratorWidget->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle file load start

   \param[in]  orc_File       Loaded file
   \param[in]  orc_OrgPath    Org path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnDatabaseLoadStarted(const QString & orc_File, const QString & orc_OrgPath)
{
   this->mc_CurrentLoadedFile = orc_File;
   this->mc_CurrentLoadedFileOrg = orc_OrgPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle file load finished

   Actions: Update currently loaded files

   \param[in]  os32_Result    Loading result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnDatabaseLoadFinished(const int32_t os32_Result)
{
   if ((this->mc_CurrentLoadedFile.isEmpty() == false) && (this->mc_CurrentLoadedFileOrg.isEmpty() == false))
   {
      //C_NO_ERR -> no err, C_WARN -> no significant error
      if ((os32_Result == C_NO_ERR) || (os32_Result == C_WARN))
      {
         stw::opensyde_gui_logic::C_SyvComMessageMonitor * const pc_MessageMonitor =
            this->mpc_Ui->pc_TraceWidget->GetMessageMonitor();

         if (pc_MessageMonitor != NULL)
         {
            const QFileInfo c_FileInfo(this->mc_CurrentLoadedFile);
            if (c_FileInfo.suffix().compare("dbc", Qt::CaseInsensitive) == 0)
            {
               //Check if already known
               if (C_CamDbHandler::h_GetInstance()->GetContainsDbc(this->mc_CurrentLoadedFileOrg) == false)
               {
                  C_CieConverter::C_CieCommDefinition c_DbcDefinition;
                  if (pc_MessageMonitor->GetDbcFile(this->mc_CurrentLoadedFile.toStdString().c_str(),
                                                    c_DbcDefinition) == C_NO_ERR)
                  {
                     //Add to currently loaded files
                     C_CamDbHandler::h_GetInstance()->AddDbcFile(this->mc_CurrentLoadedFileOrg, c_DbcDefinition);
                  }
                  else
                  {
                     tgl_assert(false);
                  }
               }
            }
            else if (c_FileInfo.suffix().compare("syde_sysdef", Qt::CaseInsensitive) == 0)
            {
               //Check if already known
               if (C_CamDbHandler::h_GetInstance()->GetContainsOsy(this->mc_CurrentLoadedFileOrg) == false)
               {
                  const stw::opensyde_core::C_OscSystemDefinition c_Tmp;
                  stw::opensyde_core::C_OscComMessageLoggerOsySysDefConfig c_SystemDefinition(c_Tmp, 0UL);
                  if (pc_MessageMonitor->GetOsySysDef(this->mc_CurrentLoadedFile.toStdString().c_str(),
                                                      c_SystemDefinition) == C_NO_ERR)
                  {
                     //Add to currently loaded files
                     C_CamDbHandler::h_GetInstance()->AddOsyFile(this->mc_CurrentLoadedFileOrg, c_SystemDefinition);
                  }
                  else
                  {
                     tgl_assert(false);
                  }
               }
            }
            else
            {
               //Unknown
            }
            //Notify signal widgets
            this->mpc_Ui->pc_GeneratorWidget->TriggerSignalReload();
         }
      }
   }
   this->m_CheckMessagesForLoadedDatabase(this->mc_CurrentLoadedFileOrg);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle file removed

   Actions: Update currently loaded files

   \param[in]  orc_File       File
   \param[in]  orc_OrgPath    Org path
   \param[in]  oq_IsUpdate    Is update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnDatabaseRemove(const QString & orc_File, const QString & orc_OrgPath, const bool oq_IsUpdate)
{
   Q_UNUSED(orc_File)
   //on removal QFileInfo might fail but the file should only be in one database
   C_CamDbHandler::h_GetInstance()->RemoveOsyFile(orc_OrgPath);
   C_CamDbHandler::h_GetInstance()->RemoveDbcFile(orc_OrgPath);
   //Handle messages
   if (oq_IsUpdate == false)
   {
      this->mpc_Ui->pc_GeneratorWidget->RemoveMessagesForFile(orc_OrgPath);
   }
   //Notify signal widgets
   this->mpc_Ui->pc_GeneratorWidget->TriggerSignalReload();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle database activation

   \param[in]  orc_File       File to change activation for (absolute path)
   \param[in]  orc_OrgPath    Original path (might be relative)
   \param[in]  oq_Active      New activation state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnActivateDatabase(const QString & orc_File, const QString & orc_OrgPath, const bool oq_Active)
{
   Q_UNUSED(orc_File)
   //the file should only be in one database
   C_CamDbHandler::h_GetInstance()->SetDbcActive(orc_OrgPath, oq_Active);
   C_CamDbHandler::h_GetInstance()->SetOsyActive(orc_OrgPath, oq_Active);
   //Handle messages
   if (oq_Active == false)
   {
      this->mpc_Ui->pc_GeneratorWidget->RemoveMessagesForFile(orc_OrgPath);
   }
   else
   {
      this->m_CheckMessagesForLoadedDatabase(orc_OrgPath);
   }
   //Notify signal widgets
   this->mpc_Ui->pc_GeneratorWidget->TriggerSignalReload();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Replace the openSYDE bus index

   \param[in]  orc_File       File to change bus index for (absolute path)
   \param[in]  orc_OrgPath    Original path (might be relative)
   \param[in]  ou32_BusIndex  New bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnOsyChangeBus(const QString & orc_File, const QString & orc_OrgPath,
                                       const uint32_t ou32_BusIndex)
{
   bool q_AnyChange = true;

   Q_UNUSED(orc_File)

   C_CamDbHandler::h_GetInstance()->ReplaceOsyBusIndex(orc_OrgPath, ou32_BusIndex, q_AnyChange);

   if (q_AnyChange)
   {
      //Handle messages
      this->mpc_Ui->pc_GeneratorWidget->RemoveMessagesForFile(orc_OrgPath);
      //Notify signal widgets
      this->mpc_Ui->pc_GeneratorWidget->TriggerSignalReload();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for a change of the CAN dll configuration

   Check and update of current CAN bitrate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_OnCanDllConfigChange(void)
{
   if (this->mq_LoggingStarted == true)
   {
      int32_t s32_Result;

      stw::can::T_STWCAN_Status c_Status;
      int32_t s32_Bitrate;

      // Get the bitrate of the CAN DLL
      s32_Result = this->mpc_CanDllDispatcher->CAN_Status(c_Status);
      if (s32_Result == C_NO_ERR)
      {
         s32_Bitrate = c_Status.iActBitrate;
      }
      else
      {
         C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR);
         c_MessageBox.SetType(C_OgeWiCustomMessage::eWARNING);
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Starting CAN monitoring"));
         c_MessageBox.SetDescription(C_GtGetText::h_GetText("Used bitrate could not used for bus load calculation."
                                                            " Bus load will not work."));
         c_MessageBox.Execute();

         s32_Bitrate = 0;
      }

      this->mc_ComDriver.UpdateBitrate(s32_Bitrate);
      this->mpc_Ui->pc_TraceWidget->SetCanBitrate(s32_Bitrate);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check messages for loaded database

   \param[in]  orc_DatabasePath  Database path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_CheckMessagesForLoadedDatabase(const QString & orc_DatabasePath)
{
   const std::vector<uint32_t> c_Indices = C_CamProHandler::h_GetInstance()->GetInvalidMessagesFromDatabase(
      orc_DatabasePath);

   this->m_DisplayCheckMessagesDialog(orc_DatabasePath, c_Indices);

   this->m_CheckForLastDatabaseLoaded(orc_DatabasePath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check for last database loaded

   \param[in]  orc_DatabasePath  Database path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_CheckForLastDatabaseLoaded(const QString & orc_DatabasePath)
{
   const std::vector<C_CamProDatabaseData> & rc_Dbs = C_CamProHandler::h_GetInstance()->GetDatabases();

   if (rc_Dbs.size() > 0UL)
   {
      const C_CamProDatabaseData & rc_Db = rc_Dbs[rc_Dbs.size() - 1UL];
      if (rc_Db.c_Name == orc_DatabasePath)
      {
         const std::vector<uint32_t> c_Indices = C_CamProHandler::h_GetInstance()->GetInvalidMessagesWithNoDatabase();
         this->m_DisplayCheckMessagesDialog(orc_DatabasePath, c_Indices);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Display check messages dialog

   \param[in]  orc_DatabasePath  Database path
   \param[in]  orc_Indices       Indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMainWindow::m_DisplayCheckMessagesDialog(const QString & orc_DatabasePath,
                                                   const std::vector<uint32_t> & orc_Indices)
{
   if (orc_Indices.size() > 0UL)
   {
      QString c_Details;
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eINFORMATION);

      c_Message.SetHeading(C_GtGetText::h_GetText("Message Generator consistency check"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Inconsistent messages in message generator found. "
                                                      "These are removed from message generator. "));

      c_Details = C_GtGetText::h_GetText("Following messages are removed from message generator: \n");

      for (uint32_t u32_It = 0UL; u32_It < orc_Indices.size(); ++u32_It)
      {
         QString c_Entry;
         const C_CamProMessageData * const pc_Message =
            C_CamProHandler::h_GetInstance()->GetMessageConst(orc_Indices[u32_It]);
         if (pc_Message != NULL)
         {
            const QString c_MessageName = C_CamProHandler::h_GetCompleteMessageName(*pc_Message);
            c_Entry = static_cast<QString>("%1 (%2)\n").arg(c_MessageName).arg(C_Uti::h_GetValueAsHex(
                                                                                  pc_Message->u32_Id));
         }
         c_Details.append(c_Entry);
      }

      c_Message.SetDetails(c_Details);

      c_Message.Execute();
      //Handle messages
      this->mpc_Ui->pc_GeneratorWidget->RemoveMessagesForFile(orc_DatabasePath, &orc_Indices);
      //Notify signal widgets
      this->mpc_Ui->pc_GeneratorWidget->TriggerSignalReload();
   }
}
