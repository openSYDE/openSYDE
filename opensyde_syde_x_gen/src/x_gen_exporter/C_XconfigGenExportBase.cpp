//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for openSYDE X-config generation command line tools.

   Provides generic function for openSYDE X-config generation tools.

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>
#include <getopt.h> //note: as we use getopt.h this application is not portable to all compilers

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscProjectFiler.hpp"
#include "C_OscProject.hpp"
#include "C_SclString.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscSystemDefinitionFiler.hpp"
#include "C_OscLoggingHandler.hpp"
#include "TglFile.hpp"
#include "C_XconfigGenExportBase.hpp"
#include "C_OscUtils.hpp"
#include "C_OscBinaryHash.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Print command line parameters

   Prints a list of options to the console.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XconfigGenExportBase::m_PrintCommandLineParameters(void) const
{
   std::cout << "Command line parameters:\n\n";
   std::cout <<
      "Switch  Alternative notation   Description                                   Default           Example\n";
   std::cout <<
      "====================================================================================================================\n";
   std::cout <<
      "-s      --systemdefinition     Path to System Definition file                <none>            -s c:\\mysysdef.syde_sysdef\n";
   std::cout <<
      "-d      --devicedefinition     Path to Device Definitions file (devices.ini) <none>            -d c:\\openSYDE_devices\\devices.ini\n";
   std::cout <<
      "-o      --outputpath           Base path to generated files                  <none>            -o c:\\temp\n";
   std::cout <<
      "-n      --node                 Node (device) to generate X-config for        <all nodes>       -n MyESX3CM\n";
   std::cout <<
      "-a      --application          Application to generate X-config for          <all programmable -a MyApplication\n";
   std::cout <<
      "                                   (only if --node is also specified)           applications>\n";
   std::cout <<
      "-e      --erasefolder          Erase target directory and all subdirectories <don't>           -e\n";
   std::cout <<
      "-h      --help                 Print command line parameters\n";
   std::cout << "Parameters that have a \"Default\" are optional. All others are mandatory.\n" << &std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get resource version number of a file as an C_SCLString

   Extracts the windows version number of the specified file and returns it
    in the commonly used STW format: "Vx.yyrz".
   This function is Windows specific and needs to be replaced by another solution
    when porting to a non-Windows system

   \param[in]   orc_FileName    file name to get version from

   \return
   string with version information ("V?.??r?" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_XconfigGenExportBase::h_GetApplicationVersion(const C_SclString & orc_FileName)
{
   VS_FIXEDFILEINFO * pc_Info;
   uint32_t u32_ValSize;
   int32_t s32_InfoSize;
   uint8_t * pu8_Buffer;
   C_SclString c_Version;

   c_Version = "V?.\?\?r?";

   s32_InfoSize = GetFileVersionInfoSizeA(orc_FileName.c_str(), NULL);
   if (s32_InfoSize != 0)
   {
      pu8_Buffer = new uint8_t[static_cast<uint32_t>(s32_InfoSize)];
      if (GetFileVersionInfoA(orc_FileName.c_str(), 0, s32_InfoSize, pu8_Buffer) != FALSE)
      {
         //reinterpret_cast required due to function interface
         if (VerQueryValueA(pu8_Buffer, "\\",
                            reinterpret_cast<PVOID *>(&pc_Info), //lint !e9176
                            &u32_ValSize) != FALSE)
         {
            c_Version.PrintFormatted("V%lu.%02lur%lu", (pc_Info->dwFileVersionMS >> 16U),
                                     pc_Info->dwFileVersionMS & 0x0000FFFFUL,
                                     (pc_Info->dwFileVersionLS >> 16U));
         }
      }
      delete[] pu8_Buffer;
   }
   return c_Version;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up
*/
//----------------------------------------------------------------------------------------------------------------------
C_XconfigGenExportBase::~C_XconfigGenExportBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up generic code export engine

   Set up class elements and print application information to console

   \param[in]  os32_Argc   number of command line arguments
   \param[in]  oppcn_Argv  command line arguments

   \retval eRESULT_OK                      init OK
   \retval eRESULT_ERASE_FILE_LIST_ERROR   could not remove pre-existing file list file
*/
//----------------------------------------------------------------------------------------------------------------------
C_XconfigGenExportBase::E_ResultCode C_XconfigGenExportBase::Init(const int32_t os32_Argc,
                                                                  char_t * const * const oppcn_Argv)
{
   E_ResultCode e_Return = eRESULT_OK;
   char_t acn_ApplicationName[MAX_PATH + 1];
   uint32_t u32_Return = GetModuleFileNameA(NULL, &acn_ApplicationName[0], MAX_PATH + 1);

   tgl_assert(u32_Return != 0);

   mq_EraseTargetFolder = false;

   mc_ExeName = acn_ApplicationName;
   mc_ExeVersion = h_GetApplicationVersion(mc_ExeName);
   mc_BinaryHash = stw::opensyde_core::C_OscBinaryHash::h_CreateBinaryHash();

   mc_LogFileName = TglChangeFileExtension(mc_ExeName, ".log");
   mc_ListOfFilesFileName = TglChangeFileExtension(mc_ExeName, "") + "_file_list.txt";

   m_PrintBanner();

   std::cout << "Version: " << mc_ExeVersion.c_str() << &std::endl;
   std::cout << "MD5-Checksum: " << mc_BinaryHash.c_str() << &std::endl;

   //configure logging engine to log to local file:
   //remove pre-existing file:
   (void)std::remove(mc_LogFileName.c_str());
   stw::opensyde_core::C_OscLoggingHandler::h_SetWriteToFileActive(true);
   stw::opensyde_core::C_OscLoggingHandler::h_SetWriteToConsoleActive(false);
   stw::opensyde_core::C_OscLoggingHandler::h_SetCompleteLogFileLocation(mc_LogFileName);
   osc_write_log_info("Starting tool", mc_ExeName + " Version: " + mc_ExeVersion + ", MD5-Checksum: " + mc_BinaryHash);
   osc_write_log_info("Call", "Command line: \"" +
                      stw::opensyde_core::C_OscUtils::h_GetCommandLineAsString(os32_Argc, oppcn_Argv) + "\"");

   //try to remove list-of-files file if it already exists
   if (stw::tgl::TglFileExists(mc_ListOfFilesFileName) == true)
   {
      //file does exist -> erase it
      u32_Return = remove(mc_ListOfFilesFileName.c_str());
      if (u32_Return != 0)
      {
         e_Return = eRESULT_ERASE_FILE_LIST_ERROR;
      }
   }
   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get options from command line

   Parse command line parameters. If required parameters are not present, print a list of options to the console.

   \param[in]  os32_Argc   number of command line arguments
   \param[in]  opacn_Argv  command line arguments

   \retval   eRESULT_OK                                Init OK
   \retval   eRESULT_HELPING                           Command line switch "help" detected
   \retval   eRESULT_COMMAND_LINE_INVALID_PARAMETERS   Parameter error
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{952}  the function getopt_long expects a non const opacn_Argv parameter
C_XconfigGenExportBase::E_ResultCode C_XconfigGenExportBase::ParseCommandLine(const int32_t os32_Argc,
                                                                              char_t * const opacn_Argv[])
{
   E_ResultCode e_Return = eRESULT_OK;
   int32_t s32_Result;
   C_SclString c_Info = "";
   bool q_PrintCommandLineParameters = false;
   bool q_ParseError = false;

   struct option ac_Options[] =
   {
      // name                         has_arg            flag  val
      {
         "systemdefinition",          required_argument, NULL, 's'
      },
      {
         "devicedefinition",          required_argument, NULL, 'd'
      },
      {
         "outputpath",                required_argument, NULL, 'o'
      },
      {
         "node",                      required_argument, NULL, 'n'
      },
      {
         "application",               required_argument, NULL, 'a'
      },
      {
         "erasefolder",               no_argument,       NULL, 'e'
      },
      {
         "help",                      no_argument,       NULL, 'h'
      },
      {
         NULL, 0, NULL, 0
      }
   };

   mc_SystemDefinitionFilePath = "";
   mc_OutputPath = "";

   do
   {
      int32_t s32_Index;

      s32_Result = getopt_long(os32_Argc, opacn_Argv, "s:d:o:n:a:he", &ac_Options[0], &s32_Index);
      if (s32_Result != -1)
      {
         switch (s32_Result)
         {
         case 'd':
            mc_DeviceDefinitions = optarg;
            break;
         case 's':
            mc_SystemDefinitionFilePath = optarg;
            break;
         case 'o':
            mc_OutputPath = optarg;
            break;
         case 'n':
            mc_DeviceName = optarg;
            break;
         case 'a':
            mc_ApplicationName = optarg;
            break;
         case 'e':
            mq_EraseTargetFolder = true;
            break;
         case 'h':
            q_PrintCommandLineParameters = true;
            break;
         case '?': //parser reports error (missing parameter option)
            q_ParseError = true;
            break;
         default:
            q_ParseError = true;
            break;
         }
      }
   }
   while (s32_Result != -1);

   // Print command line parameters to the console?
   if (q_PrintCommandLineParameters == true)
   {
      m_PrintCommandLineParameters();
      e_Return = eRESULT_HELPING;
   }
   else
   {
      // Missing parameter option or missing mandatory parameter?
      if ((q_ParseError == true) || (mc_SystemDefinitionFilePath == "") || (mc_DeviceDefinitions == "") ||
          (mc_OutputPath == ""))
      {
         //print directly to console; no need for logging this user feedback
         c_Info = "Invalid or missing command line parameters.";
         std::cout << "Error: " << c_Info.c_str() << &std::endl;
         osc_write_log_error("Parse Command Line", c_Info);
         m_PrintCommandLineParameters();
         e_Return = eRESULT_INVALID_CLI_PARAMETERS;
      }
      if ((mc_ApplicationName != "") && (mc_DeviceName == ""))
      {
         c_Info = "Command line switch \"-a\" may only be used in combination with \"-n\"";
         std::cout << "Error: " << c_Info.c_str() << &std::endl;
         osc_write_log_error("Parse Command Line", c_Info);
         m_PrintCommandLineParameters();
         e_Return = eRESULT_INVALID_CLI_PARAMETERS;
      }
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load System Definition

   Load openSYDE System Definition

   \return
   eRESULT_OK                                 System Definition loaded
   eRESULT_SYSTEM_DEFINITION_OPEN_ERROR       Problem loading System Definition
*/
//----------------------------------------------------------------------------------------------------------------------
C_XconfigGenExportBase::E_ResultCode C_XconfigGenExportBase::LoadSystemDefinition(void)
{
   E_ResultCode e_Return = eRESULT_OK;
   int32_t s32_Return;

   s32_Return = C_OscSystemDefinitionFiler::h_LoadSystemDefinitionFile(mc_SystemDefinition,
                                                                       mc_SystemDefinitionFilePath,
                                                                       mc_DeviceDefinitions);

   if (s32_Return == C_NO_ERR)
   {
      std::cout <<  "System Definition successfully loaded: " << mc_SystemDefinitionFilePath.c_str() << &std::endl;
      // loading system definition is logged to log file by C_OSCSystemDefinitionFiler
   }
   else
   {
      // Errors are logged on console by subfunctions
      e_Return = eRESULT_SYSTEM_DEFINITION_OPEN_ERROR;
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generate source code for one node

   As configured by the command line parameters.

   The actual code generation for each application will be done in a virtual function.
   Paths of created file will be added to mc_CreatedFiles;

   \param[in]  orc_Node          Node
   \param[in]  orc_OutputPath    Output path

   \return
   eRESULT_OK                                code created for all programmable application (might be zero applications)
   eRESULT_CODE_GENERATION_ERROR             problems creating code for at least one programmable application
   eRESULT_APPLICATION_NOT_FOUND             at least one application not found
   eRESULT_APPLICATION_NOT_PROGRAMMABLE      at least one application is not a programmable application
   eRESULT_APPLICATION_UNKNOWN_CODE_VERSION  at least one application has unknown code structure version
*/
//----------------------------------------------------------------------------------------------------------------------
C_XconfigGenExportBase::E_ResultCode C_XconfigGenExportBase::m_CreateNodeCode(const C_OscNode & orc_Node,
                                                                              const C_SclString & orc_OutputPath)
{
   const bool q_OneApplicationOnly = (mc_ApplicationName != "");
   E_ResultCode e_Return = eRESULT_OK;

   //do the requested device and application exist ?
   if (q_OneApplicationOnly == true)
   {
      bool q_Found = false;
      for (uint32_t u32_Application = 0U; u32_Application < orc_Node.c_Applications.size(); u32_Application++)
      {
         const C_OscNodeApplication & rc_Application = orc_Node.c_Applications[u32_Application];

         if (rc_Application.c_Name.UpperCase() == mc_ApplicationName.UpperCase())
         {
            std::vector<C_SclString> c_CreatedFiles;
            if ((rc_Application.e_Type != C_OscNodeApplication::ePROGRAMMABLE_APPLICATION) &&
                (rc_Application.e_Type != C_OscNodeApplication::ePARAMETER_SET_HALC))
            {
               e_Return = eRESULT_APPLICATION_NOT_PROGRAMMABLE;
               this->m_PrintCodeCreationInformation(orc_Node.c_Properties.c_Name, rc_Application, false,
                                                    c_CreatedFiles);
            }
            else if (rc_Application.u16_GenCodeVersion > C_OscNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
            {
               this->m_PrintCodeFormatUnknownInfo(orc_Node.c_Properties.c_Name, rc_Application);
               e_Return = eRESULT_APPLICATION_UNKNOWN_CODE_VERSION;
            }
            else
            {
               e_Return = m_CreateApplicationCode(orc_Node, mc_SystemDefinition,
                                                  orc_OutputPath, c_CreatedFiles);

               this->m_PrintCodeCreationInformation(orc_Node.c_Properties.c_Name, rc_Application,
                                                    e_Return == eRESULT_OK, c_CreatedFiles);
            }
            q_Found = true;
            break;
         }
      }
      if (q_Found == false)
      {
         e_Return = eRESULT_APPLICATION_NOT_FOUND;
      }
   }
   else
   {
      bool q_AtLeastOne = false;
      for (uint32_t u32_Application =
              0U; (u32_Application < orc_Node.c_Applications.size()) && (e_Return == eRESULT_OK);
           u32_Application++)
      {
         const C_OscNodeApplication & rc_Application = orc_Node.c_Applications[u32_Application];
         if ((rc_Application.e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION) ||
             (rc_Application.e_Type == C_OscNodeApplication::ePARAMETER_SET_HALC))
         {
            std::vector<C_SclString> c_CreatedFiles;
            q_AtLeastOne = true;
            if (rc_Application.u16_GenCodeVersion > C_OscNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
            {
               this->m_PrintCodeFormatUnknownInfo(orc_Node.c_Properties.c_Name, rc_Application);
               e_Return = eRESULT_APPLICATION_UNKNOWN_CODE_VERSION;
            }
            else
            {
               const C_SclString c_Path =
                  TglFileIncludeTrailingDelimiter(orc_OutputPath) +
                  C_OscUtils::h_NiceifyStringForFileName(rc_Application.c_Name);
               e_Return =
                  m_CreateApplicationCode(orc_Node, mc_SystemDefinition, c_Path,
                                          c_CreatedFiles);
               if (e_Return == eRESULT_OK)
               {
                  this->m_PrintCodeCreationInformation(orc_Node.c_Properties.c_Name, rc_Application, true,
                                                       c_CreatedFiles);
               }
               else
               {
                  this->m_PrintCodeCreationInformation(orc_Node.c_Properties.c_Name, rc_Application, false,
                                                       c_CreatedFiles);
               }
            }
         }
      }
      if (q_AtLeastOne == false)
      {
         const C_SclString c_Info = "Not generating code for device \"" + orc_Node.c_Properties.c_Name +
                                    "\" as it has no programmable application defined.";
         std::cout << c_Info.c_str() << &std::endl;
         osc_write_log_info("Code Generation", c_Info);
      }
   }

   if (e_Return != eRESULT_OK)
   {
      const C_SclString c_Info = "Error occured on code generation for device \"" + orc_Node.c_Properties.c_Name +
                                 "\". Stopped code generation.";
      std::cout << c_Info.c_str() << &std::endl;
      osc_write_log_error("Code Generation", c_Info);
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility for putting together information about unknown code format version.

   \param[in]  orc_NodeName      Node name
   \param[in]  orc_Application   Application information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XconfigGenExportBase::m_PrintCodeFormatUnknownInfo(const C_SclString & orc_NodeName,
                                                          const stw::opensyde_core::C_OscNodeApplication & orc_Application)
{
   std::vector<C_SclString> c_CreatedFiles; // not used but necessary for m_PrintCodeCreationInformation
   const C_SclString c_Info = "Code version 0x" +
                              C_SclString::IntToHex(orc_Application.u16_GenCodeVersion, 4U) + " is unknown.";
   this->m_PrintCodeCreationInformation(orc_NodeName, orc_Application, false, c_CreatedFiles);

   std::cout << c_Info.c_str() << &std::endl;
   osc_write_log_info("Code Generation", c_Info);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility for putting together information for console output.

   \param[in]  orc_NodeName               Node name
   \param[in]  orc_Application            Application information
   \param[in]  oq_GenerationSuccessful    Flag if code was created or not
   \param[in]  orc_CreatedFiles           List of generated files (only valid if oq_GenerationSuccessful is true)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XconfigGenExportBase::m_PrintCodeCreationInformation(const C_SclString & orc_NodeName,
                                                            const C_OscNodeApplication & orc_Application,
                                                            const bool oq_GenerationSuccessful,
                                                            std::vector<C_SclString> & orc_CreatedFiles)
{
   C_SclString c_Text;

   if (oq_GenerationSuccessful == true)
   {
      c_Text += "Generated code ";
   }
   else
   {
      c_Text += "Could not generate code ";
   }
   c_Text += "for device \"" + orc_NodeName + "\" application \"" + orc_Application.c_Name +
             "\" in code format version 0x" + C_SclString::IntToHex(orc_Application.u16_GenCodeVersion, 4U) + ".";

   if (oq_GenerationSuccessful == true)
   {
      c_Text += "\nGenerated files:";
      for (uint32_t u32_File = 0U; u32_File < orc_CreatedFiles.size(); u32_File++)
      {
         c_Text += "\n " + orc_CreatedFiles[u32_File];
         //append list of files:
         mc_CreatedFiles.push_back(orc_CreatedFiles[u32_File]);
      }
   }

   // print to console
   std::cout << c_Text.c_str() << &std::endl;

   // print to log file
   if (oq_GenerationSuccessful == true)
   {
      osc_write_log_info("Code Generation", c_Text);
   }
   else
   {
      osc_write_log_error("Code Generation", c_Text);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generate source code

   As configured by the command line parameters.

   If configured the target folder and all subfolders will be erased before starting the code generation.

   The actual code generation for each application will be done in a virtual function.

   \return
   eRESULT_OK                                   code created for all programmable application
   eRESULT_ERASE_TARGET_FOLDER_ERROR            could not erase pre-existing target folder
   eRESULT_CREATE_TARGET_FOLDER_ERROR           could not create target folder
   eRESULT_CODE_GENERATION_ERROR                problems creating code for at least one programmable application
   eRESULT_APPLICATION_DEVICE_NOT_FOUND         device specified on command line does not exist
   eRESULT_APPLICATION_DEVICE_NOT_COMPATIBLE    device specified on command line does not support openSYDE
   eRESULT_APPLICATION_NOT_FOUND                application specified on command line does not exist
   eRESULT_APPLICATION_NOT_PROGRAMMABLE         application specified on command line is not defined as "programmable"
   eRESULT_APPLICATION_UNKNOWN_CODE_VERSION     at least one application has unknown code structure version
*/
//----------------------------------------------------------------------------------------------------------------------
C_XconfigGenExportBase::E_ResultCode C_XconfigGenExportBase::GenerateSourceCode(void)
{
   E_ResultCode e_Return = eRESULT_OK;
   const bool q_OneNodeOnly = (mc_DeviceName != "");

   //erase target folder if requested
   if (mq_EraseTargetFolder == true)
   {
      if (TglDirectoryExists(mc_OutputPath) == true)
      {
         int32_t s32_Return;
         C_SclString c_Info = "Clearing pre-existing target folder (" + mc_OutputPath + ").";
         std::cout << c_Info.c_str() << &std::endl;
         osc_write_log_info("Code Generation", c_Info);

         //remove all folder contents (but keep folder itself)
         s32_Return = TglRemoveDirectory(mc_OutputPath, true);
         if (s32_Return != 0)
         {
            c_Info = "Could not clear pre-existing target folder (" + mc_OutputPath + ").";
            std::cout << "Error: " << c_Info.c_str() << &std::endl;
            osc_write_log_error("Code Generation", c_Info);

            e_Return = eRESULT_ERASE_TARGET_FOLDER_ERROR;
         }
      }
   }
   else
   {
      if (TglDirectoryExists(mc_OutputPath) == false)
      {
         int32_t s32_Return;
         C_SclString c_Info = "Creating target folder (" + mc_OutputPath + ").";
         std::cout << c_Info.c_str() << &std::endl;
         osc_write_log_info("Code Generation", c_Info);

         //create target folder if required:
         s32_Return = C_OscUtils::h_CreateFolderRecursively(mc_OutputPath);
         if (s32_Return != C_NO_ERR)
         {
            c_Info = "Could not create target folder (" + mc_OutputPath + ").";
            std::cout << "Error: " << c_Info.c_str() << &std::endl;
            osc_write_log_error("Code Generation", c_Info);

            e_Return = eRESULT_CREATE_TARGET_FOLDER_ERROR;
         }
      }
   }

   if (e_Return == eRESULT_OK)
   {
      //does the requested device exist ?
      if (q_OneNodeOnly == true)
      {
         bool q_Found = false;
         for (uint32_t u32_Node = 0U; u32_Node < mc_SystemDefinition.c_Nodes.size(); u32_Node++)
         {
            const C_OscNode & rc_Node = mc_SystemDefinition.c_Nodes[u32_Node];
            if (rc_Node.c_Properties.c_Name.UpperCase() == mc_DeviceName.UpperCase())
            {
               q_Found = true;
               if ((rc_Node.c_Properties.e_DiagnosticServer != C_OscNodeProperties::eDS_OPEN_SYDE))
               {
                  e_Return = eRESULT_DEVICE_NOT_COMPATIBLE;
               }
               else
               {
                  e_Return = m_CreateNodeCode(mc_SystemDefinition.c_Nodes[u32_Node], mc_OutputPath);
               }
               break;
            }
         }
         if (q_Found == false)
         {
            e_Return = eRESULT_DEVICE_NOT_FOUND;
         }
      }
      else
      {
         //generate code for all nodes
         for (uint32_t u32_Node = 0U; u32_Node < mc_SystemDefinition.c_Nodes.size(); u32_Node++)
         {
            const C_OscNode & rc_Node = mc_SystemDefinition.c_Nodes[u32_Node];
            if ((rc_Node.c_Properties.e_DiagnosticServer == C_OscNodeProperties::eDS_OPEN_SYDE))
            {
               const C_SclString c_Path = TglFileIncludeTrailingDelimiter(mc_OutputPath) +
                                          C_OscUtils::h_NiceifyStringForFileName(rc_Node.c_Properties.c_Name);
               e_Return = m_CreateNodeCode(mc_SystemDefinition.c_Nodes[u32_Node], c_Path);
               if (e_Return != eRESULT_OK)
               {
                  break;
               }
            }
            else
            {
               const C_SclString c_Info = "Not generating code for device \"" + rc_Node.c_Properties.c_Name +
                                          "\" as it is not an openSYDE node.";
               std::cout << c_Info.c_str() << &std::endl;
               osc_write_log_info("Code Generation", c_Info);
            }
         }
      }
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Shut down generic code export engine

   Print result of operation to console

   \param[in]  oe_ResultCode   error result code of previous operations
*/
//----------------------------------------------------------------------------------------------------------------------
C_XconfigGenExportBase::E_ResultCode C_XconfigGenExportBase::Exit(const E_ResultCode oe_ResultCode)
{
   //print enum as number; so the user can see what he/she would need to check for as ERRORLEVEL
   C_SclString c_Text = "Tool result code: " +
                        C_SclString::IntToStr(oe_ResultCode) + " "; //lint !e641  see comment above
   E_ResultCode e_Return = oe_ResultCode;

   if (oe_ResultCode == C_XconfigGenExportBase::eRESULT_OK)
   {
      //write list of created files to file:
      C_SclStringList c_List;
      for (uint32_t u32_String = 0U; u32_String < mc_CreatedFiles.size(); u32_String++)
      {
         c_List.Add(mc_CreatedFiles[u32_String]);
      }
      //write even if empty in order to have clearly defined content:
      try
      {
         c_List.SaveToFile(mc_ListOfFilesFileName);
      }
      catch (...)
      {
         e_Return = eRESULT_WRITE_FILE_LIST_ERROR;
      }
   }

   switch (e_Return)
   {
   case eRESULT_OK:
      c_Text += "(Code was generated)";
      break;
   case eRESULT_HELPING:
      c_Text += "(Help requested)";
      break;
   case eRESULT_ERASE_FILE_LIST_ERROR:
      c_Text += "(Could not remove pre-existing file list file \"" + mc_ListOfFilesFileName + "\")";
      break;
   case eRESULT_WRITE_FILE_LIST_ERROR:
      c_Text += "(Could not write file with list of generated files \"" + mc_ListOfFilesFileName + "\")";
      break;
   case eRESULT_ERASE_TARGET_FOLDER_ERROR:
      c_Text += "(Could not erase pre-existing target folder)";
      break;
   case eRESULT_CREATE_TARGET_FOLDER_ERROR:
      c_Text += "(Could not create target folder)";
      break;
   case eRESULT_INVALID_CLI_PARAMETERS:
      c_Text += "(Invalid command line parameters)";
      break;
   case eRESULT_SYSTEM_DEFINITION_OPEN_ERROR:
      c_Text += "(Specified system definition could not be loaded)";
      break;
   case eRESULT_X_CONFIG_GENERATION_ERROR:
      c_Text += "(Could not generate x-config for at least one application)";
      break;
   case eRESULT_DEVICE_NOT_FOUND:
      c_Text += "(Device specified on command line does not exist)";
      break;
   case eRESULT_DEVICE_NOT_COMPATIBLE:
      c_Text += "(Device specified on command line does not support openSYDE)";
      break;
   case eRESULT_APPLICATION_NOT_FOUND:
      c_Text += "(Application specified on command line does not exist)";
      break;
   case eRESULT_APPLICATION_NOT_PROGRAMMABLE:
      c_Text += "(Application specified on command line is not defined as \"programmable\")";
      break;
   case eRESULT_APPLICATION_UNKNOWN_CODE_VERSION:
      c_Text += "(At least one application has unknown code format version)";
      break;
   default:
      c_Text += "(Undefined result)";
      break;
   }

   std::cout << c_Text.c_str() << &std::endl;
   if (e_Return == eRESULT_OK)
   {
      osc_write_log_info("Finish Code Generation", c_Text);
   }
   else
   {
      osc_write_log_error("Finish Code Generation", c_Text);
   }

   return e_Return;
}
