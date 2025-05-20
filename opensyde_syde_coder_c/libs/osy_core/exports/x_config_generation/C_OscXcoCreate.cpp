//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: X-Config Package creation

   openSYDE: X-Config Package creation

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OscXcoCreate.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscXcoManifestFiler.hpp"
#include "C_OscSpaServicePackageCreateUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const C_SclString C_OscXcoCreate::mhc_USE_CASE = "Creating X-Config Package";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create an X-Config package

   Result of function after successful execution:
   * zipped X-Config package with the following contents:
       * system definition file of current view ("xcfg_system_definition.syde_sysdef")
       * devices.ini file
       * device definition files of all devices in current system definition of view

   Function is temporarily creating a folder to save files for zip archive.
   If the optional parameter orc_TemporaryDirectory is empty, this temporary folder is
   created next to package path. Else it is created as subfolder of the provided temporary directory.
   In any case it is tried to delete the temporary folder at the end of this function.
   The temporary folder has the extension ".syde_pkg_tmp" and must not exist already.

   Assumptions:
   * write permission to target folder

   \param[in]   orc_PackagePath           full path of target package (zip archive)
                                          (with constant extension ".syde_pkg", e.g. "C:\\012345.syde_pkg")
   \param[in]   orc_SystemDefinition      Current system definition (class instance - no file!)
   \param[in]   orc_Manifest              Manifest
   \param[out]  orc_WarningMessages       warning messages for created package (empty list if no warnings)
   \param[out]  orc_ErrorMessage          error message in case of failure (empty string if no error)
   \param[in]   orc_TemporaryDirectory    Optional parameter for temporary directory. If empty, temporary directory
                                          is created next to package path.

   \return
   C_NO_ERR    success
   C_RANGE     target file already exists
               target directory for package does not exist
               invalid package name with no package extension ".syde_pkg"
               temporary folder (orc_PackagePath with mc_PACKAGE_EXT_TMP) already exists
   C_NOACT     active node index is not in system definition
   C_RD_WR     could not create temporary folders
               could not save system definition file
               could not save device definition file
   C_BUSY      could not package result to zip archive
               could not delete temporary result folder
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXcoCreate::h_CreatePackage(const stw::scl::C_SclString & orc_PackagePath,
                                        const C_OscSystemDefinition & orc_SystemDefinition,
                                        const C_OscXcoManifest & orc_Manifest,
                                        stw::scl::C_SclStringList & orc_WarningMessages,
                                        stw::scl::C_SclString & orc_ErrorMessage,
                                        const stw::scl::C_SclString & orc_TemporaryDirectory)
{
   int32_t s32_Return;

   bool q_TemporaryFolderCreated = false; // for cleanup at the end of this function

   mh_Init();

   C_SclString c_PackagePathTmp;                           // temporary package path before
                                                           // creating zip archive
   const C_SclString c_TargetZipArchive = orc_PackagePath; // complete path of target zip
                                                           // archive
   std::set<stw::scl::C_SclString> c_XcfgFiles;            // unique container with
                                                           // relative file paths for zip
                                                           // archive
   // fill with constant file names
   c_XcfgFiles.insert(TglFileIncludeTrailingDelimiter(hc_INI_DEV_FOLDER) + hc_INI_DEV);
   c_XcfgFiles.insert(TglFileIncludeTrailingDelimiter(hc_XCFG_SYSDEF_FOLDER) + hc_XCFG_SYSDEF);
   c_XcfgFiles.insert(C_OscXcoManifestFiler::hc_FILE_NAME);
   // precondition checks
   s32_Return = mh_CheckParamsToCreatePackage(orc_PackagePath, orc_SystemDefinition, orc_Manifest);

   if (s32_Return == C_NO_ERR)
   {
      std::vector<stw::scl::C_SclString> c_AllStaticSubFolders;
      c_AllStaticSubFolders.push_back(hc_XCFG_SYSDEF_FOLDER);
      c_AllStaticSubFolders.push_back(hc_INI_DEV_FOLDER);
      s32_Return = C_OscSpaServicePackageCreateUtil::h_CreateTempFolderAndSubFolders(orc_PackagePath,
                                                                                     orc_TemporaryDirectory,
                                                                                     mhc_USE_CASE,
                                                                                     hc_PACKAGE_EXT,
                                                                                     hc_PACKAGE_EXT_TMP,
                                                                                     c_AllStaticSubFolders,
                                                                                     c_PackagePathTmp,
                                                                                     mhc_ErrorMessage);

      q_TemporaryFolderCreated = true; // at least partly
   }
   if (s32_Return == C_NO_ERR)
   {
      const C_SclString c_ManifestPath = c_PackagePathTmp + C_OscXcoManifestFiler::hc_FILE_NAME;
      s32_Return = C_OscXcoManifestFiler::h_SaveFile(orc_Manifest, c_ManifestPath);
      if (s32_Return != C_NO_ERR)
      {
         // very strange! normally the precondition check should
         // guarantee a correct behavior of h_SaveFile
         mhc_ErrorMessage = "Could not create temporary file \"" +
                            c_ManifestPath + "\".";
         osc_write_log_error(mhc_USE_CASE, mhc_ErrorMessage);
         s32_Return = C_RD_WR; // redefine because we only have a few error codes
      }
   }

   // * system definition file
   if (s32_Return == C_NO_ERR)
   {
      const stw::scl::C_SclString c_SysDefPath = TglFileIncludeTrailingDelimiter(
         c_PackagePathTmp + hc_XCFG_SYSDEF_FOLDER);

      s32_Return = C_OscSpaServicePackageCreateUtil::h_SaveSystemDefinition(orc_SystemDefinition,
                                                                            hc_XCFG_SYSDEF,
                                                                            mhc_USE_CASE,
                                                                            c_SysDefPath, TglFileIncludeTrailingDelimiter(
                                                                               hc_XCFG_SYSDEF_FOLDER), c_XcfgFiles,
                                                                            mhc_ErrorMessage);
   }
   // device.ini and device definition files
   if (s32_Return == C_NO_ERR)
   {
      const stw::scl::C_SclString c_DevDefPath = TglFileIncludeTrailingDelimiter(c_PackagePathTmp + hc_INI_DEV_FOLDER);

      s32_Return = C_OscSpaServicePackageCreateUtil::h_SaveDeviceDefinitionsAndIni(
         orc_SystemDefinition, mhc_USE_CASE,
         c_DevDefPath, TglFileIncludeTrailingDelimiter(hc_INI_DEV_FOLDER), c_XcfgFiles, mhc_ErrorMessage);
   }
   // package temporary result folder to zip file
   if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
   {
      s32_Return = C_OscSpaServicePackageCreateUtil::h_CreateZip(mhc_USE_CASE, c_PackagePathTmp,
                                                                 c_TargetZipArchive, c_XcfgFiles,
                                                                 mhc_ErrorMessage);
   }

   // cleanup: delete temporary result folder
   if (q_TemporaryFolderCreated == true)
   {
      C_OscSpaServicePackageCreateUtil::h_CleanUpTempFolder("Creating Update Package",
                                                            c_PackagePathTmp, s32_Return, mhc_ErrorMessage);
   }

   mh_GetWarningsAndErrors(orc_WarningMessages, orc_ErrorMessage);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Precondition checks for creating update package (internal function).

   \param[in]  orc_PackagePath         (see function h_CreatePackage)
   \param[in]  orc_SystemDefinition    (see function h_CreatePackage)
   \param[in]  orc_Manifest            Manifest

   \return
   C_NO_ERR    success
   C_RANGE     target package file already exists
               target directory for package does not exist
               invalid package name with no package extension ".syde_pkg"
   C_NOACT     active node index is not in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXcoCreate::mh_CheckParamsToCreatePackage(const C_SclString & orc_PackagePath,
                                                      const C_OscSystemDefinition & orc_SystemDefinition,
                                                      const C_OscXcoManifest & orc_Manifest)
{
   int32_t s32_Return = C_OscSpaServicePackageCreateUtil::h_CheckPackagePathParam(orc_PackagePath,
                                                                                  mhc_USE_CASE,
                                                                                  hc_PACKAGE_EXT,
                                                                                  hc_PACKAGE_EXT_TMP,
                                                                                  mhc_ErrorMessage);

   // active bus index in range?
   if (s32_Return == C_NO_ERR)
   {
      bool q_Contained = false;
      for (uint32_t u32_It = 0UL; (u32_It <= orc_SystemDefinition.c_Nodes.size()) && (q_Contained == false); ++u32_It)
      {
         const C_OscNode & rc_Node = orc_SystemDefinition.c_Nodes[u32_It];
         if (rc_Node.c_Properties.c_Name == orc_Manifest.c_NodeName)
         {
            q_Contained = true;
         }
      }
      if (q_Contained == false)
      {
         mhc_ErrorMessage = "Node \"" +
                            orc_Manifest.c_NodeName + "\" is not in System Definition.";
         osc_write_log_error(mhc_USE_CASE, mhc_ErrorMessage);
         s32_Return = C_NOACT;
      }
   }
   return s32_Return;
}
