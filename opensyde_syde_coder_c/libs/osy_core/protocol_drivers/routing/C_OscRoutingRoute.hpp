//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Holding the necessary information about a concrete route to a specific node from the PC.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCROUTINGROUTE_HPP
#define C_OSCROUTINGROUTE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"

#include "C_OscSystemBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscRoutingRoutePoint
{
public:
   C_OscRoutingRoutePoint();
   bool operator !=(const C_OscRoutingRoutePoint & orc_Cmp) const;
   bool operator ==(const C_OscRoutingRoutePoint & orc_Cmp) const;

   uint32_t u32_NodeIndex;

   // Input information for routing
   uint32_t u32_InBusIndex;
   uint8_t u8_InNodeId;
   uint8_t u8_InInterfaceNumber;
   C_OscSystemBus::E_Type e_InInterfaceType;
   // Output information for routing
   uint32_t u32_OutBusIndex; /// Equals 0xFFFFFFFFU if the point is target
   uint8_t u8_OutNodeId;
   uint8_t u8_OutInterfaceNumber;
   C_OscSystemBus::E_Type e_OutInterfaceType;
};

class C_OscRoutingRoute
{
public:
   explicit C_OscRoutingRoute(const uint32_t ou32_TargetNodeIndex);

   bool operator ==(const C_OscRoutingRoute & orc_Cmp) const;

   uint32_t u32_TargetNodeIndex;

   std::vector<C_OscRoutingRoutePoint> c_VecRoutePoints;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
