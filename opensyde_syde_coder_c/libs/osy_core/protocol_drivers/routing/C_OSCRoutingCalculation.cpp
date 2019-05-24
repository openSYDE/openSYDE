//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Calculation of all potential routes to a specific node in a specific view. (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <set>

#include "stwerrors.h"
#include "CSCLString.h"

#include "C_OSCRoutingCalculation.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace std;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Default constructor

   Use local only! (uses reference on nodes)

   \param[in]     orc_AllNodes         All nodes in the system definition
   \param[in]     orc_ActiveNodes      Flags for all available nodes in the system
   \param[in]     ou32_StartBusIndex   Start bus for the routing
   \param[in]     ou32_TargetNodeIndex Target node at the end of the routing
   \param[in]     oe_Mode              Decision for update or diagnostic routing
*/
//-----------------------------------------------------------------------------
C_OSCRoutingCalculation::C_OSCRoutingCalculation(const vector<C_OSCNode> & orc_AllNodes,
                                                 const vector<uint8> & orc_ActiveNodes, const uint32 ou32_StartBusIndex,
                                                 const uint32 ou32_TargetNodeIndex, const E_Mode oe_Mode) :
   mu32_StartBusIndex(ou32_StartBusIndex),
   mu32_TargetNodeIndex(ou32_TargetNodeIndex),
   me_Mode(oe_Mode),
   ms32_ResultState(C_COM),
   mq_PcBus(true),
   mrc_AllNodes(orc_AllNodes),
   mrc_ActiveNodes(orc_ActiveNodes)
{
   this->m_SearchRoute();
}

//-----------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//-----------------------------------------------------------------------------
C_OSCRoutingCalculation::~C_OSCRoutingCalculation()
{
   this->mc_AllOpenRoutes.clear();
}

//-----------------------------------------------------------------------------
/*! \brief   Returns all found routes

   \return
   Vector with all found routes
*/
//-----------------------------------------------------------------------------
const vector<C_OSCRoutingRoute> * C_OSCRoutingCalculation::GetRoutes(void) const
{
   return &this->mc_RoutesToTarget;
}

//-----------------------------------------------------------------------------
/*! \brief   Returns the best route

   \return
   Valid Pointer:    Best route to target
   NULL:             No route found
*/
//-----------------------------------------------------------------------------
const C_OSCRoutingRoute * C_OSCRoutingCalculation::GetBestRoute(void) const
{
   const C_OSCRoutingRoute * pc_Result = NULL;

   if (this->mc_RoutesToTarget.size() > 0)
   {
      uint32 u32_CountHops = 0xFFFFFFFFU;
      uint32 u32_Counter;

      // Search the smallest count of hops
      for (u32_Counter = 0U; u32_Counter < this->mc_RoutesToTarget.size(); ++u32_Counter)
      {
         if (this->mc_RoutesToTarget[u32_Counter].c_VecRoutePoints.size() < u32_CountHops)
         {
            u32_CountHops = this->mc_RoutesToTarget[u32_Counter].c_VecRoutePoints.size();

            if (pc_Result == NULL)
            {
               // Save as first result
               pc_Result = &this->mc_RoutesToTarget[u32_Counter];
            }
         }
      }

      // Search a route with smallest count of hops and an Ethernet connection
      for (u32_Counter = 0U; u32_Counter < this->mc_RoutesToTarget.size(); ++u32_Counter)
      {
         if ((this->mc_RoutesToTarget[u32_Counter].c_VecRoutePoints.size() == u32_CountHops) &&
             (this->mc_RoutesToTarget[u32_Counter].c_VecRoutePoints.size() > 0U))
         {
            uint32 u32_PointCounter;

            for (u32_PointCounter = 0U;
                 u32_PointCounter < this->mc_RoutesToTarget[u32_Counter].c_VecRoutePoints.size();
                 ++u32_PointCounter)
            {
               if (this->mc_RoutesToTarget[u32_Counter].c_VecRoutePoints[u32_PointCounter].e_OutInterfaceType ==
                   C_OSCSystemBus::eETHERNET)
               {
                  // Route with Ethernet
                  pc_Result = &this->mc_RoutesToTarget[u32_Counter];
                  break;
               }
            }
         }
      }
   }

   return pc_Result;
}

