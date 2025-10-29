//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the process of configuring all nodes.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "constants.hpp"
#include "C_Uti.hpp"
#include "C_SyvDcWidget.hpp"
#include "ui_C_SyvDcWidget.h"
#include "C_OscNodeProperties.hpp"
#include "C_GtGetText.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "TglUtils.hpp"
#include "TglTime.hpp"
#include "C_SclString.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OscUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SyvDcWidget::mhs32_INDEX_CONFIGURATION_ALL_CONNECTED_INTERFACES = 1;
const int32_t C_SyvDcWidget::mhs32_INDEX_CONFIGURATION_ONLY_USED_INTERFACES = 0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent       Reference to parent
   \param[in,out]  ou32_ViewIndex   View index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcWidget::C_SyvDcWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32_t ou32_ViewIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDcWidget),
   mpc_ParentDialog(&orc_Parent),
   mpc_DcSequences(NULL),
   mu32_ViewIndex(ou32_ViewIndex),
   mq_InitializationFinished(false),
   mu32_TempBitrate(0U),
   mq_SameBitrates(false),
   mu32_BusIndex(0U),
   mu8_BusId(0U),
   mu64_BitRate(0U),
   mq_DisconnectNecessary(false),
   me_BusType(C_OscSystemBus::eCAN),
   mq_SecurityFeatureUsed(false),
   me_Step(eSCANCANENTERFLASHLOADER),
   mc_ReportText("")
{
   const C_PuiSvData * pc_View;

   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mpc_ParentDialog->SetWidget(this);
   //Register close
   this->mpc_ParentDialog->SetNotifyAndBlockClose(true);

   // init the gui
   this->mpc_Ui->pc_PbBackToScan->setEnabled(false);
   this->mpc_Ui->pc_BushButtonOk->setVisible(false);

   this->InitText();

   //Deactivate debug text
   this->mpc_Ui->pc_GroupBoxNoExistingNodes->setTitle("");
   this->mpc_Ui->pc_GroupBoxStartScan->setTitle("");

   //Init
   m_InitScanScreen();

   // Get the connected bus id
   pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      //No check for connected because error check passed
      this->mu32_BusIndex = pc_View->GetOscPcData().GetBusIndex();
      const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(this->mu32_BusIndex);

      if (pc_Bus != NULL)
      {
         this->mu8_BusId = pc_Bus->u8_BusId;
         this->me_BusType = pc_Bus->e_Type;
         this->mu64_BitRate = pc_Bus->u64_BitRate;

         this->mpc_Ui->pc_ComboBoxBitRate->setVisible(this->me_BusType == C_OscSystemBus::eCAN);
         this->mpc_Ui->pc_LabelBitRate->setVisible(this->me_BusType == C_OscSystemBus::eCAN);

         //Update title
         this->mpc_ParentDialog->SetTitle(pc_Bus->c_Name.c_str());
      }
   }

   //Deactivate working screens
   this->mpc_Ui->pc_GroupBoxScan->setVisible(true);
   this->mpc_Ui->pc_PushButtonScan->setVisible(true);

   this->mpc_Ui->pc_GroupBoxAssignment->setVisible(false);
   this->mpc_Ui->pc_PushButtonConfigure->setVisible(false);

   this->mpc_Ui->pc_GroupBoxReport->setVisible(false);
   this->mpc_Ui->pc_PbBackToScan->setVisible(false);

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SyvDcWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SyvDcWidget::m_CancelClicked);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvDcWidget::m_Timer);
   // Progress signals
   // We need a signal which is Q_EMITted in the other thread
   // We have to register the type and use the queued connections. Auto is default, but did not work.
   qRegisterMetaType<uint32_t>("uint32_t");
   qRegisterMetaType<int32_t>("int32_t");
   qRegisterMetaType<uint8_t>("uint8_t");

   connect(this->mpc_Ui->pc_ListWidgetExistingNodesAssignment, &C_SyvDcExistingNodeList::SigConnect, this,
           &C_SyvDcWidget::m_AssignmentConnect);
   connect(this->mpc_Ui->pc_ListWidgetExistingNodesAssignment, &C_SyvDcExistingNodeList::SigDisconnect, this,
           &C_SyvDcWidget::m_AssignmentDisconnect);
   connect(this->mpc_Ui->pc_PushButtonScan, &stw::opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SyvDcWidget::m_StartSearchProper);
   connect(this->mpc_Ui->pc_PushButtonConfigure, &stw::opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SyvDcWidget::m_StartConfigProper);
   connect(this->mpc_Ui->pc_PbBackToScan, &stw::opensyde_gui_elements::C_OgePubCancel::clicked, this,
           &C_SyvDcWidget::m_BackToScan);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxBitRate, static_cast<void (QComboBox::*)(
                                                            int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDcWidget::m_OnBitRateChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxConfigurationMode, static_cast<void (QComboBox::*)(
                                                                      int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDcWidget::m_OnDeviceConfigModeChanged);
   connect(this->mpc_ParentDialog, &C_OgePopUpDialog::SigCloseIgnored, this, &C_SyvDcWidget::m_OnCloseIgnored);
   connect(this->mpc_Ui->pc_ListWidgetConnectedNodesAssignment, &C_SyvDcConnectedNodeList::SigStartDrag,
           this->mpc_Ui->pc_ListWidgetExistingNodesAssignment, &C_SyvDcExistingNodeList::StartDrag);
   connect(this->mpc_Ui->pc_ListWidgetConnectedNodesAssignment, &C_SyvDcConnectedNodeList::SigStopDrag,
           this->mpc_Ui->pc_ListWidgetExistingNodesAssignment, &C_SyvDcExistingNodeList::StopDrag);

   this->mc_Timer.setInterval(10);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcWidget::~C_SyvDcWidget(void)
{
   this->CleanUp();
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Close things

   Can be used in case of a to late call of destructor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::CleanUp(void)
{
   if (this->mpc_DcSequences != NULL)
   {
      disconnect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfCanOpenSydeDevicesProgress, this,
                 &C_SyvDcWidget::m_UpdateProgressOfOpenSydeConfig);
      disconnect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfEthOpenSydeDevicesProgress, this,
                 &C_SyvDcWidget::m_UpdateProgressOfOpenSydeConfig);

      disconnect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfCanStwFlashloaderDevicesProgress, this,
                 &C_SyvDcWidget::m_UpdateProgressOfStwFlashloaderConfig);
      disconnect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfOpenSydeDevicesState, this,
                 &C_SyvDcWidget::m_UpdateStateOfOpenSydeConfig);
      disconnect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfCanStwFlashloaderDevicesState, this,
                 &C_SyvDcWidget::m_UpdateStateOfStwFlashloaderConfig);
   }

   //Stop timer just in case
   this->mc_Timer.stop();
   delete mpc_DcSequences;
   mpc_DcSequences = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::InitText(void)
{
   // set title
   this->mpc_ParentDialog->SetSubTitle(static_cast<QString>(C_GtGetText::h_GetText("Device Configuration")));

   this->mpc_Ui->pc_LabelHeadingReport->setText(static_cast<QString>(C_GtGetText::h_GetText("Report")));
   this->mpc_Ui->pc_LabelHeadingBitrate->setText(static_cast<QString>(C_GtGetText::h_GetText("Settings")));
   this->mpc_Ui->pc_LabelConfigurationMode->setText(static_cast<QString>(C_GtGetText::h_GetText("Configuration Mode")));
   this->mpc_Ui->pc_PushButtonScan->setText(static_cast<QString>(C_GtGetText::h_GetText("Scan for Devices")));
   this->mpc_Ui->pc_LabelStartScan->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                    "Click on \"Scan for Devices\" ...")));
   this->mpc_Ui->pc_LabelNoExistingNodes->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "No valid Topology nodes found.")));
   this->mpc_Ui->pc_LabelBitRate->setText(static_cast<QString>(C_GtGetText::h_GetText("Bitrate of Connected Devices")));
   this->mpc_Ui->pc_PbBackToScan->setText(static_cast<QString>(C_GtGetText::h_GetText("Back to Scan")));
   this->mpc_Ui->pc_LabelHeadingAssignment->setText(static_cast<QString>(C_GtGetText::h_GetText("Assignment")));
   this->mpc_Ui->pc_LabelAssignment->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                     "Assign all connected devices to Topology nodes via drag&drop"
                                                                     " and click on \"Configure Devices\".")));
   this->mpc_Ui->pc_PushButtonConfigure->setText(static_cast<QString>(C_GtGetText::h_GetText("Configure Devices")));
   this->mpc_Ui->pc_LabelProgressScan->setText(static_cast<QString>(C_GtGetText::h_GetText("Scan")));
   this->mpc_Ui->pc_LabelProgressAssignmnet->setText(static_cast<QString>(C_GtGetText::h_GetText("Assignment")));
   this->mpc_Ui->pc_LabelProgressConfiguration->setText(static_cast<QString>(C_GtGetText::h_GetText("Configuration")));
   this->mpc_Ui->pc_LabelProgressFinished->setText(static_cast<QString>(C_GtGetText::h_GetText("Finished")));
   this->mpc_Ui->pc_ComboBoxConfigurationMode->clear();

   this->mpc_Ui->pc_BushButtonCancel->setText(static_cast<QString>(C_GtGetText::h_GetText("Cancel")));
   this->mpc_Ui->pc_BushButtonOk->setText(static_cast<QString>(C_GtGetText::h_GetText("OK")));

   // Order is important!
   // Index 0 = all connected interfaces in the system.
   // Index 1 = only the for the device configuration used interface
   this->mpc_Ui->pc_ComboBoxConfigurationMode->addItem(C_GtGetText::h_GetText("Only directly connected interfaces"));
   this->mpc_Ui->pc_ComboBoxConfigurationMode->addItem(C_GtGetText::h_GetText("All connected interfaces"));

   //Tool Tips
   this->mpc_Ui->pc_LabelConfigurationMode->SetToolTipInformation(
      C_GtGetText::h_GetText("Configuration Mode"),
      C_GtGetText::h_GetText(
         "Decide which device interfaces will be configured.\n \n"
         " - Only directly connected interfaces: \n"
         "   All interfaces which are connected to the bus are used by the current device configuration.\n\n"
         " - All connected interfaces: \n"
         "   All interfaces which are connected to any bus in the SYSTEM DEFINITION.\n"
         "   Attention: This may lead to inconsistent configurations on other connected buses. \n"
         "   (e.g.: CAN bitrate of configured interface does not match the bitrate of other nodes connected \n"
         "   to same bus)"));
   this->mpc_Ui->pc_LabelBitRate->SetToolTipInformation(
      C_GtGetText::h_GetText("Bitrate of Connected Devices"),
      C_GtGetText::h_GetText("Specify current bitrate of connected devices. If you defined another bitrate in SYSTEM "
                             "DEFINITION a successful device configuration will change the bitrate."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::paintEvent(QPaintEvent * const opc_Event)
{
   stw::opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);
   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: handle escape key

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (opc_Event->key() == static_cast<int32_t>(Qt::Key_Escape))
   {
      if (m_CheckQuitPossible() == true)
      {
         m_DoCompleteDisconnect();
         QWidget::keyPressEvent(opc_Event);
      }
      else
      {
         opc_Event->accept();
         m_InformUserAboutAbortedClose();
      }
   }
   else
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init com driver

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDcWidget::m_InitSequence(void)
{
   QString c_Message;
   int32_t s32_Retval = C_NO_ERR;

   // Sequence initialization
   if (this->mpc_DcSequences == NULL)
   {
      this->mpc_DcSequences = new C_SyvDcSequences();

      connect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfCanOpenSydeDevicesProgress, this,
              &C_SyvDcWidget::m_UpdateProgressOfOpenSydeConfig, Qt::QueuedConnection);
      connect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfEthOpenSydeDevicesProgress, this,
              &C_SyvDcWidget::m_UpdateProgressOfOpenSydeConfig, Qt::QueuedConnection);

      connect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfCanStwFlashloaderDevicesProgress, this,
              &C_SyvDcWidget::m_UpdateProgressOfStwFlashloaderConfig, Qt::QueuedConnection);
      connect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfOpenSydeDevicesState, this,
              &C_SyvDcWidget::m_UpdateStateOfOpenSydeConfig, Qt::QueuedConnection);
      connect(this->mpc_DcSequences, &C_SyvDcSequences::SigRunConfCanStwFlashloaderDevicesState, this,
              &C_SyvDcWidget::m_UpdateStateOfStwFlashloaderConfig, Qt::QueuedConnection);
   }

   if (this->mpc_DcSequences != NULL) // required anyway by PC lint
   {
      if (this->mpc_DcSequences->IsInitialized() == false)
      {
         s32_Retval = this->mpc_DcSequences->InitDcSequences(this->mu32_ViewIndex);
      }
   }

   C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);

   switch (s32_Retval)
   {
   case C_NO_ERR:
      break;
   case C_CONFIG:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Invalid SYSTEM DEFINITION/View configuration."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_RD_WR:
      c_Message =
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Configured communication DLL does not exist or DLL could not be opened."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_OVERFLOW:
      c_Message =
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Unknown transport protocol or unknown diagnostic server for at least one node."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_NOACT:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("System View is invalid. Action cannot be performed."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_COM:
      {
         bool q_IsEthernet = false;

         //Check if ethernet
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

         if (pc_View != NULL)
         {
            if (pc_View->GetOscPcData().GetConnected())
            {
               const C_OscSystemBus * const pc_Bus =
                  C_PuiSdHandler::h_GetInstance()->GetOscBus(pc_View->GetOscPcData().GetBusIndex());
               if (pc_Bus != NULL)
               {
                  if (pc_Bus->e_Type == C_OscSystemBus::eETHERNET)
                  {
                     q_IsEthernet = true;
                  }
               }
            }
         }
         if (q_IsEthernet)
         {
            c_Message =
               static_cast<QString>(
                  C_GtGetText::h_GetText(
                     "Ethernet initialization failed. Check your Ethernet adapter settings in Windows system network configuration."));
            c_MessageBox.SetCustomMinHeight(180, 180);
         }
         else
         {
            c_Message =
               static_cast<QString>(
                  C_GtGetText::h_GetText(
                     "CAN initialization failed. Check your PC CAN interface configuration (System View setup - "
                     "double-click on PC)."));
            c_MessageBox.SetCustomMinHeight(200, 200);
         }
      }
      break;
   case C_CHECKSUM:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Internal buffer overflow detected."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_RANGE:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Routing configuration failed."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_BUSY:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("System view error detected."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_UNKNOWN_ERR:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Wrapped error of internal function call."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_WARN:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Internal error."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   default:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   }

   if (s32_Retval != C_NO_ERR)
   {
      c_MessageBox.SetDescription(c_Message);
      m_CleanUpScan();
      c_MessageBox.Execute();

      // Simplify return value
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start search
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_StartSearchProper(void)
{
   //Init com driver
   int32_t s32_Return;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   //Prepare UI
   this->mpc_Ui->pc_BushButtonCancel->setEnabled(false);
   this->mpc_Ui->pc_ComboBoxBitRate->setEnabled(false);
   this->mpc_Ui->pc_ComboBoxConfigurationMode->setEnabled(false);
   this->mpc_Ui->pc_PushButtonScan->setEnabled(false);
   this->mpc_Ui->pc_GroupBoxStartScan->setVisible(true);
   this->mpc_Ui->pc_ListWidgetConnectedNodesScan->setVisible(false);
   // Let the user know, when the tool is occupied with itself before communicating with the bus
   this->mpc_Ui->pc_LabelStartScan->setText(C_GtGetText::h_GetText("Initializing communication interface..."));
   this->mpc_Ui->pc_LabelHeadingFoundDevices->setText(static_cast<QString>(C_GtGetText::h_GetText("Connected Devices")));

   this->mq_InitializationFinished = false;

   s32_Return = m_InitSequence();

   this->mc_FoundDevices.clear();

   if (this->mpc_DcSequences != NULL)
   {
      //No error report necessary here
      if (s32_Return == C_NO_ERR)
      {
         //Bit rate
         s32_Return = this->m_GetBitRateValue(this->mu32_TempBitrate);
         if (s32_Return == C_NO_ERR)
         {
            //Set step
            if (this->me_BusType == C_OscSystemBus::eCAN)
            {
               // Check if different bitrates are used
               this->mq_SameBitrates = (this->mu32_TempBitrate == (this->mu64_BitRate / 1000U));

               //Check scan
               this->me_Step = eSCANCANENTERFLASHLOADER;
               s32_Return = this->mpc_DcSequences->ScanCanEnterFlashloader(this->mu32_TempBitrate);
            }
            else
            {
               this->mq_SameBitrates = true;

               // No concrete enter flashloader function for Ethernet
               this->me_Step = eSCANETHGETINFOFROMOPENSYDEDEVICES;
               s32_Return = this->mpc_DcSequences->ScanEthGetInfoFromOpenSydeDevices();

               if (s32_Return == C_NO_ERR)
               {
                  // The thread was started, Ethernet sequence has no further initialization,
                  // the communication is running...
                  this->mpc_Ui->pc_LabelStartScan->setText(C_GtGetText::h_GetText("Scanning for devices..."));
                  this->mq_InitializationFinished = true;
               }
            }

            if (s32_Return == C_NO_ERR)
            {
               //Remember disconnect necessary on close
               this->mq_DisconnectNecessary = true;
               //Start polling
               this->mpc_Ui->pc_ProgressScan->SetProgress(25, false);
               this->mc_Timer.start();
            }
            else
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
               c_Message.SetHeading("Failed!");
               c_Message.SetDescription(C_GtGetText::h_GetText("Could not continue with step: enter Flashloader."));
               m_CleanUpScan();
               c_Message.SetCustomMinHeight(180, 180);
               c_Message.Execute();
            }
         }
         else
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
            c_Message.SetDescription(C_GtGetText::h_GetText("Unknown bitrate."));
            m_CleanUpScan();
            c_Message.SetCustomMinHeight(180, 180);
            c_Message.Execute();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up scan
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_CleanUpScan(void) const
{
   this->mpc_Ui->pc_BushButtonCancel->setEnabled(true);
   this->mpc_Ui->pc_ComboBoxBitRate->setEnabled(this->me_BusType == C_OscSystemBus::eCAN);
   this->mpc_Ui->pc_ComboBoxConfigurationMode->setEnabled(true);
   this->mpc_Ui->pc_PushButtonScan->setEnabled(true);
   this->mpc_Ui->pc_PushButtonScan->setFocus();
   //Latest possible cursor restoration
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scan finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ScanFinished(void)
{
   C_SclString c_NotTrimmedDevices = C_GtGetText::h_GetText("Not trimmed device types: \n");
   bool q_NotTrimmedDevicesFound = false;

   //evaluate results
   if (this->mc_FoundDevices.size() == 0)
   {
      //Zero
      this->mpc_Ui->pc_ProgressScan->SetProgress(0, false);
      this->mpc_Ui->pc_LabelStartScan->setText(C_GtGetText::h_GetText("No devices found."));
      m_InformUserAfterScan(C_GtGetText::h_GetText("No devices found! Check connection of connected devices and retry."));
   }
   else if (this->mc_FoundDevices.size() <
            static_cast<uint32_t>(this->mpc_Ui->pc_ListWidgetExistingNodes->GetCommunicatingNodeCount()))
   {
      //Less
      m_EnterScanErrorState();
      m_InformUserAfterScan(C_GtGetText::h_GetText("Fewer devices found than defined by the System View!"
                                                   " Check connection of connected devices and retry."));
   }
   else if (this->mc_FoundDevices.size() >
            static_cast<uint32_t>(this->mpc_Ui->pc_ListWidgetExistingNodes->GetCommunicatingNodeCount()))
   {
      //More
      m_EnterScanErrorState();
      m_InformUserAfterScan(C_GtGetText::h_GetText("More devices found than defined by the System View! "
                                                   "Connect only devices which are defined in the System View and retry."));
   }
   else
   {
      const bool q_SameSerialNumber = m_CheckSameSerialNumber();
      //Match
      if (q_SameSerialNumber == true)
      {
         //Same serial number error
         m_InformUserAfterScan(C_GtGetText::h_GetText(
                                  "Devices with duplicate serial numbers found! Make sure that each device "
                                  "is connected only through one interface to the device configuration and try again."));

         m_EnterScanErrorState();
      }
      else
      {
         //Switch screens
         this->mpc_Ui->pc_GroupBoxScan->setVisible(false);
         this->mpc_Ui->pc_PushButtonScan->setVisible(false);

         this->mpc_Ui->pc_GroupBoxAssignment->setVisible(true);
         this->mpc_Ui->pc_PushButtonConfigure->setVisible(true);

         m_InitAssignmentScreen();
         this->mpc_Ui->pc_LabelStartScan->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "Click on \"Scan for Devices\" ...")));
         //Progress
         this->mpc_Ui->pc_BopperleScan->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
         this->mpc_Ui->pc_ProgressScan->SetProgress(100, false);
         this->mpc_Ui->pc_BopperleAssignment->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
      }
   }

   m_CleanUpScan();

   //check if found device types are trimmed
   for (uint32_t u32_ItDevice1 = 0; u32_ItDevice1 < this->mc_FoundDevices.size(); ++u32_ItDevice1)
   {
      const C_OscDcDeviceInformation & rc_Device1 = this->mc_FoundDevices[u32_ItDevice1];

      //trim & check
      if (rc_Device1.c_DeviceName.Trim() != rc_Device1.c_DeviceName)
      {
         //not trimmed string found
         q_NotTrimmedDevicesFound = true;

         //add to list
         c_NotTrimmedDevices += "- \"" + rc_Device1.c_DeviceName + "\" \n";
      }
   }

   //evaluate status of trimmed check
   if (q_NotTrimmedDevicesFound == true)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eWARNING);
      c_Message.SetHeading(C_GtGetText::h_GetText("Device Type"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "Device type received from target is not trimmed. "
                                  "This could lead to visual misunderstanding while devices assignment."));
      c_Message.SetDetails(c_NotTrimmedDevices.c_str());
      c_Message.SetCustomMinHeight(230, 300);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if all found serial numbers are unique

   \return
   True  Not unique
   False All unique
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcWidget::m_CheckSameSerialNumber(void)
{
   bool q_Retval = false;

   //Match
   //Compare devices
   for (uint32_t u32_ItDevice1 = 0U; u32_ItDevice1 < this->mc_FoundDevices.size(); ++u32_ItDevice1)
   {
      const C_OscDcDeviceInformation & rc_Device1 = this->mc_FoundDevices[u32_ItDevice1];
      for (uint32_t u32_ItDevice2 = u32_ItDevice1 + 1U; u32_ItDevice2 < this->mc_FoundDevices.size(); ++u32_ItDevice2)
      {
         const C_OscDcDeviceInformation & rc_Device2 = this->mc_FoundDevices[u32_ItDevice2];

         if ((rc_Device1.c_SerialNumber.q_FsnSerialNumber == false) &&
             (rc_Device2.c_SerialNumber.q_FsnSerialNumber == false))
         {
            // Old format
            const int32_t s32_Diff = std::memcmp(&rc_Device1.c_SerialNumber.au8_SerialNumber[0],
                                                 &rc_Device2.c_SerialNumber.au8_SerialNumber[0], 6U);
            if (s32_Diff == 0)
            {
               // Same serial number
               // Special case: Different sub node id can have same same serial number due to a multi CPU node
               if (rc_Device1.u8_SubNodeId == rc_Device2.u8_SubNodeId)
               {
                  q_Retval = true;
               }
            }
         }
         else if ((rc_Device1.c_SerialNumber.q_FsnSerialNumber == true) &&
                  (rc_Device2.c_SerialNumber.q_FsnSerialNumber == true))
         {
            // Extended format
            if ((rc_Device1.c_SerialNumber.u8_SerialNumberManufacturerFormat ==
                 rc_Device2.c_SerialNumber.u8_SerialNumberManufacturerFormat) &&
                (rc_Device1.c_SerialNumber.c_SerialNumberExt == rc_Device2.c_SerialNumber.c_SerialNumberExt) &&
                (rc_Device1.u8_SubNodeId == rc_Device2.u8_SubNodeId))
            {
               // Same serial number with same sub node id
               // Special case: Different sub node id can have same same serial number due to a multi CPU node
               q_Retval = true;
            }
         }
         else
         {
            // Mix of POS and FSN format. Can not be the same
         }

         if (q_Retval == true)
         {
            break;
         }
      }
      if (q_Retval == true)
      {
         break;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enter scan error state

   \param[in]  orc_Text    Scan error text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_EnterScanErrorState(void)
{
   this->mpc_Ui->pc_ProgressScan->SetProgress(0, false);

   this->mpc_Ui->pc_ListWidgetConnectedNodesScan->setVisible(true);
   this->mpc_Ui->pc_ListWidgetConnectedNodesScan->SetData(this->mc_FoundDevices);
   //No drag
   this->mpc_Ui->pc_ListWidgetConnectedNodesScan->setDragEnabled(false);
   this->mpc_Ui->pc_GroupBoxStartScan->setVisible(false);
   this->mpc_Ui->pc_LabelHeadingFoundDevices->setText(
      static_cast<QString>(C_GtGetText::h_GetText("Connected Devices (%1)")).
      arg(this->mpc_Ui->pc_ListWidgetConnectedNodesScan->count()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_StartConfigProper(void)
{
   if (this->mpc_DcSequences != NULL)
   {
      int32_t s32_Result = C_NO_ERR;

      std::vector<C_SyvDcDeviceConfiguation> c_Configs =
         this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->GetConfigs();
      //Prepare
      this->mc_OpenSydeDeviceConfigurations.clear();
      this->mc_StwFlashloaderDeviceConfigurations.clear();

      //Append
      for (uint32_t u32_ItConfig = 0; (u32_ItConfig < c_Configs.size()) && (s32_Result == C_NO_ERR); ++u32_ItConfig)
      {
         C_OscNodeProperties::E_FlashLoaderProtocol e_Flashloader;
         C_SyvDcDeviceConfiguation & rc_Config = c_Configs[u32_ItConfig];
         s32_Result = this->mpc_DcSequences->FillDeviceConfig(rc_Config, e_Flashloader);
         if (s32_Result == C_NO_ERR)
         {
            if (e_Flashloader == C_OscNodeProperties::eFL_OPEN_SYDE)
            {
               this->mc_OpenSydeDeviceConfigurations.push_back(rc_Config);
            }
            else
            {
               //Flashloader type "none" is supported by the device structure, but not the UI
               tgl_assert(e_Flashloader == C_OscNodeProperties::eFL_STW);
               this->mc_StwFlashloaderDeviceConfigurations.push_back(rc_Config);
            }
         }
      }

      //Continue
      if (s32_Result == C_NO_ERR)
      {
         // Check the openSYDE device configurations
         s32_Result = this->mpc_DcSequences->CheckOpenSydeDevicesConfig(this->mc_OpenSydeDeviceConfigurations);

         if ((s32_Result == C_NO_ERR) ||
             (s32_Result == C_NOACT)) // No openSYDE devices to configure
         {
            if (this->mpc_DcSequences->IsAtLeastOneStwFlashloaderNodeActiveOnLocalBus() == true)
            {
               if (this->me_BusType == C_OscSystemBus::eCAN)
               {
                  // Run and check the STW flashloader configuration
                  s32_Result =
                     this->mpc_DcSequences->ConfCanStwFlashloaderDevices(this->mc_StwFlashloaderDeviceConfigurations);

                  if (s32_Result != C_NO_ERR)
                  {
                     C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                     c_Message.SetDescription(C_GtGetText::h_GetText("Could not configure STW Flashloader devices."));
                     c_Message.SetCustomMinHeight(180, 180);
                     c_Message.Execute();
                  }
               }
               else
               {
                  C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                  c_Message.SetDescription(C_GtGetText::h_GetText(
                                              "Could not configure STW Flashloader devices over Ethernet."));
                  c_Message.SetCustomMinHeight(180, 180);
                  c_Message.Execute();

                  s32_Result = C_NOACT;
               }
            }
            else
            {
               // No STW flashloader devices. Run and check the openSYDE configuration
               if (this->me_BusType == C_OscSystemBus::eCAN)
               {
                  s32_Result =
                     this->mpc_DcSequences->ConfCanOpenSydeDevices(this->mc_OpenSydeDeviceConfigurations,
                                                                   this->m_AreAllInterfacesToConfigure(),
                                                                   this->mq_SecurityFeatureUsed);
               }
               else
               {
                  s32_Result =
                     this->mpc_DcSequences->ConfEthOpenSydeDevices(this->mc_OpenSydeDeviceConfigurations,
                                                                   this->m_AreAllInterfacesToConfigure(),
                                                                   this->mq_SecurityFeatureUsed);
               }

               if (s32_Result != C_NO_ERR)
               {
                  C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                  c_Message.SetDescription(C_GtGetText::h_GetText("Could not configure openSYDE devices."));
                  c_Message.SetCustomMinHeight(180, 180);
                  c_Message.Execute();
               }
            }

            if (s32_Result == C_NO_ERR)
            {
               //Switch screens
               this->mpc_Ui->pc_GroupBoxAssignment->setVisible(false);
               this->mpc_Ui->pc_PushButtonConfigure->setVisible(false);

               this->mpc_Ui->pc_GroupBoxReport->setVisible(true);

               //Clear report
               this->m_ClearReportText();
               m_HandleConfigurationStarted();
               //Start
               if (this->me_BusType == C_OscSystemBus::eCAN)
               {
                  if (this->mpc_DcSequences->IsAtLeastOneStwFlashloaderNodeActiveOnLocalBus() == true)
                  {
                     this->me_Step = eCONFCANSTWFLASHLOADERDEVICES;
                  }
                  else
                  {
                     this->me_Step = eCONFCANOPENSYDEDEVICES;
                  }
               }
               else
               {
                  this->me_Step = eCONFETHOPENSYDEDEVICES;
               }

               this->mc_Timer.start();
               //Progress
               this->mpc_Ui->pc_BopperleScan->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
               this->mpc_Ui->pc_ProgressScan->SetProgress(100, false);
               this->mpc_Ui->pc_BopperleAssignment->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21,
                                                                         mc_STYLE_GUIDE_COLOR_13);
               this->mpc_Ui->pc_ProgressAssignment->SetProgress(100, false);
               this->mpc_Ui->pc_BopperleConfig->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
            }
         }
         else
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_Message.SetDescription(C_GtGetText::h_GetText("Could not configure openSYDE devices."));
            c_Message.SetCustomMinHeight(180, 180);
            c_Message.Execute();
         }
      }
      else
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText("Internal error FillDeviceConfig %1.")).
                                  arg(C_Uti::h_StwError(s32_Result)));
         c_Message.SetCustomMinHeight(180, 180);
         c_Message.Execute();
      }
   }
   else
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText("Internal error starting configuration.")));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Displays the configuration parameters
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigResult(void)
{
   uint32_t u32_DeviceMaxCount;
   uint32_t u32_DeviceCounter = 1UL;

   uint32_t u32_ConfigCounter;
   QString c_Text;

   std::vector<std::vector<C_SyvDcDeviceConfiguation> > c_SortedOpenSydeDeviceInfos;

   // In the event queue could be some emitted signals from thread left.
   // We need all information for showing the configuration result
   QApplication::processEvents();

   // Handling multi CPU nodes and group for same snr
   for (u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_OpenSydeDeviceConfigurations.size(); ++u32_ConfigCounter)
   {
      const C_SyvDcDeviceConfiguation & rc_Info = this->mc_OpenSydeDeviceConfigurations[u32_ConfigCounter];
      uint32_t u32_CompDeviceCounter;
      bool q_SameSnr = false;

      for (u32_CompDeviceCounter = 0U; u32_CompDeviceCounter < c_SortedOpenSydeDeviceInfos.size();
           ++u32_CompDeviceCounter)
      {
         tgl_assert(c_SortedOpenSydeDeviceInfos[u32_CompDeviceCounter].size() > 0);

         // The grouping is for snr, so the first element of a group is enough for comparison
         if (c_SortedOpenSydeDeviceInfos[u32_CompDeviceCounter][0].c_SerialNumber == rc_Info.c_SerialNumber)
         {
            q_SameSnr = true;
            c_SortedOpenSydeDeviceInfos[u32_CompDeviceCounter].push_back(rc_Info);
            // The device counter shall represent the entire devices, not the sub nodes
            break;
         }
      }

      if (q_SameSnr == false)
      {
         // A new group or a normal node
         std::vector<C_SyvDcDeviceConfiguation> c_NewGroup;
         c_NewGroup.push_back(rc_Info);
         c_SortedOpenSydeDeviceInfos.push_back(c_NewGroup);
      }
   }

   u32_DeviceMaxCount = static_cast<uint32_t>(c_SortedOpenSydeDeviceInfos.size() +
                                              this->mc_StwFlashloaderDeviceConfigurations.size());

   // Show configuration for all STW flashloader devices
   for (u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_StwFlashloaderDeviceConfigurations.size();
        ++u32_ConfigCounter)
   {
      std::vector<C_SyvDcDeviceConfiguation> c_StwNode;
      c_StwNode.push_back(this->mc_StwFlashloaderDeviceConfigurations[u32_ConfigCounter]);

      this->m_ShowConfigInfoOfDevice(c_StwNode,
                                     u32_DeviceMaxCount, u32_DeviceCounter, c_Text);
      ++u32_DeviceCounter;

      c_Text += "<br>";
   }

   // Show configuration for all openSYDE flashloader devices grouped by normal and squad nodes
   for (u32_ConfigCounter = 0U; u32_ConfigCounter < c_SortedOpenSydeDeviceInfos.size();
        ++u32_ConfigCounter)
   {
      this->m_ShowConfigInfoOfDevice(c_SortedOpenSydeDeviceInfos[u32_ConfigCounter], u32_DeviceMaxCount,
                                     u32_DeviceCounter, c_Text);
      ++u32_DeviceCounter;

      c_Text += "<br>";
   }

   this->m_UpdateReportText(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   handle back to scan action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_BackToScan(void)
{
   this->mpc_Ui->pc_PbBackToScan->setEnabled(false);
   this->mpc_Ui->pc_BushButtonCancel->setVisible(true);
   this->mpc_Ui->pc_BushButtonOk->setVisible(false);

   this->mpc_Ui->pc_GroupBoxScan->setVisible(true);
   this->mpc_Ui->pc_PushButtonScan->setVisible(true);

   this->mpc_Ui->pc_GroupBoxAssignment->setVisible(false);
   this->mpc_Ui->pc_PushButtonConfigure->setVisible(false);

   this->mpc_Ui->pc_GroupBoxReport->setVisible(false);
   this->mpc_Ui->pc_PbBackToScan->setVisible(false);

   this->mc_ServerStates.clear();

   this->CleanUp();
   m_InitScanScreen();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reporting the configuration of a node or node squad

   \param[in]      orc_Config            Vector of all configurations of a node squad or normal node
                                          In case of a normal node, the size must be 1
   \param[in]      ou32_DeviceMaxCount   Maximum number of
   \param[in]      ou32_DeviceCounter   Detailed output parameter description
   \param[in,out]  orc_Text   Detailed output parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigInfoOfDevice(const std::vector<C_SyvDcDeviceConfiguation> & orc_Config,
                                             const uint32_t ou32_DeviceMaxCount, const uint32_t ou32_DeviceCounter,
                                             QString & orc_Text)
{
   tgl_assert(orc_Config.size() > 0);
   tgl_assert(orc_Config[0].c_NodeIds.size() > 0);

   if ((this->mpc_DcSequences != NULL) &&
       (orc_Config.size() > 0) &&
       (orc_Config[0].c_NodeIds.size() > 0))
   {
      const C_OscProtocolDriverOsyNode c_FirstServerId(this->mu8_BusId,  orc_Config[0].c_NodeIds[0]);
      uint32_t u32_FirstNodeIndex;

      // Counter of devices
      orc_Text += static_cast<QString>("%1 %2 %3 %4<br>").arg(
         static_cast<QString>(C_GtGetText::h_GetText("Device")), QString::number(ou32_DeviceCounter),
         static_cast<QString>(C_GtGetText::h_GetText("of")), QString::number(ou32_DeviceMaxCount));

      // Serial number
      orc_Text +=
         "SN.: " +
         static_cast<QString>(orc_Config[0].c_SerialNumber.GetSerialNumberAsFormattedString().c_str()) +
         "<br>";

      if (this->mpc_DcSequences->GetNodeIndex(c_FirstServerId, u32_FirstNodeIndex) == true)
      {
         const C_OscNode * const pc_FirstNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_FirstNodeIndex);
         std::vector<uint32_t> c_SubNodeIndexes;
         std::vector<const C_OscNode *> c_SubNodes;
         std::vector<C_OscProtocolDriverOsyNode> c_ServerIds;
         uint32_t u32_SubNodeCounter;
         bool q_NodeSquad = false;

         if (orc_Config.size() > 1)
         {
            // It is a node squad. Get all nodes
            uint32_t u32_SquadIndex;
            const C_OscNodeSquad * pc_Squad;

            tgl_assert(C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(u32_FirstNodeIndex,
                                                                                       u32_SquadIndex) == C_NO_ERR);
            pc_Squad = C_PuiSdHandler::h_GetInstance()->GetOscNodeSquadConst(u32_SquadIndex);

            tgl_assert(pc_Squad != NULL);
            if (pc_Squad != NULL)
            {
               tgl_assert(pc_Squad->c_SubNodeIndexes.size() == orc_Config.size());

               c_SubNodes.reserve(pc_Squad->c_SubNodeIndexes.size());
               c_ServerIds.reserve(pc_Squad->c_SubNodeIndexes.size());

               for (u32_SubNodeCounter = 0U; u32_SubNodeCounter < pc_Squad->c_SubNodeIndexes.size();
                    ++u32_SubNodeCounter)
               {
                  const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
                     pc_Squad->c_SubNodeIndexes[u32_SubNodeCounter]);

                  c_SubNodes.push_back(pc_Node);
                  c_ServerIds.emplace_back(C_OscProtocolDriverOsyNode(this->mu8_BusId,
                                                                      orc_Config[u32_SubNodeCounter].c_NodeIds[0]));
               }
               c_SubNodeIndexes = pc_Squad->c_SubNodeIndexes;
               tgl_assert(pc_Squad->c_SubNodeIndexes.size() == c_SubNodes.size());
               q_NodeSquad = true;
            }
         }
         else
         {
            // Only one node. No multi CPU node
            c_SubNodes.push_back(pc_FirstNode);
            c_SubNodeIndexes.push_back(u32_FirstNodeIndex);
            c_ServerIds.push_back(c_FirstServerId);
         }

         if ((pc_FirstNode != NULL) &&
             (c_SubNodes.size() > 0))
         {
            uint32_t u32_IntfCounter;
            uint32_t u32_CommunicationIntf = 0U;
            std::vector<uint32_t> c_CanCounters;
            std::vector<uint32_t> c_EthCounters;

            c_CanCounters.resize(c_SubNodes.size(), 0U);
            c_EthCounters.resize(c_SubNodes.size(), 0U);

            bool q_CummunicationBusFound = false;

            // Get topology node name. In case of a node squad, the base name is necessary
            orc_Text += static_cast<QString>(C_GtGetText::h_GetText("Topology Node: ")) +
                        C_PuiSdUtil::h_GetNodeBaseNameOrName(u32_FirstNodeIndex) + "<br>";

            // Interface
            // Search the for the communication used interface first
            for (u32_IntfCounter = 0U; u32_IntfCounter < pc_FirstNode->c_Properties.c_ComInterfaces.size();
                 ++u32_IntfCounter)
            {
               // Must be the same interface for all sub nodes
               const C_OscNodeComInterfaceSettings & rc_IntfSetting =
                  pc_FirstNode->c_Properties.c_ComInterfaces[u32_IntfCounter];

               if ((rc_IntfSetting.GetBusConnected() == true) &&
                   (static_cast<uint8_t>(rc_IntfSetting.u32_BusIndex) == this->mu32_BusIndex))
               {
                  q_CummunicationBusFound = true;
                  u32_CommunicationIntf = u32_IntfCounter;
                  break;
               }
            }

            tgl_assert(q_CummunicationBusFound == true);
            if (q_CummunicationBusFound == true)
            {
               // Show the for configuration used interface first
               // This interface is on the first position in the configuration vectors
               for (u32_SubNodeCounter = 0U; u32_SubNodeCounter < c_SubNodes.size(); ++u32_SubNodeCounter)
               {
                  const C_OscNode * const pc_SubNode = c_SubNodes[u32_SubNodeCounter];

                  if (pc_SubNode != NULL)
                  {
                     // All sub nodes are connected, so the sub node with index 0 is the first connected sub node
                     // in all cases
                     const bool q_FirstSubNode = (u32_SubNodeCounter == 0U);
                     QString c_SubNodeName = "";
                     // Use the previous found interface
                     const C_OscNodeComInterfaceSettings & rc_IntfSetting =
                        pc_SubNode->c_Properties.c_ComInterfaces[u32_CommunicationIntf];

                     if (q_NodeSquad == true)
                     {
                        // Sub node name is only relevant in case of a node squad
                        c_SubNodeName =
                           C_PuiSdUtil::h_GetSubNodeDeviceName(c_SubNodeIndexes[u32_SubNodeCounter]);
                     }

                     if ((rc_IntfSetting.GetBusConnected() == true) &&
                         (static_cast<uint8_t>(rc_IntfSetting.u32_BusIndex) == this->mu32_BusIndex))
                     {
                        const C_SyvDcDeviceConfiguation & rc_CurConfig =  orc_Config[u32_SubNodeCounter];
                        const C_OscProtocolDriverOsyNode & rc_CurServerId = c_ServerIds[u32_SubNodeCounter];

                        if (rc_IntfSetting.e_InterfaceType == C_OscSystemBus::eCAN)
                        {
                           this->m_ShowConfigInfoOfCanInterface(rc_IntfSetting, rc_CurServerId, rc_CurServerId,
                                                                rc_CurConfig.c_CanBitrates[0], q_FirstSubNode,
                                                                q_NodeSquad, c_SubNodeName, orc_Text);
                           c_CanCounters[u32_SubNodeCounter] = c_CanCounters[u32_SubNodeCounter] + 1U;
                        }
                        else
                        {
                           this->m_ShowConfigInfoOfEthInterface(rc_IntfSetting, rc_CurServerId, rc_CurServerId,
                                                                rc_CurConfig.c_IpAddresses[0], q_FirstSubNode,
                                                                q_NodeSquad, c_SubNodeName, orc_Text);
                           c_EthCounters[u32_SubNodeCounter] = c_EthCounters[u32_SubNodeCounter] + 1U;
                        }
                     }
                  }
               }
            }

            // Show all other interfaces
            for (u32_IntfCounter = 0U; u32_IntfCounter < pc_FirstNode->c_Properties.c_ComInterfaces.size();
                 ++u32_IntfCounter)
            {
               // Communication interface was already handled
               if (u32_CommunicationIntf != u32_IntfCounter)
               {
                  const C_OscNodeComInterfaceSettings & rc_IntfSettingFirstNode =
                     pc_FirstNode->c_Properties.c_ComInterfaces[u32_IntfCounter];

                  bool q_BusConnected = false;

                  // Check if any sub node with this interface is connected
                  for (u32_SubNodeCounter = 0U; u32_SubNodeCounter < c_SubNodes.size(); ++u32_SubNodeCounter)
                  {
                     const C_OscNode * const pc_SubNode = c_SubNodes[u32_SubNodeCounter];

                     if (pc_SubNode != NULL)
                     {
                        //lint -e{514}  Using operator with a bool value was intended and is no accident
                        q_BusConnected |= pc_SubNode->c_Properties.c_ComInterfaces[u32_IntfCounter].GetBusConnected();
                     }
                  }

                  if (q_BusConnected == false)
                  {
                     // No further information for a not used connection
                     this->m_ShowConfigInfoOfInterface(rc_IntfSettingFirstNode, c_FirstServerId, c_FirstServerId,
                                                       true, q_NodeSquad, "",
                                                       orc_Text, false,
                                                       false);
                  }
                  else if ((pc_FirstNode->c_Properties.e_FlashLoader == C_OscNodeProperties::eFL_STW) &&
                           (((static_cast<uint8_t>(rc_IntfSettingFirstNode.u32_BusIndex) == this->mu32_BusIndex) &&
                             (rc_IntfSettingFirstNode.u8_NodeId != c_FirstServerId.u8_NodeIdentifier)) ||
                            (static_cast<uint8_t>(rc_IntfSettingFirstNode.u32_BusIndex) != this->mu32_BusIndex)))
                  {
                     // Any other interface than the communication interface of a STW flashloader device
                     // Can not be configured by the device configuration and can not be part of a node squad
                     this->m_ShowConfigInfoOfInterface(rc_IntfSettingFirstNode, c_FirstServerId, c_FirstServerId,
                                                       true, q_NodeSquad, "",
                                                       orc_Text, true,
                                                       false);
                  }
                  else
                  {
                     bool q_FirstConnectedSubNode = true;
                     for (u32_SubNodeCounter = 0U; u32_SubNodeCounter < c_SubNodes.size(); ++u32_SubNodeCounter)
                     {
                        const C_OscNode * const pc_SubNode = c_SubNodes[u32_SubNodeCounter];

                        if (pc_SubNode != NULL)
                        {
                           const C_OscNodeComInterfaceSettings & rc_IntfSetting =
                              pc_SubNode->c_Properties.c_ComInterfaces[u32_IntfCounter];
                           if (rc_IntfSetting.GetBusConnected() == true)
                           {
                              QString c_SubNodeName = "";
                              if (q_NodeSquad == true)
                              {
                                 // Sub node name is only relevant in case of a node squad
                                 c_SubNodeName = C_PuiSdUtil::h_GetSubNodeDeviceName(
                                    c_SubNodeIndexes[u32_SubNodeCounter]);
                              }

                              const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
                                 rc_IntfSetting.u32_BusIndex);
                              if (pc_Bus != NULL)
                              {
                                 const C_SyvDcDeviceConfiguation & rc_CurConfig =  orc_Config[u32_SubNodeCounter];
                                 const uint32_t u32_CurSubNodeConfigCounter = c_CanCounters[u32_SubNodeCounter] +
                                                                              c_EthCounters[u32_SubNodeCounter];

                                 if ((pc_Bus->u8_BusId == rc_CurConfig.c_BusIds[u32_CurSubNodeConfigCounter]) &&
                                     (rc_IntfSetting.u8_NodeId == rc_CurConfig.c_NodeIds[u32_CurSubNodeConfigCounter]))
                                 {
                                    const C_OscProtocolDriverOsyNode & rc_CurServerId = c_ServerIds[u32_SubNodeCounter];
                                    if (rc_IntfSetting.e_InterfaceType == C_OscSystemBus::eCAN)
                                    {
                                       this->m_ShowConfigInfoOfCanInterface(
                                          rc_IntfSetting,
                                          rc_CurServerId,
                                          C_OscProtocolDriverOsyNode(pc_Bus->u8_BusId, rc_IntfSetting.u8_NodeId),
                                          rc_CurConfig.c_CanBitrates[c_CanCounters[u32_SubNodeCounter]],
                                          q_FirstConnectedSubNode, q_NodeSquad, c_SubNodeName,
                                          orc_Text);

                                       c_CanCounters[u32_SubNodeCounter] = c_CanCounters[u32_SubNodeCounter] + 1U;
                                    }
                                    else
                                    {
                                       this->m_ShowConfigInfoOfEthInterface(
                                          rc_IntfSetting,
                                          rc_CurServerId,
                                          C_OscProtocolDriverOsyNode(pc_Bus->u8_BusId, rc_IntfSetting.u8_NodeId),
                                          rc_CurConfig.c_IpAddresses[c_EthCounters[u32_SubNodeCounter]],
                                          q_FirstConnectedSubNode, q_NodeSquad, c_SubNodeName,
                                          orc_Text);

                                       c_EthCounters[u32_SubNodeCounter] = c_EthCounters[u32_SubNodeCounter] + 1U;
                                    }
                                    // The first connected sub node for this interface was added
                                    q_FirstConnectedSubNode = false;
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates string with the information of a CAN interface

   \param[in]      orc_IntfSetting              Configuration of the current interface
   \param[in]      orc_ServerIdOnUsedBus        Server ID of the node on the for the communication used interface
   \param[in]      orc_ServerIdOnConfiguredBus  Server ID of the node on the configured interface
   \param[in]      ou32_Bitrate                 Set bitrate for the configured interface
   \param[in,out]  orc_Text                     Result text for the interface
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigInfoOfCanInterface(const C_OscNodeComInterfaceSettings & orc_IntfSetting,
                                                   const C_OscProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                                   const C_OscProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus,
                                                   const uint32_t ou32_Bitrate, const bool oq_SingleNodeOrFirstSubNode,
                                                   const bool oq_NodeSquad, const QString & orc_SubNodeName,
                                                   QString & orc_Text)
{
   this->m_ShowConfigInfoOfInterface(orc_IntfSetting, orc_ServerIdOnUsedBus, orc_ServerIdOnConfiguredBus,
                                     oq_SingleNodeOrFirstSubNode, oq_NodeSquad, orc_SubNodeName, orc_Text,
                                     true, true);

   if ((this->m_AreAllInterfacesToConfigure() == true) ||
       (orc_ServerIdOnUsedBus == orc_ServerIdOnConfiguredBus))
   {
      orc_Text += "<tr>";
      orc_Text += "<td width=\"40%\">" +
                  static_cast<QString>(C_GtGetText::h_GetText("CAN-Bitrate: ")) +
                  QString::number(ou32_Bitrate / 1000U) +
                  static_cast<QString>(" kbit/s ") + "</td>";
      orc_Text += "<td width=\"60%\">" +
                  this->m_GetStateStringOfServerStep(C_SyvDcSequences::hu32_SETCANBITRATE, orc_ServerIdOnConfiguredBus,
                                                     orc_IntfSetting.e_InterfaceType,
                                                     orc_IntfSetting.u8_InterfaceNumber) + "</td>";
      orc_Text += "</tr>";
      orc_Text += "</table>";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates string with the information of an Ethernet interface

   \param[in]      orc_IntfSetting              Configuration of the current interface
   \param[in]      orc_ServerIdOnUsedBus        Server ID of the node on the for the communication used interface
   \param[in]      orc_ServerIdOnConfiguredBus  Server ID of the node on the configured interface
   \param[in]      orc_IpAddress                Set IP address for the configured interface
   \param[in,out]  orc_Text                     Result text for the interface
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigInfoOfEthInterface(const C_OscNodeComInterfaceSettings & orc_IntfSetting,
                                                   const C_OscProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                                   const C_OscProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus,
                                                   const C_OscNodeComInterfaceSettings::C_IpAddress & orc_IpAddress,
                                                   const bool oq_SingleNodeOrFirstSubNode, const bool oq_NodeSquad,
                                                   const QString & orc_SubNodeName, QString & orc_Text)
{
   this->m_ShowConfigInfoOfInterface(orc_IntfSetting, orc_ServerIdOnUsedBus, orc_ServerIdOnConfiguredBus,
                                     oq_SingleNodeOrFirstSubNode, oq_NodeSquad, orc_SubNodeName, orc_Text,
                                     true, true);

   if ((this->m_AreAllInterfacesToConfigure() == true) ||
       (orc_ServerIdOnUsedBus == orc_ServerIdOnConfiguredBus))
   {
      QString c_IpAddress;
      QString c_NetMask;
      QString c_DefaultGateway;

      c_IpAddress = C_Uti::h_IpAddressToString(orc_IpAddress.au8_IpAddress);
      c_NetMask = C_Uti::h_IpAddressToString(orc_IpAddress.au8_NetMask);
      c_DefaultGateway = C_Uti::h_IpAddressToString(orc_IpAddress.au8_DefaultGateway);

      orc_Text += "<tr>";
      orc_Text += "<td width=\"40%\">" + static_cast<QString>(C_GtGetText::h_GetText("IP: ")) + c_IpAddress +
                  static_cast<QString>(C_GtGetText::h_GetText(" (Sub-Net: ")) + c_NetMask +
                  static_cast<QString>(C_GtGetText::h_GetText(", default gateway: ")) + c_DefaultGateway + ")</td>";
      orc_Text += "<td width=\"60%\">" + this->m_GetStateStringOfServerStep(C_SyvDcSequences::hu32_SETIPADDRESS,
                                                                            orc_ServerIdOnConfiguredBus,
                                                                            orc_IntfSetting.e_InterfaceType,
                                                                            orc_IntfSetting.u8_InterfaceNumber) +
                  "</td>";
      orc_Text += "</tr>";
      orc_Text += "</table>";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates string with the generic information of an communication interface

   \param[in]      orc_IntfSetting              Configuration of the current interface
   \param[in]      orc_ServerIdOnUsedBus        Server ID of the node on the for the communication used interface
   \param[in]      orc_ServerIdOnConfiguredBus  Server ID of the node on the configured interface
   \param[in,out]  orc_Text                     Result text for the interface
   \param[in]      oq_BusConnected              Flag if the interface is connected to a bus
   \param[in]      oq_Configured                Flag if the interface was configured by the device configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigInfoOfInterface(const C_OscNodeComInterfaceSettings & orc_IntfSetting,
                                                const C_OscProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                                const C_OscProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus,
                                                const bool oq_SingleNodeOrFirstSubNode, const bool oq_NodeSquad,
                                                const QString & orc_SubNodeName, QString & orc_Text,
                                                const bool oq_BusConnected, const bool oq_Configured)
{
   if (oq_SingleNodeOrFirstSubNode == true)
   {
      orc_Text += static_cast<QString>(C_GtGetText::h_GetText("Interface: "));

      //Interface
      orc_Text += C_PuiSdUtil::h_GetInterfaceName(orc_IntfSetting.e_InterfaceType, orc_IntfSetting.u8_InterfaceNumber);
   }

   if (oq_BusConnected == false)
   {
      orc_Text += static_cast<QString>(C_GtGetText::h_GetText(" (skipped, not used)<br>"));
   }
   else if (oq_Configured == false)
   {
      orc_Text += static_cast<QString>(C_GtGetText::h_GetText(" (skipped, not configurable on this bus)<br>"));
   }
   else if ((this->m_AreAllInterfacesToConfigure() == false) &&
            (orc_ServerIdOnUsedBus != orc_ServerIdOnConfiguredBus))
   {
      // The interface shall not be configured, because it is not the used interface by the device configuration
      orc_Text +=
         static_cast<QString>(C_GtGetText::h_GetText(
                                 " (skipped, connected to other bus. Config Mode: Only directly connected interfaces)<br>"));
   }
   else
   {
      if (oq_NodeSquad == true)
      {
         // HTML row in table start (Table row)
         orc_Text += "<table width=\"100%\" style =\" margin-left:5px\">";
         orc_Text += "<tr>";
         orc_Text += "<td width=\"100%\">" + static_cast<QString>(C_GtGetText::h_GetText(" Sub-Node: "));

         // Sub node
         orc_Text += orc_SubNodeName + "</td>";
         orc_Text += "</tr>";
         orc_Text += "</table>";
      }

      // HTML row in table start (Table row)
      orc_Text += "<table width=\"100%\" style =\" margin-left:10px\">";
      orc_Text += "<tr>";
      orc_Text += "<td width=\"40%\">" + static_cast<QString>(C_GtGetText::h_GetText("Node ID: ")) +
                  QString::number(static_cast<uint32_t>(orc_ServerIdOnConfiguredBus.u8_NodeIdentifier)) + "</td>";
      orc_Text += "<td width=\"60%\">" + this->m_GetStateStringOfServerStep(C_SyvDcSequences::hu32_SETNODEID,
                                                                            orc_ServerIdOnConfiguredBus,
                                                                            orc_IntfSetting.e_InterfaceType,
                                                                            orc_IntfSetting.u8_InterfaceNumber) +
                  "</td>";
      orc_Text += "</tr>";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the network dependent of the flashloader types after the finished configuration

   Procedure depends on used protocol(s):

   * openSYDE Ethernet: broadcast NetReset
   * openSYDE CAN: broadcast ECUReset
   * STW Flashloader: netreset
   * openSYDE CAN and STW Flashloader: manual reset or broadcast ECUReset and netreset depends of oq_WithManualReset

   Note on manual reset after changing the bitrate on all CAN nodes:
   Trying to send OSY "EcuReset" broadcast and STW Flashloader "NetReset" at
   the same time could work sometimes depending on node timing. But it could also fail.
   A manual reset should always work.

   \param[in]  oq_SameBitrate    Flag if the bitrate was not changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ResetFlashloaderAfterConfig(const bool oq_SameBitrate)
{
   if (this->mpc_DcSequences != NULL)
   {
      int32_t s32_Return;
      bool q_Manual = false;
      uint32_t u32_NewBitrate;
      std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> c_OsyNodes;
      std::vector<bool> c_OsyNodesSnrExtFormat;
      std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> c_StwNodes;
      const QString c_Details = C_GtGetText::h_GetText("Your system uses at least one node with the STW Flashloader \n"
                                                       "and the bitrate of the CAN bus has changed. \n"
                                                       "In this case the reset cannot be performed automatically.");
      uint32_t u32_WaitTime = 0U;

      if ((this->mc_StwFlashloaderDeviceConfigurations.size() > 0) &&
          (oq_SameBitrate == false))
      {
         // System with STW flashloader devices.
         // A reset of STW flashloader with changed bitrates is not reliable to get them back in flashloader
         q_Manual = true;
      }

      if (q_Manual == false)
      {
         if (this->mc_OpenSydeDeviceConfigurations.size() > 0)
         {
            // In case of openSYDE devices, send the request programming service to get the devices direct in
            // the flashloader again
            bool q_NotAccepted;
            this->mpc_DcSequences->SendOsyBroadcastRequestProgramming(q_NotAccepted);

            if (q_NotAccepted == true)
            {
               osc_write_log_warning("Send openSYDE request programming flag",
                                     "At least one node sent a \"not accepted\" response.");
            }
         }

         // No manual interaction necessary
         this->m_ResetNetwork(true);
      }

      s32_Return = this->m_GetRelevantConfigInfo(c_OsyNodes, c_OsyNodesSnrExtFormat, c_StwNodes, u32_NewBitrate);

      if ((s32_Return == C_NO_ERR) &&
          (oq_SameBitrate == false))
      {
         //for manual resetting ask user to turn devices off before reinitializing and sending on the new bitrate
         // otherwise we'd have inconsistent bitrates on the CAN bus possibly resulting in bus errors
         if (q_Manual == true)
         {
            C_OgeWiCustomMessage c_ConfirmationBox(this, C_OgeWiCustomMessage::E_Type::eWARNING);
            c_ConfirmationBox.SetHeading(C_GtGetText::h_GetText("Devices reset"));
            c_ConfirmationBox.SetDetails(c_Details);
            c_ConfirmationBox.SetOkButtonText(C_GtGetText::h_GetText("Continue"));
            c_ConfirmationBox.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                                     "Power OFF all nodes manually and press \"Continue\".")));
            c_ConfirmationBox.SetCustomMinHeight(180, 270);
            c_ConfirmationBox.Execute();
         }

         //Before reinitializing the bus wait a little to make sure the CAN reset request broadcast(s) had the chance to
         // really be sent out on the bus
         stw::tgl::TglSleep(50U);

         // Set the new bitrate for the new active configuration if the bitrate was changed
         s32_Return = this->mpc_DcSequences->InitCanAndSetCanBitrate(u32_NewBitrate);
      }

      if (s32_Return == C_NO_ERR)
      {
         if (this->me_BusType == C_OscSystemBus::eCAN)
         {
            // Get the minimum wait time
            u32_WaitTime = this->mpc_DcSequences->GetMinimumFlashloaderResetWaitTime(
               C_OscComDriverFlash::eFUNDAMENTAL_COM_CHANGES_CAN);

            // Start sending the message for getting into flashloader for at least the configured minimum time
            // In case of a manual reset, this sequence must be executed till the next dialog will be closed by user
            // and at least the process has last the minimum time.
            // Inform the user how long the devices should get time.
            s32_Return = this->mpc_DcSequences->ScanCanSendFlashloaderRequest(u32_WaitTime, q_Manual);
         }
         else
         {
            const uint32_t u32_StartTime = stw::tgl::TglGetTickCount();
            // Get the minimum wait time
            u32_WaitTime = this->mpc_DcSequences->GetMinimumFlashloaderResetWaitTime(
               C_OscComDriverFlash::eFUNDAMENTAL_COM_CHANGES_ETHERNET);

            do
            {
               //In case it takes longer do process events to handle cursor and proper show of message box
               QApplication::processEvents(QEventLoop::AllEvents, 50);
            }
            while (stw::tgl::TglGetTickCount() < (u32_WaitTime + u32_StartTime));
         }
      }
      else
      {
         osc_write_log_error("Reset to flashloader",
                             "Preparation for reset failed: " + C_SclString::IntToStr(s32_Return));
      }

      if (s32_Return == C_NO_ERR)
      {
         int32_t s32_ThreadResult;

         if (q_Manual == true)
         {
            C_OgeWiCustomMessage c_ConfirmationBox(this, C_OgeWiCustomMessage::E_Type::eWARNING);
            const uint32_t u32_WaitTimeSec = (u32_WaitTime / 1000UL) +
                                             (((u32_WaitTime % 1000UL) ==
                                               0UL) ? static_cast<uint32_t>(0UL) : static_cast<uint32_t>(1UL));
            c_ConfirmationBox.SetHeading(C_GtGetText::h_GetText("Devices reset"));
            c_ConfirmationBox.SetDetails(c_Details);
            c_ConfirmationBox.SetOkButtonText(C_GtGetText::h_GetText("Continue"));
            c_ConfirmationBox.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                                     "Turn ON all nodes manually, wait at least %1 "
                                                                     "second(s) due to Flashloader reset wait time"
                                                                     " and then press \"Continue\".")).
                                             arg(u32_WaitTimeSec));
            c_ConfirmationBox.SetCustomMinHeight(200, 270);
            c_ConfirmationBox.Execute();

            // User has finished the reset, stop the sequence
            this->mpc_DcSequences->StopScanCanSendFlashloaderRequest();
         }

         // Wait till thread is finished
         while (this->mpc_DcSequences->GetResults(s32_ThreadResult) == C_BUSY)
         {
            //In case it takes longer do process events to handle cursor and proper show of message box
            QApplication::processEvents(QEventLoop::AllEvents, 50);
         }

         // Start next sequence
         m_HandleDeviceVerificationStart();

         // Start the read back of the devices
         if (this->me_BusType == C_OscSystemBus::eCAN)
         {
            this->me_Step = eREADBACKCAN;
            s32_Return = this->mpc_DcSequences->ReadBackCan(c_OsyNodes, c_OsyNodesSnrExtFormat, c_StwNodes);
         }
         else
         {
            this->me_Step = eREADBACKETH;
            s32_Return = this->mpc_DcSequences->ReadBackEth(c_OsyNodes, c_OsyNodesSnrExtFormat);
         }

         this->mc_Timer.start();

         if (s32_Return != C_NO_ERR)
         {
            // Error occurred
            this->mc_Timer.stop();
            osc_write_log_error("Reset to flashloader",
                                "Reset to flashloader failed: " + C_SclString::IntToStr(s32_Return));
         }
      }
      else
      {
         osc_write_log_error("Reset to flashloader",
                             "Start of sending flashloader requests failed: " + C_SclString::IntToStr(s32_Return));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the necessary bitrate which was configured in the previous step

   \param[out]  orc_OpenSydeIds           IDs of openSYDE devices for configuration
   \param[out]  orc_OpenSydeSnrExtFormat  Flags for openSYDE devices if standard or extended format is used
   \param[out]  orc_StwIds                IDs of STW flashloader devices for configuration
   \param[out]  oru32_Bitrate             Bitrate for current configuration

   \return
   C_NO_ERR    Bitrate returned
   C_RANGE     Configuration invalid
   C_BUSY      previously started sequence still going on
   C_COM       Error on reading bitrate
   C_CONFIG    No dispatcher installed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDcWidget::m_GetRelevantConfigInfo(std::vector<C_OscProtocolDriverOsyNode> & orc_OpenSydeIds,
                                               std::vector<bool> & orc_OpenSydeSnrExtFormat,
                                               std::vector<C_OscProtocolDriverOsyNode> & orc_StwIds,
                                               uint32_t & oru32_Bitrate)
{
   uint32_t u32_Counter;
   int32_t s32_Return = C_NO_ERR;
   C_OscProtocolDriverOsyNode c_Id;

   oru32_Bitrate = 0U;
   orc_OpenSydeIds.clear();
   orc_OpenSydeSnrExtFormat.clear();
   orc_StwIds.clear();

   // Check all CAN openSYDE devices
   for (u32_Counter = 0U; u32_Counter < this->mc_OpenSydeDeviceConfigurations.size(); ++u32_Counter)
   {
      const C_SyvDcDeviceConfiguation & rc_Config = this->mc_OpenSydeDeviceConfigurations[u32_Counter];

      if ((rc_Config.c_NodeIds.size() > 0) &&
          (rc_Config.c_BusIds.size() > 0))
      {
         // The first entries must be the used communication interface
         c_Id.u8_NodeIdentifier = rc_Config.c_NodeIds[0];
         c_Id.u8_BusIdentifier = rc_Config.c_BusIds[0];
         orc_OpenSydeIds.push_back(c_Id);
         orc_OpenSydeSnrExtFormat.push_back(rc_Config.c_SerialNumber.q_ExtFormatUsed);

         if (this->me_BusType == C_OscSystemBus::eCAN)
         {
            // Not necessary when using Ethernet
            if (rc_Config.c_CanBitrates.size() > 0)
            {
               if (oru32_Bitrate == 0U)
               {
                  oru32_Bitrate = rc_Config.c_CanBitrates[0] / 1000U;
               }
            }
            else
            {
               s32_Return = C_RANGE;
            }
         }
      }
      else
      {
         s32_Return = C_RANGE;
      }

      if (s32_Return != C_NO_ERR)
      {
         break;
      }
   }

   if ((this->mpc_DcSequences != NULL) &&
       (s32_Return == C_NO_ERR))
   {
      // Check all STW flashloader devices
      if (this->me_BusType == C_OscSystemBus::eCAN)
      {
         for (u32_Counter = 0U; u32_Counter < this->mc_StwFlashloaderDeviceConfigurations.size(); ++u32_Counter)
         {
            const C_SyvDcDeviceConfiguation & rc_Config = this->mc_StwFlashloaderDeviceConfigurations[u32_Counter];

            if ((rc_Config.c_NodeIds.size() > 0) &&
                (rc_Config.c_BusIds.size() > 0) &&
                (rc_Config.c_CanBitrates.size() > 0))
            {
               // The first entries must be the used communication interface
               c_Id.u8_NodeIdentifier = rc_Config.c_NodeIds[0];
               c_Id.u8_BusIdentifier = rc_Config.c_BusIds[0];
               orc_StwIds.push_back(c_Id);

               if (oru32_Bitrate == 0U)
               {
                  oru32_Bitrate = rc_Config.c_CanBitrates[0] / 1000U;
               }
            }
            else
            {
               s32_Return = C_RANGE;
               break;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the devices to application

   Resets the network dependent of the flashloader types

   Procedure depends on used protocol(s):

   * openSYDE Ethernet: broadcast NetReset
   * openSYDE CAN: broadcast ECUReset
   * STW Flashloader: netreset

   \param[in]  oq_ToFlashloader  To flashloader
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ResetNetwork(const bool oq_ToFlashloader)
{
   if (this->mpc_DcSequences != NULL)
   {
      if (this->mc_OpenSydeDeviceConfigurations.size() > 0)
      {
         // CAN openSYDE flashloader
         this->mpc_DcSequences->ResetOpenSydeDevices(oq_ToFlashloader);
      }

      if (this->mc_StwFlashloaderDeviceConfigurations.size() > 0)
      {
         // STW flashloader
         this->mpc_DcSequences->ResetCanStwFlashloaderDevices();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Displays the read values

   \param[in]  os32_ActualResult    Detected error state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowReadInfo(const int32_t os32_ActualResult)
{
   if (this->mpc_DcSequences != NULL)
   {
      std::vector<C_OscDcDeviceInformation> c_DeviceInfos;
      const int32_t s32_Return = this->mpc_DcSequences->GetDeviceInfosResult(c_DeviceInfos);
      QString c_Text;

      // Last step. Nothing more to poll
      this->mc_Timer.stop();

      if (s32_Return == C_NO_ERR)
      {
         uint32_t u32_DeviceTotal;
         uint32_t u32_DeviceCounter;
         if (os32_ActualResult == C_NO_ERR)
         {
            u32_DeviceTotal = c_DeviceInfos.size();
         }
         else
         {
            u32_DeviceTotal = static_cast<uint32_t>(this->mc_OpenSydeDeviceConfigurations.size() +
                                                    this->mc_StwFlashloaderDeviceConfigurations.size());
         }

         for (u32_DeviceCounter = 0U; u32_DeviceCounter < c_DeviceInfos.size(); ++u32_DeviceCounter)
         {
            const C_OscDcDeviceInformation & rc_Info = c_DeviceInfos[u32_DeviceCounter];
            const C_OscProtocolDriverOsyNode c_ServerId(this->mu8_BusId, rc_Info.u8_NodeId);
            uint32_t u32_NodeIndex;
            QString c_TopologyNodeName = "NA";
            QString c_TopologyDeviceType = "NA";

            // Counter of nodes
            c_Text += static_cast<QString>("%1 %2 %3 %4<br>").arg(
               static_cast<QString>(C_GtGetText::h_GetText("Node")), QString::number(u32_DeviceCounter + 1U),
               static_cast<QString>(C_GtGetText::h_GetText("of")), QString::number(u32_DeviceTotal));

            // Serial number
            c_Text += "SN.: ";
            if (rc_Info.c_SerialNumber.q_IsValid == true)
            {
               c_Text +=
                  static_cast<QString>(rc_Info.c_SerialNumber.GetSerialNumberAsFormattedString().c_str());
            }
            else
            {
               c_Text += static_cast<QString>(C_GtGetText::h_GetText("Not valid."));
            }
            c_Text += "<br>";

            // Get topology node name and device type
            if (this->mpc_DcSequences->GetNodeIndex(c_ServerId, u32_NodeIndex) == true)
            {
               const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);

               if (pc_Node != NULL)
               {
                  if (pc_Node->pc_DeviceDefinition != NULL)
                  {
                     c_TopologyDeviceType = pc_Node->pc_DeviceDefinition->c_DeviceName.c_str();
                  }
                  c_TopologyNodeName = pc_Node->c_Properties.c_Name.c_str();
               }
            }

            // Topology node name
            c_Text += static_cast<QString>(C_GtGetText::h_GetText("Topology Node: ")) + c_TopologyNodeName;

            // Title of device type
            // HTML row in table start (Table row)
            c_Text += "<table width=\"100%\" style =\" margin-left:10px\">";
            c_Text += "<tr>";
            c_Text += "<td width=\"40%\">" + static_cast<QString>(C_GtGetText::h_GetText("Get type from node")) +
                      "</td>";
            if (rc_Info.q_DeviceNameValid == true)
            {
               c_Text += "<td width=\"60%\">" + static_cast<QString>(C_GtGetText::h_GetText("OK")) + "</td>";
            }
            else
            {
               c_Text += "<td width=\"60%\">" + static_cast<QString>(C_GtGetText::h_GetText("NA")) + "</td>";
            }
            c_Text += "</tr>";
            c_Text += "</table>";

            c_Text += "<table width=\"100%\" style =\" margin-left:20px\">";
            // Read device type
            c_Text += "<tr>";
            c_Text += "<td width=\"40%\">" + static_cast<QString>(C_GtGetText::h_GetText("Read Type: ")) +
                      static_cast<QString>(rc_Info.c_DeviceName.c_str()) +
                      "</td>";
            c_Text += "</tr>";

            // Expected device type
            c_Text += "<tr>";
            c_Text += "<td width=\"40%\">" + static_cast<QString>(C_GtGetText::h_GetText("Expected Type: ")) +
                      c_TopologyDeviceType +
                      "</td>";
            c_Text += "</tr>";
            c_Text += "</table>";

            // Result
            c_Text += "<table width=\"100%\" style =\" margin-left:10px\">";
            c_Text += "<tr>";
            c_Text += "<td width=\"40%\">" + static_cast<QString>(C_GtGetText::h_GetText("Verification")) + "</td>";
            if (static_cast<QString>(rc_Info.c_DeviceName.c_str()) == c_TopologyDeviceType)
            {
               c_Text += "<td width=\"20%\">" + static_cast<QString>(C_GtGetText::h_GetText("OK")) + "</td>";
            }
            else
            {
               c_Text += "<td width=\"20%\"><b>" + static_cast<QString>(C_GtGetText::h_GetText("FAILED")) + "</b></td>";
            }
            c_Text += "</tr>";

            c_Text += "</table>";
            c_Text += "<br><br>";
         }

         if (os32_ActualResult != C_NO_ERR)
         {
            c_Text += "<b>" + static_cast<QString>(C_GtGetText::h_GetText("Missing node(s) or subnode(s):")) +
                      "</b><br/>";
            //Add missing STW devices
            m_HandleMissingDevices(this->mc_StwFlashloaderDeviceConfigurations, c_DeviceInfos, c_Text);
            //Add missing openSYDE devices
            m_HandleMissingDevices(this->mc_OpenSydeDeviceConfigurations, c_DeviceInfos, c_Text);
            c_Text += static_cast<QString>(C_GtGetText::h_GetText("For more details see "));
            //Update log file
            C_OscLoggingHandler::h_Flush();
            c_Text += static_cast<QString>("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>.").
                      arg(C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                      arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                      arg(C_GtGetText::h_GetText("log file"));
            c_Text += "<br/>";
         }

         this->m_UpdateReportText(c_Text);
      }
      else
      {
         // TODO: Error handling
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for reporting the progress of the STW flashloader configuration sequence

   \param[in]  ou32_Progress  Progress of sequence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_UpdateProgressOfStwFlashloaderConfig(const uint32_t ou32_Progress) const
{
   uint32_t u32_ShowProgress;

   if ((this->mpc_DcSequences != NULL) &&
       (this->mpc_DcSequences->IsAtLeastOneOpenSydeNodeActive() == true))
   {
      // Both sequences necessary. It is only half of it.
      u32_ShowProgress = (ou32_Progress / 2U);
   }
   else
   {
      u32_ShowProgress = ou32_Progress;
   }

   this->mpc_Ui->pc_ProgressConfig->SetProgress(u32_ShowProgress, (u32_ShowProgress == 100U));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for reporting the progress of the openSYDE flashloader configuration sequence

   \param[in]  ou32_Progress  Progress of sequence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_UpdateProgressOfOpenSydeConfig(const uint32_t ou32_Progress) const
{
   uint32_t u32_ShowProgress;

   if ((this->mpc_DcSequences != NULL) &&
       (this->mpc_DcSequences->IsAtLeastOneStwFlashloaderNodeActiveOnLocalBus() == true))
   {
      // Both sequences necessary. It is the second half.
      u32_ShowProgress = 50U + (ou32_Progress / 2U);
   }
   else
   {
      u32_ShowProgress = ou32_Progress;
   }

   this->mpc_Ui->pc_ProgressConfig->SetProgress(u32_ShowProgress, (u32_ShowProgress == 100U));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Saves the state of a concrete step of openSYDE configuration sequence

   \param[in]  ou32_Step            Step of node configuration
                                    - hu32_SETNODEID
                                    - hu32_SETCANBITRATE
                                    - hu32_SETIPADDRESS
   \param[in]  os32_Result          Result of service
   \param[in]  ou8_BusIdentifier    Configured bus id of server interface
   \param[in]  ou8_NodeIdentifier   Configured node id of server interface
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_UpdateStateOfStwFlashloaderConfig(const uint32_t ou32_Step, const int32_t os32_Result,
                                                        const uint8_t ou8_BusIdentifier,
                                                        const uint8_t ou8_NodeIdentifier)
{
   // STW flashloader interface will be detected on an other position
   this->m_UpdateStateOfOpenSydeConfig(ou32_Step, os32_Result, ou8_BusIdentifier, ou8_NodeIdentifier, 0U, 0U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Saves the state of a concrete step of openSYDE configuration sequence

   \param[in]  ou32_Step            Step of node configuration
                                    - hu32_SETNODEID
                                    - hu32_SETCANBITRATE
                                    - hu32_SETIPADDRESS
   \param[in]  os32_Result          Result of service
   \param[in]  ou8_BusIdentifier    Configured bus id of server interface
   \param[in]  ou8_NodeIdentifier   Configured node id of server interface
   \param[in]  ou8_InterfaceType    Interface type
                                    - 0 is CAN
                                    - 1 is Ethernet
   \param[in]  ou8_InterfaceNumber  Number of interface
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_UpdateStateOfOpenSydeConfig(const uint32_t ou32_Step, const int32_t os32_Result,
                                                  const uint8_t ou8_BusIdentifier, const uint8_t ou8_NodeIdentifier,
                                                  const uint8_t ou8_InterfaceType, const uint8_t ou8_InterfaceNumber)
{
   const C_OscProtocolDriverOsyNode c_Server(ou8_BusIdentifier, ou8_NodeIdentifier);

   QMap<stw::opensyde_core::C_OscProtocolDriverOsyNode, std::vector<C_ServerConfStepResult> >::iterator c_ItServer;
   const C_ServerConfStepResult c_Result(ou32_Step, ou8_InterfaceType, ou8_InterfaceNumber, os32_Result);

   c_ItServer = this->mc_ServerStates.find(c_Server);

   if (c_ItServer != this->mc_ServerStates.end())
   {
      c_ItServer.value().push_back(c_Result);
   }
   else
   {
      std::vector<C_ServerConfStepResult> c_VecResults;
      c_VecResults.push_back(c_Result);

      this->mc_ServerStates.insert(c_Server, c_VecResults);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the string value as result of the state of a step of a concrete server

   \param[in]  ou32_Step            Step of node configuration
                                    - hu32_SETNODEID
                                    - hu32_SETCANBITRATE
                                    - hu32_SETIPADDRESS
   \param[in]  orc_Server           Configured server on current used bus
   \param[in]  oe_InterfaceType     Interface type
   \param[in]  ou8_InterfaceNumber  Number of interface

   \return
   String for state result
   "TBD"    No state found for server and step
   "OK"     State for server found with no error
   "FAIL"   State for server found with registered state error
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDcWidget::m_GetStateStringOfServerStep(const uint32_t ou32_Step,
                                                    const C_OscProtocolDriverOsyNode & orc_Server,
                                                    const C_OscSystemBus::E_Type oe_InterfaceType,
                                                    const uint8_t ou8_InterfaceNumber)
{
   // Default text if no concrete state is available
   QString c_Text = static_cast<QString>(C_GtGetText::h_GetText("<b>Skipped</b> (error occurred during process)"));

   QMap<stw::opensyde_core::C_OscProtocolDriverOsyNode,
        std::vector<C_ServerConfStepResult> >::const_iterator c_ItServer;

   c_ItServer = this->mc_ServerStates.find(orc_Server);

   if (c_ItServer != this->mc_ServerStates.end())
   {
      uint32_t u32_StateCounter;
      const std::vector<C_ServerConfStepResult> & rc_States = c_ItServer.value();

      for (u32_StateCounter = 0U; u32_StateCounter < rc_States.size(); ++u32_StateCounter)
      {
         if ((rc_States[u32_StateCounter].u32_ConfStep == ou32_Step) &&
             (rc_States[u32_StateCounter].u8_InterfaceNumber == ou8_InterfaceNumber) &&
             (rc_States[u32_StateCounter].u8_InterfaceType == static_cast<uint8_t>(oe_InterfaceType)))
         {
            // State found
            if (rc_States[u32_StateCounter].s32_Result == C_NO_ERR)
            {
               c_Text = static_cast<QString>(C_GtGetText::h_GetText("OK"));
            }
            else
            {
               c_Text = "<b>" + static_cast<QString>(C_GtGetText::h_GetText("FAILED")) + "</b>";
            }
            break;
         }
      }
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_OkClicked(void)
{
   // Close only when finished
   if (m_CheckQuitPossible() == true)
   {
      m_DoCompleteDisconnect();
      this->mpc_ParentDialog->accept();
   }
   else
   {
      m_InformUserAboutAbortedClose();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_CancelClicked(void)
{
   // Close only when finished
   if (m_CheckQuitPossible() == true)
   {
      m_DoCompleteDisconnect();
      this->mpc_ParentDialog->reject();
   }
   else
   {
      m_InformUserAboutAbortedClose();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize scan screen
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_InitScanScreen(void)
{
   QString c_Heading;
   int32_t s32_Return;

   this->mpc_Ui->pc_PushButtonScan->setFocus();

   m_InitBitRateComboBox();
   m_InitModeComboBox();
   s32_Return = this->mpc_Ui->pc_ListWidgetExistingNodes->SetView(this->mu32_ViewIndex, false);

   if (this->mpc_Ui->pc_ListWidgetExistingNodes->count() == 0)
   {
      this->mpc_Ui->pc_GroupBoxNoExistingNodes->setVisible(true);
      this->mpc_Ui->pc_ListWidgetExistingNodes->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxNoExistingNodes->setVisible(false);
      this->mpc_Ui->pc_ListWidgetExistingNodes->setVisible(true);
   }
   c_Heading = static_cast<QString>(C_GtGetText::h_GetText("Topology Nodes (%1)")).arg(
      this->mpc_Ui->pc_ListWidgetExistingNodes->count());
   this->mpc_Ui->pc_LabelHeadingTopologyNodes->setText(c_Heading);
   this->mpc_Ui->pc_LabelHeadingFoundDevices->setText(static_cast<QString>(C_GtGetText::h_GetText("Connected Devices")));
   //Progress
   this->mpc_Ui->pc_BopperleScan->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
   //Reset
   this->mpc_Ui->pc_BopperleAssignment->SetMainBopperleColor(Qt::transparent, Qt::transparent);
   this->mpc_Ui->pc_BopperleConfig->SetMainBopperleColor(Qt::transparent, Qt::transparent);
   this->mpc_Ui->pc_BopperleFinished->SetMainBopperleColor(Qt::transparent, Qt::transparent);
   this->mpc_Ui->pc_ProgressScan->SetProgress(0, false);
   this->mpc_Ui->pc_ProgressAssignment->SetProgress(0, false);
   this->mpc_Ui->pc_ProgressConfig->SetProgress(0, false);

   if (s32_Return != C_NO_ERR)
   {
      m_InformUserAfterScan(C_GtGetText::h_GetText(
                               "At least one node is connected to the bus with more than one interface."
                               " Check interface configuration of the devices and retry."));
      this->mpc_Ui->pc_PushButtonScan->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize assignment screen
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_InitAssignmentScreen(void) const
{
   QString c_Heading;

   this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->SetView(this->mu32_ViewIndex, true);
   c_Heading = static_cast<QString>(C_GtGetText::h_GetText("Topology Nodes (%1)")).arg(
      this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->count());
   this->mpc_Ui->pc_LabelHeadingTopologyNodesAssignment->setText(c_Heading);

   this->mpc_Ui->pc_ListWidgetConnectedNodesAssignment->SetData(this->mc_FoundDevices);
   c_Heading = static_cast<QString>(C_GtGetText::h_GetText("Connected Devices (%1)")).
               arg(this->mpc_Ui->pc_ListWidgetConnectedNodesAssignment->count());
   this->mpc_Ui->pc_LabelHeadingFoundDevicesAssignment->setText(c_Heading);
   m_AssignmentUpdateProgress();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize bitrate combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_InitBitRateComboBox(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   //Disconnect
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxBitRate, static_cast<void (QComboBox::*)(
                                                               int32_t)>(&QComboBox::currentIndexChanged), this,
              &C_SyvDcWidget::m_OnBitRateChanged);

   // Bitrate
   this->mpc_Ui->pc_ComboBoxBitRate->clear();

   if (pc_View != NULL)
   {
      std::vector<uint32_t> c_ConnectedNodes;
      std::vector<uint32_t> c_ConnectedInterfaces;
      std::vector<uint32_t> c_SupportedBitrates;
      uint32_t u32_BitrateCounter;
      const uint32_t u32_CurrentSetBitrate = pc_View->GetDeviceConfigSelectedBitRate();
      const QString c_CurrentSetBitrate = this->m_GetComboBoxString(u32_CurrentSetBitrate);
      bool q_CurrentSetBitrateFound = false;
      std::vector<uint8_t> c_ActiveNodes = pc_View->GetNodeActiveFlags();
      uint32_t u32_ActiveNodeIndex;

      C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_BusIndex,
                                                                                         c_ConnectedNodes,
                                                                                         c_ConnectedInterfaces);

      // Remove the nodes, which are not active in this view
      for (u32_ActiveNodeIndex = 0U; u32_ActiveNodeIndex < c_ActiveNodes.size(); ++u32_ActiveNodeIndex)
      {
         if (c_ActiveNodes[u32_ActiveNodeIndex] == 0U)
         {
            std::vector<uint32_t>::iterator c_ItNode;
            for (c_ItNode = c_ConnectedNodes.begin(); c_ItNode != c_ConnectedNodes.end(); ++c_ItNode)
            {
               if (u32_ActiveNodeIndex == (*c_ItNode))
               {
                  c_ConnectedNodes.erase(c_ItNode);
                  break;
               }
            }
         }
      }

      C_PuiSdHandler::h_GetInstance()->GetSupportedCanBitrates(c_ConnectedNodes, c_SupportedBitrates);

      //Fill combo box
      for (u32_BitrateCounter = 0U; u32_BitrateCounter < c_SupportedBitrates.size(); ++u32_BitrateCounter)
      {
         this->mpc_Ui->pc_ComboBoxBitRate->addItem(this->m_GetComboBoxString(c_SupportedBitrates[u32_BitrateCounter]));

         if (c_SupportedBitrates[u32_BitrateCounter] == u32_CurrentSetBitrate)
         {
            q_CurrentSetBitrateFound = true;
         }
      }

      if (q_CurrentSetBitrateFound == true)
      {
         this->mpc_Ui->pc_ComboBoxBitRate->setCurrentText(c_CurrentSetBitrate);
      }
      else
      {
         this->mpc_Ui->pc_ComboBoxBitRate->setCurrentIndex(0);
      }
   }

   //Reconnect
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxBitRate, static_cast<void (QComboBox::*)(
                                                            int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDcWidget::m_OnBitRateChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize current configuration mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_InitModeComboBox(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   //Disconnect
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxConfigurationMode, static_cast<void (QComboBox::*)(
                                                                         int32_t)>(&QComboBox::currentIndexChanged),
              this,
              &C_SyvDcWidget::m_OnDeviceConfigModeChanged);

   //Apply value
   if (pc_View != NULL)
   {
      switch (pc_View->GetDeviceConfigMode())
      {
      case C_PuiSvData::eDCM_ALL_CONNECTED_INTERFACES:
         this->mpc_Ui->pc_ComboBoxConfigurationMode->setCurrentIndex(
            C_SyvDcWidget::mhs32_INDEX_CONFIGURATION_ALL_CONNECTED_INTERFACES);
         break;
      case C_PuiSvData::eDCM_ONLY_USED_INTERFACES:
         this->mpc_Ui->pc_ComboBoxConfigurationMode->setCurrentIndex(
            C_SyvDcWidget::mhs32_INDEX_CONFIGURATION_ONLY_USED_INTERFACES);
         break;
      default:
         break;
      }
   }

   //Reconnect
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxConfigurationMode, static_cast<void (QComboBox::*)(
                                                                      int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDcWidget::m_OnDeviceConfigModeChanged);
}

//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDcWidget::m_GetComboBoxString(const uint32_t ou32_Bitrate) const
{
   const QString c_Text = QString::number(ou32_Bitrate) + static_cast<QString>(" kbit/s");

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvDcWidget::m_GetBitrateFromComboBoxString(const QString & orc_Entry) const
{
   QString c_Bitrate = orc_Entry;
   // In case of an error it returns 0
   const uint32_t u32_Bitrate = c_Bitrate.remove(" kbit/s").toInt();

   return u32_Bitrate;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle bitrate change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_OnBitRateChanged(void) const
{
   uint32_t u32_BitRate;

   if (this->m_GetBitRateValue(u32_BitRate) == C_NO_ERR)
   {
      C_PuiSvHandler::h_GetInstance()->SetViewDeviceConfigSelectedBitRate(this->mu32_ViewIndex, u32_BitRate);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle device config mode change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_OnDeviceConfigModeChanged(void) const
{
   C_PuiSvData::E_DeviceConfigurationMode e_Mode;
   if (this->mpc_Ui->pc_ComboBoxConfigurationMode->currentIndex() ==
       C_SyvDcWidget::mhs32_INDEX_CONFIGURATION_ONLY_USED_INTERFACES)
   {
      e_Mode = C_PuiSvData::eDCM_ONLY_USED_INTERFACES;
   }
   else
   {
      e_Mode = C_PuiSvData::eDCM_ALL_CONNECTED_INTERFACES;
   }
   C_PuiSvHandler::h_GetInstance()->SetViewDeviceConfigMode(this->mu32_ViewIndex, e_Mode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected bit rate

   \param[out]  oru32_Value   Bit rate

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDcWidget::m_GetBitRateValue(uint32_t & oru32_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   oru32_Value = this->m_GetBitrateFromComboBoxString(this->mpc_Ui->pc_ComboBoxBitRate->currentText());

   if (oru32_Value == 0U)
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do assignment for specified node and serial number

   \param[in] ou32_NodeIndex         Node index
   \param[in] orc_SerialNumber       Serial number
   \param[in] orc_SubNodeIdsToOldNodeIds  Detected sub node ids and the associated used node ids
                                          with same serial number
                                          - In case of a normal node, exact one sub node id which should be 0
                                          - In case of a multiple CPU, at least two sub node ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_AssignmentConnect(const uint32_t ou32_NodeIndex,
                                        const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber,
                                        const std::map<uint8_t,
                                                       C_OscDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
const
{
   this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->ConnectSerialNumber(ou32_NodeIndex, orc_SerialNumber,
                                                                           orc_SubNodeIdsToOldNodeIds);
   this->mpc_Ui->pc_ListWidgetConnectedNodesAssignment->DisableSerialNumber(orc_SerialNumber);
   m_AssignmentUpdateProgress();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect assignment for specified node and serial number

   \param[in] ou32_NodeIndex         Node index
   \param[in] orc_SerialNumber       Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_AssignmentDisconnect(const uint32_t ou32_NodeIndex,
                                           const C_OscProtocolSerialNumber & orc_SerialNumber) const
{
   this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->DisconnectSerialNumber(ou32_NodeIndex, orc_SerialNumber);
   this->mpc_Ui->pc_ListWidgetConnectedNodesAssignment->EnableSerialNumber(orc_SerialNumber);
   m_AssignmentUpdateProgress();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update assignment progress
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_AssignmentUpdateProgress(void) const
{
   const uint32_t u32_AssignedItemCount = this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->GetAssignmentCount();
   const uint32_t u32_OverallItemCount =
      static_cast<uint32_t>(this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->count());
   QString c_Text;

   c_Text = static_cast<QString>(C_GtGetText::h_GetText("Assigned: %1 of %2")).arg(u32_AssignedItemCount).arg(
      u32_OverallItemCount);

   if (u32_AssignedItemCount == u32_OverallItemCount)
   {
      this->mpc_Ui->pc_PushButtonConfigure->setEnabled(true);
      this->mpc_Ui->pc_PushButtonConfigure->setFocus();
   }
   else
   {
      this->mpc_Ui->pc_PushButtonConfigure->setEnabled(false);
   }

   this->mpc_Ui->pc_LabelAssignmentProgress->setText(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_Timer(void)
{
   if (this->mpc_DcSequences != NULL)
   {
      const QString c_Log = C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str();
      int32_t s32_Result;
      int32_t s32_SequenceResult;
      bool q_ShowFinalErrorMessage = false;
      QString c_ErrorDescription = "";

      if ((this->mq_InitializationFinished == false) &&
          (this->me_Step == eSCANCANENTERFLASHLOADER))
      {
         // Special case: The first step with CAN communication has in the sequence the initialization
         this->mq_InitializationFinished = this->mpc_DcSequences->GetCanInitializationResult();
         if (this->mq_InitializationFinished == true)
         {
            // The thread is running, CAN is now initialized, therefore the communication is running...
            this->mpc_Ui->pc_LabelStartScan->setText(C_GtGetText::h_GetText("Scanning for devices..."));
         }
      }

      s32_Result = this->mpc_DcSequences->GetResults(s32_SequenceResult);

      if (s32_Result == C_NO_ERR)
      {
         QString c_Text = "";

         // Thread finished
         this->mc_Timer.stop();

         switch (this->me_Step)
         {
         case eSCANCANENTERFLASHLOADER:
            if (s32_SequenceResult == C_NO_ERR)
            {
               if (this->mpc_DcSequences->IsAtLeastOneStwFlashloaderNodeActiveOnLocalBus() == true)
               {
                  this->me_Step = eSCANCANGETINFOFROMSTWFLASHLOADERDEVICES;
                  s32_Result = this->mpc_DcSequences->ScanCanGetInfoFromStwFlashloaderDevices();

                  if (s32_Result != C_NO_ERR)
                  {
                     C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                     c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
                     c_Message.SetDescription(C_GtGetText::h_GetText("Could not continue with step: Reading information"
                                                                     " from STW Flashloader devices"));
                     m_CleanUpScan();
                     c_Message.SetCustomMinHeight(180, 180);
                     c_Message.Execute();
                  }
               }
               else
               {
                  // No STW flashloader
                  this->me_Step = eSCANCANGETINFOFROMOPENSYDEDEVICES;
                  s32_Result = this->mpc_DcSequences->ScanCanGetInfoFromOpenSydeDevices();

                  if (s32_Result != C_NO_ERR)
                  {
                     C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                     c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
                     c_Message.SetDescription(C_GtGetText::h_GetText("Could not continue with step: Reading information"
                                                                     " from openSYDE devices."));
                     m_CleanUpScan();
                     c_Message.SetCustomMinHeight(180, 180);
                     c_Message.Execute();
                  }
               }

               //Continue with next step?
               if (s32_Result == C_NO_ERR)
               {
                  this->mpc_Ui->pc_ProgressScan->SetProgress(50, false);
                  this->mc_Timer.start();
               }
            }
            else
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
               c_Message.SetDescription(C_GtGetText::h_GetText("Could not transfer all devices into Flashloader mode."));
               m_CleanUpScan();
               c_Message.SetCustomMinHeight(180, 180);
               c_Message.Execute();
            }
            break;
         case eSCANCANGETINFOFROMSTWFLASHLOADERDEVICES:
            if (s32_SequenceResult == C_NO_ERR)
            {
               std::vector<C_OscDcDeviceInformation> c_DeviceInfo;
               this->mpc_DcSequences->GetDeviceInfosResult(c_DeviceInfo);

               this->mc_FoundDevices.reserve(this->mc_FoundDevices.size() + c_DeviceInfo.size());

               for (uint32_t u32_SnCounter = 0U; u32_SnCounter < c_DeviceInfo.size(); ++u32_SnCounter)
               {
                  this->mc_FoundDevices.push_back(c_DeviceInfo[u32_SnCounter]);
               }

               if (this->mpc_DcSequences->IsAtLeastOneOpenSydeNodeActive() == true)
               {
                  // At least one openSYDE device is active
                  this->me_Step = eSCANCANGETINFOFROMOPENSYDEDEVICES;
                  s32_Result = this->mpc_DcSequences->ScanCanGetInfoFromOpenSydeDevices();

                  //Continue with next step?
                  if (s32_Result != C_NO_ERR)
                  {
                     C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                     c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
                     c_Message.SetDescription(C_GtGetText::h_GetText("Could not continue with step: Reading information"
                                                                     " from openSYDE Devices"));
                     m_CleanUpScan();
                     c_Message.SetCustomMinHeight(180, 180);
                     c_Message.Execute();
                  }
                  else
                  {
                     this->mpc_Ui->pc_ProgressScan->SetProgress(75, false);
                     this->mc_Timer.start();
                  }
               }
               else
               {
                  // No openSYDE device active
                  this->m_ScanFinished();
               }
            }
            else
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
               c_Message.SetDescription(C_GtGetText::h_GetText("Failed while getting all information"
                                                               " from STW Flashloader Devices."));
               m_CleanUpScan();
               c_Message.SetCustomMinHeight(180, 180);
               c_Message.Execute();
            }
            break;
         case eSCANCANGETINFOFROMOPENSYDEDEVICES: // Same data handling
         case eSCANETHGETINFOFROMOPENSYDEDEVICES: // Same data handling
            if (s32_SequenceResult == C_NO_ERR)
            {
               std::vector<C_OscDcDeviceInformation> c_DeviceInfo;
               this->mpc_DcSequences->GetDeviceInfosResult(c_DeviceInfo);
               this->mpc_DcSequences->GetSecurityFeatureUsageResult(this->mq_SecurityFeatureUsed);

               this->mc_FoundDevices.reserve(this->mc_FoundDevices.size() + c_DeviceInfo.size());

               for (uint32_t u32_SnCounter = 0U; u32_SnCounter < c_DeviceInfo.size(); ++u32_SnCounter)
               {
                  this->mc_FoundDevices.push_back(c_DeviceInfo[u32_SnCounter]);
               }

               this->m_ScanFinished();
            }
            else
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
               if (s32_SequenceResult == C_CHECKSUM)
               {
                  c_Message.SetCustomMinHeight(200, 230);
                  c_Message.SetDescription(
                     C_GtGetText::h_GetText("At least one node has security activated "
                                            "and at least one node ID is not unique."));
                  c_Message.SetDetails(C_GtGetText::h_GetText(
                                          "This combination is not supported by the device configuration."));
               }
               else
               {
                  c_Message.SetCustomMinHeight(180, 180);
                  c_Message.SetDescription(C_GtGetText::h_GetText("Failed while getting all information"
                                                                  " from openSYDE Devices."));
               }
               m_CleanUpScan();
               c_Message.Execute();
            }
            break;
         case eCONFCANSTWFLASHLOADERDEVICES:
            if ((s32_SequenceResult == C_NO_ERR) ||
                (s32_SequenceResult == C_NOACT)) // No STW flashloader devices to configure
            {
               if (this->mpc_DcSequences->IsAtLeastOneOpenSydeNodeActive() == true)
               {
                  this->me_Step = eCONFCANOPENSYDEDEVICES;
                  s32_Result = this->mpc_DcSequences->ConfCanOpenSydeDevices(this->mc_OpenSydeDeviceConfigurations,
                                                                             this->m_AreAllInterfacesToConfigure(),
                                                                             this->mq_SecurityFeatureUsed);

                  if (s32_Result == C_NO_ERR)
                  {
                     this->mc_Timer.start();
                  }
                  else
                  {
                     C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                     c_Message.SetDescription(C_GtGetText::h_GetText(
                                                 "Error occurred during openSYDE device configuration."));
                     c_Message.SetCustomMinHeight(180, 180);
                     c_Message.Execute();
                  }
               }
               else
               {
                  // No openSYDE devices. Finished
                  this->m_ShowConfigResult();
                  // Finish the process. No waiting for reset necessary.
                  this->m_ResetFlashloaderAfterConfig(this->mq_SameBitrates);
               }
            }
            else
            {
               this->m_ShowConfigResult();

               q_ShowFinalErrorMessage = true;
               c_ErrorDescription =
                  C_GtGetText::h_GetText("Error occurred during STW Flashloader device configuration.");
            }
            break;
         case eCONFCANOPENSYDEDEVICES: // Same result
         case eCONFETHOPENSYDEDEVICES: // Same result
            this->m_ShowConfigResult();

            if ((s32_SequenceResult == C_NO_ERR) ||
                (s32_SequenceResult == C_NOACT)) // No openSYDE devices to configure
            {
               // Finish the process. No waiting for reset necessary.
               this->m_ResetFlashloaderAfterConfig(this->mq_SameBitrates);
            }
            else
            {
               q_ShowFinalErrorMessage = true;
               c_ErrorDescription = C_GtGetText::h_GetText("Error occurred during openSYDE device configuration.");

               c_Text = static_cast<QString>(C_GtGetText::h_GetText("For more details see "));
               //Update log file
               C_OscLoggingHandler::h_Flush();
               c_Text += static_cast<QString>("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>.").
                         arg(C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                         arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                         arg(C_GtGetText::h_GetText("log file"));
               c_Text += "<br/>";
               c_Text += "<br/><br/><br/>" + mc_REPORT_HIGHLIGHT_TAG_START;
               c_Text += static_cast<QString>(C_GtGetText::h_GetText(
                                                 "Errors occurred during device configuration. Check report for details."));

               c_Text += mc_REPORT_HIGHLIGHT_TAG_END + "<br/>";

               this->m_UpdateReportText(c_Text);

               if (s32_SequenceResult == C_CHECKSUM)
               {
                  C_OgeWiCustomMessage c_MessageAuth(this, C_OgeWiCustomMessage::E_Type::eERROR);

                  q_ShowFinalErrorMessage = false;
                  C_OscLoggingHandler::h_Flush();
                  c_MessageAuth.SetHeading(C_GtGetText::h_GetText("Device Configuration"));
                  c_MessageAuth.SetDescription(C_GtGetText::h_GetText(
                                                  "Authentication between openSYDE Tool and device(s) has failed. Access denied."));
                  c_MessageAuth.SetDetails(C_GtGetText::h_GetText("Possible reasons:<br/>"
                                                                  "- Associated private key (*.pem) not found in /certificates folder (most common)<br/>"
                                                                  "- Failure during authenfication process<br/>"
                                                                  "For more information see ") +
                                           C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"),
                                                            mc_STYLESHEET_GUIDE_COLOR_LINK,
                                                            c_Log) + C_GtGetText::h_GetText("."));
                  c_MessageAuth.SetCustomMinHeight(200, 300);
                  c_MessageAuth.Execute();
               }
            }
            break;
         case eREADBACKCAN: // Same implementation for showing the read info
         case eREADBACKETH: // Same implementation for showing the read info
            this->mpc_Ui->pc_PbBackToScan->setEnabled(true);
            this->mpc_Ui->pc_PbBackToScan->setVisible(true);
            this->mpc_Ui->pc_BushButtonOk->setFocus();
            this->m_ShowReadInfo(s32_SequenceResult);
            this->mpc_Ui->pc_BushButtonOk->setVisible(true);
            this->mpc_Ui->pc_BushButtonCancel->setVisible(false);
            this->mpc_Ui->pc_BopperleFinished->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);

            this->m_UpdateReportText(c_Text);

            //Discard disconnect necessary on close
            this->mq_DisconnectNecessary = false;

            // Start the applications
            this->m_ResetNetwork(false);

            //Notify user last?
            c_Text = "<br/><br/><br/>" + mc_REPORT_HIGHLIGHT_TAG_START;

            if ((s32_SequenceResult == C_NO_ERR) ||
                (s32_SequenceResult == C_NOACT)) // No openSYDE devices to configure
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eINFORMATION);

               c_Text +=  static_cast<QString>(C_GtGetText::h_GetText("Device Configuration successfully finished!"));
               c_Text += mc_REPORT_HIGHLIGHT_TAG_END + "<br/><br/>";
               this->m_UpdateReportText(c_Text);

               c_Message.SetHeading("Device Configuration");
               c_Message.SetDescription(C_GtGetText::h_GetText("Device Configuration successfully finished."));
               c_Message.SetCustomMinHeight(180, 180);
               c_Message.Execute();
            }
            else
            {
               q_ShowFinalErrorMessage = true;
               c_ErrorDescription = C_GtGetText::h_GetText("Error occurred during device configuration.");

               c_Text +=
                  static_cast<QString>(C_GtGetText::h_GetText(
                                          "Errors occurred during device configuration. Check report for details."));
               c_Text += mc_REPORT_HIGHLIGHT_TAG_END + "<br/><br/>";
               this->m_UpdateReportText(c_Text);

               if (s32_SequenceResult == C_CHECKSUM)
               {
                  C_OgeWiCustomMessage c_MessageAuth(this, C_OgeWiCustomMessage::E_Type::eERROR);

                  q_ShowFinalErrorMessage = false;
                  C_OscLoggingHandler::h_Flush();
                  c_MessageAuth.SetHeading(C_GtGetText::h_GetText("Device Configuration"));
                  c_MessageAuth.SetDescription(C_GtGetText::h_GetText(
                                                  "Authentication between openSYDE Tool and device(s) has failed. Access denied."));
                  c_MessageAuth.SetDetails(C_GtGetText::h_GetText("Possible reasons:<br/>"
                                                                  "- Associated private key (*.pem) not found in /certificates folder (most common)<br/>"
                                                                  "- Failure during authenfication process<br/>"
                                                                  "For more information see ") +
                                           C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"),
                                                            mc_STYLESHEET_GUIDE_COLOR_LINK,
                                                            c_Log) + C_GtGetText::h_GetText("."));
                  c_MessageAuth.SetCustomMinHeight(200, 300);
                  c_MessageAuth.Execute();
               }
            }

            if (this->me_Step == eREADBACKCAN)
            {
               //Before closing the bus wait a little to make sure the CAN reset request broadcast(s) had the chance to
               // really be sent out on the bus
               stw::tgl::TglSleep(50U);
            }

            break;
         default:
            break;
         }
      }

      if (q_ShowFinalErrorMessage == true)
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetHeading("Device Configuration");
         c_Message.SetDescription(c_ErrorDescription);
         c_Message.SetDetails(C_GtGetText::h_GetText(
                                 "Tips:\n"
                                 "- Check if your PC is really connected to the device interface, which is defined in the setup view.\n"
                                 "- Check whether the devices have performed the reset correctly.\n"
                                 "- Check if the devices are still available.\n"
                                 "- Check if in case of Ethernet the set IP address is in the same subnet like the PC Ethernet adapter?\n"
                                 "\n"
                                 "Check device configuration report for more details about warnings, errors or success."));
         c_Message.SetCustomMinHeight(180, 350);
         c_Message.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_ClearReportText(void)
{
   this->mc_ReportText = "";
   this->mpc_Ui->pc_TextBrowserReport->setText("");
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_UpdateReportText(const QString & orc_NewTextPart, const QString & orc_TemporaryText)
{
   QTextCursor c_TextCursor;
   QString c_HtmlText = "";

   // Save the text without the header for the next call
   this->mc_ReportText += orc_NewTextPart;

   // Add the entire text
   c_HtmlText += this->mc_ReportText + orc_TemporaryText;

   this->mpc_Ui->pc_TextBrowserReport->setHtml(c_HtmlText);

   // Scroll to the end
   c_TextCursor = this->mpc_Ui->pc_TextBrowserReport->textCursor();
   c_TextCursor.movePosition(QTextCursor::End);
   this->mpc_Ui->pc_TextBrowserReport->setTextCursor(c_TextCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle configuration step start
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_HandleConfigurationStarted(void)
{
   QString c_Text;
   QString c_Temporary;

   c_Text = mc_REPORT_HEADLINE_HTML_TAG_START;
   c_Text += static_cast<QString>(C_GtGetText::h_GetText("Device Configuration"));
   c_Text += mc_REPORT_HEADLINE_HTML_TAG_END;
   c_Temporary = "<p>" + static_cast<QString>(C_GtGetText::h_GetText("Waiting for results ...")) + "</p>";

   this->m_UpdateReportText(c_Text, c_Temporary);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   handle device verification step start
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_HandleDeviceVerificationStart(void)
{
   QString c_Text;
   QString c_Temporary;

   c_Text = mc_REPORT_HEADLINE_HTML_TAG_START;
   c_Text += static_cast<QString>(C_GtGetText::h_GetText("Device Type Verification"));
   c_Text += mc_REPORT_HEADLINE_HTML_TAG_END;
   c_Temporary = "<p>" + static_cast<QString>(C_GtGetText::h_GetText("Waiting for results ...")) + "</p>";

   this->m_UpdateReportText(c_Text, c_Temporary);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle potentially missing device infos

   \param[in]      orc_AllDeviceInfos     All expected device infos
   \param[in]      orc_FoundDeviceInfos   All received device infos
   \param[in,out]  orc_ReportText         Report text to append to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_HandleMissingDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_AllDeviceInfos,
                                           const std::vector<C_OscDcDeviceInformation> & orc_FoundDeviceInfos,
                                           QString & orc_ReportText) const
{
   for (uint32_t u32_ItExpected = 0; u32_ItExpected < orc_AllDeviceInfos.size(); ++u32_ItExpected)
   {
      bool q_Found = false;
      const C_SyvDcDeviceConfiguation & rc_ExpectedDevice = orc_AllDeviceInfos[u32_ItExpected];
      for (uint32_t u32_ItFound = 0; u32_ItFound < orc_FoundDeviceInfos.size(); ++u32_ItFound)
      {
         const C_OscDcDeviceInformation & rc_FoundDevice = orc_FoundDeviceInfos[u32_ItFound];
         if (rc_FoundDevice.c_SerialNumber == rc_ExpectedDevice.c_SerialNumber)
         {
            q_Found = true;
            break;
         }
      }
      if (q_Found == false)
      {
         orc_ReportText += "<b>" + static_cast<QString>(C_GtGetText::h_GetText("SN.: %1")).arg(
            static_cast<QString>(rc_ExpectedDevice.c_SerialNumber.GetSerialNumberAsFormattedString().c_str())) +
                           "</b><br/>";
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcWidget::m_AreAllInterfacesToConfigure(void) const
{
   bool q_SetAllInterfaces = false;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // Get configuration from view
   tgl_assert(pc_View != NULL);
   if (pc_View != NULL)
   {
      switch (pc_View->GetDeviceConfigMode())
      {
      case C_PuiSvData::eDCM_ALL_CONNECTED_INTERFACES:
         q_SetAllInterfaces = true;
         break;
      case C_PuiSvData::eDCM_ONLY_USED_INTERFACES:
         q_SetAllInterfaces = false;
         break;
      default:
         break;
      }
   }

   return q_SetAllInterfaces;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect from network if necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_DoCompleteDisconnect(void)
{
   if (mq_DisconnectNecessary == true)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      //Stop timer (don't continue process)
      this->mc_Timer.stop();

      if ((pc_View != NULL) && (pc_View->GetOscPcData().GetConnected() == true))
      {
         const C_OscSystemBus * const pc_Bus =
            C_PuiSdHandler::h_GetInstance()->GetOscBus(pc_View->GetOscPcData().GetBusIndex());
         if ((this->mpc_DcSequences != NULL) &&
             (pc_Bus != NULL))
         {
            bool q_Osy = false;
            bool q_Stw = false;
            bool q_StartedAnything = false;
            std::vector<uint32_t> c_NodeIndexes;
            std::vector<uint32_t> c_InterfaceIndexes;
            int32_t s32_Result;
            C_OgeWiCustomMessage c_Message(this);
            QApplication::setOverrideCursor(Qt::WaitCursor);

            //inform user whats happening
            c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
            c_Message.SetDescription(C_GtGetText::h_GetText("Exiting Update Mode..."));
            c_Message.SetCustomMinHeight(180, 180);
            c_Message.show();

            //Wait for the current thread to finish (if any)
            while (this->mpc_DcSequences->GetResults(s32_Result) == C_BUSY)
            {
               //In case it takes longer do process events to handle cursor and proper show of message box
               QApplication::processEvents(QEventLoop::AllEvents, 50);
            }

            //Check which nodes
            C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(
               pc_View->GetOscPcData().GetBusIndex(), c_NodeIndexes, c_InterfaceIndexes);
            for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
            {
               //Check if node active
               const std::vector<uint8_t> & rc_ActiveNodes = pc_View->GetNodeActiveFlags();
               if ((c_NodeIndexes[u32_ItNode] < rc_ActiveNodes.size()) && (rc_ActiveNodes[u32_ItNode] == 1U))
               {
                  const C_OscNode * const pc_Node =
                     C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndexes[u32_ItNode]);
                  //Check which type of node
                  if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL) &&
                      (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()))
                  {
                     if (pc_Bus->e_Type == C_OscSystemBus::eCAN)
                     {
                        if (pc_Node->c_Properties.e_FlashLoader == C_OscNodeProperties::eFL_OPEN_SYDE)
                        {
                           q_Osy = true;
                        }
                        else if (pc_Node->c_Properties.e_FlashLoader == C_OscNodeProperties::eFL_STW)
                        {
                           q_Stw = true;
                        }
                        else
                        {
                           // do nothing
                        }
                     }
                     else
                     {
                        if ((pc_Node->c_Properties.e_FlashLoader == C_OscNodeProperties::eFL_OPEN_SYDE) &&
                            (pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].
                             q_FlashloaderOpenSydeEthernet == true))
                        {
                           q_Osy = true;
                        }
                     }
                  }
               }
            }

            if (q_Stw == true)
            {
               while (this->mpc_DcSequences->ResetCanStwFlashloaderDevices() == C_BUSY)
               {
                  //In case it takes longer do process events to handle cursor and proper show of message box
                  QApplication::processEvents(QEventLoop::AllEvents, 50);
               }
               q_StartedAnything = true;
            }

            if (q_Osy == true)
            {
               while (this->mpc_DcSequences->ResetOpenSydeDevices(false) == C_BUSY)
               {
                  //In case it takes longer do process events to handle cursor and proper show of message box
                  QApplication::processEvents(QEventLoop::AllEvents, 50);
               }

               //Before closing the bus wait a little to make sure the CAN reset request broadcast(s) had the chance to
               // really be sent out on the bus
               stw::tgl::TglSleep(50U);

               q_StartedAnything = true;
            }

            if (q_StartedAnything == true)
            {
               //Wait for the thread to finish
               while (this->mpc_DcSequences->GetResults(s32_Result) == C_BUSY)
               {
                  //In case it takes longer do process events to handle cursor and proper show of message box
                  QApplication::processEvents(QEventLoop::AllEvents, 50);
               }
            }
            QApplication::restoreOverrideCursor();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if quit is currently allowed

   \return
   True  Exit allowed
   False Exit critical
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcWidget::m_CheckQuitPossible(void) const
{
   bool q_Retval = false;
   int32_t s32_Result;

   if ((this->mpc_DcSequences == NULL) ||
       (this->mpc_DcSequences->GetResults(s32_Result) == C_NO_ERR))
   {
      q_Retval = true;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle higher level close ignored event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_OnCloseIgnored(void)
{
   if (m_CheckQuitPossible() == true)
   {
      m_DoCompleteDisconnect();
      this->mpc_ParentDialog->reject();
   }
   else
   {
      m_InformUserAboutAbortedClose();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Inform user why close request were ignored
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_InformUserAboutAbortedClose(void) const
{
   C_OgeWiCustomMessage c_Message(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eINFORMATION);

   c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
   c_Message.SetDescription(C_GtGetText::h_GetText(
                               "While the communication is running, an abort can lead to an inconsistent status of the system."));
   c_Message.SetCustomMinHeight(180, 180);
   c_Message.Execute();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Inform user after failed scan
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcWidget::m_InformUserAfterScan(const QString & orc_Text)
{
   C_OgeWiCustomMessage c_Message(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eERROR);

   c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
   c_Message.SetDescription(orc_Text);
   c_Message.SetCustomMinHeight(180, 180);
   c_Message.Execute();
}
