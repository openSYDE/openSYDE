//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper class for Calculation of all potential routes to a specific node in a specific view.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.07.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVROROUTECALCULATION_H
#define C_SYVROROUTECALCULATION_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include <list>

#include "stwtypes.h"

#include "C_OSCRoutingCalculation.h"
#include "C_OSCRoutingRoute.h"
#include "C_OSCNode.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvRoRouteCalculation
{
public:
   C_SyvRoRouteCalculation(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_TargetNodeIndex,
                           const stw_opensyde_core::C_OSCRoutingCalculation::E_Mode oe_Mode);
   ~C_SyvRoRouteCalculation();

   const std::vector<stw_opensyde_core::C_OSCRoutingRoute> * GetRoutes(void) const;
   const stw_opensyde_core::C_OSCRoutingRoute * GetBestRoute(void) const;
   bool CheckItfNumberForRouting(const stw_types::uint32 ou32_TargetNodeIndex,
                                 const stw_opensyde_core::C_OSCNodeComInterfaceSettings & orc_ComItfSettings) const;
   stw_types::sint32 GetState(void) const;
   bool WasPcAvailable(void) const;

private:
   //Avoid call
   C_SyvRoRouteCalculation(const C_SyvRoRouteCalculation &);
   C_SyvRoRouteCalculation & operator =(const C_SyvRoRouteCalculation &);

   stw_opensyde_core::C_OSCRoutingCalculation * mpc_Calculation;
   bool mq_PcAvailable;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
