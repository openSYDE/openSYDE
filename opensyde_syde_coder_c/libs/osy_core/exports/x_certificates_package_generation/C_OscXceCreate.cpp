//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: X-Certificates Package creation

   openSYDE: X-Certificates Package creation

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OscXceCreate.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscXceManifestFiler.hpp"
#include "C_OscSpaServicePackageCreateUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw::scl::C_SclString C_OscXceCreate::mhc_USE_CASE = "Creating X-Certificates Package";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create an X-Certificates package

   Result of function after successful execution:
   * zipped X-Certificates package with the following contents:
       * all certificates in orc_CertificatesPath
       * all certificates in orc_UpdatePackageParameters
       * all pwasswords in orc_UpdatePackageParameters

   Function is temporarily creating a folder to save files for zip archive.
   If the optional parameter orc_TemporaryDirectory is empty, this temporary folder is
   created next to package path. Else it is created as subfolder of the provided temporary directory.
   In any case it is tried to delete the temporary folder at the end of this function.
   The temporary folder has the extension ".syde_pkg_tmp" and must not exist already.

   Assumptions:
   * write permission to target folder

   \param[in]   orc_PackagePath              full path of target package (zip archive)
                                             (with constant extension ".syde_pkg", e.g. "C:\\012345.syde_pkg")
   \param[in]   orc_CertificatesPath         Certificates path
   \param[in]   orc_UpdatePackageParameters  Update package parameters
   \param[out]  orc_WarningMessages          warning messages for created package (empty list if no warnings)
   \param[out]  orc_ErrorMessage             error message in case of failure (empty string if no error)
   \param[in]   orc_TemporaryDirectory       Optional parameter for temporary directory. If empty, temporary directory
                                             is created next to package path.

   \return
   C_NO_ERR    success
   C_RANGE     target file already exists
               target directory for package does not exist
               invalid package name with no package extension ".syde_xcert"
               temporary folder (orc_PackagePath with mc_PACKAGE_EXT_TMP) already exists
   C_NOACT     certificates do not exist
   C_RD_WR     could not create temporary folders
               could not save manifest file
               could not save pem file
   C_BUSY      could not package result to zip archive
               could not delete temporary result folder
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXceCreate::h_CreatePackage(const stw::scl::C_SclString & orc_PackagePath,
                                        const std::vector<stw::scl::C_SclString> & orc_CertificatesPath,
                                        const std::vector<C_OscXceUpdatePackageParameters> & orc_UpdatePackageParameters, stw::scl::C_SclStringList & orc_WarningMessages, stw::scl::C_SclString & orc_ErrorMessage,
                                        const stw::scl::C_SclString & orc_TemporaryDirectory)
{
   int32_t s32_Return;

   bool q_TemporaryFolderCreated = false; // for cleanup at the end of this function

   mh_Init();

   stw::scl::C_SclString c_PackagePathTmp;                           // temporary package path before
                                                                     // creating zip archive
   const stw::scl::C_SclString c_TargetZipArchive = orc_PackagePath; // complete path of target zip
                                                                     // archive
   std::set<stw::scl::C_SclString> c_XcertFiles;                     // unique container with
                                                                     // relative file paths for zip
                                                                     // archive
   // fill with constant file names
   c_XcertFiles.insert(C_OscXceManifestFiler::hc_FILE_NAME);

   // precondition checks
   s32_Return = mh_CheckParamsToCreatePackage(orc_PackagePath, orc_CertificatesPath, orc_UpdatePackageParameters);

   // * create folders with device application files (via h_CreateTemporaryFolder)
   //   h_CreateTemporaryFolder is creating the temporary folder (and deleting in advance if it already exists)
   //   where the other files (devices.ini, system definition etc.) also are placed
   //   and has therefore be the first action for creating service update package
   if (s32_Return == C_NO_ERR)
   {
      std::vector<stw::scl::C_SclString> c_AllStaticSubFolders;
      c_AllStaticSubFolders.push_back(mhc_CERTIFICATES_FOLDER);
      c_AllStaticSubFolders.push_back(mhc_UPDATE_PACKAGE_PARAMETERS_FOLDER);
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
      const C_OscXceManifest c_Manifest = mh_CreateManifest(orc_UpdatePackageParameters);
      const stw::scl::C_SclString c_ManifestPath = c_PackagePathTmp + C_OscXceManifestFiler::hc_FILE_NAME;
      s32_Return = C_OscXceManifestFiler::h_SaveFile(c_Manifest, c_ManifestPath);
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
   if (s32_Return == C_NO_ERR)
   {
      s32_Return =
         mh_PrepareCertFiles(c_PackagePathTmp, orc_CertificatesPath, orc_UpdatePackageParameters, c_XcertFiles);
   }
   // package temporary result folder to zip file
   if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
   {
      s32_Return = C_OscSpaServicePackageCreateUtil::h_CreateZip(mhc_USE_CASE, c_PackagePathTmp,
                                                                 c_TargetZipArchive, c_XcertFiles,
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
/*! \brief  Check params to create package

   \param[in]  orc_PackagePath               (see function h_CreatePackage)
   \param[in]  orc_CertificatesPath          (see function h_CreatePackage)
   \param[in]  orc_UpdatePackageParameters   (see function h_CreatePackage)

   \return
   C_NO_ERR    success
   C_RANGE     target package file already exists
               target directory for package does not exist
               invalid package name with no package extension ".syde_pkg"
   C_NOACT     certificates do not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXceCreate::mh_CheckParamsToCreatePackage(const stw::scl::C_SclString & orc_PackagePath,
                                                      const std::vector<stw::scl::C_SclString> & orc_CertificatesPath,
                                                      const std::vector<C_OscXceUpdatePackageParameters> & orc_UpdatePackageParameters)
{
   int32_t s32_Return = C_OscSpaServicePackageCreateUtil::h_CheckPackagePathParam(orc_PackagePath,
                                                                                  mhc_USE_CASE,
                                                                                  hc_PACKAGE_EXT,
                                                                                  hc_PACKAGE_EXT_TMP,
                                                                                  mhc_ErrorMessage);

   if (s32_Return == C_NO_ERR)
   {
      for (uint32_t u32_It = 0UL; (u32_It < orc_CertificatesPath.size()) && (s32_Return == C_NO_ERR); ++u32_It)
      {
         s32_Return = C_OscXceCreate::mh_CheckFileExists(orc_CertificatesPath[u32_It]);
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      for (uint32_t u32_It = 0UL; (u32_It < orc_UpdatePackageParameters.size()) && (s32_Return == C_NO_ERR); ++u32_It)
      {
         const C_OscXceUpdatePackageParameters & rc_In = orc_UpdatePackageParameters[u32_It];
         if (rc_In.c_AuthenticationKeyPath.IsEmpty() == false)
         {
            s32_Return = C_OscXceCreate::mh_CheckFileExists(rc_In.c_AuthenticationKeyPath);
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check file exists

   \param[in]  orc_Path    Path

   \return
   C_NO_ERR    success
   C_NOACT     certificates do not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXceCreate::mh_CheckFileExists(const stw::scl::C_SclString & orc_Path)
{
   int32_t s32_Return = C_NO_ERR;

   if (TglFileExists(orc_Path) == false)
   {
      mhc_ErrorMessage = "File \"" + orc_Path + "\" not found.";
      osc_write_log_error(mhc_USE_CASE, mhc_ErrorMessage);
      s32_Return = C_NOACT;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create manifest

   \param[in]  orc_UpdatePackageParameters   Update package parameters

   \return
   Created manifest
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscXceManifest C_OscXceCreate::mh_CreateManifest(
   const std::vector<C_OscXceUpdatePackageParameters> & orc_UpdatePackageParameters)
{
   C_OscXceManifest c_Manifest;

   c_Manifest.c_CertificatesPath = C_OscXceBase::mhc_CERTIFICATES_FOLDER;
   c_Manifest.c_UpdatePackageParameters.reserve(orc_UpdatePackageParameters.size());
   for (uint32_t u32_It = 0UL; u32_It < orc_UpdatePackageParameters.size(); ++u32_It)
   {
      C_OscXceUpdatePackageParameters c_Out;
      const C_OscXceUpdatePackageParameters & rc_In = orc_UpdatePackageParameters[u32_It];
      c_Out.c_Password = rc_In.c_Password;
      if (rc_In.c_AuthenticationKeyPath.IsEmpty())
      {
         c_Out.c_AuthenticationKeyPath = "";
      }
      else
      {
         c_Out.c_AuthenticationKeyPath = C_OscXceCreate::mh_GenOutFilePathPart(rc_In.c_AuthenticationKeyPath,
                                                                               C_OscXceBase::mhc_UPDATE_PACKAGE_PARAMETERS_FOLDER);
      }
      c_Manifest.c_UpdatePackageParameters.push_back(c_Out);
   }
   return c_Manifest;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prepare cert files

   \param[in]      orc_TmpPath                  Tmp path
   \param[in]      orc_CertificatesPath         Certificates path
   \param[in]      orc_UpdatePackageParameters  Update package parameters
   \param[in,out]  orc_XcertFiles               Xcert files

   \return
   STW error codes

   \retval   C_NO_ERR   Detailed description
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXceCreate::mh_PrepareCertFiles(const stw::scl::C_SclString & orc_TmpPath,
                                            const std::vector<stw::scl::C_SclString> & orc_CertificatesPath,
                                            const std::vector<C_OscXceUpdatePackageParameters> & orc_UpdatePackageParameters,
                                            std::set<stw::scl::C_SclString> & orc_XcertFiles)
{
   int32_t s32_Return = C_NO_ERR;

   for (uint32_t u32_It = 0UL; (u32_It < orc_CertificatesPath.size()) && (s32_Return == C_NO_ERR); ++u32_It)
   {
      s32_Return = mh_CopyFile(orc_CertificatesPath[u32_It], orc_TmpPath, mhc_CERTIFICATES_FOLDER, orc_XcertFiles);
   }
   if (s32_Return == C_NO_ERR)
   {
      for (uint32_t u32_It = 0UL; (u32_It < orc_UpdatePackageParameters.size()) && (s32_Return == C_NO_ERR); ++u32_It)
      {
         const C_OscXceUpdatePackageParameters & rc_In = orc_UpdatePackageParameters[u32_It];
         if (rc_In.c_AuthenticationKeyPath.IsEmpty() == false)
         {
            s32_Return = mh_CopyFile(rc_In.c_AuthenticationKeyPath, orc_TmpPath, mhc_UPDATE_PACKAGE_PARAMETERS_FOLDER,
                                     orc_XcertFiles);
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate out file path part

   \param[in]  orc_InPath        In path
   \param[in]  orc_TargetFolder  Target folder

   \return
   Out file path part
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscXceCreate::mh_GenOutFilePathPart(const stw::scl::C_SclString & orc_InPath,
                                                            const stw::scl::C_SclString & orc_TargetFolder)
{
   const stw::scl::C_SclString c_FileName = TglExtractFileName(orc_InPath);
   const stw::scl::C_SclString c_Retval = TglFileIncludeTrailingDelimiter(orc_TargetFolder) + c_FileName;

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy file

   \param[in]      orc_InPath       In path
   \param[in]      orc_OutBasePath  Out base path
   \param[in]      orc_OutFolder    Out folder
   \param[in,out]  orc_XcertFiles   Xcert files

   \return
   STW error codes

   \retval   C_NO_ERR   Detailed description
   \retval   C_NOACT    Detailed description
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXceCreate::mh_CopyFile(const stw::scl::C_SclString & orc_InPath,
                                    const stw::scl::C_SclString & orc_OutBasePath,
                                    const stw::scl::C_SclString & orc_OutFolder,
                                    std::set<stw::scl::C_SclString> & orc_XcertFiles)
{
   int32_t s32_Return;
   const stw::scl::C_SclString c_OutPart = C_OscXceCreate::mh_GenOutFilePathPart(orc_InPath,
                                                                                 orc_OutFolder);
   const stw::scl::C_SclString c_Target = orc_OutBasePath + c_OutPart;

   stw::scl::C_SclString c_Error;

   orc_XcertFiles.insert(c_OutPart);
   s32_Return = C_OscUtils::h_CopyFile(orc_InPath, c_Target, NULL, &c_Error);
   if (s32_Return != C_NO_ERR)
   {
      mhc_ErrorMessage = c_Error;
      osc_write_log_error(mhc_USE_CASE, mhc_ErrorMessage);
      s32_Return = C_NOACT;
   }
   return s32_Return;
}