//-----------------------------------------------------------------------------
/*! \brief   Returns the result state of the calculation

   \return
   C_NO_ERR    Routes were found
   C_RANGE     Target node does not exist
   C_COM       No routes were found
   C_NOACT     Target function (update or diagnostic) deactivated on all connected bus. No routing necessary.
*/
//-----------------------------------------------------------------------------
sint32 C_OSCRoutingCalculation::GetState(void) const
{
   return this->ms32_ResultState;
}

//-----------------------------------------------------------------------------
bool C_OSCRoutingCalculation::CheckItfNumberForRouting(const stw_types::uint32 ou32_TargetNodeIndex,
                                                       const C_OSCNodeComInterfaceSettings & orc_ComItfSettings) const
{
   // Check if the interace can be used for routing
   bool q_Return = false;

   if (orc_ComItfSettings.q_IsBusConnected == true)
   {
      if (ou32_TargetNodeIndex == this->mu32_TargetNodeIndex)
      {
         // Node is target and must support the function on the current interface

         if ((this->me_Mode == C_OSCRoutingCalculation::eDIAGNOSTIC) &&
             (orc_ComItfSettings.q_IsDiagnosisEnabled == true))
         {
            // Minimum one com interface is relevant
            q_Return = true;
         }
         else if ((this->me_Mode == C_OSCRoutingCalculation::eUPDATE) &&
                  (orc_ComItfSettings.q_IsUpdateEnabled == true))
         {
            // Minimum one com interface is relevant
            q_Return = true;
         }
         else
         {
            // Nothing to do
         }
      }
      else
      {
         // Node is for routing

         // TODO BAY: Adapt check if the new flag for diagnostic and update routing was added
         if ((this->me_Mode == C_OSCRoutingCalculation::eDIAGNOSTIC) &&
             (orc_ComItfSettings.q_IsRoutingEnabled == true))
         {
            q_Return = true;
         }
         else if ((this->me_Mode == C_OSCRoutingCalculation::eUPDATE) &&
                  (orc_ComItfSettings.q_IsRoutingEnabled == true))
         {
            q_Return = true;
         }
         else
         {
            // Nothing to do
         }
      }
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
void C_OSCRoutingCalculation::m_SearchRoute(void)
{
   // maximum number of buses
   this->mc_CheckedBusses.reserve(16);

   // reset the search
   this->mq_PcBus = true;
   this->mc_AllRoutePoints.clear();
   this->mc_RoutesToTarget.clear();
   this->mc_AllOpenRoutes.clear();

   if (this->mu32_TargetNodeIndex < this->mrc_AllNodes.size())
   {
      // Is the node configured for update or diagnostic on its connected buses and is connected to a bus
      const sint32 s32_Result = this->m_CheckTargetNodeConfig();
      if (s32_Result == C_NO_ERR)
      {
         // Start searching
         this->m_SearchRoutePointsOnBus(this->mu32_StartBusIndex);
         this->m_CalculateRoutes(this->mu32_StartBusIndex);

         if (this->mc_RoutesToTarget.size() > 0)
         {
            // Minimum one route was found. Success
            this->ms32_ResultState = C_NO_ERR;
         }
         else
         {
            osc_write_log_error("Routing calculation", "No route found (target node index: " +
                                C_SCLString::IntToStr(this->mu32_TargetNodeIndex) + ")");
            // No route available
            this->ms32_ResultState = C_COM;
         }
      }
      else
      {
         this->ms32_ResultState = s32_Result;
      }
   }
   else
   {
      osc_write_log_error("Routing calculation", "Invalid node index");
      // Target is not valid
      this->ms32_ResultState = C_RANGE;
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Checks the target node configuration for a routable setting

   \return
   C_NO_ERR    Node is configured for correct routing
   C_NOACT     Target node must not be routed, because the relevant function is deactivated on all connected buses
   C_COM       No bus is connected to minimum one activated relevant function
*/
//-----------------------------------------------------------------------------
sint32 C_OSCRoutingCalculation::m_CheckTargetNodeConfig(void) const
{
   // Check if the target node must be routable
   sint32 s32_Return;
   bool q_UsableBusFound = false;
   bool q_MinimumOneFunctionActive = false;
   const C_OSCNode * const pc_Node = &this->mrc_AllNodes[this->mu32_TargetNodeIndex];
   uint32 u32_Counter;

   for (u32_Counter = 0U; u32_Counter < pc_Node->c_Properties.c_ComInterfaces.size(); ++u32_Counter)
   {
      // Bus is connected and for the routing and check relevant
      if ((this->me_Mode == C_OSCRoutingCalculation::eDIAGNOSTIC) &&
          (pc_Node->c_Properties.c_ComInterfaces[u32_Counter].q_IsDiagnosisEnabled == true))
      {
         q_MinimumOneFunctionActive = true;

         if (pc_Node->c_Properties.c_ComInterfaces[u32_Counter].q_IsBusConnected == true)
         {
            // Minimum one com interface is relevant
            q_UsableBusFound = true;
         }
      }
      else if ((this->me_Mode == C_OSCRoutingCalculation::eUPDATE) &&
               (pc_Node->c_Properties.c_ComInterfaces[u32_Counter].q_IsUpdateEnabled == true))
      {
         q_MinimumOneFunctionActive = true;

         if (pc_Node->c_Properties.c_ComInterfaces[u32_Counter].q_IsBusConnected == true)
         {
            // Minimum one com interface is relevant
            q_UsableBusFound = true;
         }
      }
      else
      {
         // Nothing to do
      }

      if (q_UsableBusFound == true)
      {
         break;
      }
   }

   if (q_UsableBusFound == true)
   {
      // Function (diagnosis or update) is active and minimum one bus is connected to the matching interface
      s32_Return = C_NO_ERR;
   }
   else if (q_MinimumOneFunctionActive == true)
   {
      // Minimum one function (diagnosis or update) is active but no bus is connected to a matching interface
      // No routing possible
      osc_write_log_error("Routing calculation", "No connected bus found (target node index: " +
                          C_SCLString::IntToStr(this->mu32_TargetNodeIndex) + ")");
      s32_Return = C_COM;
   }
   else
   {
      // Target node must not be routed, because the relevant function is deactivated on all busses
      osc_write_log_error("Routing calculation", "No usable bus found (target node index: " +
                          C_SCLString::IntToStr(this->mu32_TargetNodeIndex) + ")");
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------

void C_OSCRoutingCalculation::m_SearchRoutePointsOnBus(const uint32 ou32_BusIndex)
{
   uint32 u32_NodeCounter;

   set<uint32> c_SetBussesToSearch;
   set<uint32>::const_iterator c_ItBusToSearch;

   // search all nodes connected to the bus
   for (u32_NodeCounter = 0U; u32_NodeCounter < this->mrc_AllNodes.size(); ++u32_NodeCounter)
   {
      if (this->mrc_ActiveNodes[u32_NodeCounter] == 1U)
      {
         uint32 u32_InItfCounter;
         const C_OSCNode * const pc_ActNode = &this->mrc_AllNodes[u32_NodeCounter];

         for (u32_InItfCounter = 0U;
              u32_InItfCounter < pc_ActNode->c_Properties.c_ComInterfaces.size();
              ++u32_InItfCounter)
         {
            if ((pc_ActNode->c_Properties.c_ComInterfaces[u32_InItfCounter].u32_BusIndex == ou32_BusIndex) &&
                (this->CheckItfNumberForRouting(u32_NodeCounter,
                                                pc_ActNode->c_Properties.c_ComInterfaces[u32_InItfCounter]) == true))
            {
               C_OSCRoutingRoutePoint c_PointIn;
               vector<uint32> c_VecBussesToSearchOfNode;
               uint32 u32_NodeBusses;

               // Configure the input and common parameter of the route point
               c_PointIn.u32_NodeIndex = u32_NodeCounter;
               c_PointIn.u32_InBusIndex = ou32_BusIndex;
               c_PointIn.u8_InNodeID = pc_ActNode->c_Properties.c_ComInterfaces[u32_InItfCounter].u8_NodeID;
               c_PointIn.e_InInterfaceType =
                  pc_ActNode->c_Properties.c_ComInterfaces[u32_InItfCounter].e_InterfaceType;
               c_PointIn.u8_InInterfaceNumber =
                  pc_ActNode->c_Properties.c_ComInterfaces[u32_InItfCounter].u8_InterfaceNumber;

               c_VecBussesToSearchOfNode = this->m_GetAllRoutePointsOfNodeOnOneInput(c_PointIn, u32_InItfCounter);

               // add all new buses of this node for the next search
               for (u32_NodeBusses = 0U; u32_NodeBusses < c_VecBussesToSearchOfNode.size(); ++u32_NodeBusses)
               {
                  c_SetBussesToSearch.insert(c_VecBussesToSearchOfNode[u32_NodeBusses]);
               }
            }
         }
      }
   }

   this->mc_CheckedBusses.push_back(ou32_BusIndex);

   // search the other buses recursively
   for (c_ItBusToSearch = c_SetBussesToSearch.begin(); c_ItBusToSearch != c_SetBussesToSearch.end(); ++c_ItBusToSearch)
   {
      this->m_SearchRoutePointsOnBus(*c_ItBusToSearch);
   }
}

//-----------------------------------------------------------------------------
vector<uint32> C_OSCRoutingCalculation::m_GetAllRoutePointsOfNodeOnOneInput(const C_OSCRoutingRoutePoint & orc_InPoint,
                                                                            const uint32 ou32_InItfNumber)
{
   uint32 u32_OutItfCounter;
   const C_OSCNode * const pc_ActNode = &this->mrc_AllNodes[orc_InPoint.u32_NodeIndex];

   vector<uint32> c_VecBussesToSearch;
   C_OSCRoutingRoutePoint c_Point = orc_InPoint;

   if (c_Point.u32_NodeIndex == this->mu32_TargetNodeIndex)
   {
      // Special case: Target found. Register it as route point
      // This route point shall not be used as output route point
      c_Point.u32_OutBusIndex = 0xFFFFFFFFU;

      // save the route point
      this->m_AddOneRoutePoint(c_Point);
   }
   else
   {
      // Check all possible output variants for the node. All are possible routes.
      // Except output channels to previous buses
      for (u32_OutItfCounter = 0U;
           u32_OutItfCounter < pc_ActNode->c_Properties.c_ComInterfaces.size();
           ++u32_OutItfCounter)
      {
         if ((u32_OutItfCounter != ou32_InItfNumber) &&
             (this->CheckItfNumberForRouting(orc_InPoint.u32_NodeIndex,
                                             pc_ActNode->c_Properties.c_ComInterfaces[u32_OutItfCounter]) == true) &&
             (orc_InPoint.u32_InBusIndex != pc_ActNode->c_Properties.c_ComInterfaces[u32_OutItfCounter].u32_BusIndex))
         {
            // not the same interface and is connected too and is not connected to same bus as the in point
            uint32 u32_PreviousBusCounter;
            bool q_NewBus = true;

            // check if it is a new bus
            for (u32_PreviousBusCounter = 0U;
                 u32_PreviousBusCounter < this->mc_CheckedBusses.size();
                 ++u32_PreviousBusCounter)
            {
               if (pc_ActNode->c_Properties.c_ComInterfaces[u32_OutItfCounter].u32_BusIndex ==
                   this->mc_CheckedBusses[u32_PreviousBusCounter])
               {
                  // this bus was already used
                  q_NewBus = false;
               }
            }

            if (q_NewBus == true)
            {
               c_Point.e_OutInterfaceType =
                  pc_ActNode->c_Properties.c_ComInterfaces[u32_OutItfCounter].e_InterfaceType;
               c_Point.u8_OutInterfaceNumber =
                  pc_ActNode->c_Properties.c_ComInterfaces[u32_OutItfCounter].u8_InterfaceNumber;
               c_Point.u32_OutBusIndex =
                  pc_ActNode->c_Properties.c_ComInterfaces[u32_OutItfCounter].u32_BusIndex;
               c_Point.u8_OutNodeID = pc_ActNode->c_Properties.c_ComInterfaces[u32_OutItfCounter].u8_NodeID;

               // save the route point
               this->m_AddOneRoutePoint(c_Point);

               // this bus must be searched for the next routing points
               c_VecBussesToSearch.push_back(pc_ActNode->c_Properties.c_ComInterfaces[u32_OutItfCounter].u32_BusIndex);
            }
         }
      }
   }

   return c_VecBussesToSearch;
}

//-----------------------------------------------------------------------------

void C_OSCRoutingCalculation::m_AddOneRoutePoint(const C_OSCRoutingRoutePoint & orc_Point)
{
   // add the point only if it is not already in the vector
   std::vector<C_OSCRoutingRoutePoint>::const_iterator c_ItPoint;
   bool q_Found = false;

   for (c_ItPoint = this->mc_AllRoutePoints.begin(); c_ItPoint != this->mc_AllRoutePoints.end(); ++c_ItPoint)
   {
      if (orc_Point == (*c_ItPoint))
      {
         q_Found = true;
      }
   }

   if (q_Found == false)
   {
      this->mc_AllRoutePoints.push_back(orc_Point);
   }
}

//-----------------------------------------------------------------------------

void C_OSCRoutingCalculation::m_CalculateRoutes(const uint32 ou32_BusIndex)
{
   uint32 u32_PointCounter;

   set<uint32> c_SetBussesToSearch;
   set<uint32>::const_iterator c_ItBusToSearch;

   // search all points with this bus as input
   for (u32_PointCounter = 0U; u32_PointCounter < this->mc_AllRoutePoints.size(); ++u32_PointCounter)
   {
      if (this->mc_AllRoutePoints[u32_PointCounter].u32_InBusIndex == ou32_BusIndex)
      {
         if (this->mq_PcBus == true)
         {
            // The first bus, add all found route points as new routes
            C_OSCRoutingRoute c_Route(this->mu32_TargetNodeIndex);

            if (this->mc_AllRoutePoints[u32_PointCounter].u32_NodeIndex == this->mu32_TargetNodeIndex)
            {
               // Special case: The target is on the bus connected to the PC already
               // No routing points necessary
               this->m_AddOneRouteToTarget(c_Route);
            }
            else
            {
               // Start of a routing route
               c_Route.c_VecRoutePoints.push_back(this->mc_AllRoutePoints[u32_PointCounter]);
               this->mc_AllOpenRoutes.push_back(c_Route);
               // Check the next bus of this routing point
               c_SetBussesToSearch.insert(this->mc_AllRoutePoints[u32_PointCounter].u32_OutBusIndex);
            }
         }
         else
         {
            // Searching the matching routing points on the previous routes
            const list<C_OSCRoutingRoute> c_AllOldOpenRoutes = this->mc_AllOpenRoutes;
            list<C_OSCRoutingRoute>::const_iterator c_ItOldRoute;

            // Further searching
            // The input bus of the actual route point must be equal to the output bus of the last routing point
            // of the route
            // One node shall be used in a route maximum one time
            for (c_ItOldRoute = c_AllOldOpenRoutes.begin();
                 c_ItOldRoute != c_AllOldOpenRoutes.end();
                 ++c_ItOldRoute)
            {
               if ((*c_ItOldRoute).c_VecRoutePoints.size() > 0)
               {
                  const vector<C_OSCRoutingRoutePoint> * const pc_RoutePoints = &((*c_ItOldRoute).c_VecRoutePoints);
                  const uint32 u32_RoutePointIndex = static_cast<uint32>(pc_RoutePoints->size() - 1U);
                  const C_OSCRoutingRoutePoint * const pc_LastPointOfRoute = &((*pc_RoutePoints)[u32_RoutePointIndex]);

                  if (this->mc_AllRoutePoints[u32_PointCounter].u32_InBusIndex ==
                      pc_LastPointOfRoute->u32_OutBusIndex)
                  {
                     if (this->mc_AllRoutePoints[u32_PointCounter].u32_NodeIndex == this->mu32_TargetNodeIndex)
                     {
                        // Special case: The target is on the current bus
                        // A usable final route.
                        // No further analysis of this variant, do not add it the open routes again.
                        this->m_AddOneRouteToTarget(*c_ItOldRoute);
                     }
                     else
                     {
                        uint32 u32_RoutePointCounter;
                        bool q_AlreadyUsed = false;

                        // Was this node already used for this route?
                        for (u32_RoutePointCounter = 0U;
                             u32_RoutePointCounter < pc_RoutePoints->size();
                             ++u32_RoutePointCounter)
                        {
                           if ((*pc_RoutePoints)[u32_RoutePointCounter].u32_NodeIndex ==
                               this->mc_AllRoutePoints[u32_PointCounter].u32_NodeIndex)
                           {
                              q_AlreadyUsed = true;
                              break;
                           }
                        }

                        if (q_AlreadyUsed == false)
                        {
                           // Make a copy of the old route and add the new route point
                           C_OSCRoutingRoute c_NewRoute = *c_ItOldRoute;
                           c_NewRoute.c_VecRoutePoints.push_back(this->mc_AllRoutePoints[u32_PointCounter]);
                           this->m_AddOneOpenRoute(c_NewRoute);

                           // Check the next bus of this routing point
                           c_SetBussesToSearch.insert(this->mc_AllRoutePoints[u32_PointCounter].u32_OutBusIndex);
                        }
                     }
                  }
               }
            }
         }
      }
   }

   this->mq_PcBus = false;

   // search the other buses recursively
   for (c_ItBusToSearch = c_SetBussesToSearch.begin(); c_ItBusToSearch != c_SetBussesToSearch.end(); ++c_ItBusToSearch)
   {
      if ((*c_ItBusToSearch) != 0xFFFFFFFFU)
      {
         this->m_CalculateRoutes(*c_ItBusToSearch);
      }
   }
}

//-----------------------------------------------------------------------------

void C_OSCRoutingCalculation::m_AddOneOpenRoute(const C_OSCRoutingRoute & orc_Route)
{
   // add the route only if it is not already in the list
   std::list<C_OSCRoutingRoute>::const_iterator c_ItRoute;
   bool q_Found = false;

   for (c_ItRoute = this->mc_AllOpenRoutes.begin(); c_ItRoute != this->mc_AllOpenRoutes.end(); ++c_ItRoute)
   {
      if (orc_Route == (*c_ItRoute))
      {
         q_Found = true;
      }
   }

   if (q_Found == false)
   {
      this->mc_AllOpenRoutes.push_back(orc_Route);
   }
}

//-----------------------------------------------------------------------------
void C_OSCRoutingCalculation::m_AddOneRouteToTarget(const C_OSCRoutingRoute & orc_Route)
{
   // add the route only if it is not already in the vector
   std::vector<C_OSCRoutingRoute>::const_iterator c_ItRoute;
   bool q_Found = false;

   for (c_ItRoute = this->mc_RoutesToTarget.begin(); c_ItRoute != this->mc_RoutesToTarget.end(); ++c_ItRoute)
   {
      if (orc_Route == (*c_ItRoute))
      {
         q_Found = true;
      }
   }

   if (q_Found == false)
   {
      this->mc_RoutesToTarget.push_back(orc_Route);
   }
}
