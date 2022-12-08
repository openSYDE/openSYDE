//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting nodes (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <vector>

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "C_SdBueNodeSelectorWidget.hpp"
#include "ui_C_SdBueNodeSelectorWidget.h"

#include "C_OscNode.hpp"
#include "C_OscSystemBus.hpp"
#include "C_PuiSdHandler.hpp"
#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_SdUtil.hpp"
#include "C_CieUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

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
C_SdBueNodeSelectorWidget::C_SdBueNodeSelectorWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueNodeSelectorWidget),
   mu32_BusIndex(0U),
   me_Protocol(C_OscCanProtocol::eLAYER2)
{
   mpc_Ui->setupUi(this);

   this->InitStaticNames();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueNodeSelectorWidget::~C_SdBueNodeSelectorWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_BueNodeSelectorTitleLabel->setText(C_GtGetText::h_GetText("Active Protocol Nodes"));
   this->mpc_Ui->pc_LabelNoNodes->setText(C_GtGetText::h_GetText("No nodes connected to this bus"));

   //tooltips
   this->mpc_Ui->pc_BueNodeSelectorTitleLabel->SetToolTipInformation(C_GtGetText::h_GetText("Active Protocol Nodes"),
                                                                     C_GtGetText::h_GetText(
                                                                        "This list shows all physical connection to "
                                                                        "this bus."
                                                                        "\nChecked connections are active participants "
                                                                        "of current selected CAN protocol."
                                                                        "\nA Datapool is created automatically on check"
                                                                        " if there is no COMM Datapool already declared"
                                                                        " on this node."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the bus id to load all connected nodes

   \param[in]     ou32_BusIndex     Bus id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::SetBusId(const uint32_t ou32_BusIndex)
{
   std::vector<uint32_t> c_NodeIndexes;
   std::vector<uint32_t> c_InterfaceIndexes;
   std::vector<QString> c_NodeNames;

   // save the bus index
   this->mu32_BusIndex = ou32_BusIndex;

   C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndexes,
                                                                                      c_InterfaceIndexes);
   if (C_SdUtil::h_GetNames(c_NodeIndexes, c_InterfaceIndexes, c_NodeNames, true) == C_NO_ERR)
   {
      this->mpc_Ui->pc_NodeSelectorListWidget->AddNodes(c_NodeNames, c_NodeIndexes,
                                                        c_InterfaceIndexes);
   }

   if (c_NodeNames.size() > 0)
   {
      this->mpc_Ui->pc_LabelNoNodes->setVisible(false);
      this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
      this->mpc_Ui->pc_VerticalSpacerTop->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
      this->mpc_Ui->pc_NodeSelectorListWidget->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_NodeSelectorListWidget->setVisible(false);
      this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 40, QSizePolicy::Fixed, QSizePolicy::Expanding);
      this->mpc_Ui->pc_VerticalSpacerTop->changeSize(0, 10, QSizePolicy::Fixed, QSizePolicy::Fixed);
      this->mpc_Ui->pc_LabelNoNodes->setVisible(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the actual protocol to mark the protocol using nodes

   \param[in]     oe_Protocol     Protocol id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::SetProtocol(const C_OscCanProtocol::E_Type oe_Protocol)
{
   uint32_t u32_NodeCounter;

   std::vector<uint32_t> c_NodeIndexes;
   std::vector<uint32_t> c_InterfaceIndexes;
   std::vector<uint32_t> c_NodeIndexesWithInterfaceDuplicates;
   std::vector<uint32_t> c_ReducedInterfaceIndexes;

   disconnect(this->mpc_Ui->pc_NodeSelectorListWidget, &C_SdBueNodeSelectorCheckBoxListWidget::SigNodeToggled,
              this, &C_SdBueNodeSelectorWidget::m_NodeToggled);
   disconnect(this->mpc_Ui->pc_NodeSelectorListWidget, &C_SdBueNodeSelectorCheckBoxListWidget::SigSwitchToCoManager,
              this, &C_SdBueNodeSelectorWidget::SigSwitchToCoManager);

   // save the protocol
   this->me_Protocol = oe_Protocol;

   C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_BusIndex,
                                                                                      c_NodeIndexes,
                                                                                      c_InterfaceIndexes);

   if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
   {
      // get all the nodes using the protocol
      for (u32_NodeCounter = 0U; u32_NodeCounter < c_NodeIndexes.size(); ++u32_NodeCounter)
      {
         const C_OscNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndexes[u32_NodeCounter]);

         if (pc_Node != NULL)
         {
            // get the protocols
            std::vector<const C_OscCanProtocol *> c_Protocols = pc_Node->GetCanProtocolsConst(oe_Protocol);
            uint32_t u32_Counter;

            for (u32_Counter = 0U; u32_Counter < c_Protocols.size(); ++u32_Counter)
            {
               const C_OscCanProtocol * const pc_Protcol = c_Protocols[u32_Counter];
               if (c_InterfaceIndexes[u32_NodeCounter] < pc_Protcol->c_ComMessages.size())
               {
                  const C_OscCanMessageContainer & rc_MessageContainer =
                     pc_Protcol->c_ComMessages[c_InterfaceIndexes[u32_NodeCounter]];
                  // check which interfaces are using the protocol
                  if (rc_MessageContainer.q_IsComProtocolUsedByInterface == true)
                  {
                     // the indexes of the nodes must map to the indexes of the interfaces
                     // use a second vector to create the mapping
                     c_NodeIndexesWithInterfaceDuplicates.push_back(c_NodeIndexes[u32_NodeCounter]);
                     c_ReducedInterfaceIndexes.push_back(c_InterfaceIndexes[u32_NodeCounter]);
                  }
               }
            }
         }
      }
   }

   this->mpc_Ui->pc_NodeSelectorListWidget->SetProtocol(oe_Protocol);

   if (oe_Protocol == C_OscCanProtocol::eCAN_OPEN)
   {
      // Special case CANopen: The CANopen Manager on this bus gets a link
      uint32_t u32_ManagerNodeIndex;
      uint8_t u8_ManagerIntfNumber;
      if (C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerNodeOnBus(this->mu32_BusIndex, u32_ManagerNodeIndex,
                                                                      &u8_ManagerIntfNumber) == C_NO_ERR)
      {
         const C_OscNode * const pc_NodeManager =
            C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ManagerNodeIndex);

         if (pc_NodeManager != NULL)
         {
            uint32_t u32_IntfCounter;
            uint32_t u32_ManagerIntfIndex = 0U;
            // The devices have to be added manually as checked due to not having own protocol information about
            // CANopen. Only the manager has all information.
            const std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManagerInfo =
               pc_NodeManager->c_CanOpenManagers.find(u8_ManagerIntfNumber);

            // Set the info for the manager
            // Getting the interface index by the interface number
            for (u32_IntfCounter = 0U; u32_IntfCounter < pc_NodeManager->c_Properties.c_ComInterfaces.size();
                 ++u32_IntfCounter)
            {
               if (pc_NodeManager->c_Properties.c_ComInterfaces[u32_IntfCounter].u8_InterfaceNumber ==
                   u8_ManagerIntfNumber)
               {
                  // Found
                  this->mpc_Ui->pc_NodeSelectorListWidget->SetSpecificNodeAsManager(u32_ManagerNodeIndex,
                                                                                    u32_IntfCounter,
                                                                                    true);
                  u32_ManagerIntfIndex = u32_IntfCounter;
                  break;
               }
            }

            tgl_assert(c_ItManagerInfo != pc_NodeManager->c_CanOpenManagers.end());
            if (c_ItManagerInfo != pc_NodeManager->c_CanOpenManagers.end())
            {
               const C_OscCanOpenManagerInfo & rc_ManagerInfo = c_ItManagerInfo->second;
               std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice;

               for (c_ItDevice = rc_ManagerInfo.c_CanOpenDevices.begin();
                    c_ItDevice != rc_ManagerInfo.c_CanOpenDevices.end(); ++c_ItDevice)
               {
                  const C_OscCanInterfaceId & rc_DeviceId = c_ItDevice->first;
                  const C_OscNode * const pc_DeviceNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
                     rc_DeviceId.u32_NodeIndex);

                  if (pc_DeviceNode != NULL)
                  {
                     for (u32_IntfCounter = 0U;
                          u32_IntfCounter < pc_DeviceNode->c_Properties.c_ComInterfaces.size();
                          ++u32_IntfCounter)
                     {
                        if (pc_DeviceNode->c_Properties.c_ComInterfaces[u32_IntfCounter].u8_InterfaceNumber ==
                            rc_DeviceId.u8_InterfaceNumber)
                        {
                           // Specific interface found
                           this->mpc_Ui->pc_NodeSelectorListWidget->SetSpecificNodeAsDevice(rc_DeviceId.u32_NodeIndex,
                                                                                            u32_IntfCounter,
                                                                                            true,
                                                                                            &rc_DeviceId,
                                                                                            u32_ManagerNodeIndex,
                                                                                            u32_ManagerIntfIndex);
                           // Add the CANopen devices to the checked nodes
                           c_NodeIndexesWithInterfaceDuplicates.push_back(rc_DeviceId.u32_NodeIndex);
                           c_ReducedInterfaceIndexes.push_back(u32_IntfCounter);
                           break;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   // Set the checkbox state
   this->mpc_Ui->pc_NodeSelectorListWidget->CheckNodes(c_NodeIndexesWithInterfaceDuplicates,
                                                       c_ReducedInterfaceIndexes);

   connect(this->mpc_Ui->pc_NodeSelectorListWidget, &C_SdBueNodeSelectorCheckBoxListWidget::SigNodeToggled,
           this, &C_SdBueNodeSelectorWidget::m_NodeToggled);
   connect(this->mpc_Ui->pc_NodeSelectorListWidget, &C_SdBueNodeSelectorCheckBoxListWidget::SigSwitchToCoManager,
           this, &C_SdBueNodeSelectorWidget::SigSwitchToCoManager);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates in case of changed the items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::Refresh(void) const
{
   this->mpc_Ui->pc_NodeSelectorListWidget->Refresh();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::m_NodeToggled(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                              const bool oq_Checked) const
{
   if (oq_Checked == true)
   {
      // protocol will be used by this node on this bus
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

      if (pc_Node != NULL)
      {
         // check if a protocol and therefore a Datapool exist
         const bool q_ProtocolAndDbExists = (pc_Node->GetCanProtocolsConst(this->me_Protocol).size() > 0U);

         if (q_ProtocolAndDbExists == true)
         {
            // No new datapool necessary
            Q_EMIT (this->SigConnectNodeToProt(ou32_NodeIndex, ou32_InterfaceIndex));
         }
         else
         {
            // No COM datapool yet. New one must be created if permissible
            if (C_SdUtil::h_CheckDatapoolNumber(ou32_NodeIndex, false, this->parentWidget()) == true)
            {
               Q_EMIT (this->SigConnectNodeToProtAndAddDataPool(ou32_NodeIndex, ou32_InterfaceIndex));
            }
            else // No more datapools can be added
            {
               //Uncheck checkbox
               this->mpc_Ui->pc_NodeSelectorListWidget->CheckSpecificNode(ou32_NodeIndex, ou32_InterfaceIndex, false);
            }
         }
      }
   }
   else
   {
      // Disconnect the COM datapool of the node
      Q_EMIT (this->SigDisconnectNodeFromProt(ou32_NodeIndex, ou32_InterfaceIndex));
   }
}
