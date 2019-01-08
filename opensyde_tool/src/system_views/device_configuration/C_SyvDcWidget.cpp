//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the process of configuring all nodes.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.11.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QApplication>

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_Uti.h"
#include "C_SyvDcWidget.h"
#include "ui_C_SyvDcWidget.h"
#include "C_OSCNodeProperties.h"
#include "C_GtGetText.h"
#include "C_Uti.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "TGLUtils.h"
#include "TGLTime.h"
#include "C_PuiSdUtil.h"
#include "C_OSCLoggingHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const QString C_SyvDcWidget::mhc_REPORT_HEADLINE_HTML_TAG_START = "<h3>";
const QString C_SyvDcWidget::mhc_REPORT_HEADLINE_HTML_TAG_END = "</h3>";
const QString C_SyvDcWidget::mhc_REPORT_HIGHLIGHT_TAG_START = "<span style=\" font-weight: bold;\">";
const QString C_SyvDcWidget::mhc_REPORT_HIGHLIGHT_TAG_END = "</span>";
const sint32 C_SyvDcWidget::mhs32_INDEX_CONFIGURATION_ALL_CONNECTED_INTERFACES = 1;
const sint32 C_SyvDcWidget::mhs32_INDEX_CONFIGURATION_ONLY_USED_INTERFACES = 0;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent           Reference to parent

   \created     27.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDcWidget::C_SyvDcWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                             const stw_types::uint32 ou32_ViewIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDcWidget),
   mpc_ParentDialog(&orc_Parent),
   mpc_DcSequences(new C_SyvDcSequences()),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_TempBitrate(0U),
   mq_SameBitrates(false),
   mu32_BusIndex(0U),
   mu8_BusId(0U),
   mu64_BitRate(0U),
   mq_DisconnectNecessary(false),
   me_BusType(C_OSCSystemBus::eCAN),
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
      this->mu32_BusIndex = pc_View->GetPcData().GetBusIndex();
      const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_BusIndex);

      if (pc_Bus != NULL)
      {
         this->mu8_BusId = pc_Bus->u8_BusID;
         this->me_BusType = pc_Bus->e_Type;
         this->mu64_BitRate = pc_Bus->u64_BitRate;

         this->mpc_Ui->pc_ComboBoxBitRate->setVisible(this->me_BusType == C_OSCSystemBus::eCAN);
         this->mpc_Ui->pc_LabelBitRate->setVisible(this->me_BusType == C_OSCSystemBus::eCAN);

         //Update title
         this->mpc_ParentDialog->SetTitle(pc_Bus->c_Name.c_str());
      }
   }

   //Deactivate working screens
   this->mpc_Ui->pc_GroupBoxScan->setVisible(true);
   this->mpc_Ui->pc_GroupBoxAssignment->setVisible(false);
   this->mpc_Ui->pc_GroupBoxReport->setVisible(false);

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SyvDcWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SyvDcWidget::m_CancelClicked);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvDcWidget::m_Timer);
   // Progress signals
   // We need a signal which is Q_EMITted in the other thread
   // We have to register the type and use the queued connections. Auto is default, but did not work.
   qRegisterMetaType<uint32>("uint32");
   qRegisterMetaType<sint32>("sint32");
   qRegisterMetaType<uint8>("uint8");
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

   connect(this->mpc_Ui->pc_ListWidgetExistingNodesAssignment, &C_SyvDcExistingNodeList::SigConnect, this,
           &C_SyvDcWidget::m_AssignmentConnect);
   connect(this->mpc_Ui->pc_ListWidgetExistingNodesAssignment, &C_SyvDcExistingNodeList::SigDisconnect, this,
           &C_SyvDcWidget::m_AssignmentDisconnect);
   connect(this->mpc_Ui->pc_PushButtonScan, &stw_opensyde_gui_elements::C_OgePubConfigure::clicked, this,
           &C_SyvDcWidget::m_StartSearchProper);
   connect(this->mpc_Ui->pc_PushButtonConfigure, &stw_opensyde_gui_elements::C_OgePubConfigure::clicked, this,
           &C_SyvDcWidget::m_StartConfigProper);
   connect(this->mpc_Ui->pc_PbBackToScan, &stw_opensyde_gui_elements::C_OgePubConfigure::clicked, this,
           &C_SyvDcWidget::m_BackToScan);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxBitRate, static_cast<void (QComboBox::*)(
                                                            sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDcWidget::m_OnBitRateChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxConfigurationMode, static_cast<void (QComboBox::*)(
                                                                      sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDcWidget::m_OnDeviceConfigModeChanged);
   connect(this->mpc_ParentDialog, &C_OgePopUpDialog::SigCloseIgnored, this, &C_SyvDcWidget::m_OnCloseIgnored);

   this->mc_Timer.setInterval(10);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     27.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDcWidget::~C_SyvDcWidget(void)
{
   this->CleanUp();
   delete mpc_Ui;
   //lint -e{1579}  no memory leak because mpc_DcSequences is deleted in CleanUp
   //lint -e{1740}  no memory leak because the ownership of these objects was never transferred to this class
}

//-----------------------------------------------------------------------------
/*!
   \brief   Close things

   Can be used in case of a to late call of destructor

   \created     01.08.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::CleanUp(void)
{
   //Stop timer just in case
   this->mc_Timer.stop();
   delete mpc_DcSequences;
   mpc_DcSequences = NULL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initializes all visible strings on the widget

   \created     06.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::InitText(void)
{
   // set title
   this->mpc_ParentDialog->SetSubTitle(QString(C_GtGetText::h_GetText("Device Configuration")));

   this->mpc_Ui->pc_LabelHeadingReport->setText(QString(C_GtGetText::h_GetText("Report")));
   this->mpc_Ui->pc_LabelHeadingBitrate->setText(QString(C_GtGetText::h_GetText("Settings")));
   this->mpc_Ui->pc_LabelConfigurationMode->setText(QString(C_GtGetText::h_GetText("Configuration Mode")));
   this->mpc_Ui->pc_PushButtonScan->setText(QString(C_GtGetText::h_GetText("Scan for Devices")));
   this->mpc_Ui->pc_LabelStartScan->setText(QString(C_GtGetText::h_GetText("Click on \"Scan for Devices\" ...")));
   this->mpc_Ui->pc_LabelNoExistingNodes->setText(QString(C_GtGetText::h_GetText("No valid Topology nodes found.")));
   this->mpc_Ui->pc_LabelBitRate->setText(QString(C_GtGetText::h_GetText("Bitrate of Connected Devices")));
   this->mpc_Ui->pc_PbBackToScan->setText(QString(C_GtGetText::h_GetText("Back to Scan")));
   this->mpc_Ui->pc_LabelHeadingAssignment->setText(QString(C_GtGetText::h_GetText("Assignment")));
   this->mpc_Ui->pc_LabelAssignment->setText(QString(C_GtGetText::h_GetText(
                                                        "Assign all connected devices to Topology nodes via drag&drop"
                                                        " and click on \"Configure Devices\".")));
   this->mpc_Ui->pc_PushButtonConfigure->setText(QString(C_GtGetText::h_GetText("Configure Devices")));
   this->mpc_Ui->pc_LabelProgressScan->setText(QString(C_GtGetText::h_GetText("Scan")));
   this->mpc_Ui->pc_LabelProgressAssignmnet->setText(QString(C_GtGetText::h_GetText("Assignment")));
   this->mpc_Ui->pc_LabelProgressConfiguration->setText(QString(C_GtGetText::h_GetText("Configuration")));
   this->mpc_Ui->pc_LabelProgressFinished->setText(QString(C_GtGetText::h_GetText("Finished")));
   this->mpc_Ui->pc_ComboBoxConfigurationMode->clear();

   // Order is important!
   // Index 0 = all connected interfaces in the system.
   // Index 1 = only the for the device configuration used interface
   this->mpc_Ui->pc_ComboBoxConfigurationMode->addItem(C_GtGetText::h_GetText("Only directly connected interfaces"));
   this->mpc_Ui->pc_ComboBoxConfigurationMode->addItem(C_GtGetText::h_GetText("All connected interfaces"));

   //Tool Tips
   this->mpc_Ui->pc_LabelConfigurationMode->SetToolTipInformation(
      C_GtGetText::h_GetText("Configuration Mode"),
      C_GtGetText::h_GetText(
         "Property to choose which interfaces will be "
         "configured by this device configuration step.\n"
         "Warning: Configuring all connected interfaces may lead to "
         "inconsistent configurations on other connected "
         "buses.\n\n"
         "Only directly connected interfaces: "
         "All interfaces which are connected to the bus which is used"
         " by the current device configuration.\n"
         "All connected interfaces: "
         "\nAll interfaces which are connected to any bus in the System Definition.\n"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information

   \created     04.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);
   QWidget::paintEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: handle escape key

   \param[in,out] opc_Event Event identification and information

   \created     07.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (opc_Event->key() == static_cast<sintn>(Qt::Key_Escape))
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

//-----------------------------------------------------------------------------
/*!
   \brief   Init com driver

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid

   \created     12.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SyvDcWidget::m_InitSequence(void)
{
   QString c_Message;
   sint32 s32_Retval;

   // Sequence initialization
   if (this->mpc_DcSequences != NULL)
   {
      if (this->mpc_DcSequences->IsInitialized() == false)
      {
         s32_Retval = this->mpc_DcSequences->InitDcSequences(this->mu32_ViewIndex);
      }
      else
      {
         s32_Retval = C_NO_ERR;
      }

      switch (s32_Retval)
      {
      case C_NO_ERR:
         break;
      case C_CONFIG:
         c_Message = QString(C_GtGetText::h_GetText("Invalid System Definition/View configuration."));
         break;
      case C_RD_WR:
         c_Message =
            QString(C_GtGetText::h_GetText("Configured communication DLL does not exist or DLL could not be opened."));
         break;
      case C_OVERFLOW:
         c_Message =
            QString(C_GtGetText::h_GetText(
                       "Unknown transport protocol or unknown diagnostic server for at least one Node."));
         break;
      case C_NOACT:
         c_Message = QString(C_GtGetText::h_GetText("System View is invalid. Action cannot be performed."));
         break;
      case C_COM:
         c_Message =
            QString(C_GtGetText::h_GetText(
                       "CAN initialization failed. Check your PC CAN interface configuration "
                       "(System View setup - double click on PC)."));
         break;
      case C_CHECKSUM:
         c_Message = QString(C_GtGetText::h_GetText("Internal buffer overflow detected."));
         break;
      case C_RANGE:
         c_Message = QString(C_GtGetText::h_GetText("Routing configuration failed."));
         break;
      case C_UNKNOWN_ERR:
         c_Message = QString(C_GtGetText::h_GetText("Wrapped error of internal function call."));
         break;
      case C_WARN:
         c_Message = QString(C_GtGetText::h_GetText("Internal error."));
         break;
      default:
         c_Message = QString(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
         break;
      }
   }
   else
   {
      c_Message = QString(C_GtGetText::h_GetText("Internal error. Sequence not available."));
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);
      c_MessageBox.SetDescription(c_Message);
      m_CleanUpScan();
      c_MessageBox.Execute();
   }

   //Simplify return value
   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start search

   \created     12.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_StartSearchProper(void)
{
   //Init com driver
   sint32 s32_Return;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   //Prepare UI
   this->mpc_Ui->pc_BushButtonCancel->setEnabled(false);
   this->mpc_Ui->pc_ComboBoxBitRate->setEnabled(false);
   this->mpc_Ui->pc_ComboBoxConfigurationMode->setEnabled(false);
   this->mpc_Ui->pc_PushButtonScan->setEnabled(false);
   this->mpc_Ui->pc_GroupBoxStartScan->setVisible(true);
   this->mpc_Ui->pc_ListWidgetConnectedNodesScan->setVisible(false);
   this->mpc_Ui->pc_LabelScanFoundDevicesWarning->setVisible(false);
   this->mpc_Ui->pc_LabelStartScan->setText(C_GtGetText::h_GetText("Scanning for Devices..."));
   this->mpc_Ui->pc_LabelHeadingFoundDevices->setText(QString(C_GtGetText::h_GetText("Connected Devices")));

   s32_Return = m_InitSequence();

   this->mc_FoundDevices.clear();

   if (this->mpc_DcSequences != NULL)
   {
      //No error report necessary here
      if (s32_Return == C_NO_ERR)
      {
         //Bit rate
         s32_Return = this->GetBitRateValue(this->mu32_TempBitrate);
         if (s32_Return == C_NO_ERR)
         {
            // Check if different bitrates are used
            this->mq_SameBitrates = (this->mu32_TempBitrate == (this->mu64_BitRate / 1000U));

            //Set step
            if (this->me_BusType == C_OSCSystemBus::eCAN)
            {
               //Check scan
               this->me_Step = eSCANCANENTERFLASHLOADER;
               s32_Return = this->mpc_DcSequences->ScanCanEnterFlashloader(this->mu32_TempBitrate);
            }
            else
            {
               // No concrete enter flashloader function for Ethernet
               this->me_Step = eSCANETHGETINFOFROMOPENSYDEDEVICES;
               s32_Return = this->mpc_DcSequences->ScanEthGetInfoFromOpenSydeDevices();
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
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading("Failed!");
               c_Message.SetDescription(C_GtGetText::h_GetText("Could not continue with step: enter Flashloader."));
               m_CleanUpScan();
               c_Message.Execute();
            }
         }
         else
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_Message.SetDescription(C_GtGetText::h_GetText("Unknown bitrate."));
            m_CleanUpScan();
            c_Message.Execute();
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up scan

   \created     12.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_CleanUpScan(void) const
{
   this->mpc_Ui->pc_BushButtonCancel->setEnabled(true);
   this->mpc_Ui->pc_ComboBoxBitRate->setEnabled(this->me_BusType == C_OSCSystemBus::eCAN);
   this->mpc_Ui->pc_ComboBoxConfigurationMode->setEnabled(true);
   this->mpc_Ui->pc_PushButtonScan->setEnabled(true);
   //Latest possible cursor restoration
   QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Scan finished

   \created     12.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_ScanFinished(void)
{
   if (this->mc_FoundDevices.size() == 0)
   {
      //Zero
      this->mpc_Ui->pc_ProgressScan->SetProgress(0, false);
      this->mpc_Ui->pc_LabelScanFoundDevicesWarning->setVisible(true);
      this->mpc_Ui->pc_LabelScanFoundDevicesWarning->setText(C_GtGetText::h_GetText("Warning: No devices found!"));
      this->mpc_Ui->pc_LabelStartScan->setText(C_GtGetText::h_GetText("No devices found."));
   }
   else if (this->mc_FoundDevices.size() < static_cast<uint32>(this->mpc_Ui->pc_ListWidgetExistingNodes->count()))
   {
      //Less
      m_EnterScanErrorState(C_GtGetText::h_GetText("Warning: Less devices found than defined by the System View!"
                                                   " Check connection of connected devices and retry."));
   }
   else if (this->mc_FoundDevices.size() > static_cast<uint32>(this->mpc_Ui->pc_ListWidgetExistingNodes->count()))
   {
      //More
      m_EnterScanErrorState(C_GtGetText::h_GetText("Warning: More devices found than defined by the System View! "
                                                   "Connect only devices which are defined in the System View and retry."));
   }
   else
   {
      const bool q_SameSerialNumber = m_CheckSameSerialNumber();
      //Match
      if (q_SameSerialNumber == true)
      {
         //Same serial number error
         m_EnterScanErrorState(
            C_GtGetText::h_GetText("Warning: Devices with duplicate serial numbers found! Please ensure each device "
                                   "is only connected by one interface for device configuration and retry."));
      }
      else
      {
         //Switch screens
         this->mpc_Ui->pc_GroupBoxScan->setVisible(false);
         this->mpc_Ui->pc_GroupBoxAssignment->setVisible(true);
         m_InitAssignmentScreen();
         this->mpc_Ui->pc_LabelStartScan->setText(QString(C_GtGetText::h_GetText("Click on \"Scan for Devices\" ...")));
         //Progress
         this->mpc_Ui->pc_BopperleScan->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
         this->mpc_Ui->pc_ProgressScan->SetProgress(100, false);
         this->mpc_Ui->pc_BopperleAssignment->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
      }
   }

   m_CleanUpScan();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if all found serial numbers are unique

   \return
   True  Not unique
   False All unique

   \created     24.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvDcWidget::m_CheckSameSerialNumber(void)
{
   bool q_Retval = false;

   //Match
   //Compare devices
   for (uint32 u32_ItDevice1 = 0; u32_ItDevice1 < this->mc_FoundDevices.size(); ++u32_ItDevice1)
   {
      const C_SyvDcDeviceInformation & rc_Device1 = this->mc_FoundDevices[u32_ItDevice1];
      for (uint32 u32_ItDevice2 = u32_ItDevice1 + 1; u32_ItDevice2 < this->mc_FoundDevices.size(); ++u32_ItDevice2)
      {
         const C_SyvDcDeviceInformation & rc_Device2 = this->mc_FoundDevices[u32_ItDevice2];
         const sintn sn_Diff = std::memcmp(&rc_Device1.au8_SerialNumber[0], &rc_Device2.au8_SerialNumber[0], 6U);
         if (sn_Diff == 0)
         {
            //same serial number
            q_Retval = true;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Enter scan error state

   \param[in] orc_Text Scan error text

   \created     24.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_EnterScanErrorState(const QString & orc_Text) const
{
   this->mpc_Ui->pc_ProgressScan->SetProgress(0, false);
   this->mpc_Ui->pc_LabelScanFoundDevicesWarning->setVisible(true);
   this->mpc_Ui->pc_LabelScanFoundDevicesWarning->setText(orc_Text);
   this->mpc_Ui->pc_ListWidgetConnectedNodesScan->setVisible(true);
   this->mpc_Ui->pc_ListWidgetConnectedNodesScan->SetData(this->mc_FoundDevices);
   //No drag
   this->mpc_Ui->pc_ListWidgetConnectedNodesScan->setDragEnabled(false);
   this->mpc_Ui->pc_GroupBoxStartScan->setVisible(false);
   this->mpc_Ui->pc_LabelHeadingFoundDevices->setText(QString(C_GtGetText::h_GetText(
                                                                 "Connected Devices (%1)")).arg(
                                                         this->mc_FoundDevices.size()));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start configuration

   \created     12.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_StartConfigProper(void)
{
   if (this->mpc_DcSequences != NULL)
   {
      sint32 s32_Result = C_NO_ERR;

      std::vector<C_SyvDcDeviceConfiguation> c_Configs =
         this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->GetConfigs();
      //Prepare
      this->mc_OpenSydeDeviceConfigurations.clear();
      this->mc_StwFlashloaderDeviceConfigurations.clear();

      //Append
      for (uint32 u32_ItConfig = 0; (u32_ItConfig < c_Configs.size()) && (s32_Result == C_NO_ERR); ++u32_ItConfig)
      {
         C_OSCNodeProperties::E_FlashLoaderProtocol e_Flashloader;
         C_SyvDcDeviceConfiguation & rc_Config = c_Configs[u32_ItConfig];
         s32_Result = this->mpc_DcSequences->FillDeviceConfig(rc_Config, e_Flashloader);
         if (s32_Result == C_NO_ERR)
         {
            if (e_Flashloader == C_OSCNodeProperties::eFL_OPEN_SYDE)
            {
               this->mc_OpenSydeDeviceConfigurations.push_back(rc_Config);
            }
            else
            {
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
               if (this->me_BusType == C_OSCSystemBus::eCAN)
               {
                  // Run and check the STW flashloader configuration
                  s32_Result =
                     this->mpc_DcSequences->ConfCanStwFlashloaderDevices(this->mc_StwFlashloaderDeviceConfigurations);

                  if (s32_Result != C_NO_ERR)
                  {
                     C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                     c_Message.SetDescription(C_GtGetText::h_GetText("Could not configure STW Flashloader devices."));
                     c_Message.Execute();
                  }
               }
               else
               {
                  C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                  c_Message.SetDescription(C_GtGetText::h_GetText(
                                              "Could not configure STW Flashloader devices over Ethernet."));
                  c_Message.Execute();

                  s32_Result = C_NOACT;
               }
            }
            else
            {
               // No STW flashloader devices. Run and check the openSYDE configuration
               if (this->me_BusType == C_OSCSystemBus::eCAN)
               {
                  s32_Result =
                     this->mpc_DcSequences->ConfCanOpenSydeDevices(this->mc_OpenSydeDeviceConfigurations,
                                                                   this->m_AreAllInterfacesToConfigure());
               }
               else
               {
                  s32_Result =
                     this->mpc_DcSequences->ConfEthOpenSydeDevices(this->mc_OpenSydeDeviceConfigurations,
                                                                   this->m_AreAllInterfacesToConfigure());
               }

               if (s32_Result != C_NO_ERR)
               {
                  C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                  c_Message.SetDescription(C_GtGetText::h_GetText("Could not configure openSYDE devices."));
                  c_Message.Execute();
               }
            }

            if (s32_Result == C_NO_ERR)
            {
               //Switch screens
               this->mpc_Ui->pc_GroupBoxAssignment->setVisible(false);
               this->mpc_Ui->pc_GroupBoxReport->setVisible(true);
               //Clear report
               this->m_ClearReportText();
               m_HandleConfigurationStarted();
               //Start
               if (this->me_BusType == C_OSCSystemBus::eCAN)
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
            c_Message.Execute();
         }
      }
      else
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetDescription(QString(C_GtGetText::h_GetText("Internal error FillDeviceConfig %1.")).
                                  arg(C_Uti::h_StwError(s32_Result)));
         c_Message.Execute();
      }
   }
   else
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetDescription(QString(C_GtGetText::h_GetText("Internal error starting configuration.")));
      c_Message.Execute();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Displays the configuration parameters

   \created     06.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigResult(void)
{
   const uint32 u32_DeviceMaxCount = static_cast<uint32>(this->mc_OpenSydeDeviceConfigurations.size() +
                                                         this->mc_StwFlashloaderDeviceConfigurations.size());
   uint32 u32_DeviceCounter = 1UL;
   uint32 u32_ConfigCounter;
   QString c_Text;

   // Show configuration for all STW flashloader devices
   for (u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_StwFlashloaderDeviceConfigurations.size();
        ++u32_ConfigCounter)
   {
      this->m_ShowConfigInfoOfDevice(this->mc_StwFlashloaderDeviceConfigurations[u32_ConfigCounter],
                                     u32_DeviceMaxCount, u32_DeviceCounter, c_Text);
      ++u32_DeviceCounter;

      c_Text += "<br>";
   }

   // Show configuration for all openSYDE flashloader devices
   for (u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_OpenSydeDeviceConfigurations.size();
        ++u32_ConfigCounter)
   {
      this->m_ShowConfigInfoOfDevice(this->mc_OpenSydeDeviceConfigurations[u32_ConfigCounter], u32_DeviceMaxCount,
                                     u32_DeviceCounter, c_Text);
      ++u32_DeviceCounter;

      c_Text += "<br>";
   }

   this->m_UpdateReportText(c_Text);
}

//-----------------------------------------------------------------------------
/*!
   \brief   handle back to scan action

   \created     13.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_BackToScan(void)
{
   this->mpc_Ui->pc_PbBackToScan->setEnabled(false);
   this->mpc_Ui->pc_BushButtonCancel->setVisible(true);
   this->mpc_Ui->pc_BushButtonOk->setVisible(false);
   this->mpc_Ui->pc_GroupBoxScan->setVisible(true);
   this->mpc_Ui->pc_GroupBoxAssignment->setVisible(false);
   this->mpc_Ui->pc_GroupBoxReport->setVisible(false);

   this->mc_ServerStates.clear();

   m_InitScanScreen();
}

//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigInfoOfDevice(const C_SyvDcDeviceConfiguation & orc_Config,
                                             const uint32 ou32_DeviceMaxCount, const uint32 ou32_DeviceCounter,
                                             QString & orc_Text)
{
   tgl_assert(orc_Config.c_NodeIds.size() > 0);
   if ((this->mpc_DcSequences != NULL) &&
       (orc_Config.c_NodeIds.size() > 0))
   {
      C_OSCProtocolDriverOsyNode c_ServerId(this->mu8_BusId, orc_Config.c_NodeIds[0]);
      uint32 u32_NodeIndex;

      // Counter of devices
      orc_Text += QString("%1 %2 %3 %4<br>").arg(
         QString(C_GtGetText::h_GetText("Device")), QString::number(ou32_DeviceCounter),
         QString(C_GtGetText::h_GetText("of")), QString::number(ou32_DeviceMaxCount));

      // Serial number
      orc_Text += "SN.: " +
                  QString(C_SyvDcDeviceInformation::h_SerialNumberToString(orc_Config.au8_SerialNumber).c_str()) +
                  "<br>";

      if (this->mpc_DcSequences->GetNodeIndex(c_ServerId, u32_NodeIndex) == true)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(u32_NodeIndex);

         if (pc_Node != NULL)
         {
            uint32 u32_IntfCounter;
            uint32 u32_CanCounter = 0U;
            uint32 u32_EthCounter = 0U;

            // Get topology node name
            orc_Text += QString(C_GtGetText::h_GetText("Topology Node: ")) +
                        QString(pc_Node->c_Properties.c_Name.c_str()) + "<br>";

            // Interface
            // Show the for configuration used interface first
            // This interface is on the first position in the configuration vectors

            for (u32_IntfCounter = 0U; u32_IntfCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                 ++u32_IntfCounter)
            {
               const C_OSCNodeComInterfaceSettings & rc_IntfSetting =
                  pc_Node->c_Properties.c_ComInterfaces[u32_IntfCounter];

               if ((rc_IntfSetting.q_IsBusConnected == true) &&
                   (static_cast<uint8>(rc_IntfSetting.u32_BusIndex) == this->mu32_BusIndex))
               {
                  if (rc_IntfSetting.e_InterfaceType == C_OSCSystemBus::eCAN)
                  {
                     this->m_ShowConfigInfoOfCanInterface(rc_IntfSetting, c_ServerId, c_ServerId,
                                                          orc_Config.c_CanBitrates[0], orc_Text);
                     ++u32_CanCounter;
                  }
                  else
                  {
                     this->m_ShowConfigInfoOfEthInterface(rc_IntfSetting, c_ServerId, c_ServerId,
                                                          orc_Config.c_IpAddresses[0],
                                                          orc_Text);
                     ++u32_EthCounter;
                  }

                  break;
               }
            }

            // Show all other interfaces
            for (u32_IntfCounter = 0U; u32_IntfCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                 ++u32_IntfCounter)
            {
               const C_OSCNodeComInterfaceSettings & rc_IntfSetting =
                  pc_Node->c_Properties.c_ComInterfaces[u32_IntfCounter];

               if (rc_IntfSetting.q_IsBusConnected == false)
               {
                  // No further information for a not used connection
                  this->m_ShowConfigInfoOfInterface(rc_IntfSetting, c_ServerId, c_ServerId, orc_Text, false, false);
               }
               else if ((pc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW) &&
                        (((static_cast<uint8>(rc_IntfSetting.u32_BusIndex) == this->mu32_BusIndex) &&
                          (rc_IntfSetting.u8_NodeID != c_ServerId.u8_NodeIdentifier)) ||
                         (static_cast<uint8>(rc_IntfSetting.u32_BusIndex) != this->mu32_BusIndex)))
               {
                  this->m_ShowConfigInfoOfInterface(rc_IntfSetting, c_ServerId, c_ServerId, orc_Text, true, false);
               }
               else
               {
                  uint32 u32_IntfConfigCounter;

                  // Start at 1, because zero is the for configuration used interface
                  for (u32_IntfConfigCounter = 1U; u32_IntfConfigCounter < orc_Config.c_NodeIds.size();
                       ++u32_IntfConfigCounter)
                  {
                     const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                        rc_IntfSetting.u32_BusIndex);
                     if (pc_Bus != NULL)
                     {
                        if ((pc_Bus->u8_BusID == orc_Config.c_BusIds[u32_IntfConfigCounter]) &&
                            (rc_IntfSetting.u8_NodeID == orc_Config.c_NodeIds[u32_IntfConfigCounter]))
                        {
                           if (rc_IntfSetting.e_InterfaceType == C_OSCSystemBus::eCAN)
                           {
                              this->m_ShowConfigInfoOfCanInterface(rc_IntfSetting,
                                                                   c_ServerId,
                                                                   C_OSCProtocolDriverOsyNode(pc_Bus->u8_BusID,
                                                                                              rc_IntfSetting.u8_NodeID),
                                                                   orc_Config.c_CanBitrates[u32_CanCounter],
                                                                   orc_Text);
                              ++u32_CanCounter;
                           }
                           else
                           {
                              this->m_ShowConfigInfoOfEthInterface(rc_IntfSetting,
                                                                   c_ServerId,
                                                                   C_OSCProtocolDriverOsyNode(pc_Bus->u8_BusID,
                                                                                              rc_IntfSetting.u8_NodeID),
                                                                   orc_Config.c_IpAddresses[u32_EthCounter],
                                                                   orc_Text);
                              ++u32_EthCounter;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Creates string with the information of a CAN interface

   \param[in]     orc_IntfSetting               Configuration of the current interface
   \param[in]     orc_ServerIdOnUsedBus         Server ID of the node on the for the communication used interface
   \param[in]     orc_ServerIdOnConfiguredBus   Server ID of the node on the configured interface
   \param[in]     ou32_Bitrate                  Set bitrate for the configured interface
   \param[in]     orc_Text                      Result text for the interface

   \created     06.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigInfoOfCanInterface(const C_OSCNodeComInterfaceSettings & orc_IntfSetting,
                                                   const C_OSCProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                                   const C_OSCProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus,
                                                   const uint32 ou32_Bitrate, QString & orc_Text)
{
   this->m_ShowConfigInfoOfInterface(orc_IntfSetting, orc_ServerIdOnUsedBus, orc_ServerIdOnConfiguredBus, orc_Text,
                                     true, true);

   if ((this->m_AreAllInterfacesToConfigure() == true) ||
       (orc_ServerIdOnUsedBus == orc_ServerIdOnConfiguredBus))
   {
      orc_Text += "<tr>";
      orc_Text += "<td width=\"40%\">" +
                  QString(C_GtGetText::h_GetText("CAN-Bitrate: ")) + QString::number(ou32_Bitrate / 1000U) +
                  QString(" kBit/s ") + "</td>";
      orc_Text += "<td width=\"20%\">" +
                  this->m_GetStateStringOfServerStep(C_SyvDcSequences::hu32_SETCANBITRATE, orc_ServerIdOnConfiguredBus,
                                                     orc_IntfSetting.e_InterfaceType,
                                                     orc_IntfSetting.u8_InterfaceNumber) + "</td>";
      orc_Text += "</tr>";
      orc_Text += "</table>";
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Creates string with the information of an Ethernet interface

   \param[in]     orc_IntfSetting               Configuration of the current interface
   \param[in]     orc_ServerIdOnUsedBus         Server ID of the node on the for the communication used interface
   \param[in]     orc_ServerIdOnConfiguredBus   Server ID of the node on the configured interface
   \param[in]     orc_IpAddress                 Set IP address for the configured interface
   \param[in]     orc_Text                      Result text for the interface

   \created     06.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigInfoOfEthInterface(const C_OSCNodeComInterfaceSettings & orc_IntfSetting,
                                                   const C_OSCProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                                   const C_OSCProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus,
                                                   const C_OSCNodeComInterfaceSettings::C_IpAddress & orc_IpAddress,
                                                   QString & orc_Text)
{
   QString c_IpAddress;
   QString c_NetMask;
   QString c_DefaultGateway;

   this->m_ShowConfigInfoOfInterface(orc_IntfSetting, orc_ServerIdOnUsedBus, orc_ServerIdOnConfiguredBus, orc_Text,
                                     true, true);

   c_IpAddress = C_Uti::h_IpAddressToString(orc_IpAddress.au8_IpAddress);
   c_NetMask = C_Uti::h_IpAddressToString(orc_IpAddress.au8_NetMask);
   c_DefaultGateway = C_Uti::h_IpAddressToString(orc_IpAddress.au8_DefaultGateway);

   orc_Text += "<tr>";
   orc_Text += "<td width=\"40%\">" + QString(C_GtGetText::h_GetText("IP: ")) + c_IpAddress +
               QString(C_GtGetText::h_GetText(" (Sub-Net: ")) + c_NetMask +
               QString(C_GtGetText::h_GetText(", default gateway: ")) + c_DefaultGateway + ")</td>";
   orc_Text += "<td width=\"20%\">" + this->m_GetStateStringOfServerStep(C_SyvDcSequences::hu32_SETIPADDRESS,
                                                                         orc_ServerIdOnConfiguredBus,
                                                                         orc_IntfSetting.e_InterfaceType,
                                                                         orc_IntfSetting.u8_InterfaceNumber) + "</td>";
   orc_Text += "</tr>";
   orc_Text += "</table>";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Creates string with the generic information of an communication interface

   \param[in]     orc_IntfSetting               Configuration of the current interface
   \param[in]     orc_ServerIdOnUsedBus         Server ID of the node on the for the communication used interface
   \param[in]     orc_ServerIdOnConfiguredBus   Server ID of the node on the configured interface
   \param[in]     orc_Text                      Result text for the interface
   \param[in]     q_BusConnected                Flag if the interface is connected to a bus
   \param[in]     q_Configured                  Flag if the interface was configured by the device configuration

   \created     06.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowConfigInfoOfInterface(const C_OSCNodeComInterfaceSettings & orc_IntfSetting,
                                                const C_OSCProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                                const C_OSCProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus,
                                                QString & orc_Text, const bool q_BusConnected, const bool q_Configured)
{
   orc_Text += QString(C_GtGetText::h_GetText("Interface: "));

   //Interface
   orc_Text += C_PuiSdUtil::h_GetInterfaceName(orc_IntfSetting.e_InterfaceType, orc_IntfSetting.u8_InterfaceNumber);

   if (q_BusConnected == false)
   {
      orc_Text += QString(C_GtGetText::h_GetText(" (skipped, not used)<br>"));
   }
   else if (q_Configured == false)
   {
      orc_Text += QString(C_GtGetText::h_GetText(" (skipped, not configurable on this bus)<br>"));
   }
   else if ((this->m_AreAllInterfacesToConfigure() == false) &&
            (orc_ServerIdOnUsedBus != orc_ServerIdOnConfiguredBus))
   {
      // The interface shall not be configured, because it is not the used interface by the device configuration
      orc_Text += QString(C_GtGetText::h_GetText(" (skipped, connected to other bus)<br>"));
   }
   else
   {
      // HTML row in table start (Table row)
      orc_Text += "<table width=\"100%\" style =\" margin-left:10px\">";
      orc_Text += "<tr>";
      orc_Text += "<td width=\"40%\">" + QString(C_GtGetText::h_GetText("Node ID: ")) +
                  QString::number(static_cast<uint32>(orc_ServerIdOnConfiguredBus.u8_NodeIdentifier)) + "</td>";
      orc_Text += "<td width=\"20%\">" + this->m_GetStateStringOfServerStep(C_SyvDcSequences::hu32_SETNODEID,
                                                                            orc_ServerIdOnConfiguredBus,
                                                                            orc_IntfSetting.e_InterfaceType,
                                                                            orc_IntfSetting.u8_InterfaceNumber) +
                  "</td>";
      orc_Text += "</tr>";
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Resets the network dependent of the flashloader types

   Procedure depends on used protocol(s):

   * openSYDE Ethernet: broadcast NetReset
   * openSYDE CAN: broadcast ECUReset
   * STW Flashloader: netreset
   * openSYDE CAN and STW Flashloader: manual reset or broadcast ECUReset and netreset depends of oq_WithManualReset

   Note on manual reset after changing the bitrate on all CAN nodes:
   Trying to send OSY "EcuReset" broadcast and STW Flashloader "NetReset" at
   the same time could work sometimes depending on node timing. But it could also fail.
   A manual reset should always work.

   \param[in]  oq_FinishReset     Flag if manual reset is necessary (always when the bitrate was changed)
   \param[in]  oq_StartReadBack   Flag if m_StartReadBack shall be started automatically after reset
   \param[in]  oq_Silent          Flag if reporting will disabled

   \return
   C_NO_ERR    Reset finished
   C_BUSY      Previously started sequence still going on
   C_COM       Error on sending reset request
   C_CONFIG    No com driver installed

   \created     06.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_ResetNetwork(const bool oq_FinishReset, const bool oq_StartReadBack, const bool oq_Silent)
{
   if (this->mpc_DcSequences != NULL)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eWARNING);
      bool q_Wait;

      sint32 s32_Return;
      QString c_Text = "<table width=\"100%\"><tr><td width=\"40%\">";

      if ((this->mc_OpenSydeDeviceConfigurations.size() > 0) &&
          (this->mc_StwFlashloaderDeviceConfigurations.size() > 0))
      {
         if (oq_FinishReset == false)
         {
            // Mixed system. A reset of STW and openSYDE flashloader with changed bitrates is not reliable
            c_MessageBox.SetHeading(C_GtGetText::h_GetText("Devices reset"));
            c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText(
                                                   "Reset every node manually and press \"Continue\".")));
            c_MessageBox.SetDetails(C_GtGetText::h_GetText(
                                       "Your system uses both Flashloader types (STW Flashloader and openSYDE Flashloader.)"
                                       "\nand the bitrate of the CAN bus was changed."
                                       "\nIn this case the reset cannot be performed automatically."));

            c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Continue"));
            c_MessageBox.Execute();

            c_Text += QString(C_GtGetText::h_GetText("User action: manual reset"));
            q_Wait = false;

            s32_Return = C_NO_ERR;
         }
         else
         {
            s32_Return = this->mpc_DcSequences->ResetCanStwFlashloaderDevices();

            if (s32_Return == C_NO_ERR)
            {
               s32_Return = this->mpc_DcSequences->ResetCanOpenSydeDevices();
            }

            c_Text += QString(C_GtGetText::h_GetText("Performing automatic reset"));

            q_Wait = true;
         }
      }
      else if (this->mc_StwFlashloaderDeviceConfigurations.size() > 0)
      {
         // Only STW flashloader
         s32_Return = this->mpc_DcSequences->ResetCanStwFlashloaderDevices();

         c_Text += QString(C_GtGetText::h_GetText("Performing automatic reset"));

         q_Wait = true;
      }
      else
      {
         // Only openSYDE flashloader
         if (this->me_BusType == C_OSCSystemBus::eCAN)
         {
            s32_Return = this->mpc_DcSequences->ResetCanOpenSydeDevices();
         }
         else
         {
            s32_Return = this->mpc_DcSequences->ResetEthOpenSydeDevices();
         }

         c_Text += QString(C_GtGetText::h_GetText("Performing automatic reset"));

         q_Wait = true;
      }

      c_Text += "</td>";
      c_Text += "<td width=\"20%\">";
      c_Text += QString(C_GtGetText::h_GetText("DONE"));
      c_Text += "</td></tr></table>";
      if (q_Wait == true)
      {
         // TODO: Waiting for reset
         stw_tgl::TGL_Sleep(1000);
      }
      if (oq_Silent == true)
      {
         this->m_UpdateReportText(c_Text);
      }

      // Start next sequence
      if ((s32_Return == C_NO_ERR) &&
          (oq_StartReadBack == true))
      {
         this->mc_Timer.start();
         this->m_StartReadBack();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Starts the read back sequence

   A successful network reset after configuration is necessary

   \created     07.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_StartReadBack(void)
{
   uint32 u32_Counter;
   sint32 s32_Result = C_NO_ERR;
   uint32 u32_NewBitrate = 0U;

   std::vector<C_OSCProtocolDriverOsyNode> c_OsyNodes;
   C_OSCProtocolDriverOsyNode c_Id;

   for (u32_Counter = 0U; u32_Counter < this->mc_OpenSydeDeviceConfigurations.size(); ++u32_Counter)
   {
      const C_SyvDcDeviceConfiguation & rc_Config = this->mc_OpenSydeDeviceConfigurations[u32_Counter];

      if ((rc_Config.c_NodeIds.size() > 0) &&
          (rc_Config.c_BusIds.size() > 0))
      {
         // The first entries must be the used communication interface
         c_Id.u8_NodeIdentifier = rc_Config.c_NodeIds[0];
         c_Id.u8_BusIdentifier = rc_Config.c_BusIds[0];
         c_OsyNodes.push_back(c_Id);

         if (this->me_BusType == C_OSCSystemBus::eCAN)
         {
            // Not necessary when using Ethernet
            if (rc_Config.c_CanBitrates.size() > 0)
            {
               if (u32_NewBitrate == 0U)
               {
                  u32_NewBitrate = rc_Config.c_CanBitrates[0] / 1000U;
               }
            }
            else
            {
               s32_Result = C_RANGE;
            }
         }
      }
      else
      {
         s32_Result = C_RANGE;
      }

      if (s32_Result != C_NO_ERR)
      {
         break;
      }
   }

   if ((this->mpc_DcSequences != NULL) &&
       (s32_Result == C_NO_ERR))
   {
      m_HandleDeviceVerificationStart();
      if (this->me_BusType == C_OSCSystemBus::eCAN)
      {
         std::vector<C_OSCProtocolDriverOsyNode> c_StwNodes;

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
               c_StwNodes.push_back(c_Id);

               if (u32_NewBitrate == 0U)
               {
                  u32_NewBitrate = rc_Config.c_CanBitrates[0] / 1000U;
               }
            }
            else
            {
               s32_Result = C_RANGE;
               break;
            }
         }

         if (s32_Result == C_NO_ERR)
         {
            this->me_Step = eREADBACKCAN;
            s32_Result = this->mpc_DcSequences->ReadBackCan(u32_NewBitrate, c_OsyNodes, c_StwNodes);
         }
         else
         {
            // TODO: Error handling
         }
      }
      else
      {
         this->me_Step = eREADBACKETH;
         s32_Result = this->mpc_DcSequences->ReadBackEth(c_OsyNodes);
      }
   }
   else
   {
      // TODO: Error handling
   }

   if (s32_Result != C_NO_ERR)
   {
      this->mc_Timer.stop();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Displays the read values

   \param[in] os32_ActualResult Detected error state

   \created     06.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_ShowReadInfo(const sint32 os32_ActualResult)
{
   if (this->mpc_DcSequences != NULL)
   {
      std::vector<C_SyvDcDeviceInformation> c_DeviceInfos;
      const sint32 s32_Return = this->mpc_DcSequences->GetDeviceInfosResult(c_DeviceInfos);
      QString c_Text;

      // Last step. Nothing more to poll
      this->mc_Timer.stop();

      if (s32_Return == C_NO_ERR)
      {
         uint32 u32_DeviceTotal;
         uint32 u32_DeviceCounter;
         if (os32_ActualResult == C_NO_ERR)
         {
            u32_DeviceTotal = c_DeviceInfos.size();
         }
         else
         {
            u32_DeviceTotal = static_cast<uint32>(this->mc_OpenSydeDeviceConfigurations.size() +
                                                  this->mc_StwFlashloaderDeviceConfigurations.size());
         }

         for (u32_DeviceCounter = 0U; u32_DeviceCounter < c_DeviceInfos.size(); ++u32_DeviceCounter)
         {
            const C_SyvDcDeviceInformation & rc_Info = c_DeviceInfos[u32_DeviceCounter];
            C_OSCProtocolDriverOsyNode c_ServerId(this->mu8_BusId, rc_Info.u8_NodeId);
            uint32 u32_NodeIndex;
            QString c_TopologyNodeName = "NA";
            QString c_TopologyDeviceType = "NA";

            // Counter of devices
            c_Text += QString("%1 %2 %3 %4<br>").arg(
               QString(C_GtGetText::h_GetText("Device")), QString::number(u32_DeviceCounter + 1U),
               QString(C_GtGetText::h_GetText("of")), QString::number(u32_DeviceTotal));

            // Serial number
            c_Text += "SN.: ";
            if (rc_Info.q_SerialNumberValid == true)
            {
               c_Text += QString(C_SyvDcDeviceInformation::h_SerialNumberToString(rc_Info.au8_SerialNumber).c_str());
            }
            else
            {
               c_Text += QString(C_GtGetText::h_GetText("Not valid."));
            }
            c_Text += "<br>";

            // Get topology node name and device type
            if (this->mpc_DcSequences->GetNodeIndex(c_ServerId, u32_NodeIndex) == true)
            {
               const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(u32_NodeIndex);

               if (pc_Node != NULL)
               {
                  c_TopologyDeviceType = pc_Node->c_DeviceType.c_str();
                  c_TopologyNodeName = pc_Node->c_Properties.c_Name.c_str();
               }
            }

            // Topology node name
            c_Text += QString(C_GtGetText::h_GetText("Topology Node: ")) + c_TopologyNodeName;

            // Title of device type
            // HTML row in table start (Table row)
            c_Text += "<table width=\"100%\" style =\" margin-left:10px\">";
            c_Text += "<tr>";
            c_Text += "<td width=\"40%\">" + QString(C_GtGetText::h_GetText("Get type from device")) + "</td>";
            if (rc_Info.q_DeviceNameValid == true)
            {
               c_Text += "<td width=\"20%\">" + QString(C_GtGetText::h_GetText("OK")) + "</td>";
            }
            else
            {
               c_Text += "<td width=\"20%\">" + QString(C_GtGetText::h_GetText("NA")) + "</td>";
            }
            c_Text += "</tr>";
            c_Text += "</table>";

            c_Text += "<table width=\"100%\" style =\" margin-left:20px\">";
            // Read device type
            c_Text += "<tr>";
            c_Text += "<td width=\"40%\">" + QString(C_GtGetText::h_GetText("Read Type: ")) +
                      QString(rc_Info.c_DeviceName.c_str()) +
                      "</td>";
            c_Text += "</tr>";

            // Expected device type
            c_Text += "<tr>";
            c_Text += "<td width=\"40%\">" + QString(C_GtGetText::h_GetText("Expected Type: ")) + c_TopologyDeviceType +
                      "</td>";
            c_Text += "</tr>";
            c_Text += "</table>";

            // Result
            c_Text += "<table width=\"100%\" style =\" margin-left:10px\">";
            c_Text += "<tr>";
            c_Text += "<td width=\"40%\">" + QString(C_GtGetText::h_GetText("Verification")) + "</td>";
            if (QString(rc_Info.c_DeviceName.c_str()) == c_TopologyDeviceType)
            {
               c_Text += "<td width=\"20%\">" + QString(C_GtGetText::h_GetText("OK")) + "</td>";
            }
            else
            {
               c_Text += "<td width=\"20%\"><b>" + QString(C_GtGetText::h_GetText("FAILED")) + "</b></td>";
            }
            c_Text += "</tr>";

            c_Text += "</table>";
            c_Text += "<br><br>";
         }

         if (os32_ActualResult != C_NO_ERR)
         {
            c_Text += "<b>" + QString(C_GtGetText::h_GetText("Missing device(s):")) + "</b><br/>";
            //Add missing STW devices
            m_HandleMissingDevices(this->mc_StwFlashloaderDeviceConfigurations, c_DeviceInfos, c_Text);
            //Add missing openSYDE devices
            m_HandleMissingDevices(this->mc_OpenSydeDeviceConfigurations, c_DeviceInfos, c_Text);
            c_Text += QString(C_GtGetText::h_GetText("For error details see "));
            //Update log file
            C_OSCLoggingHandler::h_Flush();
            c_Text += QString("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>.").
                      arg(C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
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

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for reporting the progress of the STW flashloader configuration sequence

   \param[in]     ou32_Progress     Progress of sequence

   \created     08.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_UpdateProgressOfStwFlashloaderConfig(const uint32 ou32_Progress) const
{
   uint32 u32_ShowProgress;

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

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for reporting the progress of the openSYDE flashloader configuration sequence

   \param[in]     ou32_Progress     Progress of sequence

   \created     08.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_UpdateProgressOfOpenSydeConfig(const uint32 ou32_Progress) const
{
   uint32 u32_ShowProgress;

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

//-----------------------------------------------------------------------------
/*!
   \brief   Saves the state of a concrete step of openSYDE configuration sequence

   \param[in]     ou32_Step               Step of node configuration
                                          - hu32_SETNODEID
                                          - hu32_SETCANBITRATE
                                          - hu32_SETIPADDRESS
   \param[in]     os32_Result             Result of service
   \param[in]     ou8_BusIdentifier       Configured bus id of server interface
   \param[in]     ou8_NodeIdentifier      Configured node id of server interface

   \created     11.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_UpdateStateOfStwFlashloaderConfig(const uint32 ou32_Step, const sint32 os32_Result,
                                                        const uint8 ou8_BusIdentifier, const uint8 ou8_NodeIdentifier)
{
   // STW flashloader interface will be detected on an other position
   this->m_UpdateStateOfOpenSydeConfig(ou32_Step, os32_Result, ou8_BusIdentifier, ou8_NodeIdentifier, 0U, 0U);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Saves the state of a concrete step of openSYDE configuration sequence

   \param[in]     ou32_Step               Step of node configuration
                                          - hu32_SETNODEID
                                          - hu32_SETCANBITRATE
                                          - hu32_SETIPADDRESS
   \param[in]     os32_Result             Result of service
   \param[in]     ou8_BusIdentifier       Configured bus id of server interface
   \param[in]     ou8_NodeIdentifier      Configured node id of server interface
   \param[in]     ou8_InterfaceType       Interface type
                                          - 0 is CAN
                                          - 1 is Ethernet
   \param[in]     ou8_InterfaceNumber     Number of interface

   \created     11.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_UpdateStateOfOpenSydeConfig(const uint32 ou32_Step, const sint32 os32_Result,
                                                  const uint8 ou8_BusIdentifier, const uint8 ou8_NodeIdentifier,
                                                  const uint8 ou8_InterfaceType, const uint8 ou8_InterfaceNumber)
{
   const C_OSCProtocolDriverOsyNode c_Server(ou8_BusIdentifier, ou8_NodeIdentifier);

   QMap<stw_opensyde_core::C_OSCProtocolDriverOsyNode, std::vector<C_ServerConfStepResult> >::iterator c_ItServer;
   C_ServerConfStepResult c_Result(ou32_Step, ou8_InterfaceType, ou8_InterfaceNumber, os32_Result);

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

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the string value as result of the state of a step of a concrete server

   \param[in]     ou32_Step               Step of node configuration
                                          - hu32_SETNODEID
                                          - hu32_SETCANBITRATE
                                          - hu32_SETIPADDRESS
   \param[in]     orc_Server              Configured server on current used bus
   \param[in]     oe_InterfaceType        Interface type
   \param[in]     ou8_InterfaceNumber     Number of interface

   \return
   String for state result
   "TBD"    No state found for server and step
   "OK"     State for server found with no error
   "FAIL"   State for server found with registered state error

   \created     12.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SyvDcWidget::m_GetStateStringOfServerStep(const uint32 ou32_Step,
                                                    const C_OSCProtocolDriverOsyNode & orc_Server,
                                                    const C_OSCSystemBus::E_Type oe_InterfaceType,
                                                    const uint8 ou8_InterfaceNumber)
{
   // Default text if no concrete state is available
   QString c_Text = QString(C_GtGetText::h_GetText("<b>Skipped</b> (prev. failure)"));

   QMap<stw_opensyde_core::C_OSCProtocolDriverOsyNode, std::vector<C_ServerConfStepResult> >::const_iterator c_ItServer;

   c_ItServer = this->mc_ServerStates.find(orc_Server);

   if (c_ItServer != this->mc_ServerStates.end())
   {
      uint32 u32_StateCounter;
      const std::vector<C_ServerConfStepResult> & rc_Sates = c_ItServer.value();

      for (u32_StateCounter = 0U; u32_StateCounter < rc_Sates.size(); ++u32_StateCounter)
      {
         if ((rc_Sates[u32_StateCounter].u32_ConfStep == ou32_Step) &&
             (rc_Sates[u32_StateCounter].u8_InterfaceNumber == ou8_InterfaceNumber) &&
             (rc_Sates[u32_StateCounter].u8_InterfaceType == static_cast<uint8>(oe_InterfaceType)))
         {
            // State found
            if (rc_Sates[u32_StateCounter].s32_Result == C_NO_ERR)
            {
               c_Text = QString(C_GtGetText::h_GetText("OK"));
            }
            else
            {
               c_Text = "<b>" + QString(C_GtGetText::h_GetText("FAILED")) + "</b>";
            }
            break;
         }
      }
   }

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     25.10.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Cancel button

   \created     25.10.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize scan screen

   \created     11.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_InitScanScreen(void)
{
   QString c_Heading;
   sint32 s32_Return;

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
   c_Heading = QString(C_GtGetText::h_GetText("Topology Nodes (%1)")).arg(
      this->mpc_Ui->pc_ListWidgetExistingNodes->count());
   this->mpc_Ui->pc_LabelHeadingTopologyNodes->setText(c_Heading);
   this->mpc_Ui->pc_LabelHeadingFoundDevices->setText(QString(C_GtGetText::h_GetText("Connected Devices")));
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
      this->mpc_Ui->pc_LabelScanFoundDevicesWarning->setVisible(true);
      this->mpc_Ui->pc_LabelScanFoundDevicesWarning->setText(
         C_GtGetText::h_GetText("Warning: At least one node is connected to the bus with more than one interface!"
                                " Check interface configuration of the devices and retry."));

      this->mpc_Ui->pc_PushButtonScan->setEnabled(false);
   }
   else
   {
      this->mpc_Ui->pc_LabelScanFoundDevicesWarning->setVisible(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize assignment screen

   \created     11.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_InitAssignmentScreen(void) const
{
   QString c_Heading;

   this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->SetView(this->mu32_ViewIndex, true);
   c_Heading = QString(C_GtGetText::h_GetText("Topology Nodes (%1)")).arg(
      this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->count());
   this->mpc_Ui->pc_LabelHeadingTopologyNodesAssignment->setText(c_Heading);

   this->mpc_Ui->pc_ListWidgetConnectedNodesAssignment->SetData(this->mc_FoundDevices);
   c_Heading = QString(C_GtGetText::h_GetText("Connected Devices (%1)")).arg(this->mc_FoundDevices.size());
   this->mpc_Ui->pc_LabelHeadingFoundDevicesAssignment->setText(c_Heading);
   m_AssignmentUpdateProgress();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize bitrate combo box

   \created     11.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_InitBitRateComboBox(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   //Disconnect
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxBitRate, static_cast<void (QComboBox::*)(
                                                               sintn)>(&QComboBox::currentIndexChanged), this,
              &C_SyvDcWidget::m_OnBitRateChanged);

   // Bitrate
   this->mpc_Ui->pc_ComboBoxBitRate->clear();

   if (pc_View != NULL)
   {
      std::vector<uint32> c_ConnectedNodes;
      std::vector<uint32> c_ConnectedInterfaces;
      std::vector<uint32> c_SupportedBitrates;
      uint32 u32_BitrateCounter;
      uint32 u32_CurrentSetBitrate = pc_View->GetDeviceConfigSelectedBitRate();
      const QString c_CurrentSetBitrate = this->m_GetComboBoxString(u32_CurrentSetBitrate);
      bool q_CurrentSetBitrateFound = false;
      std::vector<uint8> c_ActiveNodes = pc_View->GetNodeActiveFlags();
      uint32 u32_ActiveNodeIndex;

      C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_BusIndex,
                                                                                         c_ConnectedNodes,
                                                                                         c_ConnectedInterfaces);

      // Remove the nodes, which are not active in this view
      for (u32_ActiveNodeIndex = 0U; u32_ActiveNodeIndex < c_ActiveNodes.size(); ++u32_ActiveNodeIndex)
      {
         if (c_ActiveNodes[u32_ActiveNodeIndex] == false)
         {
            std::vector<uint32>::iterator c_ItNode;
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
                                                            sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDcWidget::m_OnBitRateChanged);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize current configuration mode

   \created     09.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_InitModeComboBox(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   //Disconnect
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxConfigurationMode, static_cast<void (QComboBox::*)(
                                                                         sintn)>(&QComboBox::currentIndexChanged), this,
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
      }
   }

   //Reconnect
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxConfigurationMode, static_cast<void (QComboBox::*)(
                                                                      sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDcWidget::m_OnDeviceConfigModeChanged);
}

//-----------------------------------------------------------------------------
QString C_SyvDcWidget::m_GetComboBoxString(const uint32 ou32_Bitrate) const
{
   const QString c_Text = QString::number(ou32_Bitrate) + QString(" KBit/s");

   return c_Text;
}

//-----------------------------------------------------------------------------
uint32 C_SyvDcWidget::m_GetBitrateFromComboBoxString(const QString & orc_Entry) const
{
   QString c_Bitrate = orc_Entry;
   // In case of an error it returns 0
   const uint32 u32_Bitrate = c_Bitrate.remove(" KBit/s").toInt();

   return u32_Bitrate;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle bitrate change

   \created     13.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_OnBitRateChanged(void) const
{
   uint32 u32_BitRate;

   if (this->GetBitRateValue(u32_BitRate) == C_NO_ERR)
   {
      C_PuiSvHandler::h_GetInstance()->SetViewDeviceConfigSelectedBitRate(this->mu32_ViewIndex, u32_BitRate);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle device config mode change

   \created     09.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get selected bit rate

   \param[out] oru32_Value Bit rate

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid

   \created     13.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SyvDcWidget::GetBitRateValue(uint32 & oru32_Value) const
{
   sint32 s32_Retval = C_NO_ERR;

   oru32_Value = this->m_GetBitrateFromComboBoxString(this->mpc_Ui->pc_ComboBoxBitRate->currentText());

   if (oru32_Value == 0U)
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do assignment for specified node and serial number

   \param[in] ou32_NodeIndex   Node index
   \param[in] orc_SerialNumber Serial number

   \created     12.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_AssignmentConnect(const uint32 ou32_NodeIndex, const QString & orc_SerialNumber) const
{
   this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->ConnectSerialNumber(ou32_NodeIndex, orc_SerialNumber);
   this->mpc_Ui->pc_ListWidgetConnectedNodesAssignment->DisableSerialNumber(orc_SerialNumber);
   m_AssignmentUpdateProgress();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Disconnect assignment for specified node and serial number

   \param[in] ou32_NodeIndex   Node index
   \param[in] orc_SerialNumber Serial number

   \created     12.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_AssignmentDisconnect(const uint32 ou32_NodeIndex, const QString & orc_SerialNumber) const
{
   this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->DisconnectSerialNumber(ou32_NodeIndex, orc_SerialNumber);
   this->mpc_Ui->pc_ListWidgetConnectedNodesAssignment->EnableSerialNumber(orc_SerialNumber);
   m_AssignmentUpdateProgress();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update assignment progress

   \created     12.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_AssignmentUpdateProgress(void) const
{
   const uint32 u32_AssignedItemCount = this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->GetAssignmentCount();
   const uint32 u32_OverallItemCount = static_cast<uint32>(this->mpc_Ui->pc_ListWidgetExistingNodesAssignment->count());
   QString c_Text;

   if (u32_AssignedItemCount == u32_OverallItemCount)
   {
      c_Text = QString(C_GtGetText::h_GetText("All devices are assigned."));
      this->mpc_Ui->pc_PushButtonConfigure->setEnabled(true);
   }
   else
   {
      c_Text = QString(C_GtGetText::h_GetText("Assigned Devices %1 of %2.")).arg(u32_AssignedItemCount).arg(
         u32_OverallItemCount);
      this->mpc_Ui->pc_PushButtonConfigure->setEnabled(false);
   }
   this->mpc_Ui->pc_LabelAssignmentProgress->setText(c_Text);
}

//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_Timer(void)
{
   if (this->mpc_DcSequences != NULL)
   {
      sint32 s32_Result;
      sint32 s32_SequenceResult;

      std::vector<C_SyvDcDeviceInformation> c_DeviceInfo;

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
                     c_Message.SetDescription(C_GtGetText::h_GetText("Could not continue with step: reading information"
                                                                     " from STW Flashloader devices"));
                     m_CleanUpScan();
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
                     c_Message.SetDescription(C_GtGetText::h_GetText("Could not continue with step: reading information"
                                                                     " from openSYDE devices."));
                     m_CleanUpScan();
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
               c_Message.SetDescription(C_GtGetText::h_GetText("Could not get all devices into Flashloader."));
               m_CleanUpScan();
               c_Message.Execute();
            }
            break;
         case eSCANCANGETINFOFROMSTWFLASHLOADERDEVICES:
            if (s32_SequenceResult == C_NO_ERR)
            {
               this->mpc_DcSequences->GetDeviceInfosResult(c_DeviceInfo);

               this->mc_FoundDevices.reserve(this->mc_FoundDevices.size() + c_DeviceInfo.size());

               for (uint32 u32_SnCounter = 0U; u32_SnCounter < c_DeviceInfo.size(); ++u32_SnCounter)
               {
                  c_Text += "Device " + QString(c_DeviceInfo[u32_SnCounter].c_DeviceName.c_str()) + ": " +
                            QString(C_SyvDcDeviceInformation::h_SerialNumberToString(
                                       c_DeviceInfo[u32_SnCounter].au8_SerialNumber).c_str()) + "\n";
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
                     c_Message.SetDescription(C_GtGetText::h_GetText("Could not continue with step: reading information"
                                                                     " from openSYDE Devices"));
                     m_CleanUpScan();
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
               c_Message.Execute();
            }
            break;
         case eSCANCANGETINFOFROMOPENSYDEDEVICES: // Same data handling
         case eSCANETHGETINFOFROMOPENSYDEDEVICES: // Same data handling
            if (s32_SequenceResult == C_NO_ERR)
            {
               this->mpc_DcSequences->GetDeviceInfosResult(c_DeviceInfo);

               this->mc_FoundDevices.reserve(this->mc_FoundDevices.size() + c_DeviceInfo.size());

               for (uint32 u32_SnCounter = 0U; u32_SnCounter < c_DeviceInfo.size(); ++u32_SnCounter)
               {
                  c_Text += "Device " + QString(c_DeviceInfo[u32_SnCounter].c_DeviceName.c_str()) + ": " +
                            QString(C_SyvDcDeviceInformation::h_SerialNumberToString(c_DeviceInfo[u32_SnCounter].
                                                                                     au8_SerialNumber).c_str()) +
                            "\n";

                  this->mc_FoundDevices.push_back(c_DeviceInfo[u32_SnCounter]);
               }

               this->m_ScanFinished();
            }
            else
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
               c_Message.SetDescription(C_GtGetText::h_GetText("Failed while getting all information"
                                                               " from openSYDE Devices."));
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
                                                                             this->m_AreAllInterfacesToConfigure());

                  if (s32_Result == C_NO_ERR)
                  {
                     this->mc_Timer.start();
                  }
                  else
                  {
                     C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                     c_Message.SetDescription(C_GtGetText::h_GetText(
                                                 "Error occurred during openSYDE device configuration. "
                                                 "Check report for details."));
                     c_Message.Execute();
                  }
               }
               else
               {
                  // No openSYDE devices. Finished
                  // In the event queue could be some emitted signals from thread left.
                  // We need all information for showing the configuration result
                  QApplication::processEvents();
                  this->m_ShowConfigResult();
                  // Finish the process. No waiting for reset necessary.
                  this->m_ResetNetwork(this->mq_SameBitrates, true);
               }
            }
            else
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetDescription(C_GtGetText::h_GetText(
                                           "Error occurred during STW Flashloader device configuration. "
                                           "Check report for details."));
               c_Message.Execute();
            }
            break;
         case eCONFCANOPENSYDEDEVICES: // Same result
         case eCONFETHOPENSYDEDEVICES: // Same result
            if ((s32_SequenceResult == C_NO_ERR) ||
                (s32_SequenceResult == C_NOACT)) // No openSYDE devices to configure
            {
               // In the event queue could be some emitted signals from thread left.
               // We need all information for showing the configuration result
               QApplication::processEvents();
               this->m_ShowConfigResult();
               // Finish the process. No waiting for reset necessary.
               this->m_ResetNetwork(this->mq_SameBitrates, true);
            }
            else
            {
               const QString c_LogFilePath = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetDescription(C_GtGetText::h_GetText(
                                           "Error occurred during openSYDE device configuration. "
                                           "Check report for details."));
               c_Message.Execute();

               c_Text = "<br/><br/><br/>" + mhc_REPORT_HIGHLIGHT_TAG_START;
               c_Text +=  QString(C_GtGetText::h_GetText("Configuration Error!"));
               c_Text += mhc_REPORT_HIGHLIGHT_TAG_END + "<br/>" +
                         QString(C_GtGetText::h_GetText("See log file for details:")) +
                         "<br/>" + c_LogFilePath + "<br/>";
               this->m_UpdateReportText(c_Text);
            }
            break;
         case eREADBACKCAN: // Same implementation for showing the read info
         case eREADBACKETH: // Same implementation for showing the read info
            this->mpc_Ui->pc_PbBackToScan->setEnabled(true);
            this->m_ShowReadInfo(s32_SequenceResult);
            this->mpc_Ui->pc_BushButtonOk->setVisible(true);
            this->mpc_Ui->pc_BushButtonCancel->setVisible(false);
            this->mpc_Ui->pc_BopperleFinished->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);

            this->m_UpdateReportText(c_Text);

            //Discard disconnect necessary on close
            this->mq_DisconnectNecessary = false;

            // Start the applications
            this->m_ResetNetwork(true, false);

            //Notify user last?
            c_Text = "<br/><br/><br/>" + mhc_REPORT_HIGHLIGHT_TAG_START;
            c_Text +=  QString(C_GtGetText::h_GetText("Configuration finished!"));
            c_Text += mhc_REPORT_HIGHLIGHT_TAG_END + "<br/><br/>";
            this->m_UpdateReportText(c_Text);
            break;
         }
      }
   }
}

//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_ClearReportText(void)
{
   this->mc_ReportText = "";
   this->mpc_Ui->pc_TextBrowserReport->setText("");
}

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Handle configuration step start

   \created     16.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_HandleConfigurationStarted(void)
{
   QString c_Text;
   QString c_Temporary;

   c_Text = mhc_REPORT_HEADLINE_HTML_TAG_START;
   c_Text += QString(C_GtGetText::h_GetText("Device Configuration"));
   c_Text += mhc_REPORT_HEADLINE_HTML_TAG_END;
   c_Temporary = "<p>" + QString(C_GtGetText::h_GetText("Waiting for results ...")) + "</p>";

   this->m_UpdateReportText(c_Text, c_Temporary);
}

//-----------------------------------------------------------------------------
/*!
   \brief   handle device verification step start

   \created     16.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_HandleDeviceVerificationStart(void)
{
   QString c_Text;
   QString c_Temporary;

   c_Text = mhc_REPORT_HEADLINE_HTML_TAG_START;
   c_Text += QString(C_GtGetText::h_GetText("Device Type Verification"));
   c_Text += mhc_REPORT_HEADLINE_HTML_TAG_END;
   c_Temporary = "<p>" + QString(C_GtGetText::h_GetText("Waiting for results ...")) + "</p>";

   this->m_UpdateReportText(c_Text, c_Temporary);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle potentially missing device infos

   \param[in]     orc_AllDeviceInfos   All expected device infos
   \param[in]     orc_FoundDeviceInfos All received device infos
   \param[in,out] orc_ReportText       Report text to append to

   \created     16.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_HandleMissingDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_AllDeviceInfos,
                                           const std::vector<C_SyvDcDeviceInformation> & orc_FoundDeviceInfos,
                                           QString & orc_ReportText) const
{
   for (uint32 u32_ItExpected = 0; u32_ItExpected < orc_AllDeviceInfos.size(); ++u32_ItExpected)
   {
      bool q_Found = false;
      const C_SyvDcDeviceConfiguation & rc_ExpectedDevice = orc_AllDeviceInfos[u32_ItExpected];
      for (uint32 u32_ItFound = 0; u32_ItFound < orc_FoundDeviceInfos.size(); ++u32_ItFound)
      {
         const C_SyvDcDeviceInformation & rc_FoundDevice = orc_FoundDeviceInfos[u32_ItFound];
         const sintn sn_Match =
            std::memcmp(&rc_FoundDevice.au8_SerialNumber[0], &rc_ExpectedDevice.au8_SerialNumber[0], 6U);
         if (sn_Match == 0)
         {
            q_Found = true;
            break;
         }
      }
      if (q_Found == false)
      {
         orc_ReportText += "<b>" + QString(C_GtGetText::h_GetText("SN.: %1")).arg(
            QString(C_SyvDcDeviceInformation::h_SerialNumberToString(
                       rc_ExpectedDevice.au8_SerialNumber).c_str())) +
                           "</b><br/>";
      }
   }
}

//-----------------------------------------------------------------------------
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
      }
   }

   return q_SetAllInterfaces;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Disconnect from network if necessary

   Info: currently this function is unused because it has to be called in the onclose event
         which this class can not implement in all cases

   \created     29.09.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_DoCompleteDisconnect(void)
{
   if (mq_DisconnectNecessary == true)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      //Stop timer (don't continue process)
      this->mc_Timer.stop();
      if ((pc_View != NULL) && (pc_View->GetPcData().GetConnected() == true))
      {
         const C_OSCSystemBus * const pc_Bus =
            C_PuiSdHandler::h_GetInstance()->GetOSCBus(pc_View->GetPcData().GetBusIndex());
         if ((this->mpc_DcSequences != NULL) &&
             (pc_Bus != NULL))
         {
            bool q_Osy = false;
            bool q_Stw = false;
            bool q_StartedAnything = false;
            std::vector<uint32> c_NodeIndexes;
            std::vector<uint32> c_InterfaceIndexes;
            sint32 s32_Result;
            C_OgeWiCustomMessage c_Message(this);
            QApplication::setOverrideCursor(Qt::WaitCursor);

            //inform user whats happening
            c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
            c_Message.SetDescription(C_GtGetText::h_GetText("Disconnecting from system..."));
            c_Message.show();

            //Wait for the current thread to finish (if any)
            while (this->mpc_DcSequences->GetResults(s32_Result) == C_BUSY)
            {
               //In case it takes longer do process events to handle cursor and proper show of message box
               QApplication::processEvents(QEventLoop::AllEvents, 50);
            }

            //Check which nodes
            C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(
               pc_View->GetPcData().GetBusIndex(), c_NodeIndexes, c_InterfaceIndexes);
            for (uint32 u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
            {
               //Check if node active
               const std::vector<uint8> & rc_ActiveNodes = pc_View->GetNodeActiveFlags();
               if ((c_NodeIndexes[u32_ItNode] < rc_ActiveNodes.size()) && (rc_ActiveNodes[u32_ItNode] == true))
               {
                  const C_OSCNode * const pc_Node =
                     C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndexes[u32_ItNode]);
                  //Check which type of node
                  if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
                  {
                     if (pc_Bus->e_Type == C_OSCSystemBus::eCAN)
                     {
                        if (pc_Node->pc_DeviceDefinition->q_FlashloaderOpenSydeCan == true)
                        {
                           q_Osy = true;
                        }
                        if (pc_Node->pc_DeviceDefinition->q_FlashloaderStwCan == true)
                        {
                           q_Stw = true;
                        }
                     }
                     else
                     {
                        if (pc_Node->pc_DeviceDefinition->q_FlashloaderOpenSydeEthernet == true)
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
               while (this->mpc_DcSequences->ResetCanOpenSydeDevices() == C_BUSY)
               {
                  //In case it takes longer do process events to handle cursor and proper show of message box
                  QApplication::processEvents(QEventLoop::AllEvents, 50);
               }
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check if quit is currently allowed

   \return
   True  Exit allowed
   False Exit critical

   \created     07.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvDcWidget::m_CheckQuitPossible(void) const
{
   bool q_Retval;
   sint32 s32_Result;

   if ((this->mpc_DcSequences != NULL) &&
       (this->mpc_DcSequences->GetResults(s32_Result) == C_NO_ERR))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle higher level close ignored event

   \created     07.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Inform user why close request were ignored

   \created     07.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDcWidget::m_InformUserAboutAbortedClose(void) const
{
   C_OgeWiCustomMessage c_Message(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eINFORMATION);

   c_Message.SetHeading(C_GtGetText::h_GetText("Device configuration"));
   c_Message.SetDescription(C_GtGetText::h_GetText(
                               "While communication is running abort can lead to inconsistent system state"));
   c_Message.Execute();
}
