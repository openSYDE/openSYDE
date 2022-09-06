//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the configuration of a specific device on a specific interface of the CANopen Manager

   Widget for handling the configuration of a specific device on a specific interface of the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeCoDeviceConfigWidget.h"
#include "ui_C_SdNdeCoDeviceConfigWidget.h"
#include "constants.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OSCCanOpenManagerDeviceInfo.h"
#include "C_SdNdeCoManagerIntfWidget.h"
#include "C_OSCLoggingHandler.h"

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

const uint8 mu8_SUB001 = 1;
const uint8 mu8_SUB002 = 2;
const uint8 mu8_SUB003 = 3;

const uint8 mu8_SUB001_COMBOBOX_INDEX = 0;
const uint8 mu8_SUB002_COMBOBOX_INDEX = 1;
const uint8 mu8_SUB003_COMBOBOX_INDEX = 2;

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
C_SdNdeCoDeviceConfigWidget::C_SdNdeCoDeviceConfigWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoDeviceConfigWidget),
   mu32_ManagerNodeIndex(0),
   mu8_ManagerInterfaceId(0)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // set producer/consumer time min/max
   this->mpc_Ui->pc_SpinBoxProducerTime->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxConsumerTime->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxProducerTime->SetMaximumCustom(65535);
   this->mpc_Ui->pc_SpinBoxConsumerTime->SetMaximumCustom(65535);

   //disable items per default
   this->mpc_Ui->pc_ComboBoxFactorySettings->setEnabled(false);
   this->mpc_Ui->pc_SpinBoxProducerTime->setEnabled(false);
   this->mpc_Ui->pc_SpinBoxConsumerTime->setEnabled(false);
   this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->setEnabled(false);

   // connects
   // Can be connected here due to no changes on real data. Only reconfiguration of ui elements
   connect(this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing, &QCheckBox::toggled, this,
           &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatProducingEnableChanged);
   connect(this->mpc_Ui->pc_CheckBoxFactorySettings, &QCheckBox::toggled, this,
           &C_SdNdeCoDeviceConfigWidget::m_OnFactorySetingsChanged);

   // Hide the option to restore parameters on configuration
   this->m_HideParametersOnConfiguration();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoDeviceConfigWidget::~C_SdNdeCoDeviceConfigWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelCANopenIdCaption->setText(C_GtGetText::h_GetText("CANopen Device Node ID"));

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

   this->mpc_Ui->pc_CheckBoxOptionalDevice->setText(C_GtGetText::h_GetText("Optional CANopen Device"));
   this->mpc_Ui->pc_CheckBoxOptionalDevice->
   SetToolTipInformation(C_GtGetText::h_GetText("Optional CANopen Device"),
                         C_GtGetText::h_GetText("If enabled, the CANopen Device is optional, and it is not "
                                                "mandatory for the start of the CANopen network"));

   this->mpc_Ui->pc_CheckBoxNoInitialization->setText(C_GtGetText::h_GetText("No Initialization"));
   this->mpc_Ui->pc_CheckBoxNoInitialization->
   SetToolTipInformation(C_GtGetText::h_GetText("No Initialization"),
                         C_GtGetText::h_GetText("If enabled, the CANopen Manager sends no configuration SDOs and "
                                                "no NMT start command to the CANopen Device. \n"
                                                "This option is meant for CANopen Devices which already start "
                                                "with valid configuration."));

   //this->mpc_Ui->pc_CheckBoxFactorySettings->setText(C_GtGetText::h_GetText("Factory Settings"));
   this->mpc_Ui->pc_CheckBoxFactorySettings->setText(C_GtGetText::h_GetText("Restore Parameters on Configuration"));
   this->mpc_Ui->pc_CheckBoxFactorySettings->
   SetToolTipInformation(C_GtGetText::h_GetText("Restore Parameters on Configuration"),
                         C_GtGetText::h_GetText("If enabled, the parameters of the CANopen Device are restored to "
                                                "defaults before (re-)configuration."
                                                "It depends on the CANopen Device which parameters can be restored."));

   this->mpc_Ui->pc_LabelOption->setText(C_GtGetText::h_GetText("Restore Option"));
   this->mpc_Ui->pc_LabelOption->
   SetToolTipInformation(C_GtGetText::h_GetText("Restore Option"),
                         C_GtGetText::h_GetText("Factory settings restore options:\n"
                                                "   - Sub:001: All parameters are restored\n"
                                                "   - Sub:002: Communication-related parameters are restored\n"
                                                "   - Sub:003: Application-related parameters are restored \n"
                                                "   - Sub:004 - Sub:127: Manufacturer-specific choice of parameters is restored"));

   this->mpc_Ui->pc_LabelHeartbeatCaption->setText(C_GtGetText::h_GetText("Heartbeat"));

   this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setText(C_GtGetText::h_GetText("Enable Heartbeat Producing"));
   this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->
   SetToolTipInformation(C_GtGetText::h_GetText("Enable Heartbeat Producing"),
                         C_GtGetText::h_GetText(
                            "If enabled, the CANopen Device sends heartbeats according to defined "
                            "\"Producer Time\" interval."));

   this->mpc_Ui->pc_LabelProducerTime->setText(C_GtGetText::h_GetText("Producer Time"));
   this->mpc_Ui->pc_LabelProducerTime->
   SetToolTipInformation(C_GtGetText::h_GetText("Producer Time"),
                         C_GtGetText::h_GetText("Heartbeat interval produced by the CANopen Device."));

   this->mpc_Ui->pc_SpinBoxProducerTime->setSuffix(C_GtGetText::h_GetText(" ms"));

   this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setText(C_GtGetText::h_GetText("Enable Heartbeat Consuming"));
   this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->
   SetToolTipInformation(C_GtGetText::h_GetText("Enable Heartbeat Consuming"),
                         C_GtGetText::h_GetText(
                            "If enabled, the CANopen Device will listen to heartbeats sent by CANopen Manager."));

   this->mpc_Ui->pc_LabelConsumerTime->setText(C_GtGetText::h_GetText("Consumer Time"));
   this->mpc_Ui->pc_LabelConsumerTime->
   SetToolTipInformation(C_GtGetText::h_GetText("Consumer Time"),
                         C_GtGetText::h_GetText("Heartbeat consumer interval."));

   this->mpc_Ui->pc_SpinBoxConsumerTime->setSuffix(C_GtGetText::h_GetText(" ms"));

   this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->setText(C_GtGetText::h_GetText("Automatic"));
   this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->
   SetToolTipInformation(C_GtGetText::h_GetText("Automatic"),
                         C_GtGetText::h_GetText("If enabled, the consumer time is calculated automatically. \n"
                                                "Formula: <device_consumer_time> = <manager_producer_time> * 1.5"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxEnableHeartbeatProducing state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatProducingEnableChanged(void) const
{
   // The checkbox must be checked. But the EDS restriction for read-only must be checked with the enabled state too
   if ((this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->isChecked() == true) &&
       (this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->isEnabled() == true))
   {
      this->mpc_Ui->pc_SpinBoxProducerTime->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxProducerTime->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxEnableHeartbeatProducing state change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumingEnableChanged(void)
{
   this->m_HandleHeartbeatConsumingEnableState();
   //check auto calc
   this->m_OnConsumerTimeAutoChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxEnableHeartbeatConsuming state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_HandleHeartbeatConsumingEnableState(void) const
{
   // The checkbox must be checked. But the EDS restriction for read-only must be checked with the enabled state too
   if ((this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->isChecked() == true) &&
       (this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->isEnabled() == true))
   {
      this->mpc_Ui->pc_SpinBoxConsumerTime->setEnabled(true);
      this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxConsumerTime->setEnabled(false);
      this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxEnableHeartbeatProducing state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_OnFactorySetingsChanged(void) const
{
   if (this->mpc_Ui->pc_CheckBoxFactorySettings->isChecked() == true)
   {
      this->mpc_Ui->pc_ComboBoxFactorySettings->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxFactorySettings->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxSameAsOpensyde state changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_OnSameAsOpensydeNodeIdChanged(void)
{
   // Avoid unnecessary duplicated calls of m_OnCoNodeIdChanged
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxCANopenId, static_cast<void (QSpinBox::*)(
                                                                sintn)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_OnCoNodeIdChanged);

   this->m_HandleSameAsOpensydeNodeIdState();

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxCANopenId, static_cast<void (QSpinBox::*)(
                                                             sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_OnCoNodeIdChanged);

   this->m_OnCoNodeIdChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle the pc_CheckBoxSameAsOpensyde state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_HandleSameAsOpensydeNodeIdState(void) const
{
   if (this->mpc_Ui->pc_CheckBoxSameAsOpensyde->isChecked() == true)
   {
      //get current node ID of node interface
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(
         this->mc_DeviceInterfaceId.u32_NodeIndex);

      // set openSYDE node id min/max
      // necessary to be compatible with complete openSYDE Node ID range
      this->mpc_Ui->pc_SpinBoxCANopenId->SetMinimumCustom(mu8_MIN_NODE_ID_OS);
      this->mpc_Ui->pc_SpinBoxCANopenId->SetMaximumCustom(mu8_MAX_NODE_ID_OS);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         const C_OSCNodeComInterfaceSettings * const pc_ComInterface = pc_Node->c_Properties.GetComInterface(
            C_OSCSystemBus::eCAN, this->mc_DeviceInterfaceId.u8_InterfaceNumber);

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
void C_SdNdeCoDeviceConfigWidget::m_OnCoNodeIdChanged(void)
{
   this->m_SaveChanges();
   this->m_CheckCoNodeId();

   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle CANopen Node ID change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_CheckCoNodeId(void) const
{
   const C_OSCCanOpenManagerInfo * const pc_CanOpenManagerInfo =
      C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId);

   if (pc_CanOpenManagerInfo != NULL)
   {
      bool q_NodeIdInvalid;
      bool q_NodeIdConflict;
      const sint32 s32_FuncReturn = pc_CanOpenManagerInfo->CheckErrorDeviceCoNodeId(this->mc_DeviceInterfaceId,
                                                                                    &q_NodeIdConflict,
                                                                                    &q_NodeIdInvalid, true);

      tgl_assert(s32_FuncReturn == C_NO_ERR);
      if (s32_FuncReturn == C_NO_ERR)
      {
         const bool q_NoConflict = (q_NodeIdConflict == false) && (q_NodeIdInvalid == false);
         C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxCANopenId, "Valid", q_NoConflict);

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
            if (q_NodeIdInvalid == true)
            {
               c_Content += C_GtGetText::h_GetText(
                  "\nThe CANopen Node ID must be in the range for CANopen (1 - 127).");
            }

            this->mpc_Ui->pc_SpinBoxCANopenId->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
            this->mpc_Ui->pc_SpinBoxCANopenId->ShowToolTipWhenDisabled(true);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxSameAsOpensyde state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_OnConsumerTimeAutoChanged(void)
{
   // Avoid unnecessary duplicated calls of m_OnHeartbeatConsumerTimeChanged
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxConsumerTime, static_cast<void (QSpinBox::*)(
                                                                   sintn)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumerTimeChanged);

   this->m_HandleConsumerTimeAutoState();

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxConsumerTime, static_cast<void (QSpinBox::*)(
                                                                sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumerTimeChanged);

   this->m_OnHeartbeatConsumerTimeChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxSameAsOpensyde state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_HandleConsumerTimeAutoState(void)
{
   const C_OSCCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId);

   if (this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->isChecked() == true)
   {
      //auto calculation of consumer time
      const float32 f32_Temp = static_cast<float32>(pc_CanOpenManagerInfo->u16_HeartbeatProducerTimeMs);
      const float32 f32_Result = f32_Temp * mf32_HEARTBEAT_CONSUMER_TIME_FACTOR;
      const uint16 u16_Result = static_cast<uint16>(f32_Result);

      this->mpc_Ui->pc_SpinBoxConsumerTime->setValue(static_cast<sintn>(u16_Result));

      this->mpc_Ui->pc_SpinBoxConsumerTime->setEnabled(false);
   }
   else
   {
      //enable spinbox (only if supported and is not read only)
      if ((this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->isChecked() == true) &&
          (this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->isEnabled() == true))
      {
         this->mpc_Ui->pc_SpinBoxConsumerTime->setEnabled(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle heartbeat produce time change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumerTimeChanged(void)
{
   this->m_SaveChanges();
   this->m_CheckHeartbeatConsumerTime();

   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle heartbeat produce time change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_CheckHeartbeatConsumerTime(void) const
{
   const C_OSCCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId);

   if (pc_CanOpenManagerInfo != NULL)
   {
      bool q_TimeError;
      const sint32 s32_FuncReturn = pc_CanOpenManagerInfo->CheckErrorDeviceHeartbeat(this->mc_DeviceInterfaceId,
                                                                                     &q_TimeError);

      tgl_assert(s32_FuncReturn == C_NO_ERR);
      if (s32_FuncReturn == C_NO_ERR)
      {
         C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxConsumerTime, "Valid", (q_TimeError == false));

         if (q_TimeError == false)
         {
            this->mpc_Ui->pc_SpinBoxConsumerTime->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
            this->mpc_Ui->pc_SpinBoxConsumerTime->ShowToolTipWhenDisabled(false);
         }
         else
         {
            const QString c_Content = C_GtGetText::h_GetText(
               "The consumer time of the Device must be lower or equal to the producer time of the Manager.");
            this->mpc_Ui->pc_SpinBoxConsumerTime->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
            this->mpc_Ui->pc_SpinBoxConsumerTime->ShowToolTipWhenDisabled(true);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Node Index and Inteface ID setter

   Sets the private node index and interface id

   \param[in]  ou32_ManagerNodeIndex     new node index
   \param[in]  ou8_ManagerInterfaceId    new interface index
   \param[in]  orc_DeviceNodeId          new Node ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::SetNodeIndexAndInterfaceId(const stw_types::uint32 ou32_ManagerNodeIndex,
                                                             const uint8 ou8_ManagerInterfaceId,
                                                             const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceNodeId)
{
   this->mu32_ManagerNodeIndex = ou32_ManagerNodeIndex;
   this->mu8_ManagerInterfaceId = ou8_ManagerInterfaceId;
   this->mc_DeviceInterfaceId = orc_DeviceNodeId;

   //load node data
   this->m_LoadFromData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload data

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::Refresh()
{
   //load node data
   this->m_LoadFromData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load information

   Load information from core data using node index and interface id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_LoadFromData(void)
{
   const C_OSCCanOpenManagerDeviceInfo * const pc_CanOpenDeviceInfo =
      C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerDevice(this->mu32_ManagerNodeIndex,
                                                               this->mu8_ManagerInterfaceId,
                                                               this->mc_DeviceInterfaceId);

   const C_OSCCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId);

   //Disconnects for SaveChanges
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxCANopenId, static_cast<void (QSpinBox::*)(
                                                                sintn)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_OnCoNodeIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxProducerTime, static_cast<void (QSpinBox::*)(
                                                                   sintn)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxConsumerTime, static_cast<void (QSpinBox::*)(
                                                                   sintn)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumerTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxFactorySettings,
              static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   disconnect(this->mpc_Ui->pc_CheckBoxSameAsOpensyde, &QCheckBox::stateChanged, this,
              &C_SdNdeCoDeviceConfigWidget::m_OnSameAsOpensydeNodeIdChanged);
   disconnect(this->mpc_Ui->pc_CheckBoxConsumerTimeAuto, &QCheckBox::stateChanged, this,
              &C_SdNdeCoDeviceConfigWidget::m_OnConsumerTimeAutoChanged);
   disconnect(this->mpc_Ui->pc_CheckBoxOptionalDevice, &QCheckBox::stateChanged, this,
              &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   disconnect(this->mpc_Ui->pc_CheckBoxNoInitialization, &QCheckBox::stateChanged, this,
              &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   disconnect(this->mpc_Ui->pc_CheckBoxFactorySettings, &QCheckBox::stateChanged, this,
              &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   disconnect(this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing, &QCheckBox::stateChanged, this,
              &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   disconnect(this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming, &QCheckBox::stateChanged, this,
              &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumingEnableChanged);

   tgl_assert(pc_CanOpenDeviceInfo != NULL);

   if (pc_CanOpenDeviceInfo != NULL)
   {
      const sintn sn_HB_OFF_VALUE = 0;
      std::set<uint8> c_Map = pc_CanOpenDeviceInfo->c_EDSFileContent.GetAllAvailableFactorySettingsSubIndices();
      std::set<uint8>::const_iterator c_ItResult;
      sintn sn_Index = 0;

      this->mpc_Ui->pc_SpinBoxCANopenId->setValue(static_cast<sintn>(pc_CanOpenDeviceInfo->u8_NodeIDValue));
      this->mpc_Ui->pc_CheckBoxSameAsOpensyde->setChecked(pc_CanOpenDeviceInfo->q_UseOpenSYDENodeID);

      this->mpc_Ui->pc_CheckBoxOptionalDevice->setChecked(pc_CanOpenDeviceInfo->q_DeviceOptional);
      this->mpc_Ui->pc_CheckBoxNoInitialization->setChecked(pc_CanOpenDeviceInfo->q_NoInitialization);

      // factory settings
      //add default items Sub:001, Sub:002, Sub:003
      this->mpc_Ui->pc_ComboBoxFactorySettings->clear();
      this->mc_ComboboxIndexToFactorySettingsSub.clear();

      this->mpc_Ui->pc_ComboBoxFactorySettings->addItem(C_GtGetText::h_GetText("Sub:001: All parameters"));
      this->mc_ComboboxIndexToFactorySettingsSub[mu8_SUB001_COMBOBOX_INDEX] = mu8_SUB001;
      this->mpc_Ui->pc_ComboBoxFactorySettings->addItem(C_GtGetText::h_GetText(
                                                           "Sub:002: Communication-related parameters"));
      this->mc_ComboboxIndexToFactorySettingsSub[mu8_SUB002_COMBOBOX_INDEX] = mu8_SUB002;
      this->mpc_Ui->pc_ComboBoxFactorySettings->addItem(C_GtGetText::h_GetText(
                                                           "Sub:003: Application-related parameters"));
      this->mc_ComboboxIndexToFactorySettingsSub[mu8_SUB003_COMBOBOX_INDEX] = mu8_SUB003;

      //check if Sub:001, Sub:002, Sub:003 are supported, if not: disabled item in combobox
      // Sub:001
      c_ItResult = c_Map.find(mu8_SUB001);
      if (c_ItResult == c_Map.end())
      {
         this->mpc_Ui->pc_ComboBoxFactorySettings->SetItemState(mu8_SUB001_COMBOBOX_INDEX, false);
      }
      // Sub:002
      c_ItResult = c_Map.find(mu8_SUB002);
      if (c_ItResult == c_Map.end())
      {
         this->mpc_Ui->pc_ComboBoxFactorySettings->SetItemState(mu8_SUB002_COMBOBOX_INDEX, false);
      }
      // Sub:003
      c_ItResult = c_Map.find(mu8_SUB003);
      if (c_ItResult == c_Map.end())
      {
         this->mpc_Ui->pc_ComboBoxFactorySettings->SetItemState(mu8_SUB003_COMBOBOX_INDEX, false);
      }

      // factory settings supported?
      if (!c_Map.empty())
      {
         //factory support enabled
         this->mpc_Ui->pc_CheckBoxFactorySettings->setEnabled(true);
         this->mpc_Ui->pc_CheckBoxFactorySettings->setChecked(pc_CanOpenDeviceInfo->q_FactorySettingsActive);

         //add Sub:004 - Sub:127: Manufacturer-specific
         for (uint8 u8_SubIndex = 4; u8_SubIndex < 128; u8_SubIndex++)
         {
            c_ItResult = c_Map.find(u8_SubIndex);
            if (c_ItResult != c_Map.end())
            {
               //add item
               if (u8_SubIndex < 10)
               {
                  this->mpc_Ui->pc_ComboBoxFactorySettings->addItem(
                     static_cast<QString>(C_GtGetText::h_GetText("Sub:00%1: Manufacturer-specific")).arg(u8_SubIndex));
               }
               else if (u8_SubIndex < 100)
               {
                  this->mpc_Ui->pc_ComboBoxFactorySettings->addItem(
                     static_cast<QString>(C_GtGetText::h_GetText("Sub:0%1: Manufacturer-specific")).arg(u8_SubIndex));
               }
               else
               {
                  this->mpc_Ui->pc_ComboBoxFactorySettings->addItem(
                     static_cast<QString>(C_GtGetText::h_GetText("Sub:%1: Manufacturer-specific")).arg(u8_SubIndex));
               }
               this->mc_ComboboxIndexToFactorySettingsSub[this->mpc_Ui->pc_ComboBoxFactorySettings->count() -
                                                          1] = u8_SubIndex;
            }
         }
         //select index
         this->m_GetIndexOfSub(pc_CanOpenDeviceInfo->u8_ResetNodeObjectDictionarySubIndex, sn_Index);
         this->mpc_Ui->pc_ComboBoxFactorySettings->setCurrentIndex(sn_Index);
      }
      else
      {
         //factory support disabled
         this->mpc_Ui->pc_CheckBoxFactorySettings->setChecked(false);
         this->mpc_Ui->pc_CheckBoxFactorySettings->setEnabled(false);
         //select first element (visual reason, no impact)
         this->mpc_Ui->pc_ComboBoxFactorySettings->setCurrentIndex(0);
      }

      //heartbeat producing supported?
      if (pc_CanOpenDeviceInfo->c_EDSFileContent.IsHeartbeatProducerSupported() == true)
      {
         //supported
         // check for read-only
         bool q_HbProducerRo = true;
         tgl_assert(pc_CanOpenDeviceInfo->c_EDSFileContent.IsHeartbeatProducerRo(q_HbProducerRo) == C_NO_ERR);
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setEnabled(!q_HbProducerRo);

         this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setChecked(pc_CanOpenDeviceInfo->q_EnableHeartbeatProducing);

         // get default value first (100 ms)
         const sintn sn_DefaultHeartbeatProducerTimeMs =
            static_cast<sintn>(pc_CanOpenDeviceInfo->u16_HeartbeatProducerTimeMs);
         sintn sn_HeartbeatProducerTimeMs = sn_DefaultHeartbeatProducerTimeMs;

         // get heartbeat producer time from EDS
         const C_OSCCanOpenObject * const pc_OscCanOpenObject = pc_CanOpenDeviceInfo->c_EDSFileContent.GetCanOpenObject(
            C_OSCCanOpenObjectDictionary::hu16_OD_INDEX_HEARTBEAT_PRODUCER);
         // in case there is an EDS entry
         if (pc_OscCanOpenObject->c_DefaultValue.IsEmpty() == false)
         {
            try
            {
               // get value from EDS file
               sn_HeartbeatProducerTimeMs = pc_OscCanOpenObject->c_DefaultValue.ToInt();
            }
            catch (...)
            {
               const stw_scl::C_SCLString c_Info = "Could not convert the following number: \"" +
                                                   pc_OscCanOpenObject->c_DefaultValue + "\". Switching heartbeat off.";
               sn_HeartbeatProducerTimeMs = sn_HB_OFF_VALUE;
               osc_write_log_warning("Read CANopen EDS Heartbeat Producer default value", c_Info);
            }
         }

         // on/off handling if default value from EDS is 0
         if (sn_HeartbeatProducerTimeMs == sn_HB_OFF_VALUE)
         {
            this->mpc_Ui->pc_SpinBoxProducerTime->setValue(sn_DefaultHeartbeatProducerTimeMs); // default value, since 0
                                                                                               // is not allowed
            this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setChecked(false);
         }
         else
         {
            this->mpc_Ui->pc_SpinBoxProducerTime->setValue(sn_HeartbeatProducerTimeMs);
         }
      }
      else
      {
         //no support
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setChecked(false);
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setEnabled(false);
      }

      //heartbeat consuming supported and manager generates HB?
      if ((pc_CanOpenDeviceInfo->c_EDSFileContent.GetNumHeartbeatConsumers() != 0) &&
          (pc_CanOpenManagerInfo->q_EnableHeartbeatProducing == true))
      {
         //supported
         // check for read-only
         bool q_HbConsumerRo = true;
         tgl_assert(pc_CanOpenDeviceInfo->c_EDSFileContent.IsHeartbeatConsumerRo(q_HbConsumerRo) == C_NO_ERR);

         this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setEnabled(!q_HbConsumerRo);

         if (q_HbConsumerRo == true)
         {
            // get default value first (100 ms)
            const sintn sn_DefaultHeartbeatConsumerTimeMs =
               static_cast<sintn>(pc_CanOpenDeviceInfo->u16_HeartbeatConsumerTimeMs);
            sintn sn_HeartbeatConsumerTimeMs = sn_DefaultHeartbeatConsumerTimeMs;

            // In case of read only, the default value of EDS file must be set
            // get heartbeat consumer time from EDS
            // Checking the first consumer because this is the entry which will be used for the user specified
            // value when it is not read-only too
            const C_OSCCanOpenObject * const pc_OscCanOpenObject =
               pc_CanOpenDeviceInfo->c_EDSFileContent.GetCanOpenSubIndexObject(
                  C_OSCCanOpenObjectDictionary::hu16_OD_INDEX_HEARTBEAT_CONSUMER, 1);
            // in case there is an EDS entry
            if ((pc_OscCanOpenObject != NULL) &&
                (pc_OscCanOpenObject->c_DefaultValue.IsEmpty() == false))
            {
               try
               {
                  // get value from EDS file
                  sn_HeartbeatConsumerTimeMs = pc_OscCanOpenObject->c_DefaultValue.ToInt();
               }
               catch (...)
               {
                  const stw_scl::C_SCLString c_Info = "Could not convert the following number: \"" +
                                                      pc_OscCanOpenObject->c_DefaultValue +
                                                      "\". Switching heartbeat off.";
                  sn_HeartbeatConsumerTimeMs = sn_HB_OFF_VALUE;
                  osc_write_log_warning("CANopen EDS Heartbeat Consumer default value", c_Info);
               }
            }

            // on/off handling if default value from EDS is 0
            if (sn_HeartbeatConsumerTimeMs == sn_HB_OFF_VALUE)
            {
               this->mpc_Ui->pc_SpinBoxConsumerTime->setValue(sn_DefaultHeartbeatConsumerTimeMs); // default value,
                                                                                                  // since 0
                                                                                                  // is not allowed
               this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setChecked(false);
            }
            else
            {
               this->mpc_Ui->pc_SpinBoxConsumerTime->setValue(sn_HeartbeatConsumerTimeMs);
               this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setChecked(true);
            }
            // In any case the automatic is deactivated
            this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->setChecked(false);
         }
         else
         {
            this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setChecked(
               pc_CanOpenDeviceInfo->q_EnableHeartbeatConsuming);
            this->mpc_Ui->pc_SpinBoxConsumerTime->setValue(static_cast<sintn>(pc_CanOpenDeviceInfo->
                                                                              u16_HeartbeatConsumerTimeMs));
            this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->setChecked(
               pc_CanOpenDeviceInfo->q_EnableHeartbeatConsumingAutoCalculation);
         }
      }
      else
      {
         //no support or manager HB producing is disabled
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setChecked(false);
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setEnabled(false);
      }
   }

   //trigger OnChange/Handle Events manually
   this->m_HandleSameAsOpensydeNodeIdState();
   this->m_OnHeartbeatProducingEnableChanged();
   this->m_HandleHeartbeatConsumingEnableState();
   this->m_HandleConsumerTimeAutoState();
   this->m_OnFactorySetingsChanged();

   //save current values to data (incorrect default values should be fixed)
   this->SaveToData();

   // Error checks after updating the data
   this->m_CheckCoNodeId();
   this->m_CheckHeartbeatConsumerTime();
   Q_EMIT (this->SigErrorChange());

   //connects for SaveChanges

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxCANopenId, static_cast<void (QSpinBox::*)(
                                                             sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_OnCoNodeIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxProducerTime, static_cast<void (QSpinBox::*)(
                                                                sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxConsumerTime, static_cast<void (QSpinBox::*)(
                                                                sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumerTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxFactorySettings,
           static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   connect(this->mpc_Ui->pc_CheckBoxSameAsOpensyde, &QCheckBox::stateChanged, this,
           &C_SdNdeCoDeviceConfigWidget::m_OnSameAsOpensydeNodeIdChanged);
   connect(this->mpc_Ui->pc_CheckBoxConsumerTimeAuto, &QCheckBox::stateChanged, this,
           &C_SdNdeCoDeviceConfigWidget::m_OnConsumerTimeAutoChanged);
   connect(this->mpc_Ui->pc_CheckBoxOptionalDevice, &QCheckBox::stateChanged, this,
           &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   connect(this->mpc_Ui->pc_CheckBoxNoInitialization, &QCheckBox::stateChanged, this,
           &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   connect(this->mpc_Ui->pc_CheckBoxFactorySettings, &QCheckBox::stateChanged, this,
           &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   connect(this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing, &QCheckBox::stateChanged, this,
           &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   connect(this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming, &QCheckBox::stateChanged, this,
           &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumingEnableChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save ui data to node

   Is called from outside
      - on system definition save
      - on page change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::SaveToData(void)
{
   uint8 u8_FactorySettingsSub = 0;

   this->m_GetSubOfIndex(this->mpc_Ui->pc_ComboBoxFactorySettings->currentIndex(), u8_FactorySettingsSub);

   C_PuiSdHandler::h_GetInstance()->SetCanOpenManagerDeviceCommonProperties(
      this->mu32_ManagerNodeIndex,
      this->mu8_ManagerInterfaceId,
      this->mc_DeviceInterfaceId,
      this->mpc_Ui->pc_CheckBoxOptionalDevice->isChecked(),
      this->mpc_Ui->pc_CheckBoxNoInitialization->isChecked(),
      this->mpc_Ui->pc_CheckBoxFactorySettings->isChecked(),
      u8_FactorySettingsSub,
      this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->isChecked(),
      static_cast<uint16>(this->mpc_Ui->pc_SpinBoxProducerTime->value()),
      this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->isChecked(),
      static_cast<uint16>(this->mpc_Ui->pc_SpinBoxConsumerTime->value()),
      this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->isChecked());
   C_PuiSdHandler::h_GetInstance()->SetCanOpenManagerDeviceNodeId(
      this->mu32_ManagerNodeIndex,
      this->mu8_ManagerInterfaceId,
      this->mc_DeviceInterfaceId,
      this->mpc_Ui->pc_CheckBoxSameAsOpensyde->isChecked(),
      static_cast<uint8>(this->mpc_Ui->pc_SpinBoxCANopenId->value()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save Changes

   Function where ui elements register a change. Changes will be saved to data.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_SaveChanges(void)
{
   SaveToData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Combobox Index of Factory Settings Sub

   Function looks into the map mc_ComboboxIndexToFactorySettingsSub and returns an index
   Called by LoadFromData();

   \param[in]       ou8_Sub       Factory Settings Sub

   \retval          Index         Found Combobox Index

*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeCoDeviceConfigWidget::m_GetIndexOfSub(const stw_types::uint8 ou8_Sub, stw_types::sintn & orsn_FoundIndex)
{
   bool q_RetVal = false;

   std::map<stw_types::sintn, stw_types::uint8>::iterator c_ItResult;

   for (c_ItResult = mc_ComboboxIndexToFactorySettingsSub.begin();
        c_ItResult != mc_ComboboxIndexToFactorySettingsSub.end(); ++c_ItResult)
   {
      if (c_ItResult->second == ou8_Sub)
      {
         q_RetVal = true;
         orsn_FoundIndex = c_ItResult->first;
         //stop search
         break;
      }
   }

   return q_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Combobox Index of Factory Settings Sub

   Function looks into the map mc_ComboboxIndexToFactorySettingsSub and returns a Factory Settings Sub
   Called by SaveToData();

   \param[in]       osn_Index    Combobox Index

   \retval          u8_RetVal    Found Factory Settings Sub

*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeCoDeviceConfigWidget::m_GetSubOfIndex(const stw_types::sintn osn_Index, stw_types::uint8 & oru8_FoundSub)
{
   bool q_RetVal = false;

   std::map<stw_types::sintn, stw_types::uint8>::iterator c_ItResult;
   // Search
   c_ItResult = mc_ComboboxIndexToFactorySettingsSub.find(osn_Index);

   if (c_ItResult != mc_ComboboxIndexToFactorySettingsSub.end())
   {
      //found
      q_RetVal = true;
      oru8_FoundSub = c_ItResult->second;
   }

   return q_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide widgets for restoring parameters on configuration i.e. factory settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_HideParametersOnConfiguration(void)
{
   this->mpc_Ui->pc_LabelOption->setVisible(false);
   this->mpc_Ui->pc_CheckBoxFactorySettings->setVisible(false);
   this->mpc_Ui->pc_ComboBoxFactorySettings->setVisible(false);
}
