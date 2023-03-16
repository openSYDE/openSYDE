//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper class for all currently known file size information

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_UsHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvUpFileSizeInformation.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;

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
C_SyvUpFileSizeInformation::C_SyvUpFileSizeInformation(void) :
   mu64_OverallFilesSize(0ULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset all stored data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::Reset(void)
{
   this->mu64_OverallFilesSize = 0ULL;
   this->mc_FileSizesByte.clear();
   this->mc_ParamFileSizesByte.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get estimated wait time in seconds

   \param[in,out]  opq_Ok  Optional flag to see if estimated wait time caluclation was successful)

   \return
   Estimated wait time in seconds (only valid if opq_Ok was set to true)
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_SyvUpFileSizeInformation::GetEstimatedTimeSeconds(bool * const opq_Ok) const
{
   uint64_t u64_Retval = 0ULL;

   if (opq_Ok != NULL)
   {
      *opq_Ok = true;
   }
   //Sum up all times for each node
   for (QMap<uint32_t, float64_t>::const_iterator c_ItNode = this->mc_FileSizesByteMapPerNode.begin();
        c_ItNode != this->mc_FileSizesByteMapPerNode.end(); ++c_ItNode)
   {
      const QMap<uint32_t, float64_t>::const_iterator c_ItDataRate = this->mc_BytesPerMsMapPerNode.find(c_ItNode.key());
      //See if that node has a known data rate
      if ((c_ItDataRate != this->mc_BytesPerMsMapPerNode.end()) && (c_ItDataRate.value() > 0.0))
      {
         //file size [byte] / Last data rate [byte/ms] -> divide by 1000 to get seconds
         const float64_t f64_EstimatedTimeNode = (c_ItNode.value() / 1000.0) / c_ItDataRate.value();
         u64_Retval += static_cast<uint64_t>(f64_EstimatedTimeNode);
      }
      else
      {
         //Signal failure to find node
         if (opq_Ok != NULL)
         {
            *opq_Ok = false;
         }
      }
   }
   return u64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get size of all contained files

   \return
   Size of all contained files
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_SyvUpFileSizeInformation::GetOverallFilesSize(void) const
{
   return this->mu64_OverallFilesSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reserve vector space

   \param[in]  ou32_NumNodes  Number of expected nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::ReserveSpace(const uint32_t ou32_NumNodes)
{
   this->mc_FileSizesByte.reserve(ou32_NumNodes);
   this->mc_ParamFileSizesByte.reserve(ou32_NumNodes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Append files to storage

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_OtherFiles    Other file sizes to append
   \param[in]  orc_ParamFiles    Param file sizes to append
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::AppendFiles(const uint32_t ou32_NodeIndex,
                                             const std::vector<uint64_t> & orc_OtherFiles,
                                             const std::vector<uint64_t> & orc_ParamFiles)
{
   uint64_t u64_SizeOfNewFiles = 0ULL;

   //Append
   this->mc_ParamFileSizesByte.push_back(orc_ParamFiles);
   this->mc_FileSizesByte.push_back(orc_OtherFiles);

   //Handle overall size
   for (uint32_t u32_ItFile = 0UL; u32_ItFile < orc_OtherFiles.size(); ++u32_ItFile)
   {
      u64_SizeOfNewFiles += orc_OtherFiles[u32_ItFile];
   }
   for (uint32_t u32_ItFile = 0UL; u32_ItFile < orc_ParamFiles.size(); ++u32_ItFile)
   {
      u64_SizeOfNewFiles += orc_ParamFiles[u32_ItFile];
   }
   this->mu64_OverallFilesSize += u64_SizeOfNewFiles;
   this->mc_FileSizesByteMapPerNode.insert(ou32_NodeIndex, static_cast<float64_t>(u64_SizeOfNewFiles));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of known parameter files

   \return
   Number of known parameter files
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpFileSizeInformation::GetNumParamFiles(void) const
{
   return this->mc_ParamFileSizesByte.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of known other files

   \return
   Number of known other files
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpFileSizeInformation::GetNumOtherFiles(void) const
{
   return this->mc_FileSizesByte.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of parameter files for this device

   \param[in]  ou32_DeviceOrder  Place number of device in flash order

   \return
   Number of parameter files for this device
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpFileSizeInformation::GetNumParamFilesForDevice(const uint32_t ou32_DeviceOrder) const
{
   uint32_t u32_Retval;

   if (ou32_DeviceOrder < this->mc_ParamFileSizesByte.size())
   {
      u32_Retval = this->mc_ParamFileSizesByte[ou32_DeviceOrder].size();
   }
   else
   {
      u32_Retval = 0UL;
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of other files for this device

   \param[in]  ou32_DeviceOrder  Place number of device in flash order

   \return
   Number of other files for this device
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpFileSizeInformation::GetNumOtherFilesForDevice(const uint32_t ou32_DeviceOrder) const
{
   uint32_t u32_Retval;

   if (ou32_DeviceOrder < this->mc_FileSizesByte.size())
   {
      u32_Retval = this->mc_FileSizesByte[ou32_DeviceOrder].size();
   }
   else
   {
      u32_Retval = 0UL;
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get parameter file size for this device

   \param[in]  ou32_DeviceOrder  Place number of device in flash order
   \param[in]  ou32_FileIndex    File index

   \return
   Parameter file size for this device
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_SyvUpFileSizeInformation::GetParamFileSizeForDevice(const uint32_t ou32_DeviceOrder,
                                                               const uint32_t ou32_FileIndex) const
{
   uint64_t u64_Retval;

   if (ou32_DeviceOrder < this->mc_ParamFileSizesByte.size())
   {
      const std::vector<uint64_t> & rc_Files = this->mc_ParamFileSizesByte[ou32_DeviceOrder];
      if (ou32_FileIndex < rc_Files.size())
      {
         u64_Retval = rc_Files[ou32_FileIndex];
      }
      else
      {
         u64_Retval = 0ULL;
      }
   }
   else
   {
      u64_Retval = 0ULL;
   }
   return u64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get other file size for this device

   \param[in]  ou32_DeviceOrder  Place number of device in flash order
   \param[in]  ou32_FileIndex    File index

   \return
   Other file size for this device
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_SyvUpFileSizeInformation::GetOtherFileSizeForDevice(const uint32_t ou32_DeviceOrder,
                                                               const uint32_t ou32_FileIndex) const
{
   uint64_t u64_Retval;

   if (ou32_DeviceOrder < this->mc_FileSizesByte.size())
   {
      const std::vector<uint64_t> & rc_Files = this->mc_FileSizesByte[ou32_DeviceOrder];
      if (ou32_FileIndex < rc_Files.size())
      {
         u64_Retval = rc_Files[ou32_FileIndex];
      }
      else
      {
         u64_Retval = 0ULL;
      }
   }
   else
   {
      u64_Retval = 0ULL;
   }
   return u64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update known elapsed time for node index

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou64_ElapsedTimeMs   Elapsed time in ms
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::SetElapsedTimeForNode(const uint32_t ou32_NodeIndex, const uint64_t ou64_ElapsedTimeMs)
{
   const QMap<uint32_t, float64_t>::const_iterator c_ItFiles = this->mc_FileSizesByteMapPerNode.find(
      ou32_NodeIndex);

   if ((c_ItFiles != this->mc_FileSizesByteMapPerNode.end()) && (ou64_ElapsedTimeMs > 0ULL))
   {
      const float64_t f64_Tmp = c_ItFiles.value() / static_cast<float64_t>(ou64_ElapsedTimeMs);
      this->mc_BytesPerMsMapPerNode.insert(ou32_NodeIndex, f64_Tmp);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings

   \param[in]  ou32_ViewIndex    View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::LoadUserSettings(const uint32_t ou32_ViewIndex)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   std::vector<uint8_t> c_NodeActiveFlags;
   const int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
      ou32_ViewIndex,
      c_NodeActiveFlags);

   this->mc_BytesPerMsMapPerNode.clear();
   // restore configuration of the view
   if ((pc_View != NULL) &&
       (s32_Retval == C_NO_ERR))
   {
      const C_UsSystemView c_UserView = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName().c_str());
      for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_NodeActiveFlags.size(); ++u32_ItNode)
      {
         QString c_Name;
         if (C_PuiSdHandler::h_GetInstance()->MapNodeIndexToName(u32_ItNode, c_Name) == C_NO_ERR)
         {
            uint32_t u32_Crc;
            if (C_PuiSvHandler::h_GetInstance()->CalcViewRoutingCrcIndex(ou32_ViewIndex, u32_ItNode,
                                                                         u32_Crc) == C_NO_ERR)
            {
               const C_UsSystemViewNode c_Node = c_UserView.GetSvNode(c_Name);
               const QMap<uint32_t, float64_t> & rc_UpdateDataRateHistory = c_Node.GetUpdateDataRateHistory();
               const QMap<uint32_t, float64_t>::const_iterator c_It = rc_UpdateDataRateHistory.find(u32_Crc);
               if (c_It != rc_UpdateDataRateHistory.end())
               {
                  this->mc_BytesPerMsMapPerNode.insert(u32_ItNode, c_It.value());
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings

   \param[in]  ou32_ViewIndex    View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::SaveUserSettings(const uint32_t ou32_ViewIndex) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   std::vector<uint8_t> c_NodeActiveFlags;
   const int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
      ou32_ViewIndex,
      c_NodeActiveFlags);

   // restore configuration of the view
   if ((pc_View != NULL) &&
       (s32_Retval == C_NO_ERR))
   {
      for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_NodeActiveFlags.size(); ++u32_ItNode)
      {
         if (c_NodeActiveFlags[u32_ItNode] == 1)
         {
            const QMap<uint32_t, float64_t>::const_iterator c_It = this->mc_BytesPerMsMapPerNode.find(
               u32_ItNode);
            if (c_It != this->mc_BytesPerMsMapPerNode.end())
            {
               QString c_Name;
               if (C_PuiSdHandler::h_GetInstance()->MapNodeIndexToName(u32_ItNode, c_Name) == C_NO_ERR)
               {
                  uint32_t u32_Crc;
                  if (C_PuiSvHandler::h_GetInstance()->CalcViewRoutingCrcIndex(ou32_ViewIndex, u32_ItNode,
                                                                               u32_Crc) == C_NO_ERR)
                  {
                     C_UsHandler::h_GetInstance()->AddProjSvNodeUpdateDataRate(pc_View->GetName().c_str(), c_Name,
                                                                               u32_Crc, c_It.value());
                  }
               }
            }
         }
      }
   }
}
