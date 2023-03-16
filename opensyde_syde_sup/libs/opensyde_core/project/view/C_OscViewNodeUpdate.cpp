//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update data (implementation)

   Node update data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscViewNodeUpdate.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::errors;
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
C_OscViewNodeUpdate::C_OscViewNodeUpdate(void) :
   u32_NodeUpdatePosition(0U),
   mq_SkipUpdateOfPemFile(false),
   me_StateSecurity(eST_SEC_NO_CHANGE),
   me_StateDebugger(eST_DEB_NO_CHANGE)
{
   // For each type of file
   this->mc_SkipUpdateOfFiles.resize(3);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::CalcHash(uint32_t & oru32_HashValue) const
{
   const uint32_t u32_Size = static_cast<int32_t>(this->mc_DataBlockPaths.size());
   const stw::scl::C_SclString c_PemFile = this->mc_PemFilePath;

   stw::scl::C_SclChecksums::CalcCRC32(&this->me_StateDebugger, sizeof(this->me_StateDebugger), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->me_StateSecurity, sizeof(this->me_StateSecurity), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&u32_Size, sizeof(u32_Size), oru32_HashValue);
   for (uint32_t u32_It = 0; u32_It < this->mc_DataBlockPaths.size(); ++u32_It)
   {
      const C_SclString & rc_QtData = this->mc_DataBlockPaths[u32_It];
      const stw::scl::C_SclString c_Data = rc_QtData;
      stw::scl::C_SclChecksums::CalcCRC32(c_Data.c_str(), c_Data.Length(), oru32_HashValue);
   }
   for (uint32_t u32_It = 0; u32_It < this->mc_FileBasedPaths.size(); ++u32_It)
   {
      const C_SclString & rc_QtData = this->mc_FileBasedPaths[u32_It];
      const stw::scl::C_SclString c_Data = rc_QtData;
      stw::scl::C_SclChecksums::CalcCRC32(c_Data.c_str(), c_Data.Length(), oru32_HashValue);
   }
   for (uint32_t u32_It = 0; u32_It < this->mc_ParamSetPaths.size(); ++u32_It)
   {
      const C_OscViewNodeUpdateParamInfo & rc_Test = this->mc_ParamSetPaths[u32_It];
      rc_Test.CalcHash(oru32_HashValue);
   }
   for (uint32_t u32_It = 0; u32_It < this->mc_SkipUpdateOfFiles.size(); ++u32_It)
   {
      for (uint32_t u32_SubIt = 0; u32_SubIt < this->mc_SkipUpdateOfFiles[u32_It].size(); ++u32_SubIt)
      {
         const bool q_Value = this->mc_SkipUpdateOfFiles[u32_It][u32_SubIt];
         stw::scl::C_SclChecksums::CalcCRC32(&q_Value, sizeof(q_Value), oru32_HashValue);
      }
   }
   stw::scl::C_SclChecksums::CalcCRC32(&u32_NodeUpdatePosition, sizeof(u32_NodeUpdatePosition), oru32_HashValue);

   stw::scl::C_SclChecksums::CalcCRC32(c_PemFile.c_str(), c_PemFile.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mq_SkipUpdateOfPemFile, sizeof(this->mq_SkipUpdateOfPemFile),
                                       oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all parameter set paths for this node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::ClearParamPaths(void)
{
   this->mc_ParamSetPaths.clear();
   this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all paths as appropriate for the type

   \param[in]  oe_Type  Selector for structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::ClearPathsAsAppropriate(const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   const int32_t s32_Type = static_cast<int32_t>(oe_Type);

   if (oe_Type == eFTP_DATA_BLOCK)
   {
      for (uint32_t u32_It = 0UL; u32_It < this->mc_DataBlockPaths.size(); ++u32_It)
      {
         this->mc_DataBlockPaths[u32_It] = "<Add File>";
         this->mc_SkipUpdateOfFiles[s32_Type][u32_It] = false;
      }
   }
   else
   {
      this->mc_FileBasedPaths.clear();
      this->mc_SkipUpdateOfFiles[s32_Type].clear();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parameter set paths

   \return
   Current parameter set paths
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OscViewNodeUpdateParamInfo> & C_OscViewNodeUpdate::GetParamInfos(void) const
{
   return this->mc_ParamSetPaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get application paths

   \param[in]  oe_Type  Selector for structure

   \return
   Current application paths
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_SclString> & C_OscViewNodeUpdate::GetPaths(const E_GenericFileType oe_Type) const
{
   return (oe_Type == eFTP_DATA_BLOCK) ? this->mc_DataBlockPaths : this->mc_FileBasedPaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parameter set skip flags

   \return
   Current application skip flags
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<bool> & C_OscViewNodeUpdate::GetSkipUpdateOfParamInfosFlags(void) const
{
   return this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get application skip flags

   \param[in]  oe_Type  Selector for structure

   \return
   Current application skip flags
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<bool> & C_OscViewNodeUpdate::GetSkipUpdateOfPathsFlags(const E_GenericFileType oe_Type) const
{
   return this->mc_SkipUpdateOfFiles[static_cast<int32_t>(oe_Type)];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set parameter set paths

   \param[in]  orc_Value   New parameter set paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::SetParamInfos(const std::vector<C_OscViewNodeUpdateParamInfo> & orc_Value)
{
   this->mc_ParamSetPaths = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set application paths

   \param[in]  orc_Value   New application paths
   \param[in]  oe_Type     Selector for structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::SetPaths(const std::vector<C_SclString> & orc_Value, const E_GenericFileType oe_Type)
{
   if (oe_Type == eFTP_DATA_BLOCK)
   {
      this->mc_DataBlockPaths = orc_Value;
   }
   else
   {
      this->mc_FileBasedPaths = orc_Value;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set parameter set skip flags

   \param[in]  orc_Value   New parameter set skip flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::SetSkipUpdateOfParamInfosFlags(const std::vector<bool> & orc_Value)
{
   this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX] = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set application skip flags

   \param[in]  orc_Value   New application skip flags
   \param[in]  oe_Type     Selector for structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::SetSkipUpdateOfPathsFlags(const std::vector<bool> & orc_Value,
                                                    const E_GenericFileType oe_Type)
{
   this->mc_SkipUpdateOfFiles[static_cast<int32_t>(oe_Type)] = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_Index           Index to access
   \param[in]  orc_FilePath         New path
   \param[in]  ou32_LastKnownCrc    Last known CRC for this file

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewNodeUpdate::SetParamInfoContent(const uint32_t ou32_Index, const C_SclString & orc_FilePath,
                                                 const uint32_t ou32_LastKnownCrc)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_ParamSetPaths.size())
   {
      C_OscViewNodeUpdateParamInfo & rc_ParamSet = this->mc_ParamSetPaths[ou32_Index];
      rc_ParamSet.SetContent(orc_FilePath, ou32_LastKnownCrc);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add path to internal structure

   \param[in]  orc_Path    New path
   \param[in]  oe_Type     Selector for structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::AddPath(const C_SclString & orc_Path, const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   if (oe_Type == eFTP_DATA_BLOCK)
   {
      this->mc_DataBlockPaths.push_back(orc_Path);
   }
   else
   {
      this->mc_FileBasedPaths.push_back(orc_Path);
   }
   this->mc_SkipUpdateOfFiles[static_cast<int32_t>(oe_Type)].push_back(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information for a parameter set

   \param[in]  orc_Value   New path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::AddParamInfo(const C_OscViewNodeUpdateParamInfo & orc_Value)
{
   this->mc_ParamSetPaths.push_back(orc_Value);
   this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].push_back(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information path

   \param[in]  ou32_Index  Index to access
   \param[in]  orc_Value   New path
   \param[in]  oe_Type     Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewNodeUpdate::SetPath(const uint32_t ou32_Index, const C_SclString & orc_Value,
                                     const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (oe_Type == eFTP_DATA_BLOCK)
   {
      if (ou32_Index < this->mc_DataBlockPaths.size())
      {
         this->mc_DataBlockPaths[ou32_Index] = orc_Value;
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      if (ou32_Index < this->mc_FileBasedPaths.size())
      {
         this->mc_FileBasedPaths[ou32_Index] = orc_Value;
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_Index  Index to access
   \param[in]  orc_Value   New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewNodeUpdate::SetParamInfo(const uint32_t ou32_Index, const C_OscViewNodeUpdateParamInfo & orc_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_ParamSetPaths.size())
   {
      this->mc_ParamSetPaths[ou32_Index] = orc_Value;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information path

   \param[in]  ou32_Index  Index to access
   \param[in]  oq_SkipFile Flag if file will be skipped when updating
   \param[in]  oe_Type     Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewNodeUpdate::SetSkipUpdateOfPath(const uint32_t ou32_Index, const bool oq_SkipFile,
                                                 const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Retval = C_NO_ERR;
   const int32_t s32_Type = static_cast<int32_t>(oe_Type);

   if (ou32_Index < this->mc_SkipUpdateOfFiles[s32_Type].size())
   {
      this->mc_SkipUpdateOfFiles[s32_Type][ou32_Index] = oq_SkipFile;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_Index  Index to access
   \param[in]  oq_SkipFile Flag if file will be skipped when updating

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewNodeUpdate::SetSkipUpdateOfParamInfo(const uint32_t ou32_Index, const bool oq_SkipFile)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].size())
   {
      this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX][ou32_Index] = oq_SkipFile;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information path

   \param[in]  ou32_Index  Index to remove
   \param[in]  oe_Type     Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewNodeUpdate::RemovePath(const uint32_t ou32_Index, const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (oe_Type == eFTP_DATA_BLOCK)
   {
      if (ou32_Index < this->mc_DataBlockPaths.size())
      {
         this->mc_DataBlockPaths.erase(this->mc_DataBlockPaths.begin() + ou32_Index);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      if (ou32_Index < this->mc_FileBasedPaths.size())
      {
         this->mc_FileBasedPaths.erase(this->mc_FileBasedPaths.begin() + ou32_Index);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      const int32_t s32_Type = static_cast<int32_t>(oe_Type);
      if (ou32_Index < this->mc_SkipUpdateOfFiles[s32_Type].size())
      {
         this->mc_SkipUpdateOfFiles[s32_Type].erase(this->mc_SkipUpdateOfFiles[s32_Type].begin() + ou32_Index);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information parameter set information

   \param[in]  ou32_Index  Index to remove

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewNodeUpdate::RemoveParamInfo(const uint32_t ou32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_ParamSetPaths.size())
   {
      this->mc_ParamSetPaths.erase(this->mc_ParamSetPaths.begin() + ou32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (ou32_Index < this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].size())
      {
         this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].erase(
            this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].begin() + ou32_Index);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information PEM file path

   \param[in]  orc_Value   New path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::SetPemFilePath(const C_SclString & orc_Value)
{
   this->mc_PemFilePath = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns node update information PEM file path

   \return
   - PEM file path
   - Empty string if no PEM file set
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscViewNodeUpdate::GetPemFilePath(void) const
{
   return this->mc_PemFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns node update information PEM file path

   Sets the state to do not change too
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::RemovePemFilePath(void)
{
   this->mc_PemFilePath = "";
   this->mq_SkipUpdateOfPemFile = false;
   this->me_StateSecurity = eST_SEC_NO_CHANGE;
   this->me_StateDebugger = eST_DEB_NO_CHANGE;
   this->mq_SkipUpdateOfPemFile = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information PEM file path skip flag

   \param[in]  oq_Skip   New flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::SetSkipUpdateOfPemFile(const bool oq_Skip)
{
   this->mq_SkipUpdateOfPemFile = oq_Skip;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns node update information PEM file path skip flag

   \retval   true       Flag for skipping the PEM file
   \retval   false      Flag for not skipping the PEM file
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscViewNodeUpdate::GetSkipUpdateOfPemFile(void) const
{
   return this->mq_SkipUpdateOfPemFile;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node update information states

   \param[in]      oe_StateSecurity   Security state of node
   \param[in]      oe_StateDebugger   Debugger state of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::SetStates(const C_OscViewNodeUpdate::E_StateSecurity oe_StateSecurity,
                                    const C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger)
{
   this->me_StateDebugger = oe_StateDebugger;
   this->me_StateSecurity = oe_StateSecurity;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the update information states of the node

   \param[out]      ore_StateSecurity   Security state of node
   \param[out]      ore_StateDebugger   Debugger state of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::GetStates(C_OscViewNodeUpdate::E_StateSecurity & ore_StateSecurity,
                                    C_OscViewNodeUpdate::E_StateDebugger & ore_StateDebugger) const
{
   ore_StateDebugger = this->me_StateDebugger;
   ore_StateSecurity = this->me_StateSecurity;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_ApplicationIndex   Application index
   \param[in]  oe_ApplicationType      Application type
   \param[in]  ou32_NumDataBlockPaths  Number of Data Block paths
   \param[in]  orc_AllApplications     All applications
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::OnSyncNodeApplicationAdded(const uint32_t ou32_ApplicationIndex,
                                                     const C_OscNodeApplication::E_Type oe_ApplicationType,
                                                     const uint32_t ou32_NumDataBlockPaths,
                                                     const std::vector<C_OscNodeApplication> & orc_AllApplications)
{
   if (oe_ApplicationType != stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC)
   {
      uint32_t u32_UpdateApplicationIndex;
      // Scenario guaranteed can only exist once [0-1]
      if (C_OscViewNodeUpdate::mh_CheckApplicationsContainParamTypeBeforeIndex(orc_AllApplications,
                                                                               ou32_ApplicationIndex))
      {
         // Skip one index as this would not have a datablock entry
         tgl_assert(ou32_ApplicationIndex > 0UL);
         u32_UpdateApplicationIndex = ou32_ApplicationIndex - 1UL;
      }
      else
      {
         u32_UpdateApplicationIndex = ou32_ApplicationIndex;
      }
      if (u32_UpdateApplicationIndex <= this->mc_DataBlockPaths.size())
      {
         const int32_t s32_TYPE = static_cast<int32_t>(C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         this->mc_DataBlockPaths.insert(this->mc_DataBlockPaths.begin() + u32_UpdateApplicationIndex, C_SclString());
         this->mc_SkipUpdateOfFiles[s32_TYPE].insert(
            this->mc_SkipUpdateOfFiles[s32_TYPE].begin() + u32_UpdateApplicationIndex, false);
      }
   }
   else
   {
      uint32_t u32_PathCounter;

      for (u32_PathCounter = 0U; u32_PathCounter < ou32_NumDataBlockPaths; ++u32_PathCounter)
      {
         this->mc_ParamSetPaths.insert(this->mc_ParamSetPaths.begin() + u32_PathCounter,
                                       C_OscViewNodeUpdateParamInfo());
         this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].insert(
            this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].begin() + u32_PathCounter, false);
      }
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_ApplicationSourceIndex   Application source index
   \param[in]  ou32_ApplicationTargetIndex   Application target index
   \param[in]  oe_ApplicationSourceType      Application source type
   \param[in]  oe_ApplicationTargetType      Application target type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::OnSyncNodeApplicationMoved(const uint32_t ou32_ApplicationSourceIndex,
                                                     const uint32_t ou32_ApplicationTargetIndex,
                                                     const C_OscNodeApplication::E_Type oe_ApplicationSourceType,
                                                     const C_OscNodeApplication::E_Type oe_ApplicationTargetType)
{
   // Moving only affects the update configuration of the applications if both applications are not NVM HALC datablocks.
   if ((oe_ApplicationSourceType != stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC) &&
       (oe_ApplicationTargetType != stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC))
   {
      C_SclString c_Entry;

      bool q_Entry = false;
      const int32_t s32_TYPE = static_cast<int32_t>(C_OscViewNodeUpdate::eFTP_DATA_BLOCK);

      if (ou32_ApplicationSourceIndex < this->mc_DataBlockPaths.size())
      {
         c_Entry = this->mc_DataBlockPaths[ou32_ApplicationSourceIndex];
         this->mc_DataBlockPaths.erase(this->mc_DataBlockPaths.begin() + ou32_ApplicationSourceIndex);

         q_Entry = this->mc_SkipUpdateOfFiles[s32_TYPE][ou32_ApplicationSourceIndex];
         this->mc_SkipUpdateOfFiles[s32_TYPE].erase(
            this->mc_SkipUpdateOfFiles[s32_TYPE].begin() + ou32_ApplicationSourceIndex);
      }
      if (ou32_ApplicationTargetIndex <= this->mc_DataBlockPaths.size())
      {
         this->mc_DataBlockPaths.insert(this->mc_DataBlockPaths.begin() + ou32_ApplicationTargetIndex, c_Entry);
         this->mc_SkipUpdateOfFiles[s32_TYPE].insert(
            this->mc_SkipUpdateOfFiles[s32_TYPE].begin() + ou32_ApplicationTargetIndex, q_Entry);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_ApplicationIndex   Application index
   \param[in]  oe_ApplicationType      Application type
   \param[in]  ou32_NumDataBlockPaths  Number of Data Block paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::OnSyncNodeApplicationAboutToBeDeleted(const uint32_t ou32_ApplicationIndex,
                                                                const C_OscNodeApplication::E_Type oe_ApplicationType,
                                                                const uint32_t ou32_NumDataBlockPaths)
{
   if (oe_ApplicationType != stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC)
   {
      if (ou32_ApplicationIndex < this->mc_DataBlockPaths.size())
      {
         const int32_t s32_TYPE = static_cast<int32_t>(C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         this->mc_DataBlockPaths.erase(this->mc_DataBlockPaths.begin() + ou32_ApplicationIndex);
         this->mc_SkipUpdateOfFiles[s32_TYPE].erase(
            this->mc_SkipUpdateOfFiles[s32_TYPE].begin() + ou32_ApplicationIndex);
      }
   }
   else
   {
      if (ou32_NumDataBlockPaths > 0U)
      {
         const uint32_t u32_EraseRange = ou32_NumDataBlockPaths;

         this->mc_ParamSetPaths.erase(this->mc_ParamSetPaths.begin(),
                                      this->mc_ParamSetPaths.begin() + u32_EraseRange);
         this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].erase(
            this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].begin(),
            this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].begin() + u32_EraseRange);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_ApplicationIndex   Application index
   \param[in]  oe_ApplicationType      Application type
   \param[in]  ou32_NumDataBlockPaths  Number of Data Block paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::OnSyncNodeApplicationAboutToBeChangedFromParamSetHalc(const uint32_t ou32_ApplicationIndex,
                                                                                const C_OscNodeApplication::E_Type oe_ApplicationType,
                                                                                const uint32_t ou32_NumDataBlockPaths)
{
   // Must be still the same type
   tgl_assert(oe_ApplicationType == stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC);
   if (oe_ApplicationType == stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC)
   {
      // Delete the param set specific information about the datablock
      this->OnSyncNodeApplicationAboutToBeDeleted(ou32_ApplicationIndex, oe_ApplicationType, ou32_NumDataBlockPaths);

      // Add as "normal" datablock
      if (ou32_ApplicationIndex <= this->mc_DataBlockPaths.size())
      {
         const int32_t s32_TYPE = static_cast<int32_t>(C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         this->mc_DataBlockPaths.insert(this->mc_DataBlockPaths.begin() + ou32_ApplicationIndex, C_SclString());
         this->mc_SkipUpdateOfFiles[s32_TYPE].insert(
            this->mc_SkipUpdateOfFiles[s32_TYPE].begin() + ou32_ApplicationIndex, false);
      }
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_ApplicationIndex   Application index
   \param[in]  oe_ApplicationType      Application type
   \param[in]  ou32_NumDataBlockPaths  Number of Data Block paths
   \param[in]  orc_AllApplications     All applications
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::OnSyncNodeApplicationChangedToParamSetHalc(const uint32_t ou32_ApplicationIndex,
                                                                     const C_OscNodeApplication::E_Type oe_ApplicationType, const uint32_t ou32_NumDataBlockPaths,
                                                                     const std::vector<C_OscNodeApplication> & orc_AllApplications)
{
   tgl_assert(oe_ApplicationType == stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC);
   if (oe_ApplicationType == stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC)
   {
      // Delete the "normal" datablock configuration
      if (ou32_ApplicationIndex < this->mc_DataBlockPaths.size())
      {
         const int32_t s32_TYPE = static_cast<int32_t>(C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         this->mc_DataBlockPaths.erase(this->mc_DataBlockPaths.begin() + ou32_ApplicationIndex);
         this->mc_SkipUpdateOfFiles[s32_TYPE].erase(
            this->mc_SkipUpdateOfFiles[s32_TYPE].begin() + ou32_ApplicationIndex);
      }

      // Add as param set datablock
      this->OnSyncNodeApplicationAdded(ou32_ApplicationIndex, oe_ApplicationType, ou32_NumDataBlockPaths,
                                       orc_AllApplications);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  oe_ApplicationType   Application type
   \param[in]  ou32_OldSize         Size of application result path before change
   \param[in]  ou32_NewSize         Size of application result path after change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewNodeUpdate::OnSyncNodeApplicationResultPathSizeChanged(
   const C_OscNodeApplication::E_Type oe_ApplicationType, const uint32_t ou32_OldSize, const uint32_t ou32_NewSize)
{
   tgl_assert(oe_ApplicationType == stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC);
   if (oe_ApplicationType == stw::opensyde_core::C_OscNodeApplication::ePARAMETER_SET_HALC)
   {
      if (ou32_OldSize < ou32_NewSize)
      {
         // 1 to 2
         // Insert one
         tgl_assert((ou32_OldSize == 1U) && (ou32_NewSize == 2U));
         this->mc_ParamSetPaths.insert(this->mc_ParamSetPaths.begin() + 1,
                                       C_OscViewNodeUpdateParamInfo());
         this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].insert(
            this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].begin() + 1, false);
      }
      else if (ou32_NewSize < ou32_OldSize)
      {
         // 2 to 1
         // Delete the second entry
         tgl_assert((ou32_OldSize == 2U) && (ou32_NewSize == 1U));
         this->mc_ParamSetPaths.erase(this->mc_ParamSetPaths.begin() + 1);
         this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].erase(
            this->mc_SkipUpdateOfFiles[mhs32_PARAMETER_SET_INDEX].begin() + 1);
      }
      else
      {
         // Same size, nothing to do.
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check applications contain param type before index

   \param[in]  orc_AllApplications     All applications
   \param[in]  ou32_ApplicationIndex   Application index

   \return
   Flags

   \retval   True    Param type found before index
   \retval   False   Param type not found before index
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscViewNodeUpdate::mh_CheckApplicationsContainParamTypeBeforeIndex(
   const std::vector<C_OscNodeApplication> & orc_AllApplications, const uint32_t ou32_ApplicationIndex)
{
   bool q_Retval = false;

   for (uint32_t u32_ItAppl = 0UL; (u32_ItAppl < orc_AllApplications.size()) && (u32_ItAppl < ou32_ApplicationIndex);
        ++u32_ItAppl)
   {
      const C_OscNodeApplication & rc_CurApplication = orc_AllApplications[u32_ItAppl];
      if (rc_CurApplication.e_Type == C_OscNodeApplication::ePARAMETER_SET_HALC)
      {
         q_Retval = true;
         break;
      }
   }
   return q_Retval;
}
