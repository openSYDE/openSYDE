//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Reconnect bus connector to new node undo command (implementation)

   Reconnect bus connector to new node undo command

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.12.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdManUnoTopologyReconnectNodeCommand.h"
#include "C_SdManUnoTopologyDeleteCommand.h"
#include "C_GiNode.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace std;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Scene            Pointer to currently active scene
   \param[in]     orc_IDs              Affected unique IDs
   \param[in]     oru64_StartingNodeID Initial node ID
   \param[in]     oru64_LastNodeID     New node ID
   \param[in]     orc_ConnectionPos    Event position
   \param[in]     ors32_Interface      Interface to connect to
   \param[in]     oru8_NodeId          New node id
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     01.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdManUnoTopologyReconnectNodeCommand::C_SdManUnoTopologyReconnectNodeCommand(QGraphicsScene * const opc_Scene,
                                                                               const vector<uint64> & orc_IDs,
                                                                               const uint64 & oru64_StartingNodeID,
                                                                               const uint64 & oru64_LastNodeID,
                                                                               const QPointF & orc_ConnectionPos,
                                                                               const sint32 & ors32_Interface,
                                                                               const sint8 & oru8_NodeId,
                                                                               QUndoCommand * const opc_Parent) :
   C_SdManUnoTopologyReconnectBaseCommand(opc_Scene, orc_IDs, oru64_StartingNodeID, oru64_LastNodeID, orc_ConnectionPos,
                                          ors32_Interface, oru8_NodeId, "Reconnect bus connector(s) to new node",
                                          opc_Parent)
{
   C_GiLiBusConnector * pc_BusConnector = m_GetBusConnector();

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
               const C_PuiSdNodeConnectionId * pc_ConnectionId = pc_BusConnector->GetConnectionData();
               if (pc_ConnectionId != NULL)
               {
                  this->mu8_InitialInterface = pc_ConnectionId->u8_InterfaceNumber;
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     01.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdManUnoTopologyReconnectNodeCommand::~C_SdManUnoTopologyReconnectNodeCommand()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reconnect from specified node to specified node using specified interface

   \param[in] oru64_StartingID ID of initial node
   \param[in] oru64_LastID     ID of new node
   \param[in] ors32_Interface  Interface number to use
   \param[in] oru8_NodeId      New node id

   \created     05.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdManUnoTopologyReconnectNodeCommand::m_Reconnect(const uint64 & oru64_StartingID, const uint64 & oru64_LastID,
                                                         const sint32 & ors32_Interface, const uint8 & oru8_NodeId)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_GiNode * pc_StartingNode = dynamic_cast<C_GiNode *>(m_GetSceneItem(oru64_StartingID));
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_GiNode * pc_LastNode = dynamic_cast<C_GiNode *>(m_GetSceneItem(oru64_LastID));
   C_GiLiBusConnector * pc_BusConnector = m_GetBusConnector();

   if (pc_BusConnector != NULL)
   {
      pc_BusConnector->Reconnect(pc_StartingNode, pc_LastNode, this->mc_ConnectionPos, ors32_Interface, oru8_NodeId);
   }
}
