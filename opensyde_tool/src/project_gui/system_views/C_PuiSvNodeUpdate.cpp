//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update data (implementation)

   Node update data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "CSCLString.h"
#include "C_GtGetText.h"
#include "CSCLChecksums.h"
#include "C_PuiSvNodeUpdate.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;

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
C_PuiSvNodeUpdate::C_PuiSvNodeUpdate(void) :
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
void C_PuiSvNodeUpdate::CalcHash(uint32 & oru32_HashValue) const
{
   const sintn sn_Size = this->mc_DataBlockPaths.size();
   const stw_scl::C_SCLString c_PemFile = this->mc_PemFilePath.toStdString().c_str();

   stw_scl::C_SCLChecksums::CalcCRC32(&this->me_StateDebugger, sizeof(this->me_StateDebugger), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->me_StateSecurity, sizeof(this->me_StateSecurity), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Size, sizeof(sn_Size), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Size, sizeof(sn_Size), oru32_HashValue);
   for (uint32 u32_It = 0; u32_It < this->mc_DataBlockPaths.size(); ++u32_It)
   {
      const QString & rc_QtData = this->mc_DataBlockPaths[u32_It];
      const stw_scl::C_SCLString c_Data = rc_QtData.toStdString().c_str();
      stw_scl::C_SCLChecksums::CalcCRC32(c_Data.c_str(), c_Data.Length(), oru32_HashValue);
   }
   for (uint32 u32_It = 0; u32_It < this->mc_FileBasedPaths.size(); ++u32_It)
   {
      const QString & rc_QtData = this->mc_FileBasedPaths[u32_It];
      const stw_scl::C_SCLString c_Data = rc_QtData.toStdString().c_str();
      stw_scl::C_SCLChecksums::CalcCRC32(c_Data.c_str(), c_Data.Length(), oru32_HashValue);
   }
   for (uint32 u32_It = 0; u32_It < this->mc_ParamSetPaths.size(); ++u32_It)
   {
      const C_PuiSvNodeUpdateParamInfo & rc_Test = this->mc_ParamSetPaths[u32_It];
      rc_Test.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_It = 0; u32_It < this->mc_SkipUpdateOfFiles.size(); ++u32_It)
   {
      for (uint32 u32_SubIt = 0; u32_SubIt < this->mc_SkipUpdateOfFiles[u32_It].size(); ++u32_SubIt)
      {
         const bool q_Value = this->mc_SkipUpdateOfFiles[u32_It][u32_SubIt];
         stw_scl::C_SCLChecksums::CalcCRC32(&q_Value, sizeof(q_Value), oru32_HashValue);
      }
   }
   stw_scl::C_SCLChecksums::CalcCRC32(&u32_NodeUpdatePosition, sizeof(u32_NodeUpdatePosition), oru32_HashValue);

   stw_scl::C_SCLChecksums::CalcCRC32(c_PemFile.c_str(), c_PemFile.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mq_SkipUpdateOfPemFile, sizeof(this->mq_SkipUpdateOfPemFile),
                                      oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all parameter set paths for this node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::ClearParamPaths(void)
{
   this->mc_ParamSetPaths.clear();
   this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all paths as appropriate for the type

   \param[in]  oe_Type  Selector for structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::ClearPathsAsAppropriate(const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   const sintn sn_Type = static_cast<sintn>(oe_Type);

   if (oe_Type == eFTP_DATA_BLOCK)
   {
      for (uint32 u32_It = 0UL; u32_It < this->mc_DataBlockPaths.size(); ++u32_It)
      {
         this->mc_DataBlockPaths[u32_It] = C_GtGetText::h_GetText("<Add File>");
         this->mc_SkipUpdateOfFiles[sn_Type][u32_It] = false;
      }
   }
   else
   {
      this->mc_FileBasedPaths.clear();
      this->mc_SkipUpdateOfFiles[sn_Type].clear();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parameter set paths

   \return
   Current parameter set paths
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvNodeUpdateParamInfo> & C_PuiSvNodeUpdate::GetParamInfos(void) const
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
const std::vector<QString> & C_PuiSvNodeUpdate::GetPaths(const E_GenericFileType oe_Type) const
{
   if (oe_Type == eFTP_DATA_BLOCK)
   {
      return this->mc_DataBlockPaths;
   }
   else
   {
      return this->mc_FileBasedPaths;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parameter set skip flags

   \return
   Current application skip flags
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<bool> & C_PuiSvNodeUpdate::GetSkipUpdateOfParamInfosFlags(void) const
{
   return this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get application skip flags

   \param[in]  oe_Type  Selector for structure

   \return
   Current application skip flags
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<bool> & C_PuiSvNodeUpdate::GetSkipUpdateOfPathsFlags(const E_GenericFileType oe_Type) const
{
   return this->mc_SkipUpdateOfFiles[static_cast<sintn>(oe_Type)];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set parameter set paths

   \param[in]  orc_Value   New parameter set paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::SetParamInfos(const std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Value)
{
   this->mc_ParamSetPaths = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set application paths

   \param[in]  orc_Value   New application paths
   \param[in]  oe_Type     Selector for structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::SetPaths(const std::vector<QString> & orc_Value, const E_GenericFileType oe_Type)
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
void C_PuiSvNodeUpdate::SetSkipUpdateOfParamInfosFlags(const std::vector<bool> & orc_Value)
{
   this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX] = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set application skip flags

   \param[in]  orc_Value   New application skip flags
   \param[in]  oe_Type     Selector for structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::SetSkipUpdateOfPathsFlags(const std::vector<bool> & orc_Value, const E_GenericFileType oe_Type)
{
   this->mc_SkipUpdateOfFiles[static_cast<sintn>(oe_Type)] = orc_Value;
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
sint32 C_PuiSvNodeUpdate::SetParamInfoContent(const uint32 ou32_Index, const QString & orc_FilePath,
                                              const uint32 ou32_LastKnownCrc)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_ParamSetPaths.size())
   {
      C_PuiSvNodeUpdateParamInfo & rc_ParamSet = this->mc_ParamSetPaths[ou32_Index];
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
void C_PuiSvNodeUpdate::AddPath(const QString & orc_Path, const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   if (oe_Type == eFTP_DATA_BLOCK)
   {
      this->mc_DataBlockPaths.push_back(orc_Path);
   }
   else
   {
      this->mc_FileBasedPaths.push_back(orc_Path);
   }
   this->mc_SkipUpdateOfFiles[static_cast<sintn>(oe_Type)].push_back(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information for a parameter set

   \param[in]  orc_Value   New path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::AddParamInfo(const C_PuiSvNodeUpdateParamInfo & orc_Value)
{
   this->mc_ParamSetPaths.push_back(orc_Value);
   this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].push_back(false);
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
sint32 C_PuiSvNodeUpdate::SetPath(const uint32 ou32_Index, const QString & orc_Value,
                                  const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvNodeUpdate::SetParamInfo(const uint32 ou32_Index, const C_PuiSvNodeUpdateParamInfo & orc_Value)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvNodeUpdate::SetSkipUpdateOfPath(const uint32 ou32_Index, const bool oq_SkipFile,
                                              const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;
   const sintn sn_Type = static_cast<sintn>(oe_Type);

   if (ou32_Index < this->mc_SkipUpdateOfFiles[sn_Type].size())
   {
      this->mc_SkipUpdateOfFiles[sn_Type][ou32_Index] = oq_SkipFile;
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
sint32 C_PuiSvNodeUpdate::SetSkipUpdateOfParamInfo(const uint32 ou32_Index, const bool oq_SkipFile)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].size())
   {
      this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX][ou32_Index] = oq_SkipFile;
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
sint32 C_PuiSvNodeUpdate::RemovePath(const uint32 ou32_Index, const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

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
      const sintn sn_Type = static_cast<sintn>(oe_Type);
      if (ou32_Index < this->mc_SkipUpdateOfFiles[sn_Type].size())
      {
         this->mc_SkipUpdateOfFiles[sn_Type].erase(this->mc_SkipUpdateOfFiles[sn_Type].begin() + ou32_Index);
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
sint32 C_PuiSvNodeUpdate::RemoveParamInfo(const uint32 ou32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

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
      if (ou32_Index < this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].size())
      {
         this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].erase(
            this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].begin() + ou32_Index);
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
void C_PuiSvNodeUpdate::SetPemFilePath(const QString & orc_Value)
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
QString C_PuiSvNodeUpdate::GetPemFilePath(void) const
{
   return this->mc_PemFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns node update information PEM file path

   Sets the state to do not change too

   \retval   Path             PEM file path
   \retval   Empty string     No PEM file is set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::RemovePemFilePath(void)
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
void C_PuiSvNodeUpdate::SetSkipUpdateOfPemFile(const bool oq_Skip)
{
   this->mq_SkipUpdateOfPemFile = oq_Skip;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns node update information PEM file path skip flag

   \retval   true       Flag for skipping the PEM file
   \retval   false      Flag for not skipping the PEM file
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvNodeUpdate::GetSkipUpdateOfPemFile(void) const
{
   return this->mq_SkipUpdateOfPemFile;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node update information states

   \param[in]      oe_StateSecurity   Security state of node
   \param[in]      oe_StateDebugger   Debugger state of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::SetStates(const C_PuiSvNodeUpdate::E_StateSecurity oe_StateSecurity,
                                  const C_PuiSvNodeUpdate::E_StateDebugger oe_StateDebugger)
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
void C_PuiSvNodeUpdate::GetStates(C_PuiSvNodeUpdate::E_StateSecurity & ore_StateSecurity,
                                  C_PuiSvNodeUpdate::E_StateDebugger & ore_StateDebugger) const
{
   ore_StateDebugger = this->me_StateDebugger;
   ore_StateSecurity = this->me_StateSecurity;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationAdded(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex)
{
   const stw_opensyde_core::C_OSCNodeApplication * const pc_App = C_PuiSdHandler::h_GetInstance()->GetApplication(
      ou32_NodeIndex,
      ou32_ApplicationIndex);

   tgl_assert(pc_App != NULL);
   if (pc_App != NULL)
   {
      if (pc_App->e_Type != stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC)
      {
         if (ou32_ApplicationIndex <= this->mc_DataBlockPaths.size())
         {
            const sintn sn_TYPE = static_cast<sintn>(C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
            this->mc_DataBlockPaths.insert(this->mc_DataBlockPaths.begin() + ou32_ApplicationIndex, QString());
            this->mc_SkipUpdateOfFiles[sn_TYPE].insert(
               this->mc_SkipUpdateOfFiles[sn_TYPE].begin() + ou32_ApplicationIndex, false);
         }
      }
      else
      {
         uint32 u32_PathCounter;

         for (u32_PathCounter = 0U; u32_PathCounter < pc_App->c_ResultPaths.size(); ++u32_PathCounter)
         {
            this->mc_ParamSetPaths.insert(this->mc_ParamSetPaths.begin() + u32_PathCounter,
                                          C_PuiSvNodeUpdateParamInfo());
            this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].insert(
               this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].begin() + u32_PathCounter, false);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationSourceIndex   Application source index
   \param[in]  ou32_ApplicationTargetIndex   Application target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationMoved(const uint32 ou32_NodeIndex,
                                                   const uint32 ou32_ApplicationSourceIndex,
                                                   const uint32 ou32_ApplicationTargetIndex)
{
   const stw_opensyde_core::C_OSCNodeApplication * const pc_AppOne = C_PuiSdHandler::h_GetInstance()->GetApplication(
      ou32_NodeIndex,
      ou32_ApplicationSourceIndex);
   const stw_opensyde_core::C_OSCNodeApplication * const pc_AppTwo = C_PuiSdHandler::h_GetInstance()->GetApplication(
      ou32_NodeIndex,
      ou32_ApplicationTargetIndex);

   tgl_assert(pc_AppOne != NULL);
   tgl_assert(pc_AppTwo != NULL);
   // Moving only affects the update configuration of the applications if both applications are not NVM HALC datablocks.
   if ((pc_AppOne != NULL) && (pc_AppTwo != NULL) &&
       (pc_AppOne->e_Type  != stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC) &&
       (pc_AppTwo->e_Type  != stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC))
   {
      QString c_Entry;

      bool q_Entry = false;
      const sintn sn_TYPE = static_cast<sintn>(C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);

      if (ou32_ApplicationSourceIndex < this->mc_DataBlockPaths.size())
      {
         c_Entry = this->mc_DataBlockPaths[ou32_ApplicationSourceIndex];
         this->mc_DataBlockPaths.erase(this->mc_DataBlockPaths.begin() + ou32_ApplicationSourceIndex);

         q_Entry = this->mc_SkipUpdateOfFiles[sn_TYPE][ou32_ApplicationSourceIndex];
         this->mc_SkipUpdateOfFiles[sn_TYPE].erase(
            this->mc_SkipUpdateOfFiles[sn_TYPE].begin() + ou32_ApplicationSourceIndex);
      }
      if (ou32_ApplicationTargetIndex <= this->mc_DataBlockPaths.size())
      {
         this->mc_DataBlockPaths.insert(this->mc_DataBlockPaths.begin() + ou32_ApplicationTargetIndex, c_Entry);
         this->mc_SkipUpdateOfFiles[sn_TYPE].insert(
            this->mc_SkipUpdateOfFiles[sn_TYPE].begin() + ou32_ApplicationTargetIndex, q_Entry);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                              const uint32 ou32_ApplicationIndex)
{
   const stw_opensyde_core::C_OSCNodeApplication * const pc_App = C_PuiSdHandler::h_GetInstance()->GetApplication(
      ou32_NodeIndex,
      ou32_ApplicationIndex);

   // Application still exists. Deletion will be done after this function
   tgl_assert(pc_App != NULL);
   if (pc_App != NULL)
   {
      if (pc_App->e_Type != stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC)
      {
         if (ou32_ApplicationIndex < this->mc_DataBlockPaths.size())
         {
            const sintn sn_TYPE = static_cast<sintn>(C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
            this->mc_DataBlockPaths.erase(this->mc_DataBlockPaths.begin() + ou32_ApplicationIndex);
            this->mc_SkipUpdateOfFiles[sn_TYPE].erase(
               this->mc_SkipUpdateOfFiles[sn_TYPE].begin() + ou32_ApplicationIndex);
         }
      }
      else
      {
         if (pc_App->c_ResultPaths.size() > 0U)
         {
            const uintn un_EraseRange = pc_App->c_ResultPaths.size();

            this->mc_ParamSetPaths.erase(this->mc_ParamSetPaths.begin(),
                                         this->mc_ParamSetPaths.begin() + un_EraseRange);
            this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].erase(
               this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].begin(),
               this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].begin() + un_EraseRange);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationAboutToBeChangedFromParamSetHALC(const uint32 ou32_NodeIndex,
                                                                              const uint32 ou32_ApplicationIndex)
{
   const stw_opensyde_core::C_OSCNodeApplication * const pc_App = C_PuiSdHandler::h_GetInstance()->GetApplication(
      ou32_NodeIndex,
      ou32_ApplicationIndex);

   // Application still exists. Deletion will be done after this function
   tgl_assert(pc_App != NULL);
   if (pc_App != NULL)
   {
      // Must be still the same type
      tgl_assert(pc_App->e_Type == stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC);
      if (pc_App->e_Type == stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC)
      {
         // Delete the param set specific information about the datablock
         this->OnSyncNodeApplicationAboutToBeDeleted(ou32_NodeIndex, ou32_ApplicationIndex);

         // Add as "normal" datablock
         if (ou32_ApplicationIndex <= this->mc_DataBlockPaths.size())
         {
            const sintn sn_TYPE = static_cast<sintn>(C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
            this->mc_DataBlockPaths.insert(this->mc_DataBlockPaths.begin() + ou32_ApplicationIndex, QString());
            this->mc_SkipUpdateOfFiles[sn_TYPE].insert(
               this->mc_SkipUpdateOfFiles[sn_TYPE].begin() + ou32_ApplicationIndex, false);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationChangedToParamSetHALC(const uint32 ou32_NodeIndex,
                                                                   const uint32 ou32_ApplicationIndex)
{
   const stw_opensyde_core::C_OSCNodeApplication * const pc_App = C_PuiSdHandler::h_GetInstance()->GetApplication(
      ou32_NodeIndex,
      ou32_ApplicationIndex);

   // Application still exists. Deletion will be done after this function
   tgl_assert(pc_App != NULL);
   if (pc_App != NULL)
   {
      tgl_assert(pc_App->e_Type == stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC);
      if (pc_App->e_Type == stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC)
      {
         // Delete the "normal" datablock configuration
         if (ou32_ApplicationIndex < this->mc_DataBlockPaths.size())
         {
            const sintn sn_TYPE = static_cast<sintn>(C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
            this->mc_DataBlockPaths.erase(this->mc_DataBlockPaths.begin() + ou32_ApplicationIndex);
            this->mc_SkipUpdateOfFiles[sn_TYPE].erase(
               this->mc_SkipUpdateOfFiles[sn_TYPE].begin() + ou32_ApplicationIndex);
         }

         // Add as param set datablock
         this->OnSyncNodeApplicationAdded(ou32_NodeIndex, ou32_ApplicationIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
   \param[in]  ou32_OldSize            Size of application result path before change
   \param[in]  ou32_NewSize            Size of application result path after change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationResultPathSizeChanged(const uint32 ou32_NodeIndex,
                                                                   const uint32 ou32_ApplicationIndex,
                                                                   const stw_types::uint32 ou32_OldSize,
                                                                   const stw_types::uint32 ou32_NewSize)
{
   const stw_opensyde_core::C_OSCNodeApplication * const pc_App = C_PuiSdHandler::h_GetInstance()->GetApplication(
      ou32_NodeIndex,
      ou32_ApplicationIndex);

   // Application still exists. Deletion will be done after this function
   tgl_assert(pc_App != NULL);
   if (pc_App != NULL)
   {
      tgl_assert(pc_App->e_Type == stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC);
      if (pc_App->e_Type == stw_opensyde_core::C_OSCNodeApplication::ePARAMETER_SET_HALC)
      {
         if (ou32_OldSize < ou32_NewSize)
         {
            // 1 to 2
            // Insert one
            tgl_assert((ou32_OldSize == 1U) && (ou32_NewSize == 2U));
            this->mc_ParamSetPaths.insert(this->mc_ParamSetPaths.begin() + 1,
                                          C_PuiSvNodeUpdateParamInfo());
            this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].insert(
               this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].begin() + 1, false);
         }
         else if (ou32_NewSize < ou32_OldSize)
         {
            // 2 to 1
            // Delete the second entry
            tgl_assert((ou32_OldSize == 2U) && (ou32_NewSize == 1U));
            this->mc_ParamSetPaths.erase(this->mc_ParamSetPaths.begin() + 1);
            this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].erase(
               this->mc_SkipUpdateOfFiles[mhsn_PARAMETER_SET_INDEX].begin() + 1);
         }
         else
         {
            // Same size, nothing to do.
         }
      }
   }
}
