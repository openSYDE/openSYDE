//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with current CANopen settings of Manager for a specific interface

   Widget with current CANopen settings of Manager for a specific interface

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeCoManagerIntfWidget.h"
#include "ui_C_SdNdeCoManagerIntfWidget.h"

#include "stwerrors.h"
#include "constants.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_OSCCanOpenManagerInfo.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_elements;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

const uint8 mu8_NMT_ERROR_RESTART_ALL_DEVICES = 0;
const uint8 mu8_NMT_ERROR_RESTART_FAILURE_DEVICE = 1;
const uint8 mu8_NMT_ERROR_STOP_ALL_DEVICES = 2;
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

   //disable items per default
   this->mpc_Ui->pc_CheckBoxNMTStartAll->setEnabled(false);
   this->mpc_Ui->pc_SpinBoxProducerTime->setEnabled(false);

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
   this->mpc_Ui->pc_LabelCANopenIdCaption->setText(C_GtGetText::h_GetText("CANopen Manager Node ID"));

   this->mpc_Ui->pc_LabelCANopenId->setText(C_GtGetText::h_GetText("CANopen Node ID"));
   this->mpc_Ui->pc_LabelCANopenId->
   SetToolTipInformation(C_GtGetText::h_GetText("CANopen Node ID"),
                         C_GtGetText::h_GetText("Unique Node ID within a CANopen network."));

   this->mpc_Ui->pc_CheckBoxSameAsOpensyde->setText(C_GtGetText::h_GetText("use openSYDE Node ID"));
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Node Index and Inteface ID setter

   Sets the private node index and interface id

   \param[in]  ou32_NodeIndex     new node index
   \param[in]  ou8_InterfaceId    new interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::SetNodeIndexAndInterfaceId(const stw_types::uint32 ou32_NodeIndex,
                                                            const uint8 ou8_InterfaceId)
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
   C_OSCCanOpenManagerInfo::E_NMTErrorBehaviourType e_NMTErrorBehaviour = C_OSCCanOpenManagerInfo::eSTOP_ALL_DEVICES;

   if (this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->currentIndex() == mu8_NMT_ERROR_RESTART_ALL_DEVICES)
   {
      e_NMTErrorBehaviour = C_OSCCanOpenManagerInfo::eRESTART_ALL_DEVICES;
   }
   else if (this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->currentIndex() == mu8_NMT_ERROR_STOP_ALL_DEVICES)
   {
      e_NMTErrorBehaviour = C_OSCCanOpenManagerInfo::eSTOP_ALL_DEVICES;
   }
   else if (this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->currentIndex() == mu8_NMT_ERROR_RESTART_FAILURE_DEVICE)
   {
      e_NMTErrorBehaviour = C_OSCCanOpenManagerInfo::eRESTART_FAILURE_DEVICE;
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
      static_cast<uint8>(this->mpc_Ui->pc_SpinBoxCANopenId->value()),
      static_cast<uint16>(this->mpc_Ui->pc_SpinBoxSDOTimeout->value()),
      this->mpc_Ui->pc_CheckBoxAutostart->isChecked(),
      this->mpc_Ui->pc_CheckBoxStartDevices->isChecked(),
      this->mpc_Ui->pc_CheckBoxNMTStartAll->isChecked(),
      e_NMTErrorBehaviour);
   C_PuiSdHandler::h_GetInstance()->SetCanOpenManagerProducerHeartbeat(
      this->mu32_NodeIndex,
      this->mu8_InterfaceId,
      static_cast<uint16>(this->mpc_Ui->pc_SpinBoxProducerTime->value()),
      this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->isChecked());
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
/*! \brief   Handle pc_CheckBoxSameAsOpensyde state changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_OnSameAsOpensydeNodeIdChanged(void) const
{
   // Avoid unnecessary duplicated calls of m_OnCoNodeIdChanged
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxCANopenId, static_cast<void (QSpinBox::*)(
                                                                sintn)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_OnCoNodeIdChanged);

   this->m_HandleSameAsOpensydeNodeIdState();

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxCANopenId, static_cast<void (QSpinBox::*)(
                                                             sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_OnCoNodeIdChanged);

   this->m_OnCoNodeIdChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle the pc_CheckBoxSameAsOpensyde state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoManagerIntfWidget::m_HandleSameAsOpensydeNodeIdState(void) const
{
   if (this->mpc_Ui->pc_CheckBoxSameAsOpensyde->isChecked() == true)
   {
      //get current node ID of node interface
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(this->mu32_NodeIndex);

      // set openSYDE node id min/max
      // necessary to be compatible with complete openSYDE Node ID range
      this->mpc_Ui->pc_SpinBoxCANopenId->SetMinimumCustom(mu8_MIN_NODE_ID_OS);
      this->mpc_Ui->pc_SpinBoxCANopenId->SetMaximumCustom(mu8_MAX_NODE_ID_OS);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         const C_OSCNodeComInterfaceSettings * const pc_ComInterface = pc_Node->c_Properties.GetComInterface(
            C_OSCSystemBus::eCAN, this->mu8_InterfaceId);

         tgl_assert(pc_ComInterface != NULL);
         if (pc_ComInterface != NULL)
         {
            this->mpc_Ui->pc_SpinBoxCANopenId->setValue(static_cast<sintn>(pc_ComInterface->u8_NodeID));
         }
      }

      this->mpc_Ui->pc_SpinBoxCANopenId->setEnabled(false);
   }
   else
   {
      // set CANopen node id min/max
      this->mpc_Ui->pc_SpinBoxCANopenId->SetMinimumCustom(mu8_MIN_NODE_ID_CANOPEN);
      this->mpc_Ui->pc_SpinBoxCANopenId->SetMaximumCustom(mu8_MAX_NODE_ID_CANOPEN);

      this->mpc_Ui->pc_SpinBoxCANopenId->setEnabled(true);
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
   const C_OSCCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_NodeIndex, this->mu8_InterfaceId);

   if (pc_CanOpenManagerInfo != NULL)
   {
      bool q_ManagerNodeIdInvalid;
      bool q_NodeIdConflict;
      bool q_NoConflict;
      pc_CanOpenManagerInfo->CheckErrorManager(&q_NodeIdConflict, &q_ManagerNodeIdInvalid, NULL, NULL, false);

      q_NoConflict = (q_NodeIdConflict == false) && (q_ManagerNodeIdInvalid == false);

      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxCANopenId, "Valid", (q_NoConflict == true));

      if (q_NoConflict == true)
      {
         this->mpc_Ui->pc_SpinBoxCANopenId->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
         this->mpc_Ui->pc_SpinBoxCANopenId->ShowToolTipWhenDisabled(false);
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

         this->mpc_Ui->pc_SpinBoxCANopenId->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
         this->mpc_Ui->pc_SpinBoxCANopenId->ShowToolTipWhenDisabled(true);
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
   const C_OSCCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
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
            "The producer time of the Manager must be higher than than the consumer time of all Devices.");
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
   const C_OSCCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_NodeIndex, this->mu8_InterfaceId);

   //Disconnects for SaveChanges
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxCANopenId, static_cast<void (QSpinBox::*)(
                                                                sintn)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_OnCoNodeIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxSDOTimeout, static_cast<void (QSpinBox::*)(
                                                                 sintn)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxProducerTime, static_cast<void (QSpinBox::*)(
                                                                   sintn)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoManagerIntfWidget::m_OnHeartbeatProducerTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour,
              static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged), this,
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

   if (pc_CanOpenManagerInfo != NULL)
   {
      this->mpc_Ui->pc_SpinBoxCANopenId->setValue(static_cast<sintn>(pc_CanOpenManagerInfo->u8_NodeIDValue));
      this->mpc_Ui->pc_CheckBoxSameAsOpensyde->setChecked(pc_CanOpenManagerInfo->q_UseOpenSYDENodeID);
      this->mpc_Ui->pc_CheckBoxAutostart->setChecked(pc_CanOpenManagerInfo->q_AutostartCanOpenManager);
      this->mpc_Ui->pc_CheckBoxStartDevices->setChecked(pc_CanOpenManagerInfo->q_StartDevices);
      this->mpc_Ui->pc_CheckBoxNMTStartAll->setChecked(pc_CanOpenManagerInfo->q_NMTStartAll);
      this->mpc_Ui->pc_SpinBoxSDOTimeout->setValue(static_cast<sintn>(pc_CanOpenManagerInfo->u16_GlobalSDOTimeoutMs));
      this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setChecked(pc_CanOpenManagerInfo->q_EnableHeartbeatProducing);
      this->mpc_Ui->pc_SpinBoxProducerTime->setValue(static_cast<sintn>(pc_CanOpenManagerInfo->
                                                                        u16_HeartbeatProducerTimeMs));

      if (pc_CanOpenManagerInfo->e_NMTErrorBehaviour == C_OSCCanOpenManagerInfo::eRESTART_ALL_DEVICES)
      {
         this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->setCurrentIndex(static_cast<sintn>(
                                                                        mu8_NMT_ERROR_RESTART_ALL_DEVICES));
      }
      else if (pc_CanOpenManagerInfo->e_NMTErrorBehaviour == C_OSCCanOpenManagerInfo::eSTOP_ALL_DEVICES)
      {
         this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->setCurrentIndex(static_cast<sintn>(mu8_NMT_ERROR_STOP_ALL_DEVICES));
      }
      else if (pc_CanOpenManagerInfo->e_NMTErrorBehaviour == C_OSCCanOpenManagerInfo::eRESTART_FAILURE_DEVICE)
      {
         this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour->setCurrentIndex(static_cast<sintn>(
                                                                        mu8_NMT_ERROR_RESTART_FAILURE_DEVICE));
      }
      else
      {
         //unknown state
         tgl_assert(false);
      }
   }

   //trigger OnChange/Handle Events manually
   this->m_HandleSameAsOpensydeNodeIdState();
   this->m_OnStartDevicesChanged();
   this->m_HandleHeartbeatEnableState();

   //save current values to data (incorrect default values should be fixed)
   this->SaveToData();

   // Error checks after updating the data
   this->m_CheckCoNodeId();
   this->m_CheckHeartbeatProducerTime();
   Q_EMIT (this->SigErrorChange());

   //connects for SaveChanges

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxCANopenId, static_cast<void (QSpinBox::*)(
                                                             sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_OnCoNodeIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxSDOTimeout, static_cast<void (QSpinBox::*)(
                                                              sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_SaveChanges);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxProducerTime, static_cast<void (QSpinBox::*)(
                                                                sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoManagerIntfWidget::m_OnHeartbeatProducerTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxNMTErrorBehaviour,
           static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged), this,
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
