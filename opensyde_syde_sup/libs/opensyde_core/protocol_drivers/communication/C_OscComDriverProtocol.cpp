//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Core communication driver protocol class (implementation)

   Core communication driver protocol class
   * manages instantiation and cleanup of all required subclasses that
     use used for all kinds of communication purposes (i.e. flashloader and diagnostics)
   * owner of all those class instances

   Will be derived from for specific uses.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include <iostream>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "TglUtils.hpp"
#include "C_OscComDriverProtocol.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscDiagProtocolOsy.hpp"
#include "C_OscProtocolDriverOsyTpCan.hpp"
#include "C_OscProtocolDriverOsyTpIp.hpp"
#include "C_OscRoutingCalculation.hpp"
#include "C_OscSecurityRsa.hpp"
#include "TglUtils.hpp"
#include "TglTime.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::can;
using namespace stw::scl;
using namespace stw::tgl;

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Initialize all members based on view
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComDriverProtocol::C_OscComDriverProtocol(void) :
   C_OscComDriverBase(),
   mq_Initialized(false),
   mpc_CanTransportProtocolBroadcast(NULL),
   mpc_IpTransportProtocolBroadcast(NULL),
   mpc_SysDef(NULL),
   mu32_ActiveNodeCount(0),
   mpc_IpDispatcher(NULL),
   mu32_ActiveBusIndex(0U),
   mpc_SecurityPemDb(NULL)
{
   //Check if client and server use same float standard, see #84517 for more details
   tgl_assert(std::numeric_limits<float32_t>::is_iec559);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComDriverProtocol::~C_OscComDriverProtocol(void)
{
   uint32_t u32_Counter;

   for (u32_Counter = 0; u32_Counter < this->mc_TransportProtocols.size(); ++u32_Counter)
   {
      delete (this->mc_TransportProtocols[u32_Counter]);
      this->mc_TransportProtocols[u32_Counter] = NULL;
   }

   for (u32_Counter = 0; u32_Counter < this->mc_LegacyRouterDispatchers.size(); ++u32_Counter)
   {
      // If stop routing was not called correctly
      delete (this->mc_LegacyRouterDispatchers[u32_Counter]);
      this->mc_LegacyRouterDispatchers[u32_Counter] = NULL;
   }

   delete this->mpc_CanTransportProtocolBroadcast;
   delete this->mpc_IpTransportProtocolBroadcast;

   this->mpc_CanTransportProtocolBroadcast = NULL;
   this->mpc_IpTransportProtocolBroadcast = NULL;
   this->mpc_IpDispatcher = NULL;  //do not delete ! not owned by us
   this->mpc_SecurityPemDb = NULL; //do not delete ! not owned by us
   this->mpc_SysDef = NULL;        //do not delete ! not owned by us
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all members

   \param[in]  orc_SystemDefinition    Entire system definition
   \param[in]  ou32_ActiveBusIndex     Bus index of bus in system definition where we are connected to
   \param[in]  orc_ActiveNodes         Flags for all available nodes in the system
   \param[in]  opc_CanDispatcher       Pointer to concrete CAN dispatcher
   \param[in]  opc_IpDispatcher        Pointer to concrete IP dispatcher
   \param[in]  opc_SecurityPemDb       Pointer to PEM database (optional)
                                       Needed if nodes with enabled security are used in the system

   \return
   C_NO_ERR      Operation success
   C_NOACT       No active nodes
   C_CONFIG      Invalid system definition for parameters
                 TCP connection failed
   C_OVERFLOW    Unknown transport protocol or unknown diagnostic server for at least one node
   C_COM         CAN initialization failed or no route found for at least one node
   C_CHECKSUM    Internal buffer overflow detected
   C_DEFAULT     Parameter ou32_ActiveBusIndex invalid
   C_RANGE       Routing configuration failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::Init(const C_OscSystemDefinition & orc_SystemDefinition,
                                     const uint32_t ou32_ActiveBusIndex, const std::vector<uint8_t> & orc_ActiveNodes,
                                     C_CanDispatcher * const opc_CanDispatcher,
                                     C_OscIpDispatcher * const opc_IpDispatcher,
                                     C_OscSecurityPemDatabase * const opc_SecurityPemDb)
{
   int32_t s32_Retval = C_NOACT;
   uint32_t u32_Counter;

   // Check the parameters
   // At least one node active ?
   for (u32_Counter = 0U; u32_Counter < orc_ActiveNodes.size(); ++u32_Counter)
   {
      if (orc_ActiveNodes[u32_Counter] == 1U)
      {
         s32_Retval = C_NO_ERR;
         break;
      }
   }

   if (ou32_ActiveBusIndex >= orc_SystemDefinition.c_Buses.size())
   {
      s32_Retval = C_DEFAULT;
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->InitBase(opc_CanDispatcher);
   }

   if (s32_Retval == C_NO_ERR)
   {
      this->mu32_ActiveBusIndex = ou32_ActiveBusIndex;
      this->mc_ActiveNodesSystem.clear();
      this->mc_ActiveNodesSystem = orc_ActiveNodes;
      this->mpc_SysDef = &orc_SystemDefinition;
      this->mpc_IpDispatcher = opc_IpDispatcher;
      this->mpc_SecurityPemDb = opc_SecurityPemDb;

      //No check for connected because error check passed
      s32_Retval = m_InitRoutesAndActiveNodes();
      if (s32_Retval == C_NO_ERR)
      {
         if (this->mu32_ActiveNodeCount > 0)
         {
            s32_Retval = m_InitServerIds();
            if (s32_Retval == C_NO_ERR)
            {
               //Init client ID
               this->mc_ClientId.u8_NodeIdentifier = mhu8_NODE_ID_CLIENT;
               this->mc_ClientId.u8_BusIdentifier = orc_SystemDefinition.c_Buses[this->mu32_ActiveBusIndex].u8_BusId;
               //Detect communication type
               switch (orc_SystemDefinition.c_Buses[this->mu32_ActiveBusIndex].e_Type)
               {
               case C_OscSystemBus::eCAN:
                  s32_Retval = m_InitForCan();
                  break;
               case C_OscSystemBus::eETHERNET:
                  s32_Retval = m_InitForEthernet();
                  break;
               default:
                  s32_Retval = C_OVERFLOW;
                  break;
               }
            }
         }
         else
         {
            osc_write_log_info("COMM driver initialization",
                               "Information: no nodes active (or not configured for update or diagnostics)");
            s32_Retval = C_NOACT;
         }
      }
      else
      {
         // Routing error
         osc_write_log_error("COMM driver initialization", "Failed: routing initialization error");
         s32_Retval = C_RANGE;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the tester present message to all active nodes

   \param[in]     opc_SkipNodes     optional pointer to a container with nodes for not sending the tester present

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::SendTesterPresent(const std::set<uint32_t> * const opc_SkipNodes)
{
   int32_t s32_Return = C_CONFIG;

   if (this->mq_Initialized == true)
   {
      uint32_t u32_Counter;

      for (u32_Counter = 0U; u32_Counter < this->mc_OsyProtocols.size(); ++u32_Counter)
      {
         // Check the optional set for skipping nodes
         if ((opc_SkipNodes == NULL) ||
             (opc_SkipNodes->find(u32_Counter) == opc_SkipNodes->end()))
         {
            C_OscProtocolDriverOsy * const pc_ProtocolOsy = this->mc_OsyProtocols[u32_Counter];
            if (pc_ProtocolOsy != NULL)
            {
               // Send tester present message without expecting a response
               s32_Return = pc_ProtocolOsy->OsyTesterPresent(1U);

               if (s32_Return != C_NO_ERR)
               {
                  // No response expected. All errors caused by client. We can break here.
                  s32_Return = C_COM;
                  break;
               }
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the tester present message to a specific node

   \param[in]     orc_ServerId     Node to send the tester present

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::SendTesterPresent(const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerId)
const
{
   int32_t s32_Return = C_CONFIG;

   if (this->mq_Initialized == true)
   {
      C_OscProtocolDriverOsy * const pc_ProtocolOsy = this->m_GetOsyProtocol(orc_ServerId);
      if (pc_ProtocolOsy != NULL)
      {
         // Send tester present message without expecting a response
         s32_Return = pc_ProtocolOsy->OsyTesterPresent(1U);

         if (s32_Return != C_NO_ERR)
         {
            // No response expected. All errors caused by client. We can break here.
            s32_Return = C_COM;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the tester present message to specific nodes

   \param[in]     orc_ActiveNodes   Active nodes to send the tester present

   \return
   C_NO_ERR    All tester present messages sent successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::SendTesterPresent(const std::vector<uint32_t> & orc_ActiveNodes)
const
{
   int32_t s32_Return = C_CONFIG;

   if (this->mq_Initialized == true)
   {
      uint32_t u32_Counter;

      for (u32_Counter = 0U; u32_Counter < orc_ActiveNodes.size(); ++u32_Counter)
      {
         const uint32_t u32_ActiveNode = orc_ActiveNodes[u32_Counter];
         if (u32_ActiveNode < this->mc_OsyProtocols.size())
         {
            C_OscProtocolDriverOsy * const pc_ProtocolOsy = this->mc_OsyProtocols[u32_ActiveNode];
            if (pc_ProtocolOsy != NULL)
            {
               // Send tester present message without expecting a response
               s32_Return = pc_ProtocolOsy->OsyTesterPresent(1U);

               if (s32_Return != C_NO_ERR)
               {
                  // No response expected. All errors caused by client. We can break here.
                  s32_Return = C_COM;
               }
            }
            else
            {
               s32_Return = C_CONFIG;
            }
         }
         else
         {
            s32_Return = C_CONFIG;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize the necessary routing configuration to start the routing for one specific server

   Prepares all active nodes with its routing configurations if necessary
   Three different types of routing:
   - openSYDE routing for an openSYDE server
   - legacy routing for a non openSYDE server
   - legacy routing for a non openSYDE after openSYDE routing to an openSYDE server

   \param[in]   ou32_NodeIndex         node index to read from
   \param[out]  opu32_ErrorNodeIndex   optional pointer for node index which caused the error on starting routing if
                                       an error occurred
                                       is set when return value is not C_NO_ERR, C_RANGE and C_CONFIG

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send request (e.g. Tx buffer full)
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_RANGE    node index out of range
   C_COM      communication driver reported error
   C_NOACT    At least one node does not support Ethernet to Ethernet routing
   C_CHECKSUM Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::StartRouting(const uint32_t ou32_NodeIndex, uint32_t * const opu32_ErrorNodeIndex)
{
   int32_t s32_Return;
   const uint32_t u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= this->mc_ActiveNodesIndexes.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = this->m_StartRoutingIp2Ip(u32_ActiveIndex, opu32_ErrorNodeIndex);

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = this->m_StartRouting(u32_ActiveIndex, opu32_ErrorNodeIndex);
      }

      if ((opu32_ErrorNodeIndex != NULL) &&
          (s32_Return != C_NO_ERR))
      {
         // Convert active node index to node index
         tgl_assert(*opu32_ErrorNodeIndex < this->mc_ActiveNodesIndexes.size());
         if (*opu32_ErrorNodeIndex < this->mc_ActiveNodesIndexes.size())
         {
            *opu32_ErrorNodeIndex = this->mc_ActiveNodesIndexes[*opu32_ErrorNodeIndex];
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stops the entire routing configuration for one specific node

   \param[in]   ou32_NodeIndex       node index to read from

   \return
   C_NO_ERR   routing stopped
   C_RANGE    node index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::StopRouting(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Return = C_NO_ERR;
   const uint32_t u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= this->mc_ActiveNodesIndexes.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      this->m_StopRouting(u32_ActiveIndex);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the need of routing for a specific node

   \param[in]   ou32_NodeIndex       node index to read from

   \return
   C_NO_ERR     Routing is necessary for node
   C_NOACT      Routing is not necessary for node or node is not active
   C_RANGE      Node index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::IsRoutingNecessary(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Return;
   bool q_Found;
   const uint32_t u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex, &q_Found);

   if (q_Found == true)
   {
      if (u32_ActiveIndex >= this->mc_Routes.size())
      {
         s32_Return = C_RANGE;
      }
      else if (this->mc_Routes[u32_ActiveIndex].c_VecRoutePoints.size() > 0)
      {
         s32_Return = C_NO_ERR;
      }
      else
      {
         s32_Return = C_NOACT;
      }
   }
   else
   {
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if a specific node must be capable of Ethernet to Ethernet routing

   \param[in]       ou32_RouterNodeIndex     Node to check if node must be capable of Ethernet to Ethernet routing
                                             for at least one route

   \retval   true     Ethernet to Ethernet Routing must be supported by router node
   \retval   false    Ethernet to Ethernet Routing must not be supported by router node
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComDriverProtocol::IsEthToEthRoutingNecessary(const uint32_t ou32_RouterNodeIndex) const
{
   bool q_Return = false;
   uint32_t u32_RouteCounter;

   // Check all routes
   for (u32_RouteCounter = 0U; u32_RouteCounter < this->mc_Routes.size(); ++u32_RouteCounter)
   {
      const C_OscRoutingRoute & rc_Route = this->mc_Routes[u32_RouteCounter];
      uint32_t u32_RoutePointCounter;

      // Check all points
      for (u32_RoutePointCounter = 0U; u32_RoutePointCounter < rc_Route.c_VecRoutePoints.size();
           ++u32_RoutePointCounter)
      {
         const C_OscRoutingRoutePoint & rc_Point = rc_Route.c_VecRoutePoints[u32_RoutePointCounter];

         if ((rc_Point.u32_NodeIndex == ou32_RouterNodeIndex) &&
             (rc_Point.e_InInterfaceType == C_OscSystemBus::eETHERNET) &&
             (rc_Point.e_OutInterfaceType == C_OscSystemBus::eETHERNET))
         {
            // Ethernet routing is necessary for this node for at least one route
            q_Return = true;
            break;
         }
      }

      if (q_Return == true)
      {
         break;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the need of routing for a specific node and returns the connected bus to the target by bus index

   \param[in]   ou32_NodeIndex       node index to read from
   \param[out]  oru32_BusIndex       bus index

   \return
   C_NO_ERR     Routing is necessary for node
   C_NOACT      Routing is not necessary for node or node is not active
   C_RANGE      Node index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::GetBusIndexOfRoutingNode(const uint32_t ou32_NodeIndex, uint32_t & oru32_BusIndex)
{
   int32_t s32_Return;
   bool q_Found;
   const uint32_t u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex, &q_Found);

   if (q_Found == true)
   {
      if (u32_ActiveIndex >= this->mc_Routes.size())
      {
         s32_Return = C_RANGE;
      }
      else if (this->mc_Routes[u32_ActiveIndex].c_VecRoutePoints.size() > 0)
      {
         s32_Return = C_NO_ERR;
         // Get the bus index on the target server
         oru32_BusIndex = this->mc_Routes[u32_ActiveIndex].c_VecRoutePoints[this->mc_Routes[u32_ActiveIndex].
                                                                            c_VecRoutePoints.size() -
                                                                            1].u32_OutBusIndex;
      }
      else
      {
         s32_Return = C_NOACT;
      }
   }
   else
   {
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the maximum count of route points which are necessary to reach a node

   \return
   Maximum number of route points
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscComDriverProtocol::GetRoutingPointMaximum(void) const
{
   uint32_t u32_Maximum = 0U;
   uint32_t u32_Counter;

   // Search the longest route
   for (u32_Counter = 0U; u32_Counter < this->mc_Routes.size(); ++u32_Counter)
   {
      if (this->mc_Routes[u32_Counter].c_VecRoutePoints.size() > u32_Maximum)
      {
         u32_Maximum = static_cast<uint32_t>(this->mc_Routes[u32_Counter].c_VecRoutePoints.size());
      }
   }

   return u32_Maximum;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the count of route points which are necessary to reach the node

   \param[in]   ou32_NodeIndex       node index to read from
   \param[out]  orq_Active           flag if node was found and active

   \return
   Number of route points
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscComDriverProtocol::GetRoutingPointCount(const uint32_t ou32_NodeIndex, bool & orq_Active) const
{
   uint32_t u32_Count = 0U;
   const uint32_t u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex, &orq_Active);

   if (orq_Active == true)
   {
      if (u32_ActiveIndex < this->mc_Routes.size())
      {
         u32_Count = static_cast<uint32_t>(this->mc_Routes[u32_ActiveIndex].c_VecRoutePoints.size());
      }
      else
      {
         orq_Active = false;
      }
   }

   return u32_Count;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the route of the node

   \param[in]     ou32_NodeIndex         Index of node
   \param[out]    orc_Route              Route of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverProtocol::GetRouteOfNode(const uint32_t ou32_NodeIndex, C_OscRoutingRoute & orc_Route) const
{
   bool q_Active;
   const uint32_t u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex, &q_Active);

   if (q_Active == true)
   {
      if (u32_ActiveIndex < this->mc_Routes.size())
      {
         orc_Route = this->mc_Routes[u32_ActiveIndex];
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the used interface type of the node on its route

   \param[in]   ou32_NodeIndex         Node index to get info from
   \param[out]  ore_InterfaceType      Used interface type of target node on its route

   \retval   C_NO_ERR   Detailed description of 1st return value
   \retval   C_RANGE    Node index out of range
   \retval   C_CONFIG   Route of node is not valid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::GetRoutingTargetInterfaceType(const uint32_t ou32_NodeIndex,
                                                              C_OscSystemBus::E_Type & ore_InterfaceType) const
{
   int32_t s32_Return;
   const uint32_t u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= this->mc_ActiveNodesIndexes.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      const C_OscRoutingRoute & rc_ActRoute = this->mc_Routes[u32_ActiveIndex];

      if (rc_ActRoute.c_VecRoutePoints.size() > 0)
      {
         // Check the last routing point. The out interface type is connected to the bus which is
         // connected to the target
         ore_InterfaceType = rc_ActRoute.c_VecRoutePoints[rc_ActRoute.c_VecRoutePoints.size() - 1].e_OutInterfaceType;

         s32_Return = C_NO_ERR;
      }
      else
      {
         s32_Return = C_CONFIG;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the last route point on route before target

   \param[in]   ou32_NodeIndex         Node index of target of route
   \param[out]  orc_RouterServerId     Router server Id of last router on route to target ou32_NodeIndex

   \retval   C_NO_ERR   Detailed description of 1st return value
   \retval   C_RANGE    Node index out of range
   \retval   C_CONFIG   Route of node is not valid or has no route points
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::GetServerIdOfLastRouter(const uint32_t ou32_NodeIndex,
                                                        C_OscProtocolDriverOsyNode & orc_RouterServerId) const
{
   int32_t s32_Return;
   const uint32_t u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= this->mc_ActiveNodesIndexes.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      const C_OscRoutingRoute & rc_ActRoute = this->mc_Routes[u32_ActiveIndex];

      if (rc_ActRoute.c_VecRoutePoints.size() > 0)
      {
         // Get the next to last routing point. That is the last router.
         const uint32_t u32_RouterNodeIndex =
            rc_ActRoute.c_VecRoutePoints[rc_ActRoute.c_VecRoutePoints.size() - 1].u32_NodeIndex;

         orc_RouterServerId = this->mc_ServerIds[this->m_GetActiveIndex(u32_RouterNodeIndex)];

         s32_Return = C_NO_ERR;
      }
      else
      {
         s32_Return = C_CONFIG;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the client id

   \return
   Client id
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscProtocolDriverOsyNode & C_OscComDriverProtocol::GetClientId(void) const
{
   return this->mc_ClientId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the node index by the server id

   \param[in]     orc_ServerId         Server id
   \param[out]    oru32_NodeIndex      Found node index

   \return
   true     Node index found
   false    Node index not found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComDriverProtocol::GetNodeIndex(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                          uint32_t & oru32_NodeIndex) const
{
   uint32_t u32_Counter;
   bool q_Found = false;

   tgl_assert(this->mc_ServerIds.size() == this->mc_ActiveNodesIndexes.size());

   for (u32_Counter = 0U; u32_Counter < this->mc_ServerIds.size(); ++u32_Counter)
   {
      if (orc_ServerId == this->mc_ServerIds[u32_Counter])
      {
         oru32_NodeIndex = this->mc_ActiveNodesIndexes[u32_Counter];
         q_Found = true;
         break;
      }
   }

   return q_Found;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Dump all messages of receive queue of CAN dispatcher
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverProtocol::ClearDispatcherQueue(void)
{
   if (this->mq_Initialized == true)
   {
      uint32_t u32_Counter;
      // Get all messages for clearing them
      if (this->mpc_CanDispatcher != NULL)
      {
         (void)mpc_CanDispatcher->DispatchIncoming();
      }

      for (u32_Counter = 0U; u32_Counter < this->mc_TransportProtocols.size(); ++u32_Counter)
      {
         C_OscProtocolDriverOsyTpCan * const pc_CanTp =
            dynamic_cast<C_OscProtocolDriverOsyTpCan *>(this->mc_TransportProtocols[u32_Counter]);

         if (pc_CanTp != NULL)
         {
            pc_CanTp->ClearDispatcherQueue();
         }
      }

      if (this->mpc_CanTransportProtocolBroadcast != NULL)
      {
         this->mpc_CanTransportProtocolBroadcast->ClearDispatcherQueue();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get is initialized flag

   \return
   Is initialized flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComDriverProtocol::IsInitialized(void) const
{
   return this->mq_Initialized;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Re-connect to openSYDE node

   Required after losing the connection (e.g. after target reset)
   Assumptions (responsibility of the caller):
   * selected node exists
   * selected node is an openSYDE node

   \param[in]  orc_ServerId   node to re-connect to

   \return
   C_NO_ERR   re-connected
   C_BUSY     could not re-connect to node
   C_RANGE    node not found or no openSYDE protocol installed
   C_CONFIG   no transport protocol installed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::ReConnectNode(const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerId) const
{
   int32_t s32_Return = C_RANGE;
   bool q_Found;
   const uint32_t u32_ActiveNodeIndex = this->m_GetActiveIndex(orc_ServerId, q_Found);

   if (q_Found == true)
   {
      C_OscProtocolDriverOsy * const pc_ProtocolOsy = this->mc_OsyProtocols[u32_ActiveNodeIndex];
      if (pc_ProtocolOsy != NULL)
      {
         s32_Return = pc_ProtocolOsy->ReConnect();
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect from openSYDE node

   \param[in]  orc_ServerId   node to disconnect from

   \return
   C_NO_ERR   disconnected
   C_NOACT    could not re-connect to node
   C_RANGE    node not found or no openSYDE protocol installed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::DisconnectNode(const C_OscProtocolDriverOsyNode & orc_ServerId) const
{
   int32_t s32_Return = C_RANGE;
   bool q_Found;
   const uint32_t u32_ActiveNodeIndex = this->m_GetActiveIndex(orc_ServerId, q_Found);

   if (q_Found == true)
   {
      C_OscProtocolDriverOsy * const pc_ProtocolOsy = this->mc_OsyProtocols[u32_ActiveNodeIndex];
      if (pc_ProtocolOsy != NULL)
      {
         s32_Return = pc_ProtocolOsy->Disconnect();
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnecting from all openSYDE nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverProtocol::DisconnectNodes(void) const
{
   uint32_t u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->mc_OsyProtocols.size(); ++u32_Counter)
   {
      C_OscProtocolDriverOsy * const pc_ProtocolOsy = this->mc_OsyProtocols[u32_Counter];
      if (pc_ProtocolOsy != NULL)
      {
         pc_ProtocolOsy->Disconnect();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare for shutting down class

   To be called by child classes on shutdown, before they destroy all owned class instances
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverProtocol::PrepareForDestruction(void)
{
   //go through TP instances and let them know there will be no more dispatcher ...
   for (uint32_t u32_ItTp = 0; u32_ItTp < this->mc_TransportProtocols.size(); ++u32_ItTp)
   {
      C_OscProtocolDriverOsyTpCan * const pc_Tp =
         dynamic_cast<C_OscProtocolDriverOsyTpCan *>(this->mc_TransportProtocols[u32_ItTp]);
      //do we have a CAN TP ?
      if (pc_Tp != NULL)
      {
         pc_Tp->SetDispatcher(NULL); //we are about to destroy the dispatcher; make sure TP disconnects from it
      }
      else
      {
         C_OscProtocolDriverOsyTpIp * const pc_TpIp =
            dynamic_cast<C_OscProtocolDriverOsyTpIp *>(this->mc_TransportProtocols[u32_ItTp]);
         if (pc_TpIp != NULL)
         {
            pc_TpIp->SetDispatcher(NULL, 0U);
         }
      }
   }

   //also clean up broadcast instances:
   if (this->mpc_CanTransportProtocolBroadcast != NULL)
   {
      this->mpc_CanTransportProtocolBroadcast->SetDispatcher(NULL);
   }
   if (this->mpc_IpTransportProtocolBroadcast != NULL)
   {
      this->mpc_IpTransportProtocolBroadcast->SetDispatcher(NULL, 0U);
   }

   C_OscComDriverBase::PrepareForDestruction();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get openSYDE transport protocol of specified active node

   \param[in] ou32_NodeIndex  index of node within system definition

   \return
   ou32_NodeIndex out of range of system definition: NULL
   ou32_NodeIndex is not active: NULL

   else: pointer to openSYDE transport protocol
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolDriverOsyTpBase * C_OscComDriverProtocol::GetOsyTransportProtocol(const uint32_t ou32_NodeIndex)
{
   C_OscProtocolDriverOsyTpBase * pc_Tp = NULL;

   for (uint16_t u16_Index = 0U; u16_Index < mc_TransportProtocols.size(); u16_Index++)
   {
      if (mc_ActiveNodesIndexes[u16_Index] == ou32_NodeIndex)
      {
         pc_Tp = mc_TransportProtocols[u16_Index];
      }
   }

   return pc_Tp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the count of active nodes

   \return
   Count of active registered nodes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscComDriverProtocol::m_GetActiveNodeCount(void) const
{
   return this->mu32_ActiveNodeCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of node in list of active nodes

   If no active node is found that matches the passed absolute index the function will fail with an assertion.

   \param[in]     ou32_NodeIndex    absolute index of node within system description
   \param[out]    opq_Found         Optional flag if server id was found

   \return   index of node within list of active nodes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscComDriverProtocol::m_GetActiveIndex(const uint32_t ou32_NodeIndex, bool * const opq_Found) const
{
   uint32_t u32_Index;
   bool q_Found = false;

   for (u32_Index = 0U; u32_Index < this->mc_ActiveNodesIndexes.size(); ++u32_Index)
   {
      if (this->mc_ActiveNodesIndexes[u32_Index] == ou32_NodeIndex)
      {
         q_Found = true;
         break;
      }
   }

   if (opq_Found != NULL)
   {
      *opq_Found = q_Found;
   }
   else
   {
      tgl_assert(q_Found == true);
   }

   return u32_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of node in list of active nodes

   If no active node is found that matches the passed absolute index the function will fail with an assertion.

   \param[in]     orc_ServerId             Server id for communication
   \param[out]    orq_Found                Flag if server id was found

   \return   index of node within list of active nodes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscComDriverProtocol::m_GetActiveIndex(const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerId,
                                                  bool & orq_Found) const
{
   uint32_t u32_Counter;

   orq_Found = false;

   for (u32_Counter = 0U; u32_Counter < this->mc_ServerIds.size(); ++u32_Counter)
   {
      if (orc_ServerId == this->mc_ServerIds[u32_Counter])
      {
         // Index found
         orq_Found = true;
         break;
      }
   }

   return u32_Counter;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns pointer to CAN dispatcher

   \return
   Pointer to CAN dispatcher
*/
//----------------------------------------------------------------------------------------------------------------------
C_CanDispatcher * C_OscComDriverProtocol::m_GetCanDispatcher(void)
{
   return this->mpc_CanDispatcher;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns pointer to IP dispatcher

   \return
   Pointer to IP dispatcher
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscIpDispatcher * C_OscComDriverProtocol::m_GetIpDispatcher(void)
{
   return this->mpc_IpDispatcher;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the pointer to the openSYDE protocol of specific server id

   \param[in]     orc_ServerId             Server id for communication

   \return
   Valid pointer  Protocol for server found
   NULL           No protocol for server found
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolDriverOsy * C_OscComDriverProtocol::m_GetOsyProtocol(const C_OscProtocolDriverOsyNode & orc_ServerId) const
{
   C_OscProtocolDriverOsy * pc_Return = NULL;
   uint32_t u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->mc_ServerIds.size(); ++u32_Counter)
   {
      if (orc_ServerId == this->mc_ServerIds[u32_Counter])
      {
         // Index found
         if ((u32_Counter < this->mc_OsyProtocols.size()) &&
             (this->mc_OsyProtocols[u32_Counter] != NULL))
         {
            pc_Return = this->mc_OsyProtocols[u32_Counter];
            break;
         }
      }
   }

   return pc_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node name

   \param[in] ou32_ActiveNodeIndex   Active node index

   \return
   Node name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComDriverProtocol::m_GetActiveNodeName(const uint32_t ou32_ActiveNodeIndex) const
{
   C_SclString c_Retval = "Unknown";

   if (ou32_ActiveNodeIndex < this->mc_ActiveNodesIndexes.size())
   {
      const uint32_t u32_NodeIndex = this->mc_ActiveNodesIndexes[ou32_ActiveNodeIndex];
      if ((this->mpc_SysDef != NULL) &&
          (u32_NodeIndex < this->mpc_SysDef->c_Nodes.size()))
      {
         c_Retval = this->mpc_SysDef->c_Nodes[u32_NodeIndex].c_Properties.c_Name;
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a node into a session

   \param[in]     ou32_ActiveNode       active node index of vector mc_ActiveNodes
   \param[in]     ou8_SessionId         session ID to switch to
   \param[in]     oq_CheckForSession    checks the current session id on the server. only if it is different, the
                                        new session id will be set
   \param[out]    opu8_NrCode           if != NULL: negative response code

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Nodes has no openSYDE protocol
   C_COM       Communication problem
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_SetNodeSessionId(const uint32_t ou32_ActiveNode, const uint8_t ou8_SessionId,
                                                   const bool oq_CheckForSession, uint8_t * const opu8_NrCode) const
{
   C_OscProtocolDriverOsy * pc_ProtocolOsy = NULL;
   int32_t s32_Return;

   if (ou32_ActiveNode < this->mc_OsyProtocols.size())
   {
      pc_ProtocolOsy = this->mc_OsyProtocols[ou32_ActiveNode];
      s32_Return = this->m_SetNodeSessionId(pc_ProtocolOsy, ou8_SessionId, oq_CheckForSession, opu8_NrCode);
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a node into a session

   \param[in]     opc_ExistingProtocol  Protocol to use for communication
   \param[in]     ou8_SessionId         session ID to switch to
   \param[in]     oq_CheckForSession    checks the current session id on the server. only if it is different, the
                                        new session id will be set
   \param[out]    opu8_NrCode           if != NULL: negative response code

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Nodes has no openSYDE protocol
   C_COM       Communication problem
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_SetNodeSessionId(C_OscProtocolDriverOsy * const opc_ExistingProtocol,
                                                   const uint8_t ou8_SessionId, const bool oq_CheckForSession,
                                                   uint8_t * const opu8_NrCode) const
{
   int32_t s32_Return = C_CONFIG;

   if (this->mq_Initialized == true)
   {
      if (opc_ExistingProtocol != NULL)
      {
         bool q_SetNewSession = true;

         if (oq_CheckForSession == true)
         {
            uint8_t u8_CurrentSession;

            // Get the current session
            s32_Return = opc_ExistingProtocol->OsyReadActiveDiagnosticSession(u8_CurrentSession, opu8_NrCode);

            if (s32_Return == C_NO_ERR)
            {
               if (u8_CurrentSession == ou8_SessionId)
               {
                  // Session was already set.
                  q_SetNewSession = false;
               }
            }
            else if ((s32_Return != C_TIMEOUT) && (s32_Return != C_WARN))
            {
               // Do not change the C_TIMEOUT error
               s32_Return = C_COM;
            }
            else
            {
               // nothing to do
            }
         }
         else
         {
            s32_Return = C_NO_ERR;
         }

         if (s32_Return == C_NO_ERR)
         {
            if (q_SetNewSession == true)
            {
               // Set the session
               s32_Return = opc_ExistingProtocol->OsyDiagnosticSessionControl(ou8_SessionId, opu8_NrCode);

               if ((s32_Return != C_NO_ERR) &&
                   (s32_Return != C_TIMEOUT) &&
                   (s32_Return != C_WARN))
               {
                  // Do not change the C_TIMEOUT error
                  s32_Return = C_COM;
               }
            }
            else
            {
               // No change necessary
               s32_Return = C_NO_ERR;
            }
         }
      }
      else
      {
         s32_Return = C_NOACT;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a node into a session

   \param[in]        ou8_SessionId         session ID to switch to
   \param[in]        oq_CheckForSession    checks the current session id on the server. only if it is different, the
                                           new session id will be set
   \param[in,out]    orc_DefectNodeIndices List of active node indices which encountered an error

   Nodes with previous errors registered in orc_DefectNodeIndices will be skipped

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
   C_TIMEOUT   Expected response not received within timeout
               or at least one node was registered in orc_DefectNodeIndices
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_SetNodesSessionId(const uint8_t ou8_SessionId, const bool oq_CheckForSession,
                                                    std::set<uint32_t> & orc_DefectNodeIndices) const
{
   std::vector<uint32_t> c_AllActiveNodes;
   uint32_t u32_Counter;

   c_AllActiveNodes.resize(this->mc_ActiveNodesIndexes.size());
   for (u32_Counter = 0U; u32_Counter < c_AllActiveNodes.size(); ++u32_Counter)
   {
      c_AllActiveNodes[u32_Counter] = u32_Counter;
   }

   return this->m_SetNodesSessionId(c_AllActiveNodes, ou8_SessionId, oq_CheckForSession, orc_DefectNodeIndices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a node into a session

   \param[in]        orc_ActiveNodes       All indexes of active nodes to set the
                                           session (indexes of mc_ActiveNodesIndexes)
   \param[in]        ou8_SessionId         session ID to switch to
   \param[in]        oq_CheckForSession    checks the current session id on the server. only if it is different, the
                                           new session id will be set
   \param[in,out]    orc_DefectNodeIndices List of active node indices which encountered an error

   Nodes with previous errors registered in orc_DefectNodeIndices will be skipped

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
   C_TIMEOUT   Expected response not received within timeout
               or at least one node was registered in orc_DefectNodeIndices
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_SetNodesSessionId(const std::vector<uint32_t> & orc_ActiveNodes,
                                                    const uint8_t ou8_SessionId, const bool oq_CheckForSession,
                                                    std::set<uint32_t> & orc_DefectNodeIndices) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->mq_Initialized == true)
   {
      uint32_t u32_Counter;

      for (u32_Counter = 0U; u32_Counter < orc_ActiveNodes.size(); ++u32_Counter)
      {
         const uint32_t u32_ActiveNode = orc_ActiveNodes[u32_Counter];
         // Search the input values for a previous problem with the node
         // Further communication is only necessary if the node was ok in the first place
         if (orc_DefectNodeIndices.find(u32_ActiveNode) == orc_DefectNodeIndices.end())
         {
            const int32_t s32_Return =
               this->m_SetNodeSessionId(u32_ActiveNode, ou8_SessionId, oq_CheckForSession, NULL);

            if ((s32_Return != C_NO_ERR) && (s32_Return != C_NOACT))
            {
               // Do not change the C_TIMEOUT error
               if ((s32_Return != C_TIMEOUT) && (s32_Return != C_WARN))
               {
                  s32_Retval = C_COM;
               }
               else
               {
                  s32_Retval = s32_Return;
               }
               orc_DefectNodeIndices.insert(u32_ActiveNode);
            }
         }
         else
         {
            // It was a previous error. Only in case of a previous timeout it would be continued to here
            s32_Retval = C_TIMEOUT;
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NOACT)
   {
      // A server with no openSYDE protocol is no error
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a node into an expected session

   If ou8_ExpectedNeededSession session can not be set on the server (error response), the other
   session will be set.

   \param[in]     ou32_ActiveNode             Active node which session shall be changed
   \param[in]     ou8_ExpectedNeededSession   Expected and needed session.
                                              Two sessions possible:
                                              - hu8_DIAGNOSTIC_SESSION_PREPROGRAMMING
                                              - hu8_DIAGNOSTIC_SESSION_EXTENDED_DIAGNOSIS

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
   C_TIMEOUT   Expected response not received within timeout
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_SetNodeSessionIdWithExpectation(const uint32_t ou32_ActiveNode,
                                                                  const uint8_t ou8_ExpectedNeededSession) const
{
   // We need a new session
   int32_t s32_Return = this->m_SetNodeSessionId(ou32_ActiveNode, ou8_ExpectedNeededSession, true, NULL);

   if (s32_Return == C_WARN)
   {
      uint8_t u8_NewExpectedNeededSession;

      if (ou8_ExpectedNeededSession ==
          C_OscProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_PREPROGRAMMING)
      {
         u8_NewExpectedNeededSession =
            C_OscProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_EXTENDED_DIAGNOSIS;
      }
      else
      {
         u8_NewExpectedNeededSession = C_OscProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_PREPROGRAMMING;
      }

      // Special case: It is possible, that the routing node is not in the
      // server or in the flashloader as expected. It returns an error that it does not support the
      // session. Try the other possibility.
      s32_Return = this->m_SetNodeSessionId(ou32_ActiveNode, u8_NewExpectedNeededSession, false, NULL);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a node into a security level

   \param[in]     ou32_ActiveNode       active node index of vector mc_ActiveNodes
   \param[in]     ou8_SecurityLevel     level of requested security
   \param[out]    opu8_NrCode           if != NULL: negative response code

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly or
               PEM database was needed but not set.
   C_NOACT     Nodes has no openSYDE protocol
   C_COM       Communication problem
   C_WARN      Error response
   C_TIMEOUT   Expected response not received within timeout
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
               Detailed error codes are logged with opu8_NrCode
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_SetNodeSecurityAccess(const uint32_t ou32_ActiveNode, const uint8_t ou8_SecurityLevel,
                                                        uint8_t * const opu8_NrCode) const
{
   C_OscProtocolDriverOsy * const pc_ProtocolOsy = this->mc_OsyProtocols[ou32_ActiveNode];
   const int32_t s32_Return = this->m_SetNodeSecurityAccess(pc_ProtocolOsy, ou8_SecurityLevel, opu8_NrCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a node into a security level

   If the first request fails with the error hu8_NR_CODE_REQUIRED_TIME_DELAY_NOT_EXPIRED, the node is not ready for
   the security access request. The reason is to avoid brute force attacks. Waiting for one second before try again
   in this case.

   \param[in]     opc_ExistingProtocol  Protocol to use for communication
   \param[in]     ou8_SecurityLevel     level of requested security
   \param[out]    opu8_NrCode           if != NULL: negative response code

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly or
               PEM database was needed but not set.
   C_NOACT     Nodes has no openSYDE protocol
   C_COM       Communication problem
   C_WARN      Error response
   C_TIMEOUT   Expected response not received within timeout
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
               Detailed error codes are logged with opu8_NrCode
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_SetNodeSecurityAccess(C_OscProtocolDriverOsy * const opc_ExistingProtocol,
                                                        const uint8_t ou8_SecurityLevel,
                                                        uint8_t * const opu8_NrCode) const
{
   int32_t s32_Return = C_CONFIG;

   if (this->mq_Initialized == true)
   {
      if (opc_ExistingProtocol != NULL)
      {
         // Set the security level
         bool q_SecureMode;
         uint64_t u64_Seed;
         uint8_t ou8_SecurityAlgorithm;
         uint8_t u8_NrErrorCode = 0U;

         s32_Return = opc_ExistingProtocol->OsySecurityAccessRequestSeed(ou8_SecurityLevel, q_SecureMode, u64_Seed,
                                                                         ou8_SecurityAlgorithm, &u8_NrErrorCode);

         if (opu8_NrCode != NULL)
         {
            (*opu8_NrCode) = u8_NrErrorCode;
         }

         if ((s32_Return == C_WARN) &&
             (u8_NrErrorCode == C_OscProtocolDriverOsy::hu8_NR_CODE_REQUIRED_TIME_DELAY_NOT_EXPIRED))
         {
            // Special case: The server must wait for a second to allow a request after a reset.
            // In some scenarios the normal waiting times after resets are not enough (this times are not calculated for
            // this security scenario), so wait in this case for another second and try again
            osc_write_log_warning("Security Access", "The node was not ready for the Security Access request. Trying"
                                  " after waiting for another second again.");

            stw::tgl::TglSleep(1000);

            s32_Return = opc_ExistingProtocol->OsySecurityAccessRequestSeed(ou8_SecurityLevel, q_SecureMode, u64_Seed,
                                                                            ou8_SecurityAlgorithm, opu8_NrCode);
         }

         if (s32_Return == C_NO_ERR)
         {
            if (q_SecureMode == false)
            {
               const uint32_t u32_KEY = 23U; // fixed in UDS stack for non secure mode
               if (u64_Seed != 42U)
               {
                  C_SclString c_Tmp;
                  c_Tmp.PrintFormatted("Received seed in non secure mode does not match the "
                                       "expected value, expected: 42, got %i", u64_Seed);
                  // Should be a fixed value too
                  osc_write_log_warning("Security Access", c_Tmp.c_str());
               }

               s32_Return = opc_ExistingProtocol->OsySecurityAccessSendKey(ou8_SecurityLevel, u32_KEY, opu8_NrCode);
            }
            else
            {
               //check pem database on NULL
               if (mpc_SecurityPemDb != NULL)
               {
                  //we need the server's certificate snr to look up the correct key
                  std::vector<uint8_t> c_CertSnr;
                  s32_Return = opc_ExistingProtocol->OsyReadCertificateSerialNumber(c_CertSnr, opu8_NrCode);

                  if (s32_Return == C_NO_ERR)
                  {
                     //get PEM file by serial number from database
                     const C_OscSecurityPemKeyInfo * const pc_PemKeyInfo =
                        this->mpc_SecurityPemDb->GetPemFileBySerialNumber(c_CertSnr);

                     if (pc_PemKeyInfo != NULL)
                     {
                        std::vector<uint8_t> c_Signature;
                        std::vector<uint8_t> c_RandomValue;
                        std::vector<uint8_t> c_PrivKey;
                        c_Signature.resize(128, 0U);
                        c_RandomValue.resize(8, 0U);

                        c_RandomValue[0] = static_cast<uint8_t>(u64_Seed >> 56U);
                        c_RandomValue[1] = static_cast<uint8_t>(u64_Seed >> 48U);
                        c_RandomValue[2] = static_cast<uint8_t>(u64_Seed >> 40U);
                        c_RandomValue[3] = static_cast<uint8_t>(u64_Seed >> 32U);
                        c_RandomValue[4] = static_cast<uint8_t>(u64_Seed >> 24U);
                        c_RandomValue[5] = static_cast<uint8_t>(u64_Seed >> 16U);
                        c_RandomValue[6] = static_cast<uint8_t>(u64_Seed >> 8U);
                        c_RandomValue[7] = static_cast<uint8_t>(u64_Seed);

                        //get private key from PEM file
                        c_PrivKey = pc_PemKeyInfo->GetPrivKeyTextDecoded();

                        //calculate RSA signature with private key and random value from server (u64_Seed)
                        s32_Return = C_OscSecurityRsa::h_SignSignature(c_PrivKey, c_RandomValue, c_Signature);

                        if (s32_Return != C_NO_ERR)
                        {
                           C_SclString c_Tmp;
                           c_Tmp.PrintFormatted("Error on calculating RSA signature: %d", s32_Return);
                           osc_write_log_error("Security Access", c_Tmp.c_str());
                           s32_Return = C_CHECKSUM;
                        }
                        else
                        {
                           //sanity check: calculated rsa signature 128 byte long?
                           if (c_Signature.size() == 128U)
                           {
                              //send signature back to server
                              s32_Return = opc_ExistingProtocol->OsySecurityAccessSendKey(ou8_SecurityLevel,
                                                                                          c_Signature,
                                                                                          opu8_NrCode);
                              if (s32_Return != C_NO_ERR)
                              {
                                 C_SclString c_Tmp;
                                 c_Tmp.PrintFormatted("Error on calculating RSA signature: %d", s32_Return);
                                 osc_write_log_error("Security Access", c_Tmp.c_str());
                                 s32_Return = C_CHECKSUM;
                              }
                           }
                        }
                     }
                     else
                     {
                        osc_write_log_error("Security Access", "No PEM file found for received serial number.");
                        s32_Return = C_CHECKSUM;
                     }
                  }
               }
               else
               {
                  osc_write_log_error("Security Access", "PEM database not initialized.");
                  s32_Return = C_CONFIG;
               }
            }
         }

         if ((s32_Return != C_NO_ERR) &&
             (s32_Return != C_TIMEOUT) &&
             (s32_Return != C_WARN) &&
             (s32_Return != C_CONFIG) &&
             (s32_Return != C_CHECKSUM)) //security related error
         {
            s32_Return = C_COM;
         }
      }
      else
      {
         s32_Return = C_NOACT;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets all nodes into a specific security level

   \param[in]     ou8_SecurityLevel     level of requested security
   \param[out]    orc_ErrorActiveNodes  All active node indexes of nodes which can not be reached

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
   C_TIMEOUT   Expected response not received within timeout
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
               Detailed error codes are logged with opu8_NrCode
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_SetNodesSecurityAccess(const uint8_t ou8_SecurityLevel,
                                                         std::set<uint32_t> & orc_ErrorActiveNodes) const
{
   std::vector<uint32_t> c_AllActiveNodes;
   uint32_t u32_Counter;

   c_AllActiveNodes.resize(this->mc_ActiveNodesIndexes.size());
   for (u32_Counter = 0U; u32_Counter < c_AllActiveNodes.size(); ++u32_Counter)
   {
      c_AllActiveNodes[u32_Counter] = u32_Counter;
   }

   return this->m_SetNodesSecurityAccess(c_AllActiveNodes, ou8_SecurityLevel, orc_ErrorActiveNodes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets all nodes into a specific security level

   \param[in]     orc_ActiveNodes       All indexes of active nodes to set the
                                        security access (indexes of mc_ActiveNodesIndexes)
   \param[in]     ou8_SecurityLevel     level of requested security
   \param[out] orc_ErrorActiveNodes     All active node indexes of nodes which can not be reached

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
   C_TIMEOUT   Expected response not received within timeout
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
               Detailed error codes are logged with opu8_NrCode
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_SetNodesSecurityAccess(const std::vector<uint32_t> & orc_ActiveNodes,
                                                         const uint8_t ou8_SecurityLevel,
                                                         std::set<uint32_t> & orc_ErrorActiveNodes) const
{
   int32_t s32_Return = C_CONFIG;

   if (this->mq_Initialized == true)
   {
      uint32_t u32_Counter;

      s32_Return = C_NOACT;

      for (u32_Counter = 0U; u32_Counter < orc_ActiveNodes.size(); ++u32_Counter)
      {
         const uint32_t u32_ActiveNode = orc_ActiveNodes[u32_Counter];
         s32_Return = this->m_SetNodeSecurityAccess(u32_ActiveNode, ou8_SecurityLevel, NULL);

         if ((s32_Return != C_NO_ERR) &&
             (s32_Return != C_NOACT))
         {
            //Store invalid node
            if (u32_ActiveNode < this->mc_ActiveNodesIndexes.size())
            {
               orc_ErrorActiveNodes.insert(this->mc_ActiveNodesIndexes[u32_ActiveNode]);
            }
            // Do not change the C_TIMEOUT and C_CHECKSUM error
            if ((s32_Return != C_TIMEOUT) &&
                (s32_Return != C_CHECKSUM))
            {
               s32_Return = C_COM;
            }
            break;
         }
      }
   }

   if (s32_Return == C_NOACT)
   {
      // A server with no openSYDE protocol is no error
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize the necessary routing configuration for IP to IP routing for a specific node

   Prepares the active nodes on the route with its configuration for IP to IP routing.

   Be careful: In case of using IP to IP routing all previous CAN routing configurations will be reseted and closed
               Call m_StartRoutingIp2Ip before m_StartRouting!
               Both functions must be called in correct order to get routing work in all cases.

   \param[in]     ou32_ActiveNode             active node index of vector mc_ActiveNodes
   \param[out]    opu32_ErrorActiveNodeIndex  optional pointer for active node index which caused the error on starting
                                              routing if an error occurred

   \return
   C_NO_ERR    IP to IP routing started
               No IP to IP routing necessary
   C_CONFIG    Pre-requisites not correct; e.g. driver not initialized
   C_WARN      Error response
   C_BUSY      Connection to at least one server failed
   C_COM       Communication problem
   C_TIMEOUT   Expected response not received within timeout
   C_RD_WR     Unexpected content in response
   C_NOACT     At least one node does not support Ethernet to Ethernet routing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_StartRoutingIp2Ip(const uint32_t ou32_ActiveNode,
                                                    uint32_t * const opu32_ErrorActiveNodeIndex)
{
   int32_t s32_Return = C_NO_ERR;
   bool q_Started = false;

   const C_OscRoutingRoute & rc_ActRoute = this->mc_Routes[ou32_ActiveNode];

   if (rc_ActRoute.c_VecRoutePoints.size() > 0)
   {
      if ((this->mpc_SysDef != NULL) &&
          (this->mc_ActiveNodesIndexes[ou32_ActiveNode] < this->mpc_SysDef->c_Nodes.size()))
      {
         C_OscProtocolDriverOsy * pc_ProtocolOsy = NULL;
         uint32_t u32_OsyRoutingTarget;
         bool q_Found;

         if (this->m_IsRoutingSpecificNecessary(this->mpc_SysDef->c_Nodes[this->mc_ActiveNodesIndexes[ou32_ActiveNode]])
             ==
             true)
         {
            // Specific protocol necessary. Routing dispatcher necessary.
            // Use the last openSYDE node protocol instance for the IP to IP routing
            u32_OsyRoutingTarget = this->m_GetActiveIndex(
               rc_ActRoute.c_VecRoutePoints[rc_ActRoute.c_VecRoutePoints.size() - 1].u32_NodeIndex, &q_Found);
         }
         else
         {
            // No error. No specific protocol necessary. We need only openSYDE protocol.
            u32_OsyRoutingTarget = ou32_ActiveNode;
            q_Found = true;
         }

         if (q_Found == true)
         {
            // Using the same connection for configuring the router and the final openSYDE target
            pc_ProtocolOsy =
               dynamic_cast<C_OscProtocolDriverOsy *>(this->mc_OsyProtocols[u32_OsyRoutingTarget]);
         }

         if (pc_ProtocolOsy != NULL)
         {
            // In case of Ethernet save the number of protocols which use the same TCP connection
            const std::map<uint32_t, uint32_t>::const_iterator c_Iter =
               this->mc_ActiveNodeIp2IpDispatcher.find(ou32_ActiveNode);

            if ((c_Iter != this->mc_ActiveNodeIp2IpDispatcher.end()) &&
                (c_Iter->second < this->mc_ActiveNodeIp2IpDispatcherNodeCount.size()))
            {
               this->mc_ActiveNodeIp2IpDispatcherNodeCount[c_Iter->second] =
                  this->mc_ActiveNodeIp2IpDispatcherNodeCount[c_Iter->second] + 1U;
            }
         }

         // Connect only one time if connection was used already for the node itself or an legacy routing target
         if ((pc_ProtocolOsy != NULL) &&
             (pc_ProtocolOsy->IsConnected() != C_NO_ERR))
         {
            uint32_t u32_RoutePoint;

            for (u32_RoutePoint = 0U; u32_RoutePoint < rc_ActRoute.c_VecRoutePoints.size(); ++u32_RoutePoint)
            {
               const C_OscRoutingRoutePoint & rc_RoutePoint = rc_ActRoute.c_VecRoutePoints[u32_RoutePoint];

               // Start routing for each hop with IP to IP routing
               if ((rc_RoutePoint.e_InInterfaceType == C_OscSystemBus::eETHERNET) &&
                   (rc_RoutePoint.e_OutInterfaceType == C_OscSystemBus::eETHERNET))
               {
                  const uint32_t u32_RouterActiveIndex =
                     this->m_GetActiveIndex(rc_RoutePoint.u32_NodeIndex, &q_Found);
                  uint32_t u32_NextEthernetNode = 0U;
                  bool q_NextOneError = false;

                  if (q_Found == true)
                  {
                     const uint32_t u32_NextRoutePoint = u32_RoutePoint + 1U;

                     // Clear all queues. In case of CAN tp the change causes that more than one queue receives
                     // service responses
                     this->ClearDispatcherQueue();
                     // Because of using the same TCP connection with the IP of the router,
                     // the transport protocol must be adapted to configure the router
                     pc_ProtocolOsy->SetNodeIdentifiers(this->mc_ClientId,
                                                        this->mc_ServerIds[u32_RouterActiveIndex]);

                     if (u32_NextRoutePoint < rc_ActRoute.c_VecRoutePoints.size())
                     {
                        // Next node is a further routing node
                        u32_NextEthernetNode =
                           this->m_GetActiveIndex(rc_ActRoute.c_VecRoutePoints[u32_NextRoutePoint].u32_NodeIndex,
                                                  &q_Found);

                        if (q_Found == false)
                        {
                           s32_Return = C_CONFIG;
                        }
                     }
                     else
                     {
                        // Next node is target node
                        u32_NextEthernetNode = ou32_ActiveNode;
                     }
                  }
                  else
                  {
                     s32_Return = C_CONFIG;
                  }

                  if (s32_Return == C_NO_ERR)
                  {
                     // Do not disconnect after. The connection is necessary to hold the routing alive.
                     s32_Return = pc_ProtocolOsy->ReConnect();
                  }

                  // Check if node supports IP to IP routing
                  if (s32_Return == C_NO_ERR)
                  {
                     C_OscProtocolDriverOsy::C_ListOfFeatures c_Features;
                     s32_Return = pc_ProtocolOsy->OsyReadListOfFeatures(c_Features);

                     if (s32_Return == C_NO_ERR)
                     {
                        if (c_Features.q_EthernetToEthernetRoutingSupported == false)
                        {
                           // Node is not capable for Ethernet to Ethernet routing
                           s32_Return = C_NOACT;
                        }
                     }
                  }

                  if (s32_Return == C_NO_ERR)
                  {
                     s32_Return =
                        this->m_SetNodeSessionIdWithExpectation(u32_OsyRoutingTarget, this->m_GetRoutingSessionId());
                  }

                  if (s32_Return == C_NO_ERR)
                  {
                     // We need an other security level
                     s32_Return = this->m_SetNodeSecurityAccess(u32_OsyRoutingTarget, 5, NULL);
                  }

                  if (s32_Return == C_NO_ERR)
                  {
                     // Get the Bus IDs
                     if ((rc_RoutePoint.u32_InBusIndex < this->mpc_SysDef->c_Buses.size()) &&
                         (rc_RoutePoint.u32_OutBusIndex < this->mpc_SysDef->c_Buses.size()))
                     {
                        const uint8_t u8_SourceBusId =
                           this->mpc_SysDef->c_Buses[rc_RoutePoint.u32_InBusIndex].u8_BusId;
                        const uint8_t u8_TargetBusId =
                           this->mpc_SysDef->c_Buses[rc_RoutePoint.u32_OutBusIndex].u8_BusId;

                        // Configuration of the routing to the next Ethernet node
                        s32_Return = pc_ProtocolOsy->OsySetRouteIp2IpCommunication(
                           static_cast<uint8_t>(rc_RoutePoint.e_OutInterfaceType),
                           rc_RoutePoint.u8_OutInterfaceNumber,
                           u8_SourceBusId,
                           u8_TargetBusId,
                           this->mc_ServerIpAddresses[u32_NextEthernetNode].au8_IpAddress);
                     }
                     else
                     {
                        s32_Return = C_CONFIG;
                     }
                  }

                  // Poll for success
                  if (s32_Return == C_NO_ERR)
                  {
                     const uint32_t u32_StartTime = TglGetTickCount();

                     do
                     {
                        uint8_t u8_Status;
                        s32_Return = pc_ProtocolOsy->OsyCheckRouteIp2IpCommunication(u8_Status);

                        if ((s32_Return != C_NO_ERR) ||
                            (u8_Status >= C_OscProtocolDriverOsy::hu8_OSY_IP_2_IP_STATUS_ERROR))
                        {
                           // Finished or error
                           q_Started = true;

                           if ((s32_Return == C_NO_ERR) &&
                               (u8_Status == C_OscProtocolDriverOsy::hu8_OSY_IP_2_IP_STATUS_ERROR))
                           {
                              // Error of service
                              s32_Return = C_WARN;
                              // If this service will not be finished without error, the router node is available
                              // and the target of the next point is not reachable
                              q_NextOneError = true;
                           }
                           else
                           {
                              q_NextOneError = false;
                           }
                           break;
                        }
                        else
                        {
                           // No error and not finished yet, set timeout in case of loop abort condition
                           s32_Return = C_TIMEOUT;
                           // If this service will not be finished in its time, the router node is available and
                           // the target of the next point is not reachable
                           q_NextOneError = true;
                           // Let the node do its work
                           TglSleep(20);
                        }
                     }
                     // TODO BAY: Timeout time!
                     while (TglGetTickCount() < (u32_StartTime + 1000U));
                  }

                  if ((s32_Return != C_NO_ERR) && (opu32_ErrorActiveNodeIndex != NULL))
                  {
                     if (q_NextOneError == false)
                     {
                        // Current router node caused the error
                        *opu32_ErrorActiveNodeIndex = u32_RouterActiveIndex;
                     }
                     else
                     {
                        // The next one is to blame
                        *opu32_ErrorActiveNodeIndex = u32_NextEthernetNode;
                     }
                  }

                  // Clear all queues. In case of CAN tp the change causes that more than one queue receives
                  // service responses
                  this->ClearDispatcherQueue();
                  // Configuring IP to IP routing finished, reset the transport protocol configuration
                  pc_ProtocolOsy->SetNodeIdentifiers(this->mc_ClientId, this->mc_ServerIds[u32_OsyRoutingTarget]);
               }
            }
         }
      }
      else
      {
         s32_Return = C_CONFIG;
      }
   }

   if ((s32_Return == C_NO_ERR) &&
       (q_Started == true))
   {
      osc_write_log_info("Start IP to IP Routing",
                         "IP to IP Routing to node " +
                         C_SclString::IntToStr(this->mc_ActiveNodesIndexes[ou32_ActiveNode]) +
                         " over " + C_SclString::IntToStr(this->mc_Routes[ou32_ActiveNode].c_VecRoutePoints.size()) +
                         " routing points started.");
   }
   else if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Start IP to IP Routing",
                          "Error on starting IP to IP routing to node " + C_SclString::IntToStr(
                             this->mc_ActiveNodesIndexes[ou32_ActiveNode]) + " with error " +
                          C_OscLoggingHandler::h_StwError(s32_Return));
   }
   else
   {
      // Nothing to do
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize the necessary routing configuration to start the routing for a specific node

   Prepares the active nodes on the route with its routing configurations if necessary
   Three different types of routing:
   - openSYDE routing for an openSYDE server
   - legacy routing for a non openSYDE server
   - legacy routing for a non openSYDE after openSYDE routing to an openSYDE server

   The routing must be activated to all nodes after each node
   For example:
   Node 1 - Bus 1 - Node 2 - Bus 2 - Node 3 - Bus 3 - Node 4
   The routing must be activated in this combination:
   Node 1 -> Node 2 (To configure the routing on Node 2)
   Node 1 -> Node 3 (To configure the routing on Node 3)
   Node 1 -> Node 4 (For real purpose and communication)
   Node 2 -> Node 3 (To configure the routing on Node 3)
   Node 2 -> Node 4 (For real purpose and communication)
   Node 3 -> Node 4 (For real purpose and communication)

   \param[in]     ou32_ActiveNode             active node index of vector mc_ActiveNodes
   \param[out]    opu32_ErrorActiveNodeIndex  optional pointer for active node index which caused the error on starting
                                              routing if an error occurred

   \return
   C_NO_ERR   request sent, positive response received or no routing necessary
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send request (e.g. Tx buffer full)
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_RANGE    node index out of range
   C_COM      communication driver reported error
   C_CHECKSUM Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_StartRouting(const uint32_t ou32_ActiveNode,
                                               uint32_t * const opu32_ErrorActiveNodeIndex)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mc_Routes[ou32_ActiveNode].c_VecRoutePoints.size() > 0)
   {
      s32_Return = C_CONFIG;
      // Routing is necessary

      if (this->mpc_SysDef != NULL)
      {
         const C_OscNode * const pc_Node = &this->mpc_SysDef->c_Nodes[this->mc_ActiveNodesIndexes[ou32_ActiveNode]];

         if (this->mc_ActiveNodesIndexes[ou32_ActiveNode] < this->mpc_SysDef->c_Nodes.size())
         {
            bool q_OsyRouting = false;
            C_OscRoutingRoute c_ActRoute = this->mc_Routes[ou32_ActiveNode];
            C_OscCanDispatcherOsyRouter * pc_RoutingDispatcher = NULL;
            const C_OscRoutingRoutePoint c_LastNodeOfRouting =
               c_ActRoute.c_VecRoutePoints[c_ActRoute.c_VecRoutePoints.size() - 1];
            const uint32_t u32_ActiveLastNode = this->m_GetActiveIndex(c_LastNodeOfRouting.u32_NodeIndex);
            C_OscProtocolDriverOsy * pc_ProtocolOsyOfLastNodeOfRouting =
               dynamic_cast<C_OscProtocolDriverOsy *>(this->mc_OsyProtocols[u32_ActiveLastNode]);
            bool q_EthernetRouter = false;
            uint32_t u32_ActiveOsyTargetNode = ou32_ActiveNode;

            s32_Return = this->m_StartRoutingSpecific(ou32_ActiveNode, pc_Node, c_LastNodeOfRouting,
                                                      pc_ProtocolOsyOfLastNodeOfRouting,
                                                      &pc_RoutingDispatcher);

            if (s32_Return == C_NO_ERR)
            {
               // Specific protocol necessary. Routing dispatcher necessary.
               u32_ActiveOsyTargetNode = u32_ActiveLastNode;

               if (c_ActRoute.c_VecRoutePoints.size() > 1)
               {
                  // Both routing variants necessary
                  q_OsyRouting = true;

                  // Remove the last routing point. This last step will be made by the legacy routing
                  c_ActRoute.c_VecRoutePoints.resize(c_ActRoute.c_VecRoutePoints.size() - 1);
               }
            }
            else if (s32_Return == C_NOACT)
            {
               // No error. No specific protocol necessary. We need only openSYDE protocol.
               s32_Return = C_NO_ERR;
               pc_ProtocolOsyOfLastNodeOfRouting = NULL;
               q_OsyRouting = true;
            }
            else
            {
               pc_ProtocolOsyOfLastNodeOfRouting = NULL;
            }

            // Start the routing
            if ((s32_Return == C_NO_ERR) &&
                (q_OsyRouting == true))
            {
               // Setup and start openSYDE routing configuration
               uint32_t u32_CounterRoutePoints;
               const uint32_t u32_SourceBusIndex = c_ActRoute.c_VecRoutePoints[0].u32_InBusIndex;

               for (u32_CounterRoutePoints = 0U;
                    u32_CounterRoutePoints < c_ActRoute.c_VecRoutePoints.size();
                    ++u32_CounterRoutePoints)
               {
                  const C_OscRoutingRoutePoint & rc_Point = c_ActRoute.c_VecRoutePoints[u32_CounterRoutePoints];

                  // In case of Ethernet as out interface the ip to ip routing handles this server
                  if (rc_Point.e_OutInterfaceType != C_OscSystemBus::eETHERNET)
                  {
                     const uint32_t u32_ActiveRouterNode = this->m_GetActiveIndex(rc_Point.u32_NodeIndex);

                     C_OscProtocolDriverOsy * const pc_ProtocolOsyTarget =
                        dynamic_cast<C_OscProtocolDriverOsy *>(this->mc_OsyProtocols[u32_ActiveOsyTargetNode]);

                     if (pc_ProtocolOsyTarget != NULL)
                     {
                        // Clear all queues. In case of CAN tp the change causes that more than one queue receives
                        // service responses
                        this->ClearDispatcherQueue();
                        // Because of using the same connection,
                        // the protocol must be adapted to configure the router
                        pc_ProtocolOsyTarget->SetNodeIdentifiers(this->mc_ClientId,
                                                                 this->mc_ServerIds[u32_ActiveRouterNode]);

                        if (q_EthernetRouter == false)
                        {
                           // We have to reconnect to the server in case of Ethernet
                           // But only if IP to IP routing did not connected already.
                           s32_Return = pc_ProtocolOsyTarget->ReConnect();
                        }

                        if (s32_Return == C_NO_ERR)
                        {
                           // We need a new session
                           s32_Return = this->m_SetNodeSessionIdWithExpectation(u32_ActiveOsyTargetNode,
                                                                                this->m_GetRoutingSessionId());
                        }

                        if (s32_Return == C_NO_ERR)
                        {
                           // We need an other security level
                           s32_Return = this->m_SetNodeSecurityAccess(u32_ActiveOsyTargetNode, 5, NULL);
                        }

                        if (s32_Return == C_NO_ERR)
                        {
                           uint32_t u32_CounterTargetPoint;
                           for (u32_CounterTargetPoint = u32_CounterRoutePoints;
                                u32_CounterTargetPoint < c_ActRoute.c_VecRoutePoints.size();
                                ++u32_CounterTargetPoint)
                           {
                              const C_OscRoutingRoutePoint & rc_PointTarget =
                                 c_ActRoute.c_VecRoutePoints[u32_CounterTargetPoint];

                              // Get the Bus IDs
                              if ((u32_SourceBusIndex < this->mpc_SysDef->c_Buses.size()) &&
                                  (rc_PointTarget.u32_OutBusIndex < this->mpc_SysDef->c_Buses.size()))
                              {
                                 const uint8_t u8_SourceBusId = this->mpc_SysDef->c_Buses[u32_SourceBusIndex].u8_BusId;
                                 const uint8_t u8_TargetBusId =
                                    this->mpc_SysDef->c_Buses[rc_PointTarget.u32_OutBusIndex].u8_BusId;

                                 // Configure the real node for each layer
                                 s32_Return = pc_ProtocolOsyTarget->OsySetRouteDiagnosisCommunication(
                                    static_cast<uint8_t>(rc_Point.e_InInterfaceType),
                                    rc_Point.u8_InInterfaceNumber,
                                    static_cast<uint8_t>(rc_Point.e_OutInterfaceType),
                                    rc_Point.u8_OutInterfaceNumber,
                                    u8_SourceBusId,
                                    u8_TargetBusId);
                              }
                              else
                              {
                                 s32_Return = C_CONFIG;
                              }

                              if (s32_Return != C_NO_ERR)
                              {
                                 break;
                              }
                           }
                        }

                        if ((s32_Return != C_NO_ERR) && (opu32_ErrorActiveNodeIndex != NULL))
                        {
                           *opu32_ErrorActiveNodeIndex = u32_ActiveRouterNode;
                        }

                        // Clear all queues. In case of CAN tp the change causes that more than one queue receives
                        // service responses
                        this->ClearDispatcherQueue();
                        // Configuring of routing finished, reset the protocol configuration
                        pc_ProtocolOsyTarget->SetNodeIdentifiers(this->mc_ClientId,
                                                                 this->mc_ServerIds[u32_ActiveOsyTargetNode]);
                     }
                     else
                     {
                        s32_Return = C_CONFIG;
                     }
                  }
                  else
                  {
                     q_EthernetRouter = true;
                  }

                  if (s32_Return != C_NO_ERR)
                  {
                     break;
                  }
               }
            }

            // Init of the legacy routing
            if ((pc_RoutingDispatcher != NULL) &&
                (pc_ProtocolOsyOfLastNodeOfRouting != NULL) &&
                (s32_Return == C_NO_ERR))
            {
               // Special case: No openSYDE routing, but legacy routing.
               // In case of Ethernet the connection to the legacy routing server must be established.
               if ((q_OsyRouting == false) &&
                   (c_ActRoute.c_VecRoutePoints.size() == 1))
               {
                  C_OscProtocolDriverOsy * const pc_ProtocolOsyRouter =
                     dynamic_cast<C_OscProtocolDriverOsy *>(this->mc_OsyProtocols[u32_ActiveLastNode]);

                  if ((pc_ProtocolOsyRouter != NULL) &&
                      (q_EthernetRouter == false))
                  {
                     s32_Return = pc_ProtocolOsyRouter->ReConnect();
                  }
               }

               if (s32_Return == C_NO_ERR)
               {
                  // Use the index for the the routing dispatcher
                  // We need a new session
                  s32_Return =
                     this->m_SetNodeSessionIdWithExpectation(u32_ActiveLastNode, this->m_GetRoutingSessionId());

                  if (s32_Return == C_NO_ERR)
                  {
                     // We need an other security level
                     s32_Return = this->m_SetNodeSecurityAccess(u32_ActiveLastNode, 5, NULL);
                  }

                  if (s32_Return == C_NO_ERR)
                  {
                     // Start legacy routing
                     s32_Return = pc_RoutingDispatcher->CAN_Init();
                  }
               }

               if ((s32_Return != C_NO_ERR) && (opu32_ErrorActiveNodeIndex != NULL))
               {
                  *opu32_ErrorActiveNodeIndex = u32_ActiveLastNode;
               }
            }

            if (s32_Return == C_NO_ERR)
            {
               const std::map<uint32_t,
                              uint32_t>::const_iterator c_ItRouterNode = this->mc_ActiveNodeIp2CanDispatcher.find(
                  ou32_ActiveNode);

               if (c_ItRouterNode != this->mc_ActiveNodeIp2CanDispatcher.end())
               {
                  // Special case: IP to CAN routing with an dispatcher of an other node
                  if (c_ItRouterNode->second < this->mc_ActiveNodeIp2CanDispatcherNodeCount.size())
                  {
                     this->mc_ActiveNodeIp2CanDispatcherNodeCount[c_ItRouterNode->second] =
                        this->mc_ActiveNodeIp2CanDispatcherNodeCount[c_ItRouterNode->second] + 1U;
                  }
                  else
                  {
                     s32_Return = C_CONFIG;
                  }
               }

               // Routing started. Save the active node index of the last router node on the route
               this->mc_ActiveNodesLastCanRouters.push_back(u32_ActiveLastNode);
            }
         } //lint !e429  //pc_RoutingDispatcher will be saved in mc_LegacyRouterDispatchers and deleted by destructor
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("Start Routing",
                         "Routing to node " + C_SclString::IntToStr(this->mc_ActiveNodesIndexes[ou32_ActiveNode]) +
                         " over " + C_SclString::IntToStr(this->mc_Routes[ou32_ActiveNode].c_VecRoutePoints.size()) +
                         " routing points started.");
   }
   else
   {
      osc_write_log_error("Start Routing",
                          "Error on starting routing to node " + C_SclString::IntToStr(
                             this->mc_ActiveNodesIndexes[ou32_ActiveNode]) + " with error " +
                          C_OscLoggingHandler::h_StwError(s32_Return));
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stops the entire routing configuration for one specific node

   \param[in]   ou32_ActiveNode       node index to stop routing for

   \return
   C_NO_ERR   routing stopped
   C_RANGE    node index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverProtocol::m_StopRouting(const uint32_t ou32_ActiveNode)
{
   if ((ou32_ActiveNode < this->mc_Routes.size()) &&
       (this->mc_Routes[ou32_ActiveNode].c_VecRoutePoints.size() > 0) &&
       (this->mpc_SysDef != NULL))
   {
      const C_OscNode * const pc_Node = &this->mpc_SysDef->c_Nodes[this->mc_ActiveNodesIndexes[ou32_ActiveNode]];
      const C_OscRoutingRoute & rc_ActRoute = this->mc_Routes[ou32_ActiveNode];
      const uint32_t u32_LastRoutePointIndex = static_cast<uint32_t>(rc_ActRoute.c_VecRoutePoints.size() - 1);
      uint32_t u32_ActiveOsyTargetNode;
      bool q_Found;

      // Stop specific routing first. It needs the openSYDE routing if configured
      this->m_StopRoutingSpecific(ou32_ActiveNode);

      if (this->m_IsRoutingSpecificNecessary(*pc_Node) == true)
      {
         // Specific protocol necessary. Routing dispatcher necessary.
         // Use the last openSYDE node protocol instance for the IP to IP routing
         u32_ActiveOsyTargetNode = this->m_GetActiveIndex(
            rc_ActRoute.c_VecRoutePoints[rc_ActRoute.c_VecRoutePoints.size() - 1].u32_NodeIndex, &q_Found);
      }
      else
      {
         // No error. No specific protocol necessary. We need only openSYDE protocol.
         u32_ActiveOsyTargetNode = ou32_ActiveNode;
         q_Found = true;
      }

      if (q_Found == true)
      {
         int32_t s32_CounterRoutePoints;

         for (s32_CounterRoutePoints = static_cast<int32_t>(u32_LastRoutePointIndex);
              s32_CounterRoutePoints >= 0;
              --s32_CounterRoutePoints)
         {
            // Stop the routing from behind
            if (this->m_StopRoutingOfRoutingPoint(ou32_ActiveNode, u32_ActiveOsyTargetNode,
                                                  rc_ActRoute.c_VecRoutePoints[s32_CounterRoutePoints],
                                                  (s32_CounterRoutePoints == 0)) != C_NO_ERR)
            {
               break;
            }
         }
      }

      osc_write_log_info("Stop Routing",
                         "Routing to node " + C_SclString::IntToStr(this->mc_ActiveNodesIndexes[ou32_ActiveNode]) +
                         " stopped.");
   }

   this->mc_ActiveNodesLastCanRouters.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stops the entire routing configuration for all active nodes

   Difference to m_StopRouting:
   Stopping the routing configuration from behind for all nodes.

   For example:

   Client - A -|- B -|- C
               |     |- D
               |
               |- C -|- E

   Stopping in this direction will be done with m_StopRoutingOfActiveNodes:
   B - C
   B - D
   C - E
   A - B
   A - C

   Calling m_StopRouting for all nodes would cause following problem: The routing of one route can be necessary
   to get to an other routing point for a clean stopping. The stop service will close all active routing
   configurations on one node.

   For example closing A - B will close A - C as well. C - E could no be stopped properly.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverProtocol::m_StopRoutingOfActiveNodes(void)
{
   if (this->mpc_SysDef != NULL)
   {
      uint32_t u32_ActiveNode;
      int32_t s32_LastRouteHop = 0U;
      int32_t s32_RouteHopCounter;

      std::vector<int32_t> c_ActiveOsyTargetNodes;

      c_ActiveOsyTargetNodes.resize(this->mc_ActiveNodesIndexes.size(), -1);

      for (u32_ActiveNode = 0U; u32_ActiveNode < this->mc_ActiveNodesIndexes.size(); ++u32_ActiveNode)
      {
         const C_OscRoutingRoute & rc_ActRoute = this->mc_Routes[u32_ActiveNode];

         if (rc_ActRoute.c_VecRoutePoints.size() > 0)
         {
            const C_OscNode * const pc_Node = &this->mpc_SysDef->c_Nodes[this->mc_ActiveNodesIndexes[u32_ActiveNode]];

            // Stop specific routing of all nodes first. It needs the openSYDE routing if configured
            this->m_StopRoutingSpecific(u32_ActiveNode);

            // Get the necessary active node indexes of the last openSYDE nodes on the route
            if (this->m_IsRoutingSpecificNecessary(*pc_Node) == true)
            {
               bool q_Found;

               // Specific protocol necessary. Routing dispatcher necessary.
               // Use the last openSYDE node protocol instance for the IP to IP routing
               const uint32_t u32_Index = this->m_GetActiveIndex(
                  rc_ActRoute.c_VecRoutePoints[rc_ActRoute.c_VecRoutePoints.size() - 1].u32_NodeIndex, &q_Found);

               if (q_Found == true)
               {
                  c_ActiveOsyTargetNodes[u32_ActiveNode] = u32_Index;
               }
            }
            else
            {
               // No error. No specific protocol necessary. We need only openSYDE protocol.
               c_ActiveOsyTargetNodes[u32_ActiveNode] = u32_ActiveNode;
            }

            // Get the most far route hop
            if (rc_ActRoute.c_VecRoutePoints.size() > static_cast<uint32_t>(s32_LastRouteHop))
            {
               s32_LastRouteHop = static_cast<uint32_t>(rc_ActRoute.c_VecRoutePoints.size());
            }
         }
      }

      // Stop the routing for each 'hop distance' from behind
      for (s32_RouteHopCounter = s32_LastRouteHop; s32_RouteHopCounter >= 0; --s32_RouteHopCounter)
      {
         for (u32_ActiveNode = 0U; u32_ActiveNode < this->mc_ActiveNodesIndexes.size(); ++u32_ActiveNode)
         {
            if (c_ActiveOsyTargetNodes[u32_ActiveNode] >= 0)
            {
               const C_OscRoutingRoute & rc_ActRoute = this->mc_Routes[u32_ActiveNode];

               if (static_cast<uint32_t>(s32_RouteHopCounter) < rc_ActRoute.c_VecRoutePoints.size())
               {
                  if (this->m_StopRoutingOfRoutingPoint(u32_ActiveNode, c_ActiveOsyTargetNodes[u32_ActiveNode],
                                                        rc_ActRoute.c_VecRoutePoints[s32_RouteHopCounter],
                                                        (s32_RouteHopCounter == 0)) != C_NO_ERR)
                  {
                     // No further stopping of routing necessary for this node
                     c_ActiveOsyTargetNodes[u32_ActiveNode] = -1;
                  }
               }
            }
         }
      }
   }

   this->mc_ActiveNodesLastCanRouters.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stops the routing configuration for one specific point in the route to a node

   \param[in]   ou32_ActiveNode            active node index to stop routing for
   \param[in]   ou32_ActiveOsyTargetNode   active node index of last openSYDE node in the route
                                           can be the same as ou32_ActiveNode
   \param[in]   orc_Point                  current routing point to stop
   \param[in]   oq_FirstPoint              flag if point is the first point in the route

   \return
   C_NO_ERR    Routing for point deactivated
   C_NOACT     Routing for point deactivated and no further stopping necessary for this node
   C_COM       communication driver reported error
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_StopRoutingOfRoutingPoint(const uint32_t ou32_ActiveNode,
                                                            const uint32_t ou32_ActiveOsyTargetNode,
                                                            const C_OscRoutingRoutePoint & orc_Point,
                                                            const bool oq_FirstPoint)
{
   int32_t s32_Return = C_NO_ERR;

   // Using the same connection for configuring the router and the final openSYDE target
   C_OscProtocolDriverOsy * const pc_ProtocolOsyTarget =
      dynamic_cast<C_OscProtocolDriverOsy *>(this->mc_OsyProtocols[ou32_ActiveOsyTargetNode]);

   if (pc_ProtocolOsyTarget != NULL)
   {
      bool q_DoDisconnect = true;

      if (orc_Point.e_OutInterfaceType == C_OscSystemBus::eCAN)
      {
         const uint32_t u32_CurrentNode = this->m_GetActiveIndex(orc_Point.u32_NodeIndex);
         int32_t s32_Retval;

         // Clear all queues. In case of CAN tp the change causes that more than one queue receives
         // service responses
         this->ClearDispatcherQueue();
         // Because of using the same connection,
         // the protocol must be adapted to configure the router
         pc_ProtocolOsyTarget->SetNodeIdentifiers(this->mc_ClientId,
                                                  this->mc_ServerIds[u32_CurrentNode]);

         // We need an other security level
         s32_Retval = this->m_SetNodeSecurityAccess(ou32_ActiveOsyTargetNode, 5, NULL);

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = pc_ProtocolOsyTarget->OsyStopRouteDiagnosisCommunication();

            if (s32_Retval != C_NO_ERR)
            {
               osc_write_log_error("Stop Routing",
                                   "Error on stopping routing to node " + C_SclString::IntToStr(
                                      orc_Point.u32_NodeIndex) + " with error " +
                                   C_OscLoggingHandler::h_StwError(s32_Retval));
            }
         }
         else
         {
            osc_write_log_error("Stop Routing",
                                "Error on setting security access on node " + C_SclString::IntToStr(
                                   orc_Point.u32_NodeIndex) + " with error " +
                                C_OscLoggingHandler::h_StwError(s32_Retval));
         }

         if (oq_FirstPoint == true)
         {
            // In case of Ethernet to CAN routing check the number of protocols which use the same TCP connection
            // Disconnect only if no one is left
            const std::map<uint32_t,
                           uint32_t>::const_iterator c_ItRouterNode = this->mc_ActiveNodeIp2CanDispatcher.find(
               ou32_ActiveNode);

            if ((c_ItRouterNode != this->mc_ActiveNodeIp2CanDispatcher.end()) &&
                (c_ItRouterNode->second < this->mc_ActiveNodeIp2CanDispatcherNodeCount.size()))
            {
               // Special case: IP to CAN routing with an dispatcher of an other node
               if (this->mc_ActiveNodeIp2CanDispatcherNodeCount[c_ItRouterNode->second] > 0U)
               {
                  this->mc_ActiveNodeIp2CanDispatcherNodeCount[c_ItRouterNode->second] =
                     this->mc_ActiveNodeIp2CanDispatcherNodeCount[c_ItRouterNode->second] - 1U;
               }

               if (this->mc_ActiveNodeIp2CanDispatcherNodeCount[c_ItRouterNode->second] > 0U)
               {
                  q_DoDisconnect = false;
               }
            }

            if (q_DoDisconnect == true)
            {
               pc_ProtocolOsyTarget->Disconnect();
            }
         }

         // Clear all queues. In case of CAN tp the change causes that more than one queue receives
         // service responses
         this->ClearDispatcherQueue();
         // Configuring of routing finished, reset the protocol configuration
         pc_ProtocolOsyTarget->SetNodeIdentifiers(this->mc_ClientId,
                                                  this->mc_ServerIds[ou32_ActiveOsyTargetNode]);
      }

      // In case of Ethernet as out interface the ip to ip routing is active and will be closed
      // by closing the TCP connection with the disconnect
      if (orc_Point.e_InInterfaceType == C_OscSystemBus::eETHERNET)
      {
         // Ethernet routing to this point

         // Special case: IP to IP routing. The protocol instance of the target server is used to
         // communicate with the first router server

         // In case of Ethernet check the number of protocols which use the same TCP connection
         // Disconnect only if no one is left
         const std::map<uint32_t, uint32_t>::const_iterator c_Iter =
            this->mc_ActiveNodeIp2IpDispatcher.find(ou32_ActiveNode);

         if ((c_Iter != this->mc_ActiveNodeIp2IpDispatcher.end()) &&
             (c_Iter->second < this->mc_ActiveNodeIp2IpDispatcherNodeCount.size()))
         {
            if (this->mc_ActiveNodeIp2IpDispatcherNodeCount[c_Iter->second] > 0U)
            {
               this->mc_ActiveNodeIp2IpDispatcherNodeCount[c_Iter->second] =
                  this->mc_ActiveNodeIp2IpDispatcherNodeCount[c_Iter->second] - 1U;
            }

            if (this->mc_ActiveNodeIp2IpDispatcherNodeCount[c_Iter->second] > 0U)
            {
               q_DoDisconnect = false;
            }
         }

         if (q_DoDisconnect == true)
         {
            pc_ProtocolOsyTarget->Disconnect();
         }

         // No further stopping for this node necessary
         s32_Return = C_NOACT;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stops the specific routing configuration for one specific node

   Cleans up the legacy router dispatcher. Must be called be overloaded functions

   \param[in]     ou32_ActiveNode                      Active node index of vector mc_ActiveNodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverProtocol::m_StopRoutingSpecific(const uint32_t ou32_ActiveNode)
{
   // Close the tunneling and clean up the routing dispatcher
   if ((ou32_ActiveNode < this->mc_LegacyRouterDispatchers.size()) &&
       (this->mc_LegacyRouterDispatchers[ou32_ActiveNode] != NULL))
   {
      if (this->mc_Routes[ou32_ActiveNode].c_VecRoutePoints.size() > 0)
      {
         // For the exit we need an other security level
         const uint32_t u32_ActiveRouterNode = this->m_GetActiveIndex(this->mc_Routes[ou32_ActiveNode].
                                                                      c_VecRoutePoints[this->mc_Routes[ou32_ActiveNode].
                                                                                       c_VecRoutePoints.size() -
                                                                                       1].u32_NodeIndex);

         this->m_SetNodeSecurityAccess(u32_ActiveRouterNode, 5, NULL);
      }

      this->mc_LegacyRouterDispatchers[ou32_ActiveNode]->CAN_Exit();
      delete (this->mc_LegacyRouterDispatchers[ou32_ActiveNode]);
      this->mc_LegacyRouterDispatchers[ou32_ActiveNode] = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize Routes for each node

   Route configuration depends on child implementation of m_GetRoutingMode.

   \return
   C_NO_ERR      Operation success
   C_CONFIG      Invalid system definition
   C_RANGE       Minimum one target node does not exist
   C_COM         For minimum one target no routes were found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_InitRoutesAndActiveNodes(void)
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->mpc_SysDef != NULL)
   {
      //Count active nodes
      C_OscRoutingCalculation::E_Mode e_Mode;
      const bool q_RoutingUsed = this->m_GetRoutingMode(e_Mode);

      this->mu32_ActiveNodeCount = 0;
      this->mc_ActiveNodesIndexes.clear();
      this->mc_Routes.clear();

      //Init routes
      for (uint32_t u32_ItActiveFlag = 0; u32_ItActiveFlag < this->mc_ActiveNodesSystem.size(); ++u32_ItActiveFlag)
      {
         if (this->mc_ActiveNodesSystem[u32_ItActiveFlag] == 1U)
         {
            //Calculate all routes
            const C_OscRoutingCalculation c_RouteCalculation(this->mpc_SysDef->c_Nodes, this->mc_ActiveNodesSystem,
                                                             this->mu32_ActiveBusIndex,
                                                             u32_ItActiveFlag, e_Mode);
            s32_Retval = c_RouteCalculation.GetState();
            if (s32_Retval == C_NO_ERR)
            {
               // Get the best route for this node
               const C_OscRoutingRoute * const pc_Route = c_RouteCalculation.GetBestRoute();
               if (pc_Route != NULL)
               {
                  // If no routing shall be used, register only nodes which are connected to the PC bus directly
                  if ((q_RoutingUsed == true) ||
                      (pc_Route->c_VecRoutePoints.size() == 0))
                  {
                     this->mc_Routes.push_back(*pc_Route);
                     this->mc_ActiveNodesIndexes.push_back(u32_ItActiveFlag);
                     ++this->mu32_ActiveNodeCount;
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            else if (s32_Retval == C_NOACT)
            {
               //Reset error because node may still be active
               s32_Retval = C_NO_ERR;
            }
            else
            {
               // Routing error
               break;
            }
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize Server ID for each node

   Assemble a table of all server IDs seen from the bus interface we are connected to.
   Also create table of IP addresses.

   \return
   C_NO_ERR Operation success
   C_CONFIG Invalid initialization
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_InitServerIds(void)
{
   int32_t s32_Retval = C_NO_ERR;

   if ((this->mpc_SysDef != NULL) &&
       (this->mc_Routes.size() == this->mu32_ActiveNodeCount))
   {
      //Init routes
      this->mc_ServerIds.clear();
      this->mc_ServerIpAddresses.clear();
      this->mc_ServerIds.reserve(this->mu32_ActiveNodeCount);
      this->mc_ServerIpAddresses.reserve(this->mu32_ActiveNodeCount);
      for (uint32_t u32_ItActiveNode = 0;
           (u32_ItActiveNode < this->mc_ActiveNodesIndexes.size()) && (s32_Retval == C_NO_ERR);
           ++u32_ItActiveNode)
      {
         const C_OscSystemBus * pc_Bus;
         uint32_t u32_BusIndex = 0xFFFFFFFFU;
         C_OscRoutingRoute & rc_Route = this->mc_Routes[u32_ItActiveNode];
         //Get bus index
         if (rc_Route.c_VecRoutePoints.size() == 0)
         {
            //PC bus
            u32_BusIndex = this->mu32_ActiveBusIndex;
         }
         else
         {
            const C_OscRoutingRoutePoint & rc_LastHop =
               rc_Route.c_VecRoutePoints[rc_Route.c_VecRoutePoints.size() - 1U];

            if (rc_LastHop.u32_NodeIndex < this->mpc_SysDef->c_Nodes.size())
            {
               const C_OscNode * const pc_LastHopNode = &this->mpc_SysDef->c_Nodes[rc_LastHop.u32_NodeIndex];

               const C_OscNodeComInterfaceSettings * const pc_Interface =
                  pc_LastHopNode->c_Properties.GetComInterface(rc_LastHop.e_OutInterfaceType,
                                                               rc_LastHop.u8_OutInterfaceNumber);
               if (pc_Interface != NULL)
               {
                  if (pc_Interface->GetBusConnected() == true)
                  {
                     //Explicit bus
                     u32_BusIndex = pc_Interface->u32_BusIndex;
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }

         //Find connected interface of target node to found bus index
         if (u32_BusIndex < this->mpc_SysDef->c_Buses.size())
         {
            pc_Bus = &this->mpc_SysDef->c_Buses[u32_BusIndex];

            if (u32_ItActiveNode < this->mpc_SysDef->c_Nodes.size())
            {
               const C_OscNode * const pc_Node =
                  &this->mpc_SysDef->c_Nodes[this->mc_ActiveNodesIndexes[u32_ItActiveNode]];

               const std::vector<C_OscNodeComInterfaceSettings> & rc_ComInterfaces =
                  pc_Node->c_Properties.c_ComInterfaces;
               bool q_Found = false;
               C_OscProtocolDriverOsyNode c_NewServerId;
               C_OscNodeComInterfaceSettings::C_IpAddress c_IpAddress;

               for (uint32_t u32_ItComInterface = 0; u32_ItComInterface < rc_ComInterfaces.size();
                    ++u32_ItComInterface)
               {
                  const C_OscNodeComInterfaceSettings & rc_CurComInterface =
                     rc_ComInterfaces[u32_ItComInterface];
                  if (rc_CurComInterface.GetBusConnected() == true)
                  {
                     if (rc_CurComInterface.u32_BusIndex == u32_BusIndex)
                     {
                        q_Found = true;
                        //Extract relevant information
                        c_NewServerId.u8_NodeIdentifier = rc_CurComInterface.u8_NodeId;
                        c_NewServerId.u8_BusIdentifier = pc_Bus->u8_BusId;
                        c_IpAddress = rc_CurComInterface.c_Ip;

                        // The first interface with at least one activated feature is enough
                        // If no feature is active on this interface, keep searching for a 'better' interface.
                        // If no better interface is found, use the already found interface
                        // Handling of only one server id is possible
                        if (this->m_CheckInterfaceForFunctions(rc_CurComInterface) == true)
                        {
                           // Interface with at least one activated function found
                           break;
                        }
                     }
                  }
               }

               if (q_Found == true)
               {
                  this->mc_ServerIds.push_back(c_NewServerId);
                  this->mc_ServerIpAddresses.push_back(c_IpAddress);
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize for CAN connection

   \return
   C_NO_ERR      Operation success
   C_COM         CAN initialization failed
   C_CHECKSUM    Internal buffer overflow detected
   C_OVERFLOW    Error on setting dispatcher or node identifier in transport protocol
   C_RANGE       Count of server ids does not match to the count of nodes
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_InitForCan(void)
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->mc_ServerIds.size() == this->mu32_ActiveNodeCount)
   {
      C_OscProtocolDriverOsyTpCan * pc_TransportProtocol;

      if (this->mpc_CanDispatcher != NULL)
      {
         //Transport protocols
         this->mc_TransportProtocols.resize(static_cast<uint32_t>(this->mu32_ActiveNodeCount), NULL);
         this->mc_LegacyRouterDispatchers.resize(static_cast<uint32_t>(this->mu32_ActiveNodeCount), NULL);
         for (uint32_t u32_ItActiveNode = 0;
              (u32_ItActiveNode < this->mu32_ActiveNodeCount) && (s32_Retval == C_NO_ERR);
              ++u32_ItActiveNode)
         {
            pc_TransportProtocol = new C_OscProtocolDriverOsyTpCan();
            s32_Retval = pc_TransportProtocol->SetNodeIdentifiers(this->mc_ClientId,
                                                                  this->mc_ServerIds[u32_ItActiveNode]);
            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = pc_TransportProtocol->SetDispatcher(this->mpc_CanDispatcher);
               if (s32_Retval != C_NO_ERR)
               {
                  C_SclString c_Text = "Node \"";
                  c_Text += this->m_GetActiveNodeName(u32_ItActiveNode);
                  c_Text += "\" - SetDispatcher - error: ";
                  c_Text += C_OscLoggingHandler::h_StwError(s32_Retval);
                  c_Text += "\nC_CONFIG   could not register with dispatcher\n"
                            "C_NOACT    could not configure Rx filter\n";
                  osc_write_log_warning("Asynchronous communication", c_Text.c_str());
                  s32_Retval = C_OVERFLOW;
               }
            }
            else
            {
               C_SclString c_Text = "Node \"";
               c_Text += this->m_GetActiveNodeName(u32_ItActiveNode);
               c_Text += "\" - SetNodeIdentifiers - error: ";
               c_Text += C_OscLoggingHandler::h_StwError(s32_Retval);
               c_Text += "\nC_RANGE    client and/or server identifier out of range\n"
                         "C_NOACT    could not reconfigure Rx filters\n";
               osc_write_log_warning("Asynchronous communication", c_Text.c_str());
               s32_Retval = C_OVERFLOW;
            }
            this->mc_TransportProtocols[u32_ItActiveNode] = pc_TransportProtocol;
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Broadcast
            mpc_CanTransportProtocolBroadcast = new C_OscProtocolDriverOsyTpCan();
            s32_Retval = this->mpc_CanTransportProtocolBroadcast->SetNodeIdentifiersForBroadcasts(this->mc_ClientId);
            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = this->mpc_CanTransportProtocolBroadcast->SetDispatcher(this->mpc_CanDispatcher);
               if (s32_Retval != C_NO_ERR)
               {
                  C_SclString c_Text = "Broadcast - SetDispatcher - error: ";
                  c_Text += C_OscLoggingHandler::h_StwError(s32_Retval);
                  c_Text += "\nC_CONFIG   could not register with dispatcher\n"
                            "C_NOACT    could not configure Rx filter\n";
                  osc_write_log_warning("Asynchronous communication", c_Text.c_str());
                  s32_Retval = C_OVERFLOW;
               }
            }
            else
            {
               C_SclString c_Text = "Broadcast - SetNodeIdentifiers - error: ";
               c_Text += C_OscLoggingHandler::h_StwError(s32_Retval);
               c_Text += "\nC_RANGE    client and/or server identifier out of range\n"
                         "C_NOACT    could not reconfigure Rx filters\n";
               osc_write_log_warning("Asynchronous communication", c_Text.c_str());
               s32_Retval = C_OVERFLOW;
            }
         }
      }
      else
      {
         s32_Retval = C_COM;
      }
      //lint -e{593}  pc_TransportProtocol will be saved in mc_TransportProtocols and deleted by destructor
   }
   else
   {
      // Routing error
      osc_write_log_error("Comm driver initialization",
                          "Failed: CAN initialization error because of different node count to server IDs");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize for Ethernet connection

   \return
   C_NO_ERR      Operation success
   C_CONFIG      TCP initialization failed
   C_COM         IP dispatcher is NULL or UPD Socket initialization failed
   C_OVERFLOW    Error on setting dispatcher or node identifier in transport protocol
   C_RANGE       Count of server ids does not match to the count of nodes
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_InitForEthernet(void)
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->mc_ServerIds.size() == this->mu32_ActiveNodeCount)
   {
      C_OscProtocolDriverOsyTpIp * pc_TransportProtocol;
      std::vector<uint32_t> c_IpDispatcherHandles;

      //Dispatcher
      c_IpDispatcherHandles.resize(this->mu32_ActiveNodeCount, 0U);

      this->mc_ActiveNodeIp2IpDispatcher.clear();
      this->mc_ActiveNodeIp2IpDispatcherNodeCount.resize(this->mu32_ActiveNodeCount, 0U);

      this->mc_ActiveNodeIp2CanDispatcher.clear();
      this->mc_ActiveNodeIp2CanDispatcherNodeCount.resize(this->mu32_ActiveNodeCount, 0U);

      if (this->mpc_IpDispatcher != NULL)
      {
         uint32_t u32_ItActiveNode;
         std::map<uint32_t, uint32_t> c_ActiveNodeIp2IpRouters;

         // We can communicate only with 'direct' connected Ethernet devices. All other devices will be
         // reached by routing and shall use the IP of the first router node.
         // But for each routing must exist an own connection in case of IP to IP routing, but
         // not in case of IP to CAN routing.

         tgl_assert(this->mc_Routes.size() == this->mu32_ActiveNodeCount);

         // Initialize all Ethernet connections without routing
         for (u32_ItActiveNode = 0; u32_ItActiveNode < this->mu32_ActiveNodeCount; ++u32_ItActiveNode)
         {
            // These connections will be used by Ethernet to CAN routing too
            if (this->mc_Routes[u32_ItActiveNode].c_VecRoutePoints.size() == 0)
            {
               s32_Retval = this->m_InitTcp(mc_ServerIpAddresses[u32_ItActiveNode].au8_IpAddress,
                                            c_IpDispatcherHandles[u32_ItActiveNode]);
               if (s32_Retval != C_NO_ERR)
               {
                  break;
               }
            }
         }

         if (s32_Retval == C_NO_ERR)
         {
            // Initialize all Ethernet connections with IP to IP routing
            for (u32_ItActiveNode = 0; u32_ItActiveNode < this->mu32_ActiveNodeCount; ++u32_ItActiveNode)
            {
               if (this->mc_Routes[u32_ItActiveNode].c_VecRoutePoints.size() != 0)
               {
                  uint32_t u32_Ip2IpRouterActiveNodeClientSide;
                  uint32_t u32_Ip2IpRouterActiveNodeTargetSide;

                  s32_Retval = m_GetActiveIndexOfIp2IpRouter(u32_ItActiveNode, u32_Ip2IpRouterActiveNodeClientSide,
                                                             u32_Ip2IpRouterActiveNodeTargetSide);

                  if (s32_Retval == C_NO_ERR)
                  {
                     // Exist a dispatcher handle for this router server
                     const std::map<uint32_t,
                                    uint32_t>::const_iterator c_IterUniqueIp2IpRouters = c_ActiveNodeIp2IpRouters.find(
                        u32_Ip2IpRouterActiveNodeTargetSide);

                     if (c_IterUniqueIp2IpRouters == c_ActiveNodeIp2IpRouters.end())
                     {
                        // New dispatcher handle necessary
                        s32_Retval = this->m_InitTcp(
                           mc_ServerIpAddresses[u32_Ip2IpRouterActiveNodeClientSide].au8_IpAddress,
                           c_IpDispatcherHandles[u32_ItActiveNode]);

                        // Save the associated position of the dispatcher handle for this specific router
                        c_ActiveNodeIp2IpRouters.insert(
                           std::pair<uint32_t, uint32_t>(u32_Ip2IpRouterActiveNodeTargetSide, u32_ItActiveNode));
                     }
                     else
                     {
                        uint32_t u32_Handle;

                        // Reuse the dispatcher handle of the same router
                        tgl_assert(c_IterUniqueIp2IpRouters->second < c_IpDispatcherHandles.size());

                        u32_Handle = c_IpDispatcherHandles[c_IterUniqueIp2IpRouters->second];
                        c_IpDispatcherHandles[u32_ItActiveNode] = u32_Handle;
                     }

                     // Save the associated IP to IP router of the active node for later stopping the route
                     this->mc_ActiveNodeIp2IpDispatcher.insert(
                        std::pair<uint32_t, uint32_t>(u32_ItActiveNode, u32_Ip2IpRouterActiveNodeTargetSide));
                  }
                  else if (s32_Retval == C_NOACT)
                  {
                     uint32_t u32_Ip2CanRouterActiveNode;

                     // No error, no IP to IP routing, check for IP to CAN routing
                     s32_Retval = this->m_GetActiveIndexOfIp2CanRouter(u32_ItActiveNode, u32_Ip2CanRouterActiveNode);

                     if (s32_Retval == C_NO_ERR)
                     {
                        uint32_t u32_Handle;

                        // Reuse the dispatcher handle of the same router
                        tgl_assert(u32_Ip2CanRouterActiveNode < c_IpDispatcherHandles.size());

                        // IP to CAN routing has only one dispatcher and must be reused
                        u32_Handle = c_IpDispatcherHandles[u32_Ip2CanRouterActiveNode];
                        c_IpDispatcherHandles[u32_ItActiveNode] = u32_Handle;

                        // Save the associated IP to CAN router of the active node for later stopping the route
                        this->mc_ActiveNodeIp2CanDispatcher.insert(
                           std::pair<uint32_t, uint32_t>(u32_ItActiveNode, u32_Ip2CanRouterActiveNode));
                     }
                     else if (s32_Retval == C_NOACT)
                     {
                        // No error, but no initialization here too
                        s32_Retval = C_NO_ERR;
                     }
                     else
                     {
                        osc_write_log_error("Ethernet initialization",
                                            "Could not get index of IP to CAN router. Error Code: " +
                                            C_SclString::IntToStr(s32_Retval));
                     }
                  }
                  else
                  {
                     osc_write_log_error("Ethernet initialization",
                                         "Could not get index of IP to IP router. Error Code: " +
                                         C_SclString::IntToStr(s32_Retval));
                  }

                  if (s32_Retval != C_NO_ERR)
                  {
                     break;
                  }
               }
            }
         }

         if (s32_Retval == C_NO_ERR)
         {
            //Broadcast
            s32_Retval = mpc_IpDispatcher->InitUdp();

            if (s32_Retval == C_NO_ERR)
            {
               //Transport protocols
               this->mc_TransportProtocols.resize(static_cast<uint32_t>(this->mu32_ActiveNodeCount), NULL);
               this->mc_LegacyRouterDispatchers.resize(static_cast<uint32_t>(this->mu32_ActiveNodeCount), NULL);

               for (u32_ItActiveNode = 0;
                    (u32_ItActiveNode < this->mu32_ActiveNodeCount) && (s32_Retval == C_NO_ERR);
                    ++u32_ItActiveNode)
               {
                  pc_TransportProtocol = new C_OscProtocolDriverOsyTpIp();
                  s32_Retval = pc_TransportProtocol->SetNodeIdentifiers(this->mc_ClientId,
                                                                        this->mc_ServerIds[u32_ItActiveNode]);
                  if (s32_Retval == C_NO_ERR)
                  {
                     s32_Retval = pc_TransportProtocol->SetDispatcher(this->mpc_IpDispatcher,
                                                                      c_IpDispatcherHandles[u32_ItActiveNode]);
                     if (s32_Retval != C_NO_ERR)
                     {
                        osc_write_log_error("Ethernet initialization", "Could not set IP dispatcher. Error Code: " +
                                            C_SclString::IntToStr(s32_Retval));

                        //Invalid configuration = programming error
                        s32_Retval = C_OVERFLOW;
                     }
                  }
                  else
                  {
                     osc_write_log_error("Ethernet initialization", "Could not set node identifiers. Error Code: " +
                                         C_SclString::IntToStr(s32_Retval));

                     //Invalid configuration = programming error
                     s32_Retval = C_OVERFLOW;
                  }
                  this->mc_TransportProtocols[u32_ItActiveNode] = pc_TransportProtocol;
               }
               if (s32_Retval == C_NO_ERR)
               {
                  //Broadcast
                  mpc_IpTransportProtocolBroadcast = new C_OscProtocolDriverOsyTpIp();
                  s32_Retval = this->mpc_IpTransportProtocolBroadcast->SetDispatcher(this->mpc_IpDispatcher, 0U);
                  if (s32_Retval != C_NO_ERR)
                  {
                     //Invalid configuration = programming error
                     osc_write_log_error("Ethernet initialization", "Could not set broadcast dispatcher. Error Code: " +
                                         C_SclString::IntToStr(s32_Retval));

                     s32_Retval = C_OVERFLOW;
                  }
               }
            }
            else
            {
               osc_write_log_error("Ethernet initialization", "Could not initialize UDP. Error Code: " +
                                   C_SclString::IntToStr(s32_Retval));
               s32_Retval = C_COM;
            }
         }
      }
      else
      {
         osc_write_log_error("Ethernet initialization", "No dispatcher defined.");
         s32_Retval = C_COM;
      }
   }
   else
   {
      osc_write_log_error("Ethernet initialization", "Incorrect node count.");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the active index of the node which is necessary for IP to IP routing

   The IP to IP router oru32_ActiveIndexRouterClient is connected to the client directly.
   The IP to IP router target oru32_ActiveIndexRouterTarget is the last server connected to Ethernet bus.
   It can be the target itself or a further router server which routes in the next step over CAN.

   Using IP to IP routing, the IP and connection of the router server and the server ID of
   the target server will be used with an own connection.

   \param[in]     ou32_ActiveIndexTarget        The node which will be communicated to
   \param[out]    oru32_ActiveIndexRouterClient The node which is on the route for IP to IP routing on client side
   \param[out]    oru32_ActiveIndexRouterTarget The node which is the last on the route for IP to IP
                                                routing on target side or the target itself

   \return
   C_NO_ERR    Router found
   C_NOACT     No routing necessary
               No Ethernet to Ethernet routing on the first node in the route
   C_RANGE     ou32_ActiveIndexTarget invalid
   C_CONFIG    Route of ou32_ActiveIndexTarget is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_GetActiveIndexOfIp2IpRouter(const uint32_t ou32_ActiveIndexTarget,
                                                              uint32_t & oru32_ActiveIndexRouterClient,
                                                              uint32_t & oru32_ActiveIndexRouterTarget)
{
   int32_t s32_Return = C_RANGE;

   oru32_ActiveIndexRouterClient = 0U;
   oru32_ActiveIndexRouterTarget = 0U;

   if (ou32_ActiveIndexTarget < this->mc_Routes.size())
   {
      const C_OscRoutingRoute & rc_Route = this->mc_Routes[ou32_ActiveIndexTarget];
      if (rc_Route.c_VecRoutePoints.size() == 0)
      {
         // No routing necessary for the target, but Ethernet connection is necessary
         s32_Return = C_NOACT;
      }
      else
      {
         if ((rc_Route.c_VecRoutePoints[0].e_InInterfaceType == C_OscSystemBus::eETHERNET) &&
             (rc_Route.c_VecRoutePoints[0].e_OutInterfaceType == C_OscSystemBus::eETHERNET))
         {
            bool q_Found;

            oru32_ActiveIndexRouterClient =
               this->m_GetActiveIndex(rc_Route.c_VecRoutePoints[0].u32_NodeIndex, &q_Found);

            if (q_Found == true)
            {
               s32_Return = C_NO_ERR;
            }
            else
            {
               s32_Return = C_CONFIG;
            }

            if (s32_Return == C_NO_ERR)
            {
               int32_t s32_RoutePointCounter;
               const int32_t s32_LastRoutePoint = static_cast<int32_t>(rc_Route.c_VecRoutePoints.size()) - 1;

               // Search the other 'side'
               for (s32_RoutePointCounter = s32_LastRoutePoint; s32_RoutePointCounter >= 0; --s32_RoutePointCounter)
               {
                  if (rc_Route.c_VecRoutePoints[s32_RoutePointCounter].e_InInterfaceType == C_OscSystemBus::eETHERNET)
                  {
                     if ((s32_RoutePointCounter == s32_LastRoutePoint) &&
                         (rc_Route.c_VecRoutePoints[s32_RoutePointCounter].e_OutInterfaceType ==
                          C_OscSystemBus::eETHERNET))
                     {
                        // Special case: The concrete target is the last Ethernet target
                        oru32_ActiveIndexRouterTarget = ou32_ActiveIndexTarget;
                     }
                     else
                     {
                        // Ethernet router server found
                        oru32_ActiveIndexRouterTarget =
                           this->m_GetActiveIndex(rc_Route.c_VecRoutePoints[s32_RoutePointCounter].u32_NodeIndex,
                                                  &q_Found);

                        if (q_Found == true)
                        {
                           s32_Return = C_NO_ERR;
                        }
                        else
                        {
                           s32_Return = C_CONFIG;
                        }
                     }

                     break;
                  }
               }
            }
         }
         else
         {
            // Both interfaces must be Ethernet for IP to IP routing
            s32_Return = C_NOACT;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the active index of the node which is necessary for IP to CAN routing

   \param[in]     ou32_ActiveIndexTarget        The node which will be communicated to
   \param[out]    oru32_ActiveIndexRouter       The node which is on the first route point for IP to CAN routing on
                                                client side

   \return
   C_NO_ERR    Router found
   C_NOACT     No routing necessary
               No Ethernet to Ethernet routing on the first node in the route
   C_RANGE     ou32_ActiveIndexTarget invalid
   C_CONFIG    Route of ou32_ActiveIndexTarget is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_GetActiveIndexOfIp2CanRouter(const uint32_t ou32_ActiveIndexTarget,
                                                               uint32_t & oru32_ActiveIndexRouter)
{
   int32_t s32_Return = C_RANGE;

   oru32_ActiveIndexRouter = 0U;

   if (ou32_ActiveIndexTarget < this->mc_Routes.size())
   {
      const C_OscRoutingRoute & rc_Route = this->mc_Routes[ou32_ActiveIndexTarget];
      if (rc_Route.c_VecRoutePoints.size() == 0)
      {
         // No routing necessary for the target, but Ethernet connection is necessary
         s32_Return = C_NOACT;
      }
      else
      {
         if ((rc_Route.c_VecRoutePoints[0].e_InInterfaceType == C_OscSystemBus::eETHERNET) &&
             (rc_Route.c_VecRoutePoints[0].e_OutInterfaceType == C_OscSystemBus::eCAN))
         {
            bool q_Found;

            oru32_ActiveIndexRouter = this->m_GetActiveIndex(rc_Route.c_VecRoutePoints[0].u32_NodeIndex, &q_Found);

            if (q_Found == true)
            {
               s32_Return = C_NO_ERR;
            }
            else
            {
               s32_Return = C_CONFIG;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverProtocol::m_InitTcp(const uint8_t (&orau8_Ip)[4], uint32_t & oru32_Handle)
{
   int32_t s32_Retval = C_COM;

   if (this->mpc_IpDispatcher != NULL)
   {
      s32_Retval = mpc_IpDispatcher->InitTcp(orau8_Ip, oru32_Handle);

      if (s32_Retval != C_NO_ERR)
      {
         C_SclString c_Text;

         // Using the IP address of the router if IP to IP routing is used.
         // In case of no routing u32_Ip2IpRouterActiveNode equals u32_ItActiveNode
         c_Text.PrintFormatted("Could not set up TCP connection to %d.%d.%d.%d",
                               orau8_Ip[0],
                               orau8_Ip[1],
                               orau8_Ip[2],
                               orau8_Ip[3]);
         osc_write_log_error("Ethernet initialization", c_Text);
      }
   }

   return s32_Retval;
}
