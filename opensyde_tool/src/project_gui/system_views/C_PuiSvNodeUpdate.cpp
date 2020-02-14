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
#include "CSCLString.h"
#include "C_GtGetText.h"
#include "CSCLChecksums.h"
#include "C_PuiSvNodeUpdate.h"

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
   u32_NodeUpdatePosition(0U)
{
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all parameter set paths for this node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::ClearParamPaths(void)
{
   this->mc_ParamSetPaths.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all paths as appropriate for the type

   \param[in]  oe_Type  Selector for structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::ClearPathsAsAppropriate(const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   if (oe_Type == eFTP_DATA_BLOCK)
   {
      for (uint32 u32_It = 0UL; u32_It < this->mc_DataBlockPaths.size(); ++u32_It)
      {
         this->mc_DataBlockPaths[u32_It] = C_GtGetText::h_GetText("<Add File>");
      }
   }
   else
   {
      this->mc_FileBasedPaths.clear();
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information for a parameter set

   \param[in]  orc_Value   New path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::AddParamInfo(const C_PuiSvNodeUpdateParamInfo & orc_Value)
{
   this->mc_ParamSetPaths.push_back(orc_Value);
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationAdded(const uint32 ou32_ApplicationIndex)
{
   if (ou32_ApplicationIndex <= this->mc_DataBlockPaths.size())
   {
      this->mc_DataBlockPaths.insert(this->mc_DataBlockPaths.begin() + ou32_ApplicationIndex, QString());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_ApplicationSourceIndex   Application source index
   \param[in]  ou32_ApplicationTargetIndex   Application target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationMoved(const uint32 ou32_ApplicationSourceIndex,
                                                   const uint32 ou32_ApplicationTargetIndex)
{
   QString c_Entry;

   if (ou32_ApplicationSourceIndex < this->mc_DataBlockPaths.size())
   {
      c_Entry = this->mc_DataBlockPaths[ou32_ApplicationSourceIndex];
      this->mc_DataBlockPaths.erase(this->mc_DataBlockPaths.begin() + ou32_ApplicationSourceIndex);
   }
   if (ou32_ApplicationTargetIndex <= this->mc_DataBlockPaths.size())
   {
      this->mc_DataBlockPaths.insert(this->mc_DataBlockPaths.begin() + ou32_ApplicationTargetIndex, c_Entry);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationAboutToBeDeleted(const uint32 ou32_ApplicationIndex)
{
   if (ou32_ApplicationIndex < this->mc_DataBlockPaths.size())
   {
      this->mc_DataBlockPaths.erase(this->mc_DataBlockPaths.begin() + ou32_ApplicationIndex);
   }
}
