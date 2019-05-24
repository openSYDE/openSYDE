//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Holding the necessary information about a concrete route to a specific node from the PC.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCRoutingRoute.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;

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
C_OSCRoutingRoutePoint::C_OSCRoutingRoutePoint(void) :
   u32_NodeIndex(0U),
   u32_InBusIndex(0U),
   u8_InNodeID(0U),
   u8_InInterfaceNumber(0U),
   e_InInterfaceType(C_OSCSystemBus::eCAN),
   u32_OutBusIndex(0U),
   u8_OutNodeID(0U),
   u8_OutInterfaceNumber(0U),
   e_OutInterfaceType(C_OSCSystemBus::eCAN)
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
bool C_OSCRoutingRoutePoint::operator !=(const C_OSCRoutingRoutePoint & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_NodeIndex != orc_Cmp.u32_NodeIndex) ||
       (this->u32_InBusIndex != orc_Cmp.u32_InBusIndex) ||
       (this->u8_InNodeID != orc_Cmp.u8_InNodeID) ||
       (this->u8_InInterfaceNumber != orc_Cmp.u8_InInterfaceNumber) ||
       (this->e_InInterfaceType != orc_Cmp.e_InInterfaceType) ||
       (this->u32_OutBusIndex != orc_Cmp.u32_OutBusIndex) ||
       (this->u8_OutNodeID != orc_Cmp.u8_OutNodeID) ||
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
bool C_OSCRoutingRoutePoint::operator ==(const C_OSCRoutingRoutePoint & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_NodeIndex == orc_Cmp.u32_NodeIndex) &&
       (this->u32_InBusIndex == orc_Cmp.u32_InBusIndex) &&
       (this->u8_InNodeID == orc_Cmp.u8_InNodeID) &&
       (this->u8_InInterfaceNumber == orc_Cmp.u8_InInterfaceNumber) &&
       (this->e_InInterfaceType == orc_Cmp.e_InInterfaceType) &&
       (this->u32_OutBusIndex == orc_Cmp.u32_OutBusIndex) &&
       (this->u8_OutNodeID == orc_Cmp.u8_OutNodeID) &&
       (this->u8_OutInterfaceNumber == orc_Cmp.u8_OutInterfaceNumber) &&
       (this->e_OutInterfaceType == orc_Cmp.e_OutInterfaceType))
   {
      q_Return = true;
   }

   return q_Return;
}

C_OSCRoutingRoute::C_OSCRoutingRoute(const uint32 ou32_TargetNodeIndex) :
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
bool C_OSCRoutingRoute::operator ==(const C_OSCRoutingRoute & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_TargetNodeIndex == orc_Cmp.u32_TargetNodeIndex) &&
       (this->c_VecRoutePoints.size() == orc_Cmp.c_VecRoutePoints.size()))
   {
      uint32 u32_Counter;
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
