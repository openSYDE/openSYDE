//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Core communication driver protocol class (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMDRIVERPROTOCOLH
#define C_OSCCOMDRIVERPROTOCOLH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <set>
#include <map>
#include "stwtypes.hpp"
#include "C_OscRoutingRoute.hpp"
#include "C_OscProtocolDriverOsyTpBase.hpp"
#include "C_OscIpDispatcher.hpp"
#include "C_OscCanDispatcherOsyRouter.hpp"
#include "C_OscRoutingCalculation.hpp"
#include "C_OscProtocolDriverOsyTpCan.hpp"
#include "C_OscProtocolDriverOsyTpIp.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscComDriverBase.hpp"
#include "C_OscSecurityPemDatabase.hpp"
#include "TglTasks.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComDriverProtocol :
   public C_OscComDriverBase
{
public:
   C_OscComDriverProtocol(void);
   virtual ~C_OscComDriverProtocol(void);

   virtual int32_t Init(const C_OscSystemDefinition & orc_SystemDefinition, const uint32_t ou32_ActiveBusIndex,
                        const std::vector<uint8_t> & orc_ActiveNodes,
                        stw::can::C_CanDispatcher * const opc_CanDispatcher, C_OscIpDispatcher * const opc_IpDispatcher,
                        C_OscSecurityPemDatabase * const opc_SecurityPemDb);
   int32_t SendTesterPresent(const std::set<uint32_t> * const opc_SkipNodes = NULL);
   int32_t SendTesterPresent(const std::vector<uint32_t> & orc_ActiveNodes) const;
   int32_t SendTesterPresent(const C_OscProtocolDriverOsyNode & orc_ServerId) const;
   int32_t StartRouting(const uint32_t ou32_NodeIndex, uint32_t * const opu32_ErrorNodeIndex = NULL);
   int32_t StopRouting(const uint32_t ou32_NodeIndex);
   int32_t IsRoutingNecessary(const uint32_t ou32_NodeIndex);
   bool IsEthToEthRoutingNecessary(const uint32_t ou32_RouterNodeIndex) const;
   int32_t GetBusIndexOfRoutingNode(const uint32_t ou32_NodeIndex, uint32_t & oru32_BusIndex);
   uint32_t GetRoutingPointMaximum(void) const;
   uint32_t GetRoutingPointCount(const uint32_t ou32_NodeIndex, bool & orq_Active) const;
   void GetRouteOfNode(const uint32_t ou32_NodeIndex, C_OscRoutingRoute & orc_Route) const;
   int32_t GetRoutingTargetInterfaceType(const uint32_t ou32_NodeIndex,
                                         C_OscSystemBus::E_Type & ore_InterfaceType) const;
   int32_t GetServerIdOfLastRouter(const uint32_t ou32_NodeIndex,
                                   C_OscProtocolDriverOsyNode & orc_RouterServerId) const;

   const C_OscProtocolDriverOsyNode & GetClientId(void) const;
   bool GetNodeIndex(const C_OscProtocolDriverOsyNode & orc_ServerId, uint32_t & oru32_NodeIndex) const;
   void ClearDispatcherQueue(void);
   bool IsInitialized(void) const;

   int32_t ReConnectNode(const C_OscProtocolDriverOsyNode & orc_ServerId) const;
   int32_t DisconnectNode(const C_OscProtocolDriverOsyNode & orc_ServerId) const;
   void DisconnectNodes(void) const;

   virtual void PrepareForDestruction(void);

   C_OscProtocolDriverOsyTpBase * GetOsyTransportProtocol(const uint32_t ou32_NodeIndex);

protected:
   ///Init state
   bool mq_Initialized;
   ///Holds all indices of all relevant nodes for the current use case. This active nodes represent the initialized
   /// active node (for example mc_OsyProtocols) indexes which are part of mc_ActiveNodesSystem.
   std::vector<uint32_t> mc_ActiveNodesIndexes;
   ///Has pointer to created instances of generic openSYDE protocol if available. Lifetime is handled by child classes.
   ///Length matches number of active nodes.
   std::vector<C_OscProtocolDriverOsy *> mc_OsyProtocols;
   ///Holds routes to use for each active node
   std::vector<C_OscRoutingRoute> mc_Routes;
   ///Holds server Ids for each active node
   std::vector<C_OscProtocolDriverOsyNode> mc_ServerIds;
   ///Holds server IP addresses for each active node
   std::vector<C_OscNodeComInterfaceSettings::C_IpAddress> mc_ServerIpAddresses;
   ///Holds created instances of either CAN or IP TP. Length matches number of active nodes.
   std::vector<C_OscProtocolDriverOsyTpBase *> mc_TransportProtocols;
   ///Holds created instances of routing dispatcher (one for each routed KEFEX diagnostic server).
   ///Length matches number of active nodes.
   std::vector<C_OscCanDispatcherOsyRouter *> mc_LegacyRouterDispatchers;

   ///Active nodes which are the last CAN node on a route before the concrete target
   std::vector<uint32_t> mc_ActiveNodesLastCanRouters;

   C_OscProtocolDriverOsyTpCan * mpc_CanTransportProtocolBroadcast;
   C_OscProtocolDriverOsyTpIp * mpc_IpTransportProtocolBroadcast;

   const C_OscSystemDefinition * mpc_SysDef;

   uint32_t m_GetActiveNodeCount(void) const;
   uint32_t m_GetActiveIndex(const uint32_t ou32_NodeIndex, bool * const opq_Found = NULL) const;
   uint32_t m_GetActiveIndex(const C_OscProtocolDriverOsyNode & orc_ServerId, bool & orq_Found) const;
   stw::can::C_CanDispatcher * m_GetCanDispatcher(void);
   C_OscIpDispatcher * m_GetIpDispatcher(void);
   virtual bool m_GetRoutingMode(C_OscRoutingCalculation::E_Mode & ore_Mode) const = 0;
   virtual uint8_t m_GetRoutingSessionId(void) const = 0;
   C_OscProtocolDriverOsy * m_GetOsyProtocol(const C_OscProtocolDriverOsyNode & orc_ServerId) const;
   stw::scl::C_SclString m_GetActiveNodeName(const uint32_t ou32_ActiveNodeIndex) const;

   int32_t m_SetNodeSessionId(const uint32_t ou32_ActiveNode, const uint8_t ou8_SessionId,
                              const bool oq_CheckForSession, uint8_t * const opu8_NrCode) const;
   int32_t m_SetNodeSessionId(C_OscProtocolDriverOsy * const opc_ExistingProtocol, const uint8_t ou8_SessionId,
                              const bool oq_CheckForSession, uint8_t * const opu8_NrCode) const;
   int32_t m_SetNodesSessionId(const uint8_t ou8_SessionId, const bool oq_CheckForSession,
                               std::set<uint32_t> & orc_DefectNodeIndices) const;
   int32_t m_SetNodesSessionId(const std::vector<uint32_t> & orc_ActiveNodes, const uint8_t ou8_SessionId,
                               const bool oq_CheckForSession, std::set<uint32_t> & orc_DefectNodeIndices) const;
   int32_t m_SetNodeSessionIdWithExpectation(const uint32_t ou32_ActiveNode,
                                             const uint8_t ou8_ExpectedNeededSession) const;
   int32_t m_SetNodeSecurityAccess(const uint32_t ou32_ActiveNode, const uint8_t ou8_SecurityLevel,
                                   uint8_t * const opu8_NrCode) const;
   int32_t m_SetNodeSecurityAccess(C_OscProtocolDriverOsy * const opc_ExistingProtocol, const uint8_t ou8_SecurityLevel,
                                   uint8_t * const opu8_NrCode) const;
   int32_t m_SetNodesSecurityAccess(const uint8_t ou8_SecurityLevel, std::set<uint32_t> & orc_ErrorActiveNodes) const;
   int32_t m_SetNodesSecurityAccess(const std::vector<uint32_t> & orc_ActiveNodes, const uint8_t ou8_SecurityLevel,
                                    std::set<uint32_t> & orc_ErrorActiveNodes) const;

   int32_t m_StartRoutingIp2Ip(const uint32_t ou32_ActiveNode, uint32_t * const opu32_ErrorActiveNodeIndex);
   int32_t m_StartRouting(const uint32_t ou32_ActiveNode, uint32_t * const opu32_ErrorActiveNodeIndex);
   void m_StopRouting(const uint32_t ou32_ActiveNode);
   void m_StopRoutingOfActiveNodes(void);
   int32_t m_StopRoutingOfRoutingPoint(const uint32_t ou32_ActiveNode, const uint32_t ou32_ActiveOsyTargetNode,
                                       const C_OscRoutingRoutePoint & orc_Point, const bool oq_FirstPoint);

   //-----------------------------------------------------------------------------
   /*!
      \brief   Checks if the routing for a not openSYDE server is necessary

      \param[in]     orc_Node                             Current node

      \return
      true    Specific server and legacy routing necessary
      false   No specific server and legacy routing necessary
   */
   //-----------------------------------------------------------------------------
   virtual bool m_IsRoutingSpecificNecessary(const C_OscNode & orc_Node) const = 0;

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
   virtual int32_t m_StartRoutingSpecific(const uint32_t ou32_ActiveNode, const C_OscNode * const opc_Node,
                                          const C_OscRoutingRoutePoint & orc_LastNodeOfRouting,
                                          C_OscProtocolDriverOsy * opc_ProtocolOsyOfLastNodeOfRouting,
                                          C_OscCanDispatcherOsyRouter ** const oppc_RoutingDispatcher) = 0;

   virtual void m_StopRoutingSpecific(const uint32_t ou32_ActiveNode);

   virtual bool m_CheckInterfaceForFunctions(const C_OscNodeComInterfaceSettings & orc_ComItfSettings) const = 0;

private:
   static const uint8_t mhu8_NODE_ID_CLIENT = 126;

   C_OscProtocolDriverOsyNode mc_ClientId; ///< Client/our own ID
   uint32_t mu32_ActiveNodeCount;          ///< Active node count in mc_ActiveNodesIndexes, not in mc_ActiveNodesSystem
   C_OscIpDispatcher * mpc_IpDispatcher;   ///< Holds created instance of IP dispatcher

   std::map<uint32_t, uint32_t> mc_ActiveNodeIp2IpDispatcher; ///< Mapping from active node index to
   // node
   // TCP dispatcher handle for IP to IP
   // routing
   std::vector<uint32_t> mc_ActiveNodeIp2IpDispatcherNodeCount; ///< Count of nodes which use the TCP
   // dispatcher handle for IP to IP
   // routing

   std::map<uint32_t, uint32_t> mc_ActiveNodeIp2CanDispatcher; ///< Mapping from active node index to
   // node
   // TCP dispatcher handle for IP to CAN
   // routing
   std::vector<uint32_t> mc_ActiveNodeIp2CanDispatcherNodeCount; ///< Count of nodes which use the TCP
   // dispatcher handle for IP to CAN
   // routing

   std::vector<uint8_t> mc_ActiveNodesSystem; ///< List of flags for all nodes which are active in the system
   // definition set by Init call

   uint32_t mu32_ActiveBusIndex;
   C_OscSecurityPemDatabase * mpc_SecurityPemDb;

   int32_t m_InitRoutesAndActiveNodes(void);
   int32_t m_InitServerIds(void);
   int32_t m_InitForCan(void);
   int32_t m_InitForEthernet(void);

   int32_t m_GetActiveIndexOfIp2IpRouter(const uint32_t ou32_ActiveIndexTarget,
                                         uint32_t & oru32_ActiveIndexRouterClient,
                                         uint32_t & oru32_ActiveIndexRouterTarget);
   int32_t m_GetActiveIndexOfIp2CanRouter(const uint32_t ou32_ActiveIndexTarget, uint32_t & oru32_ActiveIndexRouter);
   int32_t m_InitTcp(const uint8_t (&orau8_Ip)[4], uint32_t & oru32_Handle);

   //Avoid call
   C_OscComDriverProtocol(const C_OscComDriverProtocol &);
   C_OscComDriverProtocol & operator =(const C_OscComDriverProtocol &); //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
