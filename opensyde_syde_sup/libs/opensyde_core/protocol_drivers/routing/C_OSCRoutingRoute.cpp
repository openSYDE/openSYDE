//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Holding the necessary information about a concrete route to a specific node from the PC.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscRoutingRoute.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscRoutingRoutePoint::C_OscRoutingRoutePoint(void) :
   u32_NodeIndex(0U),
   u32_InBusIndex(0U),
   u8_InNodeId(0U),
   u8_InInterfaceNumber(0U),
   e_InInterfaceType(C_OscSystemBus::eCAN),
   u32_OutBusIndex(0U),
   u8_OutNodeId(0U),
   u8_OutInterfaceNumber(0U),
   e_OutInterfaceType(C_OscSystemBus::eCAN)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current not equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   false     Current equal to orc_Cmp
   true      Current not equal to orc_Cmp
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscRoutingRoutePoint::operator !=(const C_OscRoutingRoutePoint & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_NodeIndex != orc_Cmp.u32_NodeIndex) ||
       (this->u32_InBusIndex != orc_Cmp.u32_InBusIndex) ||
       (this->u8_InNodeId != orc_Cmp.u8_InNodeId) ||
       (this->u8_InInterfaceNumber != orc_Cmp.u8_InInterfaceNumber) ||
       (this->e_InInterfaceType != orc_Cmp.e_InInterfaceType) ||
       (this->u32_OutBusIndex != orc_Cmp.u32_OutBusIndex) ||
       (this->u8_OutNodeId != orc_Cmp.u8_OutNodeId) ||
       (this->u8_OutInterfaceNumber != orc_Cmp.u8_OutInterfaceNumber) ||
       (this->e_OutInterfaceType != orc_Cmp.e_OutInterfaceType))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   true      Current equal to orc_Cmp
   false     Current not equal to orc_Cmp
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscRoutingRoutePoint::operator ==(const C_OscRoutingRoutePoint & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_NodeIndex == orc_Cmp.u32_NodeIndex) &&
       (this->u32_InBusIndex == orc_Cmp.u32_InBusIndex) &&
       (this->u8_InNodeId == orc_Cmp.u8_InNodeId) &&
       (this->u8_InInterfaceNumber == orc_Cmp.u8_InInterfaceNumber) &&
       (this->e_InInterfaceType == orc_Cmp.e_InInterfaceType) &&
       (this->u32_OutBusIndex == orc_Cmp.u32_OutBusIndex) &&
       (this->u8_OutNodeId == orc_Cmp.u8_OutNodeId) &&
       (this->u8_OutInterfaceNumber == orc_Cmp.u8_OutInterfaceNumber) &&
       (this->e_OutInterfaceType == orc_Cmp.e_OutInterfaceType))
   {
      q_Return = true;
   }

   return q_Return;
}

C_OscRoutingRoute::C_OscRoutingRoute(const uint32_t ou32_TargetNodeIndex) :
   u32_TargetNodeIndex(ou32_TargetNodeIndex)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   true     Current equal to orc_Cmp
   false    Current not equal to orc_Cmp
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscRoutingRoute::operator ==(const C_OscRoutingRoute & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_TargetNodeIndex == orc_Cmp.u32_TargetNodeIndex) &&
       (this->c_VecRoutePoints.size() == orc_Cmp.c_VecRoutePoints.size()))
   {
      uint32_t u32_Counter;
      q_Return = true;

      // compare the content of the vector
      for (u32_Counter = 0U; u32_Counter < this->c_VecRoutePoints.size(); ++u32_Counter)
      {
         if (this->c_VecRoutePoints[u32_Counter] != orc_Cmp.c_VecRoutePoints[u32_Counter])
         {
            q_Return = false;
            break;
         }
      }
   }

   return q_Return;
}
