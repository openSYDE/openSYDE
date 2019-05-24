//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add undo command (implementation)

   Add undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdManUnoTopologyAddCommand.h"
#include "C_SdTopologyScene.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor for adding a generic element

   \param[in,out] opc_Scene                         Pointer to currently active scene
   \param[in]     orc_IDs                           Affected unique IDs
   \param[in]     ore_Type                          Item type
   \param[in]     orc_NewPos                        New position
   \param[in]     orc_AdditionalInformation         Additional string information
   \param[in,out] opc_Parent                        Optional pointer to parent
   \param[in]     orq_ForceUseAdditionalInformation Optional flag to force use of additional string information
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddCommand::C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene,
                                                           const std::vector<stw_types::uint64> & orc_IDs,
                                                           const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                                           const QString & orc_AdditionalInformation,
                                                           QUndoCommand * const opc_Parent,
                                                           const bool & orq_ForceUseAdditionalInformation) :
   C_SdManUnoTopologyAddBaseCommand(opc_Scene, orc_IDs, "Add drawing element(s)",
                                    opc_Parent),
   me_Type(ore_Type),
   mc_NewPos(orc_NewPos),
   mc_AdditionalInformation(orc_AdditionalInformation),
   mu64_BusConnectorNodeID(0),
   mu64_BusConnectorBusID(0),
   mu8_InterfaceNumber(0),
   mu8_NodeId(0),
   mq_ForceUseAdditionalInformation(orq_ForceUseAdditionalInformation)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor for adding a bus connector

   \param[in,out] opc_Scene                 Pointer to currently active scene
   \param[in]     orc_IDs                   Affected unique IDs
   \param[in]     ore_Type                  Item type
   \param[in]     orc_NewPos                New position
   \param[in]     oru64_BusConnectorNodeID  Node ID for bus connector creation
   \param[in]     oru64_BusConnectorBusID   Bus ID for bus connector creation
   \param[in]     oru8_InterfaceNumber      Interface number for bus connector creation
   \param[in]     oru8_NodeId               New node id
   \param[in,out] opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddCommand::C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene,
                                                           const std::vector<stw_types::uint64> & orc_IDs,
                                                           const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                                           const stw_types::uint64 & oru64_BusConnectorNodeID,
                                                           const stw_types::uint64 & oru64_BusConnectorBusID,
                                                           const stw_types::uint8 & oru8_InterfaceNumber,
                                                           const stw_types::uint8 & oru8_NodeId,
                                                           QUndoCommand * const opc_Parent) :
   C_SdManUnoTopologyAddBaseCommand(opc_Scene, orc_IDs, "Add drawing element(s)",
                                    opc_Parent),
   me_Type(ore_Type),
   mc_NewPos(orc_NewPos),
   mu64_BusConnectorNodeID(oru64_BusConnectorNodeID),
   mu64_BusConnectorBusID(oru64_BusConnectorBusID),
   mu8_InterfaceNumber(oru8_InterfaceNumber),
   mu8_NodeId(oru8_NodeId),
   mq_ForceUseAdditionalInformation(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddCommand::~C_SdManUnoTopologyAddCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add elements without previous knowledge
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddCommand::m_AddNew(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdTopologyScene * const pc_Scene = dynamic_cast<C_SdTopologyScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      const std::vector<stw_types::uint64> c_IDs = this->GetIDs();
      if (c_IDs.size() > 0)
      {
         C_GiNode * pc_Node;
         C_GiLiBus * pc_Bus;
         C_GiTextElementBus * pc_TextElementBus;

         switch (this->me_Type)
         {
         case E_ElementType::eNODE:
            pc_Scene->AddNode(this->mc_AdditionalInformation, this->mc_NewPos, &(c_IDs[0]));
            break;
         case E_ElementType::eCAN_BUS:
            pc_TextElementBus = pc_Scene->AddTextElementBus(&(c_IDs[1]));
            if (this->mq_ForceUseAdditionalInformation == true)
            {
               pc_Scene->AddCanBus(this->mc_NewPos, &(c_IDs[0]), pc_TextElementBus, &this->mc_AdditionalInformation);
            }
            else
            {
               pc_Scene->AddCanBus(this->mc_NewPos, &(c_IDs[0]), pc_TextElementBus);
            }
            break;
         case E_ElementType::eETHERNET_BUS:
            pc_TextElementBus = pc_Scene->AddTextElementBus(&(c_IDs[1]));
            if (this->mq_ForceUseAdditionalInformation == true)
            {
               pc_Scene->AddEthernetBus(this->mc_NewPos, &(c_IDs[0]), pc_TextElementBus,
                                        &this->mc_AdditionalInformation);
            }
            else
            {
               pc_Scene->AddEthernetBus(this->mc_NewPos, &(c_IDs[0]), pc_TextElementBus);
            }
            break;
         case E_ElementType::eLINE_ARROW:
            pc_Scene->AddLine(this->mc_NewPos, &(c_IDs[0]));
            break;
         case E_ElementType::eBOUNDARY:
            pc_Scene->AddBoundary(this->mc_NewPos, &(c_IDs[0]));
            break;
         case E_ElementType::eTEXT_ELEMENT:
            pc_Scene->AddTextElement(this->mc_AdditionalInformation, this->mc_NewPos, &(c_IDs[0]));
            break;
         case E_ElementType::eIMAGE:
            pc_Scene->AddImage(this->mc_AdditionalInformation, this->mc_NewPos, &(c_IDs[0]));
            break;
         case E_ElementType::eBUS_CONNECTOR:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Node = dynamic_cast<C_GiNode *>(m_GetSceneItem(this->mu64_BusConnectorNodeID));
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Bus = dynamic_cast<C_GiLiBus *>(m_GetSceneItem(this->mu64_BusConnectorBusID));
            if ((pc_Node != NULL) && (pc_Bus != NULL))
            {
               if (pc_Node->CheckInterfaceAvailable(pc_Bus->GetType(), this->mu8_InterfaceNumber) == true)
               {
                  pc_Scene->AddBusConnector(pc_Node, pc_Bus, this->mu8_InterfaceNumber, this->mu8_NodeId,
                                            this->mc_NewPos, &(c_IDs[0]));
               }
            }
            break;
         case E_ElementType::eUNKNOWN:
         default:
            //No element to create
            break;
         }
      }
   }
}
