//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Core communication driver protocol class (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSYCOMDRIVERH
#define C_OSYCOMDRIVERH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <set>
#include <map>
#include "stwtypes.h"
#include "C_OSCRoutingRoute.h"
#include "C_OSCProtocolDriverOsyTpBase.h"
#include "C_OSCIpDispatcher.h"
#include "C_OSCCanDispatcherOsyRouter.h"
#include "C_OSCRoutingCalculation.h"
#include "C_OSCProtocolDriverOsyTpCan.h"
#include "C_OSCProtocolDriverOsyTpIp.h"
#include "C_OSCSystemDefinition.h"
#include "C_OSCComDriverBase.h"
#include "TGLTasks.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCComDriverProtocol :
   public C_OSCComDriverBase
{
public:
   C_OSCComDriverProtocol(void);
   virtual ~C_OSCComDriverProtocol(void);

   virtual stw_types::sint32 Init(const C_OSCSystemDefinition & orc_SystemDefinition,
                                  const stw_types::uint32 ou32_ActiveBusIndex,
                                  const std::vector<stw_types::uint8> & orc_ActiveNodes,
                                  stw_can::C_CAN_Dispatcher * const opc_CanDispatcher,
                                  C_OSCIpDispatcher * const opc_IpDispatcher);
   stw_types::sint32 SendTesterPresent(const std::set<stw_types::uint32> * const opc_SkipNodes = NULL);
   stw_types::sint32 StartRouting(const stw_types::uint32 ou32_NodeIndex,
                                  stw_types::uint32 * const opu32_ErrorNodeIndex = NULL);
   stw_types::sint32 StopRouting(const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 IsRoutingNecessary(const stw_types::uint32 ou32_NodeIndex);
   bool IsEthToEthRoutingNecessary(const stw_types::uint32 ou32_RouterNodeIndex) const;
   stw_types::sint32 GetBusIndexOfRoutingNode(const stw_types::uint32 ou32_NodeIndex,
                                              stw_types::uint32 & oru32_BusIndex);
   stw_types::uint32 GetRoutingPointMaximum(void) const;
   stw_types::uint32 GetRoutingPointCount(const stw_types::uint32 ou32_NodeIndex, bool & orq_Active) const;
   void GetRouteOfNode(const stw_types::uint32 ou32_NodeIndex, C_OSCRoutingRoute & orc_Route) const;

   const stw_opensyde_core::C_OSCProtocolDriverOsyNode & GetClientId(void) const;
   bool GetNodeIndex(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                     stw_types::uint32 & oru32_NodeIndex) const;
   void ClearDispatcherQueue(void);
   bool IsInitialized(void) const;

   stw_types::sint32 ReConnectNode(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId) const;
   stw_types::sint32 DisconnectNode(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId) const;
   void DisconnectNodes(void) const;

   virtual void PrepareForDestruction(void);

   stw_opensyde_core::C_OSCProtocolDriverOsyTpBase * GetOsyTransportProtocol(const stw_types::uint32 ou32_NodeIndex);

protected:
   ///Init state
   bool mq_Initialized;
   ///Holds all indices of all relevant nodes for the current use case
   std::vector<stw_types::uint32> mc_ActiveNodesIndexes;
   ///Has pointer to created instances of generic openSYDE protocol if available. Lifetime is handled by child classes.
   ///Length matches number of active nodes.
   std::vector<stw_opensyde_core::C_OSCProtocolDriverOsy *> mc_OsyProtocols;
   ///Holds routes to use for each active node
   std::vector<stw_opensyde_core::C_OSCRoutingRoute> mc_Routes;
   ///Holds server Ids for each active node
   std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyNode> mc_ServerIDs;
   ///Holds server IP addresses for each active node
   std::vector<C_OSCNodeComInterfaceSettings::C_IpAddress> mc_ServerIpAddresses;
   ///Holds created instances of either CAN or IP TP. Length matches number of active nodes.
   std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyTpBase *> mc_TransportProtocols;
   ///Holds created instances of routing dispatcher (one for each routed KEFEX diagnostic server).
   ///Length matches number of active nodes.
   std::vector<stw_opensyde_core::C_OSCCanDispatcherOsyRouter *> mc_LegacyRouterDispatchers;

   ///Active nodes which are the last CAN node on a route before the concrete target
   std::vector<stw_types::uint32> mc_ActiveNodesLastCanRouters;

   stw_opensyde_core::C_OSCProtocolDriverOsyTpCan * mpc_CanTransportProtocolBroadcast;
   stw_opensyde_core::C_OSCProtocolDriverOsyTpIp * mpc_IpTransportProtocolBroadcast;

   const C_OSCSystemDefinition * mpc_SysDef;

   stw_types::uint32 m_GetActiveNodeCount(void) const;
   stw_types::uint32 m_GetActiveIndex(const stw_types::uint32 ou32_NodeIndex, bool * const opq_Found = NULL) const;
   stw_types::uint32 m_GetActiveIndex(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                      bool & orq_Found) const;
   stw_can::C_CAN_Dispatcher * m_GetCanDispatcher(void);
   virtual bool m_GetRoutingMode(stw_opensyde_core::C_OSCRoutingCalculation::E_Mode & ore_Mode) const = 0;
   virtual stw_types::uint8 m_GetRoutingSessionId(void) const = 0;
   stw_opensyde_core::C_OSCProtocolDriverOsy * m_GetOsyProtocol(
      const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId) const;
   stw_scl::C_SCLString m_GetActiveNodeName(const stw_types::uint32 ou32_NodeIndex) const;

   stw_types::sint32 m_SetNodeSessionId(const stw_types::uint32 ou32_ActiveNode, const stw_types::uint8 ou8_SessionId,
                                        const bool oq_CheckForSession, stw_types::uint8 * const opu8_NrCode) const;
   stw_types::sint32 m_SetNodesSessionId(const stw_types::uint8 ou8_SessionId, const bool oq_CheckForSession,
                                         std::set<stw_types::uint32> & orc_DefectNodeIndices) const;
   stw_types::sint32 m_SetNodeSessionIdWithExpectation(const stw_types::uint32 ou32_ActiveNode,
                                                       const stw_types::uint8 ou8_ExpectedNeededSession) const;
   stw_types::sint32 m_SetNodeSecurityAccess(const stw_types::uint32 ou32_ActiveNode,
                                             const stw_types::uint8 ou8_SecurityLevel,
                                             stw_types::uint8 * const opu8_NrCode) const;
   stw_types::sint32 m_SetNodesSecurityAccess(const stw_types::uint8 ou8_SecurityLevel,
                                              std::set<stw_types::uint32> & orc_ErrorActiveNodes) const;

   stw_types::sint32 m_StartRoutingIp2Ip(const stw_types::uint32 ou32_ActiveNode,
                                         stw_types::uint32 * const opu32_ErrorActiveNodeIndex);
   stw_types::sint32 m_StartRouting(const stw_types::uint32 ou32_ActiveNode,
                                    stw_types::uint32 * const opu32_ErrorActiveNodeIndex);
   void m_StopRouting(const stw_types::uint32 ou32_ActiveNode);
   void m_StopRoutingOfActiveNodes(void);
   stw_types::sint32 m_StopRoutingOfRoutingPoint(const stw_types::uint32 ou32_ActiveNode,
                                                 const stw_types::uint32 ou32_ActiveOsyTargetNode,
                                                 const C_OSCRoutingRoutePoint & orc_Point, const bool oq_FirstPoint);

   //-----------------------------------------------------------------------------
   /*!
      \brief   Checks if the routing for a not openSYDE server is necessary

      \param[in]     orc_Node                             Current node

      \return
      true    Specific server and legacy routing necessary
      false   No specific server and legacy routing necessary
   */
   //-----------------------------------------------------------------------------
   virtual bool m_IsRoutingSpecificNecessary(const stw_opensyde_core::C_OSCNode & orc_Node) const =
      0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Prepares the routing for a not openSYDE server

      The legacy routing dispatcher must be created and assigned to opc_ProtocolOsyOfLastNodeOfRouting if
      necessary.

      \param[in]     ou32_ActiveNode                      Active node index of vector mc_ActiveNodes
      \param[in]     opc_Node                             Pointer to current node
      \param[in]     orc_LastNodeOfRouting                The last node in the routing chain before the final target server
      \param[in]     opc_ProtocolOsyOfLastNodeOfRouting   The protocol of the last node
      \param[out]    oppc_RoutingDispatcher               The legacy routing dispatcher

      \return
      C_NO_ERR    Specific server necessary and legacy routing dispatcher created
      C_NOACT     No specific server necessary
      C_CONFIG    opc_ProtocolOsyOfLastNodeOfRouting is NULL
                  Diagnose protocol is NULL
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 m_StartRoutingSpecific(const stw_types::uint32 ou32_ActiveNode,
                                                    const stw_opensyde_core::C_OSCNode * const opc_Node,
                                                    const stw_opensyde_core::C_OSCRoutingRoutePoint & orc_LastNodeOfRouting, stw_opensyde_core::C_OSCProtocolDriverOsy * opc_ProtocolOsyOfLastNodeOfRouting, stw_opensyde_core::C_OSCCanDispatcherOsyRouter ** const oppc_RoutingDispatcher)
      =
         0;

   virtual void m_StopRoutingSpecific(const stw_types::uint32 ou32_ActiveNode);

   virtual bool m_CheckInterfaceForFunctions(const C_OSCNodeComInterfaceSettings & orc_ComItfSettings) const = 0;

private:
   static const stw_types::uint8 mhu8_NODE_ID_CLIENT = 126;

   stw_opensyde_core::C_OSCProtocolDriverOsyNode mc_ClientID; ///< Client/our own ID
   stw_types::uint32 mu32_ActiveNodeCount;                    ///< Active node count
   stw_opensyde_core::C_OSCIpDispatcher * mpc_IpDispatcher;   ///< Holds created instance of IP dispatcher

   std::map<stw_types::uint32, stw_types::uint32> mc_ActiveNodeIp2IpDispatcher; ///< Mapping from active node index to
                                                                                // node
                                                                                // TCP dispatcher handle for IP to IP
                                                                                // routing
   std::vector<stw_types::uint32> mc_ActiveNodeIp2IpDispatcherNodeCount;        ///< Count of nodes which use the TCP
                                                                                // dispatcher handle for IP to IP
                                                                                // routing

   std::map<stw_types::uint32, stw_types::uint32> mc_ActiveNodeIp2CanDispatcher; ///< Mapping from active node index to
                                                                                 // node
                                                                                 // TCP dispatcher handle for IP to CAN
                                                                                 // routing
   std::vector<stw_types::uint32> mc_ActiveNodeIp2CanDispatcherNodeCount;        ///< Count of nodes which use the TCP
                                                                                 // dispatcher handle for IP to CAN
                                                                                 // routing

   std::vector<stw_types::uint8> mc_ActiveNodesSystem; ///< List of flags for all nodes which are active in the system
   stw_types::uint32 mu32_ActiveBusIndex;

   stw_types::sint32 m_InitRoutesAndActiveNodes(void);
   stw_types::sint32 m_InitServerIds(void);
   stw_types::sint32 m_InitForCAN(void);
   stw_types::sint32 m_InitForEthernet(void);

   stw_types::sint32 m_GetActiveIndexOfIp2IpRouter(const stw_types::uint32 ou32_ActiveIndexTarget,
                                                   stw_types::uint32 & oru32_ActiveIndexRouterClient,
                                                   stw_types::uint32 & oru32_ActiveIndexRouterTarget);
   stw_types::sint32 m_GetActiveIndexOfIp2CanRouter(const stw_types::uint32 ou32_ActiveIndexTarget,
                                                    stw_types::uint32 & oru32_ActiveIndexRouter);
   stw_types::sint32 m_InitTcp(const stw_types::uint8 (&orau8_Ip)[4], stw_types::uint32 & oru32_Handle);

   //Avoid call
   C_OSCComDriverProtocol(const C_OSCComDriverProtocol &);
   C_OSCComDriverProtocol & operator =(const C_OSCComDriverProtocol &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
