//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class to hold sub-node specific for all sub-nodes grouped together in a C_GiSvNodeSyvUpdate.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "TglUtils.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiUtil.hpp"
#include "C_GiSvSubNodeData.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvSubNodeData::C_GiSvSubNodeData(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex) :
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_NodeIndex(ou32_NodeIndex),
   mq_UpdateInProgress(false),
   mq_UpdateFailed(false),
   mq_UpdateSuccess(false),
   mq_ValidStatus(false),
   mq_Discarded(false),
   me_InitialStatus(C_SyvUtil::eI_UNKNOWN),
   me_UpdateStatus(C_SyvUtil::eU_UNKNOWN),
   mu32_FailedApplicationIndex(0),
   mq_AnyDatablockFound(false),
   mq_ConnectStatesSet(false),
   mq_UpdateStatesSet(false)
{
   this->m_CheckThirdParty();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvSubNodeData::~C_GiSvSubNodeData(void) noexcept
{
   if (this->mc_DeviceInfo.pc_StwDevice != NULL)
   {
      delete mc_DeviceInfo.pc_StwDevice;
   }
   if (this->mc_DeviceInfo.pc_OpenSydeDevice != NULL)
   {
      delete mc_DeviceInfo.pc_OpenSydeDevice;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connection change

   \param[in]  oq_Active   Flag if connected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::SetConnected(const bool oq_Active)
{
   this->m_CheckThirdParty();
   //Reset status
   if (oq_Active == false)
   {
      if (this->me_InitialStatus != C_SyvUtil::eI_UPDATE_DISABLED)
      {
         this->me_InitialStatus = C_SyvUtil::eI_UNKNOWN;
      }
      if (this->me_UpdateStatus != C_SyvUtil::eU_UPDATE_DISABLED)
      {
         this->me_UpdateStatus = C_SyvUtil::eU_UNKNOWN;
      }
      //Discard info
      this->mc_HexFileInfos.clear();
      this->mc_HexAppInfoAmbiguous.clear();
      this->mc_ParamFileInfos.clear();
      this->mc_FileInfos.clear();
      this->mc_PemFileInfo = "";
      if (this->mc_DeviceInfo.pc_StwDevice != NULL)
      {
         delete (mc_DeviceInfo.pc_StwDevice);
         this->mc_DeviceInfo.pc_StwDevice = NULL;
      }
      if (this->mc_DeviceInfo.pc_OpenSydeDevice != NULL)
      {
         delete (mc_DeviceInfo.pc_OpenSydeDevice);
         this->mc_DeviceInfo.pc_OpenSydeDevice = NULL;
      }

      this->mq_ConnectStatesSet = false;
      this->mq_UpdateStatesSet = false;
      this->mq_UpdateFailed = false;
   }

   //Always reset known status information flag
   this->mq_ValidStatus = false;
   this->mq_UpdateSuccess = false;
   this->mq_Discarded = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal if update in progress

   \param[in]  oq_Active   Flag if update in progress
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::SetUpdating(const bool oq_Active)
{
   this->mq_UpdateInProgress = oq_Active;

   if ((this->CheckUpdateDisabledState() == true) && (this->me_InitialStatus != C_SyvUtil::eI_ERROR))
   {
      //Update status
      if ((this->me_UpdateStatus == C_SyvUtil::eU_UPDATE_SUCCESS) && (this->CheckAlwaysUpdate() == false))
      {
         //Set last updated node to success
         if (oq_Active == false)
         {
            this->me_InitialStatus = C_SyvUtil::eI_APPLICATION_MATCH;
         }
         this->mq_Discarded = false;
         this->me_UpdateStatus = C_SyvUtil::eU_UP_TO_DATE;
      }
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
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                                const uint32_t ou32_FailedApplicationIndex)
{
   if (oq_Active == true)
   {
      this->mq_UpdateFailed = false;
      this->mq_UpdateSuccess = false;
      this->me_UpdateStatus = C_SyvUtil::eU_UPDATING;
   }
   else
   {
      switch (this->me_UpdateStatus)
      {
      case C_SyvUtil::eU_UNKNOWN:
         this->me_UpdateStatus = C_SyvUtil::eU_WAITING;
         break;
      case C_SyvUtil::eU_UPDATING:
         this->mq_UpdateSuccess = true;
         if (this->CheckAlwaysUpdate() == false)
         {
            //If user aborted we assume the application has to be updated, else the application should be finished
            if (oq_Aborted == false)
            {
               this->mq_Discarded = false;
               this->me_InitialStatus = C_SyvUtil::eI_APPLICATION_MATCH;
               this->me_UpdateStatus = C_SyvUtil::eU_UPDATE_SUCCESS;
            }
            else
            {
               this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
               this->me_UpdateStatus = C_SyvUtil::eU_UNKNOWN;
               this->mq_UpdateFailed = true;
               this->mu32_FailedApplicationIndex = ou32_FailedApplicationIndex;
            }
         }
         else
         {
            //For file based devices we never change the status
            this->me_UpdateStatus = C_SyvUtil::eU_UNKNOWN;
         }
         break;
      case C_SyvUtil::eU_WAITING:
      case C_SyvUtil::eU_UP_TO_DATE:
      case C_SyvUtil::eU_UPDATE_SUCCESS:
      case C_SyvUtil::eU_UPDATE_DISABLED:
      default:
         //No change
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply error state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::SetErrorState(void)
{
   //Validate current status
   this->mq_ValidStatus = true;
   this->me_InitialStatus = C_SyvUtil::eI_ERROR;

   if (this->mq_UpdateInProgress == true)
   {
      this->mq_UpdateFailed = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update package status

   \param[in]  orc_DeviceApplicationInfos    Device application information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::UpdateInitialPackageStatus(const C_SyvUpDeviceInfo & orc_DeviceApplicationInfos)
{
   //Validate current status
   this->mq_Discarded = false;
   this->mq_ValidStatus = true;

   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   //Clean up current info
   this->mc_HexFileInfos.clear();
   this->mc_HexAppInfoAmbiguous.clear();
   this->mc_ParamFileInfos.clear();
   this->mc_FileInfos.clear();
   this->mc_PemFileInfo = "";
   if (this->mc_DeviceInfo.pc_StwDevice != NULL)
   {
      delete (mc_DeviceInfo.pc_StwDevice);
      this->mc_DeviceInfo.pc_StwDevice = NULL;
   }
   if (this->mc_DeviceInfo.pc_OpenSydeDevice != NULL)
   {
      delete (mc_DeviceInfo.pc_OpenSydeDevice);
      this->mc_DeviceInfo.pc_OpenSydeDevice = NULL;
   }

   //COPY current info
   if (orc_DeviceApplicationInfos.pc_OpenSydeDevice != NULL)
   {
      this->mc_DeviceInfo.pc_OpenSydeDevice =
         new C_OscSuSequences::C_OsyDeviceInformation(*orc_DeviceApplicationInfos.pc_OpenSydeDevice);
   }
   else if (orc_DeviceApplicationInfos.pc_StwDevice != NULL)
   {
      this->mc_DeviceInfo.pc_StwDevice =
         new C_OscSuSequences::C_XflDeviceInformation(*orc_DeviceApplicationInfos.pc_StwDevice);
   }
   else
   {
      //Unexpected
   }

   if (this->CheckUpdateDisabledState() == false)
   {
      this->me_InitialStatus = C_SyvUtil::eI_UNKNOWN;
      if (pc_View != NULL)
      {
         const C_OscNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
         const C_OscViewNodeUpdate * const pc_UpdateInformation =
            pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);
         if ((pc_UpdateInformation != NULL) && (pc_Node != NULL))

         {
            this->m_InitPackageData(*pc_Node, *pc_UpdateInformation);

            this->m_InitStatusFromPackage(orc_DeviceApplicationInfos, *pc_Node);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if node should be in connected but update disabled state

   \retval   True    Update disabled check result
   \retval   False   Update not disabled check result
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::CheckUpdateDisabledState(void) const
{
   bool q_Retval = this->HasNodeAnAvailableFlashloader();

   if (q_Retval == true)
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
      if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
      {
         if (pc_Node->IsAnyUpdateAvailable() == false)
         {
            //Update via all interfaces disabled
            q_Retval = true;
         }
         else
         {
            if (pc_Node->c_Applications.size() > 0UL)
            {
               q_Retval = false;
            }
            else
            {
               tgl_assert(pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size());
               if (pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].q_FlashloaderStwCan == true)
               {
                  //STW flashloader with no data block
                  q_Retval = true;
               }
               else
               {
                  const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
                  if (pc_View != NULL)
                  {
                     const C_OscViewNodeUpdate * const pc_UpdateInfo =
                        pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);
                     if (pc_UpdateInfo != NULL)
                     {
                        if ((pc_UpdateInfo->GetParamInfos().size() == 0UL) &&
                            (pc_UpdateInfo->GetPaths(C_OscViewNodeUpdate::eFTP_FILE_BASED).size() == 0UL))
                        {
                           //No file associated
                           q_Retval = true;
                        }
                        else
                        {
                           //Any file associated
                           q_Retval = false;
                        }
                     }
                     else
                     {
                        //Unexpected
                        q_Retval = false;
                     }
                  }
                  else
                  {
                     //Unexpected
                     q_Retval = false;
                  }
               }
            }
         }
      }
      else
      {
         //Unexpected
         q_Retval = false;
      }
   }
   else
   {
      //Not flashloader to communicate with
      q_Retval = true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if device has a flashloader available to communicate with

   \return
   True  STW device
   False Third party
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::HasNodeAnAvailableFlashloader(void) const
{
   const bool q_Retval = C_PuiSdHandler::h_GetInstance()->HasNodeAnAvailableFlashloader(this->mu32_NodeIndex);

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if this node always needs an update

   \retval true  Node always requires update
   \retval false Node might not need update
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::CheckAlwaysUpdate(void) const
{
   bool q_Retval;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   const C_OscNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if ((pc_View != NULL) && (pc_Node != NULL) &&
       (pc_Node->pc_DeviceDefinition != NULL) &&
       (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()))
   {
      const C_OscViewNodeUpdate * const pc_UpdateInformation = pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);
      if (((pc_UpdateInformation->GetParamInfos().size() == 0UL) &&
           (pc_UpdateInformation->GetPaths(C_OscViewNodeUpdate::eFTP_FILE_BASED).size() == 0UL)) &&
          (pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].q_FlashloaderOpenSydeIsFileBased ==
           false))
      {
         q_Retval = false;
      }
      else
      {
         q_Retval = true;
      }
   }
   else
   {
      //Error
      q_Retval = true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the connect state of all nodes

   \param[in]       orc_NodeStates             Node connect states
   \param[in]       orc_PreconditionErrors     All node precondition error flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::SetNodeConnectStates(const C_OscSuSequencesNodeConnectStates & orc_NodeStates,
                                             const C_GiSvSubNodeDataPreconditionErrors & orc_PreconditionErrors)
{
   this->mc_ConnectStates = orc_NodeStates;
   this->mc_PreconditionErrors = orc_PreconditionErrors;
   this->mq_ConnectStatesSet = true;
   // When setting the connect state, the update states can not be actual anymore
   this->mq_UpdateStatesSet = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the connect state of all nodes

   \param[in]       orc_NodeStates     Node connect states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::SetNodeUpdateStates(const C_OscSuSequencesNodeUpdateStates & orc_NodeStates)
{
   this->mc_UpdateStates = orc_NodeStates;
   this->mq_UpdateStatesSet = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy initial status

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::CopyInitialStatus(C_GiSvSubNodeData & orc_NodeData) const
{
   orc_NodeData.me_InitialStatus = this->me_InitialStatus;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy update and connect status and states

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::CopyUpdateStatus(C_GiSvSubNodeData & orc_NodeData) const
{
   orc_NodeData.me_UpdateStatus = this->me_UpdateStatus;
   orc_NodeData.mc_ConnectStates = this->mc_ConnectStates;
   orc_NodeData.mc_UpdateStates = this->mc_UpdateStates;
   orc_NodeData.mc_PreconditionErrors = this->mc_PreconditionErrors;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy STW device info

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::CopyStwDeviceInfo(C_GiSvSubNodeData & orc_NodeData) const
{
   // We need a real copy of the element
   stw::opensyde_core::C_OscSuSequences::C_XflDeviceInformation * pc_Copy = NULL;

   if (this->mc_DeviceInfo.pc_StwDevice != NULL)
   {
      pc_Copy = new stw::opensyde_core::C_OscSuSequences::C_XflDeviceInformation();
      *pc_Copy = *this->mc_DeviceInfo.pc_StwDevice;
   }

   orc_NodeData.mc_DeviceInfo.pc_StwDevice = pc_Copy;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy OSY device info

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::CopyOpenSydeDeviceInfo(C_GiSvSubNodeData & orc_NodeData) const
{
   // We need a real copy of the element
   stw::opensyde_core::C_OscSuSequences::C_OsyDeviceInformation * pc_Copy = NULL;

   if (this->mc_DeviceInfo.pc_OpenSydeDevice != NULL)
   {
      pc_Copy = new stw::opensyde_core::C_OscSuSequences::C_OsyDeviceInformation();
      *pc_Copy = *this->mc_DeviceInfo.pc_OpenSydeDevice;
   }

   orc_NodeData.mc_DeviceInfo.pc_OpenSydeDevice = pc_Copy;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy discarded status

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::CopyDiscardedStatus(C_GiSvSubNodeData & orc_NodeData) const
{
   orc_NodeData.mq_Discarded = this->mq_Discarded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is device info discarded

   \return
   Device info discarded status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::IsDeviceInfoDiscarded(void) const
{
   return this->mq_Discarded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is there any hex file information

   \return
   true  Hex file info present
   false Hex file info not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::IsThereAnyHexFileInformation() const
{
   return (this->mc_HexFileInfos.size() > 0UL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Discard info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::DiscardInfo()
{
   if (this->IsThereAnyHexFileInformation())
   {
      this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
      this->me_UpdateStatus = C_SyvUtil::eU_WAITING;
      this->mq_UpdateFailed = false;
      this->mq_Discarded = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the node index

   \return
   Node index of node or sub node
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_GiSvSubNodeData::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data block found status

   \return
   Data block found status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::GetDataBlockFoundStatus(void) const
{
   return this->mq_AnyDatablockFound;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get update failed status

   \return
   Update failed status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::GetUpdateFailedStatus() const
{
   return this->mq_UpdateFailed;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get update success status

   \return
   Update success status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::GetUpdateSuccessStatus() const
{
   return this->mq_UpdateSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get valid status

   \return
   Valid status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::GetValidStatus() const
{
   return this->mq_ValidStatus;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get initial status

   \return
   Initial status
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUtil::E_NodeUpdateInitialStatus C_GiSvSubNodeData::GetInitialStatus() const
{
   return this->me_InitialStatus;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get update status

   \return
   Update status
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUtil::E_NodeUpdateStatus C_GiSvSubNodeData::GetUpdateStatus() const
{
   return this->me_UpdateStatus;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get failed application index

   \return
   Failed application index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_GiSvSubNodeData::GetFailedApplicationIndex() const
{
   return this->mu32_FailedApplicationIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get hex file info

   \param[in]  ou32_ApplicationIndex   Application index

   \return
   Hex file info
*/
//----------------------------------------------------------------------------------------------------------------------
const stw::diag_lib::C_XFLECUInformation * C_GiSvSubNodeData::GetHexFileInfo(const uint32_t ou32_ApplicationIndex) const
{
   const stw::diag_lib::C_XFLECUInformation * pc_Retval = NULL;

   if (ou32_ApplicationIndex < this->mc_HexFileInfos.size())
   {
      pc_Retval = &this->mc_HexFileInfos[ou32_ApplicationIndex];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get hex file infos count

   \return
   Hex file infos count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_GiSvSubNodeData::GetHexFileInfosCount() const
{
   return this->mc_HexFileInfos.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get param file infos count

   \return
   Param file infos count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_GiSvSubNodeData::GetParamFileInfosCount() const
{
   return this->mc_ParamFileInfos.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get file infos count

   \return
   File infos count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_GiSvSubNodeData::GetFileInfosCount() const
{
   return this->mc_FileInfos.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the flag if a PEM file is set

   \retval   true   A PEM file is set
   \retval   false  No PEM file is set
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::IsPemFileInfoSet(void) const
{
   return (this->mc_PemFileInfo == "") ? false : true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get hex app info ambiguous array size

   \return
   Hex app info ambiguous size
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_GiSvSubNodeData::GetHexAppInfoAmbiguousSize() const
{
   return this->mc_HexAppInfoAmbiguous.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get hex app info ambiguous

   \param[in]  ou32_ApplicationIndex   Application index

   \return
   Hex app info ambiguous info
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::GetHexAppInfoAmbiguous(const uint32_t ou32_ApplicationIndex) const
{
   bool q_Retval = false;

   if (ou32_ApplicationIndex < this->mc_HexAppInfoAmbiguous.size())
   {
      q_Retval = this->mc_HexAppInfoAmbiguous[ou32_ApplicationIndex];
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get device info

   \return
   Device info
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpDeviceInfo C_GiSvSubNodeData::GetDeviceInfo() const
{
   return this->mc_DeviceInfo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the connect state of the sub node

   \return
   Node connect states reference
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscSuSequencesNodeConnectStates & C_GiSvSubNodeData::GetNodeConnectStates(void) const
{
   return this->mc_ConnectStates;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the connect precondition error states of the sub node

   \return
   Node connect states reference
*/
//----------------------------------------------------------------------------------------------------------------------
const C_GiSvSubNodeData::C_GiSvSubNodeDataPreconditionErrors & C_GiSvSubNodeData::GetNodeConnectPreconditionErrors(void)
const
{
   return this->mc_PreconditionErrors;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the connect state of the sub node

   Update states are set when the update process is finished. Check with IsNodeUpdateStatesSet first, if states
   are already usable

   \return
   Node update states reference
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscSuSequencesNodeUpdateStates & C_GiSvSubNodeData::GetNodeUpdateStates(void) const
{
   return this->mc_UpdateStates;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the flag if connect state of all sub nodes was already set for the current process

   \retval   true   node update states are set and usable
   \retval   false  node update states are not set and usable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::IsNodeConnectStatesSet(void) const
{
   return this->mq_ConnectStatesSet;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the flag if update state of all sub nodes was already set for the current process

   \retval   true   node update states are set and usable
   \retval   false  node update states are not set and usable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvSubNodeData::IsNodeUpdateStatesSet(void) const
{
   return this->mq_UpdateStatesSet;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check third party node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::m_CheckThirdParty(void)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OscDeviceDefinition * const pc_Device = pc_Node->pc_DeviceDefinition;
      const uint32_t u32_SubDeviceIndex = pc_Node->u32_SubDeviceIndex;
      tgl_assert(pc_Device != NULL);
      if (pc_Device != NULL)
      {
         tgl_assert(u32_SubDeviceIndex < pc_Device->c_SubDevices.size());
         if (u32_SubDeviceIndex < pc_Device->c_SubDevices.size())
         {
            if (((pc_Device->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderStwCan == false) &&
                 (pc_Device->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeCan == false)) &&
                (pc_Device->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeEthernet == false))
            {
               this->me_InitialStatus = C_SyvUtil::eI_UPDATE_DISABLED;
               this->me_UpdateStatus = C_SyvUtil::eU_UPDATE_DISABLED;
            }
            //Check if update is necessary
            else if ((pc_Node->c_Applications.size() > 0) ||
                     (this->CheckAlwaysUpdate() == true))
            {
               this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
            }
            else
            {
               this->me_InitialStatus = C_SyvUtil::eI_UPDATE_DISABLED;
               this->me_UpdateStatus = C_SyvUtil::eU_UPDATE_DISABLED;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init package data

   \param[in]  orc_Node                Node
   \param[in]  orc_UpdateInformation   Update information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::m_InitPackageData(const C_OscNode & orc_Node, const C_OscViewNodeUpdate & orc_UpdateInformation)
{
   if (orc_Node.c_Applications.size() ==
       orc_UpdateInformation.GetPaths(C_OscViewNodeUpdate::eFTP_DATA_BLOCK).size())
   {
      if (orc_Node.c_Applications.size() > 0UL)
      {
         //Register any datablocks found
         this->mq_AnyDatablockFound = true;
      }
      tgl_assert(orc_Node.u32_SubDeviceIndex < orc_Node.pc_DeviceDefinition->c_SubDevices.size());
      if (orc_Node.pc_DeviceDefinition->c_SubDevices[orc_Node.u32_SubDeviceIndex].q_FlashloaderOpenSydeIsFileBased
          ==
          false)
      {
         uint32_t u32_ItApplication;
         uint32_t u32_ItApplicationPath = 0U;
         std::vector<QString> c_FinalApplicationPaths;
         const std::vector<C_SclString> & rc_ApplicationPaths = orc_UpdateInformation.GetPaths(
            C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         c_FinalApplicationPaths.reserve(orc_Node.c_Applications.size());

         for (u32_ItApplication = 0U; u32_ItApplication < orc_Node.c_Applications.size();
              ++u32_ItApplication)
         {
            const C_OscNodeApplication & rc_Application = orc_Node.c_Applications[u32_ItApplication];

            // The HALC NVM param files files will be handled with the other param files
            if (rc_Application.e_Type != C_OscNodeApplication::ePARAMETER_SET_HALC)
            {
               const QString & rc_ViewApplicationPath = rc_ApplicationPaths[u32_ItApplicationPath].c_str();
               if (rc_ViewApplicationPath.compare("") == 0)
               {
                  // In not NVM HALC case, only 1 path for each datablock exists
                  c_FinalApplicationPaths.push_back(
                     C_PuiUtil::h_GetResolvedAbsPathFromDbProject(rc_Application.c_ProjectPath.c_str(),
                                                                  rc_Application.c_ResultPaths[0].c_str()));
               }
               else
               {
                  c_FinalApplicationPaths.push_back(C_PuiUtil::h_GetResolvedAbsPathFromProject(
                                                       rc_ViewApplicationPath));
               }

               // The application paths match only 1:1 in case of an application which is not the type NVM_HALC
               ++u32_ItApplicationPath;
            }
         }

         //Read all applications
         m_InitPackageDataForApplicationsFromFiles(c_FinalApplicationPaths);
      }
   }
   m_InitPackageDataForOtherFiles(orc_UpdateInformation);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init package data for applications from files

   \param[in]  orc_FinalFilePaths   Final file paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::m_InitPackageDataForApplicationsFromFiles(const std::vector<QString> & orc_FinalFilePaths)
{
   this->mc_HexFileInfos.reserve(orc_FinalFilePaths.size());
   this->mc_HexAppInfoAmbiguous.reserve(orc_FinalFilePaths.size());
   for (uint32_t u32_ItFile = 0; u32_ItFile < orc_FinalFilePaths.size(); ++u32_ItFile)
   {
      const stw::scl::C_SclString c_Path = orc_FinalFilePaths[u32_ItFile].toStdString().c_str();
      // c_Path is already absolute and placeholder variables got resolved!
      C_OsyHexFile c_HexFile;
      uint32_t u32_Result;

      u32_Result = c_HexFile.LoadFromFile(c_Path.c_str());
      if (u32_Result == stw::hex_file::NO_ERR)
      {
         stw::diag_lib::C_XFLECUInformation c_FileApplicationInfo;
         const int32_t s32_Result = c_HexFile.ScanApplicationInformationBlockFromHexFile(
            c_FileApplicationInfo);
         if ((s32_Result == C_NO_ERR) || (s32_Result == C_WARN))
         {
            this->mc_HexFileInfos.push_back(c_FileApplicationInfo);
            this->mc_HexAppInfoAmbiguous.push_back(s32_Result == C_WARN);
         }
      }
      else
      {
         const stw::scl::C_SclString c_Text = "Could not open HEX file \"" +
                                              c_Path + "\" Details: " +
                                              c_HexFile.ErrorCodeToErrorText(u32_Result);
         osc_write_log_error("Preparing Flashloader Information", c_Text);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init package data for other files

   \param[in]  orc_UpdateInformation   Update information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::m_InitPackageDataForOtherFiles(const C_OscViewNodeUpdate & orc_UpdateInformation)
{
   const std::vector<C_OscViewNodeUpdateParamInfo> & rc_ParamInfo = orc_UpdateInformation.GetParamInfos();
   const std::vector<C_SclString> & rc_Files =
      orc_UpdateInformation.GetPaths(C_OscViewNodeUpdate::eFTP_FILE_BASED);

   //Handle param files
   this->mc_ParamFileInfos.reserve(rc_ParamInfo.size());
   for (uint32_t u32_ItParamFile = 0; u32_ItParamFile < rc_ParamInfo.size(); ++u32_ItParamFile)
   {
      const C_OscViewNodeUpdateParamInfo & rc_CurParamInfo = rc_ParamInfo[u32_ItParamFile];
      this->mc_ParamFileInfos.emplace_back(rc_CurParamInfo.GetPath().c_str());
   }

   //Handle files
   this->mc_FileInfos.reserve(rc_Files.size());
   for (uint32_t u32_ItFile = 0; u32_ItFile < rc_Files.size(); ++u32_ItFile)
   {
      this->mc_FileInfos.emplace_back(rc_Files[u32_ItFile].c_str());
   }

   // Handle PEM file
   this->mc_PemFileInfo = orc_UpdateInformation.GetPemFilePath().c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init status from package

   \param[in]  orc_DeviceApplicationInfos    Device application infos
   \param[in]  orc_Node                      Node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvSubNodeData::m_InitStatusFromPackage(
   const stw::opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos, const C_OscNode & orc_Node)
{
   if ((orc_Node.c_Properties.e_FlashLoader == C_OscNodeProperties::eFL_OPEN_SYDE) &&
       (orc_DeviceApplicationInfos.pc_OpenSydeDevice != NULL))
   {
      //Always update if there is at least one parameter file
      if (this->CheckAlwaysUpdate())
      {
         this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
         this->me_UpdateStatus = C_SyvUtil::eU_WAITING;
      }
      else
      {
         bool q_AllApplicationsMatch = true;
         for (uint32_t u32_ItFile = 0; u32_ItFile < this->mc_HexFileInfos.size(); ++u32_ItFile)
         {
            bool q_Found = false;

            // only search if application information is not ambiguous
            if (this->mc_HexAppInfoAmbiguous[u32_ItFile] == false)
            {
               const stw::diag_lib::C_XFLECUInformation & rc_FileInfo =
                  this->mc_HexFileInfos[u32_ItFile];

               //Skip first application because this is the flashloader (OSY ONLY!)
               for (uint32_t u32_ItApplication = 1;
                    u32_ItApplication < orc_DeviceApplicationInfos.pc_OpenSydeDevice->c_Applications.size();
                    ++u32_ItApplication)
               {
                  const C_OscProtocolDriverOsy::C_FlashBlockInfo & rc_OsyDeviceInfo =
                     orc_DeviceApplicationInfos.pc_OpenSydeDevice->c_Applications[u32_ItApplication];
                  //Search for match
                  if ((rc_OsyDeviceInfo.c_ApplicationName.Trim() == rc_FileInfo.GetProjectName().Trim()) &&
                      (rc_OsyDeviceInfo.c_ApplicationVersion.Trim() == rc_FileInfo.GetProjectVersion().Trim()) &&
                      (rc_OsyDeviceInfo.c_BuildDate.Trim() == rc_FileInfo.GetDate().Trim()) &&
                      (rc_OsyDeviceInfo.c_BuildTime.Trim() == rc_FileInfo.GetTime().Trim()))

                  {
                     if (rc_OsyDeviceInfo.u8_SignatureValid == 0U) //0 == valid
                     {
                        q_Found = true;
                        break;
                     }
                  }
               }
            }

            if (q_Found == false)
            {
               q_AllApplicationsMatch = false;
               break;
            }
         }
         if (q_AllApplicationsMatch == true)
         {
            this->me_InitialStatus = C_SyvUtil::eI_APPLICATION_MATCH;
            this->me_UpdateStatus = C_SyvUtil::eU_UP_TO_DATE;
         }
         else
         {
            this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
            this->me_UpdateStatus = C_SyvUtil::eU_WAITING;
         }
      }
   }
   else if ((orc_Node.c_Properties.e_FlashLoader == C_OscNodeProperties::eFL_STW) &&
            (orc_DeviceApplicationInfos.pc_StwDevice != NULL))
   {
      bool q_AllApplicationsMatch = true;
      for (uint32_t u32_ItFile = 0; u32_ItFile < this->mc_HexFileInfos.size(); ++u32_ItFile)
      {
         bool q_Found = false;
         // only search if application information is not ambiguous
         if (this->mc_HexAppInfoAmbiguous[u32_ItFile] == false)
         {
            const stw::diag_lib::C_XFLECUInformation & rc_FileInfo =
               this->mc_HexFileInfos[u32_ItFile];
            for (int32_t s32_ItDeviceInfoBlock = 0;
                 s32_ItDeviceInfoBlock <
                 orc_DeviceApplicationInfos.pc_StwDevice->c_BasicInformation.c_DeviceInfoBlocks.GetLength();
                 ++s32_ItDeviceInfoBlock)
            {
               const stw::diag_lib::C_XFLECUInformation & rc_StwDeviceInfo =
                  orc_DeviceApplicationInfos.pc_StwDevice->c_BasicInformation.c_DeviceInfoBlocks[s32_ItDeviceInfoBlock];
               //Search for match
               if ((rc_StwDeviceInfo.GetProjectName().Trim() == rc_FileInfo.GetProjectName().Trim()) &&
                   (rc_StwDeviceInfo.GetProjectVersion().Trim() == rc_FileInfo.GetProjectVersion().Trim()) &&
                   (rc_StwDeviceInfo.GetDate().Trim() == rc_FileInfo.GetDate().Trim()) &&
                   (rc_StwDeviceInfo.GetTime().Trim() == rc_FileInfo.GetTime().Trim()))
               {
                  q_Found = true;
                  break;
               }
            }
         }
         if (q_Found == false)
         {
            q_AllApplicationsMatch = false;
            break;
         }
      }
      if (q_AllApplicationsMatch == true)
      {
         this->me_InitialStatus = C_SyvUtil::eI_APPLICATION_MATCH;
         this->me_UpdateStatus = C_SyvUtil::eU_UP_TO_DATE;
      }
      else
      {
         this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
         this->me_UpdateStatus = C_SyvUtil::eU_WAITING;
      }
   }
   else
   {
      //Should not happen
      this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
      this->me_UpdateStatus = C_SyvUtil::eU_WAITING;
   }
}
