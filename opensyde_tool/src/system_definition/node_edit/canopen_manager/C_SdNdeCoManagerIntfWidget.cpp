//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with current CANopen settings of Manager for a specific interface

   Widget with current CANopen settings of Manager for a specific interface

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdNdeCoManagerIntfWidget.hpp"
#include "ui_C_SdNdeCoManagerIntfWidget.h"

#include "stwerrors.hpp"
#include "constants.hpp"
#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscCanOpenManagerInfo.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_elements;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

const uint8_t mu8_NMT_ERROR_RESTART_ALL_DEVICES = 0;
const uint8_t mu8_NMT_ERROR_RESTART_FAILURE_DEVICE = 1;
const uint8_t mu8_NMT_ERROR_STOP_ALL_DEVICES = 2;
/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoManagerIntfWidget::C_SdNdeCoManagerIntfWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoManagerIntfWidget),
   mu32_NodeIndex(0),
   mu8_InterfaceId(0)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // set producer time min/max
   this->mpc_Ui->pc_SpinBoxProducerTime->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxProducerTime->SetMaximumCustom(65535);

   // set SDO timeout min/max
   this->mpc_Ui->pc_SpinBoxSDOTimeout->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxSDOTimeout->SetMaximumCustom(65535);

   // set SYNC times min/max
   this->mpc_Ui->pc_SpinBoxSYNCPeriod->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxSYNCPeriod->SetMaximumCustom(0x7FFFFFFF);

   this->mpc_Ui->pc_SpinBoxSYNCWindowLength->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxSYNCWindowLength->SetMaximumCustom(0x7FFFFFFF);

   //disable items per default
   this->mpc_Ui->pc_CheckBoxNMTStartAll->setEnabled(false);
   this->mpc_Ui->pc_SpinBoxProducerTime->setEnabled(false);
   this->mpc_Ui->pc_SpinBoxSYNCPeriod->setEnabled(false);
   this->mpc_Ui->pc_SpinBoxSYNCWindowLength->setEnabled(false);

   // connects
   // Can be connected here due to no changes on real data. Only reconfiguration of ui elements
   connect(this->mpc_Ui->pc_CheckBoxStartDevices, &QCheckBox::toggled, this,
           &C_SdNdeCoManagerIntfWidget::m_OnStartDevicesChanged);
   connect(this->mpc_Ui->pc_CheckBoxNMTStartAll, &QCheckBox::toggled, this,
           &C_SdNdeCoManagerIntfWidget::m_OnStartDevicesChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoManagerIntfWidget::~C_SdNdeCoManagerIntfWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelCanOpenIdCaption->setText(C_GtGetText::h_GetText("CANopen Manager Node ID"));

   this->mpc_Ui->pc_LabelCanOpenId->setText(C_GtGetText::h_GetText("CANopen Node ID"));
   this->mpc_Ui->pc_LabelCanOpenId->
   SetToolTipInformation(C_GtGetText::h_GetText("CANopen Node ID"),
                         C_GtGetText::h_GetText("Unique Node ID within a CANopen network."));

   this->mpc_Ui->pc_CheckBoxSameAsOpensyde->setText(C_GtGetText::h_GetText("Use openSYDE Node ID"));
   this->mpc_Ui->pc_CheckBoxSameAsOpensyde->
   SetToolTipInformation(C_GtGetText::h_GetText("Use openSYDE Node ID"),
                         C_GtGetText::h_GetText("If enabled, openSYDE Node ID is used as CANopen Node ID. \n"
                                                "openSYDE Node ID is defined in \"Properties Tab\"."));

   this->mpc_Ui->pc_LabelSettingsCaption->setText(C_GtGetText::h_GetText("Settings"));

   this->mpc_Ui->pc_CheckBoxAutostart->setText(C_GtGetText::h_GetText("Autostart CANopen Manager"));
   this->mpc_Ui->pc_CheckBoxAutostart->
   SetToolTipInformation(C_GtGetText::h_GetText("Autostart CANopen Manager"),
                         C_GtGetText::h_GetText(
                            "If enabled, the CANopen Manager goes automatically to operational mode on startup when"
                            " all mandatory CANopen Devices are ready.\n"
                            "If disabled, the CANopen Manager must be started by the application."));

   this->mpc_Ui->pc_CheckBoxStartDevices->setText(C_GtGetText::h_GetText("Start CANopen Devices"));
   this->mpc_Ui->pc_CheckBoxStartDevices->
   SetToolTipInformation(C_GtGetText::h_GetText("Start CANopen Devices"),
                         C_GtGetText::h_GetText(
                            "If enabled, the CANopen Manager starts the CANopen Devices on startup.\n"
                            "If disabled, the CANopen Devices must be started by the application."));

   this->mpc_Ui->pc_CheckBoxNMTStartAll->setText(C_GtGetText::h_GetText("Use \"NMT Start All\" Command"));
   this->mpc_Ui->pc_CheckBoxNMTStartAll->
   SetToolTipInformation(C_GtGetText::h_GetText("Use \"NMT Start All\" Command"),
                         C_GtGetText::h_GetText(
                            "If enabled, the CANopen Manager starts all CANopen Devices with an "
                            "\"NMT Start All\" command.\n"
                            "\"NMT Start All\" command can only be used if all optional CANopen Devices are ready. "
                            "Otherwise the CANopen Manager starts every CANopen Device separately."));

   this->mpc_Ui->pc_LabelNMTErrorBehaviour->setText(C_GtGetText::h_GetText("NMT Error Behavior"));
   this->mpc_Ui->pc_LabelNMTErrorBehaviour->SetToolTipInformation(C_GtGetText::h_GetText("NMT Error Behaviour"),
                                                                  C_GtGetText::h_GetText(
                                                                     "\"Restart all CANopen Devices\": CANopen Manager restarts all CANopen Devices automatically "
                                                                     "(NMT Reset + SDO Configuration + NMT Start).\n"
                                                                     "\"Restart failure CANopen Device\": CANopen Manager restarts the failure CANopen Device automatically "
                                                                     "(NMT Reset + SDO Configuration + NMT Start).\n"
                                                                     "\"Stop all CANopen Device\": CANopen Manager stops all CANopen Devices. "
                                                                     "The CANopen Devices must be resetted by the application."));
   this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->addItem(C_GtGetText::h_GetText("Restart all CANopen Devices"));
   this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->addItem(C_GtGetText::h_GetText("Restart failure CANopen Device"));
   this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->addItem(C_GtGetText::h_GetText("Stop all CANopen Devices"));

   this->mpc_Ui->pc_LabelSDOTimeout->setText(C_GtGetText::h_GetText("SDO Timeout"));
   this->mpc_Ui->pc_LabelSDOTimeout->
   SetToolTipInformation(C_GtGetText::h_GetText("SDO Timeout"),
                         C_GtGetText::h_GetText("CANopen Manager expects a response from CANopen Device within this "
                                                "time after sending an SDO command."));

   this->mpc_Ui->pc_SpinBoxSDOTimeout->setSuffix(C_GtGetText::h_GetText(" ms"));

   this->mpc_Ui->pc_LabelHeartbeatCaption->setText(C_GtGetText::h_GetText("Heartbeat"));

   this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setText(C_GtGetText::h_GetText("Enable Heartbeat Producing"));
   this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->
   SetToolTipInformation(C_GtGetText::h_GetText("Enable Heartbeat Producing"),
                         C_GtGetText::h_GetText(
                            "If enabled, the CANopen Manager sends heartbeats according to defined "
                            "\"Producer Time\" interval.\n"
                            "When disabled, heartbeat consuming option is disabled for all CANopen Devices."));

   this->mpc_Ui->pc_LabelProducerTime->setText(C_GtGetText::h_GetText("Producer Time"));
   this->mpc_Ui->pc_LabelProducerTime->
   SetToolTipInformation(C_GtGetText::h_GetText("Producer Time"),
                         C_GtGetText::h_GetText("Heartbeat interval produced by the CANopen Manager."));

   this->mpc_Ui->pc_SpinBoxProducerTime->setSuffix(C_GtGetText::h_GetText(" ms"));

   this->mpc_Ui->pc_LabelSYNCPdoCaption->setText(C_GtGetText::h_GetText("SYNC PDO"));

   this->mpc_Ui->pc_CheckBoxSYNCProduceEnabled->setText(C_GtGetText::h_GetText("Produce SYNC"));
   this->mpc_Ui->pc_CheckBoxSYNCProduceEnabled->
   SetToolTipInformation(C_GtGetText::h_GetText("Produce SYNC"),
                         C_GtGetText::h_GetText(
                            "If enabled, the CANopen Manager sends the SYNC message with a fixed COB-ID 0x80 according "
                            "to the defined \"Cycle Period\" interval.\nAll CANopen Devices get "
                            "the \"Window Length\" time distributed to Object 0x1007.\n"
                            "When disabled, the PDO SYNC will not be sent and the CANopen Devices will not get the"
                            "\"Window Length\" time configuration distributed."));

   this->mpc_Ui->pc_LabelSYNCPeriod->setText(C_GtGetText::h_GetText("Cycle Period"));
   this->mpc_Ui->pc_LabelSYNCPeriod->
   SetToolTipInformation(C_GtGetText::h_GetText("Cycle Period"),
                         C_GtGetText::h_GetText(
                            "SYNC PDO produced with the interval of \"Cycle Period\" by the CANopen Manager."));

   this->mpc_Ui->pc_SpinBoxSYNCPeriod->setSuffix(C_GtGetText::h_GetText(" us"));

   this->mpc_Ui->pc_LabelSYNCWindowLength->setText(C_GtGetText::h_GetText("Window Length"));
   this->mpc_Ui->pc_LabelSYNCWindowLength->
   SetToolTipInformation(C_GtGetText::h_GetText("Window Length"),
                         C_GtGetText::h_GetText(
                            "SYNC PDO \"Window Length\" will be distributed by CANopen Manager"
                            " to Object 0x1007 of all CANopen Devices.\n"
                            "Will not be used by CANopen Manager for incoming PDOs."));

   this->mpc_Ui->pc_SpinBoxSYNCWindowLength->setSuffix(C_GtGetText::h_GetText(" us"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Node Index and Inteface ID setter

   Sets the private node index and interface id

   \param[in]  ou32_NodeIndex     new node index
   \param[in]  ou8_InterfaceId    new interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::SetNodeIndexAndInterfaceId(const uint32_t ou32_NodeIndex,
                                                            const uint8_t ou8_InterfaceId)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu8_InterfaceId = ou8_InterfaceId;

   //load node data
   this->m_LoadFromData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save ui data to node

   Is called from outside
      - on system definition save
      - on page change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::SaveToData(void) const
{
   C_OscCanOpenManagerInfo::E_NmtErrorBehaviourType e_NmtErrorBehaviour = C_OscCanOpenManagerInfo::eSTOP_ALL_DEVICES;

   if (this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->currentIndex() == mu8_NMT_ERROR_RESTART_ALL_DEVICES)
   {
      e_NmtErrorBehaviour = C_OscCanOpenManagerInfo::eRESTART_ALL_DEVICES;
   }
   else if (this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->currentIndex() == mu8_NMT_ERROR_STOP_ALL_DEVICES)
   {
      e_NmtErrorBehaviour = C_OscCanOpenManagerInfo::eSTOP_ALL_DEVICES;
   }
   else if (this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->currentIndex() == mu8_NMT_ERROR_RESTART_FAILURE_DEVICE)
   {
      e_NmtErrorBehaviour = C_OscCanOpenManagerInfo::eRESTART_FAILURE_DEVICE;
   }
   else
   {
      //unknown state
      tgl_assert(false);
   }

   C_PuiSdHandler::h_GetInstance()->SetCanOpenManagerCommonProperties(
      this->mu32_NodeIndex,
      this->mu8_InterfaceId,
      this->mpc_Ui->pc_CheckBoxSameAsOpensyde->isChecked(),
      static_cast<uint8_t>(this->mpc_Ui->pc_SpinBoxCanOpenId->value()),
      static_cast<uint16_t>(this->mpc_Ui->pc_SpinBoxSDOTimeout->value()),
      this->mpc_Ui->pc_CheckBoxAutostart->isChecked(),
      this->mpc_Ui->pc_CheckBoxStartDevices->isChecked(),
      this->mpc_Ui->pc_CheckBoxNMTStartAll->isChecked(),
      e_NmtErrorBehaviour);
   C_PuiSdHandler::h_GetInstance()->SetCanOpenManagerProducerHeartbeat(
      this->mu32_NodeIndex,
      this->mu8_InterfaceId,
      static_cast<uint16_t>(this->mpc_Ui->pc_SpinBoxProducerTime->value()),
      this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->isChecked());

   // SYNC PDO
   C_PuiSdHandler::h_GetInstance()->SetCanOpenManagerPdoSync(
      this->mu32_NodeIndex,
      this->mu8_InterfaceId,
      static_cast<uint32_t>(this->mpc_Ui->pc_SpinBoxSYNCPeriod->value()),
      static_cast<uint32_t>(this->mpc_Ui->pc_SpinBoxSYNCWindowLength->value()),
      this->mpc_Ui->pc_CheckBoxSYNCProduceEnabled->isChecked());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Refresh config page data

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::Refresh()
{
   this->m_LoadFromData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxStartDevices state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_OnStartDevicesChanged(void) const
{
   if (this->mpc_Ui->pc_CheckBoxStartDevices->isChecked() == true)
   {
      this->mpc_Ui->pc_CheckBoxNMTStartAll->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_CheckBoxNMTStartAll->setChecked(false);
      this->mpc_Ui->pc_CheckBoxNMTStartAll->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxEnableHeartbeatProducing state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_OnHeartbeatEnableChanged(void) const
{
   this->m_HandleHeartbeatEnableState();
   this->m_OnHeartbeatProducerTimeChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxEnableHeartbeatProducing state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_HandleHeartbeatEnableState(void) const
{
   if (this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->isChecked() == true)
   {
      this->mpc_Ui->pc_SpinBoxProducerTime->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxProducerTime->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxEnableHeartbeatProducing state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_OnPdoSyncEnableChanged(void) const
{
   this->m_HandlePdoSyncEnableState();
   this->m_OnPdoSyncTimeValueChanged();

   // Update COMM Datapools due to own error check in Datapool view
   Q_EMIT (this->SigCommDatapoolsChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxEnableHeartbeatProducing state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_HandlePdoSyncEnableState(void) const
{
   const bool q_Enabled = (this->mpc_Ui->pc_CheckBoxSYNCProduceEnabled->isChecked() == true);

   this->mpc_Ui->pc_SpinBoxSYNCPeriod->setEnabled(q_Enabled);
   this->mpc_Ui->pc_SpinBoxSYNCWindowLength->setEnabled(q_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle SYNC PDO cycle period or window length time change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_OnPdoSyncTimeValueChanged(void) const
{
   this->m_SaveChanges();

   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxSameAsOpensyde state changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_OnSameAsOpensydeNodeIdChanged(void) const
{
   // Avoid unnecessary duplicated calls of m_OnCoNodeIdChanged
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxCanOpenId, static_cast<void (QSpinBox::*)(
                                                                int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_OnCoNodeIdChanged);

   this->m_HandleSameAsOpensydeNodeIdState();

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxCanOpenId, static_cast<void (QSpinBox::*)(
                                                             int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_OnCoNodeIdChanged);

   this->m_OnCoNodeIdChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle the pc_CheckBoxSameAsOpensyde state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_HandleSameAsOpensydeNodeIdState(void) const
{
   m_HandleNodeIdRange(this->mpc_Ui->pc_CheckBoxSameAsOpensyde->isChecked());
   if (this->mpc_Ui->pc_CheckBoxSameAsOpensyde->isChecked() == true)
   {
      //get current node ID of node interface
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNode(this->mu32_NodeIndex);
      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         const C_OscNodeComInterfaceSettings * const pc_ComInterface = pc_Node->c_Properties.GetComInterface(
            C_OscSystemBus::eCAN, this->mu8_InterfaceId);
         tgl_assert(pc_ComInterface != NULL);
         if (pc_ComInterface != NULL)
         {
            this->mpc_Ui->pc_SpinBoxCanOpenId->setValue(static_cast<int32_t>(pc_ComInterface->u8_NodeId));
         }
      }

      this->mpc_Ui->pc_SpinBoxCanOpenId->setEnabled(false);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxCanOpenId->setEnabled(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle node id range

   \param[in]  oq_IsOpensydeNodeId  Is opensyde node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_HandleNodeIdRange(const bool oq_IsOpensydeNodeId) const
{
   if (oq_IsOpensydeNodeId == true)
   {
      // set openSYDE node id min/max
      // necessary to be compatible with complete openSYDE Node ID range
      this->mpc_Ui->pc_SpinBoxCanOpenId->SetMinimumCustom(mu8_MIN_NODE_ID_OS);
      this->mpc_Ui->pc_SpinBoxCanOpenId->SetMaximumCustom(mu8_MAX_NODE_ID_OS);
   }
   else
   {
      // set CANopen node id min/max
      this->mpc_Ui->pc_SpinBoxCanOpenId->SetMinimumCustom(mu8_MIN_NODE_ID_CANOPEN);
      this->mpc_Ui->pc_SpinBoxCanOpenId->SetMaximumCustom(mu8_MAX_NODE_ID_CANOPEN);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle CANopen Node ID change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_OnCoNodeIdChanged(void) const
{
   this->m_SaveChanges();
   this->m_CheckCoNodeId();

   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle CANopen Node ID change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_CheckCoNodeId(void) const
{
   const C_OscCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_NodeIndex, this->mu8_InterfaceId);

   if (pc_CanOpenManagerInfo != NULL)
   {
      bool q_ManagerNodeIdInvalid;
      bool q_NodeIdConflict;
      bool q_NoConflict;
      pc_CanOpenManagerInfo->CheckErrorManager(&q_NodeIdConflict, &q_ManagerNodeIdInvalid, NULL, NULL, false);

      q_NoConflict = (q_NodeIdConflict == false) && (q_ManagerNodeIdInvalid == false);

      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxCanOpenId, "Valid", (q_NoConflict == true));

      if (q_NoConflict == true)
      {
         this->mpc_Ui->pc_SpinBoxCanOpenId->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
         this->mpc_Ui->pc_SpinBoxCanOpenId->ShowToolTipWhenDisabled(false);
      }
      else
      {
         QString c_Content;
         if (q_NodeIdConflict == true)
         {
            c_Content = C_GtGetText::h_GetText(
               "The CANopen Node ID must be unique on the bus.");
         }
         if (q_ManagerNodeIdInvalid == true)
         {
            c_Content += C_GtGetText::h_GetText(
               "\nThe CANopen Node ID must be in the range for CANopen (1 - 127).");
         }

         this->mpc_Ui->pc_SpinBoxCanOpenId->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
         this->mpc_Ui->pc_SpinBoxCanOpenId->ShowToolTipWhenDisabled(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle heartbeat produce time change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_OnHeartbeatProducerTimeChanged(void) const
{
   this->m_SaveChanges();
   this->m_CheckHeartbeatProducerTime();

   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle heartbeat produce time change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_CheckHeartbeatProducerTime(void) const
{
   const C_OscCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_NodeIndex, this->mu8_InterfaceId);

   if (pc_CanOpenManagerInfo != NULL)
   {
      bool q_TimeError;
      pc_CanOpenManagerInfo->CheckErrorManager(NULL, NULL, NULL, &q_TimeError, false);

      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxProducerTime, "Valid", (q_TimeError == false));

      if (q_TimeError == false)
      {
         this->mpc_Ui->pc_SpinBoxProducerTime->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
      }
      else
      {
         const QString c_Content = C_GtGetText::h_GetText(
            "The producer time of the Manager must be lower than than the consumer time of all Devices.");
         this->mpc_Ui->pc_SpinBoxProducerTime->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load information

   Load information from core data using node index and interface id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_LoadFromData(void)
{
   const C_OscCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_NodeIndex, this->mu8_InterfaceId);

   //Disconnects for SaveChanges
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxCanOpenId, static_cast<void (QSpinBox::*)(
                                                                int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_OnCoNodeIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxSDOTimeout, static_cast<void (QSpinBox::*)(
                                                                 int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxProducerTime, static_cast<void (QSpinBox::*)(
                                                                   int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_OnHeartbeatProducerTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxSYNCPeriod, static_cast<void (QSpinBox::*)(
                                                                 int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_OnPdoSyncTimeValueChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxSYNCWindowLength, static_cast<void (QSpinBox::*)(
                                                                       int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_OnPdoSyncTimeValueChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_SaveChanges);

   disconnect(this->mpc_Ui->pc_CheckBoxSameAsOpensyde, &QCheckBox::stateChanged, this,
              &C_SdNdeCoManagerIntfWidget::m_OnSameAsOpensydeNodeIdChanged);
   disconnect(this->mpc_Ui->pc_CheckBoxAutostart, &QCheckBox::stateChanged, this,
              &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   disconnect(this->mpc_Ui->pc_CheckBoxStartDevices, &QCheckBox::stateChanged, this,
              &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   disconnect(this->mpc_Ui->pc_CheckBoxNMTStartAll, &QCheckBox::stateChanged, this,
              &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   disconnect(this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing, &QCheckBox::stateChanged, this,
              &C_SdNdeCoManagerIntfWidget::m_OnHeartbeatEnableChanged);
   disconnect(this->mpc_Ui->pc_CheckBoxSYNCProduceEnabled, &QCheckBox::stateChanged, this,
              &C_SdNdeCoManagerIntfWidget::m_OnPdoSyncEnableChanged);

   if (pc_CanOpenManagerInfo != NULL)
   {
      //Change range before setting value
      m_HandleNodeIdRange(pc_CanOpenManagerInfo->q_UseOpenSydeNodeId);

      this->mpc_Ui->pc_SpinBoxCanOpenId->setValue(static_cast<int32_t>(pc_CanOpenManagerInfo->u8_NodeIdValue));
      this->mpc_Ui->pc_CheckBoxSameAsOpensyde->setChecked(pc_CanOpenManagerInfo->q_UseOpenSydeNodeId);
      this->mpc_Ui->pc_CheckBoxAutostart->setChecked(pc_CanOpenManagerInfo->q_AutostartCanOpenManager);
      this->mpc_Ui->pc_CheckBoxStartDevices->setChecked(pc_CanOpenManagerInfo->q_StartDevices);
      this->mpc_Ui->pc_CheckBoxNMTStartAll->setChecked(pc_CanOpenManagerInfo->q_NmtStartAll);
      this->mpc_Ui->pc_SpinBoxSDOTimeout->setValue(static_cast<int32_t>(pc_CanOpenManagerInfo->u16_GlobalSdoTimeoutMs));
      this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setChecked(pc_CanOpenManagerInfo->q_EnableHeartbeatProducing);
      this->mpc_Ui->pc_SpinBoxProducerTime->setValue(static_cast<int32_t>(pc_CanOpenManagerInfo->
                                                                          u16_HeartbeatProducerTimeMs));

      if (pc_CanOpenManagerInfo->e_NmtErrorBehaviour == C_OscCanOpenManagerInfo::eRESTART_ALL_DEVICES)
      {
         this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->setCurrentIndex(static_cast<int32_t>(
                                                                        mu8_NMT_ERROR_RESTART_ALL_DEVICES));
      }
      else if (pc_CanOpenManagerInfo->e_NmtErrorBehaviour == C_OscCanOpenManagerInfo::eSTOP_ALL_DEVICES)
      {
         this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->setCurrentIndex(
            static_cast<int32_t>(mu8_NMT_ERROR_STOP_ALL_DEVICES));
      }
      else if (pc_CanOpenManagerInfo->e_NmtErrorBehaviour == C_OscCanOpenManagerInfo::eRESTART_FAILURE_DEVICE)
      {
         this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->setCurrentIndex(static_cast<int32_t>(
                                                                        mu8_NMT_ERROR_RESTART_FAILURE_DEVICE));
      }
      else
      {
         //unknown state
         tgl_assert(false);
      }

      // SYNC PDO
      this->mpc_Ui->pc_SpinBoxSYNCPeriod->setValue(static_cast<int32_t>(pc_CanOpenManagerInfo->u32_SyncCyclePeriodUs));
      this->mpc_Ui->pc_SpinBoxSYNCWindowLength->setValue(static_cast<int32_t>(pc_CanOpenManagerInfo->
                                                                              u32_SyncWindowLengthUs));
      this->mpc_Ui->pc_CheckBoxSYNCProduceEnabled->setChecked(pc_CanOpenManagerInfo->q_ProduceSyncMessage);
   }

   //trigger OnChange/Handle Events manually
   this->m_HandleSameAsOpensydeNodeIdState();
   this->m_OnStartDevicesChanged();
   this->m_HandleHeartbeatEnableState();
   this->m_HandlePdoSyncEnableState();

   //save current values to data (incorrect default values should be fixed)
   this->SaveToData();

   // Error checks after updating the data
   this->m_CheckCoNodeId();
   this->m_CheckHeartbeatProducerTime();
   Q_EMIT (this->SigErrorChange());

   //connects for SaveChanges

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxCanOpenId, static_cast<void (QSpinBox::*)(
                                                             int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_OnCoNodeIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxSDOTimeout, static_cast<void (QSpinBox::*)(
                                                              int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxProducerTime, static_cast<void (QSpinBox::*)(
                                                                int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_OnHeartbeatProducerTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxSYNCPeriod, static_cast<void (QSpinBox::*)(
                                                              int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_OnPdoSyncTimeValueChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxSYNCWindowLength, static_cast<void (QSpinBox::*)(
                                                                    int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_OnPdoSyncTimeValueChanged);

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_SaveChanges);

   connect(this->mpc_Ui->pc_CheckBoxSameAsOpensyde, &QCheckBox::stateChanged, this,
           &C_SdNdeCoManagerIntfWidget::m_OnSameAsOpensydeNodeIdChanged);
   connect(this->mpc_Ui->pc_CheckBoxAutostart, &QCheckBox::stateChanged, this,
           &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   connect(this->mpc_Ui->pc_CheckBoxStartDevices, &QCheckBox::stateChanged, this,
           &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   connect(this->mpc_Ui->pc_CheckBoxNMTStartAll, &QCheckBox::stateChanged, this,
           &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   connect(this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing, &QCheckBox::stateChanged, this,
           &C_SdNdeCoManagerIntfWidget::m_OnHeartbeatEnableChanged);
   connect(this->mpc_Ui->pc_CheckBoxSYNCProduceEnabled, &QCheckBox::stateChanged, this,
           &C_SdNdeCoManagerIntfWidget::m_OnPdoSyncEnableChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save Changes

   Function where ui elements register a change. Changes will be saved to data.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_SaveChanges(void) const
{
   SaveToData();
}
