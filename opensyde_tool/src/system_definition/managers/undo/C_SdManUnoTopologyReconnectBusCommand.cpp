//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Reconnect bus connector to new bus undo command (implementation)

   Reconnect bus connector to new bus undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "C_SdUtil.hpp"
#include "C_GiLiBus.hpp"
#include "C_SdManUnoTopologyReconnectBusCommand.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Scene              Pointer to currently active scene
   \param[in]      orc_Ids                Affected unique IDs
   \param[in]      oru64_StartingNodeId   Initial bus ID
   \param[in]      oru64_LastNodeId       New bus ID
   \param[in]      orc_ConnectionPos      Event position
   \param[in]      ors32_Interface        Interface to connect to
   \param[in]      orc_Properties         Properties
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyReconnectBusCommand::C_SdManUnoTopologyReconnectBusCommand(QGraphicsScene * const opc_Scene,
                                                                             const std::vector<uint64_t> & orc_Ids,
                                                                             const uint64_t & oru64_StartingNodeId,
                                                                             const uint64_t & oru64_LastNodeId,
                                                                             const QPointF & orc_ConnectionPos,
                                                                             const int32_t & ors32_Interface,
                                                                             const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                                                             QUndoCommand * const opc_Parent) :
   C_SdManUnoTopologyReconnectBaseCommand(opc_Scene, orc_Ids, oru64_StartingNodeId, oru64_LastNodeId, orc_ConnectionPos,
                                          ors32_Interface, orc_Properties, "Reconnect bus connetor(s) to new bus",
                                          opc_Parent)
{
   C_GiLiBusConnector * const pc_BusConnector = m_GetBusConnector();

   if (pc_BusConnector != NULL)
   {
      C_GiNode *  const pc_Node = pc_BusConnector->GetNodeItem();
      if (pc_Node != NULL)
      {
         const int32_t s32_NodeIndex = pc_Node->GetIndex();
         pc_Node->UpdateData();
         if (s32_NodeIndex >= 0)
         {
            const std::vector<uint32_t> c_NodeIndices =
               C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
                  static_cast<uint32_t>(s32_NodeIndex));
            this->mc_InitialProperties.clear();
            this->mc_InitialProperties.reserve(c_NodeIndices.size());
            for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
            {
               const stw::opensyde_core::C_OscNode * const pc_NodeData =
                  C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndices[u32_ItNode]);
               if (pc_NodeData != NULL)
               {
                  const C_PuiSdNodeConnectionId * const pc_ConnectionId = pc_BusConnector->GetConnectionData();
                  if (pc_ConnectionId != NULL)
                  {
                     const C_OscNodeComInterfaceSettings * const pc_ComInterface =
                        pc_NodeData->c_Properties.GetComInterface(pc_ConnectionId->e_InterfaceType,
                                                                  pc_ConnectionId->u8_InterfaceNumber);
                     this->mu8_InitialInterface = pc_ConnectionId->u8_InterfaceNumber;
                     if (pc_ComInterface != NULL)
                     {
                        C_PuiSdNodeInterfaceAutomaticProperties c_Property;
                        c_Property.c_Ip.reserve(4);
                        for (uint32_t u32_It = 0UL; u32_It < 4; ++u32_It)
                        {
                           c_Property.c_Ip.push_back(pc_ComInterface->c_Ip.au8_IpAddress[u32_It]);
                        }
                        c_Property.u8_NodeId = pc_ComInterface->u8_NodeId;
                        this->mc_InitialProperties.push_back(c_Property);
                     }
                  }
               }
            }
            tgl_assert(this->mc_InitialProperties.size() == c_NodeIndices.size());
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyReconnectBusCommand::~C_SdManUnoTopologyReconnectBusCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect from specified bus to specified bus using specified interface

   \param[in]  oru64_StartingId  ID of initial bus
   \param[in]  oru64_LastId      ID of new bus
   \param[in]  ors32_Interface   Interface number to use
   \param[in]  orc_Properties    Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyReconnectBusCommand::m_Reconnect(const uint64_t & oru64_StartingId,
                                                        const uint64_t & oru64_LastId, const int32_t & ors32_Interface,
                                                        const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   C_GiLiBus * const pc_StartingBus = dynamic_cast<C_GiLiBus *>(m_GetSceneItem(oru64_StartingId));
   C_GiLiBus * const pc_LastBus = dynamic_cast<C_GiLiBus *>(m_GetSceneItem(oru64_LastId));
   C_GiLiBusConnector * const pc_BusConnector = m_GetBusConnector();

   if (pc_BusConnector != NULL)
   {
      pc_BusConnector->Reconnect(pc_StartingBus, pc_LastBus, this->mc_ConnectionPos, ors32_Interface, orc_Properties);
   }
}
