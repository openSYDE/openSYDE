//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper class for Calculation of all potential routes to a specific node in a specific view.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvRoRouteCalculation.hpp"

#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiSvData.hpp"
#include "C_PuiSvPc.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

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
C_SyvRoRouteCalculation::C_SyvRoRouteCalculation(const uint32_t ou32_ViewIndex, const uint32_t ou32_TargetNodeIndex,
                                                 const C_OscRoutingCalculation::E_Mode oe_Mode)
{
   const stw::opensyde_core::C_OscSystemDefinition & rc_SystemDefintion =
      C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst();
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);
   const stw::opensyde_core::C_OscViewPc & rc_Pc = pc_View->GetOscPcData();

   this->mq_PcAvailable = rc_Pc.GetConnected();

   // Use the original node active flags. The potential deactivated sub nodes has no impact for the usages as
   // routing points
   this->mpc_Calculation = new C_OscRoutingCalculation(rc_SystemDefintion.c_Nodes,
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
const std::vector<C_OscRoutingRoute> * C_SyvRoRouteCalculation::GetRoutes(void) const
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
const C_OscRoutingRoute * C_SyvRoRouteCalculation::GetBestRoute(void) const
{
   return this->mpc_Calculation->GetBestRoute();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks a specific interface of a specific node if it is relevant and usable

   \param[in]       ou32_TargetNodeIndex     Node index of node to check
   \param[in]       orc_ComItfSettings       Interface of node for check

   \retval   true   Interface can be used
   \retval   false  Interface can not be used or is not relevant
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvRoRouteCalculation::CheckItfNumberForRouting(const uint32_t ou32_TargetNodeIndex,
                                                       const C_OscNodeComInterfaceSettings & orc_ComItfSettings) const
{
   return this->mpc_Calculation->CheckItfNumberForRouting(ou32_TargetNodeIndex, orc_ComItfSettings);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the result state of the calculation

   \return
   C_NO_ERR    Routes were found
   C_RANGE     Target node does not exist
   C_COM       No routes were found
   C_CONFIG    A route was removed due to not possible routing from CAN to Ethernet
   C_NOACT     Target function (update or diagnostic) deactivated on all connected bus. No routing necessary.
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvRoRouteCalculation::GetState(void) const
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
