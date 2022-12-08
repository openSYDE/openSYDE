//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Reconnect bus connector to new node undo command (implementation)

   Reconnect bus connector to new node undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdManUnoTopologyReconnectNodeCommand.hpp"
#include "C_SdManUnoTopologyDeleteCommand.hpp"
#include "C_GiNode.hpp"
#include "C_SdUtil.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace std;

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
   \param[in]      oru64_StartingNodeId   Initial node ID
   \param[in]      oru64_LastNodeId       New node ID
   \param[in]      orc_ConnectionPos      Event position
   \param[in]      ors32_Interface        Interface to connect to
   \param[in]      orc_Properties         Properties
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyReconnectNodeCommand::C_SdManUnoTopologyReconnectNodeCommand(QGraphicsScene * const opc_Scene,
                                                                               const vector<uint64_t> & orc_Ids,
                                                                               const uint64_t & oru64_StartingNodeId,
                                                                               const uint64_t & oru64_LastNodeId,
                                                                               const QPointF & orc_ConnectionPos,
                                                                               const int32_t & ors32_Interface,
                                                                               const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                                                               QUndoCommand * const opc_Parent) :
   C_SdManUnoTopologyReconnectBaseCommand(opc_Scene, orc_Ids, oru64_StartingNodeId, oru64_LastNodeId, orc_ConnectionPos,
                                          ors32_Interface, orc_Properties,
                                          "Reconnect bus connector(s) to new node",
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
            const stw::opensyde_core::C_OscNode * const pc_NodeData =
               C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(static_cast<uint32_t>(s32_NodeIndex));
            if (pc_NodeData != NULL)
            {
               const C_PuiSdNodeConnectionId * const pc_ConnectionId = pc_BusConnector->GetConnectionData();
               if (pc_ConnectionId != NULL)
               {
                  this->mu8_InitialInterface = pc_ConnectionId->u8_InterfaceNumber;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyReconnectNodeCommand::~C_SdManUnoTopologyReconnectNodeCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect from specified node to specified node using specified interface

   \param[in]  oru64_StartingId  ID of initial node
   \param[in]  oru64_LastId      ID of new node
   \param[in]  ors32_Interface   Interface number to use
   \param[in]  orc_Properties    Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyReconnectNodeCommand::m_Reconnect(const uint64_t & oru64_StartingId,
                                                         const uint64_t & oru64_LastId, const int32_t & ors32_Interface,
                                                         const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   C_GiNode * const pc_StartingNode = dynamic_cast<C_GiNode *>(m_GetSceneItem(oru64_StartingId));
   C_GiNode * const pc_LastNode = dynamic_cast<C_GiNode *>(m_GetSceneItem(oru64_LastId));
   C_GiLiBusConnector * const pc_BusConnector = m_GetBusConnector();

   if (pc_BusConnector != NULL)
   {
      pc_BusConnector->Reconnect(pc_StartingNode, pc_LastNode, this->mc_ConnectionPos, ors32_Interface, orc_Properties);
   }
}
