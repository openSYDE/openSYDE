//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for openSYDE source code generation command line tools.

   Provides generic function for openSYDE source code generation tools.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <iostream>
#include <getopt.h> //note: as we use getopt.h this application is not portable to all compilers

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCProjectFiler.h"
#include "C_OSCProject.h"
#include "CSCLString.h"
#include "C_OSCSystemDefinition.h"
#include "C_OSCSystemDefinitionFiler.h"
#include "C_OSCLoggingHandler.h"
#include "TGLFile.h"
#include "C_OsyCodeExportBase.h"
#include "C_OSCUtils.h"
#include "C_OSCBinaryHash.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_scl;
using namespace stw_tgl;

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
void C_OsyCodeExportBase::m_PrintCommandLineParameters(void) const
{
   std::cout << "Command line parameters:\n\n";
   std::cout <<
      "Switch  Alternative notation   Description                                   Default           Example\n";
   std::cout <<
      "====================================================================================================================\n";
   std::cout <<
      "-s      --systemdefinition     Path to System Definition file                <none>            -s c:\\mysysdef.syde_sysdef\n";
   std::cout <<
      "-o      --outputpath           Base path to generated files                  <none>            -o c:\\temp\n";
   std::cout <<
      "-n      --node                 Node (device) to generate code for            <all nodes>       -n MyESX3CM\n";
   std::cout <<
      "-a      --application          Application to generate code for              <all programmable -a MyApplication\n";
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
C_SCLString C_OsyCodeExportBase::h_GetApplicationVersion(const C_SCLString & orc_FileName)
{
   VS_FIXEDFILEINFO * pt_Info;
   uintn un_ValSize;
   sint32 s32_InfoSize;
   uint8 * pu8_Buffer;
   C_SCLString c_Version;

   c_Version = "V?.\?\?r?";

   s32_InfoSize = GetFileVersionInfoSizeA(orc_FileName.c_str(), NULL);
   if (s32_InfoSize != 0)
   {
      pu8_Buffer = new uint8[static_cast<uintn>(s32_InfoSize)];
      if (GetFileVersionInfoA(orc_FileName.c_str(), 0, s32_InfoSize, pu8_Buffer) != FALSE)
      {
         //reinterpret_cast required due to function interface
         if (VerQueryValueA(pu8_Buffer, "\\",
                            reinterpret_cast<PVOID *>(&pt_Info), //lint !e929
                            &un_ValSize) != FALSE)
         {
            c_Version.PrintFormatted("V%d.%02dr%d", (pt_Info->dwFileVersionMS >> 16),
                                     pt_Info->dwFileVersionMS & 0x0000FFFFUL,
                                     (pt_Info->dwFileVersionLS >> 16));
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
C_OsyCodeExportBase::~C_OsyCodeExportBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up generic code export engine

   Set up class elements and print application information to console

   \retval eRESULT_OK                      init OK
   \retval eRESULT_ERASE_FILE_LIST_ERROR   could not remove pre-existing file list file
*/
//----------------------------------------------------------------------------------------------------------------------
C_OsyCodeExportBase::E_ResultCode C_OsyCodeExportBase::Init(void)
{
   E_ResultCode e_Return = eRESULT_OK;
   charn acn_ApplicationName[MAX_PATH + 1];
   sintn sn_Return = GetModuleFileNameA(NULL, &acn_ApplicationName[0], MAX_PATH + 1);

   tgl_assert(sn_Return != 0);

   mq_EraseTargetFolder = false;

   mc_ExeName = acn_ApplicationName;
   mc_ExeVersion = h_GetApplicationVersion(mc_ExeName);
   mc_BinaryHash = stw_opensyde_core::C_OSCBinaryHash::h_CreateBinaryHash();

   mc_LogFileName = TGL_ChangeFileExtension(mc_ExeName, ".log");
   mc_ListOfFilesFileName = TGL_ChangeFileExtension(mc_ExeName, "") + "_file_list.txt";

   m_PrintBanner();

   std::cout << "Version: " << mc_ExeVersion.c_str() << &std::endl;
   std::cout << "MD5-Checksum: " << mc_BinaryHash.c_str() << &std::endl;

   //configure logging engine to log to local file:
   //remove pre-existing file:
   (void)std::remove(mc_LogFileName.c_str());
   stw_opensyde_core::C_OSCLoggingHandler::h_SetWriteToFileActive(true);
   stw_opensyde_core::C_OSCLoggingHandler::h_SetWriteToConsoleActive(false);
   stw_opensyde_core::C_OSCLoggingHandler::h_SetCompleteLogFileLocation(mc_LogFileName);
   osc_write_log_info("Starting tool", mc_ExeName + " Version: " + mc_ExeVersion + ", MD5-Checksum: " + mc_BinaryHash);

   //try to remove list-of-files file if it already exists
   if (stw_tgl::TGL_FileExists(mc_ListOfFilesFileName) == true)
   {
      //file does exist -> erase it
      sn_Return = remove(mc_ListOfFilesFileName.c_str());
      if (sn_Return != 0)
      {
         e_Return = eRESULT_ERASE_FILE_LIST_ERROR;
      }
   }
   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get options from command line

   Parse command line parameters. If required parameters are not present, print a list of options to the console.

   \param[in]   osn_Argc     number of command line arguments
   \param[in]   oapcn_Argv   command line arguments

   \retval   eRESULT_OK                                Init OK
   \retval   eRESULT_HELPING                           Command line switch "help" detected
   \retval   eRESULT_COMMAND_LINE_INVALID_PARAMETERS   Parameter error
*/
//----------------------------------------------------------------------------------------------------------------------
C_OsyCodeExportBase::E_ResultCode C_OsyCodeExportBase::ParseCommandLine(const sintn osn_Argc,
                                                                        charn * const oapcn_Argv[])
{
   E_ResultCode e_Return = eRESULT_OK;
   sintn sn_Result;
   C_SCLString c_Info = "";
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
      sintn sn_Index;

      sn_Result = getopt_long(osn_Argc, oapcn_Argv, "s:d:o:n:a:he", &ac_Options[0], &sn_Index);
      if (sn_Result != -1)
      {
         switch (sn_Result)
         {
         case 'd':
            c_Info = "Information: Command line parameter --devicedefinition ignored "
                     "(Device Definition is no longer required).";
            std::cout << c_Info.c_str() << &std::endl;
            osc_write_log_info("Parse Command Line", c_Info);
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
   while (sn_Result != -1);

   // Print command line parameters to the console?
   if (q_PrintCommandLineParameters == true)
   {
      m_PrintCommandLineParameters();
      e_Return = eRESULT_HELPING;
   }
   else
   {
      // Missing parameter option or missing mandatory parameter?
      if ((q_ParseError == true) || (mc_SystemDefinitionFilePath == "") || (mc_OutputPath == ""))
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
C_OsyCodeExportBase::E_ResultCode C_OsyCodeExportBase::LoadSystemDefinition(void)
{
   E_ResultCode e_Return = eRESULT_OK;

   // Load system definition
   const sint32 s32_Return =
      C_OSCSystemDefinitionFiler::h_LoadSystemDefinitionFile(mc_SystemDefinition, mc_SystemDefinitionFilePath, "",
                                                             false);

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

   \return
   eRESULT_OK                                code created for all programmable application (might be zero applications)
   eRESULT_CODE_GENERATION_ERROR             problems creating code for at least one programmable application
   eRESULT_APPLICATION_NOT_FOUND             at least one application not found
   eRESULT_APPLICATION_NOT_PROGRAMMABLE      at least one application is not a programmable application
   eRESULT_APPLICATION_UNKNOWN_CODE_VERSION  at least one application has unknown code structure version
*/
//----------------------------------------------------------------------------------------------------------------------
C_OsyCodeExportBase::E_ResultCode C_OsyCodeExportBase::m_CreateNodeCode(const C_OSCNode & orc_Node,
                                                                        const C_SCLString & orc_OutputPath)
{
   const bool q_OneApplicationOnly = (mc_ApplicationName != "");
   E_ResultCode e_Return = eRESULT_OK;

   //do the requested device and application exist ?
   if (q_OneApplicationOnly == true)
   {
      bool q_Found = false;
      for (uint32 u32_Application = 0U; u32_Application < orc_Node.c_Applications.size(); u32_Application++)
      {
         const C_OSCNodeApplication & rc_Application = orc_Node.c_Applications[u32_Application];

         if (rc_Application.c_Name.UpperCase() == mc_ApplicationName.UpperCase())
         {
            std::vector<C_SCLString> c_CreatedFiles;
            if (rc_Application.e_Type != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               e_Return = eRESULT_APPLICATION_NOT_PROGRAMMABLE;
               this->m_PrintCodeCreationInformation(orc_Node.c_Properties.c_Name, rc_Application, false,
                                                    c_CreatedFiles);
            }
            else if (rc_Application.u16_GenCodeVersion > C_OSCNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
            {
               this->m_PrintCodeFormatUnknownInfo(orc_Node.c_Properties.c_Name, rc_Application);
               e_Return = eRESULT_APPLICATION_UNKNOWN_CODE_VERSION;
            }
            else
            {
               e_Return = m_CreateApplicationCode(orc_Node, static_cast<uint16>(u32_Application), orc_OutputPath,
                                                  c_CreatedFiles);

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
      for (uint32 u32_Application = 0U; (u32_Application < orc_Node.c_Applications.size()) && (e_Return == eRESULT_OK);
           u32_Application++)
      {
         const C_OSCNodeApplication & rc_Application = orc_Node.c_Applications[u32_Application];
         if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
         {
            std::vector<C_SCLString> c_CreatedFiles;
            q_AtLeastOne = true;
            if (rc_Application.u16_GenCodeVersion > C_OSCNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
            {
               this->m_PrintCodeFormatUnknownInfo(orc_Node.c_Properties.c_Name, rc_Application);
               e_Return = eRESULT_APPLICATION_UNKNOWN_CODE_VERSION;
            }
            else
            {
               const C_SCLString c_Path =
                  TGL_FileIncludeTrailingDelimiter(orc_OutputPath) +
                  C_OSCUtils::h_NiceifyStringForFileName(rc_Application.c_Name);
               e_Return =
                  m_CreateApplicationCode(orc_Node, static_cast<uint16>(u32_Application), c_Path, c_CreatedFiles);
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
         C_SCLString c_Info = "Not generating code for device \"" + orc_Node.c_Properties.c_Name +
                              "\" as it has no programmable application defined.";
         std::cout << c_Info.c_str() << &std::endl;
         osc_write_log_info("Code Generation", c_Info);
      }
   }

   if (e_Return != eRESULT_OK)
   {
      const C_SCLString c_Info = "Error occured on code generation for device \"" + orc_Node.c_Properties.c_Name +
                                 "\". Stopped code generation.";
      std::cout << c_Info.c_str() << &std::endl;
      osc_write_log_error("Code Generation", c_Info);
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility for putting together information about unknown code format version.

   \param[in]       orc_NodeName             Node name
   \param[in]       orc_Application          Application information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OsyCodeExportBase::m_PrintCodeFormatUnknownInfo(const C_SCLString & orc_NodeName,
                                                       const C_OSCNodeApplication & orc_Application)
{
   std::vector<C_SCLString> c_CreatedFiles; // not used but necessary for m_PrintCodeCreationInformation
   const C_SCLString c_Info = "Code version 0x" +
                              C_SCLString::IntToHex(orc_Application.u16_GenCodeVersion, 4U) + " is unknown.";
   this->m_PrintCodeCreationInformation(orc_NodeName, orc_Application, false, c_CreatedFiles);

   std::cout << c_Info.c_str() << &std::endl;
   osc_write_log_info("Code Generation", c_Info);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility for putting together information for console output.

   \param[in]       orc_NodeName             Node name
   \param[in]       orc_Application          Application information
   \param[in]       oq_GenerationSuccessful  Flag if code was created or not
   \param[in]       orc_CreatedFiles         List of generated files (only valid if oq_GenerationSuccessful is true)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OsyCodeExportBase::m_PrintCodeCreationInformation(const C_SCLString & orc_NodeName,
                                                         const C_OSCNodeApplication & orc_Application,
                                                         const bool oq_GenerationSuccessful,
                                                         std::vector<C_SCLString> & orc_CreatedFiles)
{
   C_SCLString c_Text;

   if (oq_GenerationSuccessful == true)
   {
      c_Text += "Generated code ";
   }
   else
   {
      c_Text += "Could not generate code ";
   }
   c_Text += "for device \"" + orc_NodeName + "\" application \"" + orc_Application.c_Name +
             "\" in code format version 0x" + C_SCLString::IntToHex(orc_Application.u16_GenCodeVersion, 4U) + ".";

   if (oq_GenerationSuccessful == true)
   {
      c_Text += "\nGenerated files:";
      for (uint32 u32_File = 0U; u32_File < orc_CreatedFiles.size(); u32_File++)
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
C_OsyCodeExportBase::E_ResultCode C_OsyCodeExportBase::GenerateSourceCode(void)
{
   E_ResultCode e_Return = eRESULT_OK;
   const bool q_OneNodeOnly = (mc_DeviceName != "");

   //erase target folder if requested
   if (mq_EraseTargetFolder == true)
   {
      if (TGL_DirectoryExists(mc_OutputPath) == true)
      {
         sint32 s32_Return;
         C_SCLString c_Info = "Clearing pre-existing target folder (" + mc_OutputPath + ").";
         std::cout << c_Info.c_str() << &std::endl;
         osc_write_log_info("Code Generation", c_Info);

         //remove all folder contents (but keep folder itself)
         s32_Return = TGL_RemoveDirectory(mc_OutputPath, true);
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
      if (TGL_DirectoryExists(mc_OutputPath) == false)
      {
         sint32 s32_Return;
         C_SCLString c_Info = "Creating target folder (" + mc_OutputPath + ").";
         std::cout << c_Info.c_str() << &std::endl;
         osc_write_log_info("Code Generation", c_Info);

         //create target folder if required:
         s32_Return = C_OSCUtils::h_CreateFolderRecursively(mc_OutputPath);
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
         for (uint32 u32_Node = 0U; u32_Node < mc_SystemDefinition.c_Nodes.size(); u32_Node++)
         {
            const C_OSCNode & rc_Node = mc_SystemDefinition.c_Nodes[u32_Node];
            if (rc_Node.c_Properties.c_Name.UpperCase() == mc_DeviceName.UpperCase())
            {
               q_Found = true;
               if ((rc_Node.c_Properties.e_DiagnosticServer != C_OSCNodeProperties::eDS_OPEN_SYDE))
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
         for (uint32 u32_Node = 0U; u32_Node < mc_SystemDefinition.c_Nodes.size(); u32_Node++)
         {
            const C_OSCNode & rc_Node = mc_SystemDefinition.c_Nodes[u32_Node];

            if ((rc_Node.c_Properties.e_DiagnosticServer == C_OSCNodeProperties::eDS_OPEN_SYDE))
            {
               const C_SCLString c_Path = TGL_FileIncludeTrailingDelimiter(mc_OutputPath) +
                                          C_OSCUtils::h_NiceifyStringForFileName(rc_Node.c_Properties.c_Name);
               e_Return = m_CreateNodeCode(mc_SystemDefinition.c_Nodes[u32_Node], c_Path);
               if (e_Return != eRESULT_OK)
               {
                  break;
               }
            }
            else
            {
               const C_SCLString c_Info = "Not generating code for device \"" + rc_Node.c_Properties.c_Name +
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
C_OsyCodeExportBase::E_ResultCode C_OsyCodeExportBase::Exit(const E_ResultCode oe_ResultCode)
{
   //print enum as number; so the user can see what he/she would need to check for as ERRORLEVEL
   C_SCLString c_Text = "Tool result code: " +
                        C_SCLString::IntToStr(oe_ResultCode) + " "; //lint !e641  see comment above
   E_ResultCode e_Return = oe_ResultCode;

   if (oe_ResultCode == C_OsyCodeExportBase::eRESULT_OK)
   {
      //write list of created files to file:
      C_SCLStringList c_List;
      for (uint32 u32_String = 0U; u32_String < mc_CreatedFiles.size(); u32_String++)
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
   case eRESULT_CODE_GENERATION_ERROR:
      c_Text += "(Could not generate code for at least one application)";
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
