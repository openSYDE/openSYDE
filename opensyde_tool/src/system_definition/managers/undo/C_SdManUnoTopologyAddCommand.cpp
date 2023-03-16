//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add undo command (implementation)

   Add undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdManUnoTopologyAddCommand.hpp"
#include "C_SdTopologyScene.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor for adding a generic element

   \param[in,out]  opc_Scene                          Pointer to currently active scene
   \param[in]      orc_Ids                            Affected unique IDs
   \param[in]      ore_Type                           Item type
   \param[in]      orc_NewPos                         New position
   \param[in]      orc_AdditionalInformation          Additional string information
   \param[in,out]  opc_Parent                         Optional pointer to parent
   \param[in]      orq_ForceUseAdditionalInformation  Optional flag to force use of additional string information
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddCommand::C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene,
                                                           const std::vector<uint64_t> & orc_Ids,
                                                           const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                                           const QString & orc_AdditionalInformation,
                                                           QUndoCommand * const opc_Parent,
                                                           const bool & orq_ForceUseAdditionalInformation) :
   C_SdManUnoTopologyAddBaseCommand(opc_Scene, orc_Ids, "Add drawing element(s)",
                                    opc_Parent),
   me_Type(ore_Type),
   mc_NewPos(orc_NewPos),
   mc_AdditionalInformation(orc_AdditionalInformation),
   mu64_BusConnectorNodeId(0),
   mu64_BusConnectorBusId(0),
   mu8_InterfaceNumber(0),
   mc_Properties(),
   mq_ForceUseAdditionalInformation(orq_ForceUseAdditionalInformation)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor for adding a bus connector

   \param[in,out]  opc_Scene                 Pointer to currently active scene
   \param[in]      orc_Ids                   Affected unique IDs
   \param[in]      ore_Type                  Item type
   \param[in]      orc_NewPos                New position
   \param[in]      ou64_BusConnectorNodeId   Bus connector node ID
   \param[in]      ou64_BusConnectorBusId    Bus connector bus ID
   \param[in]      ou8_InterfaceNumber       Interface number
   \param[in]      orc_Properties            Properties
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddCommand::C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene,
                                                           const std::vector<uint64_t> & orc_Ids,
                                                           const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                                           const uint64_t ou64_BusConnectorNodeId,
                                                           const uint64_t ou64_BusConnectorBusId,
                                                           const uint8_t ou8_InterfaceNumber,
                                                           const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                                           QUndoCommand * const opc_Parent) :
   C_SdManUnoTopologyAddBaseCommand(opc_Scene, orc_Ids, "Add drawing element(s)",
                                    opc_Parent),
   me_Type(ore_Type),
   mc_NewPos(orc_NewPos),
   mu64_BusConnectorNodeId(ou64_BusConnectorNodeId),
   mu64_BusConnectorBusId(ou64_BusConnectorBusId),
   mu8_InterfaceNumber(ou8_InterfaceNumber),
   mc_Properties(orc_Properties),
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
   C_SdTopologyScene * const pc_Scene = dynamic_cast<C_SdTopologyScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      const std::vector<uint64_t> c_Ids = this->m_GetIds();
      if (c_Ids.size() > 0)
      {
         C_GiNode * pc_Node;
         C_GiLiBus * pc_Bus;
         float64_t f64_BusZetValue;
         C_GiTextElementBus * pc_TextElementBus;

         switch (this->me_Type)
         {
         case E_ElementType::eNODE:
            pc_Scene->AddNode(this->mc_AdditionalInformation, this->mc_NewPos, &(c_Ids[0]));
            break;
         case E_ElementType::eCAN_BUS:
            pc_TextElementBus = pc_Scene->AddTextElementBus(&(c_Ids[1]), f64_BusZetValue);
            if (this->mq_ForceUseAdditionalInformation == true)
            {
               pc_Scene->AddCanBus(this->mc_NewPos, &(c_Ids[0]), f64_BusZetValue, pc_TextElementBus,
                                   &this->mc_AdditionalInformation);
            }
            else
            {
               pc_Scene->AddCanBus(this->mc_NewPos, &(c_Ids[0]), f64_BusZetValue, pc_TextElementBus);
            }
            break;
         case E_ElementType::eETHERNET_BUS:
            pc_TextElementBus = pc_Scene->AddTextElementBus(&(c_Ids[1]), f64_BusZetValue);
            if (this->mq_ForceUseAdditionalInformation == true)
            {
               pc_Scene->AddEthernetBus(this->mc_NewPos, &(c_Ids[0]), f64_BusZetValue, pc_TextElementBus,
                                        &this->mc_AdditionalInformation);
            }
            else
            {
               pc_Scene->AddEthernetBus(this->mc_NewPos, &(c_Ids[0]), f64_BusZetValue, pc_TextElementBus);
            }
            break;
         case E_ElementType::eLINE_ARROW:
            pc_Scene->AddLine(this->mc_NewPos, &(c_Ids[0]));
            break;
         case E_ElementType::eBOUNDARY:
            pc_Scene->AddBoundary(this->mc_NewPos, &(c_Ids[0]));
            break;
         case E_ElementType::eTEXT_ELEMENT:
            pc_Scene->AddTextElement(this->mc_AdditionalInformation, this->mc_NewPos, &(c_Ids[0]));
            break;
         case E_ElementType::eIMAGE:
            pc_Scene->AddImage(this->mc_AdditionalInformation, this->mc_NewPos, &(c_Ids[0]));
            break;
         case E_ElementType::eBUS_CONNECTOR:

            pc_Node = dynamic_cast<C_GiNode *>(m_GetSceneItem(this->mu64_BusConnectorNodeId));

            pc_Bus = dynamic_cast<C_GiLiBus *>(m_GetSceneItem(this->mu64_BusConnectorBusId));
            if ((pc_Node != NULL) && (pc_Bus != NULL))
            {
               if (pc_Node->CheckInterfaceAvailable(pc_Bus->GetType(), this->mu8_InterfaceNumber) == true)
               {
                  pc_Scene->AddBusConnector(pc_Node, pc_Bus, this->mu8_InterfaceNumber, this->mc_Properties,
                                            this->mc_NewPos, &(c_Ids[0]));
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
