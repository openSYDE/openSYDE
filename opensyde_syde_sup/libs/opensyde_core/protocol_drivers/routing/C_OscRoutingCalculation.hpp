//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Calculation of all potential routes to a specific node in a specific view. (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCROUTINGCALCULATION_H
#define C_OSCROUTINGCALCULATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <list>

#include "stwtypes.hpp"

#include "C_OscRoutingRoute.hpp"
#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscRoutingCalculation
{
public:
   enum E_Mode ///< mode of routing
   {
      eUPDATE,       ///< routing for update communication
      eDIAGNOSTIC,   ///< routing for diagnostic communication
      eROUTING_CHECK ///< routing calculation only for validation of an existing route independent of the use case
   };

   C_OscRoutingCalculation(const std::vector<C_OscNode> & orc_AllNodes, const std::vector<uint8_t> & orc_ActiveNodes,
                           const uint32_t ou32_StartBusIndex, const uint32_t ou32_TargetNodeIndex,
                           const E_Mode oe_Mode);
   ~C_OscRoutingCalculation();

   const std::vector<C_OscRoutingRoute> * GetRoutes(void) const;
   const C_OscRoutingRoute * GetBestRoute(void) const;
   int32_t GetState(void) const;
   bool CheckItfNumberForRouting(const uint32_t ou32_TargetNodeIndex,
                                 const C_OscNodeComInterfaceSettings & orc_ComItfSettings) const;

private:
   //Avoid call
   C_OscRoutingCalculation(const C_OscRoutingCalculation &);
   C_OscRoutingCalculation & operator =(const C_OscRoutingCalculation &);

   void m_SearchRoute(void);
   int32_t m_CheckTargetNodeConfig(void) const;
   void m_SearchRoutePointsOnBus(const uint32_t ou32_BusIndex);
   std::vector<uint32_t> m_GetAllRoutePointsOfNodeOnOneInput(const C_OscRoutingRoutePoint & orc_InPoint,
                                                             const uint32_t ou32_InItfNumber);
   void m_AddOneRoutePoint(const C_OscRoutingRoutePoint & orc_Point);

   void m_CalculateRoutes(const uint32_t ou32_BusIndex);
   void m_AddOneOpenRoute(const C_OscRoutingRoute & orc_Route);
   void m_AddOneRouteToTarget(const C_OscRoutingRoute & orc_Route);

   int32_t m_CheckRoutesForLimitations(void);

   uint32_t mu32_StartBusIndex;
   uint32_t mu32_TargetNodeIndex;
   E_Mode me_Mode;

   std::vector<C_OscRoutingRoutePoint> mc_AllRoutePoints;
   std::vector<C_OscRoutingRoute> mc_RoutesToTarget;
   std::list<C_OscRoutingRoute> mc_AllOpenRoutes;
   int32_t ms32_ResultState;

   std::vector<uint32_t> mc_CheckedBuses;
   bool mq_PcBus;

   //lint -e{1725} reference used for performance reasons
   const std::vector<C_OscNode> & mrc_AllNodes;
   //lint -e{1725} reference used for performance reasons
   const std::vector<uint8_t> & mrc_ActiveNodes;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
