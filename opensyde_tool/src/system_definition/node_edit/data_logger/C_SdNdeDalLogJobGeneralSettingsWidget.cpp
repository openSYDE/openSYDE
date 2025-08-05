//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Creating General X-App settings

   Creating General X-App settings

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdNdeDalLogJobGeneralSettingsWidget.hpp"
#include "ui_C_SdNdeDalLogJobGeneralSettingsWidget.h"

#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobGeneralSettingsWidget::C_SdNdeDalLogJobGeneralSettingsWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDalLogJobGeneralSettingsWidget),
   mu32_NodeIndex(0)
{
   this->mpc_Ui->setupUi(this);

   // panel colors
   this->mpc_Ui->pc_WiHardwareDescriptionFilePanel->SetBackgroundColor(11);
   this->mpc_Ui->pc_LabelHardwareDescriptionFileTitle->SetBackgroundColor(0);
   this->mpc_Ui->pc_LabelHardwareDescriptionFileTitle->SetForegroundColor(3);
   this->mpc_Ui->pc_LabelHardwareDescriptionFileTitle->SetFontPixel(16, true);
   this->mpc_Ui->pc_LabelHardwareDescriptionFileTitle->setText(C_GtGetText::h_GetText("General X.App Settings"));

   //labels
   this->mpc_Ui->pc_LabelClientInterface->setText(C_GtGetText::h_GetText("Client Interface"));
   this->mpc_Ui->pc_LabelPollingInterval->setText(C_GtGetText::h_GetText("Polling Interval COMM Data"));
   this->mpc_Ui->pc_LabelRemoteDataRequestInterval->setText(C_GtGetText::h_GetText("Remote Data Request Interval"));

   //Tool tips
   this->mpc_Ui->pc_LabelClientInterface->SetToolTipInformation(C_GtGetText::h_GetText("Client Interface"),
                                                                C_GtGetText::h_GetText(
                                                                   "X.App client interface to use when communicating with other topology devices."));

   this->mpc_Ui->pc_LabelPollingInterval->SetToolTipInformation(C_GtGetText::h_GetText("Polling Interval COMM Data"),
                                                                C_GtGetText::h_GetText(
                                                                   "Polling interval for checking (local) COMM data elements.\n"
                                                                   "Use-case: Option for tuning the CPU load of the file-based device."));

   this->mpc_Ui->pc_LabelRemoteDataRequestInterval->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                             "Remote Data Request Interval"),
                                                                          C_GtGetText::h_GetText(
                                                                             "Interval of the X.App to requests remote data from other topology devices. For remote requests the openSYDE protocol is used. This may cause higher bus load.\n"
                                                                             "Use-case: Option for tuning the bus load of file-based device."));

   // set PollingInterval min/max
   this->mpc_Ui->pc_SpnBoxPollingInterval->SetMinimumCustom(10);
   this->mpc_Ui->pc_SpnBoxPollingInterval->SetMaximumCustom(100);

   // set RemoteDataRequestInterval min/max
   this->mpc_Ui->pc_SpnBoxRemoteDataRequestInterval->SetMinimumCustom(50);
   this->mpc_Ui->pc_SpnBoxRemoteDataRequestInterval->SetMaximumCustom(5000);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobGeneralSettingsWidget::~C_SdNdeDalLogJobGeneralSettingsWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node index.

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobGeneralSettingsWidget::SetNode(const uint32_t ou32_NodeIndex)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   this->mu32_NodeIndex = ou32_NodeIndex;
   this->m_GetSupportedClientInterfaces();
   this->m_DisconnectChangeTriggers();
   if (pc_Node != NULL)
   {
      const C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
      this->mc_Properties = pc_Node->c_XappProperties;
      if (this->mc_Properties.e_ConnectedInterfaceType == C_OscSystemBus::E_Type::eETHERNET)
      {
         this->mpc_Ui->pc_ComboBoxClientInterface->setCurrentIndex(static_cast<uint8_t>(this->mc_Properties.
                                                                                        u8_ConnectedInterfaceNumber +
                                                                                        pc_DevDef->u8_NumCanBusses));
      }
      else
      {
         this->mpc_Ui->pc_ComboBoxClientInterface->setCurrentIndex(static_cast<uint8_t>(this->mc_Properties.
                                                                                        u8_ConnectedInterfaceNumber));
      }
      this->mpc_Ui->pc_SpnBoxPollingInterval->setValue(this->mc_Properties.
                                                       u32_PollingIntervalMs);
      this->mpc_Ui->pc_SpnBoxRemoteDataRequestInterval->setValue(this->mc_Properties.
                                                                 u32_DataRequestIntervalMs);
   }
   this->m_ReconnectChangeTriggers();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobGeneralSettingsWidget::Save()
{
   C_PuiSdHandler::h_GetInstance()->SetXappInterface(mu32_NodeIndex,
                                                     this->mc_Properties.e_ConnectedInterfaceType,
                                                     this->mc_Properties.u8_ConnectedInterfaceNumber);
   C_PuiSdHandler::h_GetInstance()->SetXappProperties(mu32_NodeIndex,
                                                      this->mc_Properties.u32_PollingIntervalMs,
                                                      this->mc_Properties.u32_DataRequestIntervalMs);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} TODO
