//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Reconnect bus connector to new bus undo command (implementation)

   Reconnect bus connector to new bus undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "TGLUtils.h"
#include "C_SdUtil.h"
#include "C_GiLiBus.h"
#include "C_SdManUnoTopologyReconnectBusCommand.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Scene              Pointer to currently active scene
   \param[in]      orc_IDs                Affected unique IDs
   \param[in]      oru64_StartingNodeID   Initial bus ID
   \param[in]      oru64_LastNodeID       New bus ID
   \param[in]      orc_ConnectionPos      Event position
   \param[in]      ors32_Interface        Interface to connect to
   \param[in]      orc_Properties         Properties
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyReconnectBusCommand::C_SdManUnoTopologyReconnectBusCommand(QGraphicsScene * const opc_Scene,
                                                                             const std::vector<uint64> & orc_IDs,
                                                                             const uint64 & oru64_StartingNodeID,
                                                                             const stw_types::uint64 & oru64_LastNodeID,
                                                                             const QPointF & orc_ConnectionPos,
                                                                             const stw_types::sint32 & ors32_Interface,
                                                                             const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                                                             QUndoCommand * const opc_Parent) :
   C_SdManUnoTopologyReconnectBaseCommand(opc_Scene, orc_IDs, oru64_StartingNodeID, oru64_LastNodeID, orc_ConnectionPos,
                                          ors32_Interface, orc_Properties, "Reconnect bus connetor(s) to new bus",
                                          opc_Parent)
{
   C_GiLiBusConnector * const pc_BusConnector = m_GetBusConnector();

   if (pc_BusConnector != NULL)
   {
      C_GiNode *  const pc_Node = pc_BusConnector->GetNodeItem();
      if (pc_Node != NULL)
      {
         const sint32 s32_NodeIndex = pc_Node->GetIndex();
         pc_Node->UpdateData();
         if (s32_NodeIndex >= 0)
         {
            const std::vector<uint32> c_NodeIndices =
               C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
                  static_cast<uint32>(s32_NodeIndex));
            this->mc_InitialProperties.clear();
            this->mc_InitialProperties.reserve(c_NodeIndices.size());
            for (uint32 u32_ItNode = 0UL; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
            {
               const stw_opensyde_core::C_OSCNode * const pc_NodeData =
                  C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndices[u32_ItNode]);
               if (pc_NodeData != NULL)
               {
                  const C_PuiSdNodeConnectionId * const pc_ConnectionId = pc_BusConnector->GetConnectionData();
                  if (pc_ConnectionId != NULL)
                  {
                     const C_OSCNodeComInterfaceSettings * const pc_ComInterface =
                        pc_NodeData->c_Properties.GetComInterface(pc_ConnectionId->e_InterfaceType,
                                                                  pc_ConnectionId->u8_InterfaceNumber);
                     this->mu8_InitialInterface = pc_ConnectionId->u8_InterfaceNumber;
                     if (pc_ComInterface != NULL)
                     {
                        C_PuiSdNodeInterfaceAutomaticProperties c_Property;
                        c_Property.c_IP.reserve(4);
                        for (uint32 u32_It = 0UL; u32_It < 4; ++u32_It)
                        {
                           c_Property.c_IP.push_back(pc_ComInterface->c_Ip.au8_IpAddress[u32_It]);
                        }
                        c_Property.u8_NodeId = pc_ComInterface->u8_NodeID;
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

   \param[in]  oru64_StartingID  ID of initial bus
   \param[in]  oru64_LastID      ID of new bus
   \param[in]  ors32_Interface   Interface number to use
   \param[in]  orc_Properties    Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyReconnectBusCommand::m_Reconnect(const uint64 & oru64_StartingID, const uint64 & oru64_LastID,
                                                        const sint32 & ors32_Interface,
                                                        const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   C_GiLiBus * const pc_StartingBus = dynamic_cast<C_GiLiBus *>(m_GetSceneItem(oru64_StartingID));
   C_GiLiBus * const pc_LastBus = dynamic_cast<C_GiLiBus *>(m_GetSceneItem(oru64_LastID));
   C_GiLiBusConnector * const pc_BusConnector = m_GetBusConnector();

   if (pc_BusConnector != NULL)
   {
      pc_BusConnector->Reconnect(pc_StartingBus, pc_LastBus, this->mc_ConnectionPos, ors32_Interface, orc_Properties);
   }
}
