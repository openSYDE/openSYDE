//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export parameter set image for an openSYDE node.

   Create a .syde_psi file providing entire NVM hardware configuration.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCExportParamSet.h"

#include "CSCLChecksums.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "C_OSCExportUti.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCParamSetHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get file name for parameter set image file *.syde_psi

   \param[in]  orc_DataBlock  Data block definition
   \param[in]  oq_IsSafe      Flag for safe vs. non-safe configuration

   \return
   file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportParamSet::h_GetFileName(const C_OSCNodeApplication & orc_DataBlock, const bool oq_IsSafe)
{
   C_SCLString c_Retval;

   tgl_assert(orc_DataBlock.c_ResultPaths.size() > 0);

   if (oq_IsSafe == true)
   {
      // first result path is always safe file
      c_Retval = TGL_ExtractFileName(orc_DataBlock.c_ResultPaths[0]);
   }
   else
   {
      // if there are two Data Blocks, the first result path is the safe file and the second is the non-safe file
      if (orc_DataBlock.c_ResultPaths.size() > 1)
      {
         c_Retval = TGL_ExtractFileName(orc_DataBlock.c_ResultPaths[1]);
      }
      else
      {
         c_Retval = TGL_ExtractFileName(orc_DataBlock.c_ResultPaths[0]);
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create parameter set image

   \param[in]   orc_Path               Storage path for created files
   \param[in]   orc_Node               Node as information source for exported code
   \param[in]   ou16_ApplicationIndex  Index of Data Block within orc_Node.c_Applications
   \param[out]  orc_Files              List of all exported file names (with absolute or relative path)
   \param[in]   orc_ExportToolName     Name of calling executable
   \param[in]   orc_ExportToolVersion  Version of calling executable

   \return
   C_NO_ERR    Success
   C_RD_WR     Problems accessing file system
   C_CONFIG    Internal data invalid (e.g. incorrect number of lists or datasets in HALC NVM Datapool)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportParamSet::h_CreateParameterSetImage(const C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                                      const uint16 ou16_ApplicationIndex,
                                                      std::vector<C_SCLString> & orc_Files,
                                                      const C_SCLString & orc_ExportToolName,
                                                      const C_SCLString & orc_ExportToolVersion)
{
   sint32 s32_Retval = C_NO_ERR;

   const C_OSCNodeApplication & rc_Application = orc_Node.c_Applications[ou16_ApplicationIndex];

   // make sure version is known
   if (rc_Application.u16_GenCodeVersion > C_OSCNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
   {
      s32_Retval = C_NOACT;
      osc_write_log_error("Creating PSI file",
                          "Did not generate code for HALC configuration because file format version is unknown.");
   }

   // plausibility check if number of Datablock output paths matches number of files we want to write
   if ((orc_Node.c_HALCConfig.e_SafetyMode == C_OSCHalcDefBase::eTWO_LEVELS_WITH_DROPPING) ||
       (orc_Node.c_HALCConfig.e_SafetyMode == C_OSCHalcDefBase::eTWO_LEVELS_WITHOUT_DROPPING))
   {
      if (rc_Application.c_ResultPaths.size() != 2)
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      if (rc_Application.c_ResultPaths.size() != 1)
      {
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval != C_NO_ERR)
   {
      osc_write_log_error("Creating PSI file",
                          "Could not generate files because number of output files is not correctly configured.");
   }

   // safe part
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_Node.c_HALCConfig.e_SafetyMode != C_OSCHalcDefBase::eONE_LEVEL_ALL_NON_SAFE)
      {
         const bool q_IsSafe = true;
         C_OSCParamSetRawNode c_RawNodeSafe;
         C_OSCParamSetInterpretedNode c_IntNodeSafe;

         s32_Retval = mh_FillPsiStructure(orc_Node, q_IsSafe, ou16_ApplicationIndex, c_RawNodeSafe, c_IntNodeSafe);

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_OSCExportParamSet::mh_WriteParameterSetImage(
               c_RawNodeSafe, c_IntNodeSafe, q_IsSafe, rc_Application, orc_Path, orc_Files,
               orc_ExportToolName, orc_ExportToolVersion);
         }
      }
   }

   // non safe part
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_Node.c_HALCConfig.e_SafetyMode != C_OSCHalcDefBase::eONE_LEVEL_ALL_SAFE)
      {
         const bool q_IsSafe = false;
         C_OSCParamSetRawNode c_RawNodeNonSafe;
         C_OSCParamSetInterpretedNode c_IntNodeNonSafe;

         s32_Retval =
            mh_FillPsiStructure(orc_Node, q_IsSafe, ou16_ApplicationIndex, c_RawNodeNonSafe, c_IntNodeNonSafe);

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_OSCExportParamSet::mh_WriteParameterSetImage(
               c_RawNodeNonSafe, c_IntNodeNonSafe, q_IsSafe, rc_Application, orc_Path, orc_Files,
               orc_ExportToolName, orc_ExportToolVersion);
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill PSI data structure with data from safe or non-safe Datapools

   \param[in]   orc_Node               Node as information source for exported code
   \param[in]   oq_IsSafe              Flag if to generate safe or non safe structure
   \param[in]   ou16_ApplicationIndex  Index of Data Block within orc_Node.c_Applications
   \param[out]  orc_RawNode            Raw data structure
   \param[out]  orc_IntNode            Interpreted data structure

   \return
   C_NO_ERR    Success
   C_CONFIG    Internal data invalid (e.g. incorrect number of lists or datasets in HALC NVM Datapool)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportParamSet::mh_FillPsiStructure(const C_OSCNode & orc_Node, const bool oq_IsSafe,
                                                const uint16 ou16_ApplicationIndex, C_OSCParamSetRawNode & orc_RawNode,
                                                C_OSCParamSetInterpretedNode & orc_IntNode)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_DatapoolFound = false;

   orc_RawNode.c_Name = orc_Node.c_Properties.c_Name;
   orc_IntNode.c_Name = orc_Node.c_Properties.c_Name;

   const C_OSCNodeDataPool * const pc_FirstDataPool = orc_Node.GetHalDataPoolConst(oq_IsSafe);

   if (pc_FirstDataPool != NULL)
   {
      // get raw data (data from only dataset of list "configuration" of first Datapool)
      std::vector<uint8> c_ConfigRawBytes;
      s32_Retval = mh_GetConfigurationRawBytes(*pc_FirstDataPool, c_ConfigRawBytes);

      if (s32_Retval != C_NO_ERR)
      {
         osc_write_log_error("Creating PSI file",
                             "Could not generate files because of unexpected number of lists or datasets or list "
                             "elements in Datapool " + orc_Node.c_Properties.c_Name + "::" + pc_FirstDataPool->c_Name);
      }

      // add data of all Datapools of specified safety
      for (uint32 u32_ItDataPool = 0U; (u32_ItDataPool < orc_Node.c_DataPools.size()) && (s32_Retval == C_NO_ERR);
           ++u32_ItDataPool)
      {
         const C_OSCNodeDataPool & rc_SdDataPool = orc_Node.c_DataPools[u32_ItDataPool];

         if ((rc_SdDataPool.s32_RelatedDataBlockIndex == ou16_ApplicationIndex) &&
             (rc_SdDataPool.e_Type == C_OSCNodeDataPool::eHALC_NVM) &&
             (rc_SdDataPool.q_IsSafety == oq_IsSafe))
         {
            C_OSCParamSetInterpretedDataPool c_IntDatapool;

            q_DatapoolFound = true;

            // fill interpreted Datapool data
            mh_FillInterpretedDatapool(rc_SdDataPool, c_IntDatapool);

            orc_IntNode.c_DataPools.push_back(c_IntDatapool);
            orc_RawNode.c_DataPools.push_back(c_IntDatapool.c_DataPoolInfo);

            // fill raw entries
            s32_Retval = mh_FillRawEntries(rc_SdDataPool, c_ConfigRawBytes, orc_RawNode.c_Entries);
         }

         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Creating PSI file",
                                "Could not generate files because of unexpected number of lists or unexpected NVM "
                                "size in Datapool " + orc_Node.c_Properties.c_Name + "::" + rc_SdDataPool.c_Name);
         }
      }
   }

   if (q_DatapoolFound == false)
   {
      s32_Retval = C_CONFIG;
      osc_write_log_error("Creating PSI file",
                          "Datapool missing for specified safety mode of HALC definition.");
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add node to parameter set image

   \param[in,out]  orc_SdDataPool   Datapool definition from system definition
   \param[in,out]  orc_IntDataPool  Interpreted Datapool data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportParamSet::mh_FillInterpretedDatapool(const C_OSCNodeDataPool & orc_SdDataPool,
                                                     C_OSCParamSetInterpretedDataPool & orc_IntDataPool)
{
   orc_IntDataPool.c_DataPoolInfo.c_Name = orc_SdDataPool.c_Name;
   orc_SdDataPool.CalcDefinitionHash(orc_IntDataPool.c_DataPoolInfo.u32_DataPoolCrc);
   orc_IntDataPool.c_DataPoolInfo.u32_NvMStartAddress = orc_SdDataPool.u32_NvMStartAddress;
   orc_IntDataPool.c_DataPoolInfo.u32_NvMSize = orc_SdDataPool.u32_NvMSize;
   memcpy(orc_IntDataPool.c_DataPoolInfo.au8_Version, orc_SdDataPool.au8_Version, 3);

   // Add all lists
   for (uint16 u16_ListIndex = 0U; u16_ListIndex < orc_SdDataPool.c_Lists.size(); u16_ListIndex++)
   {
      const C_OSCNodeDataPoolList & rc_SdList = orc_SdDataPool.c_Lists[u16_ListIndex];

      C_OSCParamSetInterpretedList c_IntList;
      c_IntList.c_Name = rc_SdList.c_Name;

      // Add all list elements
      for (uint16 u16_ElementIndex = 0U; u16_ElementIndex < rc_SdList.c_Elements.size(); u16_ElementIndex++)
      {
         const C_OSCNodeDataPoolListElement rc_SdElement = rc_SdList.c_Elements[u16_ElementIndex];

         C_OSCParamSetInterpretedElement c_IntElement;
         c_IntElement.c_Name = rc_SdElement.c_Name;
         c_IntElement.c_NvmValue = rc_SdElement.c_NvmValue;

         c_IntList.c_Elements.push_back(c_IntElement);
      }

      orc_IntDataPool.c_Lists.push_back(c_IntList);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill raw entries with data

   Add 4 raw entries:
      1. CRC and HALC NVM configuration values from the only Dataset of list "configuration"
      2.-4. CRC and zeroes

   \param[in]      orc_SdDataPool      Datapool definition from system definition (might be a "HAL Datapool copy")
   \param[in]      orc_ConfigRawBytes  Raw bytes containing HALC NVM configuration data
   \param[in,out]  orc_Entries         Raw entries to be filled with data

   \return
   C_NO_ERR    Success
   C_CONFIG    number of Datapool lists or datasets or list elements not as expected
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportParamSet::mh_FillRawEntries(const C_OSCNodeDataPool & orc_SdDataPool,
                                              const std::vector<uint8> & orc_ConfigRawBytes,
                                              std::vector<C_OSCParamSetRawEntry> & orc_Entries)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_SdDataPool.c_Lists.size() == 4) // exactly 4 expected: configuration, inputs, outputs, statuses
   {
      for (uint16 u16_ListIndex = 0U; (u16_ListIndex < orc_SdDataPool.c_Lists.size()) && (s32_Retval == C_NO_ERR);
           u16_ListIndex++)
      {
         C_OSCParamSetRawEntry c_RawEntry;

         c_RawEntry.u32_StartAddress = orc_SdDataPool.c_Lists[u16_ListIndex].u32_NvMStartAddress;
         c_RawEntry.c_Bytes.clear();

         if (u16_ListIndex == 0)
         {
            // extra handling for list "configuration"
            c_RawEntry.c_Bytes = orc_ConfigRawBytes;
         }
         else
         {
            c_RawEntry.c_Bytes.resize(orc_SdDataPool.c_Lists[u16_ListIndex].u32_NvMSize, 0);
         }

         s32_Retval = mh_InsertCRC16(c_RawEntry.c_Bytes);
         orc_Entries.push_back(c_RawEntry);
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get file info for parameter set image file

   \param[in]  orc_ExportToolName      Name of calling executable
   \param[in]  orc_ExportToolVersion   Version of calling executable

   \return
   file info class instance
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCParamSetInterpretedFileInfoData C_OSCExportParamSet::mh_GetFileInfo(const C_SCLString & orc_ExportToolName,
                                                                         const C_SCLString & orc_ExportToolVersion)
{
   C_OSCParamSetInterpretedFileInfoData c_Info;

   C_SCLString c_Tmp;
   C_TGLDateTime c_DateTime;

   TGL_GetDateTimeNow(c_DateTime);
   c_Info.c_DateTime = C_OSCLoggingHandler::h_UtilConvertDateTimeToString(c_DateTime);
   TGL_GetSystemUserName(c_Tmp);
   c_Info.c_Creator = c_Tmp;
   c_Info.c_ToolName = orc_ExportToolName;
   c_Info.c_ToolVersion = orc_ExportToolVersion;
   c_Info.c_ProjectName = "N/A";    // info not available as we only load system definition
   c_Info.c_ProjectVersion = "N/A"; // info not available as we only load system definition
   c_Info.c_UserComment = "Automatically generated parameter set image file.";

   return c_Info;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Write parameter set image

   \param[in,out]  orc_RawNode            Raw node containing data
   \param[in,out]  orc_IntNode            Interpreted node containing data
   \param[in]      oq_IsSafe              Flag for safe vs. non-safe configuration
   \param[in]      orc_DataBlock          Data Block definition
   \param[in]      orc_Path               Storage path for created file
   \param[in,out]  orc_Files              List of all exported file names (with absolute or relative path)
   \param[in]      orc_ExportToolName     Name of calling executable
   \param[in]      orc_ExportToolVersion  Version of calling executable

   \return
   C_NO_ERR    Operation success
   C_CONFIG    Internal data invalid (e.g. incorrect number of lists or datasets in HALC NVM Datapool)
   C_RD_WR     Problems accessing file system
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportParamSet::mh_WriteParameterSetImage(const C_OSCParamSetRawNode & orc_RawNode,
                                                      const C_OSCParamSetInterpretedNode & orc_IntNode,
                                                      const bool oq_IsSafe, const C_OSCNodeApplication & orc_DataBlock,
                                                      const C_SCLString & orc_Path,
                                                      std::vector<C_SCLString> & orc_Files,
                                                      const C_SCLString & orc_ExportToolName,
                                                      const C_SCLString & orc_ExportToolVersion)
{
   sint32 s32_Retval;

   const C_SCLString c_Path = TGL_FileIncludeTrailingDelimiter(orc_Path) +
                              h_GetFileName(orc_DataBlock, oq_IsSafe);
   const C_OSCParamSetInterpretedFileInfoData c_FileInfo = mh_GetFileInfo(orc_ExportToolName, orc_ExportToolVersion);

   C_OSCParamSetHandler c_DataHandler;

   // Add raw & interpreted data
   s32_Retval = c_DataHandler.AddInterpretedDataForNode(orc_IntNode);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = c_DataHandler.AddRawDataForNode(orc_RawNode);
   }

   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_CONFIG;
      osc_write_log_error("Creating PSI file",
                          "Could not generate files because of invalid internal structure.");
   }
   else
   {
      // Remove pre-existing files because PSI writing will else result in an error
      if (TGL_FileExists(c_Path) == true)
      {
         sintn sn_Return;
         sn_Return = std::remove(c_Path.c_str());
         if (sn_Return != 0)
         {
            osc_write_log_error("Creating PSI file", "Could not erase pre-existing file \"" + c_Path + "\".");
            s32_Retval = C_RD_WR;
         }
      }

      // Add file info data and write the file
      c_DataHandler.AddInterpretedFileData(c_FileInfo);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = c_DataHandler.CreateCleanFileWithoutCRC(c_Path);
      }

      // Add file CRC
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OSCParamSetHandler::h_UpdateCRCForFile(c_Path);
      }

      // Handle file names
      if (s32_Retval == C_NO_ERR)
      {
         C_OSCExportUti::h_CollectFilePaths(orc_Files, orc_Path,
                                            C_OSCExportParamSet::h_GetFileName(orc_DataBlock, oq_IsSafe), false);
      }
      else
      {
         s32_Retval = C_RD_WR;
         osc_write_log_error("Writing PSI file", "Could not write to file \"" + c_Path + "\".");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert 16-bit CRC value

   Calculate CRC value over bytes 3+ and insert in byte 1 and 2.
   The 16 bit CRC_CCITT is used with start value 0x1D0F.

   \param[in,out]  orc_Bytes  Bytes

   \return
   C_NO_ERR    everything okay
   C_CONFIG    number of Datapool list elements not as expected
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportParamSet::mh_InsertCRC16(std::vector<uint8> & orc_Bytes)
{
   sint32 s32_Retval = C_NO_ERR;
   uint16 u16_Crc = 0x1d0f;

   if (orc_Bytes.size() > 2)
   {
      stw_scl::C_SCLChecksums::CalcCRC16(&orc_Bytes[2], static_cast<uint32>(orc_Bytes.size()) - 2, u16_Crc);
      orc_Bytes[0] = static_cast<uint8>(u16_Crc >> 8U);
      orc_Bytes[1] = static_cast<uint8>(u16_Crc);
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set raw bytes corresponding to list configuration.

   Fill raw bytes of raw entry corresponding to list "configuration".
   Use the values from the one and only dataset.

   \param[in]   orc_SdDataPool   Datapool definition from system definition
   \param[out]  orc_Bytes        Bytes to be filled with configuration data

   \return
   C_NO_ERR    everything okay
   C_CONFIG    number of Datapool lists or datasets not as expected
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportParamSet::mh_GetConfigurationRawBytes(const C_OSCNodeDataPool & orc_SdDataPool,
                                                        std::vector<uint8> & orc_Bytes)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_SdDataPool.c_Lists.size() > 0)
   {
      orc_Bytes.clear();
      orc_Bytes.reserve(orc_SdDataPool.c_Lists[0].u32_NvMSize);

      // insert dummies for CRC bytes
      // do not use list's GetCRCAsBigEndianBlob resp. u32_NvMCRC as this members content would need calculation first
      std::vector<stw_types::uint8> c_CRCDummy;
      c_CRCDummy.resize(2, 0);
      orc_Bytes.insert(orc_Bytes.begin(), c_CRCDummy.begin(), c_CRCDummy.end());

      // insert configured data
      for (uint32 u32_ItElement = 0U;
           (u32_ItElement < orc_SdDataPool.c_Lists[0].c_Elements.size()) && (s32_Retval == C_NO_ERR);
           ++u32_ItElement)
      {
         const C_OSCNodeDataPoolListElement & rc_Element = orc_SdDataPool.c_Lists[0].c_Elements[u32_ItElement];
         if (rc_Element.c_DataSetValues.size() == 1)
         {
            std::vector<stw_types::uint8> c_NewBytes;
            rc_Element.c_DataSetValues[0].GetValueAsBigEndianBlob(c_NewBytes);
            orc_Bytes.insert(orc_Bytes.end(), c_NewBytes.begin(), c_NewBytes.end());
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }

      // make sure that number of bytes equals specified size of list and fill eventually remaining space with zeroes
      orc_Bytes.resize(orc_SdDataPool.c_Lists[0].u32_NvMSize, 0);
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}
