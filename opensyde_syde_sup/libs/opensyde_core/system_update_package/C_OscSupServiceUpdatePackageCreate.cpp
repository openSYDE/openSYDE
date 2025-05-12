//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Service Update Package creation

   For details cf. documentation in .h file.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <fstream>
#include <iterator>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "C_OscSupServiceUpdatePackageV1.hpp"
#include "C_OscSupServiceUpdatePackageCreate.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscSystemDefinitionFiler.hpp"
#include "C_OscSystemDefinitionFilerV2.hpp"
#include "C_OscDeviceDefinition.hpp"
#include "C_OscDeviceDefinitionFiler.hpp"
#include "C_OscSuSequences.hpp"
#include "TglFile.hpp"
#include "C_SclIniFile.hpp"
#include "C_OscSuSequences.hpp"
#include "C_OscUtils.hpp"
#include "C_OscZipFile.hpp"
#include "C_OscAesFile.hpp"
#include "C_OscSupSignatureFiler.hpp"
#include "C_OscSupDefinitionFiler.hpp"
#include "C_OscSecurityPemSecUpdate.hpp"
#include "C_OscSupNodeDefinitionFiler.hpp"
#include "C_OscSpaServicePackageCreateUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::diag_lib;
using namespace std;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create an update package

   Result of function after successful execution:
   * zipped service update package with the following contents:
       * system definition file of current view ("sup_system_definition.syde_sysdef")
       * devices.ini file
       * device definition files of all devices in current system definition of view
       * folders with device application files (only for nodes to update)
       * service update package file ("service_update_package.syde_supdef")

   Function is temporarily creating a folder to save files for zip archive.
   If the optional parameter orc_TemporaryDirectory is empty, this temporary folder is
   created next to package path. Else it is created as subfolder of the provided temporary directory.
   In any case it is tried to delete the temporary folder at the end of this function.
   The temporary folder has the extension ".syde_sup_tmp" and must not exist already.

   Assumptions:
   * write permission to target folder

   \param[in]   orc_PackagePath              full path of target package (zip archive)
                                             (with constant extension ".syde_sup", e.g. "C:\\012345.syde_sup")
   \param[in]   orc_SystemDefinition         Current system definition (class instance - no file!)
   \param[in]   ou32_ActiveBusIndex          index of bus the client is connected to
   \param[in]   orc_ActiveNodes              list of all nodes contains information which node is available for update
   \param[in]   orc_NodesUpdateOrder         update order of nodes (index is update position, value is node index)
   \param[in]   orc_ApplicationsToWrite      files for updating nodes
   \param[out]  orc_WarningMessages          warning messages for created package (empty list if no warnings)
   \param[out]  orc_ErrorMessage             error message in case of failure (empty string if no error)
   \param[in]   orc_TemporaryDirectory       Optional parameter for temporary directory. If empty, temporary directory
                                             is created next to package path.
   \param[in]   orc_EncryptNodes             list of all nodes contains information which node is encrypted
   \param[in]   orc_EncryptNodesPassword     list of all nodes contains information which node uses which password
   \param[in]   orc_AddSignatureNodes        list of all nodes contains information which node to add signature for
   \param[in]   orc_NodeSignaturePemFiles    list of all nodes contains information which pem file to use for which node

   \return
   C_NO_ERR    success
   C_WARN      could not find update position for active node (internal error which should not occur,
                                                               difficult to test and reproduce)
   C_RANGE     target file already exists
               target directory for package does not exist
               invalid package name with no package extension ".syde_sup"
               temporary folder (orc_PackagePath with mc_PACKAGE_EXT_TMP) already exists
   C_CONFIG    no active node for update
               no element in update order of nodes
               no application to update for one specific node
   C_OVERFLOW  size of orc_ActiveNodes does not match system definition
               size of orc_ActiveNodes is not the same as the size of nodes in orc_ApplicationsToWrite
   C_NOACT     active bus index is not in system definition
   C_RD_WR     could not create temporary folder with application files
               could not save system definition file
               could not save device definition file
               could not save .syde_supdef file
               could not read pem file
               pem file did not contain private key with the correct length
   C_BUSY      could not package result to zip archive
               could not delete temporary result folder
   C_CHECKSUM  size of orc_EncryptNodes does not match system definition
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::h_CreatePackageUsingPemFiles(const C_SclString & orc_PackagePath,
                                                                         const C_OscSystemDefinition & orc_SystemDefinition, const uint32_t ou32_ActiveBusIndex, const std::vector<uint8_t> & orc_ActiveNodes, const std::vector<uint32_t> & orc_NodesUpdateOrder, const std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite, C_SclStringList & orc_WarningMessages, C_SclString & orc_ErrorMessage, const C_SclString & orc_TemporaryDirectory, const std::vector<uint8_t> & orc_EncryptNodes, const std::vector<C_SclString> & orc_EncryptNodesPassword, const std::vector<uint8_t> & orc_AddSignatureNodes,
                                                                         const std::vector<C_SclString> & orc_NodeSignaturePemFiles)
{
   int32_t s32_Retval;

   mh_Init();

   s32_Retval =
      mh_CheckPemFileParameters(orc_NodeSignaturePemFiles, orc_AddSignatureNodes,
                                static_cast<uint32_t>(orc_SystemDefinition.c_Nodes.size()));
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<uint8_t> c_AddSignatureNodes;
      std::vector<std::vector<uint8_t> > c_NodeSignatureKeys;
      s32_Retval =
         mh_GetPemFileContent(orc_ActiveNodes, orc_AddSignatureNodes, orc_NodeSignaturePemFiles,
                              static_cast<uint32_t>(orc_SystemDefinition.c_Nodes.size()), c_AddSignatureNodes,
                              c_NodeSignatureKeys);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = h_CreatePackage(orc_PackagePath, orc_SystemDefinition, ou32_ActiveBusIndex, orc_ActiveNodes,
                                      orc_NodesUpdateOrder, orc_ApplicationsToWrite, orc_WarningMessages,
                                      orc_ErrorMessage, orc_TemporaryDirectory, orc_EncryptNodes,
                                      orc_EncryptNodesPassword, c_AddSignatureNodes, c_NodeSignatureKeys);
      }
   }
   mh_GetWarningsAndErrors(orc_WarningMessages, orc_ErrorMessage);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create an update package

   Result of function after successful execution:
   * zipped service update package with the following contents:
       * system definition file of current view ("sup_system_definition.syde_sysdef")
       * devices.ini file
       * device definition files of all devices in current system definition of view
       * folders with device application files (only for nodes to update)
       * service update package file ("service_update_package.syde_supdef")

   Function is temporarily creating a folder to save files for zip archive.
   If the optional parameter orc_TemporaryDirectory is empty, this temporary folder is
   created next to package path. Else it is created as subfolder of the provided temporary directory.
   In any case it is tried to delete the temporary folder at the end of this function.
   The temporary folder has the extension ".syde_sup_tmp" and must not exist already.

   Assumptions:
   * write permission to target folder

   \param[in]   orc_PackagePath           full path of target package (zip archive)
                                          (with constant extension ".syde_sup", e.g. "C:\\012345.syde_sup")
   \param[in]   orc_SystemDefinition      Current system definition (class instance - no file!)
   \param[in]   ou32_ActiveBusIndex       index of bus the client is connected to
   \param[in]   orc_ActiveNodes           list of all nodes contains information which node is available for update
   \param[in]   orc_NodesUpdateOrder      update order of nodes (index is update position, value is node index)
   \param[in]   orc_ApplicationsToWrite   files for updating nodes
   \param[out]  orc_WarningMessages       warning messages for created package (empty list if no warnings)
   \param[out]  orc_ErrorMessage          error message in case of failure (empty string if no error)
   \param[in]   orc_TemporaryDirectory    Optional parameter for temporary directory. If empty, temporary directory
                                          is created next to package path.
   \param[in]   orc_EncryptNodes          list of all nodes contains information which node is encrypted
   \param[in]   orc_EncryptNodesPassword  list of all nodes contains information which node uses which password
   \param[in]   orc_AddSignatureNodes     list of all nodes contains information which node to add signature for
   \param[in]   orc_NodeSignatureKeys     list of all nodes contains information which key to use for which node

   \return
   C_NO_ERR    success
   C_WARN      could not find update position for active node (internal error which should not occur,
                                                               difficult to test and reproduce)
   C_RANGE     target file already exists
               target directory for package does not exist
               invalid package name with no package extension ".syde_sup"
               temporary folder (orc_PackagePath with mc_PACKAGE_EXT_TMP) already exists
   C_CONFIG    no active node for update
               no element in update order of nodes
               no application to update for one specific node
   C_OVERFLOW  size of orc_ActiveNodes does not match system definition
               size of orc_ActiveNodes is not the same as the size of nodes in orc_ApplicationsToWrite
   C_NOACT     active bus index is not in system definition
   C_RD_WR     could not create temporary folder with application files
               could not save system definition file
               could not save device definition file
               could not save .syde_supdef file
   C_BUSY      could not package result to zip archive
               could not delete temporary result folder
   C_CHECKSUM  size of orc_EncryptNodes does not match system definition
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::h_CreatePackage(const C_SclString & orc_PackagePath,
                                                            const C_OscSystemDefinition & orc_SystemDefinition,
                                                            const uint32_t ou32_ActiveBusIndex,
                                                            const vector<uint8_t> & orc_ActiveNodes,
                                                            const vector<uint32_t> & orc_NodesUpdateOrder,
                                                            const vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite, C_SclStringList & orc_WarningMessages, C_SclString & orc_ErrorMessage, const C_SclString & orc_TemporaryDirectory, const std::vector<uint8_t> & orc_EncryptNodes, const std::vector<C_SclString> & orc_EncryptNodesPassword, const std::vector<uint8_t> & orc_AddSignatureNodes,
                                                            const std::vector<std::vector<uint8_t> > & orc_NodeSignatureKeys)
{
   int32_t s32_Return;

   bool q_TemporaryFolderCreated = false; // for cleanup at the end of this function

   mh_Init();

   C_SclString c_PackagePathTmp;                                                        // temporary package path before
                                                                                        // creating zip archive
   const C_SclString c_TargetZipArchive = orc_PackagePath;                              // complete path of target zip
                                                                                        // archive
   vector<C_OscSuSequences::C_DoFlash> c_ApplicationsToWrite = orc_ApplicationsToWrite; // paths of applications
   std::set<stw::scl::C_SclString> c_SupFiles;                                          // unique container with
                                                                                        // relative file paths for zip
                                                                                        // archive
   // fill with constant file names
   c_SupFiles.insert(mhc_INI_DEV);
   c_SupFiles.insert(mhc_SUP_SYSDEF);
   c_SupFiles.insert(C_OscSupDefinitionFiler::hc_PACKAGE_UPDATE_DEF);

   // precondition checks
   s32_Return = mh_CheckParamsToCreatePackage(orc_PackagePath, orc_SystemDefinition, ou32_ActiveBusIndex,
                                              orc_ActiveNodes, orc_NodesUpdateOrder, orc_ApplicationsToWrite,
                                              orc_EncryptNodes, orc_EncryptNodesPassword, orc_AddSignatureNodes,
                                              orc_NodeSignatureKeys);

   // * create folders with device application files (via h_CreateTemporaryFolder)
   //   h_CreateTemporaryFolder is creating the temporary folder (and deleting in advance if it already exists)
   //   where the other files (devices.ini, system definition etc.) also are placed
   //   and has therefore be the first action for creating service update package
   if (s32_Return == C_NO_ERR)
   {
      stw::scl::C_SclString c_ErrorPath;
      C_OscSpaServicePackageCreateUtil::h_GetTempFolderName(orc_PackagePath, orc_TemporaryDirectory,
                                                            "Creating Update Package", mhc_PACKAGE_EXT,
                                                            mhc_PACKAGE_EXT_TMP, c_PackagePathTmp);

      // create folders and copy applications to them
      s32_Return = C_OscSuSequences::h_CreateTemporaryFolder(orc_SystemDefinition.c_Nodes, orc_ActiveNodes,
                                                             c_PackagePathTmp, c_ApplicationsToWrite, &c_ErrorPath);
      if (s32_Return != C_NO_ERR)
      {
         // very strange! normally the precondition check should
         // guarantee a correct behavior of h_CreateTemporaryFolder
         mhc_ErrorMessage = "Could not create temporary folder \"" +
                            c_PackagePathTmp + "\" with application files.";
         if (c_ErrorPath.IsEmpty() == false)
         {
            mhc_ErrorMessage += " Issue in path: \"" + c_ErrorPath + "\"";
         }
         osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         s32_Return = C_RD_WR; // redefine because we only have a few error codes
      }

      q_TemporaryFolderCreated = true; // at least partly
   }

   // * system definition file
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = C_OscSpaServicePackageCreateUtil::h_SaveSystemDefinition(orc_SystemDefinition, mhc_SUP_SYSDEF,
                                                                            "Creating Update Package",
                                                                            c_PackagePathTmp, "", c_SupFiles,
                                                                            mhc_ErrorMessage);
   }

   // device.ini and device definition files
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = C_OscSpaServicePackageCreateUtil::h_SaveDeviceDefinitionsAndIni(orc_SystemDefinition,
                                                                                   "Creating Update Package",
                                                                                   c_PackagePathTmp, "",
                                                                                   c_SupFiles,
                                                                                   mhc_ErrorMessage);
   }

   // * service update package file (service_update_package.syde_supdef)
   if (s32_Return == C_NO_ERR)
   {
      C_OscSupDefinition c_SupDefContent;
      s32_Return = mh_SupDefParamAdapter(ou32_ActiveBusIndex,
                                         orc_ActiveNodes, orc_NodesUpdateOrder,
                                         c_ApplicationsToWrite, c_SupDefContent);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = mh_CreateDefFilesAndZipSecureFiles(orc_SystemDefinition, c_SupDefContent, c_PackagePathTmp,
                                                         orc_ActiveNodes, c_ApplicationsToWrite, c_SupFiles,
                                                         orc_EncryptNodes, orc_EncryptNodesPassword,
                                                         orc_AddSignatureNodes, orc_NodeSignatureKeys);
      }
   }
   //only happens if we want a .syde_sup file. Otherwise the temp folder is our save destination
   //and is has no longer suffix "temp" -> therefore no deleting either
   // package temporary result folder to zip file
   if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
   {
      s32_Return = C_OscSpaServicePackageCreateUtil::h_CreateZip("Creating Update Package", c_PackagePathTmp,
                                                                 c_TargetZipArchive, c_SupFiles,
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
/*! \brief  Check security parameters

   \param[in]  orc_EncryptNodes           Encrypt nodes
   \param[in]  orc_EncryptNodesPassword   Encrypt nodes password
   \param[in]  orc_SignatureNodes         Signature nodes
   \param[in]  orc_NodeSignatureKeys      Node signature keys
   \param[in]  ou32_NumNodes              Num nodes
   \param[in]  orc_Mode                   Mode
   \param[in]  orc_Function               Function

   \return
   C_NO_ERR    success
   C_CHECKSUM  size of orc_EncryptNodes does not match system definition
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_CheckSecurityParameters(const std::vector<uint8_t> & orc_EncryptNodes,
                                                                       const std::vector<C_SclString> & orc_EncryptNodesPassword, const std::vector<uint8_t> & orc_SignatureNodes, const std::vector<std::vector<uint8_t> > & orc_NodeSignatureKeys, const uint32_t ou32_NumNodes, const C_SclString & orc_Mode,
                                                                       const stw::scl::C_SclString & orc_Function)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_SignatureNodes.size() != orc_NodeSignatureKeys.size())
   {
      mhc_ErrorMessage = "The container of signature nodes and signature nodes keys have not the same size.";
      osc_write_log_error(orc_Function, mhc_ErrorMessage);
      s32_Return = C_CHECKSUM;
   }

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = mh_CheckCommonSecurityParameters(orc_EncryptNodes, orc_EncryptNodesPassword, orc_NodeSignatureKeys,
                                                    ou32_NumNodes, orc_Mode, orc_Function);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check pem file parameters

   \param[in]  orc_NodeSignaturePemFiles  Node signature pem files
   \param[in]  orc_SignatureNodes         Signature nodes
   \param[in]  ou32_NumNodes              Num nodes

   \return
   C_NO_ERR    success
   C_CHECKSUM  size of orc_NodeSignaturePemFiles does not match system definition
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_CheckPemFileParameters(
   const std::vector<C_SclString> & orc_NodeSignaturePemFiles, const std::vector<uint8_t> & orc_SignatureNodes,
   const uint32_t ou32_NumNodes)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_SignatureNodes.size() != orc_NodeSignaturePemFiles.size())
   {
      mhc_ErrorMessage = "The container of signature nodes and signature nodes pem files have not the same size.";
      osc_write_log_error("Create package using pem files", mhc_ErrorMessage);
      s32_Return = C_CHECKSUM;
   }

   if (s32_Return == C_NO_ERR)
   {
      if (((orc_NodeSignaturePemFiles.size() != 0UL) && (orc_NodeSignaturePemFiles.size() != 1UL)) &&
          (orc_NodeSignaturePemFiles.size() != ou32_NumNodes))
      {
         mhc_ErrorMessage = "The container of signature pem files and nodes have not the same size.";
         osc_write_log_error("Create package using pem files", mhc_ErrorMessage);
         s32_Return = C_CHECKSUM;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt signature parameters

   \param[in]      orc_InAddSignatureNodes   In add signature nodes
   \param[in]      orc_InNodeSignatureKeys   In node signature keys
   \param[in]      ou32_NodeCount            Node count
   \param[in,out]  orc_OutAddSignatureNodes  Out add signature nodes
   \param[in,out]  orc_OutNodeSignatureKeys  Out node signature keys
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageCreate::mh_AdaptSignatureParameters(
   const std::vector<uint8_t> & orc_InAddSignatureNodes,
   const std::vector<std::vector<uint8_t> > & orc_InNodeSignatureKeys, const uint32_t ou32_NodeCount,
   std::vector<uint8_t> & orc_OutAddSignatureNodes, std::vector<std::vector<uint8_t> > & orc_OutNodeSignatureKeys)
{
   orc_OutAddSignatureNodes = orc_InAddSignatureNodes;
   if (orc_OutAddSignatureNodes.size() == 0UL)
   {
      //Default
      orc_OutAddSignatureNodes.resize(ou32_NodeCount, 0U);
   }
   else if (orc_OutAddSignatureNodes.size() == 1UL)
   {
      //All same
      orc_OutAddSignatureNodes.resize(ou32_NodeCount, orc_InAddSignatureNodes[0U]);
   }
   else
   {
      //No change necessary
   }
   mh_AdaptCommonSignatureParameters(orc_InNodeSignatureKeys, ou32_NodeCount, orc_OutNodeSignatureKeys);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt pem file parameters

   \param[in]      orc_InAddSignatureNodes         In add signature nodes
   \param[in]      orc_InNodeSignaturePemFiles     In node signature pem files
   \param[in]      ou32_NodeCount                  Node count
   \param[in,out]  orc_OutNodeSignaturePemFiles    Out node signature pem files
   \param[in,out]  orc_OutAddSignatureNodes        Out add signature nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageCreate::mh_AdaptPemFileParameters(const std::vector<uint8_t> & orc_InAddSignatureNodes,
                                                                   const std::vector<C_SclString> & orc_InNodeSignaturePemFiles, const uint32_t ou32_NodeCount, std::vector<C_SclString> & orc_OutNodeSignaturePemFiles,
                                                                   std::vector<uint8_t> & orc_OutAddSignatureNodes)
{
   orc_OutAddSignatureNodes = orc_InAddSignatureNodes;
   if (orc_OutAddSignatureNodes.size() == 0UL)
   {
      //Default
      orc_OutAddSignatureNodes.resize(ou32_NodeCount, 0U);
   }
   else if (orc_OutAddSignatureNodes.size() == 1UL)
   {
      //All same
      orc_OutAddSignatureNodes.resize(ou32_NodeCount, orc_InAddSignatureNodes[0U]);
   }
   else
   {
      //No change necessary
   }
   orc_OutNodeSignaturePemFiles = orc_InNodeSignaturePemFiles;
   if (orc_OutNodeSignaturePemFiles.size() == 0UL)
   {
      //Default
      orc_OutNodeSignaturePemFiles.resize(ou32_NodeCount, "");
   }
   else if (orc_OutNodeSignaturePemFiles.size() == 1UL)
   {
      //All same
      orc_OutNodeSignaturePemFiles.resize(ou32_NodeCount, orc_InNodeSignaturePemFiles[0U]);
   }
   else
   {
      //No change necessary
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Precondition checks for creating update package (internal function).

   \param[in]  orc_PackagePath            (see function h_CreatePackage)
   \param[in]  orc_SystemDefinition       (see function h_CreatePackage)
   \param[in]  ou32_ActiveBusIndex        (see function h_CreatePackage)
   \param[in]  orc_ActiveNodes            (see function h_CreatePackage)
   \param[in]  orc_NodesUpdateOrder       (see function h_CreatePackage)
   \param[in]  orc_ApplicationsToWrite    (see function h_CreatePackage)
   \param[in]  orc_EncryptNodes           (see function h_CreatePackage)
   \param[in]  orc_EncryptNodesPassword   (see function h_CreatePackage)
   \param[in]  orc_AddSignatureNodes      (see function h_CreatePackage)
   \param[in]  orc_NodeSignatureKeys      (see function h_CreatePackage)

   \return
   C_NO_ERR    success
   C_RANGE     target package file already exists
               target directory for package does not exist
               invalid package name with no package extension ".syde_sup"
   C_CONFIG    no active node for update
               no element in update order of nodes
   C_OVERFLOW  size of orc_ActiveNodes does not match system definition
               size of orc_ActiveNodes is not the same as the size of nodes in orc_ApplicationsToWrite
   C_NOACT     active bus index is not in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_CheckParamsToCreatePackage(const C_SclString & orc_PackagePath,
                                                                          const C_OscSystemDefinition & orc_SystemDefinition, const uint32_t ou32_ActiveBusIndex, const vector<uint8_t> & orc_ActiveNodes, const vector<uint32_t> & orc_NodesUpdateOrder, const vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite, const std::vector<uint8_t> & orc_EncryptNodes, const std::vector<stw::scl::C_SclString> & orc_EncryptNodesPassword, const std::vector<uint8_t> & orc_AddSignatureNodes,
                                                                          const std::vector<std::vector<uint8_t> > & orc_NodeSignatureKeys)
{
   int32_t s32_Return = C_OscSpaServicePackageCreateUtil::h_CheckPackagePathParam(orc_PackagePath,
                                                                                  "Creating Update Package",
                                                                                  mhc_PACKAGE_EXT,
                                                                                  mhc_PACKAGE_EXT_TMP,
                                                                                  mhc_ErrorMessage);

   // active bus index in range?
   if (s32_Return == C_NO_ERR)
   {
      if (ou32_ActiveBusIndex >= orc_SystemDefinition.c_Buses.size())
      {
         mhc_ErrorMessage = "Active Bus Index \"" +
                            C_SclString::IntToStr(ou32_ActiveBusIndex) + "\" is not in System Definition.";
         osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         s32_Return = C_NOACT;
      }
   }

   // is number of nodes correct and is there any active node?
   if (s32_Return == C_NO_ERR)
   {
      // number of nodes must match system definition
      if (orc_ActiveNodes.size() != orc_SystemDefinition.c_Nodes.size())
      {
         mhc_ErrorMessage = "Number of nodes does match System Definition.";
         osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         s32_Return = C_OVERFLOW;
      }
      else
      {
         // there has to be at least one active node
         bool q_Tmp = false;
         for (uint32_t u32_Pos = 0; (u32_Pos < orc_ActiveNodes.size()) && (q_Tmp == false); u32_Pos++)
         {
            if (orc_ActiveNodes[u32_Pos] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
            {
               q_Tmp = true;
            }
         }
         if (q_Tmp == false)
         {
            mhc_ErrorMessage = "There is no active node for update.";
            osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
            s32_Return = C_CONFIG;
         }
      }
   }

   // at least one element in node order for update?
   if (s32_Return == C_NO_ERR)
   {
      if (orc_NodesUpdateOrder.size() == 0)
      {
         mhc_ErrorMessage = "There is no element in update order of nodes.";
         osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         s32_Return = C_CONFIG;
      }
   }

   // do the container of active nodes and applications to update have the same size? (if not then internal config
   // error)
   if (s32_Return == C_NO_ERR)
   {
      if (orc_ActiveNodes.size() != orc_ApplicationsToWrite.size())
      {
         mhc_ErrorMessage = "The container of active nodes and update applications have not the same size.";
         osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         s32_Return = C_OVERFLOW;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = mh_CheckSecurityParameters(orc_EncryptNodes, orc_EncryptNodesPassword, orc_AddSignatureNodes,
                                              orc_NodeSignatureKeys,
                                              static_cast<uint32_t>(orc_ActiveNodes.size()), "encrypt",
                                              "Creating Update Package");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Helper function to convert parameters for service update package (internal function).

   Assumptions:
   * checked parameters

   \param[in]   ou32_ActiveBusIndex       (see function h_CreatePackage)
   \param[in]   orc_ActiveNodes           (see function h_CreatePackage)
   \param[in]   orc_NodesUpdateOrder      (see function h_CreatePackage)
   \param[in]   orc_ApplicationsToWrite   (see function h_CreatePackage)
   \param[out]  orc_SupDefContent         content to write service update package

   \return
   C_NO_ERR     success
   C_WARN       could not find update position for active node
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_SupDefParamAdapter(const uint32_t ou32_ActiveBusIndex,
                                                                  const vector<uint8_t> & orc_ActiveNodes,
                                                                  const vector<uint32_t> & orc_NodesUpdateOrder,
                                                                  const vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                                                  C_OscSupDefinition & orc_SupDefContent)
{
   int32_t s32_Return = C_NO_ERR;
   C_OscSupDefinition c_SupDefContent;

   // get general content
   c_SupDefContent.u32_ActiveBusIndex = ou32_ActiveBusIndex;

   // get contents for each node
   for (uint32_t u32_Pos = 0; u32_Pos < orc_ActiveNodes.size(); u32_Pos++)
   {
      C_OscSupNodeDefinition c_SupDefNodeContent;
      c_SupDefNodeContent.u8_Active = orc_ActiveNodes[u32_Pos];
      // in case we have an active node and (!) application(s) for update are available
      if ((c_SupDefNodeContent.u8_Active == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE) &&
          ((orc_ApplicationsToWrite[u32_Pos].c_FilesToFlash.size() > 0) ||
           (orc_ApplicationsToWrite[u32_Pos].c_FilesToWriteToNvm.size() > 0) ||
           (orc_ApplicationsToWrite[u32_Pos].c_PemFile != "")))
      {
         // get update position of node
         // nodes update order only contains active nodes with applications
         const int32_t s32_Tmp = mh_GetUpdatePositionOfNode(orc_NodesUpdateOrder, u32_Pos,
                                                            c_SupDefNodeContent.u32_Position);
         if (s32_Tmp != C_NO_ERR)
         {
            // strange: internal configuration error - should not happen!
            const C_SclString c_Message = "Could not find update position for active node \"" +
                                          C_SclString::IntToStr(u32_Pos) + "\".";
            mhc_WarningMessages.Append(c_Message);
            osc_write_log_warning("Creating Update Package", c_Message);
            s32_Return = C_WARN;
         }
      }

      // get relative application path of node
      // get applications of node
      for (vector<C_SclString>::const_iterator c_IterAppl = orc_ApplicationsToWrite[u32_Pos].c_FilesToFlash.begin();
           c_IterAppl != orc_ApplicationsToWrite[u32_Pos].c_FilesToFlash.end();
           ++c_IterAppl)
      {
         // store application file names with relative path
         const C_SclString c_Tmp = TglExtractFileName(*c_IterAppl);
         c_SupDefNodeContent.c_ApplicationFileNames.push_back(c_Tmp);
      }
      // get parameter sets of node
      for (vector<C_SclString>::const_iterator c_IterParam =
              orc_ApplicationsToWrite[u32_Pos].c_FilesToWriteToNvm.begin();
           c_IterParam != orc_ApplicationsToWrite[u32_Pos].c_FilesToWriteToNvm.end();
           ++c_IterParam)
      {
         // store application file names with relative path
         const C_SclString c_Tmp = TglExtractFileName(*c_IterParam);
         c_SupDefNodeContent.c_NvmFileNames.push_back(c_Tmp);
      }

      // get PEM file and its settings of node
      if (orc_ApplicationsToWrite[u32_Pos].c_PemFile != "")
      {
         const C_SclString c_Tmp = TglExtractFileName(orc_ApplicationsToWrite[u32_Pos].c_PemFile);
         c_SupDefNodeContent.c_PemFile = c_Tmp;

         c_SupDefNodeContent.q_SendSecurityEnabledState = orc_ApplicationsToWrite[u32_Pos].q_SendSecurityEnabledState;
         c_SupDefNodeContent.q_SecurityEnabled = orc_ApplicationsToWrite[u32_Pos].q_SecurityEnabled;
         c_SupDefNodeContent.q_SendDebuggerEnabledState = orc_ApplicationsToWrite[u32_Pos].q_SendDebuggerEnabledState;
         c_SupDefNodeContent.q_DebuggerEnabled = orc_ApplicationsToWrite[u32_Pos].q_DebuggerEnabled;
      }

      c_SupDefContent.c_Nodes.push_back(c_SupDefNodeContent);
   }

   orc_SupDefContent = c_SupDefContent;
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Finds update position of node.

   \param[in]   orc_NodesUpdateOrder   (see function h_CreatePackage)
   \param[in]   ou32_NodeForUpdate     node with applications (!) to update
   \param[out]  oru32_UpdatePosition   update position of node

   \return
   C_NO_ERR     successful
   C_NOACT      node not available in node update order
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_GetUpdatePositionOfNode(const vector<uint32_t> & orc_NodesUpdateOrder,
                                                                       const uint32_t ou32_NodeForUpdate,
                                                                       uint32_t & oru32_UpdatePosition)
{
   int32_t s32_Return = C_NOACT;

   // node position is index of vector orc_NodesUpdateOrder
   for (uint32_t u32_Pos = 0; u32_Pos < orc_NodesUpdateOrder.size(); u32_Pos++)
   {
      if (orc_NodesUpdateOrder[u32_Pos] == ou32_NodeForUpdate)
      {
         // node with position found
         oru32_UpdatePosition = u32_Pos;
         s32_Return = C_NO_ERR;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get sydeSUP secure file names

   \param[in]      orc_SystemDefinition   System definition
   \param[in]      orc_TargetPath         Target path
   \param[in,out]  orc_AbsPath            Abs path
   \param[in,out]  orc_RelPath            Rel path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageCreate::mh_GetSydeSecureFileNames(const C_OscSystemDefinition & orc_SystemDefinition,
                                                                   const C_SclString & orc_TargetPath,
                                                                   std::vector<C_SclString> & orc_AbsPath,
                                                                   std::vector<C_SclString> & orc_RelPath)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < orc_SystemDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      const C_OscNode & rc_Node = orc_SystemDefinition.c_Nodes[u32_ItNode];
      const C_SclString c_RelFile = C_OscUtils::h_NiceifyStringForFileName(
         rc_Node.c_Properties.c_Name) + ".syde_suc";
      const C_SclString c_AbsFile = orc_TargetPath + c_RelFile;
      orc_AbsPath.push_back(c_AbsFile);
      orc_RelPath.push_back(c_RelFile);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Append flash files to secure file sections

   \param[in]      orc_ApplicationsToWrite   Applications to write
   \param[in]      orc_NodeFoldersAbs        Node folders abs
   \param[in,out]  orc_SecureFiles           Secure files
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageCreate::mh_AppendFlashFilesToSecureFileSections(
   const std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
   const std::vector<C_SclString> & orc_NodeFoldersAbs, std::vector<std::set<C_SclString> > & orc_SecureFiles)
{
   tgl_assert(orc_ApplicationsToWrite.size() == orc_NodeFoldersAbs.size());
   if (orc_ApplicationsToWrite.size() == orc_NodeFoldersAbs.size())
   {
      for (uint32_t u32_ItFolder = 0UL; u32_ItFolder < orc_ApplicationsToWrite.size(); ++u32_ItFolder)
      {
         std::set<stw::scl::C_SclString> c_NodeSecFiles;
         const C_OscSuSequences::C_DoFlash c_DoFlash = orc_ApplicationsToWrite[u32_ItFolder]; // current node
         // for service_update_package.syde_supdef we need relative paths!
         //Files
         C_OscZipFile::h_AppendFilesRelative(c_NodeSecFiles, c_DoFlash.c_FilesToFlash,
                                             orc_NodeFoldersAbs[u32_ItFolder]);
         //Parameter set files
         C_OscZipFile::h_AppendFilesRelative(c_NodeSecFiles, c_DoFlash.c_FilesToWriteToNvm,
                                             orc_NodeFoldersAbs[u32_ItFolder]);
         //PEM file
         if (c_DoFlash.c_PemFile != "")
         {
            std::vector<C_SclString> c_PemFiles;
            c_PemFiles.push_back(c_DoFlash.c_PemFile);
            C_OscZipFile::h_AppendFilesRelative(c_NodeSecFiles, c_PemFiles, orc_NodeFoldersAbs[u32_ItFolder]);
         }
         orc_SecureFiles.push_back(c_NodeSecFiles);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create def files and zip secure files

   \param[in]      orc_SystemDefinition      System definition
   \param[in]      orc_SupDefContent         Sup def content
   \param[in]      orc_PackagePathTmp        Package path tmp path
   \param[in]      orc_ActiveNodes           Active nodes
   \param[in]      orc_ApplicationsToWrite   Applications to write
   \param[in,out]  orc_SupFiles              SYDEsup files
   \param[in]      orc_EncryptNodes          list of all nodes contains information which node is encrypted
   \param[in]      orc_EncryptNodesPassword  list of all nodes contains information which node uses which password
   \param[in]      orc_AddSignatureNodes     list of all nodes contains information which node to add signature for
   \param[in]      orc_NodeSignatureKeys     Node signature keys

   \return
   STW error codes

   \retval   C_NO_ERR   No err
   \retval   C_RD_WR    read/write error (see log file)
   \retval   C_CONFIG   Input invalid
   \retval   C_NOACT    could not add data to zip file (does the path to the file exist ?)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_CreateDefFilesAndZipSecureFiles(
   const C_OscSystemDefinition & orc_SystemDefinition, C_OscSupDefinition & orc_SupDefContent,
   const C_SclString & orc_PackagePathTmp, const std::vector<uint8_t> & orc_ActiveNodes,
   const std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
   std::set<stw::scl::C_SclString> & orc_SupFiles, const std::vector<uint8_t> & orc_EncryptNodes,
   const std::vector<stw::scl::C_SclString> & orc_EncryptNodesPassword,
   const std::vector<uint8_t> & orc_AddSignatureNodes, const std::vector<std::vector<uint8_t> > & orc_NodeSignatureKeys)
{
   int32_t s32_Return;

   vector<std::set<stw::scl::C_SclString> > c_SecFiles;
   std::vector<stw::scl::C_SclString> c_SecPackageFilesRel;
   std::vector<stw::scl::C_SclString> c_SecPackageFilesAbs;
   std::vector<stw::scl::C_SclString> c_SecDefFilesRel;
   std::vector<stw::scl::C_SclString> c_SecDefFilesAbs;
   std::vector<stw::scl::C_SclString> c_NodeFoldersRel;
   std::vector<stw::scl::C_SclString> c_NodeFoldersAbs;

   mh_GetNodeFolderNames(orc_SystemDefinition, orc_PackagePathTmp, c_NodeFoldersAbs, c_NodeFoldersRel);
   mh_GetSydeSecureDefFileNames(orc_SystemDefinition, orc_PackagePathTmp, c_SecDefFilesAbs, c_SecDefFilesRel);
   mh_GetSydeSecureFileNames(orc_SystemDefinition, orc_PackagePathTmp, c_SecPackageFilesAbs, c_SecPackageFilesRel);
   mh_AppendFlashFilesToSecureFileSections(orc_ApplicationsToWrite,
                                           c_NodeFoldersAbs, c_SecFiles);
   // create and store 'service_update_package' file
   s32_Return = C_OscSupDefinitionFiler::h_CreateUpdatePackageDefFile(orc_PackagePathTmp, orc_SupDefContent,
                                                                      c_SecPackageFilesRel);
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = mh_HandleNodeDefCreation(orc_ActiveNodes, c_SecDefFilesAbs, c_SecDefFilesRel, orc_AddSignatureNodes,
                                            static_cast<uint32_t>(orc_SystemDefinition.c_Nodes.size()),
                                            orc_SupDefContent.c_Nodes,
                                            c_SecFiles);
   }
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = mh_HandleSignatureCreation(c_NodeFoldersAbs, orc_ActiveNodes, orc_AddSignatureNodes,
                                              orc_NodeSignatureKeys,
                                              static_cast<uint32_t>(orc_SystemDefinition.c_Nodes.size()), c_SecFiles);
   }
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = mh_CreateNodesZip(c_SecFiles, c_SecPackageFilesRel, c_SecPackageFilesAbs, c_NodeFoldersAbs,
                                     orc_ActiveNodes,
                                     orc_EncryptNodes, orc_EncryptNodesPassword,
                                     static_cast<uint32_t>(orc_SystemDefinition.c_Nodes.size()), orc_SupFiles);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create nodes zip

   \param[in]      orc_SecFiles              Sec files
   \param[in]      orc_SecPackageFilesRel    Sec package files rel
   \param[in]      orc_SecPackageFilesAbs    Sec package files abs
   \param[in]      orc_NodeFoldersAbs        Node folders abs
   \param[in]      orc_ActiveNodes           Active nodes
   \param[in]      orc_EncryptNodes          list of all nodes contains information which node is encrypted
   \param[in]      orc_EncryptNodesPassword  list of all nodes contains information which node uses which password
   \param[in]      ou32_NodeCount            Node count
   \param[in,out]  orc_SupFiles              Sup files

   \return
   STW error codes

   \retval   C_NO_ERR    Encrypted zip file generated
   \retval   C_CONFIG    at least one input file does not exist
   \retval   C_RD_WR     could not open input file
                         could not write output file
   \retval   C_NOACT     could not add data to zip file (does the path to the file exist ?)
                         output file could not be written
   \retval   C_BUSY      Problems with deleting the temporary file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_CreateNodesZip(const std::vector<std::set<C_SclString> > & orc_SecFiles,
                                                              const std::vector<C_SclString> & orc_SecPackageFilesRel,
                                                              const std::vector<C_SclString> & orc_SecPackageFilesAbs,
                                                              const std::vector<C_SclString> & orc_NodeFoldersAbs,
                                                              const std::vector<uint8_t> & orc_ActiveNodes,
                                                              const std::vector<uint8_t> & orc_EncryptNodes,
                                                              const std::vector<C_SclString> & orc_EncryptNodesPassword,
                                                              const uint32_t ou32_NodeCount,
                                                              std::set<stw::scl::C_SclString> & orc_SupFiles)
{
   int32_t s32_Return = C_NO_ERR;

   tgl_assert((orc_NodeFoldersAbs.size() == orc_SecFiles.size()) && ((orc_SecPackageFilesRel.size() ==
                                                                      orc_SecFiles.size()) &&
                                                                     ((
                                                                         orc_ActiveNodes.size() ==
                                                                         orc_SecFiles.size()) &&
                                                                      (
                                                                         orc_SecFiles.size() ==
                                                                         orc_SecPackageFilesAbs
                                                                         .size()))));
   if
   ((orc_NodeFoldersAbs.size() == orc_SecFiles.size()) && ((orc_SecPackageFilesRel.size() == orc_SecFiles.size()) &&
                                                           ((
                                                               orc_ActiveNodes.size() == orc_SecFiles.size()) &&
                                                            (
                                                               orc_SecFiles.size() ==
                                                               orc_SecPackageFilesAbs.size()))))
   {
      std::vector<uint8_t> c_EncryptNodes;
      std::vector<stw::scl::C_SclString> c_EncryptNodesPassword;
      mh_AdaptEncryptionParameters(orc_EncryptNodes, orc_EncryptNodesPassword,
                                   ou32_NodeCount, c_EncryptNodes, c_EncryptNodesPassword);
      for (uint32_t u32_File = 0UL; (u32_File < orc_SecFiles.size()) && (s32_Return == C_NO_ERR); ++u32_File)
      {
         if (orc_ActiveNodes[u32_File] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
         {
            if (c_EncryptNodes[u32_File] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
            {
               s32_Return =
                  C_OscAesFile::h_CreateEncryptedZipFile(orc_NodeFoldersAbs[u32_File], orc_SecFiles[u32_File],
                                                         orc_SecPackageFilesAbs[u32_File],
                                                         c_EncryptNodesPassword[u32_File], &mhc_ErrorMessage
                                                         );
            }
            else
            {
               s32_Return =
                  C_OscZipFile::h_CreateZipFile(orc_NodeFoldersAbs[u32_File], orc_SecFiles[u32_File],
                                                orc_SecPackageFilesAbs[u32_File], &mhc_ErrorMessage);
            }
            if (s32_Return == C_NO_ERR)
            {
               orc_SupFiles.insert(orc_SecPackageFilesRel[u32_File]);
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle node def creation

   \param[in]      orc_ActiveNodes        Active nodes
   \param[in]      orc_SecDefFilesAbs     Sec def files abs
   \param[in]      orc_SecDefFilesRel     Sec def files rel
   \param[in]      orc_AddSignatureNodes  list of all nodes contains information which node to add signature for
   \param[in]      ou32_NodeCount         Node count
   \param[in,out]  orc_SupDefNodes        Sup def nodes
   \param[in,out]  orc_SecFiles           Sec files

   \return
   STW error codes

   \retval   C_NO_ERR   No error
   \retval   C_RD_WR    File(s) could not be created
   \retval   C_CONFIG   Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_HandleNodeDefCreation(const std::vector<uint8_t> & orc_ActiveNodes,
                                                                     const std::vector<C_SclString> & orc_SecDefFilesAbs, const std::vector<C_SclString> & orc_SecDefFilesRel, const std::vector<uint8_t> & orc_AddSignatureNodes, const uint32_t ou32_NodeCount, std::vector<C_OscSupNodeDefinition> & orc_SupDefNodes,
                                                                     std::vector<std::set<C_SclString> > & orc_SecFiles)
{
   int32_t s32_Return;

   std::vector<uint8_t> c_AddSignatureNodes;
   const std::vector<std::vector<uint8_t> > c_Unused;
   std::vector<std::vector<uint8_t> > c_Unused2;
   mh_AdaptSignatureParameters(orc_AddSignatureNodes, c_Unused, ou32_NodeCount, c_AddSignatureNodes, c_Unused2);

   //Add sig files to nodes
   tgl_assert((orc_ActiveNodes.size() == orc_SupDefNodes.size()) &&
              (orc_ActiveNodes.size() == c_AddSignatureNodes.size()));
   if ((orc_ActiveNodes.size() == orc_SupDefNodes.size()) && (orc_ActiveNodes.size() == c_AddSignatureNodes.size()))
   {
      for (uint32_t u32_File = 0UL; u32_File < orc_SupDefNodes.size(); ++u32_File)
      {
         if (c_AddSignatureNodes[u32_File] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
         {
            if (orc_ActiveNodes[u32_File] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
            {
               orc_SupDefNodes[u32_File].u8_SignaturePresent = C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE;
               orc_SupDefNodes[u32_File].c_SignatureFile = C_OscSupSignatureFiler::h_GetSignatureFileName();
            }
         }
      }
   }
   s32_Return = C_OscSupNodeDefinitionFiler::h_SaveNodes(orc_SecDefFilesAbs, orc_SupDefNodes);
   //Add new files
   if (s32_Return == C_NO_ERR)
   {
      tgl_assert((orc_ActiveNodes.size() == orc_SecDefFilesRel.size()) &&
                 (orc_SecFiles.size() == orc_SecDefFilesRel.size()));
      if ((orc_ActiveNodes.size() == orc_SecDefFilesRel.size()) &&
          (orc_SecFiles.size() == orc_SecDefFilesRel.size()))
      {
         for (uint32_t u32_File = 0UL; u32_File < orc_SecDefFilesRel.size(); ++u32_File)
         {
            if (orc_ActiveNodes[u32_File] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
            {
               orc_SecFiles[u32_File].insert(orc_SecDefFilesRel[u32_File]);
            }
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle signature creation

   \param[in]      orc_NodeFoldersAbs     Node folders abs
   \param[in]      orc_ActiveNodes        Active nodes
   \param[in]      orc_AddSignatureNodes  list of all nodes contains information which node to add signature for
   \param[in]      orc_NodeSignatureKeys  Node signature keys
   \param[in]      ou32_NodeCount         Node count
   \param[in,out]  orc_SecFiles           Sec files

   \return
   STW error codes

   \retval   C_NO_ERR   No err
   \retval   C_RD_WR    File not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_HandleSignatureCreation(
   const std::vector<C_SclString> & orc_NodeFoldersAbs, const std::vector<uint8_t> & orc_ActiveNodes,
   const std::vector<uint8_t> & orc_AddSignatureNodes, const std::vector<std::vector<uint8_t> > & orc_NodeSignatureKeys,
   const uint32_t ou32_NodeCount, std::vector<std::set<C_SclString> > & orc_SecFiles)
{
   int32_t s32_Retval = C_NO_ERR;

   std::vector<uint8_t> c_AddSignatureNodes;
   std::vector<std::vector<uint8_t> > c_NodeSignatureKeys;
   mh_AdaptSignatureParameters(orc_AddSignatureNodes, orc_NodeSignatureKeys, ou32_NodeCount, c_AddSignatureNodes,
                               c_NodeSignatureKeys);

   tgl_assert((((orc_ActiveNodes.size() == orc_NodeFoldersAbs.size()) &&
                (orc_ActiveNodes.size() == orc_SecFiles.size())) &&
               (orc_ActiveNodes.size() == c_AddSignatureNodes.size())) &&
              (orc_ActiveNodes.size() == c_NodeSignatureKeys.size()));
   if ((((orc_ActiveNodes.size() == orc_NodeFoldersAbs.size()) &&
         (orc_ActiveNodes.size() == orc_SecFiles.size())) &&
        (orc_ActiveNodes.size() == c_AddSignatureNodes.size())) &&
       (orc_ActiveNodes.size() == c_NodeSignatureKeys.size()))
   {
      for (uint32_t u32_File = 0UL; (u32_File < orc_SecFiles.size()) && (s32_Retval == C_NO_ERR); ++u32_File)
      {
         if (c_AddSignatureNodes[u32_File] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
         {
            if (orc_ActiveNodes[u32_File] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
            {
               C_SclString c_Signature;
               s32_Retval = mh_CalcSig(orc_NodeFoldersAbs[u32_File],
                                       orc_SecFiles[u32_File],
                                       c_NodeSignatureKeys[u32_File], c_Signature);

               if (s32_Retval == C_NO_ERR)
               {
                  const C_SclString c_SignatureFilePath = orc_NodeFoldersAbs[u32_File] +
                                                          C_OscSupSignatureFiler::h_GetSignatureFileName();
                  s32_Retval = C_OscSupSignatureFiler::h_CreateSignatureFile(c_SignatureFilePath, c_Signature);
                  if (s32_Retval == C_NO_ERR)
                  {
                     orc_SecFiles[u32_File].insert(C_OscSupSignatureFiler::h_GetSignatureFileName());
                  }
               }
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc sig

   \param[in]      orc_SourcePath   Source path
   \param[in]      orc_SupFiles     Sup files
   \param[in]      orc_Key          Key
   \param[in,out]  orc_Signature    Signature

   \return
   STW error codes

   \retval   C_NO_ERR   No error
   \retval   C_RD_WR    File not found
   \retval   C_NOACT    Could not generate signature
   \retval   C_RANGE    Invalid EC private key (not present or length not correct)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_CalcSig(const C_SclString & orc_SourcePath,
                                                       const std::set<C_SclString> & orc_SupFiles,
                                                       const std::vector<uint8_t> & orc_Key,
                                                       C_SclString & orc_Signature)
{
   int32_t s32_Retval = C_RANGE;

   if (orc_Key.size() == C_OscSecurityEcdsa::hu32_SECP256R1_PRIVATE_KEY_LENGTH)
   {
      uint8_t au8_BinDigest[C_OscSecurityEcdsa::hu32_SHA256_FINAL_LENGTH];

      s32_Retval = C_OscSupServiceUpdatePackageBase::mh_CalcDigest(orc_SourcePath, orc_SupFiles, au8_BinDigest, false);
      if (s32_Retval == C_NO_ERR)
      {
         C_OscSecurityEcdsa::C_Ecdsa256Signature c_Signature;
         uint8_t au8_PrivateKey[C_OscSecurityEcdsa::hu32_SECP256R1_PRIVATE_KEY_LENGTH];

         C_SclString c_Log;
         C_SclString c_Error;

         (void)std::memcpy(&au8_PrivateKey[0], &orc_Key[0], C_OscSecurityEcdsa::hu32_SECP256R1_PRIVATE_KEY_LENGTH);

         s32_Retval =
            C_OscSecurityEcdsa::h_CalcEcdsaSecp256r1Signature(au8_BinDigest, au8_PrivateKey, c_Signature, c_Error);
         if (s32_Retval != C_NO_ERR)
         {
            mhc_ErrorMessage = c_Error;
            osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         }
         else
         {
            s32_Retval = c_Signature.GetAsDerString(orc_Signature);
            tgl_assert(s32_Retval == C_NO_ERR); //the library gave us this signature; why would it not be valid ?

            mh_DigestToString(au8_BinDigest, c_Log);
            osc_write_log_info("Creating Update Package", "security digest: " + c_Log);
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pem file content

   \param[in]      orc_ActiveNodes              Active nodes
   \param[in]      orc_SignatureNodes           Signature nodes
   \param[in]      orc_NodeSignaturePemFiles    Node signature pem files
   \param[in]      ou32_NumNodes                Num nodes
   \param[in,out]  orc_PreparedSignatureNodes   Prepared signature nodes
   \param[in,out]  orc_NodeSignatureKeys        Node signature keys

   \return
   STW error codes

   \retval   C_NO_ERR    Information extracted
   \retval   C_RD_WR     Issue reading pem file
   \retval   C_RD_WR     pem file does not contain a private key with the correct length
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageCreate::mh_GetPemFileContent(const std::vector<uint8_t> & orc_ActiveNodes,
                                                                 const std::vector<uint8_t> & orc_SignatureNodes,
                                                                 const std::vector<C_SclString> & orc_NodeSignaturePemFiles, const uint32_t ou32_NumNodes, std::vector<uint8_t> & orc_PreparedSignatureNodes,
                                                                 std::vector<std::vector<uint8_t> > & orc_NodeSignatureKeys)
{
   int32_t s32_Retval = C_NO_ERR;

   std::vector<C_SclString> c_NodeSignaturePemFiles;
   mh_AdaptPemFileParameters(orc_SignatureNodes, orc_NodeSignaturePemFiles, ou32_NumNodes, c_NodeSignaturePemFiles,
                             orc_PreparedSignatureNodes);
   orc_NodeSignatureKeys.reserve(orc_PreparedSignatureNodes.size());
   tgl_assert(orc_PreparedSignatureNodes.size() == orc_ActiveNodes.size());
   if (orc_PreparedSignatureNodes.size() == orc_ActiveNodes.size())
   {
      for (uint32_t u32_ItNode = 0UL; (u32_ItNode < orc_PreparedSignatureNodes.size()) && (s32_Retval == C_NO_ERR);
           ++u32_ItNode)
      {
         if (orc_ActiveNodes[u32_ItNode] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
         {
            std::string c_Err;
            C_OscSecurityPemSecUpdate c_Pem;
            s32_Retval = c_Pem.LoadFromFile(c_NodeSignaturePemFiles[u32_ItNode].c_str(), c_Err);
            if (s32_Retval != C_NO_ERR)
            {
               mhc_ErrorMessage = c_Err;
               osc_write_log_error("Reading pem files", mhc_ErrorMessage);
            }
            else
            {
               const std::vector<uint8_t> c_PrivateKey = c_Pem.GetKeyInfo().GetPrivateKey();
               if (c_PrivateKey.size() != C_OscSecurityEcdsa::hu32_SECP256R1_PRIVATE_KEY_LENGTH)
               {
                  mhc_ErrorMessage = "ECDSA private key not provided in pem file \"" +
                                     c_NodeSignaturePemFiles[u32_ItNode] + "\" or provided with incorrect length";
                  osc_write_log_error("Reading pem files", mhc_ErrorMessage);
                  s32_Retval = C_RD_WR;
               }
               else
               {
                  orc_NodeSignatureKeys.push_back(c_PrivateKey);
               }
            }
         }
         else
         {
            orc_NodeSignatureKeys.push_back(std::vector<uint8_t>());
         }
      }
   }
   //Remap error
   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_RD_WR;
   }
   return s32_Retval;
}