void C_SdNdeDalLogJobGeneralSettingsWidget::SaveUserSettings() const
{
   //TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add client interfaces (CAN/ETHERNET) into related combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobGeneralSettingsWidget::m_GetSupportedClientInterfaces() const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   const C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;

   std::vector<QString> c_InterfaceNames;
   QString c_InterfaceName;

   for (uint8_t u8_Iterator = 0; u8_Iterator < pc_DevDef->u8_NumCanBusses; ++u8_Iterator)
   {
      c_InterfaceName = C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eCAN, u8_Iterator);
      c_InterfaceNames.push_back(c_InterfaceName);
   }

   for (uint8_t u8_Iterator = 0; u8_Iterator < pc_DevDef->u8_NumEthernetBusses; ++u8_Iterator)
   {
      c_InterfaceName = C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eETHERNET, u8_Iterator);
      c_InterfaceNames.push_back(c_InterfaceName);
   }

   for (uint32_t u32_InterfaceNamesCounter = 0U;
        u32_InterfaceNamesCounter < static_cast<uint32_t>(c_InterfaceNames.size());
        ++u32_InterfaceNamesCounter)
   {
      const C_OscNodeComInterfaceSettings & rc_Interface =
         pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceNamesCounter];
      if (rc_Interface.GetBusConnected() == true)
      {
         const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
            rc_Interface.u32_BusIndex);
         if (pc_Bus != NULL)
         {
            this->mpc_Ui->pc_ComboBoxClientInterface->addItem(static_cast<QString>(C_GtGetText::h_GetText(
                                                                                      "%1 (linked to %2)")).arg(
                                                                 c_InterfaceNames[
                                                                    u32_InterfaceNamesCounter
                                                                 ]).arg(pc_Bus->c_Name.c_str()));
         }
      }
      else
      {
         this->mpc_Ui->pc_ComboBoxClientInterface->addItem(static_cast<QString>(C_GtGetText::h_GetText(
                                                                                   "%1 (not connected)")).arg(
                                                              c_InterfaceNames[
                                                                 u32_InterfaceNamesCounter
                                                              ]));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for client interface combo box index change.

   \param[in]  os32_NewIndex  New combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobGeneralSettingsWidget::m_OnClientInterfaceChanged(const int32_t os32_NewIndex)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;

      if (os32_NewIndex < pc_DevDef->u8_NumCanBusses)
      {
         this->mc_Properties.e_ConnectedInterfaceType = C_OscSystemBus::E_Type::eCAN;
         this->mc_Properties.u8_ConnectedInterfaceNumber = static_cast<uint8_t>(os32_NewIndex);
      }
      else
      {
         this->mc_Properties.e_ConnectedInterfaceType = C_OscSystemBus::E_Type::eETHERNET;
         this->mc_Properties.u8_ConnectedInterfaceNumber = static_cast<uint8_t>(os32_NewIndex) -
                                                           pc_DevDef->u8_NumCanBusses;
      }
   }
   C_PuiSdHandler::h_GetInstance()->SetXappInterface(mu32_NodeIndex,
                                                     this->mc_Properties.e_ConnectedInterfaceType,
                                                     this->mc_Properties.u8_ConnectedInterfaceNumber);
   Q_EMIT this->SigReloadDataLoggerDataElements();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for X-App general settings data value changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobGeneralSettingsWidget::m_OnXappSettingsChanged()
{
   this->mc_Properties.u32_DataRequestIntervalMs = this->mpc_Ui->pc_SpnBoxRemoteDataRequestInterval->value();
   this->mc_Properties.u32_PollingIntervalMs = this->mpc_Ui->pc_SpnBoxPollingInterval->value();
   this->Save();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Disconnect change triggers
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobGeneralSettingsWidget::m_DisconnectChangeTriggers() const
{
   disconnect(this->mpc_Ui->pc_ComboBoxClientInterface,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeDalLogJobGeneralSettingsWidget::m_OnClientInterfaceChanged);
   disconnect(this->mpc_Ui->pc_SpnBoxPollingInterval,
              static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged),
              this, &C_SdNdeDalLogJobGeneralSettingsWidget::m_OnXappSettingsChanged);
   disconnect(this->mpc_Ui->pc_SpnBoxRemoteDataRequestInterval,
              static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged),
              this, &C_SdNdeDalLogJobGeneralSettingsWidget::m_OnXappSettingsChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reconnect change triggers
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobGeneralSettingsWidget::m_ReconnectChangeTriggers() const
{
   connect(this->mpc_Ui->pc_ComboBoxClientInterface,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobGeneralSettingsWidget::m_OnClientInterfaceChanged);
   connect(this->mpc_Ui->pc_SpnBoxPollingInterval,
           static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged),
           this, &C_SdNdeDalLogJobGeneralSettingsWidget::m_OnXappSettingsChanged);
   connect(this->mpc_Ui->pc_SpnBoxRemoteDataRequestInterval,
           static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged),
           this, &C_SdNdeDalLogJobGeneralSettingsWidget::m_OnXappSettingsChanged);
}
