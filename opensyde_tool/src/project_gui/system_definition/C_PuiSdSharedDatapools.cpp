//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System definition shared Datapool information (implementation)

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <algorithm> //for std::sort

#include "C_PuiSdSharedDatapools.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
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
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdSharedDatapools::C_PuiSdSharedDatapools(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdSharedDatapools::CalcHash(uint32 & oru32_HashValue) const
{
   std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> >::const_iterator c_ItSharedGroup;
   uint32 u32_GroupCounter = 0U;

   for (c_ItSharedGroup = this->c_SharedDatapools.begin(); c_ItSharedGroup != this->c_SharedDatapools.end();
        ++c_ItSharedGroup)
   {
      uint32 u32_DpCounter;

      // The group as index is relevant too
      stw_scl::C_SCLChecksums::CalcCRC32(&u32_GroupCounter, sizeof(u32_GroupCounter), oru32_HashValue);

      for (u32_DpCounter = 0U; u32_DpCounter < c_ItSharedGroup->size(); ++u32_DpCounter)
      {
         const C_OSCNodeDataPoolId & rc_DpId = c_ItSharedGroup->at(u32_DpCounter);
         stw_scl::C_SCLChecksums::CalcCRC32(&rc_DpId.u32_NodeIndex, sizeof(rc_DpId.u32_NodeIndex), oru32_HashValue);
         stw_scl::C_SCLChecksums::CalcCRC32(&rc_DpId.u32_DataPoolIndex, sizeof(rc_DpId.u32_DataPoolIndex),
                                            oru32_HashValue);
      }

      ++u32_GroupCounter;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clears all data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdSharedDatapools::Clear(void)
{
   this->c_SharedDatapools.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adding a new shared Datapool to a shared Datapool group

   \param[in]  orc_NewDatapool      ID of new Datapool
   \param[in]  orc_ShareDatapool    ID of the Datapool which shall be the share partner
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdSharedDatapools::AddSharedDatapool(const C_OSCNodeDataPoolId & orc_NewDatapool,
                                               const C_OSCNodeDataPoolId & orc_ShareDatapool)
{
   uint32 u32_GroupIndex = 0U;

   // Check if the new Datapool was not already shared. That should not happen
   tgl_assert(this->IsSharedDatapool(orc_NewDatapool, NULL) == false);

   // Is the shared Datapool already shared or are both Datapools still stand alone Datapools
   if (this->IsSharedDatapool(orc_ShareDatapool, &u32_GroupIndex) == true)
   {
      // Group exists already
      tgl_assert(u32_GroupIndex < this->c_SharedDatapools.size());
      this->c_SharedDatapools[u32_GroupIndex].push_back(orc_NewDatapool);

      std::sort(this->c_SharedDatapools[u32_GroupIndex].begin(), this->c_SharedDatapools[u32_GroupIndex].end());
   }
   else
   {
      // New group
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> c_Group;

      c_Group.push_back(orc_ShareDatapool);
      c_Group.push_back(orc_NewDatapool);

      std::sort(c_Group.begin(), c_Group.end());

      this->c_SharedDatapools.push_back(c_Group);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adding a new shared Datapool to a specific shared Datapool group

   \param[in]  orc_NewDatapool            ID of new Datapool
   \param[in]  ou32_SharedDatapoolGroup   Index of shared Datapool group

   \retval C_NO_ERR   everything ok
   \retval C_RANGE    Index of shared Datapool group out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdSharedDatapools::AddSharedDatapool(const C_OSCNodeDataPoolId & orc_NewDatapool,
                                                 const uint32 ou32_SharedDatapoolGroup)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_SharedDatapoolGroup < this->c_SharedDatapools.size())
   {
      this->c_SharedDatapools[ou32_SharedDatapoolGroup].push_back(orc_NewDatapool);

      std::sort(this->c_SharedDatapools[ou32_SharedDatapoolGroup].begin(),
                this->c_SharedDatapools[ou32_SharedDatapoolGroup].end());

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removing a shared Datapool from a shared Datapool group

   \param[in]  orc_Datapool   ID of Datapool for remove

   \return
   C_NO_ERR    Datapool Id removed from group
   C_WARN      Entire group was removed due to less other entries in this group
   C_NOACT     Nothing was removed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdSharedDatapools::RemoveSharedDatapool(const C_OSCNodeDataPoolId & orc_Datapool)
{
   sint32 s32_Return = C_NOACT;

   std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> >::iterator c_ItGroup;

   // Search for a matching Datapool
   for (c_ItGroup = this->c_SharedDatapools.begin(); c_ItGroup != this->c_SharedDatapools.end(); ++c_ItGroup)
   {
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> & rc_Group = *c_ItGroup;
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolId>::iterator c_ItSharedDatapool;
      bool q_Found = false;

      for (c_ItSharedDatapool = rc_Group.begin(); c_ItSharedDatapool != rc_Group.end(); ++c_ItSharedDatapool)
      {
         if (orc_Datapool == *c_ItSharedDatapool)
         {
            if (rc_Group.size() > 2)
            {
               // Erase one element from the group
               rc_Group.erase(c_ItSharedDatapool);
               s32_Return = C_NO_ERR;
            }
            else
            {
               // Special case: Only one Datapool would be left. Nothing to share anymore. Remove the entire group.
               this->c_SharedDatapools.erase(c_ItGroup);
               s32_Return = C_WARN;
            }

            q_Found = true;
            break;
         }
      }

      if (q_Found == true)
      {
         break;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the Datapool is shared

   \param[in]   ou32_SharedNodeIndex         Node index of Datapool for check
   \param[in]   ou32_SharedDatapoolIndex     Datapool index of Datapool for check
   \param[out]  opu32_SharedDatapoolGroup    Optional pointer to return index of shared Datapool group
                                             Can be used with function GetSharedDatapoolGroup

   \return
   true     Datapool is shared
   false    Datapool is stand-alone
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdSharedDatapools::IsSharedDatapool(const uint32 ou32_SharedNodeIndex, const uint32 ou32_SharedDatapoolIndex,
                                              uint32 * const opu32_SharedDatapoolGroup) const
{
   return this->IsSharedDatapool(C_OSCNodeDataPoolId(ou32_SharedNodeIndex, ou32_SharedDatapoolIndex),
                                 opu32_SharedDatapoolGroup);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the Datapool is shared

   \param[in]   orc_Datapool                 ID of Datapool for check
   \param[out]  opu32_SharedDatapoolGroup    Optional pointer to return index of shared Datapool group
                                             Can be used with function GetSharedDatapoolGroup

   \return
   true     Datapool is shared
   false    Datapool is stand-alone
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdSharedDatapools::IsSharedDatapool(const C_OSCNodeDataPoolId & orc_Datapool,
                                              uint32 * const opu32_SharedDatapoolGroup) const
{
   uint32 u32_GroupCounter;
   bool q_Found = false;

   // Search for a matching Datapool
   for (u32_GroupCounter = 0U; u32_GroupCounter < this->c_SharedDatapools.size(); ++u32_GroupCounter)
   {
      uint32 u32_SharedDatapoolCounter;
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> & rc_Group = this->c_SharedDatapools[u32_GroupCounter];

      for (u32_SharedDatapoolCounter = 0U; u32_SharedDatapoolCounter < rc_Group.size(); ++u32_SharedDatapoolCounter)
      {
         if (orc_Datapool == rc_Group[u32_SharedDatapoolCounter])
         {
            if (opu32_SharedDatapoolGroup != NULL)
            {
               // Return the group index
               *opu32_SharedDatapoolGroup = u32_GroupCounter;
            }

            q_Found = true;
            break;
         }
      }

      if (q_Found == true)
      {
         break;
      }
   }

   return q_Found;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all connected shared Datapools of one group

   \param[in]   ou32_SharedDatapoolGroup  Index of shared Datapool group
   \param[out]  orc_SharedDatapoolGroup   Result vector with all connected shared Datapool IDs of one group
                                          Will be cleared in any case

   \return
   C_NO_ERR    Shared Datapool group found and copied
   C_RANGE     ou32_SharedDatapoolGroup is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdSharedDatapools::GetSharedDatapoolGroup(const uint32 ou32_SharedDatapoolGroup,
                                                      std::vector<C_OSCNodeDataPoolId> & orc_SharedDatapoolGroup)
const
{
   sint32 s32_Return = C_RANGE;

   orc_SharedDatapoolGroup.clear();

   if (ou32_SharedDatapoolGroup < this->c_SharedDatapools.size())
   {
      s32_Return = C_NO_ERR;
      // Copy the entire group
      orc_SharedDatapoolGroup = this->c_SharedDatapools[ou32_SharedDatapoolGroup];
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the Datapool is shared and returns the Datapool Ids of the shared group

   The Datapool Id of the parameter orc_Datapool will not be added to orc_SharedDatapoolGroup.
   Only the shared Datapools of orc_Datapool will be added.

   \param[in]   ou32_SharedNodeIndex      Node index of Datapool for check
   \param[in]   ou32_SharedDatapoolIndex  Datapool index of Datapool for check
   \param[out]  orc_SharedDatapoolGroup   Result vector with all connected shared Datapool IDs of one group
                                          Will be cleared in any case

   \return
   true     Datapool is shared
   false    Datapool is stand-alone
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdSharedDatapools::IsSharedAndGetDatapoolGroup(const uint32 ou32_SharedNodeIndex,
                                                         const uint32 ou32_SharedDatapoolIndex,
                                                         std::vector<C_OSCNodeDataPoolId> & orc_SharedDatapoolGroup)
const
{
   return this->IsSharedAndGetDatapoolGroup(C_OSCNodeDataPoolId(ou32_SharedNodeIndex, ou32_SharedDatapoolIndex),
                                            orc_SharedDatapoolGroup);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the Datapool is shared and returns the Datapool Ids of the shared group

   The Datapool Id of the parameter orc_Datapool will not be added to orc_SharedDatapoolGroup.
   Only the shared Datapools of orc_Datapool will be added.

   \param[in]   orc_Datapool              ID of Datapool for check
   \param[out]  orc_SharedDatapoolGroup   Result vector with all connected shared Datapool IDs of one group
                                          Will be cleared in any case

   \return
   true     Datapool is shared
   false    Datapool is stand-alone
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdSharedDatapools::IsSharedAndGetDatapoolGroup(const C_OSCNodeDataPoolId & orc_Datapool,
                                                         std::vector<C_OSCNodeDataPoolId> & orc_SharedDatapoolGroup)
const
{
   uint32 u32_SharedGroup;
   bool q_Found = this->IsSharedDatapool(orc_Datapool, &u32_SharedGroup);

   orc_SharedDatapoolGroup.clear();

   if (q_Found == true)
   {
      // Copy the shared Datapool Ids
      uint32 u32_DpIdCounter;
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> & rc_Group = this->c_SharedDatapools[u32_SharedGroup];

      for (u32_DpIdCounter = 0U; u32_DpIdCounter < rc_Group.size(); ++u32_DpIdCounter)
      {
         // Add only the other shared Datapool Ids
         if (rc_Group[u32_DpIdCounter] != orc_Datapool)
         {
            orc_SharedDatapoolGroup.push_back(rc_Group[u32_DpIdCounter]);
         }
      }
   }

   return q_Found;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the existing shared Datapool configuration when a node was removed

   \param[in]  ou32_NodeIndex    Index of removed node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdSharedDatapools::OnNodeRemoved(const uint32 ou32_NodeIndex)
{
   uint32 u32_GroupCounter = 0U;

   // Check all groups
   while (u32_GroupCounter < this->c_SharedDatapools.size())
   {
      uint32 u32_DpIdCounter = 0U;
      std::vector<C_OSCNodeDataPoolId> & rc_Group = this->c_SharedDatapools[u32_GroupCounter];
      bool q_GroupRemoved = false;

      while (u32_DpIdCounter < rc_Group.size())
      {
         if (rc_Group[u32_DpIdCounter].u32_NodeIndex == ou32_NodeIndex)
         {
            // Search for Datapool Ids which have to be removed
            if (this->RemoveSharedDatapool(rc_Group[u32_DpIdCounter]) == C_WARN)
            {
               q_GroupRemoved = true;
               break;
            }
         }
         else if (rc_Group[u32_DpIdCounter].u32_NodeIndex > ou32_NodeIndex)
         {
            // Adapt Datapool Ids
            --rc_Group[u32_DpIdCounter].u32_NodeIndex;
            ++u32_DpIdCounter;
         }
         else
         {
            // Nothing to adapt
            ++u32_DpIdCounter;
         }
      }

      if (q_GroupRemoved == false)
      {
         ++u32_GroupCounter;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the existing shared Datapool configuration when a new Datapool was inserted

   All Datapool indexes of the same node equal or higher than the new Datapool index will be increased

   \param[in]  orc_Datapool   ID of Datapool for remove
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdSharedDatapools::OnDatapoolInserted(const C_OSCNodeDataPoolId & orc_Datapool)
{
   uint32 u32_GroupCounter;

   // Check all groups
   for (u32_GroupCounter = 0U; u32_GroupCounter < this->c_SharedDatapools.size(); ++u32_GroupCounter)
   {
      uint32 u32_DpIdCounter;
      std::vector<C_OSCNodeDataPoolId> & rc_Group = this->c_SharedDatapools[u32_GroupCounter];

      for (u32_DpIdCounter = 0U; u32_DpIdCounter < rc_Group.size(); ++u32_DpIdCounter)
      {
         C_OSCNodeDataPoolId & rc_CurDpId = rc_Group[u32_DpIdCounter];

         // Only Datapools of same node are relevant
         if ((rc_CurDpId.u32_NodeIndex == orc_Datapool.u32_NodeIndex) &&
             (rc_CurDpId.u32_DataPoolIndex >= orc_Datapool.u32_DataPoolIndex))
         {
            ++rc_CurDpId.u32_DataPoolIndex;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the existing shared Datapool configuration when a Datapool was removed

   All Datapool indexes of the same node higher than the new Datapool index will be decreased.
   If the removed Datapool was a shared Datapool, its entry will be removed too.

   \param[in]  orc_Datapool   ID of Datapool for remove
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdSharedDatapools::OnDatapoolRemoved(const C_OSCNodeDataPoolId & orc_Datapool)
{
   uint32 u32_GroupCounter;

   // Search for an complete match for removing first for each group to avoid removing an already adapted Datapool Id
   this->RemoveSharedDatapool(orc_Datapool);

   // Check all groups
   for (u32_GroupCounter = 0U; u32_GroupCounter < this->c_SharedDatapools.size(); ++u32_GroupCounter)
   {
      uint32 u32_DpIdCounter;
      std::vector<C_OSCNodeDataPoolId> & rc_Group = this->c_SharedDatapools[u32_GroupCounter];

      for (u32_DpIdCounter = 0U; u32_DpIdCounter < rc_Group.size(); ++u32_DpIdCounter)
      {
         C_OSCNodeDataPoolId & rc_CurDpId = rc_Group[u32_DpIdCounter];

         // Only Datapools of same node are relevant
         if ((rc_CurDpId.u32_NodeIndex == orc_Datapool.u32_NodeIndex) &&
             (rc_CurDpId.u32_DataPoolIndex > orc_Datapool.u32_DataPoolIndex))
         {
            --rc_CurDpId.u32_DataPoolIndex;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the existing shared Datapool configuration when a Datapool was moved

   \param[in]  orc_SourceDatapool   ID of source Datapool for move
   \param[in]  orc_TargetDatapool   ID of target Datapool for move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdSharedDatapools::OnDatapoolMoved(const C_OSCNodeDataPoolId & orc_SourceDatapool,
                                             const C_OSCNodeDataPoolId & orc_TargetDatapool)
{
   uint32 u32_SharedGroup = 0U;
   // Save if the Datapool is a shared Datapool
   const bool q_IsShared = this->IsSharedDatapool(orc_SourceDatapool, &u32_SharedGroup);

   std::vector<C_OSCNodeDataPoolId> c_SharedGroup;

   if (q_IsShared == true)
   {
      // Save the group in case of a shared Datapool
      tgl_assert(this->GetSharedDatapoolGroup(u32_SharedGroup, c_SharedGroup) == C_NO_ERR);
   }

   // Handle the origin Datapool Id as removed
   this->OnDatapoolRemoved(orc_SourceDatapool);

   // And the new Datapool Id as inserted
   this->OnDatapoolInserted(orc_TargetDatapool);

   if (q_IsShared == true)
   {
      // Register the Datapool as shared Datapool again
      if (c_SharedGroup.size() > 2)
      {
         // Standard case. Add the adapted Datapool Id as shared Datapool again
         tgl_assert(this->AddSharedDatapool(orc_TargetDatapool, u32_SharedGroup) == C_NO_ERR);
      }
      else if (c_SharedGroup.size() == 2)
      {
         // Special case. Shared Datapool with only one other Datapool
         // The group was removed completely with OnDatapoolRemoved.
         // Add both Datapool Ids again.
         C_OSCNodeDataPoolId c_OtherDatapoolId;

         // Get the "other" Datapool Id
         if (c_SharedGroup[0] == orc_SourceDatapool)
         {
            c_OtherDatapoolId = c_SharedGroup[1];
         }
         else
         {
            c_OtherDatapoolId = c_SharedGroup[0];
         }

         // Adapt the other Datapool if necessary
         if (c_OtherDatapoolId.u32_NodeIndex == orc_SourceDatapool.u32_NodeIndex)
         {
            // Compensate the "remove" by comparing with the source index
            if (c_OtherDatapoolId.u32_DataPoolIndex > orc_SourceDatapool.u32_DataPoolIndex)
            {
               --c_OtherDatapoolId.u32_DataPoolIndex;
            }

            // Compensate the "insert" by comparing with the target index
            if (c_OtherDatapoolId.u32_DataPoolIndex >= orc_TargetDatapool.u32_DataPoolIndex)
            {
               ++c_OtherDatapoolId.u32_DataPoolIndex;
            }
         }

         // Add the pair of Datapool Ids again
         this->AddSharedDatapool(orc_TargetDatapool, c_OtherDatapoolId);
      }
      else
      {
         // Should not happen
         tgl_assert(c_SharedGroup.size() < 2);
      }
   }
}
