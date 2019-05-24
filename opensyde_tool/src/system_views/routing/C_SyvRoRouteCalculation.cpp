//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper class for Calculation of all potential routes to a specific node in a specific view.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SyvRoRouteCalculation.h"

#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"
#include "C_PuiSvPc.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Use local only! (uses reference on nodes)

   \param[in]     ou32_ViewIndex       View index
   \param[in]     ou32_TargetNodeIndex Target node at the end of the routing
   \param[in]     oe_Mode              Decision for update or diagnostic routing
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvRoRouteCalculation::C_SyvRoRouteCalculation(const uint32 ou32_ViewIndex, const uint32 ou32_TargetNodeIndex,
                                                 const C_OSCRoutingCalculation::E_Mode oe_Mode)
{
   const stw_opensyde_core::C_OSCSystemDefinition & rc_SystemDefintion =
      C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst();
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);
   const C_PuiSvPc & rc_Pc = pc_View->GetPcData();

   this->mq_PcAvailable = rc_Pc.GetConnected();

   this->mpc_Calculation = new C_OSCRoutingCalculation(rc_SystemDefintion.c_Nodes,
                                                       pc_View->GetNodeActiveFlags(),
                                                       rc_Pc.GetBusIndex(),
                                                       ou32_TargetNodeIndex, oe_Mode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvRoRouteCalculation::~C_SyvRoRouteCalculation()
{
   delete this->mpc_Calculation;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all found routes

   \return
   Pointer to Vector with all found routes
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OSCRoutingRoute> * C_SyvRoRouteCalculation::GetRoutes(void) const
{
   return this->mpc_Calculation->GetRoutes();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the best route

   \return
   Valid Pointer:    Best route to target
   NULL:             No route found
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCRoutingRoute * C_SyvRoRouteCalculation::GetBestRoute(void) const
{
   return this->mpc_Calculation->GetBestRoute();
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvRoRouteCalculation::CheckItfNumberForRouting(const uint32 ou32_TargetNodeIndex,
                                                       const C_OSCNodeComInterfaceSettings & orc_ComItfSettings) const
{
   return this->mpc_Calculation->CheckItfNumberForRouting(ou32_TargetNodeIndex, orc_ComItfSettings);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the result state of the calculation

   \return
   C_NO_ERR    Routes were found
   C_RANGE     Target node does not exist
   C_COM       No routes were found
   C_NOACT     Target function (update or diagnostic) deactivated on all connected bus. No routing necessary.
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvRoRouteCalculation::GetState(void) const
{
   return this->mpc_Calculation->GetState();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state of the PC at routing calculation time

   \return
   true     PC was available
   false    PC was not available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvRoRouteCalculation::WasPcAvailable(void) const
{
   return this->mq_PcAvailable;
}
