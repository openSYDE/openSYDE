//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SupCreatePackage.hpp"

#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSuServiceUpdatePackage.hpp"
#include "C_OscViewData.hpp"
#include "C_OscSystemDefinitionFiler.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscViewFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in]  oq_Quiet             Quiet option to show less console output
   \param[in]  orc_SupFilePath      Service update package file path
   \param[in]  orc_OsyProjectPath   openSYDE projec path
   \param[in]  orc_ViewName         View name
   \param[in]  orc_DeviceDefPath    Device definitions path
   \param[in]  orc_TempDir          Temporary directory for creating zip package
*/
//----------------------------------------------------------------------------------------------------------------------
C_SupCreatePackage::C_SupCreatePackage(const bool oq_Quiet, const C_SclString & orc_SupFilePath,
                                       const C_SclString & orc_OsyProjectPath, const C_SclString & orc_ViewName,
                                       const C_SclString & orc_DeviceDefPath, const C_SclString & orc_TempDir) :
   mq_Quiet(oq_Quiet),
   mc_SupFilePath(orc_SupFilePath),
   mc_OsyProjectPath(orc_OsyProjectPath),
   mc_ViewName(orc_ViewName),
   mc_DeviceDefPath(orc_DeviceDefPath),
   mc_TempDir(orc_TempDir)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create service update package from given project

   \return
   eERR_CREATE_PROJ_LOAD_FAILED     Could not load project (system definition and/or system views)
   eERR_CREATE_VIEW_NOT_FOUND       Could not find a view with the name provided by user
   eERR_CREATE_ZIP_RD_RW            File writing problems occured on output file creation or deletion
   eERR_CREATE_ZIP_CONFIG           Configuration problems occured on update package creation
                                     (see C_OscSuServiceUpdatePackage::h_CreatePackage for further details)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSup::E_Result C_SupCreatePackage::Create()
{
   C_SydeSup::E_Result e_Result = C_SydeSup::eOK;
   int32_t s32_Return;
   C_SclString c_SysDefPath;
   C_SclString c_SysViewPath;
   C_OscSystemDefinition c_SystemDefinition;
   C_OscViewData c_View;

   std::vector<C_OscViewData> c_SysViews;

   // Check if project file exists
   if (TglFileExists(mc_OsyProjectPath) == false)
   {
      e_Result = C_SydeSup::eERR_CREATE_PROJ_LOAD_FAILED;
   }

   // Load system definition and views
   if (e_Result == C_SydeSup::eOK)
   {
      C_OscSystemFilerUtil::h_AdaptProjectPathToSystemDefinition(mc_OsyProjectPath, c_SysDefPath);
      C_OscSystemFilerUtil::h_AdaptProjectPathToSystemViews(mc_OsyProjectPath, c_SysViewPath);
      s32_Return = C_OscSystemDefinitionFiler::h_LoadSystemDefinitionFile(c_SystemDefinition, c_SysDefPath,
                                                                          mc_DeviceDefPath);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = C_OscViewFiler::h_LoadSystemViewsFile(c_SysViews, c_SysViewPath, c_SystemDefinition.c_Nodes);
      }
      if (s32_Return != C_NO_ERR)
      {
         e_Result = C_SydeSup::eERR_CREATE_PROJ_LOAD_FAILED;
      }
   }

   // Get selected view
   if (e_Result == C_SydeSup::eOK)
   {
      e_Result = m_FindView(c_SysViews, c_View);
   }

   // If service update package already exists: remove it
   if (e_Result == C_SydeSup::eOK)
   {
      if (TglFileExists(mc_SupFilePath) == true)
      {
         if (remove(mc_SupFilePath.c_str()) != 0)
         {
            e_Result = C_SydeSup::eERR_CREATE_ZIP_RD_RW;
         }
      }
   }

   // Collect information and create zip package
   if (e_Result == C_SydeSup::eOK)
   {
      C_SclStringList c_Warnings;
      C_SclString c_Error;
      std::vector<uint8_t> c_NodeActiveFlags = c_View.GetNodeActiveFlags();
      std::vector<C_OscSuSequences::C_DoFlash> c_ApplicationsToWrite;
      uint32_t u32_HighestUpdatePos = 0;
      std::vector<uint32_t> c_NodesUpdateOrder;

      // First: fill applications to write
      this->m_GetUpdatePackage(c_View, c_SystemDefinition, c_ApplicationsToWrite);

      // Second: adapt node active flags (no files -> treat node as inactive)
      tgl_assert(c_NodeActiveFlags.size() == c_ApplicationsToWrite.size());
      for (uint32_t u32_NodeCounter = 0; u32_NodeCounter < c_ApplicationsToWrite.size(); ++u32_NodeCounter)
      {
         if ((c_ApplicationsToWrite[u32_NodeCounter].c_FilesToFlash.size() == 0) &&
             (c_ApplicationsToWrite[u32_NodeCounter].c_FilesToWriteToNvm.size() == 0) &&
             (c_ApplicationsToWrite[u32_NodeCounter].c_PemFile == ""))
         {
            c_NodeActiveFlags[u32_NodeCounter] = 0U;
         }
      }

      // Last: determine node update order
      // Get highest update position (can be higher than node count, e.g. when a node was deleted)
      for (uint32_t u32_NodeCount = 0; u32_NodeCount < c_View.GetAllNodeUpdateInformation().size(); ++u32_NodeCount)
      {
         if ((c_NodeActiveFlags[u32_NodeCount] == 1U) &&
             (c_View.GetAllNodeUpdateInformation()[u32_NodeCount].u32_NodeUpdatePosition > u32_HighestUpdatePos))
         {
            u32_HighestUpdatePos = c_View.GetAllNodeUpdateInformation()[u32_NodeCount].u32_NodeUpdatePosition;
         }
      }
      // Find all used update positions
      for (uint32_t u32_UpdatePos = 0; u32_UpdatePos <= u32_HighestUpdatePos; ++u32_UpdatePos)
      {
         // Search current update position
         for (uint32_t u32_NodeCount = 0; u32_NodeCount < c_View.GetAllNodeUpdateInformation().size(); ++u32_NodeCount)
         {
            if ((c_NodeActiveFlags[u32_NodeCount] == 1U) &&
                (u32_UpdatePos == c_View.GetAllNodeUpdateInformation()[u32_NodeCount].u32_NodeUpdatePosition))
            {
               c_NodesUpdateOrder.push_back(u32_NodeCount);
               // Do not break here! Node update position unfortunately is not unique and falls back to position in
               // system definition node order
            }
         }
      }

      // Now we have all data collected and can create the package
      s32_Return = C_OscSuServiceUpdatePackage::h_CreatePackage(mc_SupFilePath, c_SystemDefinition,
                                                                c_View.GetOscPcData().GetBusIndex(),
                                                                c_NodeActiveFlags, c_NodesUpdateOrder,
                                                                c_ApplicationsToWrite,
                                                                c_Warnings, c_Error, false, true, mc_TempDir);

      // File writing issues
      if ((s32_Return == C_RD_WR) || (s32_Return == C_RANGE) || (s32_Return == C_BUSY))
      {
         e_Result = C_SydeSup::eERR_CREATE_ZIP_RD_RW;
      }
      // Configuration issues
      else if (s32_Return != C_NO_ERR)
      {
         e_Result = C_SydeSup::eERR_CREATE_ZIP_CONFIG;
      }
      else
      {
         // no error, no problem
      }
   }

   return e_Result;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find view in project that maches given view name

   \param[in]   orc_Views  List of system views
   \param[out]  orc_View   System view that matches given view name

   \retval   eOK                          View found and returend in orc_View
   \retval   eERR_CREATE_VIEW_NOT_FOUND   View not found
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSup::E_Result C_SupCreatePackage::m_FindView(const std::vector<C_OscViewData> & orc_Views,
                                                   C_OscViewData & orc_View)
{
   C_SydeSup::E_Result e_Result = C_SydeSup::eOK;
   bool q_ViewFound = false;

   for (std::vector<C_OscViewData>::const_iterator c_ItViews = orc_Views.begin(); c_ItViews < orc_Views.end();
        ++c_ItViews)
   {
      if (c_ItViews->GetName() == mc_ViewName)
      {
         orc_View = *c_ItViews;
         q_ViewFound = true;
      }
   }

   if (q_ViewFound == false)
   {
      e_Result = C_SydeSup::eERR_CREATE_VIEW_NOT_FOUND;
   }

   return e_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get update package containing information for the system update

   \param[in]      orc_View                  System view for which the update package should get created
   \param[in]      orc_SystemDefinition      System definition
   \param[in,out]  orc_ApplicationsToWrite   Node update configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SupCreatePackage::m_GetUpdatePackage(const C_OscViewData & orc_View,
                                            const C_OscSystemDefinition & orc_SystemDefinition,
                                            std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite) const
{
   const std::vector<C_OscViewNodeUpdate> & rc_AllNodesUpdateInfo = orc_View.GetAllNodeUpdateInformation();

   // Check precondition
   tgl_assert(orc_SystemDefinition.c_Nodes.size() == rc_AllNodesUpdateInfo.size());

   // Resize to insert data into vectors with [] operator.
   // Flash data vector must have same size like node count (not only active nodes!) so in fact we fill with dummies
   orc_ApplicationsToWrite.resize(rc_AllNodesUpdateInfo.size(), C_OscSuSequences::C_DoFlash());

   // Collect node update data
   for (uint32_t u32_NodeCounter = 0; u32_NodeCounter < rc_AllNodesUpdateInfo.size(); ++u32_NodeCounter)
   {
      if (orc_View.GetNodeActive(u32_NodeCounter) == true)
      {
         const C_OscViewNodeUpdate & rc_NodeUpdateInfo = rc_AllNodesUpdateInfo[u32_NodeCounter];
         C_OscSuSequences::C_DoFlash & rc_AppsToWrite = orc_ApplicationsToWrite[u32_NodeCounter];
         const C_OscNode & rc_Node = orc_SystemDefinition.c_Nodes[u32_NodeCounter];

         // Fill other known device names
         if (rc_Node.pc_DeviceDefinition != NULL)
         {
            if (rc_Node.u32_SubDeviceIndex < rc_Node.pc_DeviceDefinition->c_SubDevices.size())
            {
               rc_AppsToWrite.c_OtherAcceptedDeviceNames =
                  rc_Node.pc_DeviceDefinition->c_SubDevices[rc_Node.u32_SubDeviceIndex].c_OtherAcceptedNames;
            }
         }

         // Applications: Data Blocks, file based, parameter sets (PSI), PEM
         // Data Blocks
         m_GetDataBlocksToWrite(rc_NodeUpdateInfo, rc_Node, rc_AppsToWrite);

         // File based
         m_GetFileBasedFilesToWrite(rc_NodeUpdateInfo, rc_Node, rc_AppsToWrite);

         // Parameter sets
         m_GetParamSetsToWrite(rc_NodeUpdateInfo, rc_Node, rc_AppsToWrite);

         // PEM File
         if (rc_NodeUpdateInfo.GetPemFilePath() != "")
         {
            if (rc_NodeUpdateInfo.GetSkipUpdateOfPemFile() == false)
            {
               C_OscViewNodeUpdate::E_StateSecurity e_StateSecurity;
               C_OscViewNodeUpdate::E_StateDebugger e_StateDebugger;

               const C_SclString c_AbsolutePath =
                  TglIsRelativePath(rc_NodeUpdateInfo.GetPemFilePath()) ?
                  TglExpandFileName(rc_NodeUpdateInfo.GetPemFilePath(), TglExtractFilePath(mc_OsyProjectPath)) :
                  rc_NodeUpdateInfo.GetPemFilePath();
               rc_AppsToWrite.c_PemFile = c_AbsolutePath;
               // Fill PEM states
               rc_NodeUpdateInfo.GetStates(e_StateSecurity, e_StateDebugger);
               C_OscSuSequences::h_FillDoFlashWithPemStates(e_StateSecurity, e_StateDebugger, rc_AppsToWrite);
               C_SydeSup::h_WriteLog(mc_LoggingCategory,
                                     "For node \"" + rc_Node.c_Properties.c_Name + "\" use PEM file: " +
                                     c_AbsolutePath, false, mq_Quiet);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add Data Block files to write to flash data structure

   Handle dependency to Data Block data (default output path, relativeness to Data Block project, placeholder variables)

   \param[in]   orc_NodeUpdate            Node update data
   \param[in]   orc_Node                  System definition node data
   \param[out]  orc_ApplicationToWrite    Application to write
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SupCreatePackage::m_GetDataBlocksToWrite(const C_OscViewNodeUpdate & orc_NodeUpdate, const C_OscNode & orc_Node,
                                                C_OscSuSequences::C_DoFlash & orc_ApplicationToWrite) const
{
   const std::vector<C_SclString> c_Paths = orc_NodeUpdate.GetPaths(C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
   const std::vector<bool> c_SkipUpdate =
      orc_NodeUpdate.GetSkipUpdateOfPathsFlags(C_OscViewNodeUpdate::eFTP_DATA_BLOCK);

   // use TglExpandFileName to get an absolute path for openSYDE project
   //(else we get issues below with resolving paths from Data Blocks)
   C_SclString c_OsyProjDir =
      TglIsRelativePath(mc_OsyProjectPath) ? TglExpandFileName(mc_OsyProjectPath, "./") : mc_OsyProjectPath;

   c_OsyProjDir = TglExtractFilePath(c_OsyProjDir);

   tgl_assert(c_Paths.size() == c_SkipUpdate.size());
   if (c_Paths.size() == c_SkipUpdate.size())
   {
      uint32_t u32_FileCounter = 0;
      for (uint32_t u32_DataBlockCounter = 0; u32_DataBlockCounter < orc_Node.c_Applications.size();
           ++u32_DataBlockCounter)
      {
         // Resolve placeholder variables and make path absolute
         const C_OscNodeApplication & rc_DataBlock = orc_Node.c_Applications[u32_DataBlockCounter];

         // HALC PSI Data Block output files are handled in PSI file section
         if (rc_DataBlock.e_Type != C_OscNodeApplication::ePARAMETER_SET_HALC)
         {
            const C_SclString c_DbProjPath = rc_DataBlock.c_ProjectPath;
            C_SclString c_RealPath;

            for (uint32_t u32_DatablockFileCounter = 0; u32_DatablockFileCounter < rc_DataBlock.c_ResultPaths.size();
                 ++u32_DatablockFileCounter)
            {
               tgl_assert(u32_FileCounter < c_Paths.size());
               if (u32_FileCounter < c_Paths.size())
               {
                  if (c_SkipUpdate[u32_FileCounter] == false)
                  {
                     if (c_Paths[u32_FileCounter] == "")
                     {
                        // Use path from system definition
                        // (which might be relative to Data Block project -> resolve this)
                        c_RealPath = rc_DataBlock.c_ResultPaths[u32_DatablockFileCounter];
                        c_RealPath =
                           C_OscUtils::h_MakeIndependentOfDbProjectPath(c_DbProjPath, c_OsyProjDir, c_RealPath);
                     }
                     else
                     {
                        // Use path from nodes update information
                        c_RealPath = c_Paths[u32_FileCounter];
                     }

                     // resolve placeholder variables
                     c_RealPath = C_OscUtils::h_ResolvePlaceholderVariables(c_RealPath, c_OsyProjDir, c_DbProjPath);

                     // make path absolute (if relative it is relative to openSYDE project)
                     if (TglIsRelativePath(c_RealPath) == true)
                     {
                        c_RealPath = TglExpandFileName(c_RealPath, c_OsyProjDir);
                     }

                     orc_ApplicationToWrite.c_FilesToFlash.push_back(c_RealPath);
                     C_SydeSup::h_WriteLog(mc_LoggingCategory,
                                           "For node \"" + orc_Node.c_Properties.c_Name + "\" and application \"" +
                                           rc_DataBlock.c_Name + "\" use file: " + c_RealPath, false, mq_Quiet);
                  }
                  // Increment update file counter
                  ++u32_FileCounter;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add file based files to write to flash data structure

   \param[in]   orc_NodeUpdate            Node update data
   \param[in]   orc_Node                  System definition node data
   \param[out]  orc_ApplicationToWrite    Application to write
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SupCreatePackage::m_GetFileBasedFilesToWrite(const C_OscViewNodeUpdate & orc_NodeUpdate,
                                                    const C_OscNode & orc_Node,
                                                    C_OscSuSequences::C_DoFlash & orc_ApplicationToWrite) const
{
   const std::vector<C_SclString> c_Paths = orc_NodeUpdate.GetPaths(C_OscViewNodeUpdate::eFTP_FILE_BASED);
   const std::vector<bool> c_SkipUpdate =
      orc_NodeUpdate.GetSkipUpdateOfPathsFlags(C_OscViewNodeUpdate::eFTP_FILE_BASED);
   const C_SclString c_OsyProjDir = TglExtractFilePath(mc_OsyProjectPath);

   tgl_assert(c_Paths.size() == c_SkipUpdate.size());
   if (c_Paths.size() == c_SkipUpdate.size())
   {
      for (uint32_t u32_FileCounter = 0; u32_FileCounter < c_Paths.size(); ++u32_FileCounter)
      {
         if (c_SkipUpdate[u32_FileCounter] == false)
         {
            // Make path absolute (no placeholder in file based case)
            const C_SclString c_AbsolutePath =
               TglIsRelativePath(c_Paths[u32_FileCounter]) ?
               TglExpandFileName(c_Paths[u32_FileCounter], c_OsyProjDir) : c_Paths[u32_FileCounter];
            orc_ApplicationToWrite.c_FilesToFlash.push_back(c_AbsolutePath);

            C_SydeSup::h_WriteLog(mc_LoggingCategory,
                                  "For node \"" + orc_Node.c_Properties.c_Name + "\" use file: " + c_AbsolutePath,
                                  false, mq_Quiet);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add parameter (PSI files) sets to write to flash data structure

   \param[in]   orc_NodeUpdate            Node update data
   \param[in]   orc_Node                  System definition node data
   \param[out]  orc_ApplicationToWrite    Application to write
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SupCreatePackage::m_GetParamSetsToWrite(const C_OscViewNodeUpdate & orc_NodeUpdate, const C_OscNode & orc_Node,
                                               C_OscSuSequences::C_DoFlash & orc_ApplicationToWrite) const
{
   const std::vector<bool> c_SkipParamUpdate = orc_NodeUpdate.GetSkipUpdateOfParamInfosFlags();
   const std::vector<C_OscViewNodeUpdateParamInfo> c_ParamInfos = orc_NodeUpdate.GetParamInfos();

   tgl_assert(c_ParamInfos.size() == c_SkipParamUpdate.size());
   if (c_ParamInfos.size() == c_SkipParamUpdate.size())
   {
      uint32_t u32_DbFileCounter = 0;
      for (uint32_t u32_ParamInfoCounter = 0; u32_ParamInfoCounter < c_ParamInfos.size(); ++u32_ParamInfoCounter)
      {
         if (c_SkipParamUpdate[u32_ParamInfoCounter] == false)
         {
            C_SclString c_RealPath = c_ParamInfos[u32_ParamInfoCounter].GetPath();
            if (c_RealPath == "")
            {
               // Find corresponding Data Block of type HALC Parameter set. Must be unique.
               for (std::vector<C_OscNodeApplication>::const_iterator c_It = orc_Node.c_Applications.begin();
                    c_It < orc_Node.c_Applications.end(); ++c_It)
               {
                  if (c_It->e_Type == C_OscNodeApplication::ePARAMETER_SET_HALC)
                  {
                     c_RealPath = C_OscUtils::h_MakeIndependentOfDbProjectPath(
                        c_It->c_ProjectPath, mc_OsyProjectPath, c_It->c_ResultPaths[u32_DbFileCounter]);
                     u32_DbFileCounter++;
                     break;
                  }
               }
            }
            // make path absolute and add it to NVM update files
            if (TglIsRelativePath(c_RealPath) == true)
            {
               c_RealPath = TglExpandFileName(c_RealPath, TglExtractFilePath(mc_OsyProjectPath));
            }
            orc_ApplicationToWrite.c_FilesToWriteToNvm.push_back(c_RealPath);

            C_SydeSup::h_WriteLog(mc_LoggingCategory,
                                  "For node \"" + orc_Node.c_Properties.c_Name + "\" use parameter set image file: " +
                                  c_RealPath, false, mq_Quiet);
         }
      }
   }
}
