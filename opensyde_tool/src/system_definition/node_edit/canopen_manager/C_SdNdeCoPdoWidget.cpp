//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager

   Widget for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "TglUtils.hpp"
#include "C_SdNdeCoPdoWidget.hpp"
#include "ui_C_SdNdeCoPdoWidget.h"
#include "C_OscCanInterfaceId.hpp"
#include "C_OscNode.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoPdoWidget::C_SdNdeCoPdoWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoPdoWidget),
   mu32_ManagerNodeIndex(0UL),
   mu8_ManagerInterfaceId(0U)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // bus link
   this->mpc_Ui->pc_LinkToBusLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
   this->mpc_Ui->pc_LinkToBusLabel->setOpenExternalLinks(false);
   this->mpc_Ui->pc_LinkToBusLabel->setFocusPolicy(Qt::NoFocus);
   connect(this->mpc_Ui->pc_LinkToBusLabel, &QLabel::linkActivated, this,
           &C_SdNdeCoPdoWidget::m_OnLinkSwitchToBus);
   connect(this->mpc_Ui->pc_PdoTableView, &C_SdNdeCoPdoTableView::SigSwitchToBusProtocolMessage, this,
           &C_SdNdeCoPdoWidget::m_OnLinkSwitchToBusProtocolMessage);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoPdoWidget::~C_SdNdeCoPdoWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoWidget::LoadUserSettings() const
{
   this->mpc_Ui->pc_PdoTableView->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoWidget::SaveUserSettings() const
{
   this->mpc_Ui->pc_PdoTableView->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoWidget::InitStaticNames() const
{
   this->mpc_Ui->pc_LabelNoChannels->setText(C_GtGetText::h_GetText("No PDO data for node found."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoWidget::UpdateData(void)
{
   this->mpc_Ui->pc_PdoTableView->SetNodeIndexAndInterfaceId(this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId,
                                                             this->mc_DeviceInterfaceId);
   this->mpc_Ui->pc_PdoTableView->UpdateData();
   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Node Index and Inteface ID setter

   Sets the private node index and interface id

   \param[in]  ou32_ManagerNodeIndex     new node index
   \param[in]  ou8_ManagerInterfaceId    new interface index
   \param[in]  orc_DeviceNodeId          new Node ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoWidget::SetNodeIndexAndInterfaceId(const uint32_t ou32_ManagerNodeIndex,
                                                    const uint8_t ou8_ManagerInterfaceId,
                                                    const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId)
{
   bool q_DeviceChanged = false;

   if (this->mc_DeviceInterfaceId.u32_NodeIndex != orc_DeviceNodeId.u32_NodeIndex)
   {
      // Save the user settings for each device
      q_DeviceChanged = true;
      this->SaveUserSettings();
   }

   this->mu32_ManagerNodeIndex = ou32_ManagerNodeIndex;
   this->mu8_ManagerInterfaceId = ou8_ManagerInterfaceId;
   this->mc_DeviceInterfaceId = orc_DeviceNodeId;

   //load node data
   this->UpdateData();

   if (q_DeviceChanged == true)
   {
      this->LoadUserSettings();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update UI
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoWidget::m_UpdateUi()
{
   if (this->mpc_Ui->pc_PdoTableView->GetCountRows() > 0)
   {
      this->mpc_Ui->pc_LabelNoChannels->setVisible(false);
      this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
      this->mpc_Ui->pc_PdoTableView->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_PdoTableView->setVisible(false);
      this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 40, QSizePolicy::Fixed, QSizePolicy::Expanding);
      this->mpc_Ui->pc_LabelNoChannels->setVisible(true);
   }

   // set bus link
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_ManagerNodeIndex);

   if ((pc_Node != NULL) &&
       (this->mu8_ManagerInterfaceId < pc_Node->c_Properties.c_ComInterfaces.size()))
   {
      const C_OscNodeComInterfaceSettings & rc_ComInterface =
         pc_Node->c_Properties.c_ComInterfaces[this->mu8_ManagerInterfaceId];

      if (rc_ComInterface.GetBusConnected() == true)
      {
         const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
            rc_ComInterface.u32_BusIndex);
         QString c_BusName;

         // Get bus name for the link and the interface combo box
         if (pc_Bus != NULL)
         {
            c_BusName = pc_Bus->c_Name.c_str();
         }

         this->mpc_Ui->pc_LinkToBusLabel->setText(
            C_Uti::h_GetLink(c_BusName + " - CANopen Messages",
                             mc_STYLE_GUIDE_COLOR_6,
                             c_BusName));
      }
   }

   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal for switching to bus

   \param[in]  orc_Link    Link
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoWidget::m_OnLinkSwitchToBus(const QString & orc_Link) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_ManagerNodeIndex);

   if ((pc_Node != NULL) &&
       (this->mu8_ManagerInterfaceId < pc_Node->c_Properties.c_ComInterfaces.size()))
   {
      const C_OscNodeComInterfaceSettings & rc_ComInterface =
         pc_Node->c_Properties.c_ComInterfaces[this->mu8_ManagerInterfaceId];

      if (rc_ComInterface.GetBusConnected() == true)
      {
         Q_EMIT (this->SigSwitchToBusProtocol(rc_ComInterface.u32_BusIndex, orc_Link, C_OscCanProtocol::eCAN_OPEN));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal for switching to bus

   \param[in]  orc_MessageId    Message id to jump to in bus COMM messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoWidget::m_OnLinkSwitchToBusProtocolMessage(
   const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_ManagerNodeIndex);

   if ((pc_Node != NULL) &&
       (this->mu8_ManagerInterfaceId < pc_Node->c_Properties.c_ComInterfaces.size()))
   {
      const C_OscNodeComInterfaceSettings & rc_ComInterface =
         pc_Node->c_Properties.c_ComInterfaces[this->mu8_ManagerInterfaceId];

      if (rc_ComInterface.GetBusConnected() == true)
      {
         const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(rc_ComInterface.u32_BusIndex);
         tgl_assert(pc_Bus != NULL);
         if (pc_Bus != NULL)
         {
            Q_EMIT (this->SigSwitchToBusProtocolMessage(rc_ComInterface.u32_BusIndex,
                                                        static_cast<QString>(pc_Bus->c_Name.c_str()), orc_MessageId));
         }
      }
   }
}
