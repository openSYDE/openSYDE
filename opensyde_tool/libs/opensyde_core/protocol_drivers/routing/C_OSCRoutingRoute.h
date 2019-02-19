//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Holding the necessary information about a concrete route to a specific node from the PC.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.07.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVROROUTE_H
#define C_SYVROROUTE_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>

#include "stwtypes.h"

#include "C_OSCSystemBus.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCRoutingRoutePoint
{
public:
   C_OSCRoutingRoutePoint();
   bool operator !=(const C_OSCRoutingRoutePoint & orc_Cmp) const;
   bool operator ==(const C_OSCRoutingRoutePoint & orc_Cmp) const;

   stw_types::uint32 u32_NodeIndex;

   // Input information for routing
   stw_types::uint32 u32_InBusIndex;
   stw_types::uint8 u8_InNodeID;
   stw_types::uint8 u8_InInterfaceNumber;
   C_OSCSystemBus::E_Type e_InInterfaceType;
   // Output information for routing
   stw_types::uint32 u32_OutBusIndex;
   stw_types::uint8 u8_OutNodeID;
   stw_types::uint8 u8_OutInterfaceNumber;
   C_OSCSystemBus::E_Type e_OutInterfaceType;
};

class C_OSCRoutingRoute
{
public:
   explicit C_OSCRoutingRoute(const stw_types::uint32 ou32_TargetNodeIndex);

   bool operator ==(const C_OSCRoutingRoute & orc_Cmp) const;

   stw_types::uint32 u32_TargetNodeIndex;

   std::vector<C_OSCRoutingRoutePoint> c_VecRoutePoints;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
