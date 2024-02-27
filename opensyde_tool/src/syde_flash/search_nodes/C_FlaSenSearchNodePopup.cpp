//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief

   This popup is used to scan the available nodes and to allow selection of the node to be updated.
   It fetches the device information, mainly the Node ID in this case which is mandatory for the
   node update.
   Once the node is selected, the Node ID is fetched in the MainWindow and updated in the Report Widget.


   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "C_OscLoggingHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_Uti.hpp"

#include "C_FlaSenSearchNodePopup.hpp"
#include "ui_C_FlaSenSearchNodePopup.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent    Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaSenSearchNodePopup::C_FlaSenSearchNodePopup(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_FlaSenSearchNodePopup),
   mrc_ParentDialog(orc_Parent),
   mpc_DcSequences(NULL)
{
   this->mpc_Ui->setupUi(this);
   this->setFocusPolicy(Qt::StrongFocus); // necessary for getting ESC key press event while searching

   InitStaticNames();

   this->mpc_Ui->pc_ListWidgetConnectedNodes->SetDragDropMode(false);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_FlaSenSearchNodePopup::m_ApplyClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_FlaSenSearchNodePopup::m_OnCancel);
   connect(this->mpc_Ui->pc_ListWidgetConnectedNodes, &C_SyvDcConnectedNodeList::doubleClicked, this,
           &C_FlaSenSearchNodePopup::m_ApplyClicked);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_FlaSenSearchNodePopup::m_Timer);

   this->mc_Timer.setInterval(25);

   // Progress signals: We need a signal which is emitted in the other thread.
   // We have to register the type and use the queued connections. Auto is default, but did not work.
   // Issue with the signal  "SigReportDevicesInfoRead" emitted from C_FlaSenDcBasicSequences::m_ReportDevicesInfoRead.
   // Following message appears if the meta type is not registered:
   // "QObject::connect: Cannot queue arguments of type 'std::vector<stw::opensyde_core::C_OscDcDeviceInformation>'
   // (Make sure 'std::vector<stw::opensyde_core::C_OscDcDeviceInformation>' is registered using qRegisterMetaType().)"

   qRegisterMetaType<std::vector<stw::opensyde_core::C_OscDcDeviceInformation> >(
      "std::vector<stw::opensyde_core::C_OscDcDeviceInformation>");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaSenSearchNodePopup::~C_FlaSenSearchNodePopup(void)
{
   this->m_CleanupDcSequence();
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenSearchNodePopup::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Search Node"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Found Nodes"));
   this->mpc_Ui->pc_LabelScanningNodes->setText(C_GtGetText::h_GetText("Scanning for nodes..."));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Apply"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize sequence and start search if successful

   \param[in]  orc_CanDllPath                   Path to CAN DLL file
   \param[in]  os32_CanBitrate                  CAN Bitrate in kBit/s
   \param[in]  ou32_FlashloaderResetWaitTime    Flashloader reset wait time

   \return
   C_NO_ERR    everything ok
   C_COM       could not initialize sequence, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSenSearchNodePopup::StartSearch(const QString & orc_CanDllPath, const int32_t os32_CanBitrate,
                                             const uint32_t ou32_FlashloaderResetWaitTime)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->m_InitDcSequence(orc_CanDllPath, os32_CanBitrate) == C_NO_ERR)
   {
      this->m_ScanNodes(ou32_FlashloaderResetWaitTime);
   }
   else
   {
      s32_Return = C_COM;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenSearchNodePopup::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         this->m_ApplyClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   // Handle escape key manually: do not close on escape if currently searching and make sure to reset else
   else if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Escape))
   {
      q_CallOrg = false;
      if (this->mpc_Ui->pc_LabelScanningNodes->isVisible() == false)
      {
         this->m_OnCancel();
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
/*! \brief  Initialize sequence for search node

   \param[in]  orc_CanDllPath    Path to CAN DLL file
   \param[in]  os32_CanBitrate   CAN Bitrate in kBit/s

   \return
   C_NO_ERR    everything ok
   else        error occurred, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSenSearchNodePopup::m_InitDcSequence(const QString & orc_CanDllPath, const int32_t os32_CanBitrate)
{
   int32_t s32_Return;

   if (this->mpc_DcSequences == NULL)
   {
      this->mpc_DcSequences = new C_FlaSenDcBasicSequences();

      connect(mpc_DcSequences, &C_FlaSenDcBasicSequences::SigReportProgress,
              this, &C_FlaSenSearchNodePopup::SigReportProgress);
      connect(this->mpc_DcSequences, &stw::opensyde_gui_logic::C_FlaSenDcBasicSequences::SigReportDevicesInfoRead,
              this, &C_FlaSenSearchNodePopup::m_DeviceInfoReceived);
   }

   s32_Return = this->mpc_DcSequences->Init(orc_CanDllPath.toStdString(), os32_CanBitrate);

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
/*! \brief  Cleanup sequence for search node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenSearchNodePopup::m_CleanupDcSequence(void)
{
   if (this->mpc_DcSequences != NULL)
   {
      disconnect(mpc_DcSequences, &C_FlaSenDcBasicSequences::SigReportProgress,
                 this, &C_FlaSenSearchNodePopup::SigReportProgress);
      disconnect(this->mpc_DcSequences, &stw::opensyde_gui_logic::C_FlaSenDcBasicSequences::SigReportDevicesInfoRead,
                 this, &C_FlaSenSearchNodePopup::m_DeviceInfoReceived);
   }

   delete this->mpc_DcSequences;
   this->mpc_DcSequences = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Apply button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenSearchNodePopup::m_ApplyClicked(void)
{
   const int32_t s32_CurrentItem = this->mpc_Ui->pc_ListWidgetConnectedNodes->selectedItems().count();

   if (s32_CurrentItem > 0)
   {
      this->mpc_DcSequences->ResetSystem();

      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenSearchNodePopup::m_OnCancel(void)
{
   this->mpc_DcSequences->ResetSystem();

   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Scan nodes

   \param[in]  ou32_FlashloaderResetWaitTime    Flashloader reset wait time
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenSearchNodePopup::m_ScanNodes(const uint32_t ou32_FlashloaderResetWaitTime)
{
   this->mpc_Ui->pc_ListWidgetConnectedNodes->setVisible(false);

   // Disable user interaction
   this->mpc_Ui->pc_PushButtonOk->setEnabled(false);
   this->mpc_Ui->pc_PushButtonCancel->setEnabled(false);

   // Start scan
   if ((this->mpc_DcSequences->StartScanEnterFlashloader(ou32_FlashloaderResetWaitTime)) == C_NO_ERR)
   {
      this->mc_Timer.start();
   }
   else
   {
      tgl_assert(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports device information read from found devices

   Called by StartScanGetInfo() after it has scanned information from connected devices

   \param[in]  orc_DeviceInfoResult    Device information results
   \param[in]  oq_SecurityFeatureUsed  Security feature used for at least one node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenSearchNodePopup::m_DeviceInfoReceived(
   const std::vector<stw::opensyde_core::C_OscDcDeviceInformation> & orc_DeviceInfoResult,
   const bool oq_SecurityFeatureUsed)
{
   //SYDEflash is not able to cope with Security mechanisms and we don't want to give additional user feedback if a Node
   // has Security activated. In the Search Node context there is no functional restriction anyway.
   Q_UNUSED(oq_SecurityFeatureUsed)

   if (orc_DeviceInfoResult.size() == 0)
   {
      this->m_ShowErrorNoDevices();
      this->m_OnCancel();
   }
   else
   {
      const QString c_NodeCountText = C_GtGetText::h_GetText("Found Nodes (") + QString::number(
         orc_DeviceInfoResult.size()) + C_GtGetText::h_GetText(")");

      this->mpc_Ui->pc_LabelHeadingPreview->setText(c_NodeCountText);

      this->mpc_Ui->pc_LabelScanningNodes->setVisible(false);
      this->mpc_Ui->pc_ListWidgetConnectedNodes->setVisible(true);

      this->mpc_Ui->pc_ListWidgetConnectedNodes->SetData(orc_DeviceInfoResult);

      // this function should be called after "SetData" because in "SetData" an initial
      // width is set to the list item which needs to be modified here to fit the dialog.
      this->mpc_Ui->pc_ListWidgetConnectedNodes->SetListItemWidth(this->mpc_Ui->pc_ListWidgetConnectedNodes->width());

      // select first row by default
      this->mpc_Ui->pc_ListWidgetConnectedNodes->SelectRow(0);
   }

   // Enable user interaction
   this->mpc_Ui->pc_PushButtonOk->setEnabled(true);
   this->mpc_Ui->pc_PushButtonCancel->setEnabled(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Timer function connected on timeout
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenSearchNodePopup::m_Timer(void)
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

      // Thread finished
      this->mc_Timer.stop();
      s32_ThreadResult = this->mpc_DcSequences->StartScanGetInfo();

      // very unlikely that thread start failed (C_BUSY)
      tgl_assert(s32_ThreadResult == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Displays an error message popup when no devices are found

*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenSearchNodePopup::m_ShowErrorNoDevices(void)
{
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);

   // Show message
   c_Message.SetHeading(C_GtGetText::h_GetText("Device Configuration"));
   c_Message.SetDescription(C_GtGetText::h_GetText("No devices found! Check connection of connected devices and retry."));
   c_Message.Execute();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fetch the selected node id

   \return
   Node Id
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_FlaSenSearchNodePopup::GetSelectedNodeId(void) const
{
   return this->mpc_Ui->pc_ListWidgetConnectedNodes->GetSelectedNodeId();
}
