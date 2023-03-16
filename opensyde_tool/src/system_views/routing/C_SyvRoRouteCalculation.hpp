//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper class for Calculation of all potential routes to a specific node in a specific view.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVROROUTECALCULATION_H
#define C_SYVROROUTECALCULATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <list>

#include "stwtypes.hpp"

#include "C_OscRoutingCalculation.hpp"
#include "C_OscRoutingRoute.hpp"
#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvRoRouteCalculation
{
public:
   C_SyvRoRouteCalculation(const uint32_t ou32_ViewIndex, const uint32_t ou32_TargetNodeIndex,
                           const stw::opensyde_core::C_OscRoutingCalculation::E_Mode oe_Mode);
   virtual ~C_SyvRoRouteCalculation();

   const std::vector<stw::opensyde_core::C_OscRoutingRoute> * GetRoutes(void) const;
   const stw::opensyde_core::C_OscRoutingRoute * GetBestRoute(void) const;
   bool CheckItfNumberForRouting(const uint32_t ou32_TargetNodeIndex,
                                 const stw::opensyde_core::C_OscNodeComInterfaceSettings & orc_ComItfSettings) const;
   int32_t GetState(void) const;
   bool WasPcAvailable(void) const;

private:
   //Avoid call
   C_SyvRoRouteCalculation(const C_SyvRoRouteCalculation &);
   C_SyvRoRouteCalculation & operator =(const C_SyvRoRouteCalculation &) &;

   stw::opensyde_core::C_OscRoutingCalculation * mpc_Calculation;
   bool mq_PcAvailable;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
