//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper class for all currently known file size information

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SyvUpFileSizeInformation.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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

   \param[in,out] opq_Ok Optional flag to see if estimated wait time caluclation was successful)

   \return
   Estimated wait time in seconds (only valid if opq_Ok was set to true)
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_SyvUpFileSizeInformation::GetEstimatedTimeS(bool * const opq_Ok) const
{
   uint64 u64_Retval = 0ULL;

   if (opq_Ok != NULL)
   {
      *opq_Ok = true;
   }
   //Sum up all times for each node
   for (QMap<uint32, float64>::const_iterator c_ItNode = this->mc_FileSizesByteMapPerNode.begin();
        c_ItNode != this->mc_FileSizesByteMapPerNode.end(); ++c_ItNode)
   {
      const QMap<uint32, float64>::const_iterator c_ItDataRate = this->mc_BytesPerMsMapPerNode.find(c_ItNode.key());
      //See if that node has a known data rate
      if ((c_ItDataRate != this->mc_BytesPerMsMapPerNode.end()) && (c_ItDataRate.value() > 0.0))
      {
         //file size [byte] / Last data rate [byte/ms] -> divide by 1000 to get seconds
         const float64 f64_EstimatedTimeNode = (c_ItNode.value() / 1000.0) / c_ItDataRate.value();
         u64_Retval += static_cast<uint64>(f64_EstimatedTimeNode);
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
uint64 C_SyvUpFileSizeInformation::GetOverallFilesSize(void) const
{
   return this->mu64_OverallFilesSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reserve vector space

   \param[in] ou32_NumNodes Number of expected nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::ReserveSpace(const uint32 ou32_NumNodes)
{
   this->mc_FileSizesByte.reserve(ou32_NumNodes);
   this->mc_ParamFileSizesByte.reserve(ou32_NumNodes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Append files to storage

   \param[in] ou32_NodeIndex Node index
   \param[in] orc_OtherFiles Other file sizes to append
   \param[in] orc_ParamFiles Param file sizes to append
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::AppendFiles(const uint32 ou32_NodeIndex, const std::vector<uint64> & orc_OtherFiles,
                                             const std::vector<uint64> & orc_ParamFiles)
{
   uint64 u64_SizeOfNewFiles = 0ULL;

   //Append
   this->mc_ParamFileSizesByte.push_back(orc_ParamFiles);
   this->mc_FileSizesByte.push_back(orc_OtherFiles);

   //Handle overall size
   for (uint32 u32_ItFile = 0UL; u32_ItFile < orc_OtherFiles.size(); ++u32_ItFile)
   {
      u64_SizeOfNewFiles += orc_OtherFiles[u32_ItFile];
   }
   for (uint32 u32_ItFile = 0UL; u32_ItFile < orc_ParamFiles.size(); ++u32_ItFile)
   {
      u64_SizeOfNewFiles += orc_ParamFiles[u32_ItFile];
   }
   this->mu64_OverallFilesSize += u64_SizeOfNewFiles;
   this->mc_FileSizesByteMapPerNode.insert(ou32_NodeIndex, static_cast<float64>(u64_SizeOfNewFiles));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of known parameter files

   \return
   Number of known parameter files
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpFileSizeInformation::GetNumParamFiles(void) const
{
   return this->mc_ParamFileSizesByte.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of known other files

   \return
   Number of known other files
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpFileSizeInformation::GetNumOtherFiles(void) const
{
   return this->mc_FileSizesByte.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of parameter files for this device

   \param[in] ou32_DeviceOrder Place number of device in flash order

   \return
   Number of parameter files for this device
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpFileSizeInformation::GetNumParamFilesForDevice(const uint32 ou32_DeviceOrder) const
{
   uint32 u32_Retval;

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

   \param[in] ou32_DeviceOrder Place number of device in flash order

   \return
   Number of other files for this device
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpFileSizeInformation::GetNumOtherFilesForDevice(const uint32 ou32_DeviceOrder) const
{
   uint32 u32_Retval;

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

   \param[in] ou32_DeviceOrder Place number of device in flash order
   \param[in] ou32_FileIndex   File index

   \return
   Parameter file size for this device
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_SyvUpFileSizeInformation::GetParamFileSizeForDevice(const uint32 ou32_DeviceOrder,
                                                             const uint32 ou32_FileIndex) const
{
   uint64 u64_Retval;

   if (ou32_DeviceOrder < this->mc_ParamFileSizesByte.size())
   {
      const std::vector<uint64> & rc_Files = this->mc_ParamFileSizesByte[ou32_DeviceOrder];
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

   \param[in] ou32_DeviceOrder Place number of device in flash order
   \param[in] ou32_FileIndex   File index

   \return
   Other file size for this device
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_SyvUpFileSizeInformation::GetOtherFileSizeForDevice(const uint32 ou32_DeviceOrder,
                                                             const uint32 ou32_FileIndex) const
{
   uint64 u64_Retval;

   if (ou32_DeviceOrder < this->mc_FileSizesByte.size())
   {
      const std::vector<uint64> & rc_Files = this->mc_FileSizesByte[ou32_DeviceOrder];
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

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou64_ElapsedTimeMs Elapsed time in ms
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::SetElapsedTimeForNode(const uint32 ou32_NodeIndex, const uint64 ou64_ElapsedTimeMs)
{
   const QMap<uint32, float64>::const_iterator c_ItFiles = this->mc_FileSizesByteMapPerNode.find(
      ou32_NodeIndex);

   if ((c_ItFiles != this->mc_FileSizesByteMapPerNode.end()) && (ou64_ElapsedTimeMs > 0ULL))
   {
      const float64 f64_Tmp = c_ItFiles.value() / static_cast<float64>(ou64_ElapsedTimeMs);
      this->mc_BytesPerMsMapPerNode.insert(ou32_NodeIndex, f64_Tmp);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get map for bytes per milli second grouped by node index

   \return
   Map for bytes per milli second grouped by node index
*/
//----------------------------------------------------------------------------------------------------------------------
const QMap<uint32, float64> & C_SyvUpFileSizeInformation::GetBytesPerMsMapPerNode(void) const
{
   return mc_BytesPerMsMapPerNode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set map for bytes per milli second grouped by node index

   \param[in] orc_Value Map for bytes per milli second grouped by node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpFileSizeInformation::SetFileSizesByteMapPerNode(const QMap<stw_types::uint32,
                                                                       stw_types::float64> & orc_Value)
{
   mc_FileSizesByteMapPerNode = orc_Value;
}
