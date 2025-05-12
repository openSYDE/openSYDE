//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A concrete node selector widget for C_SdBueNodeSelectorCheckBoxListWidget (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdBueNodeSelectorCheckBoxItemWidget.hpp"
#include "ui_C_SdBueNodeSelectorCheckBoxItemWidget.h"

#include "stwerrors.hpp"
#include "C_OscNode.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_GtGetText.hpp"
#include "TglUtils.hpp"
#include "C_Uti.hpp"
#include "C_SdUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_NodeIndex         Index of node
   \param[in]      ou32_InterfaceIndex    Index of node interface
   \param[in]      orc_Name               Name of node
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueNodeSelectorCheckBoxItemWidget::C_SdBueNodeSelectorCheckBoxItemWidget(const uint32_t ou32_NodeIndex,
                                                                             const uint32_t ou32_InterfaceIndex,
                                                                             const QString & orc_Name,
                                                                             QWidget * const opc_Parent) :
   C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_SdBueNodeSelectorCheckBoxItemWidget),
   mc_IconActive(":/images/system_definition/IconNode.svg"),
   mc_IconInactive(":/images/system_definition/IconNodeInactive.svg"),
   me_ProtocolType(stw::opensyde_core::C_OscCanProtocol::eLAYER2),
   mq_IsManager(false),
   mq_IsDevice(false),
   mu32_ManagerNodeIndex(0U),
   mu32_ManagerIntfIndex(0U)
{
   mpc_Ui->setupUi(this);

   mpc_CheckBox = new C_OgeChxDefaultSmall(ou32_NodeIndex, ou32_InterfaceIndex, this);
   this->mpc_CheckBox->setText(orc_Name);
   this->mpc_CheckBox->setIcon(this->mc_IconInactive);

   //Link
   this->mpc_Ui->pc_LinkToManagerLabel->setVisible(false);
   this->mpc_Ui->pc_LinkToManagerLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
   this->mpc_Ui->pc_LinkToManagerLabel->setOpenExternalLinks(false);
   this->mpc_Ui->pc_LinkToManagerLabel->setFocusPolicy(Qt::NoFocus);
   this->mpc_Ui->pc_LinkToManagerLabel->setContextMenuPolicy(Qt::NoContextMenu);

   connect(this->mpc_Ui->pc_LinkToManagerLabel, &QLabel::linkActivated, this,
           &C_SdBueNodeSelectorCheckBoxItemWidget::m_OnLinkSwitchToManager);

   connect(this->mpc_CheckBox, &C_OgeChxDefaultSmall::SigCheckBoxToggled,
           this, &C_SdBueNodeSelectorCheckBoxItemWidget::m_NodeToggled);

   this->mpc_Ui->pc_HorizontalLayout->insertWidget(0, this->mpc_CheckBox);

   this->UpdateToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_CheckBox and the Qt memory management
C_SdBueNodeSelectorCheckBoxItemWidget::~C_SdBueNodeSelectorCheckBoxItemWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the configured indexes

   \param[out]  oru32_Index      First index
   \param[out]  oru32_SubIndex   Second index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::GetIndexes(uint32_t & oru32_Index, uint32_t & oru32_SubIndex) const
{
   this->mpc_CheckBox->GetIndexes(oru32_Index, oru32_SubIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setting checked state of check box

   \param[in]  oq_Checked  Check flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::SetChecked(const bool oq_Checked)
{
   this->mpc_CheckBox->setChecked(oq_Checked);
   this->m_AdaptIcon(oq_Checked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returning the check state of the check box

   \return
   true: checked
   false: not checked
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueNodeSelectorCheckBoxItemWidget::IsChecked(void) const
{
   return this->mpc_CheckBox->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the actual protocol to mark the protocol using nodes

   \param[in]  oe_Protocol    Protocol id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::SetProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol)
{
   const bool oq_CanOpen = (oe_Protocol == C_OscCanProtocol::eCAN_OPEN);

   this->me_ProtocolType = oe_Protocol;

   // Special case CANopen: Flags are read only
   this->mpc_CheckBox->setEnabled(!oq_CanOpen);

   // In case of a changed protocol, deactivate the manager link in any case as first step
   this->SetNodeAsManager(false);
   this->SetNodeAsDevice(false, NULL);

   this->UpdateToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the node item as CANopen Manager

   \param[in]       oq_Manager     Flag if CANopen Manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::SetNodeAsManager(const bool oq_Manager)
{
   if (this->mq_IsManager != oq_Manager)
   {
      this->mq_IsManager = oq_Manager;

      this->mpc_Ui->pc_LinkToManagerLabel->setVisible(oq_Manager);
      if (oq_Manager == true)
      {
         this->mpc_Ui->pc_LinkToManagerLabel->setText(
            C_Uti::h_GetLink(C_GtGetText::h_GetText("Manager"),
                             mc_STYLE_GUIDE_COLOR_6,
                             C_GtGetText::h_GetText("Manager")));

         this->UpdateToolTip();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the node item as CANopen Device

   \param[in]  oq_Device               Flag if CANopen Device
   \param[in]  opc_DeviceId            Device Id (NULL if not a device)
   \param[in]  ou32_ManagerNodeIndex   Index of Manager Node which the Device is assigned to (if oq_Device == true)
   \param[in]  ou32_ManagerIntfIndex   Interface index of Manager Node which the Device is
                                          assigned to (if oq_Device == true)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::SetNodeAsDevice(const bool oq_Device,
                                                            const stw::opensyde_core::C_OscCanInterfaceId * const opc_DeviceId, const uint32_t ou32_ManagerNodeIndex,
                                                            const uint32_t ou32_ManagerIntfIndex)
{
   if (this->mq_IsDevice != oq_Device)
   {
      this->mq_IsDevice = oq_Device;
      if ((this->mq_IsDevice == true) &&
          (opc_DeviceId != NULL))
      {
         this->mc_DeviceId = *opc_DeviceId;
         this->mu32_ManagerNodeIndex = ou32_ManagerNodeIndex;
         this->mu32_ManagerIntfIndex = ou32_ManagerIntfIndex;
      }

      this->UpdateToolTip();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the tool tip of the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::UpdateToolTip(void)
{
   uint32_t u32_NodeIndex;
   uint32_t u32_InterfaceIndex;
   const C_OscNode * pc_Node;
   const bool q_CanOpenActive = (this->me_ProtocolType == C_OscCanProtocol::eCAN_OPEN);

   this->mpc_CheckBox->GetIndexes(u32_NodeIndex, u32_InterfaceIndex);
   pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      // Prepare tooltip
      QString c_Content = "";
      uint32_t u32_RxMessageCount = 0U;
      uint32_t u32_TxMessageCount = 0U;
      uint32_t u32_SignalCount = 0U;
      const stw::opensyde_core::C_OscCanInterfaceId * pc_DeviceId = NULL;

      // Get the interface name
      c_Content += C_GtGetText::h_GetText("Interface: ");
      tgl_assert(u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
      if (u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         c_Content += C_PuiSdUtil::h_GetInterfaceName(
            pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceIndex].e_InterfaceType,
            pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceIndex].u8_InterfaceNumber);
      }

      // Begin the counting
      if (this->mq_IsDevice == true)
      {
         // Special case CANopen: It is a device, so the CANopen manager must be used for counting instead of the device
         u32_NodeIndex = this->mu32_ManagerNodeIndex;
         u32_InterfaceIndex = this->mu32_ManagerIntfIndex;
         pc_DeviceId = &this->mc_DeviceId;
      }

      if (C_SdUtil::h_GetMessageCountOfNode(u32_NodeIndex, u32_InterfaceIndex, this->me_ProtocolType, pc_DeviceId,
                                            u32_RxMessageCount, u32_TxMessageCount, &u32_SignalCount) != C_NO_ERR)
      {
         tgl_assert(false);
      }

      // Messages
      if ((u32_RxMessageCount > 0U) ||
          (u32_TxMessageCount > 0U))
      {
         c_Content += static_cast<QString>(C_GtGetText::h_GetText("\nMessages: %1 Tx, %2 Rx")).
                      arg(u32_TxMessageCount).arg(u32_RxMessageCount);
      }
      else
      {
         c_Content += C_GtGetText::h_GetText("\nNo Messages");
      }

      // Signals
      if (u32_SignalCount > 0U)
      {
         c_Content += static_cast<QString>(C_GtGetText::h_GetText("\nSignals: %1")).arg(u32_SignalCount);
      }
      else
      {
         c_Content += C_GtGetText::h_GetText("\nNo Signals");
      }

      // CANopen specific part
      if (q_CanOpenActive == true)
      {
         if (this->mq_IsManager == true)
         {
            c_Content += C_GtGetText::h_GetText("\nCANopen Role: Manager");
         }
         else if (this->mq_IsDevice == true)
         {
            c_Content += C_GtGetText::h_GetText("\nCANopen Role: Device");
         }
         else
         {
            // Nothing to do
         }
      }

      this->mpc_CheckBox->SetToolTipInformation(pc_Node->c_Properties.c_Name.c_str(), c_Content);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::m_NodeToggled(const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_InterfaceIndex, const bool oq_Checked)
{
   this->m_AdaptIcon(oq_Checked);

   Q_EMIT this->SigNodeToggled(ou32_NodeIndex, ou32_InterfaceIndex, oq_Checked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the icon of the check box

   \param[in]  oq_Checked  Flag if check box is checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::m_AdaptIcon(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->mpc_CheckBox->setIcon(this->mc_IconActive);
   }
   else
   {
      this->mpc_CheckBox->setIcon(this->mc_IconInactive);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal for switching to manager of this device

   \param[in]  orc_Link    Link
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::m_OnLinkSwitchToManager(const QString & orc_Link) const
{
   uint32_t u32_ManagerNodeIndex;
   uint32_t u32_ManagerIntfIndex;
   const C_OscNode * pc_ManagerNode;

   Q_UNUSED(orc_Link)

   this->GetIndexes(u32_ManagerNodeIndex, u32_ManagerIntfIndex);

   pc_ManagerNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ManagerNodeIndex);
   tgl_assert(pc_ManagerNode != NULL);
   if (pc_ManagerNode != NULL)
   {
      if (u32_ManagerIntfIndex < pc_ManagerNode->c_Properties.c_ComInterfaces.size())
      {
         Q_EMIT (this->SigSwitchToCoManager(u32_ManagerNodeIndex,
                                            static_cast<QString>(pc_ManagerNode->c_Properties.c_Name.c_str()),
                                            pc_ManagerNode->c_Properties.c_ComInterfaces[u32_ManagerIntfIndex].
                                            u8_InterfaceNumber));
      }
   }
}
