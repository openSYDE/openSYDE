//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Service Update Package V1

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

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::diag_lib;
using namespace std;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const C_SclString mc_PACKAGE_EXT = ".syde_sup";
static const C_SclString mc_PACKAGE_EXT_TMP = ".syde_sup_tmp"; // intermediate directory before creating zip archive
static const C_SclString mc_PACKAGE_UPDATE_DEF = "service_update_package.syde_supdef";
static const uint8_t mu8_ACTIVE_NODE = 1U;
static const uint16_t mu16_FILE_VERSION = 1U;
static const C_SclString mc_SUP_SYSDEF = "sup_system_definition.syde_sysdef";
static const C_SclString mc_INI_DEV = "devices.ini";
// XML node names of service update package definition
static const C_SclString mc_ROOT_NAME = "opensyde-updatepack-definition";       // xml root node
static const C_SclString mc_FILE_VERSION = "file-version";                      // xml node
static const C_SclString mc_NODES = "nodes";                                    // xml node
static const C_SclString mc_NODE = "node";                                      // xml node
static const C_SclString mc_NODE_ACTIVE_ATTR = "active";                        // xml node attribute
static const C_SclString mc_NODE_POSITION_ATTR = "position";                    // xml node attribute
static const C_SclString mc_FILES = "files";                                    // xml node
static const C_SclString mc_FILE = "file";                                      // xml node
static const C_SclString mc_PARAM_FILES = "param-files";                        // xml node
static const C_SclString mc_PARAM_FILE = "param-file";                          // xml node
static const C_SclString mc_PEM_FILE_CONFIG = "pem-file-config";                // xml node
static const C_SclString mc_PEM_FILE = "pem-file";                              // xml node
static const C_SclString mc_PEM_FILE_CONFIG_SEC_ENAB_ATTR = "security-enabled"; // xml node attribute
static const C_SclString mc_PEM_FILE_CONFIG_SEC_SEND_ATTR = "security-send";    // xml node attribute
static const C_SclString mc_PEM_FILE_CONFIG_DEB_ENAB_ATTR = "debugger-enabled"; // xml node attribute
static const C_SclString mc_PEM_FILE_CONFIG_DEB_SEND_ATTR = "debugger-send";    // xml node attribute
static const C_SclString mc_FILE_NAME_ATTR = "name";                            // xml node attribute
static const C_SclString mc_BUS_INDEX = "bus-index-client";                     // xml node

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
stw::scl::C_SclStringList C_OscSupServiceUpdatePackageV1::mhc_WarningMessages; // global warnings e.g. if update
                                                                               // position
                                                                               // of
                                                                               // active node is not available
