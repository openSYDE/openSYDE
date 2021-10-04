//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Reconnect bus connector to new node undo command (implementation)

   Reconnect bus connector to new node undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdManUnoTopologyReconnectNodeCommand.h"
#include "C_SdManUnoTopologyDeleteCommand.h"
#include "C_GiNode.h"
#include "C_SdUtil.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
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
   \param[in]      orc_IDs                Affected unique IDs
   \param[in]      oru64_StartingNodeID   Initial node ID
   \param[in]      oru64_LastNodeID       New node ID
   \param[in]      orc_ConnectionPos      Event position
   \param[in]      ors32_Interface        Interface to connect to
   \param[in]      orc_NodeIds            Node ids
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyReconnectNodeCommand::C_SdManUnoTopologyReconnectNodeCommand(QGraphicsScene * const opc_Scene,
                                                                               const vector<uint64> & orc_IDs,
                                                                               const uint64 & oru64_StartingNodeID,
                                                                               const uint64 & oru64_LastNodeID,
                                                                               const QPointF & orc_ConnectionPos,
                                                                               const sint32 & ors32_Interface,
                                                                               const std::vector<uint8> & orc_NodeIds,
                                                                               QUndoCommand * const opc_Parent) :
   C_SdManUnoTopologyReconnectBaseCommand(opc_Scene, orc_IDs, oru64_StartingNodeID, oru64_LastNodeID, orc_ConnectionPos,
                                          ors32_Interface, orc_NodeIds, "Reconnect bus connector(s) to new node",
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
            const stw_opensyde_core::C_OSCNode * const pc_NodeData =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(static_cast<uint32>(s32_NodeIndex));
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

   \param[in]  oru64_StartingID  ID of initial node
   \param[in]  oru64_LastID      ID of new node
   \param[in]  ors32_Interface   Interface number to use
   \param[in]  orc_NodeIds       Node ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyReconnectNodeCommand::m_Reconnect(const uint64 & oru64_StartingID, const uint64 & oru64_LastID,
                                                         const sint32 & ors32_Interface,
                                                         const std::vector<uint8> & orc_NodeIds)
{
   C_GiNode * const pc_StartingNode = dynamic_cast<C_GiNode *>(m_GetSceneItem(oru64_StartingID));
   C_GiNode * const pc_LastNode = dynamic_cast<C_GiNode *>(m_GetSceneItem(oru64_LastID));
   C_GiLiBusConnector * const pc_BusConnector = m_GetBusConnector();

   if (pc_BusConnector != NULL)
   {
      pc_BusConnector->Reconnect(pc_StartingNode, pc_LastNode, this->mc_ConnectionPos, ors32_Interface, orc_NodeIds);
   }
}
