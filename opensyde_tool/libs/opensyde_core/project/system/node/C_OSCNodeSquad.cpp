//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       The squad of nodes. In case of multiple CPUs it has all indexes of the sub nodes and has its base name

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"

#include "C_OSCNodeSquad.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_scl::C_SCLString C_OSCNodeSquad::hc_Separator = "::";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeSquad::C_OSCNodeSquad(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeSquad::~C_OSCNodeSquad(void)
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
stw_scl::C_SCLString C_OSCNodeSquad::h_CombineNames(const stw_scl::C_SCLString & orc_MainDeviceName,
                                                    const stw_scl::C_SCLString & orc_SubDeviceName)
{
   stw_scl::C_SCLString c_Retval;
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
      c_Retval = orc_MainDeviceName + C_OSCNodeSquad::hc_Separator + orc_SubDeviceName;
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
sint32 C_OSCNodeSquad::SetBaseName(std::vector<C_OSCNode> & orc_Nodes, const stw_scl::C_SCLString & orc_NodeBaseName)
{
   // Check the indexes first
   uint32 u32_NodeIndexCounter;
   sint32 s32_Return = C_NO_ERR;

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
         const uint32 u32_NodeIndex = this->c_SubNodeIndexes[u32_NodeIndexCounter];

         if (u32_NodeIndex < orc_Nodes.size())
         {
            C_OSCNode & rc_Node = orc_Nodes[u32_NodeIndex];

            if ((rc_Node.pc_DeviceDefinition != NULL) &&
                (rc_Node.u32_SubDeviceIndex < rc_Node.pc_DeviceDefinition->c_SubDevices.size()))
            {
               // Setting the new name for the sub node
               rc_Node.c_Properties.c_Name =
                  C_OSCNodeSquad::h_CombineNames(orc_NodeBaseName,
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
bool C_OSCNodeSquad::h_CheckIsMultiDevice(const uint32 ou32_NodeIndex,
                                          const std::vector<C_OSCNodeSquad> & orc_AvailableGroups,
                                          uint32 * const opu32_GroupIndex)
{
   bool q_Retval = false;

   if (opu32_GroupIndex != NULL)
   {
      *opu32_GroupIndex = 0UL;
   }

   for (uint32 u32_ItGroup = 0; u32_ItGroup < orc_AvailableGroups.size(); ++u32_ItGroup)
   {
      const C_OSCNodeSquad & rc_Group = orc_AvailableGroups[u32_ItGroup];

      for (uint32 u32_ItSubDevice = 0UL; u32_ItSubDevice < rc_Group.c_SubNodeIndexes.size(); ++u32_ItSubDevice)
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
