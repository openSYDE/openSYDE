//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the configuration of a specific device on a specific interface of the CANopen Manager

   Widget for handling the configuration of a specific device on a specific interface of the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdNdeCoDeviceConfigWidget.hpp"
#include "ui_C_SdNdeCoDeviceConfigWidget.h"
#include "constants.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscImportEdsDcf.hpp"
#include "C_OscCanOpenManagerDeviceInfo.hpp"
#include "C_SdNdeCoManagerIntfWidget.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_elements;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

const uint8_t mu8_SUB001 = 1;
const uint8_t mu8_SUB002 = 2;
const uint8_t mu8_SUB003 = 3;

const uint8_t mu8_SUB001_COMBOBOX_INDEX = 0;
const uint8_t mu8_SUB002_COMBOBOX_INDEX = 1;
const uint8_t mu8_SUB003_COMBOBOX_INDEX = 2;

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
   this->mpc_Ui->pc_LabelCanOpenIdCaption->setText(C_GtGetText::h_GetText("CANopen Device Node ID"));

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
                                                "defaults before (re-)configuration. "
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
   disconnect(this->mpc_Ui->pc_SpinBoxCanOpenId, static_cast<void (QSpinBox::*)(
                                                                int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_OnCoNodeIdChanged);

   this->m_HandleSameAsOpensydeNodeIdState();

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxCanOpenId, static_cast<void (QSpinBox::*)(
                                                             int32_t)>(&QSpinBox::valueChanged), this,
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
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNode(
         this->mc_DeviceInterfaceId.u32_NodeIndex);

      // set openSYDE node id min/max
      // necessary to be compatible with complete openSYDE Node ID range
      this->mpc_Ui->pc_SpinBoxCanOpenId->SetMinimumCustom(mu8_MIN_NODE_ID_OS);
      this->mpc_Ui->pc_SpinBoxCanOpenId->SetMaximumCustom(mu8_MAX_NODE_ID_OS);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         const C_OscNodeComInterfaceSettings * const pc_ComInterface = pc_Node->c_Properties.GetComInterface(
            C_OscSystemBus::eCAN, this->mc_DeviceInterfaceId.u8_InterfaceNumber);

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
      // set CANopen node id min/max
      this->mpc_Ui->pc_SpinBoxCanOpenId->SetMinimumCustom(mu8_MIN_NODE_ID_CANOPEN);
      this->mpc_Ui->pc_SpinBoxCanOpenId->SetMaximumCustom(mu8_MAX_NODE_ID_CANOPEN);

      this->mpc_Ui->pc_SpinBoxCanOpenId->setEnabled(true);
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
   const C_OscCanOpenManagerInfo * const pc_CanOpenManagerInfo =
      C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId);

   if (pc_CanOpenManagerInfo != NULL)
   {
      bool q_NodeIdInvalid;
      bool q_NodeIdConflict;
      const int32_t s32_FuncReturn = pc_CanOpenManagerInfo->CheckErrorDeviceCoNodeId(this->mc_DeviceInterfaceId,
                                                                                     &q_NodeIdConflict,
                                                                                     &q_NodeIdInvalid, true);

      tgl_assert(s32_FuncReturn == C_NO_ERR);
      if (s32_FuncReturn == C_NO_ERR)
      {
         const bool q_NoConflict = (q_NodeIdConflict == false) && (q_NodeIdInvalid == false);
         C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxCanOpenId, "Valid", q_NoConflict);

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
            if (q_NodeIdInvalid == true)
            {
               c_Content += C_GtGetText::h_GetText(
                  "\nThe CANopen Node ID must be in the range for CANopen (1 - 127).");
            }

            this->mpc_Ui->pc_SpinBoxCanOpenId->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
            this->mpc_Ui->pc_SpinBoxCanOpenId->ShowToolTipWhenDisabled(true);
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
                                                                   int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumerTimeChanged);

   this->m_HandleConsumerTimeAutoState();

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxConsumerTime, static_cast<void (QSpinBox::*)(
                                                                int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumerTimeChanged);

   this->m_OnHeartbeatConsumerTimeChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle pc_CheckBoxSameAsOpensyde state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceConfigWidget::m_HandleConsumerTimeAutoState(void)
{
   const C_OscCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId);

   if (this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->isChecked() == true)
   {
      //auto calculation of consumer time
      const float32_t f32_Temp = static_cast<float32_t>(pc_CanOpenManagerInfo->u16_HeartbeatProducerTimeMs);
      const float32_t f32_Result = f32_Temp * mf32_HEARTBEAT_CONSUMER_TIME_FACTOR;
      const uint16_t u16_Result = static_cast<uint16_t>(f32_Result);

      this->mpc_Ui->pc_SpinBoxConsumerTime->setValue(static_cast<int32_t>(u16_Result));

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
   const C_OscCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId);

   if (pc_CanOpenManagerInfo != NULL)
   {
      bool q_TimeError;
      const int32_t s32_FuncReturn = pc_CanOpenManagerInfo->CheckErrorDeviceHeartbeat(this->mc_DeviceInterfaceId,
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
               "The consumer time of the Device must be higher than the producer time of the Manager.");
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
void C_SdNdeCoDeviceConfigWidget::SetNodeIndexAndInterfaceId(const uint32_t ou32_ManagerNodeIndex,
                                                             const uint8_t ou8_ManagerInterfaceId,
                                                             const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId)
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
   const C_OscCanOpenManagerDeviceInfo * const pc_CanOpenDeviceInfo =
      C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerDevice(this->mu32_ManagerNodeIndex,
                                                               this->mu8_ManagerInterfaceId,
                                                               this->mc_DeviceInterfaceId);

   const C_OscCanOpenManagerInfo * const pc_CanOpenManagerInfo = C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(
      this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId);

   //Disconnects for SaveChanges
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxCanOpenId, static_cast<void (QSpinBox::*)(
                                                                int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_OnCoNodeIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxProducerTime, static_cast<void (QSpinBox::*)(
                                                                   int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxConsumerTime, static_cast<void (QSpinBox::*)(
                                                                   int32_t)>(&QSpinBox::valueChanged), this,
              &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumerTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxFactorySettings,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
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
      std::set<uint8_t> c_Map = pc_CanOpenDeviceInfo->c_EdsFileContent.GetAllAvailableFactorySettingsSubIndices();
      std::set<uint8_t>::const_iterator c_ItResult;
      int32_t s32_Index = 0;

      this->mpc_Ui->pc_SpinBoxCanOpenId->setValue(static_cast<int32_t>(pc_CanOpenDeviceInfo->u8_NodeIdValue));
      this->mpc_Ui->pc_CheckBoxSameAsOpensyde->setChecked(pc_CanOpenDeviceInfo->q_UseOpenSydeNodeId);

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
         for (uint8_t u8_SubIndex = 4; u8_SubIndex < 128; u8_SubIndex++)
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
         this->m_GetIndexOfSub(pc_CanOpenDeviceInfo->u8_ResetNodeObjectDictionarySubIndex, s32_Index);
         this->mpc_Ui->pc_ComboBoxFactorySettings->setCurrentIndex(s32_Index);
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
      if (pc_CanOpenDeviceInfo->c_EdsFileContent.IsHeartbeatProducerSupported() == true)
      {
         //supported
         // check for read-only
         bool q_HbProducerRo = true;
         tgl_assert(pc_CanOpenDeviceInfo->c_EdsFileContent.IsHeartbeatProducerRo(q_HbProducerRo) == C_NO_ERR);
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setEnabled(!q_HbProducerRo);

         this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setChecked(pc_CanOpenDeviceInfo->q_EnableHeartbeatProducing);
      }
      else
      {
         //no support
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setChecked(false);
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatProducing->setEnabled(false);
      }

      //heartbeat consuming supported and manager generates HB?
      if ((pc_CanOpenDeviceInfo->c_EdsFileContent.GetNumHeartbeatConsumers() != 0) &&
          (pc_CanOpenManagerInfo->q_EnableHeartbeatProducing == true))
      {
         //supported
         // check for read-only
         bool q_HbConsumerRo = true;
         tgl_assert(pc_CanOpenDeviceInfo->c_EdsFileContent.IsHeartbeatConsumerRo(q_HbConsumerRo) == C_NO_ERR);

         this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setEnabled(!q_HbConsumerRo);

         this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setChecked(
            pc_CanOpenDeviceInfo->q_EnableHeartbeatConsuming);
         this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->setChecked(
            pc_CanOpenDeviceInfo->q_EnableHeartbeatConsumingAutoCalculation);
      }
      else
      {
         //no support or manager HB producing is disabled
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setChecked(false);
         this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->setEnabled(false);
      }

      //Always fill some value in spin boxes
      this->mpc_Ui->pc_SpinBoxProducerTime->setValue(static_cast<int32_t>(pc_CanOpenDeviceInfo->
                                                                          u16_HeartbeatProducerTimeMs));
      this->mpc_Ui->pc_SpinBoxConsumerTime->setValue(static_cast<int32_t>(pc_CanOpenDeviceInfo->
                                                                          u16_HeartbeatConsumerTimeMs));
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
   connect(this->mpc_Ui->pc_SpinBoxCanOpenId, static_cast<void (QSpinBox::*)(
                                                             int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_OnCoNodeIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxProducerTime, static_cast<void (QSpinBox::*)(
                                                                int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_SaveChanges);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxConsumerTime, static_cast<void (QSpinBox::*)(
                                                                int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeCoDeviceConfigWidget::m_OnHeartbeatConsumerTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxFactorySettings,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
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
   uint8_t u8_FactorySettingsSub = 0;

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
      static_cast<uint16_t>(this->mpc_Ui->pc_SpinBoxProducerTime->value()),
      this->mpc_Ui->pc_CheckBoxEnableHeartbeatConsuming->isChecked(),
      static_cast<uint16_t>(this->mpc_Ui->pc_SpinBoxConsumerTime->value()),
      this->mpc_Ui->pc_CheckBoxConsumerTimeAuto->isChecked());
   C_PuiSdHandler::h_GetInstance()->SetCanOpenManagerDeviceNodeId(
      this->mu32_ManagerNodeIndex,
      this->mu8_ManagerInterfaceId,
      this->mc_DeviceInterfaceId,
      this->mpc_Ui->pc_CheckBoxSameAsOpensyde->isChecked(),
      static_cast<uint8_t>(this->mpc_Ui->pc_SpinBoxCanOpenId->value()));
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

   \param[in]       ou8_Sub             Factory Settings Sub
   \param[out]      ors32_FoundIndex    Found Factory Settings Index

   \retval          Flag if index found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeCoDeviceConfigWidget::m_GetIndexOfSub(const uint8_t ou8_Sub, int32_t & ors32_FoundIndex)
{
   bool q_RetVal = false;

   std::map<int32_t, uint8_t>::iterator c_ItResult;

   for (c_ItResult = mc_ComboboxIndexToFactorySettingsSub.begin();
        c_ItResult != mc_ComboboxIndexToFactorySettingsSub.end(); ++c_ItResult)
   {
      if (c_ItResult->second == ou8_Sub)
      {
         q_RetVal = true;
         ors32_FoundIndex = c_ItResult->first;
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

   \param[in]       os32_Index       Combobox Index
   \param[out]      oru8_FoundSub    Found Factory Settings Sub

   \retval          Flag if sub found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeCoDeviceConfigWidget::m_GetSubOfIndex(const int32_t os32_Index, uint8_t & oru8_FoundSub)
{
   bool q_RetVal = false;

   std::map<int32_t, uint8_t>::iterator c_ItResult;
   // Search
   c_ItResult = mc_ComboboxIndexToFactorySettingsSub.find(os32_Index);

   if (c_ItResult != mc_ComboboxIndexToFactorySettingsSub.end())
   {
      //found
      q_RetVal = true;
      oru8_FoundSub = c_ItResult->second;
   }

   return q_RetVal;
}
