//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Creates a dialog to configure a Node with an Node ID and a certain Bitrate.
      The dialog holds logic that monitors the threads for the necessary sequences (enter flashloader,
      configure node, reset system) and starts/stops them in order. The report output of all
      sequences is forwarded to the MainWindow where the Report Widget is filled with information.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "C_FlaConNodeConfigPopup.hpp"
#include "ui_C_FlaConNodeConfigPopup.h"

#include "stwerrors.hpp"
#include "stwtypes.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_Uti.hpp"
#include "C_UsHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
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

   \param[in,out]  orc_Parent                      Reference to parent
   \param[in]      ou8_NodeId                      Node ID to be set in this dialog as default
   \param[in]      ou32_Bitrate                    Bitrate to be set in this dialog as default
   \param[in]      ou32_FlashloaderResetWaitTime   Wait time for flashloader reset from settings widget
   \param[in]      orc_CanDllPath                  CAN DLL file path
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaConNodeConfigPopup::C_FlaConNodeConfigPopup(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                 const uint8_t ou8_NodeId, const uint32_t ou32_Bitrate,
                                                 const uint32_t ou32_FlashloaderResetWaitTime,
                                                 const QString & orc_CanDllPath) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_FlaConNodeConfigPopup),
   mrc_ParentDialog(orc_Parent),
   mpc_DcSequences(NULL),
   ms32_SequenceResult(C_NO_ERR),
   mu8_CurrentNodeId(ou8_NodeId),
   mu32_CurrentBitrate(ou32_Bitrate),
   mu32_FlashloaderResetWaitTime(ou32_FlashloaderResetWaitTime),
   mc_CanDllPath(orc_CanDllPath)
{
   //we save the index zero based but want to display it based on 1.
   const QString c_CbxText = "CAN " + QString::number(C_UsHandler::h_GetInstance()->GetNodeCfgInterfaceIndex() + 1);

   this->mpc_Ui->setupUi(this);
   this->setFocusPolicy(Qt::StrongFocus); // necessary for getting ESC key press event while searching

   this->m_InitStaticNames();

   this->m_InitStaticGuiElements();

   this->mpc_Ui->pc_SpxNodeId->SetMaximumCustom(125);

   //init values
   this->mpc_Ui->pc_SpxNodeId->setValue(mu8_CurrentNodeId);
   this->mpc_Ui->pc_CbxBitrate->setCurrentText(QString::number(mu32_CurrentBitrate) + " kbit/s");
   this->mpc_Ui->pc_CbxInterface->setCurrentText(c_CbxText);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // Progress signals
   // We need a signal which is emitted in the other thread
   // We have to register the type and use the queued connections. Auto is default, but did not work.
   qRegisterMetaType<std::vector<stw::opensyde_core::C_OscDcDeviceInformation> >(
      "std::vector<stw::opensyde_core::C_OscDcDeviceInformation>");

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_FlaConNodeConfigPopup::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_FlaConNodeConfigPopup::m_OnCancel);

   //Timer
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_FlaConNodeConfigPopup::m_MonitorSequence);
   this->mc_Timer.setInterval(25);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaConNodeConfigPopup::~C_FlaConNodeConfigPopup(void) noexcept
{
   this->m_CleanUpDcSequence();
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node ID

   \retval   Node ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_FlaConNodeConfigPopup::GetNodeId() const
{
   const uint8_t u8_Retval = static_cast<uint8_t>(this->mpc_Ui->pc_SpxNodeId->value());

   return u8_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get bitrate from combobox

   \retval   bitrate in kbit/s
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_FlaConNodeConfigPopup::GetBitrate() const
{
   QString c_Text = this->mpc_Ui->pc_CbxBitrate->currentText();
   const uint32_t u32_Retval = static_cast<uint32_t>(c_Text.remove(" kbit/s").toInt());

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OkClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   // Handle escape key manually
   else if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Escape))
   {
      //prohibit close on ESC when process is running, allow otherwise
      if (this->mc_Timer.isActive())
      {
         q_CallOrg = false;
      }
      else
      {
         q_CallOrg = true;
      }
   }
   else
   {
      // no special handling for other buttons
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::m_InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Configure Node"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_LabelSettings->setText(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Apply"));

   this->mpc_Ui->pc_LabelInterface->setText(C_GtGetText::h_GetText("Interface"));
   this->mpc_Ui->pc_LabelNodeId->setText(C_GtGetText::h_GetText("New Node ID"));
   this->mpc_Ui->pc_LabelBitrate->setText(C_GtGetText::h_GetText("New Bitrate"));

   this->mpc_Ui->pc_LabelInterface->SetToolTipInformation(C_GtGetText::h_GetText("Interface"),
                                                          C_GtGetText::h_GetText(
                                                             "Interface the configuration shall be applied for. "
                                                             "\nThis is not"
                                                             " necessarily the interface the Node is currently connected to."));
   this->mpc_Ui->pc_LabelNodeId->SetToolTipInformation(C_GtGetText::h_GetText("New Node ID"),
                                                       C_GtGetText::h_GetText(
                                                          "Node ID which shall be applied to the selected interface "
                                                          "during node configuration process. "
                                                          "\nThe Node ID is used for addressing in the openSYDE "
                                                          "communication protocol."));
   this->mpc_Ui->pc_LabelBitrate->SetToolTipInformation(C_GtGetText::h_GetText("New Bitrate"),
                                                        C_GtGetText::h_GetText(
                                                           "CAN bitrate which shall be applied to the selected interface "
                                                           "during node configuration process."));

   this->mpc_Ui->pc_SpxNodeId->SetToolTipInformation(C_GtGetText::h_GetText("Value Range"),
                                                     C_GtGetText::h_GetText("Minimum: 0\n"
                                                                            "Maximum: 125"));
   //fill bitrate combobox
   this->mpc_Ui->pc_CbxBitrate->addItem("100 kbit/s");
   this->mpc_Ui->pc_CbxBitrate->addItem("125 kbit/s");
   this->mpc_Ui->pc_CbxBitrate->addItem("250 kbit/s");
   this->mpc_Ui->pc_CbxBitrate->addItem("500 kbit/s");
   this->mpc_Ui->pc_CbxBitrate->addItem("800 kbit/s");
   this->mpc_Ui->pc_CbxBitrate->addItem("1000 kbit/s");

   //fill interface combobox. We just want to show 6 CAN interfaces, since there probably no ECU with more
   for (uint8_t u8_IfCount = 1; u8_IfCount < 7; ++u8_IfCount)
   {
      this->mpc_Ui->pc_CbxInterface->addItem("CAN " + QString::number(u8_IfCount));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize color, fonts, etc.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::m_InitStaticGuiElements() const
{
   this->mpc_Ui->pc_LabelSettings->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelSettings->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelSettings->SetFontPixel(14);

   this->mpc_Ui->pc_LabelNodeId->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelNodeId->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelNodeId->SetFontPixel(12);

   this->mpc_Ui->pc_LabelBitrate->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelBitrate->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelBitrate->SetFontPixel(12);

   this->mpc_Ui->pc_LabelInterface->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelInterface->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelInterface->SetFontPixel(12);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize sequence for search node or for configure node

   \return
   C_NO_ERR    everything ok
   else        error occurred, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaConNodeConfigPopup::m_InitDcSequence(void)
{
   int32_t s32_Return;

   if (this->mpc_DcSequences == NULL)
   {
      this->mpc_DcSequences = new C_FlaSenDcBasicSequences();

      connect(this->mpc_DcSequences, &C_FlaSenDcBasicSequences::SigReportProgress, this,
              &C_FlaConNodeConfigPopup::SigReportProgress);
      connect(this->mpc_DcSequences, &stw::opensyde_gui_logic::C_FlaSenDcBasicSequences::SigReportDevicesInfoRead,
              this, &C_FlaConNodeConfigPopup::m_DeviceInfoRead);
   }

   s32_Return = this->mpc_DcSequences->InitDcSequences(this->mc_CanDllPath.toStdString(), this->mu32_CurrentBitrate);

   if (s32_Return != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Initialization failed"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Failed to initialize CAN interface."));
      c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("For details see ")) +
                           C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLESHEET_GUIDE_COLOR_LINK,
                                            C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()) + ".");
      C_OscLoggingHandler::h_Flush();
      c_Message.Execute();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Cleanup sequence for search node or for configure node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::m_CleanUpDcSequence(void)
{
   if (this->mpc_DcSequences != NULL)
   {
      disconnect(mpc_DcSequences, &C_FlaSenDcBasicSequences::SigReportProgress, this,
                 &C_FlaConNodeConfigPopup::SigReportProgress);
      disconnect(this->mpc_DcSequences, &stw::opensyde_gui_logic::C_FlaSenDcBasicSequences::SigReportDevicesInfoRead,
                 this, &C_FlaConNodeConfigPopup::m_DeviceInfoRead);
   }

   delete this->mpc_DcSequences;
   this->mpc_DcSequences = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::m_OkClicked(void)
{
   int32_t s32_Result;

   s32_Result = this->m_InitDcSequence();

   if (s32_Result == C_NO_ERR)
   {
      this->m_EnableUi(false);

      //activate waiting cursor
      QApplication::setOverrideCursor(Qt::WaitCursor);

      this->mpc_DcSequences->StartScanEnterFlashloader(mu32_FlashloaderResetWaitTime);
      this->mc_Timer.start();
   }
   else
   {
      //user got already informed via error message box from m_InitDcSequence
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::m_OnCancel()
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Monitors the sequence that is currently running (triggered by timeout of mc_Timer).

   Stops sequences and triggers certain actions according to current sequence.
   Sequences relevant here are:
      * SCAN_ENTER_FLASHLOADER
      * SCAN_GET_INFO
      * CONF_DEVICES
      * RESET
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::m_MonitorSequence()
{
   int32_t s32_ThreadResult = C_CONFIG;
   int32_t s32_SequenceResult = C_UNKNOWN_ERR;

   if (this->mpc_DcSequences != NULL)
   {
      s32_ThreadResult = this->mpc_DcSequences->GetResults(s32_SequenceResult);
   }

   if (s32_ThreadResult == C_NO_ERR)
   {
      // In the event queue could be some emitted signals from thread left.
      // We need all information for showing the result
      QApplication::processEvents();

      //what sequence are we currently looking at?
      switch (this->mpc_DcSequences->GetCurrentSequence())
      {
      case C_FlaSenDcBasicSequences::eSCAN_ENTER_FLASHLOADER:
         //stop FL sequence
         this->mc_Timer.stop();
         this->mpc_DcSequences->StartScanGetInfo();
         this->mc_Timer.start();
         break;
      case C_FlaSenDcBasicSequences::eSCAN_GET_INFO:
         //stop Scan sequence
         this->mc_Timer.stop();
         if (ms32_SequenceResult == C_NO_ERR)
         {
            const uint8_t u8_InterfaceIndex = static_cast<uint8_t>(this->mpc_Ui->pc_CbxInterface->currentIndex());

            //start the actual configuration sequence
            this->mpc_DcSequences->StartConfigureDevice(mu8_CurrentNodeId, this->GetNodeId(), this->GetBitrate(),
                                                        u8_InterfaceIndex);
            this->mc_Timer.start();
         }
         else
         {
            //more than one device was responding
            this->m_FinishConfig(ms32_SequenceResult);
         }
         break;
      case C_FlaSenDcBasicSequences::eCONF_DEVICES:
         //stop config sequence and finish it
         this->mc_Timer.stop();

         //tmp saving error code
         ms32_SequenceResult = s32_SequenceResult;
         this->m_FinishConfig(ms32_SequenceResult);
         break;
      case C_FlaSenDcBasicSequences::eRESET_SYSTEM:
         //stop reset sequence
         this->mc_Timer.stop();
         this->m_CleanUpDcSequence();
         //we close the dialog only if config successful
         if (ms32_SequenceResult == C_NO_ERR)
         {
            mrc_ParentDialog.accept();
         }
         else
         {
            //reset value for next try
            ms32_SequenceResult = C_NO_ERR;
         }
         break;
      //this sequence is irrelevant here
      case C_FlaSenDcBasicSequences::eNOT_ACTIVE:
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gives user feedback when Node Configuration is finished (fail or success) and initiates a
            system reset either way

   \param[in]   os32_SequenceResult   Error code from Node Configuration sequence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::m_FinishConfig(const int32_t os32_SequenceResult)
{
   bool q_ResetSystem = false;

   //deactivate waiting cursor
   QApplication::restoreOverrideCursor();

   if (os32_SequenceResult != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Configure Node"));
      c_Message.SetDescription(C_GtGetText::h_GetText(m_GetErrorAsString(os32_SequenceResult).toStdString().c_str()));
      c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("For details see ")) +
                           C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLESHEET_GUIDE_COLOR_LINK,
                                            C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()) + ".");
      C_OscLoggingHandler::h_Flush();

      //enable buttons again for retry
      if (c_Message.exec() == static_cast<int32_t>(C_OgeWiCustomMessage::Accepted))
      {
         this->m_EnableUi(true);
         q_ResetSystem = true;
      }
   }
   else
   {
      const uint8_t u8_NodeId = this->GetNodeId();
      const uint32_t u32_Bitrate = this->GetBitrate();
      C_OgeWiCustomMessage c_Message(this);
      // no more steps to do
      c_Message.SetHeading(C_GtGetText::h_GetText("Node Configuration successful"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Successfully configured node interface \"") +
                               this->mpc_Ui->pc_CbxInterface->currentText() + "\":\nNew Node ID = " +
                               QString::number(u8_NodeId) +
                               C_GtGetText::h_GetText("\nNew Bitrate = ") +
                               QString::number(u32_Bitrate) + " kbit/s");

      C_UsHandler::h_GetInstance()->SetNodeCfgInterfaceIndex(this->mpc_Ui->pc_CbxInterface->currentIndex());

      //confirming message box closes node config dialog
      if (c_Message.exec() == static_cast<int32_t>(C_OgeWiCustomMessage::Accepted))
      {
         q_ResetSystem = true;
      }
   }

   //we always make a reset
   if (q_ResetSystem == true)
   {
      this->mpc_DcSequences->StartResetSystem();
      this->mc_Timer.start();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Connected to signal in openSYDE Core that sends all the information.

   Looks how many devices answer with the Node ID about to be configured and logs an error if > 1.

   \param[in]       orc_DeviceInfoResult    device information for every found node
   \param[in]       oq_SecurityFeatureUsed   security used for at least one node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::m_DeviceInfoRead(
   const std::vector<stw::opensyde_core::C_OscDcDeviceInformation> & orc_DeviceInfoResult,
   const bool oq_SecurityFeatureUsed)
{
   //SYDEflash is not able to cope with Security mechanisms and we don't want to give additional user feedback if a Node
   // has Security activated. Configure Node will run into an error that is handled gracefully and reported to log file
   Q_UNUSED(oq_SecurityFeatureUsed)

   uint8_t u8_FoundNodeCounter = 0;

   for (uint32_t u32_It = 0; u32_It < orc_DeviceInfoResult.size(); ++u32_It)
   {
      if (orc_DeviceInfoResult[u32_It].u8_NodeId == this->mu8_CurrentNodeId)
      {
         u8_FoundNodeCounter++;
      }

      if (u8_FoundNodeCounter > 1)
      {
         ms32_SequenceResult = C_RANGE;
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper function to en/disable the UI when process is running or not.

   \param[in]       oq_Enabled   true: UI will be enabled, false: will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaConNodeConfigPopup::m_EnableUi(const bool oq_Enabled)
{
   this->mpc_Ui->pc_PushButtonOk->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_PushButtonCancel->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_CbxInterface->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_CbxBitrate->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_SpxNodeId->setEnabled(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Composes an error message based on input

   \param[in]       os32_SequenceResult   Error code

   \return
   error message
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_FlaConNodeConfigPopup::m_GetErrorAsString(const int32_t os32_SequenceResult)
{
   QString c_Retval;

   if (os32_SequenceResult == C_RANGE)
   {
      c_Retval = "More than one node answered with the current Node ID " + QString::number(mu8_CurrentNodeId);
   }
   else if (os32_SequenceResult == C_WARN)
   {
      c_Retval = "The requested CAN Interface does not exist or is out of range for this Node.\n";
   }
   else if (os32_SequenceResult == C_TIMEOUT)
   {
      c_Retval = "No node found for Node ID " + QString::number(mu8_CurrentNodeId);
   }
   else
   {
      c_Retval = "Failure when configuring node. Sequence result: " + QString::number(os32_SequenceResult);
   }

   return c_Retval;
}
