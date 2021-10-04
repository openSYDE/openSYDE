//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class to hold node specific data in C_GiSvNodeSyvUpdate.

   Data class to hold node specific data in C_GiSvNodeSyvUpdate.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_PuiSdHandler.h"
#include "C_GiSvNodeData.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvNodeData::C_GiSvNodeData() :
   mq_UpdateConnected(false),
   mq_UpdateInProgress(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::Init(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex)
{
   uint32 u32_NodeSquadIndex;

   if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(ou32_NodeIndex,
                                                                       u32_NodeSquadIndex) == C_NO_ERR)
   {
      // part of a node squad
      const stw_opensyde_core::C_OSCNodeSquad * const pc_NodeSquad =
         C_PuiSdHandler::h_GetInstance()->GetOSCNodeSquadConst(u32_NodeSquadIndex);
      if (pc_NodeSquad != NULL)
      {
         std::vector<stw_types::uint32>::const_iterator c_ItSubNodeIndices;
         for (c_ItSubNodeIndices = pc_NodeSquad->c_SubNodeIndexes.begin();
              c_ItSubNodeIndices != pc_NodeSquad->c_SubNodeIndexes.end();
              ++c_ItSubNodeIndices)
         {
            const C_GiSvSubNodeData c_SubNode(ou32_ViewIndex, *c_ItSubNodeIndices);
            this->mc_SubNodes.push_back(c_SubNode);
         }
      }
   }
   else
   {
      // normal node
      const C_GiSvSubNodeData c_SubNode(ou32_ViewIndex, ou32_NodeIndex);
      this->mc_SubNodes.push_back(c_SubNode);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connection change

   \param[in]  oq_Active   Flag if connected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::SetConnected(const bool oq_Active)
{
   this->mq_UpdateConnected = oq_Active;
   for (uint32 u32_ItSubNodes = 0; u32_ItSubNodes < this->mc_SubNodes.size(); u32_ItSubNodes++)
   {
      this->mc_SubNodes[u32_ItSubNodes].SetConnected(oq_Active);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal if update in progress

   \param[in]  oq_Active   Flag if update in progress
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::SetUpdating(const bool oq_Active)
{
   this->mq_UpdateInProgress = oq_Active;
   for (uint32 u32_ItSubNodes = 0; u32_ItSubNodes < this->mc_SubNodes.size(); u32_ItSubNodes++)
   {
      this->mc_SubNodes[u32_ItSubNodes].SetUpdating(oq_Active);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set update waiting state

   False: Waiting
   True: In progress
   False after true once: Update success

   \param[in]  oq_Active                     Flag if update in progress
   \param[in]  oq_Aborted                    Flag if action was aborted
   \param[in]  ou32_FailedApplicationIndex   If aborted and currently updating this is the currently updated application
   \param[in]  ou32_UpdatedNodeIndex         Updated node index
   \param[in]  ou32_CurrentNodeIndex         Current node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                             const uint32 ou32_FailedApplicationIndex,
                                             const uint32 ou32_UpdatedNodeIndex, const uint32 ou32_CurrentNodeIndex)
{
   if (oq_Active)
   {
      const std::vector<uint32> c_NodeIndices = C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
         ou32_CurrentNodeIndex);
      if (c_NodeIndices.size() == this->mc_SubNodes.size())
      {
         for (uint32 u32_ItSubNodes = 0; u32_ItSubNodes < this->mc_SubNodes.size();
              u32_ItSubNodes++)
         {
            const C_OSCNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndices[u32_ItSubNodes]);
            if ((pc_Node != NULL) && (pc_Node->u32_SubDeviceIndex < this->mc_SubNodes.size()))
            {
               if (ou32_UpdatedNodeIndex == c_NodeIndices[u32_ItSubNodes])
               {
                  this->mc_SubNodes[pc_Node->u32_SubDeviceIndex].SetNodeUpdateInProgress(oq_Active, oq_Aborted,
                                                                                         ou32_FailedApplicationIndex);
               }
               else
               {
                  this->mc_SubNodes[pc_Node->u32_SubDeviceIndex].SetNodeUpdateInProgress(false, oq_Aborted,
                                                                                         ou32_FailedApplicationIndex);
               }
            }
         }
      }
   }
   else
   {
      for (uint32 u32_ItSubNodes = 0; u32_ItSubNodes < this->mc_SubNodes.size();
           u32_ItSubNodes++)
      {
         this->mc_SubNodes[u32_ItSubNodes].SetNodeUpdateInProgress(oq_Active, oq_Aborted, ou32_FailedApplicationIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply no response state

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::SetNoResponse(const uint32 ou32_NodeIndex)
{
   const uint32 u32_SubNodeIndex = m_GetCorrespondingSubNodeIndex(ou32_NodeIndex);

   this->mc_SubNodes[u32_SubNodeIndex].SetNoResponse();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update package status

   \param[in]  orc_DeviceApplicationInfos    Device application information
   \param[in]  ou32_NodeIndex                Node index of node where the application information belongs to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::UpdateInitialPackageStatus(const C_SyvUpDeviceInfo & orc_DeviceApplicationInfos,
                                                const uint32 ou32_NodeIndex)
{
   const uint32 u32_SubNodeIndex = m_GetCorrespondingSubNodeIndex(ou32_NodeIndex);

   this->mc_SubNodes[u32_SubNodeIndex].UpdateInitialPackageStatus(orc_DeviceApplicationInfos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy initial status

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::CopyInitialStatus(C_GiSvNodeData & orc_NodeData) const
{
   tgl_assert(this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size());
   if (this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size())
   {
      for (uint32 u32_It = 0UL; u32_It < this->mc_SubNodes.size(); ++u32_It)
      {
         this->mc_SubNodes[u32_It].CopyInitialStatus(orc_NodeData.mc_SubNodes[u32_It]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy update status

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::CopyUpdateStatus(C_GiSvNodeData & orc_NodeData) const
{
   tgl_assert(this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size());
   if (this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size())
   {
      for (uint32 u32_It = 0UL; u32_It < this->mc_SubNodes.size(); ++u32_It)
      {
         this->mc_SubNodes[u32_It].CopyUpdateStatus(orc_NodeData.mc_SubNodes[u32_It]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy STW device info

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::CopySTWDeviceInfo(C_GiSvNodeData & orc_NodeData) const
{
   tgl_assert(this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size());
   if (this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size())
   {
      for (uint32 u32_It = 0UL; u32_It < this->mc_SubNodes.size(); ++u32_It)
      {
         this->mc_SubNodes[u32_It].CopySTWDeviceInfo(orc_NodeData.mc_SubNodes[u32_It]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy OSY device info

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::CopyOSYDeviceInfo(C_GiSvNodeData & orc_NodeData) const
{
   tgl_assert(this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size());
   if (this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size())
   {
      for (uint32 u32_It = 0UL; u32_It < this->mc_SubNodes.size(); ++u32_It)
      {
         this->mc_SubNodes[u32_It].CopyOSYDeviceInfo(orc_NodeData.mc_SubNodes[u32_It]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy discarded status

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::CopyDiscardedStatus(C_GiSvNodeData & orc_NodeData) const
{
   tgl_assert(this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size());
   if (this->mc_SubNodes.size() == orc_NodeData.mc_SubNodes.size())
   {
      for (uint32 u32_It = 0UL; u32_It < this->mc_SubNodes.size(); ++u32_It)
      {
         this->mc_SubNodes[u32_It].CopyDiscardedStatus(orc_NodeData.mc_SubNodes[u32_It]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if STW device and active

   \return
   True  STW device
   False Third party
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::IsStwDevice() const
{
   bool q_Retval = false;

   for (uint32 u32_ItSubNodes = 0; u32_ItSubNodes < this->mc_SubNodes.size(); u32_ItSubNodes++)
   {
      if (this->mc_SubNodes[u32_ItSubNodes].IsStwDevice() == true)
      {
         q_Retval = true;
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Are all nodes valid

   \return
   true  All valid
   false At least one not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::AreAllNodesValid() const
{
   bool q_Retval = true;

   for (uint32 u32_It = 0UL; (u32_It < this->mc_SubNodes.size()) && (q_Retval == true); ++u32_It)
   {
      q_Retval = this->mc_SubNodes[u32_It].GetValidStatus();
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is there any hex file information

   \return
   true  Hex file info present
   false Hex file info not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::IsThereAnyHexFileInformation() const
{
   bool q_Retval = false;

   for (uint32 u32_It = 0UL; (u32_It < this->mc_SubNodes.size()) && (q_Retval == false); ++u32_It)
   {
      q_Retval = this->IsThereAnyHexFileInformationForDevice(u32_It);
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is device info discarded

   \param[in]  ou32_SubDeviceIndex  Sub device index

   \return
   Device info discarded status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::IsDeviceInfoDiscarded(const uint32 ou32_SubDeviceIndex) const
{
   bool q_Retval = false;

   if (ou32_SubDeviceIndex < this->mc_SubNodes.size())
   {
      q_Retval = this->mc_SubNodes[ou32_SubDeviceIndex].IsDeviceInfoDiscarded();
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is there any hex file information for device

   \param[in]  ou32_SubDeviceIndex  Sub device index

   \return
   true  Hex file info present
   false Hex file info not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::IsThereAnyHexFileInformationForDevice(const uint32 ou32_SubDeviceIndex) const
{
   bool q_Retval = false;

   if (ou32_SubDeviceIndex < this->mc_SubNodes.size())
   {
      q_Retval = this->mc_SubNodes[ou32_SubDeviceIndex].IsThereAnyHexFileInformation();
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Discard info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeData::DiscardInfo()
{
   for (uint32 u32_It = 0UL; u32_It < this->mc_SubNodes.size(); ++u32_It)
   {
      this->mc_SubNodes[u32_It].DiscardInfo();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if this node always needs an update

   \retval true  Node always requires update
   \retval false Node might not need update
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::CheckAlwaysUpdate(void) const
{
   bool q_Retval = false;

   for (uint32 u32_It = 0UL; (u32_It < this->mc_SubNodes.size()) && (q_Retval == false); ++u32_It)
   {
      q_Retval = this->mc_SubNodes[u32_It].CheckAlwaysUpdate();
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if node should be in connected but update disabled state

   \retval   True    Update disabled check result
   \retval   False   Update not disabled check result
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::CheckUpdateDisabledState(void) const
{
   bool q_Retval = true;

   for (uint32 u32_It = 0UL; (u32_It < this->mc_SubNodes.size()) && (q_Retval == true); ++u32_It)
   {
      q_Retval = this->mc_SubNodes[u32_It].CheckUpdateDisabledState();
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get update connected status

   \return
   Update connected status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::GetUpdateConnectedStatus() const
{
   return this->mq_UpdateConnected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get update in progress status

   \return
   Update in progress status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::GetUpdateInProgressStatus() const
{
   return this->mq_UpdateInProgress;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get overall update state

   \return
   overall update state
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUtil::E_NodeUpdateStatus C_GiSvNodeData::GetOverallUpdateState() const
{
   C_SyvUtil::E_NodeUpdateStatus e_UpdateStatus = C_SyvUtil::eU_WAITING;
   bool q_AtLeastOneUpdating = false;
   bool q_AtLeastOneWaiting = false;
   bool q_AtLeastOneUpToDate = false;
   bool q_AtLeastOneUpdateSuccess = false;
   bool q_AtLeastOneUpdateDisabled = false;
   bool q_AtLeastOneUnknown = false;

   for (uint32 u32_ItSubNodes = 0; u32_ItSubNodes < this->mc_SubNodes.size(); u32_ItSubNodes++)
   {
      switch (this->mc_SubNodes[u32_ItSubNodes].GetUpdateStatus())
      {
      case C_SyvUtil::eU_UPDATING:
         q_AtLeastOneUpdating = true;
         break;
      case C_SyvUtil::eU_WAITING:
         q_AtLeastOneWaiting = true;
         break;
      case C_SyvUtil::eU_UP_TO_DATE:
         q_AtLeastOneUpToDate = true;
         break;
      case C_SyvUtil::eU_UPDATE_SUCCESS:
         q_AtLeastOneUpdateSuccess = true;
         break;
      case C_SyvUtil::eU_UPDATE_DISABLED:
         q_AtLeastOneUpdateDisabled = true;
         break;
      case C_SyvUtil::eU_UNKNOWN:
      default:
         q_AtLeastOneUnknown = true;
         break;
      }
   }
   if (!q_AtLeastOneUnknown)
   {
      const bool q_AllUpdated = q_AtLeastOneUpdateSuccess && ((!q_AtLeastOneWaiting) && (!q_AtLeastOneUpdating));
      if (q_AllUpdated)
      {
         e_UpdateStatus = C_SyvUtil::eU_UPDATE_SUCCESS;
      }
      else
      {
         const bool q_AllAlreadyUpToDate = q_AtLeastOneUpToDate && ((!q_AtLeastOneWaiting) && (!q_AtLeastOneUpdating));
         if (q_AllAlreadyUpToDate)
         {
            e_UpdateStatus = C_SyvUtil::eU_UP_TO_DATE;
         }
         else
         {
            if (q_AtLeastOneUpdating)
            {
               e_UpdateStatus = C_SyvUtil::eU_UPDATING;
            }
            else
            {
               if (q_AtLeastOneWaiting)
               {
                  e_UpdateStatus = C_SyvUtil::eU_WAITING;
               }
               else
               {
                  if (q_AtLeastOneUpdateDisabled)
                  {
                     e_UpdateStatus = C_SyvUtil::eU_UPDATE_DISABLED;
                  }
                  else
                  {
                     e_UpdateStatus = C_SyvUtil::eU_UNKNOWN;
                  }
               }
            }
         }
      }
   }
   else
   {
      e_UpdateStatus = C_SyvUtil::eU_UNKNOWN;
   }
   return e_UpdateStatus;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get initial state accumulated over all sub nodes

   The following rules apply in this order:
      1. at least one node eI_NO_RESPONSE                   -> over all eI_NO_RESPONSE
      2. at least one node eI_TO_BE_UPDATED                 -> over all eI_TO_BE_UPDATED
      3. at least one node eI_UNKNOWN                       -> over all eI_UNKNOWN (should never occur...)
      4. all nodes eI_UPDATE_DISABLED                       -> over all eI_UPDATE_DISABLED
      5. mix of eI_UPDATE_DISABLED and eI_APPLICATION_MATCH -> over all eI_APPLICATION_MATCH

   \return
   over all initial state
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUtil::E_NodeUpdateInitialStatus C_GiSvNodeData::GetOverallInitialState(void) const
{
   C_SyvUtil::E_NodeUpdateInitialStatus e_InitialState = C_SyvUtil::eI_UNKNOWN;
   bool q_StopChecking = false;

   for (uint32 u32_ItSubNodes = 0; (u32_ItSubNodes < this->mc_SubNodes.size()) && (q_StopChecking == false);
        u32_ItSubNodes++)
   {
      switch (this->mc_SubNodes[u32_ItSubNodes].GetInitialStatus())
      {
      case C_SyvUtil::eI_NO_RESPONSE:
         e_InitialState = C_SyvUtil::eI_NO_RESPONSE;
         q_StopChecking = true;
         break;
      case C_SyvUtil::eI_TO_BE_UPDATED:
         e_InitialState = C_SyvUtil::eI_TO_BE_UPDATED;
         break;
      case C_SyvUtil::eI_APPLICATION_MATCH:
         if (e_InitialState != C_SyvUtil::eI_TO_BE_UPDATED)
         {
            e_InitialState = C_SyvUtil::eI_APPLICATION_MATCH;
         }
         break;
      case C_SyvUtil::eI_UPDATE_DISABLED:
         if ((e_InitialState != C_SyvUtil::eI_TO_BE_UPDATED) && (e_InitialState != C_SyvUtil::eI_APPLICATION_MATCH))
         {
            e_InitialState = C_SyvUtil::eI_UPDATE_DISABLED;
         }
         break;
      case C_SyvUtil::eI_UNKNOWN:
      default:
         e_InitialState = C_SyvUtil::eI_UNKNOWN;
         q_StopChecking = true;
         break;
      }
   }

   return e_InitialState;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get sub node count

   \return
   Sub node count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_GiSvNodeData::GetSubNodeCount() const
{
   return this->mc_SubNodes.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get initial state using node index

   \param[in]  ou32_NodeIndex    Node index

   \return
   Initial state for node index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUtil::E_NodeUpdateInitialStatus C_GiSvNodeData::GetInitialStateByNodeIndex(const uint32 ou32_NodeIndex) const
{
   C_SyvUtil::E_NodeUpdateInitialStatus e_Retval = C_SyvUtil::eI_UNKNOWN;
   const uint32 u32_SubDeviceIndex = this->m_GetCorrespondingSubNodeIndex(ou32_NodeIndex);
   if (u32_SubDeviceIndex < this->mc_SubNodes.size())
   {
      e_Retval = this->mc_SubNodes[u32_SubDeviceIndex].GetInitialStatus();
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get STW device info by node index

   \param[in]  ou32_NodeIndex    Node index

   \return
   STW device info by node index
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeData::GetSTWDeviceInfoByNodeIndex(const uint32 ou32_NodeIndex) const
{
   bool q_Retval = false;
   const uint32 u32_SubDeviceIndex = this->m_GetCorrespondingSubNodeIndex(ou32_NodeIndex);

   if (u32_SubDeviceIndex < this->mc_SubNodes.size())
   {
      q_Retval = this->mc_SubNodes[u32_SubDeviceIndex].IsStwDevice();
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get sub node by node index

   \param[in]  ou32_NodeIndex    Node index

   \return
   Corresponding sub node by node index
*/
//----------------------------------------------------------------------------------------------------------------------
const C_GiSvSubNodeData * C_GiSvNodeData::GetSubNodeByNodeIndex(const uint32 ou32_NodeIndex) const
{
   const C_GiSvSubNodeData * pc_Retval = NULL;
   const uint32 u32_SubDeviceIndex = this->m_GetCorrespondingSubNodeIndex(ou32_NodeIndex);

   if (u32_SubDeviceIndex < this->mc_SubNodes.size())
   {
      pc_Retval = &this->mc_SubNodes[u32_SubDeviceIndex];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get corresponding sub node index

   Return index in mc_SubNodes corresponding to given node index.

   \param[in]  ou32_NodeIndex    Node index

   \return
   corresponding sub node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_GiSvNodeData::m_GetCorrespondingSubNodeIndex(const uint32 ou32_NodeIndex) const
{
   uint32 u32_SubNodeIndex = 0;

   if (this->mc_SubNodes.size() > 1)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
      if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
      {
         if (pc_Node->u32_SubDeviceIndex < this->mc_SubNodes.size())
         {
            u32_SubNodeIndex = pc_Node->u32_SubDeviceIndex;
         }
      }
   }

   return u32_SubNodeIndex;
}
