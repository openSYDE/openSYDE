//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export code of an openSYDE node. (implementation)

   Export code of an openSYDE node

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_OscExportUti.hpp"
#include "C_OscExportNode.hpp"
#include "C_OscExportDataPool.hpp"
#include "C_OscExportCommunicationStack.hpp"
#include "C_OscExportCanOpenConfig.hpp"
#include "C_OscExportCanOpenInit.hpp"
#include "C_OscExportHalc.hpp"
#include "C_OscExportParamSet.hpp"
#include "C_OscExportOsyInit.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create files for one application

   Creates files for one Data Block.

   If the target path orc_Path does not exist the function can create it (also recursively).
   The function will store all files in the folder specified by orc_Path.

   Creates:
   * Datapools
   * comm definition
   * openSYDE server initialization wrapper
   * hardware configuration

   \param[in]   orc_Node               Node as information source for exported code
   \param[in]   ou16_ApplicationIndex  Index of Data Block within orc_Node.c_Applications
   \param[in]   orc_Path               Storage path for created files
   \param[out]  orc_Files              List of all exported file names (with absolute or relative path)
   \param[in]   orc_ExportToolName     Name of calling executable
   \param[in]   orc_ExportToolVersion  Version of calling executable

   \return
   C_NO_ERR  Operation success
   C_RD_WR   Cannot store files
   C_NOACT   Application is not of file generation type or has unknown/invalid code structure version
   C_RANGE   Information which application runs the DPD is invalid or refers to an invalid application
             Datapool does not provide information about owning application or refers to an invalid application
             ApplicationIndex references invalid application
   C_CONFIG  Protocol or Datapool not available in node for interface or
             Input data not suitable for file generation. Details will be written to OSC Log.
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::h_CreateSourceCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                            const C_SclString & orc_Path, std::vector<C_SclString> & orc_Files,
                                            const C_SclString & orc_ExportToolName,
                                            const C_SclString & orc_ExportToolVersion)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Files.clear();

   // check node prerequisites
   if (ou16_ApplicationIndex >= orc_Node.c_Applications.size())
   {
      s32_Retval = C_RANGE;
   }
   else
   {
      s32_Retval = mh_CheckPrerequisites(orc_Node);
   }

   // check application prerequisites
   if (s32_Retval == C_NO_ERR)
   {
      const C_OscNodeApplication & rc_Application = orc_Node.c_Applications[ou16_ApplicationIndex];

      //we only need to create code for programmable applications and PSI Data Blocks
      if ((rc_Application.e_Type != C_OscNodeApplication::ePROGRAMMABLE_APPLICATION) &&
          (rc_Application.e_Type != C_OscNodeApplication::ePARAMETER_SET_HALC))
      {
         osc_write_log_error("Creating files", "File generation for \"" + rc_Application.c_Name + "\" is disabled.");
         s32_Retval = C_NOACT;
      }
      //check if the code structure version is unknown
      else if (rc_Application.u16_GenCodeVersion > C_OscNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
      {
         osc_write_log_error("Creating source code", "Structure version of application \"" +
                             rc_Application.c_Name + "\" unknown.");
         s32_Retval = C_NOACT;
      }
      else
      {
         // no further prerequisites to check
      }
   }

   // create target folder
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscUtils::h_CreateFolderRecursively(orc_Path);
      if (s32_Retval != C_NO_ERR)
      {
         osc_write_log_error("Creating source code", "Could not create target directory \"" + orc_Path + "\".");
         s32_Retval = C_RD_WR;
      }
   }

   if (orc_Node.c_Applications[ou16_ApplicationIndex].e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
   {
      const C_SclString c_ExportToolInfo = orc_ExportToolName + " " + orc_ExportToolVersion;

      // export openSYDE initialization
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_CreateOsyInitCode(orc_Node, ou16_ApplicationIndex, orc_Path, orc_Files, c_ExportToolInfo);
      }

      // export Datapool
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_CreateDatapoolCode(orc_Node, ou16_ApplicationIndex, orc_Path, orc_Files, c_ExportToolInfo);
      }

      // export COMM definition
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_CreateCommStackCode(orc_Node, ou16_ApplicationIndex, orc_Path, orc_Files,
                                             c_ExportToolInfo);
      }

      // export HAL configuration
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_CreateHalConfigCode(orc_Node, ou16_ApplicationIndex, orc_Path, orc_Files, c_ExportToolInfo);
      }
   }
   else
   {
      // export HAL NVM information as parameter set image
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_CreateHalNvmData(orc_Node, ou16_ApplicationIndex, orc_Path, orc_Files, orc_ExportToolName,
                                          orc_ExportToolVersion);
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create openSYDE initialization C code

   \param[in]   orc_Node               Node as information source for exported code
   \param[in]   ou16_ApplicationIndex  Index of Data Block within orc_Node.c_Applications
   \param[in]   orc_Path               Storage path for created files
   \param[out]  orc_Files              List of all exported file names (with absolute or relative path)
   \param[in]   orc_ExportToolInfo     Information about calling executable (name + version)

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store files
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::mh_CreateOsyInitCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                              const C_SclString & orc_Path, std::vector<C_SclString> & orc_Files,
                                              const C_SclString & orc_ExportToolInfo)
{
   int32_t s32_Retval;

   bool q_CreateDpdInit;

   if (ou16_ApplicationIndex == orc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex)
   {
      //create DPD and DPH init functions
      q_CreateDpdInit = true;
   }
   else
   {
      //create DPH init functions
      q_CreateDpdInit = false;
   }

   s32_Retval = C_OscExportOsyInit::h_CreateSourceCode(orc_Path, orc_Node, q_CreateDpdInit,
                                                       ou16_ApplicationIndex, orc_ExportToolInfo);

   //Handle file names
   if (s32_Retval == C_NO_ERR)
   {
      C_OscExportUti::h_CollectFilePaths(orc_Files, orc_Path, C_OscExportOsyInit::h_GetFileName(), true);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create Datapool C code

   \param[in]   orc_Node               Node as information source for exported code
   \param[in]   ou16_ApplicationIndex  Index of Data Block within orc_Node.c_Applications
   \param[in]   orc_Path               Storage path for created files
   \param[out]  orc_Files              List of all exported file names (with absolute or relative path)
   \param[in]   orc_ExportToolInfo     Information about calling executable (name + version)

   \return
   C_NO_ERR Operation success
   C_RD_WR  Cannot store files
   C_CONFIG Input data not suitable for code generation. Details will be written to OSC Log.
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::mh_CreateDatapoolCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                               const C_SclString & orc_Path, std::vector<C_SclString> & orc_Files,
                                               const C_SclString & orc_ExportToolInfo)
{
   int32_t s32_Retval = C_NO_ERR;

   //index of Datapool within this application (as there can be Datapools owned by other applications
   // this value is not identical to the Datapool index within the whole list)
   uint8_t u8_DataPoolIndexWithinApplication = 0U;

   //Export Datapools
   for (uint32_t u32_ItDataPool = 0U; u32_ItDataPool < orc_Node.c_DataPools.size(); ++u32_ItDataPool)
   {
      bool q_Create = false;
      const C_OscNodeDataPool & rc_DataPool = orc_Node.c_DataPools[u32_ItDataPool];
      C_OscExportDataPool::E_Linkage e_Relation = C_OscExportDataPool::eLOCAL;

      //owned by this application ?
      if (rc_DataPool.s32_RelatedDataBlockIndex == ou16_ApplicationIndex)
      {
         e_Relation = C_OscExportDataPool::eLOCAL;
         q_Create = true;
      }
      //if the Datapool is not owned by this application, but has public scope,
      // then make the Datapool known as public remote Datapool
      else if ((orc_Node.c_Applications[ou16_ApplicationIndex].u16_GenCodeVersion >= 4U) &&
               (rc_DataPool.q_ScopeIsPrivate == false))
      {
         e_Relation = C_OscExportDataPool::eREMOTEPUBLIC;
         q_Create = true;
      }
      //if the Datapool is not owned by this application and does not have public scope, but this
      // application runs the protocol driver, then make the Datapool known as remote Datapool
      else if (ou16_ApplicationIndex == orc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex)
      {
         e_Relation = C_OscExportDataPool::eREMOTE;
         q_Create = true;
      }
      else
      {
         //finished here ...
      }

      // check if the datapool is empty (e.g. COM datapool without any signal)
      if (q_Create == true)
      {
         bool q_AtLeastOneElement = false;
         for (uint16_t u16_ListIndex = 0U; u16_ListIndex < rc_DataPool.c_Lists.size(); u16_ListIndex++)
         {
            const C_OscNodeDataPoolList & rc_List = rc_DataPool.c_Lists[u16_ListIndex];
            if (rc_List.c_Elements.size() != 0)
            {
               q_AtLeastOneElement = true;
               break;
            }
         }

         // do not create files for empty datapools
         if (q_AtLeastOneElement == false)
         {
            q_Create = false;
         }
      }

      if (q_Create == true)
      {
         //create source code
         const uint16_t u16_GenCodeVersion = orc_Node.c_Applications[ou16_ApplicationIndex].u16_GenCodeVersion;
         uint8_t u8_ProcessId;
         uint8_t u8_DataPoolIndexRemote;

         //we need the process ID of the owner process and the Datapool index within the owning application
         if (e_Relation == C_OscExportDataPool::eLOCAL)
         {
            //all your base are belong to us
            u8_ProcessId = orc_Node.c_Applications[ou16_ApplicationIndex].u8_ProcessId;
            u8_DataPoolIndexRemote = 0U; //does not really matter
         }
         else
         {
            u8_ProcessId = orc_Node.c_Applications[rc_DataPool.s32_RelatedDataBlockIndex].u8_ProcessId;
            //get index of Datapool within the remote process:
            u8_DataPoolIndexRemote = 0U;
            for (uint8_t u8_DataPool = 0U; u8_DataPool < u32_ItDataPool; u8_DataPool++)
            {
               if (orc_Node.c_DataPools[u8_DataPool].s32_RelatedDataBlockIndex ==
                   rc_DataPool.s32_RelatedDataBlockIndex)
               {
                  u8_DataPoolIndexRemote++;
               }
            }
         }

         if (rc_DataPool.e_Type == C_OscNodeDataPool::eCOM)
         {
            C_OscNodeDataPool c_DataPool;
            //Patch Datapool to include message names
            s32_Retval = C_OscExportNode::mh_GetAdaptedComDataPool(orc_Node, u32_ItDataPool, c_DataPool);
            tgl_assert(s32_Retval == C_NO_ERR);

            //Export
            s32_Retval = C_OscExportDataPool::h_CreateSourceCode(orc_Path, u16_GenCodeVersion,
                                                                 orc_Node.c_Properties.c_CodeExportSettings.
                                                                 e_ScalingSupport, c_DataPool,
                                                                 u8_DataPoolIndexWithinApplication,
                                                                 e_Relation, u8_DataPoolIndexRemote,
                                                                 u8_ProcessId, orc_ExportToolInfo);
         }
         else
         {
            //Export
            s32_Retval = C_OscExportDataPool::h_CreateSourceCode(orc_Path, u16_GenCodeVersion,
                                                                 orc_Node.c_Properties.c_CodeExportSettings.
                                                                 e_ScalingSupport, rc_DataPool,
                                                                 u8_DataPoolIndexWithinApplication,
                                                                 e_Relation, u8_DataPoolIndexRemote,
                                                                 u8_ProcessId, orc_ExportToolInfo);
         }
         //Handle file names
         if (s32_Retval == C_NO_ERR)
         {
            C_OscExportUti::h_CollectFilePaths(orc_Files, orc_Path,
                                               C_OscExportDataPool::h_GetFileName(rc_DataPool), true);
         }
         else
         {
            break;
         }
         u8_DataPoolIndexWithinApplication++;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create COMM stack C code

   \param[in]   orc_Node               Node as information source for exported code
   \param[in]   ou16_ApplicationIndex  Index of Data Block within orc_Node.c_Applications
   \param[in]   orc_Path               Storage path for created files
   \param[out]  orc_Files              List of all exported file names (with absolute or relative path)
   \param[in]   orc_ExportToolInfo     Information about calling executable (name + version)

   \return
   C_NO_ERR    Operation success
   C_NOACT     Application is not of type ePROGRAMMABLE_APPLICATION or has unknown code structure version
   C_RD_WR     Cannot store files
   C_CONFIG    Protocol or Datapool not available in node for interface or application index out of range
   C_RANGE     Application index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::mh_CreateCommStackCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                                const C_SclString & orc_Path, std::vector<C_SclString> & orc_Files,
                                                const C_SclString & orc_ExportToolInfo)
{
   int32_t s32_Retval = C_NO_ERR;

   for (uint32_t u32_ItProtocol = 0U; u32_ItProtocol < orc_Node.c_ComProtocols.size(); ++u32_ItProtocol)
   {
      const C_OscCanProtocol & rc_Protocol = orc_Node.c_ComProtocols[u32_ItProtocol];

      //logic: C_OscCanProtocol refers to a Datapool; if that Datapool is owned by the application than create code
      if (orc_Node.c_DataPools[rc_Protocol.u32_DataPoolIndex].s32_RelatedDataBlockIndex == ou16_ApplicationIndex)
      {
         std::vector<uint8_t> c_IfWithCanOpenManager;
         for (uint32_t u32_ItInterface = 0U; u32_ItInterface < rc_Protocol.c_ComMessages.size();
              ++u32_ItInterface)
         {
            //handle special case CANopen
            //For CANopen we allow to have configuration with just the manager but no assigned devices
            // (and therefore no messages). "c_ComMessages" lists all interfaces.
            //So we create code for all of those interfaces where a manager is active:
            if ((rc_Protocol.e_Type == C_OscCanProtocol::eCAN_OPEN) &&
                (orc_Node.c_CanOpenManagers.count(static_cast<uint8_t>(u32_ItInterface)) != 0))
            {
               s32_Retval = C_OscExportCanOpenConfig::h_CreateSourceCode(orc_Path, orc_Node,
                                                                         ou16_ApplicationIndex,
                                                                         static_cast<uint8_t>(u32_ItInterface),
                                                                         rc_Protocol.u32_DataPoolIndex,
                                                                         orc_ExportToolInfo);
               //handle file names
               if (s32_Retval == C_NO_ERR)
               {
                  C_OscExportUti::h_CollectFilePaths(
                     orc_Files, orc_Path,
                     C_OscExportCanOpenConfig::h_GetFileName(static_cast<uint8_t>(u32_ItInterface)),
                     true);
               }

               c_IfWithCanOpenManager.push_back(static_cast<uint8_t>(u32_ItInterface));
            }
            //all other COMM protocol types remain as COMMStack export
            else
            {
               //only if at least one message is defined:
               const C_OscCanMessageContainer & rc_ComMessageContainer = rc_Protocol.c_ComMessages[u32_ItInterface];
               if ((rc_ComMessageContainer.c_TxMessages.size() > 0) ||
                   (rc_ComMessageContainer.c_RxMessages.size() > 0))
               {
                  s32_Retval =
                     C_OscExportCommunicationStack::h_CreateSourceCode(orc_Path, orc_Node, ou16_ApplicationIndex,
                                                                       static_cast<uint8_t>(u32_ItInterface),
                                                                       rc_Protocol.u32_DataPoolIndex,
                                                                       rc_Protocol.e_Type, orc_ExportToolInfo);
                  //Handle file names
                  if (s32_Retval == C_NO_ERR)
                  {
                     C_OscExportUti::h_CollectFilePaths(orc_Files, orc_Path,
                                                        C_OscExportCommunicationStack::h_GetFileName(
                                                           static_cast<uint8_t>(u32_ItInterface),
                                                           rc_Protocol.e_Type), true);
                  }
               }
            }
            if (s32_Retval != C_NO_ERR)
            {
               break;
            }
         }
         if (c_IfWithCanOpenManager.size() > 0)
         {
            s32_Retval = C_OscExportCanOpenInit::h_CreateSourceCode(orc_Path, orc_Node, c_IfWithCanOpenManager,
                                                                    orc_ExportToolInfo);
            //handle file names
            if (s32_Retval == C_NO_ERR)
            {
               C_OscExportUti::h_CollectFilePaths(orc_Files, orc_Path, C_OscExportCanOpenInit::h_GetFileName(), true);
            }
         }
         if (s32_Retval != C_NO_ERR)
         {
            break;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create HAL configuration C code

   \param[in]   orc_Node               Node as information source for exported code
   \param[in]   ou16_ApplicationIndex  Index of Data Block within orc_Node.c_Applications
   \param[in]   orc_Path               Storage path for created files
   \param[out]  orc_Files              List of all exported file names (with absolute or relative path)
   \param[in]   orc_ExportToolInfo     Information about calling executable (name + version)

   \return
   C_NO_ERR    Operation success
   C_RD_WR     Problems accessing file system
   C_NOACT     Datapool is not of type HALC or HALC NVM
   C_RANGE     Invalid internal parameter or file already exists
   C_CONFIG    Internal data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::mh_CreateHalConfigCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                                const C_SclString & orc_Path, std::vector<C_SclString> & orc_Files,
                                                const C_SclString & orc_ExportToolInfo)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Node.c_HalcConfig.IsClear() == false)
   {
      std::map<bool, int32_t> c_HalDataPools;

      // Get HAL Datapool indices for safe and for non-safe Datapool
      s32_Retval = mh_GetHalDataPoolIndices(orc_Node, c_HalDataPools);

      if (s32_Retval == C_NO_ERR)
      {
         std::map<bool, int32_t>::const_iterator c_ItHalDataPools;

         // Generate HALC files corresponding to safe and to non-safe Datapool
         for (c_ItHalDataPools = c_HalDataPools.begin(); c_ItHalDataPools != c_HalDataPools.end(); ++c_ItHalDataPools)
         {
            if (c_ItHalDataPools->second >= 0) // no safe resp. non-safe Datapool -> ok, no HALC file generation
            {
               const C_OscNodeDataPool & rc_HalDataPool = orc_Node.c_DataPools[c_ItHalDataPools->second];

               //HALC Datapool owned by this application?
               if (rc_HalDataPool.s32_RelatedDataBlockIndex == ou16_ApplicationIndex)
               {
                  s32_Retval =
                     C_OscExportHalc::h_CreateSourceCode(
                        orc_Path, orc_Node.c_Applications[ou16_ApplicationIndex].u16_GenCodeVersion,
                        orc_Node.c_HalcConfig, rc_HalDataPool, orc_ExportToolInfo);

                  //Handle file names
                  if (s32_Retval == C_NO_ERR)
                  {
                     const C_SclString c_FileName = C_OscExportHalc::h_GetFileName(rc_HalDataPool.q_IsSafety);

                     C_OscExportUti::h_CollectFilePaths(orc_Files, orc_Path, c_FileName, true);
                  }
                  else
                  {
                     break;
                  }
               }
            }
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create HALC NVM configuration as parameter set image.

   \param[in]   orc_Node               Node as information source for exported code
   \param[in]   ou16_ApplicationIndex  Index of Data Block within orc_Node.c_Applications
   \param[in]   orc_Path               Storage path for created files
   \param[out]  orc_Files              List of all exported file names (with absolute or relative path)
   \param[in]   orc_ExportToolName     Name of calling executable
   \param[in]   orc_ExportToolVersion  Version of calling executable

   \return
   C_NO_ERR    Success
   C_NOACT     HALC is not configured at all
   C_RD_WR     Problems accessing file system
   C_CONFIG    Internal data invalid (e.g. incorrect number of lists or datasets in HALC NVM Datapool)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::mh_CreateHalNvmData(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                             const C_SclString & orc_Path, std::vector<C_SclString> & orc_Files,
                                             const C_SclString & orc_ExportToolName,
                                             const C_SclString & orc_ExportToolVersion)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Node.c_HalcConfig.IsClear() == false)
   {
      s32_Retval = C_OscExportParamSet::h_CreateParameterSetImage(orc_Path, orc_Node, ou16_ApplicationIndex,
                                                                  orc_Files, orc_ExportToolName, orc_ExportToolVersion);
   }
   else
   {
      s32_Retval = C_NOACT;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for correct preconditions for creating source code

   Checks for:
      - valid application running DPD (only if not NVM based HALC node)
      - valid related application index for all affected Datapools

   Note: As we use the system definition without device definition here, we can not check for programmable flag
         and need to use NVM-based-HALC flag instead.

   \param[in]  orc_Node    Node as information source for exported code

   \return
   C_NO_ERR  Node configuration is OK
   C_RANGE   Information which application runs the DPD is invalid or refers to an invalid application
             Datapool does not provide information about owning application or refers to an invalid application
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::mh_CheckPrerequisites(const C_OscNode & orc_Node)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Node.c_HalcConfig.q_NvmBasedConfig == false)
   {
      //valid application running DPD ?
      if ((orc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex == -1) ||
          (orc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex >=
           static_cast<int16_t>(orc_Node.c_Applications.size())))
      {
         osc_write_log_error("Creating source code",
                             "Invalid definition of application running the diagnostic protocol driver.");
         s32_Retval = C_RANGE;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      //Valid related application index ?
      for (uint8_t u8_DataPool = 0U; u8_DataPool < orc_Node.c_DataPools.size(); u8_DataPool++)
      {
         const C_OscNodeDataPool & rc_CurDataPool = orc_Node.c_DataPools[u8_DataPool];
         if ((rc_CurDataPool.s32_RelatedDataBlockIndex == -1) ||
             (rc_CurDataPool.s32_RelatedDataBlockIndex >= static_cast<int32_t>(orc_Node.c_Applications.size())))
         {
            if ((orc_Node.c_HalcConfig.q_NvmBasedConfig == false) ||
                ((orc_Node.c_HalcConfig.q_NvmBasedConfig == true) &&
                 (rc_CurDataPool.e_Type == C_OscNodeDataPool::eHALC_NVM)))
            {
               osc_write_log_error("Creating source code",
                                   "Invalid definition of owner application for Datapool \"" +
                                   rc_CurDataPool.c_Name + "\".");
               s32_Retval = C_RANGE;
               break;
            }
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt com Datapool elements

   \param[in]   orc_Node            Current node containing Datapools
   \param[in]   ou32_DataPoolIndex  Datapool index within Datapools owned by node
   \param[out]  orc_DataPool        Copy of Datapool with adapted names

   \return
   C_NO_ERR Operation successful
   C_RANGE  Datapool index or type invalid
   C_CONFIG Protocol not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::mh_GetAdaptedComDataPool(const C_OscNode & orc_Node, const uint32_t ou32_DataPoolIndex,
                                                  C_OscNodeDataPool & orc_DataPool)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DataPoolIndex < orc_Node.c_DataPools.size())
   {
      orc_DataPool = orc_Node.c_DataPools[ou32_DataPoolIndex];

      s32_Retval = C_OscExportNode::mh_AdaptComDataPool(orc_Node, ou32_DataPoolIndex, orc_DataPool);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt com Datapool elements

   \param[in]     orc_Node            Current node containing Datapools
   \param[in]     ou32_DataPoolIndex  Datapool index within Datapools owned by node
   \param[in,out] orc_DataPool        Datapool to update with adapted names

   \return
   C_NO_ERR Operation successful
   C_RANGE  Datapool index or type invalid
   C_CONFIG Protocol not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::mh_AdaptComDataPool(const C_OscNode & orc_Node, const uint32_t ou32_DataPoolIndex,
                                             C_OscNodeDataPool & orc_DataPool)
{
   int32_t s32_Retval = C_NO_ERR;

   if ((ou32_DataPoolIndex < orc_Node.c_DataPools.size()) &&
       (orc_DataPool.e_Type == C_OscNodeDataPool::eCOM))
   {
      bool q_Found = false;
      //Search protocol
      for (uint32_t u32_ItProtocol = 0; u32_ItProtocol < orc_Node.c_ComProtocols.size(); ++u32_ItProtocol)
      {
         const C_OscCanProtocol & rc_Protocol = orc_Node.c_ComProtocols[u32_ItProtocol];
         if (rc_Protocol.u32_DataPoolIndex == ou32_DataPoolIndex)
         {
            q_Found = true;
            //For each interface
            for (uint32_t u32_ItMessageContainer = 0; u32_ItMessageContainer < rc_Protocol.c_ComMessages.size();
                 ++u32_ItMessageContainer)
            {
               const C_OscCanMessageContainer & rc_MessageContainer =
                  rc_Protocol.c_ComMessages[u32_ItMessageContainer];
               //For each direction
               for (uint8_t u8_Toggle = 0; u8_Toggle < 2; ++u8_Toggle)
               {
                  const bool q_IsTx = (u8_Toggle == 0) ? true : false;
                  //Get matching list
                  C_OscNodeDataPoolList * const pc_List =
                     C_OscCanProtocol::h_GetComList(orc_DataPool, u32_ItMessageContainer, q_IsTx);
                  const std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(q_IsTx);
                  //For each message
                  for (uint32_t u32_ItMessage = 0; u32_ItMessage < rc_Messages.size(); ++u32_ItMessage)
                  {
                     const C_OscCanMessage & rc_Message = rc_Messages[u32_ItMessage];
                     //For each signal
                     for (uint32_t u32_ItSignal = 0; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
                     {
                        const C_OscCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
                        if (rc_Signal.u32_ComDataElementIndex < pc_List->c_Elements.size())
                        {
                           //Combine message and signal name
                           C_OscNodeDataPoolListElement & rc_Element =
                              pc_List->c_Elements[rc_Signal.u32_ComDataElementIndex];
                           rc_Element.c_Name = rc_Message.c_Name + '_' + rc_Element.c_Name;
                        }
                     }
                  }
               }
            }
            break;
         }
      }
      if (q_Found == false)
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get indices of first safe and non-safe HAL Datapool

   \param[in]   orc_Node            Node as information source for exported code
   \param[out]  orc_HalcDataPools   Corresponding data pools
                                    true = safe; false = non-safe; integer is corresponding Datapool index

   \return
   C_NO_ERR Operation success
   C_CONFIG Safety mode of HALC and existence of safe/non-safe Datapool do not match
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportNode::mh_GetHalDataPoolIndices(const C_OscNode & orc_Node, std::
                                                  map<bool, int32_t> & orc_HalcDataPools)
{
   int32_t s32_Retval = C_NO_ERR;

   // Reset
   orc_HalcDataPools[true] = -1;
   orc_HalcDataPools[false] = -1;

   // Get indices of first safe and first non-safe Datapool
   for (uint32_t u32_ItDataPool = 0U; u32_ItDataPool < orc_Node.c_DataPools.size(); ++u32_ItDataPool)
   {
      const C_OscNodeDataPool & rc_DataPool = orc_Node.c_DataPools[u32_ItDataPool];
      if ((rc_DataPool.e_Type == C_OscNodeDataPool::eHALC) || (rc_DataPool.e_Type == C_OscNodeDataPool::eHALC_NVM))
      {
         if (orc_HalcDataPools[rc_DataPool.q_IsSafety] == -1)
         {
            orc_HalcDataPools[rc_DataPool.q_IsSafety] = static_cast<int32_t>(u32_ItDataPool);
         }
      }
   }

   // Plausibility check against HALC configuration's safety mode
   if (orc_Node.c_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eONE_LEVEL_ALL_NON_SAFE)
   {
      if (orc_HalcDataPools[true] >= 0)
      {
         s32_Retval = C_CONFIG;
      }
   }
   else if (orc_Node.c_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eONE_LEVEL_ALL_SAFE)
   {
      if (orc_HalcDataPools[false] >= 0)
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      if (orc_Node.c_HalcConfig.IsClear() == false)
      {
         if ((orc_HalcDataPools[true] < 0) || (orc_HalcDataPools[false] < 0))
         {
            s32_Retval = C_CONFIG;
         }
      }
   }

   if (s32_Retval != C_NO_ERR)
   {
      osc_write_log_error("Creating source code",
                          "Safety mode of HALC definition does not match safety settings of HALC Datapools.");
   }

   return s32_Retval;
}