stw::scl::C_SclString C_OscSupServiceUpdatePackageV1::mhc_ErrorMessage;        // description of error which caused the
                                                                               // service update package to fail

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

   \param[in]   orc_PackagePath                 full path of target package (zip archive)
                                                (with constant extension ".syde_sup", e.g. "C:\\012345.syde_sup")
   \param[in]   orc_SystemDefinition            Current system definition (class instance - no file!)
   \param[in]   ou32_ActiveBusIndex             index of bus the client is connected to
   \param[in]   orc_ActiveNodes                 list of all nodes contains information which node is available for update
   \param[in]   orc_NodesUpdateOrder            update order of nodes (index is update position, value is node index)
   \param[in]   orc_ApplicationsToWrite         files for updating nodes
   \param[out]  orc_WarningMessages             warning messages for created package (empty list if no warnings)
   \param[out]  orc_ErrorMessage                error message in case of failure (empty string if no error)
   \param[in]   oq_SaveInCompatibilityFormat    Flag to export in compatibility format (V2)
   \param[in]   oq_SaveAsFile                   True: content of System Update Package is saved in .syde_sup format
                                                False: content of System Update Package is saved to a directory
   \param[in]   orc_TemporaryDirectory          Optional parameter for temporary directory. If empty, temporary directory
                                                is created next to package path.

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
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageV1::h_CreatePackage(const C_SclString & orc_PackagePath,
                                                        const C_OscSystemDefinition & orc_SystemDefinition,
                                                        const uint32_t ou32_ActiveBusIndex,
                                                        const vector<uint8_t> & orc_ActiveNodes,
                                                        const vector<uint32_t> & orc_NodesUpdateOrder,
                                                        const vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite, C_SclStringList & orc_WarningMessages, C_SclString & orc_ErrorMessage, const bool oq_SaveInCompatibilityFormat, const bool oq_SaveAsFile,
                                                        const C_SclString & orc_TemporaryDirectory)
{
   int32_t s32_Return;

   bool q_TemporaryFolderCreated = false; // for cleanup at the end of this function

   mhc_WarningMessages.Clear(); // clear old warning messages
   mhc_ErrorMessage = "";       // clear old error message

   set<C_SclString> c_DeviceDefinitionFiles;                                            // unique container to store
                                                                                        // full device definitions file
                                                                                        // paths
   C_SclString c_PackagePathTmp;                                                        // temporary package path before
                                                                                        // creating zip archive
   const C_SclString c_TargetZipArchive = orc_PackagePath;                              // complete path of target zip
                                                                                        // archive
   vector<C_OscSuSequences::C_DoFlash> c_ApplicationsToWrite = orc_ApplicationsToWrite; // paths of applications
   std::set<stw::scl::C_SclString> c_SupFiles;                                          // unique container with
                                                                                        // relative file paths for zip
                                                                                        // archive
   // fill with constant file names
   c_SupFiles.insert(mc_INI_DEV);
   c_SupFiles.insert(mc_SUP_SYSDEF);
   c_SupFiles.insert(mc_PACKAGE_UPDATE_DEF);

   // precondition checks
   s32_Return = mh_CheckParamsToCreatePackage(orc_PackagePath, orc_SystemDefinition, ou32_ActiveBusIndex,
                                              orc_ActiveNodes, orc_NodesUpdateOrder, orc_ApplicationsToWrite,
                                              oq_SaveAsFile);

   // * create folders with device application files (via h_CreateTemporaryFolder)
   //   h_CreateTemporaryFolder is creating the temporary folder (and deleting in advance if it already exists)
   //   where the other files (devices.ini, system definition etc.) also are placed
   //   and has therefore be the first action for creating service update package
   if (s32_Return == C_NO_ERR)
   {
      stw::scl::C_SclString c_ErrorPath;
      if (oq_SaveAsFile)
      {
         // path for temporary folder to create contents
         if (orc_TemporaryDirectory.IsEmpty() == true)
         {
            c_PackagePathTmp = orc_PackagePath;
         }
         else
         {
            // use sub folder named like package
            c_PackagePathTmp = TglFileIncludeTrailingDelimiter(orc_TemporaryDirectory) +
                               TglExtractFileName(orc_PackagePath);
            // TglExpandFileName() in the linux tgl only works for existing paths, but our path does not yet exist,
            // so we need to expand the path manually
         }
         c_PackagePathTmp = c_PackagePathTmp.SubString(1, c_PackagePathTmp.Pos(mc_PACKAGE_EXT) - 1) +
                            mc_PACKAGE_EXT_TMP;
      }
      else
      {
         // if we want to save the Update Package as a directory, we just use the temp folder, don't rename and delete
         // it. Obviously the zipping does not happen either (see below).
         c_PackagePathTmp = orc_PackagePath.SubString(1, orc_PackagePath.Pos(mc_PACKAGE_EXT) - 1);
      }

      // add trailing path delimiter to temporary folder if not present
      c_PackagePathTmp = TglFileIncludeTrailingDelimiter(c_PackagePathTmp);

      osc_write_log_info("Creating Update Package", "Temporary folder path: " + c_PackagePathTmp);

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
      else
      {
         vector<C_OscSuSequences::C_DoFlash>::const_iterator c_Iter;
         for (c_Iter = c_ApplicationsToWrite.begin(); c_Iter != c_ApplicationsToWrite.end(); ++c_Iter)
         {
            const C_OscSuSequences::C_DoFlash c_DoFlash = *c_Iter; // current node
            // for service_update_package.syde_supdef we need relative paths!
            //Files
            C_OscZipFile::h_AppendFilesRelative(c_SupFiles, c_DoFlash.c_FilesToFlash, c_PackagePathTmp);
            //Parameter set files
            C_OscZipFile::h_AppendFilesRelative(c_SupFiles, c_DoFlash.c_FilesToWriteToNvm, c_PackagePathTmp);
            //PEM file
            if (c_DoFlash.c_PemFile != "")
            {
               std::vector<C_SclString> c_PemFiles;
               c_PemFiles.push_back(c_DoFlash.c_PemFile);
               C_OscZipFile::h_AppendFilesRelative(c_SupFiles, c_PemFiles, c_PackagePathTmp);
            }
         }
      }

      q_TemporaryFolderCreated = true; // at least partly
   }

   // * system definition file
   if (s32_Return == C_NO_ERR)
   {
      // take current system definition of view (is required) and store to file
      const C_SclString c_SysDefPath = c_PackagePathTmp + mc_SUP_SYSDEF;
      if (oq_SaveInCompatibilityFormat == true)
      {
         s32_Return = C_OscSystemDefinitionFilerV2::h_SaveSystemDefinitionFile(orc_SystemDefinition, c_SysDefPath);
      }
      else
      {
         std::vector<C_SclString> c_AdditionalFiles;
         s32_Return = C_OscSystemDefinitionFiler::h_SaveSystemDefinitionFile(orc_SystemDefinition,
                                                                             c_SysDefPath, &c_AdditionalFiles);
         if (s32_Return == C_NO_ERR)
         {
            //Add files to pack
            for (uint32_t u32_ItFile = 0UL; u32_ItFile < c_AdditionalFiles.size(); ++u32_ItFile)
            {
               c_SupFiles.insert(c_AdditionalFiles[u32_ItFile]);
            }
         }
      }

      if (s32_Return != C_NO_ERR)
      {
         // write log
         mhc_ErrorMessage = "Could not save System Definition file to path \"" + c_PackagePathTmp + "\".";
         osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         s32_Return = C_RD_WR;
      }
   }

   // extract information of system definition for device.ini and device definition files
   if (s32_Return == C_NO_ERR)
   {
      // go through nodes in system definition to get device definition names and paths
      for (uint32_t u32_Pos = 0; u32_Pos < orc_SystemDefinition.c_Nodes.size(); u32_Pos++)
      {
         const C_OscDeviceDefinition * const pc_DeviceDefinition =
            orc_SystemDefinition.c_Nodes[u32_Pos].pc_DeviceDefinition;
         tgl_assert(pc_DeviceDefinition != NULL);
         if (pc_DeviceDefinition != NULL)
         {
            const C_SclString c_DevDefPath = pc_DeviceDefinition->c_FilePath;
            c_DeviceDefinitionFiles.insert(c_DevDefPath);
         }
      }

      // * device.ini file
      // create and store specific devices.ini
      s32_Return = mh_CreateDeviceIniFile(c_PackagePathTmp, c_DeviceDefinitionFiles);
   }

   // * device definition files
   // copy device definition files (is safer than using device definition filer):
   // we have to store all device definition files of current system definition
   // because of routing functionality
   if (s32_Return == C_NO_ERR)
   {
      set<C_SclString>::const_iterator c_Iter;
      for (c_Iter = c_DeviceDefinitionFiles.begin();
           (c_Iter != c_DeviceDefinitionFiles.end()) && (s32_Return == C_NO_ERR);
           ++c_Iter)
      {
         const C_SclString c_TargetFileName = TglExtractFileName(*c_Iter);
         c_SupFiles.insert(c_TargetFileName);
         const C_SclString c_TargetFilePath = c_PackagePathTmp + c_TargetFileName;
         s32_Return = C_OscUtils::h_CopyFile(*c_Iter, c_TargetFilePath, NULL, &mhc_ErrorMessage);
         if (s32_Return != C_NO_ERR)
         {
            mhc_ErrorMessage = "Could not save device definition file \"" +
                               TglExtractFileName(c_TargetFilePath) + "\" to path \"" + orc_PackagePath + "\".";
            osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
            s32_Return = C_RD_WR;
         }
      }
   }

   // * service update package file (service_update_package.syde_supdef)
   if (s32_Return == C_NO_ERR)
   {
      C_SupDefContent c_SupDefContent;
      s32_Return = mh_SupDefParamAdapter(orc_SystemDefinition, ou32_ActiveBusIndex,
                                         orc_ActiveNodes, orc_NodesUpdateOrder,
                                         c_ApplicationsToWrite, c_SupDefContent);
      if (s32_Return == C_NO_ERR)
      {
         // create and store 'service_update_package' file
         s32_Return = mh_CreateUpdatePackageDefFile(c_PackagePathTmp, c_SupDefContent);
      }
   }

   //only happens if we want a .syde_sup file. Otherwise the temp folder is our save destination
   //and is has no longer suffix "temp" -> therefore no deleting either
   if (oq_SaveAsFile)
   {
      // package temporary result folder to zip file
      if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
      {
         s32_Return =
            C_OscZipFile::h_CreateZipFile(c_PackagePathTmp, c_SupFiles, c_TargetZipArchive, &mhc_ErrorMessage);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         }
      }

      // cleanup: delete temporary result folder
      if (q_TemporaryFolderCreated == true)
      {
         const int32_t s32_Tmp = TglRemoveDirectory(c_PackagePathTmp, false);
         if (s32_Tmp != 0)
         {
            const C_SclString c_Message = "Could not delete temporary result folder \"" + c_PackagePathTmp + "\".";
            osc_write_log_error("Creating update package", c_Message);
            if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN)) // do not redefine error in case we had even a
                                                                    // problem
                                                                    // earlier
            {
               mhc_ErrorMessage = c_Message;
               s32_Return = C_BUSY;
            }
         }
      }
   }

   orc_WarningMessages = mhc_WarningMessages; // set warning messages for caller
   orc_ErrorMessage = mhc_ErrorMessage;       // set error message for caller

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Unpack an update package

   We have to unzip the package persistent to target folder on disk (orc_TargetUnzipPath) because we need
   the application files for update. If orc_TargetUnzipPath folder exists already it will be deleted.
   Will create the target folder (recursively if it needs to be).

   Result of function after successful execution:
   * unzipped package at given target path with the following contents:
       * system definition as class instance
       * index of bus the client is connected to
       * information which nodes are active or not
       * update order of active nodes
       * path of application files for active nodes

   Assumptions:
   * valid and compatible service update package created by C_OscSuServiceUpdatePackage::h_CreatePackage function
   * write permission of target path for unzipped package

   \param[in]   orc_PackagePath           full path of package
                                          (with constant extension ".syde_sup", e.g. 012345.syde_sup)
   \param[in]   orc_TargetUnzipPath       folder of path to unzip the service update package
                                          (works with or without trailing path delimiter)
   \param[out]  orc_SystemDefinition      system definition (class instance - no file!)
   \param[out]  oru32_ActiveBusIndex      index of bus the client is connected to
   \param[out]  orc_ActiveNodes           list of all nodes contains information which node is available for update
   \param[out]  orc_NodesUpdateOrder      update order of active nodes
   \param[out]  orc_ApplicationsToWrite   files for updating nodes
   \param[out]  orc_WarningMessages       warning messages for imported package (empty list if no warnings)
   \param[out]  orc_ErrorMessage          error message in case of failure (empty string if no error)
   \param[in]   oq_IsZip                  true: given package is ".syde_sup" format
                                          false: given package is just a plain directory

   \return
   C_NO_ERR    success
   C_CONFIG    could not find update package archive or update package directory
   C_RD_WR     could not unzip update package from disk to target path
   C_BUSY      could not erase pre-existing target path (note: can result in partially erased target path)
   C_RANGE     error code of a called core function (should not occur for valid and compatible service update package)
   C_NOACT     error code of a called core function (should not occur for valid and compatible service update package)
   C_OVERFLOW  error code of a called core function (should not occur for valid and compatible service update package)
   C_DEFAULT   error code of a called core function (should not occur for valid and compatible service update package)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageV1::h_ProcessPackage(const C_SclString & orc_PackagePath,
                                                         const C_SclString & orc_TargetUnzipPath,
                                                         C_OscSystemDefinition & orc_SystemDefinition,
                                                         uint32_t & oru32_ActiveBusIndex,
                                                         vector<uint8_t> & orc_ActiveNodes,
                                                         vector<uint32_t> & orc_NodesUpdateOrder,
                                                         vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                                         C_SclStringList & orc_WarningMessages,
                                                         C_SclString & orc_ErrorMessage, const bool oq_IsZip)
{
   int32_t s32_Return = C_NO_ERR;

   C_SclString c_TargetUnzipPath;
   C_SclString c_PackagePath;

   mhc_WarningMessages.Clear(); // clear old warning messages
   mhc_ErrorMessage = "";       // clear old error message

   if (orc_TargetUnzipPath != "")
   {
      // add trailing path delimiter in case there is none
      c_TargetUnzipPath = TglFileIncludeTrailingDelimiter(orc_TargetUnzipPath);
   }
   else
   {
      c_TargetUnzipPath = "";
   }

   // as a precaution of inconsistent data, clear the input container
   orc_ActiveNodes.clear();
   orc_NodesUpdateOrder.clear();
   orc_ApplicationsToWrite.clear();

   if (oq_IsZip)
   {
      // check if zip archive exists
      if (TglFileExists(orc_PackagePath) == false)
      {
         mhc_ErrorMessage = "Zip archive \"" + orc_PackagePath + "\" does not exist.";
         osc_write_log_error("Unpacking Update Package", mhc_ErrorMessage);
         s32_Return = C_CONFIG;
      }

      //erase target path if it exists
      if (s32_Return == C_NO_ERR)
      {
         if (TglDirectoryExists(c_TargetUnzipPath) == true)
         {
            s32_Return = TglRemoveDirectory(c_TargetUnzipPath, false);
            if (s32_Return != 0)
            {
               mhc_ErrorMessage = "Could not remove folder \"" + c_TargetUnzipPath +
                                  "\" to extract contents of zip archive.";
               osc_write_log_error("Unpacking Update Package", mhc_ErrorMessage);
               s32_Return = C_BUSY;
            }
         }
      }

      // create target unzip path
      if (s32_Return == C_NO_ERR)
      {
         //create target folder (from bottom-up if required):
         s32_Return = C_OscUtils::h_CreateFolderRecursively(c_TargetUnzipPath);
         if (s32_Return != C_NO_ERR)
         {
            mhc_ErrorMessage = "Could not create folder \"" + c_TargetUnzipPath + "\" for zip archive.";
            osc_write_log_error("Unpacking Update Package", mhc_ErrorMessage);
            s32_Return = C_RD_WR;
         }
      }

      // open zip file and unpack contents to target folder
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = C_OscZipFile::h_UnpackZipFile(orc_PackagePath, c_TargetUnzipPath, &mhc_ErrorMessage);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Unpacking Update Package", mhc_ErrorMessage);
         }
      }

      //check if all files are present
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = C_OscSupServiceUpdatePackageV1::mh_CheckSupFiles(c_TargetUnzipPath);
         if (s32_Return != C_NO_ERR)
         {
            mhc_ErrorMessage = "Could not find necessary files within \"" + c_TargetUnzipPath +
                               "\" for update package to be complete.";
            osc_write_log_error("Unpacking Update Package", mhc_ErrorMessage);
         }
      }
   }
   else
   {
      //no zip -> we have a plain directory. Does it even exist?
      if (TglDirectoryExists(orc_PackagePath) == true)
      {
         //We need the base path for handling relative paths of files to transfer:
         c_TargetUnzipPath = TglFileIncludeTrailingDelimiter(orc_PackagePath);

         //check if necessary files are present
         s32_Return = C_OscSupServiceUpdatePackageV1::mh_CheckSupFiles(orc_PackagePath);
         if (s32_Return != C_NO_ERR)
         {
            mhc_ErrorMessage = "Could not find necessary files within \"" + orc_PackagePath +
                               "\" for update package to be complete.";
            osc_write_log_error("Processing Update Package", mhc_ErrorMessage);
         }
      }
      else
      {
         mhc_ErrorMessage = "Directory \"" + orc_PackagePath + "\" does not exist.";
         osc_write_log_error("Processing Update Package", mhc_ErrorMessage);
         s32_Return = C_CONFIG;
      }
   }

   // load service update package definition
   if (s32_Return == C_NO_ERR)
   {
      C_OscXmlParser c_XmlParser;

      if (oq_IsZip)
      {
         c_XmlParser.LoadFromFile(c_TargetUnzipPath + mc_PACKAGE_UPDATE_DEF);
      }
      else
      {
         c_PackagePath = TglFileIncludeTrailingDelimiter(orc_PackagePath);
         c_XmlParser.LoadFromFile(c_PackagePath + mc_PACKAGE_UPDATE_DEF);
      }

      tgl_assert(c_XmlParser.SelectRoot() == mc_ROOT_NAME); // we shall have a valid and compatible update package

      // active bus index
      tgl_assert(c_XmlParser.SelectNodeChild(mc_BUS_INDEX) == mc_BUS_INDEX);
      const C_SclString c_BusIndex = c_XmlParser.GetNodeContent();
      oru32_ActiveBusIndex = static_cast<uint32_t>(c_BusIndex.ToInt());

      // get active nodes with update positions and files to flash

      tgl_assert(c_XmlParser.SelectRoot() == mc_ROOT_NAME); // we shall have a valid and compatible update package
      tgl_assert(c_XmlParser.SelectNodeChild(mc_NODES) == mc_NODES);

      tgl_assert(c_XmlParser.SelectNodeChild(mc_NODE) == mc_NODE);

      // go through all nodes
      C_SclString c_SelectedNode;
      map<uint32_t, uint32_t> c_UpdateOrderByNodes; // to store node update positions which are represented by index at
      // orc_NodesUpdateOrder
      uint32_t u32_NodeCounter = 0;
      do
      {
         // get content of node
         const uint8_t u8_NodeActive = static_cast<uint8_t>(c_XmlParser.GetAttributeUint32(mc_NODE_ACTIVE_ATTR));
         orc_ActiveNodes.push_back(u8_NodeActive);
         C_OscSuSequences::C_DoFlash c_DoFlash;
         if (u8_NodeActive == mu8_ACTIVE_NODE)
         {
            // get update position
            const uint32_t u32_UpdatePosition = static_cast<uint8_t>(
               c_XmlParser.GetAttributeUint32(mc_NODE_POSITION_ATTR));
            C_OscSupServiceUpdatePackageV1::mh_LoadFilesSection(c_DoFlash.c_FilesToFlash, u32_NodeCounter,
                                                                u32_UpdatePosition, c_UpdateOrderByNodes,
                                                                c_TargetUnzipPath,
                                                                c_XmlParser, mc_FILES, mc_FILE);
            C_OscSupServiceUpdatePackageV1::mh_LoadFilesSection(c_DoFlash.c_FilesToWriteToNvm, u32_NodeCounter,
                                                                u32_UpdatePosition, c_UpdateOrderByNodes,
                                                                c_TargetUnzipPath,
                                                                c_XmlParser, mc_PARAM_FILES, mc_PARAM_FILE);
            C_OscSupServiceUpdatePackageV1::mh_LoadPemConfigSection(c_DoFlash, u32_NodeCounter,
                                                                    u32_UpdatePosition, c_UpdateOrderByNodes,
                                                                    c_TargetUnzipPath,
                                                                    c_XmlParser);
         }
         u32_NodeCounter++;                            // next active node
         orc_ApplicationsToWrite.push_back(c_DoFlash); // push back in any case even if we have no applications
                                                       // because of an inactive device
         //Next
         c_SelectedNode = c_XmlParser.SelectNodeNext(mc_NODE);
      }
      while (c_SelectedNode == mc_NODE);

      mh_SetNodesUpdateOrder(c_UpdateOrderByNodes, orc_NodesUpdateOrder);

      // load system definition (has constant name) for active nodes
      C_SclString c_SysDefPath;
      C_SclString c_DevIniPath;
      if (oq_IsZip)
      {
         c_SysDefPath = c_TargetUnzipPath + mc_SUP_SYSDEF;
         c_DevIniPath = c_TargetUnzipPath + mc_INI_DEV;
      }
      else
      {
         c_SysDefPath = c_PackagePath + mc_SUP_SYSDEF;
         c_DevIniPath = c_PackagePath + mc_INI_DEV;
      }

      s32_Return = C_OscSystemDefinitionFiler::h_LoadSystemDefinitionFile(orc_SystemDefinition, c_SysDefPath,
                                                                          c_DevIniPath, true, NULL, &orc_ActiveNodes,
                                                                          true); // skip content
   }

   // get "other accepted names" for active nodes
   if (s32_Return == C_NO_ERR)
   {
      for (uint8_t u8_Node = 0U; u8_Node < orc_SystemDefinition.c_Nodes.size(); u8_Node++)
      {
         if ((orc_ActiveNodes[u8_Node] == 1U) &&
             (orc_SystemDefinition.c_Nodes[u8_Node].pc_DeviceDefinition != NULL))
         {
            const C_OscNode & rc_CurNode = orc_SystemDefinition.c_Nodes[u8_Node];
            tgl_assert(rc_CurNode.u32_SubDeviceIndex < rc_CurNode.pc_DeviceDefinition->c_SubDevices.size());

            orc_ApplicationsToWrite[u8_Node].c_OtherAcceptedDeviceNames =
               rc_CurNode.pc_DeviceDefinition->c_SubDevices[rc_CurNode.u32_SubDeviceIndex].c_OtherAcceptedNames;
         }
      }
   }

   orc_WarningMessages = mhc_WarningMessages; // set warning messages for caller
   orc_ErrorMessage = mhc_ErrorMessage;       // set error message for caller

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns fix defined service update package extension.

   \return
   service update package extension
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscSupServiceUpdatePackageV1::h_GetPackageExtension()
{
   return mc_PACKAGE_EXT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  This function shall check if certain files are existent in the update package. (internal function)

   The update package can be a created directory after unpacking a ".syde_sup" file or a directory itself,
   if there was no ".syde_sup" file.   In order to successfully perform the actual update we need to make sure that
   whether the "syde_sup" nor the plain directory got "corrupted". There are some defensive checks for files which are
   necessary to perform an update routine and if those are missing, we want to handle this gracefully.
   Files we need to make sure are present:
      * ".syde_sysdef" //the expected name MUST NOT be changed
      * ".syde_defsup" //the expected name MUST NOT be changed
      * devices.ini    //the expected name MUST NOT be changed
   Those are the absolute basics we need. This is an all or nothing approach. If one file is missing -> fail.

   This function does not claim to be 100% defensive. We surely could go more into detail regarding the checks for
   certain files, but for now it looks like the existing engines handle everything gracefully.

   \param[in]       orc_PackagePath     path to unzipped package folder or plain directory

   \retval   C_NO_ERR   directory contains all necessary files
   \retval   C_DEFAULT  at least one file is missing in given directory
                        (due to lack of alternatives C_DEFAULT was chosen to have a unique error to redirect to
                         SYDEsup specific error codes in C_SYDEsup::Update)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageV1::mh_CheckSupFiles(const C_SclString & orc_PackagePath)
{
   int32_t s32_Return = C_NO_ERR;

   C_SclDynamicArray<C_TglFileSearchRecord> c_Files; //storage for found files
   std::vector<C_SclString> c_NecessaryFiles;        //those are the files we look for

   c_NecessaryFiles.push_back(mc_PACKAGE_UPDATE_DEF); //".syde_supdef"
   c_NecessaryFiles.push_back(mc_SUP_SYSDEF);         //".syde_sysdef"
   c_NecessaryFiles.push_back(mc_INI_DEV);            //"devices.ini"

   for (uint32_t u32_It = 0; u32_It < c_NecessaryFiles.size(); ++u32_It)
   {
      if (s32_Return == C_DEFAULT)
      {
         break;
      }
      const C_SclString c_FileExt = TglExtractFileExtension(c_NecessaryFiles[u32_It]);
      //define search pattern for TGL_FileFind including the package path, otherwise function would search the whole
      //system. This probably would slow us down. We first search for a certain extension and in second step for
      //specific name.
      const C_SclString c_SearchPattern = TglFileIncludeTrailingDelimiter(orc_PackagePath) + "*" + c_FileExt;

      TglFileFind(c_SearchPattern, c_Files);

      //only one of the specified files is allowed
      if (c_Files.GetLength() == 1)
      {
         //if the correct amount of files is present, we need to have a match on the exact name, otherwise -> fail.
         if (c_Files[0].c_FileName != c_NecessaryFiles[u32_It])
         {
            s32_Return = C_DEFAULT;
         }
      }
      else
      {
         //more than one file of the specified type, smells fishy -> abort
         s32_Return = C_DEFAULT;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Precondition checks for creating update package (internal function).

   \param[in]  orc_PackagePath            (see function h_CreatePackage)
   \param[in]  orc_SystemDefinition       (see function h_CreatePackage)
   \param[in]  ou32_ActiveBusIndex        (see function h_CreatePackage)
   \param[in]  orc_ActiveNodes            (see function h_CreatePackage)
   \param[in]  orc_NodesUpdateOrder       (see function h_CreatePackage)
   \param[in]  orc_ApplicationsToWrite    (see function h_CreatePackage)
   \param[in]  oq_SaveAsFile              (see function h_CreatePackage)

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
int32_t C_OscSupServiceUpdatePackageV1::mh_CheckParamsToCreatePackage(const C_SclString & orc_PackagePath,
                                                                      const C_OscSystemDefinition & orc_SystemDefinition, const uint32_t ou32_ActiveBusIndex, const vector<uint8_t> & orc_ActiveNodes, const vector<uint32_t> & orc_NodesUpdateOrder, const vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                                                      const bool oq_SaveAsFile)
{
   int32_t s32_Return = C_NO_ERR;

   // does package already exist ?
   const bool q_FileExists = TglFileExists(orc_PackagePath);
   const bool q_DirectoryExists = TglDirectoryExists(orc_PackagePath);

   if ((q_FileExists == true) || (q_DirectoryExists == true))
   {
      mhc_ErrorMessage = "Update Package \"" + orc_PackagePath + "\" exists already.";
      osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
      s32_Return = C_RANGE;
   }

   // does target directory for package exist ?
   if (s32_Return == C_NO_ERR)
   {
      const C_SclString c_TargetDir = TglExtractFilePath(orc_PackagePath);

      // package name without path or './' leads to target directory "",
      // which should not lead to error as it will result in creating package next to executable
      if ((c_TargetDir.IsEmpty() == false) && (TglDirectoryExists(c_TargetDir) == false))
      {
         mhc_ErrorMessage = "Target directory \"" + c_TargetDir + "\" does not exist.";
         osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         s32_Return = C_RANGE;
      }
   }

   // valid package name? Only relevant if we save SUP as file
   if ((s32_Return == C_NO_ERR) && (oq_SaveAsFile == true))
   {
      // package name must contain ".syde_sup" and ".syde_sup" must be the last characters of the given path
      if ((orc_PackagePath.Pos(mc_PACKAGE_EXT) == 0) ||
          (orc_PackagePath.Length() != ((orc_PackagePath.Pos(mc_PACKAGE_EXT) + mc_PACKAGE_EXT.Length()) - 1)))
      {
         mhc_ErrorMessage = "Package name must have extension \"" + mc_PACKAGE_EXT + "\".";
         osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         s32_Return = C_RANGE;
      }
   }

   // does temporary result folder exist ? (would be really strange, but who knows)
   if (s32_Return == C_NO_ERR)
   {
      C_SclString c_TmpPackagePath = orc_PackagePath.SubString(1, orc_PackagePath.Pos(mc_PACKAGE_EXT) - 1) +
                                     mc_PACKAGE_EXT_TMP;
      // add trailing path delimiter to temporary folder if not present
      c_TmpPackagePath = TglFileIncludeTrailingDelimiter(c_TmpPackagePath);
      if ((TglFileExists(c_TmpPackagePath) == true) || (TglDirectoryExists(c_TmpPackagePath) == true))
      {
         mhc_ErrorMessage = "Temporary result folder \"" + c_TmpPackagePath +
                            "\" to create zip archive already exists.";
         osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
         s32_Return = C_RANGE;
      }
   }

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
            if (orc_ActiveNodes[u32_Pos] == mu8_ACTIVE_NODE)
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

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates update package definition file (internal function).

   Version 1.0

   Assumptions:
   * write permission of target folder

   \param[in]  orc_Path             destination path
   \param[in]  orc_SupDefContent    content to write in service update package xml file

   \return
   C_NO_ERR    success
   C_RD_WR     read/write error (see log file)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageV1::mh_CreateUpdatePackageDefFile(const C_SclString & orc_Path,
                                                                      const C_SupDefContent & orc_SupDefContent)
{
   const C_SclString c_FileName = TglFileIncludeTrailingDelimiter(orc_Path) + mc_PACKAGE_UPDATE_DEF;
   int32_t s32_Result;

   // fill update package definition
   C_OscXmlParser c_XmlParser;

   //Root Node
   c_XmlParser.CreateNodeChild(mc_ROOT_NAME);
   tgl_assert(c_XmlParser.SelectRoot() == mc_ROOT_NAME);

   //File version
   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(mc_FILE_VERSION) == mc_FILE_VERSION);
   c_XmlParser.SetNodeContent(C_SclString::IntToStr(mu16_FILE_VERSION));

   //Return
   tgl_assert(c_XmlParser.SelectNodeParent() == mc_ROOT_NAME);

   //Nodes
   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(mc_NODES) == mc_NODES);

   //List nodes
   for (uint32_t u32_Pos = 0; u32_Pos < orc_SupDefContent.c_Nodes.size(); u32_Pos++)
   {
      const C_SupDefNodeContent c_CurrentNode = orc_SupDefContent.c_Nodes[u32_Pos];

      //Node
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild(mc_NODE) == mc_NODE);
      c_XmlParser.SetAttributeUint32(mc_NODE_ACTIVE_ATTR, static_cast<uint32_t>(c_CurrentNode.u8_Active));

      // active node?
      if (c_CurrentNode.u8_Active == mu8_ACTIVE_NODE)
      {
         // if there are files to update for active node then list files
         if ((c_CurrentNode.c_ApplicationFileNames.size() > 0) ||
             (c_CurrentNode.c_NvmFileNames.size() > 0) ||
             (c_CurrentNode.c_PemFile != ""))
         {
            //Update Position
            c_XmlParser.SetAttributeUint32(mc_NODE_POSITION_ATTR, c_CurrentNode.u32_Position);
         }

         mh_SaveFiles(c_CurrentNode.c_ApplicationFileNames, c_XmlParser, mc_FILES, mc_FILE);
         mh_SaveFiles(c_CurrentNode.c_NvmFileNames, c_XmlParser, mc_PARAM_FILES, mc_PARAM_FILE);
         mh_SavePemConfig(c_CurrentNode, c_XmlParser);

         tgl_assert(c_XmlParser.SelectNodeParent() == mc_NODES);
      }
      else
      {
         //Return for next node
         tgl_assert(c_XmlParser.SelectNodeParent() == mc_NODES);
      }
   }

   //Return
   tgl_assert(c_XmlParser.SelectNodeParent() == mc_ROOT_NAME);

   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(mc_BUS_INDEX) == mc_BUS_INDEX);
   c_XmlParser.SetNodeContent(C_SclString::IntToStr(orc_SupDefContent.u32_ActiveBusIndex));

   //Return
   tgl_assert(c_XmlParser.SelectNodeParent() == mc_ROOT_NAME);

   // save update package definition file
   s32_Result = c_XmlParser.SaveToFile(c_FileName);
   if (s32_Result != C_NO_ERR)
   {
      s32_Result = C_RD_WR;
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates specific device definition (internal function).

   C_OscSystemDefinitionFiler::h_LoadSystemDefinitionFile needs device definition.
   Because we don't want a generic device definition of all devices in the service update
   package, a specific one is created.

   Assumptions:
   * write permission of target folder
   * valid path (should be OK for an internal function)

   \param[in]  orc_Path                   destination path
   \param[in]  orc_DeviceDefinitionPaths  container of full device definition file paths

   \return
   C_NO_ERR    success
   C_RD_WR     read/write error (see log file)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageV1::mh_CreateDeviceIniFile(const C_SclString & orc_Path,
                                                               const set<C_SclString> & orc_DeviceDefinitionPaths)
{
   int32_t s32_Return = C_NO_ERR;

   const C_SclString c_HEAD_SECTION = "DeviceTypes"; //                      -"-
   const C_SclString c_FIRST_KEY = "NumTypes";       //                      -"-
   const int32_t s32_FIRST_VALUE = 1;                //                      -"-
   const C_SclString c_SECOND_KEY = "TypeName1";     //                      -"-
   const C_SclString c_DEVICE_SECTION = "UsedDevices";
   const C_SclString c_DEVICE_COUNT = "DeviceCount";
   const C_SclString c_DEVICE_KEY = "Device";
   const C_SclString c_IniDevPath = TglFileIncludeTrailingDelimiter(orc_Path) + mc_INI_DEV;

   // build up devices.ini --> device definitions are in the same folder
   try
   {
      C_SclIniFile c_IniFile(c_IniDevPath); // devices.ini
      uint32_t u32_DeviceCounter = 1;
      // write header section
      c_IniFile.WriteInteger(c_HEAD_SECTION, c_FIRST_KEY, s32_FIRST_VALUE);
      c_IniFile.WriteString(c_HEAD_SECTION, c_SECOND_KEY, c_DEVICE_SECTION);

      // write content section
      c_IniFile.WriteInteger(c_DEVICE_SECTION, c_DEVICE_COUNT, static_cast<int32_t>(orc_DeviceDefinitionPaths.size()));
      // fill up with device definitions
      set<C_SclString>::const_iterator c_Iter;
      for (c_Iter = orc_DeviceDefinitionPaths.begin(); c_Iter != orc_DeviceDefinitionPaths.end(); ++c_Iter)
      {
         const C_SclString c_Key = c_DEVICE_KEY + C_SclString::IntToStr(u32_DeviceCounter);
         const C_SclString c_Value = TglExtractFileName(*c_Iter);
         c_IniFile.WriteString(c_DEVICE_SECTION, c_Key, c_Value);
         u32_DeviceCounter++;
      }
      c_IniFile.UpdateFile(); // make data persistent
   }
   catch (...)
   {
      mhc_ErrorMessage = "Could not write devices.ini to path \"" + orc_Path + "\"";
      osc_write_log_error("Creating Update Package", mhc_ErrorMessage);
      s32_Return = C_RD_WR;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Helper function to convert parameters for service update package (internal function).

   Assumptions:
   * checked parameters

   \param[in]   orc_SystemDefinition      (see function h_CreatePackage)
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
int32_t C_OscSupServiceUpdatePackageV1::mh_SupDefParamAdapter(const C_OscSystemDefinition & orc_SystemDefinition,
                                                              const uint32_t ou32_ActiveBusIndex,
                                                              const vector<uint8_t> & orc_ActiveNodes,
                                                              const vector<uint32_t> & orc_NodesUpdateOrder,
                                                              const vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                                              C_OscSupServiceUpdatePackageV1::C_SupDefContent & orc_SupDefContent)
{
   int32_t s32_Return = C_NO_ERR;
   C_SupDefContent c_SupDefContent;

   // get general content
   c_SupDefContent.u32_ActiveBusIndex = ou32_ActiveBusIndex;

   // get contents for each node
   for (uint32_t u32_Pos = 0; u32_Pos < orc_ActiveNodes.size(); u32_Pos++)
   {
      C_SupDefNodeContent c_SupDefNodeContent;
      c_SupDefNodeContent.u8_Active = orc_ActiveNodes[u32_Pos];
      // in case we have an active node and (!) application(s) for update are available
      if ((c_SupDefNodeContent.u8_Active == mu8_ACTIVE_NODE) &&
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
      const C_SclString c_Folder =
         C_OscUtils::h_NiceifyStringForFileName(orc_SystemDefinition.c_Nodes[u32_Pos].c_Properties.c_Name);
      // get applications of node
      for (vector<C_SclString>::const_iterator c_IterAppl = orc_ApplicationsToWrite[u32_Pos].c_FilesToFlash.begin();
           c_IterAppl != orc_ApplicationsToWrite[u32_Pos].c_FilesToFlash.end();
           ++c_IterAppl)
      {
         // store application file names with relative path
         const C_SclString c_Tmp = c_Folder + "/" + TglExtractFileName(*c_IterAppl);
         c_SupDefNodeContent.c_ApplicationFileNames.push_back(c_Tmp);
      }
      // get parameter sets of node
      for (vector<C_SclString>::const_iterator c_IterParam =
              orc_ApplicationsToWrite[u32_Pos].c_FilesToWriteToNvm.begin();
           c_IterParam != orc_ApplicationsToWrite[u32_Pos].c_FilesToWriteToNvm.end();
           ++c_IterParam)
      {
         // store application file names with relative path
         const C_SclString c_Tmp = c_Folder + "/" + TglExtractFileName(*c_IterParam);
         c_SupDefNodeContent.c_NvmFileNames.push_back(c_Tmp);
      }

      // get PEM file and its settings of node
      if (orc_ApplicationsToWrite[u32_Pos].c_PemFile != "")
      {
         const C_SclString c_Tmp = c_Folder + "/" + TglExtractFileName(orc_ApplicationsToWrite[u32_Pos].c_PemFile);
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
int32_t C_OscSupServiceUpdatePackageV1::mh_GetUpdatePositionOfNode(const vector<uint32_t> & orc_NodesUpdateOrder,
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
/*! \brief   Adapts nodes update order by interchanging index with value to match openSYDE API functions.

   \param[in]   orc_UpdateOrderByNodes    key is node position, value is update position
   \param[out]  orc_NodesUpdateOrder      update position is index and value is node position

   \return
   C_NO_ERR   successful
   C_WARN     contains no nodes for update order
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageV1::mh_SetNodesUpdateOrder(const map<uint32_t, uint32_t> & orc_UpdateOrderByNodes,
                                                               vector<uint32_t> & orc_NodesUpdateOrder)
{
   int32_t s32_Return = 0;

   if (orc_UpdateOrderByNodes.size() == 0)
   {
      s32_Return = C_WARN;
   }
   else
   {
      orc_NodesUpdateOrder.resize(orc_UpdateOrderByNodes.size());
      map<uint32_t, uint32_t>::const_iterator c_Iter;
      for (c_Iter = orc_UpdateOrderByNodes.begin(); c_Iter != orc_UpdateOrderByNodes.end(); ++c_Iter)
      {
         // interchange index with value
         orc_NodesUpdateOrder[c_Iter->second] = c_Iter->first;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load files section from service update package definition file

   \param[in,out]  orc_Files              Files vector to append to
   \param[in]      ou32_NodeCounter       Current node index
   \param[in]      ou32_UpdatePos         Current update position
   \param[in,out]  orc_PositionMap        Map for node indices and update positions
   \param[in]      orc_TargetUnzipPath    Path where all the files will be unzipped to
   \param[in,out]  orc_XmlParser          XMLParser for service update package definition file
   \param[in]      orc_BaseNodeName       XML node name to use on base level
   \param[in]      orc_ElementNodeName    XML node name to use on item level
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageV1::mh_LoadFilesSection(std::vector<C_SclString> & orc_Files,
                                                         const uint32_t ou32_NodeCounter, const uint32_t ou32_UpdatePos,
                                                         std::map<uint32_t, uint32_t> & orc_PositionMap,
                                                         const C_SclString & orc_TargetUnzipPath,
                                                         C_OscXmlParserBase & orc_XmlParser,
                                                         const C_SclString & orc_BaseNodeName,
                                                         const C_SclString & orc_ElementNodeName)
{
   if (orc_XmlParser.SelectNodeChild(orc_BaseNodeName) == orc_BaseNodeName)
   {
      C_SclString c_SelectedNode;
      // node has applications to update
      orc_PositionMap.insert(std::pair<uint32_t, uint32_t>(ou32_NodeCounter, ou32_UpdatePos));
      // get update application paths
      tgl_assert(orc_XmlParser.SelectNodeChild(orc_ElementNodeName) == orc_ElementNodeName);

      // go through all files
      do
      {
         // we have to take care of OS dependent path delimiters for windows '\\'
         const C_SclString c_XmlAttr = orc_XmlParser.GetAttributeString(mc_FILE_NAME_ATTR);
         const C_SclString c_FileName = TglExtractFileName(c_XmlAttr);
         // subfolder without '/'
         const C_SclString c_SubFolder =
            c_XmlAttr.SubString(1, (c_XmlAttr.Length() - c_FileName.Length()) - 1);
         const C_SclString c_FilePath = TglFileIncludeTrailingDelimiter(
            orc_TargetUnzipPath + c_SubFolder) + c_FileName;
         orc_Files.push_back(c_FilePath);
         c_SelectedNode = orc_XmlParser.SelectNodeNext(orc_ElementNodeName);
      }
      while (c_SelectedNode == orc_ElementNodeName);
      tgl_assert(orc_XmlParser.SelectNodeParent() == orc_BaseNodeName);
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_NODE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load PEM configuration section from service update package definition file

   \param[in,out]  orc_DoFlash            Node configuration with PEM configuration
   \param[in]      ou32_NodeCounter       Current node index
   \param[in]      ou32_UpdatePos         Current update position
   \param[in,out]  orc_PositionMap        Map for node indices and update positions
   \param[in]      orc_TargetUnzipPath    Path where all the files will be unzipped to
   \param[in,out]  orc_XmlParser          XMLParser for service update package definition file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageV1::mh_LoadPemConfigSection(C_OscSuSequences::C_DoFlash & orc_DoFlash,
                                                             const uint32_t ou32_NodeCounter,
                                                             const uint32_t ou32_UpdatePos, std::map<uint32_t,
                                                                                                     uint32_t> & orc_PositionMap, const C_SclString & orc_TargetUnzipPath,
                                                             C_OscXmlParserBase & orc_XmlParser)
{
   if (orc_XmlParser.SelectNodeChild(mc_PEM_FILE_CONFIG) == mc_PEM_FILE_CONFIG)
   {
      // get PEM file path
      tgl_assert(orc_XmlParser.SelectNodeChild(mc_PEM_FILE) == mc_PEM_FILE);

      // we have to take care of OS dependent path delimiters for windows '\\'
      const C_SclString c_XmlAttr = orc_XmlParser.GetAttributeString(mc_FILE_NAME_ATTR);
      if (c_XmlAttr != "")
      {
         const C_SclString c_FileName = TglExtractFileName(c_XmlAttr);
         // subfolder without '/'
         const C_SclString c_SubFolder =
            c_XmlAttr.SubString(1, (c_XmlAttr.Length() - c_FileName.Length()) - 1);
         const C_SclString c_FilePath = TglFileIncludeTrailingDelimiter(
            orc_TargetUnzipPath + c_SubFolder) + c_FileName;
         orc_DoFlash.c_PemFile = c_FilePath;

         // node has applications to update
         orc_PositionMap.insert(std::pair<uint32_t, uint32_t>(ou32_NodeCounter, ou32_UpdatePos));
      }

      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_PEM_FILE_CONFIG);

      if (orc_DoFlash.c_PemFile != "")
      {
         // Only in case of a PEM file, the states are relevant
         orc_DoFlash.q_SendSecurityEnabledState =
            orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_SEC_SEND_ATTR, false);
         orc_DoFlash.q_SecurityEnabled = orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_SEC_ENAB_ATTR, false);

         orc_DoFlash.q_SendDebuggerEnabledState =
            orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_DEB_SEND_ATTR, false);
         orc_DoFlash.q_DebuggerEnabled = orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_DEB_ENAB_ATTR, false);
      }

      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_NODE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save files in XML

   \param[in,out]  orc_Files              Files to save
   \param[in,out]  orc_XmlParser          XMLParser for service update package definition file
   \param[in]      orc_BaseNodeName       XML node name to use on base level
   \param[in]      orc_ElementNodeName    XML node name to use on item level
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageV1::mh_SaveFiles(const std::vector<C_SclString> & orc_Files,
                                                  C_OscXmlParserBase & orc_XmlParser,
                                                  const C_SclString & orc_BaseNodeName,
                                                  const C_SclString & orc_ElementNodeName)
{
   if (orc_Files.size() > 0)
   {
      //Files
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(orc_BaseNodeName) == orc_BaseNodeName);
      for (uint32_t u32_PosFile = 0; u32_PosFile < orc_Files.size(); u32_PosFile++)
      {
         //File
         tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(orc_ElementNodeName) == orc_ElementNodeName);
         orc_XmlParser.SetAttributeString(mc_FILE_NAME_ATTR, orc_Files[u32_PosFile]);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == orc_BaseNodeName);
      }
      //Return for next node
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_NODE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save PEM file configuration in XML

   \param[in,out]  orc_CurrentNode  Current node configuration with PEM file configuration
   \param[in,out]  orc_XmlParser    XMLParser for service update package definition file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageV1::mh_SavePemConfig(const C_SupDefNodeContent & orc_CurrentNode,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   if (orc_CurrentNode.c_PemFile != "")
   {
      //Files
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_PEM_FILE_CONFIG) == mc_PEM_FILE_CONFIG);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_SEC_SEND_ATTR, orc_CurrentNode.q_SendSecurityEnabledState);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_SEC_ENAB_ATTR, orc_CurrentNode.q_SecurityEnabled);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_DEB_SEND_ATTR, orc_CurrentNode.q_SendDebuggerEnabledState);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_DEB_ENAB_ATTR, orc_CurrentNode.q_DebuggerEnabled);

      //File
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_PEM_FILE) == mc_PEM_FILE);
      orc_XmlParser.SetAttributeString(mc_FILE_NAME_ATTR, orc_CurrentNode.c_PemFile);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_PEM_FILE_CONFIG);

      //Return for next node
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_NODE);
   }
}
