//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: X-Config Package loading

   openSYDE: X-Config Package loading

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscXcoLoad.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscXcoManifestFiler.hpp"
#include "C_OscSpaServicePackageLoadUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::scl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw::scl::C_SclString C_OscXcoLoad::mhc_USE_CASE = "Unpacking X-Config Package";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Unpack an X-Config package

   We have to unzip the package persistent to target folder on disk (orc_TargetUnzipPath) because we need
   the application files for update. If orc_TargetUnzipPath folder exists already it will be deleted.
   Will create the target folder (recursively if it needs to be).

   Result of function after successful execution:
   * unzipped package at given target path with the following contents
   * content of manifest file as output parameter

   Assumptions:
   * valid and compatible X-Config package created by C_OscXcoCreate::h_CreatePackage function
   * write permission of target path for unzipped package

   \param[in]      orc_PackagePath           full path of package
                                             (with constant extension ".syde_xcfg", e.g. 012345.syde_xcfg)
   \param[in]      orc_TargetUnzipPath       folder of path to unzip the service update package
                                             (works with or without trailing path delimiter)
   \param[in,out]  orc_Manifest              Manifest
   \param[out]     orc_SystemDefinitionPath  path to unpacked System Definition (.syde_sysdef file)
   \param[out]     orc_DeviceDefinitionPath  path to unpacked Device Definitions (devices.ini file)
   \param[out]     orc_WarningMessages       warning messages for imported package (empty list if no warnings)
   \param[out]     orc_ErrorMessage          error message in case of failure (empty string if no error)

   \return
   C_NO_ERR    success
   C_CONFIG    could not find package archive or package directory
   C_RD_WR     could not unzip package from disk to target path
   C_BUSY      could not erase pre-existing target path (note: can result in partially erased target path)
   C_RANGE     error code of a called core function (should not occur for valid and compatible update package)
   C_NOACT     error code of a called core function (should not occur for valid and compatible update package)
   C_OVERFLOW  error code of a called core function (should not occur for valid and compatible update package)
   C_DEFAULT   error code of a called core function (should not occur for valid and compatible update package)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXcoLoad::h_ProcessPackage(const C_SclString & orc_PackagePath, const C_SclString & orc_TargetUnzipPath,
                                       C_OscXcoManifest & orc_Manifest, C_SclString & orc_SystemDefinitionPath,
                                       C_SclString & orc_DeviceDefinitionPath, C_SclStringList & orc_WarningMessages,
                                       C_SclString & orc_ErrorMessage)
{
   int32_t s32_Return;

   const stw::scl::C_SclString c_TargetUnzipPath = C_OscSpaServicePackageLoadUtil::h_GetUnzipPath(
      orc_TargetUnzipPath);

   mh_Init();

   //check parameters and perform unzip action
   s32_Return = mh_CheckParamsToProcessPackage(orc_PackagePath, c_TargetUnzipPath);
   if (s32_Return == C_NO_ERR)
   {
      // report system definition and device definition paths
      orc_SystemDefinitionPath = c_TargetUnzipPath + TglFileIncludeTrailingDelimiter(hc_XCFG_SYSDEF_FOLDER) +
                                 hc_XCFG_SYSDEF;
      orc_DeviceDefinitionPath = c_TargetUnzipPath + TglFileIncludeTrailingDelimiter(hc_INI_DEV_FOLDER) + hc_INI_DEV;

      const stw::scl::C_SclString c_ManifestPath = c_TargetUnzipPath + C_OscXcoManifestFiler::hc_FILE_NAME;
      s32_Return = C_OscXcoManifestFiler::h_LoadFile(orc_Manifest, c_ManifestPath);
   }

   mh_GetWarningsAndErrors(orc_WarningMessages, orc_ErrorMessage);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check params to process package

   \param[in]      orc_PackagePath        (see function h_ProcessPackage)
   \param[in,out]  orc_TargetUnzipPath    (see function h_ProcessPackage)

   \return
   STW error codes

   \retval   C_NO_ERR   success
   \retval   C_CONFIG   could not find update package archive or update package directory
   \retval   C_BUSY     could not erase pre-existing target path (note: can result in partially erased target path)
   \retval   C_RD_WR    could not unzip update package from disk to target path
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXcoLoad::mh_CheckParamsToProcessPackage(const stw::scl::C_SclString & orc_PackagePath,
                                                     const stw::scl::C_SclString & orc_TargetUnzipPath)
{
   int32_t s32_Return = C_OscSpaServicePackageLoadUtil::h_CheckParamsToProcessZipPackage(orc_PackagePath,
                                                                                         orc_TargetUnzipPath,
                                                                                         mhc_USE_CASE,
                                                                                         mhc_ErrorMessage);

   //check if all files are present
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = mh_CheckXcfgFiles(orc_TargetUnzipPath);
      if (s32_Return != C_NO_ERR)
      {
         mhc_ErrorMessage = "Could not find necessary files within \"" + orc_TargetUnzipPath +
                            "\" for update package to be complete.";
         osc_write_log_error(mhc_USE_CASE, mhc_ErrorMessage);
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  This function shall check if certain files are existent in the X-Config package. (internal function)

   The update package will be a created directory after unpacking a ".syde_xcfg" file,
   In order to successfully import we need to make sure that
   whether the "syde_xcfg" got "corrupted". There are some defensive checks for files which are
   necessary and if those are missing, we want to handle this gracefully.
   Files we need to make sure are present:
      * ".syde_sysdef" //the expected name MUST NOT be changed
      * ".syde_pkg" //the expected name MUST NOT be changed
      * devices.ini    //the expected name MUST NOT be changed
   Those are the absolute basics we need. This is an all or nothing approach. If one file is missing -> fail.

   This function does not claim to be 100% defensive. We surely could go more into detail regarding the checks for
   certain files, but for now it looks like the existing engines handle everything gracefully.

   \param[in]  orc_PackagePath   path to unzipped package folder

   \retval   C_NO_ERR   directory contains all necessary files
   \retval   C_DEFAULT  at least one file is missing in given directory
                        (due to lack of alternatives C_DEFAULT was chosen to have a unique error to redirect to
                         tool specific error codes)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXcoLoad::mh_CheckXcfgFiles(const stw::scl::C_SclString & orc_PackagePath)
{
   int32_t s32_Return;

   std::vector<stw::scl::C_SclString> c_NecessaryFilesTop;    //those are the files we look for
   std::vector<stw::scl::C_SclString> c_NecessaryFilesSysDef; //those are the files we look for
   std::vector<stw::scl::C_SclString> c_NecessaryFilesDevDef; //those are the files we look for

   c_NecessaryFilesTop.push_back(C_OscXcoManifestFiler::hc_FILE_NAME); //".syde_pkg"
   c_NecessaryFilesSysDef.push_back(hc_XCFG_SYSDEF);                   //".syde_sysdef"
   c_NecessaryFilesDevDef.push_back(hc_INI_DEV);                       //"devices.ini"

   s32_Return = C_OscSpaServicePackageLoadUtil::h_SearchFilesInPath(orc_PackagePath, c_NecessaryFilesTop);
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = C_OscSpaServicePackageLoadUtil::h_SearchFilesInPath(orc_PackagePath + hc_XCFG_SYSDEF_FOLDER,
                                                                       c_NecessaryFilesSysDef);
   }
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = C_OscSpaServicePackageLoadUtil::h_SearchFilesInPath(orc_PackagePath + hc_INI_DEV_FOLDER,
                                                                       c_NecessaryFilesDevDef);
   }

   return s32_Return;
}
