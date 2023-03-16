//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       The squad of nodes. In case of multiple CPUs it has all indexes of the sub nodes and has its base name

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"

#include "C_OscNodeSquad.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw::scl::C_SclString C_OscNodeSquad::hc_SEPARATOR = "::";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeSquad::C_OscNodeSquad(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeSquad::~C_OscNodeSquad(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Combine names

   \param[in]  orc_MainDeviceName   Main device name
   \param[in]  orc_SubDeviceName    Sub device name

   \return
   Combined name, if possible
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscNodeSquad::h_CombineNames(const stw::scl::C_SclString & orc_MainDeviceName,
                                                     const stw::scl::C_SclString & orc_SubDeviceName)
{
   stw::scl::C_SclString c_Retval;
   if (orc_MainDeviceName.IsEmpty())
   {
      c_Retval = orc_SubDeviceName;
   }
   else if (orc_SubDeviceName.IsEmpty())
   {
      c_Retval = orc_MainDeviceName;
   }
   else
   {
      c_Retval = orc_MainDeviceName + C_OscNodeSquad::hc_SEPARATOR + orc_SubDeviceName;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the base name for the squad and updates the names of all sub nodes

   \param[in,out]  orc_Nodes           Vector of all available nodes. Sub nodes of node squad must be part of it
   \param[in]      orc_NodeBaseName    New name of squad node

   \retval   C_NO_ERR   Base name and names of all sub nodes updated
   \retval   C_RANGE    At least one sub node index does not exist in orc_Nodes
   \retval   C_CONFIG   At least one sub node device definition is not valid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeSquad::SetBaseName(std::vector<C_OscNode> & orc_Nodes, const stw::scl::C_SclString & orc_NodeBaseName)
{
   // Check the indexes first
   uint32_t u32_NodeIndexCounter;
   int32_t s32_Return = C_NO_ERR;

   this->c_BaseName = orc_NodeBaseName;

   for (u32_NodeIndexCounter = 0U; u32_NodeIndexCounter < this->c_SubNodeIndexes.size(); ++u32_NodeIndexCounter)
   {
      if (this->c_SubNodeIndexes[u32_NodeIndexCounter] >= orc_Nodes.size())
      {
         // At least one index is not valid
         s32_Return = C_RANGE;
         break;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      for (u32_NodeIndexCounter = 0U; u32_NodeIndexCounter < this->c_SubNodeIndexes.size(); ++u32_NodeIndexCounter)
      {
         const uint32_t u32_NodeIndex = this->c_SubNodeIndexes[u32_NodeIndexCounter];

         if (u32_NodeIndex < orc_Nodes.size())
         {
            C_OscNode & rc_Node = orc_Nodes[u32_NodeIndex];

            if ((rc_Node.pc_DeviceDefinition != NULL) &&
                (rc_Node.u32_SubDeviceIndex < rc_Node.pc_DeviceDefinition->c_SubDevices.size()))
            {
               // Setting the new name for the sub node
               rc_Node.c_Properties.c_Name =
                  C_OscNodeSquad::h_CombineNames(orc_NodeBaseName,
                                                 rc_Node.pc_DeviceDefinition->c_SubDevices[rc_Node.u32_SubDeviceIndex].
                                                 c_SubDeviceName);
            }
            else
            {
               // Should not happen
               s32_Return = C_CONFIG;
               break;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check is multi device

   \param[in]   ou32_NodeIndex         Node index
   \param[in]   orc_AvailableGroups    Available groups
   \param[out]  opu32_GroupIndex       Group index

   \return
   Is multi device
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeSquad::h_CheckIsMultiDevice(const uint32_t ou32_NodeIndex,
                                          const std::vector<C_OscNodeSquad> & orc_AvailableGroups,
                                          uint32_t * const opu32_GroupIndex)
{
   bool q_Retval = false;

   if (opu32_GroupIndex != NULL)
   {
      *opu32_GroupIndex = 0UL;
   }

   for (uint32_t u32_ItGroup = 0; u32_ItGroup < orc_AvailableGroups.size(); ++u32_ItGroup)
   {
      const C_OscNodeSquad & rc_Group = orc_AvailableGroups[u32_ItGroup];

      for (uint32_t u32_ItSubDevice = 0UL; u32_ItSubDevice < rc_Group.c_SubNodeIndexes.size(); ++u32_ItSubDevice)
      {
         if (ou32_NodeIndex == rc_Group.c_SubNodeIndexes[u32_ItSubDevice])
         {
            q_Retval = true;
            if (opu32_GroupIndex != NULL)
            {
               *opu32_GroupIndex = u32_ItGroup;
            }
            break;
         }
      }
   }
   return q_Retval;
}
