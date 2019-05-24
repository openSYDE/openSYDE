//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Calculation of all potential routes to a specific node in a specific view. (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCROUTINGCALCULATION_H
#define C_OSCROUTINGCALCULATION_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include <list>

#include "stwtypes.h"

#include "C_OSCRoutingRoute.h"
#include "C_OSCNode.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCRoutingCalculation
{
public:
   enum E_Mode ///< mode of routing
   {
      eUPDATE,    ///< routing for update communication
      eDIAGNOSTIC ///< routing for diagnostic communication
   };

   C_OSCRoutingCalculation(const std::vector<C_OSCNode> & orc_AllNodes,
                           const std::vector<stw_types::uint8> & orc_ActiveNodes,
                           const stw_types::uint32 ou32_StartBusIndex, const stw_types::uint32 ou32_TargetNodeIndex,
                           const E_Mode oe_Mode);
   ~C_OSCRoutingCalculation();

   const std::vector<C_OSCRoutingRoute> * GetRoutes(void) const;
   const C_OSCRoutingRoute * GetBestRoute(void) const;
   stw_types::sint32 GetState(void) const;
   bool CheckItfNumberForRouting(const stw_types::uint32 ou32_TargetNodeIndex,
                                 const C_OSCNodeComInterfaceSettings & orc_ComItfSettings) const;

private:
   //Avoid call
   C_OSCRoutingCalculation(const C_OSCRoutingCalculation &);
   C_OSCRoutingCalculation & operator =(const C_OSCRoutingCalculation &);

   void m_SearchRoute(void);
   stw_types::sint32 m_CheckTargetNodeConfig(void) const;
   void m_SearchRoutePointsOnBus(const stw_types::uint32 ou32_BusIndex);
   std::vector<stw_types::uint32> m_GetAllRoutePointsOfNodeOnOneInput(const C_OSCRoutingRoutePoint & orc_InPoint,
                                                                      const stw_types::uint32 ou32_InItfNumber);
   void m_AddOneRoutePoint(const C_OSCRoutingRoutePoint & orc_Point);

   void m_CalculateRoutes(const stw_types::uint32 ou32_BusIndex);
   void m_AddOneOpenRoute(const C_OSCRoutingRoute & orc_Route);
   void m_AddOneRouteToTarget(const C_OSCRoutingRoute & orc_Route);

   stw_types::uint32 mu32_StartBusIndex;
   stw_types::uint32 mu32_TargetNodeIndex;
   E_Mode me_Mode;

   std::vector<C_OSCRoutingRoutePoint> mc_AllRoutePoints;
   std::vector<C_OSCRoutingRoute> mc_RoutesToTarget;
   std::list<C_OSCRoutingRoute> mc_AllOpenRoutes;
   stw_types::sint32 ms32_ResultState;

   std::vector<stw_types::uint32> mc_CheckedBusses;
   bool mq_PcBus;

   //lint -e{1725} reference used for performance reasons
   const std::vector<C_OSCNode> & mrc_AllNodes;
   //lint -e{1725} reference used for performance reasons
   const std::vector<stw_types::uint8> & mrc_ActiveNodes;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
