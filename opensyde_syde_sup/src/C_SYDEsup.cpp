//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class of SYDEsup (implementation)

   This is the base class and contains most functionality of SYDEsup.
   SYDEsup is a console application for updating your system with a service
   update package created with openSYDE.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.08.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <iostream>
#include <getopt.h> //note: as we use getopt.h this application is not portable to all compilers

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "C_SYDEsup.h"
#include "C_OSCSuServiceUpdatePackage.h"
#include "CCAN.h"
#include "C_OSCIpDispatcherWinSock.h"
#include "C_OSCLoggingHandler.h"
#include "TGLTime.h"
#include "TGLFile.h"
#include "C_SUPSuSequences.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;
using namespace stw_can;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     24.08.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_SYDEsup::C_SYDEsup()
{
   mq_Quiet = false;
}
//-----------------------------------------------------------------------------
/*!
   \brief   Get options from command line

   Parse command line parameters.
   If required parameters are not present print a list of options to the console.

   Parameters are:
   * -h for help
   * -q for quiet
   * -p for package file path
   * -i for CAN interface (path to CAN DLL) (optional but needed if packages active bus is CAN, not Ethernet)
   * -z for path to unzip directory (optional)
   * -l for path to log file (optional)

   \param[in]   osn_Argc     number of command line arguments
   \param[in]   oapcn_Argv   command line arguments

   \return
   eOK                        initialization worked
   eERR_PARSE_COMMAND_LINE    missing command line parameters or help requested

   \created     27.08.18  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsup::ParseCommandLine(const sintn osn_Argc, charn * const oapcn_Argv[])
{
   C_SYDEsup::E_Result e_Return = eOK;
   sintn sn_Result;
   bool q_ParseError = false;
   bool q_ShowHelp = false;
   mq_Quiet = false;

   const struct option ac_Options[] =
   {
      /* name, has_arg, flag, val */
      { "help",         no_argument,         NULL,    'h' },
      { "quiet",        no_argument,         NULL,    'q' },
      { "packagefile",  required_argument,   NULL,    'p' },
      { "caninterface", required_argument,   NULL,    'i' },
      { "unzipdir",     required_argument,   NULL,    'z' },
      { "logdir",       required_argument,   NULL,    'l' },
      { NULL,           0,                   NULL,    0   }
   };

   mc_SUPFilePath = "";
   mc_CanDLLPath = "";
   mc_UnzipPath = "";
   mc_LogPath = "";

   do
   {
      sintn sn_Index;
      sn_Result = getopt_long(osn_Argc, oapcn_Argv, "hqp:i:z:l:", &ac_Options[0], &sn_Index);
      if (sn_Result != -1)
      {
         switch (sn_Result)
         {
         case 'h':
            q_ShowHelp = true;
            break;
         case 'q':
            mq_Quiet = true;
            break;
         case 'p':
            mc_SUPFilePath = optarg;
            break;
         case 'i':
            mc_CanDLLPath = optarg;
            break;
         case 'z':
            mc_UnzipPath = optarg;
            break;
         case 'l':
            mc_LogPath = optarg;
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

   if (q_ShowHelp == true)
   {
      //print directly to console; no need for logging this user feedback
      this->m_PrintInformation(h_GetApplicationVersion(oapcn_Argv[0]));
      std::cout << "SYDEsup is a console application for updating your system "
         "with a Service Update Package created with openSYDE.\n\n";
      e_Return = eERR_PARSE_COMMAND_LINE;
   }
   else if ((mc_SUPFilePath == "") || (q_ParseError == true))
   {
      //print directly to console; no need for logging this user feedback
      this->m_PrintInformation(h_GetApplicationVersion(oapcn_Argv[0]));
      std::cout << "Error: Invalid or missing command line parameters.\n\n";
      e_Return = eERR_PARSE_COMMAND_LINE;
   }
   else
   {
      // Initialize optional parameters
      this->m_InitOptionalParameters();
      // log version
      h_WriteLog("SYDEsup Version", ("Version: " + h_GetApplicationVersion(oapcn_Argv[0])).c_str());
   }

   return e_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update to the given Service Update Package.

   Previous call to ParseCommandLineParameters required
   (or alternative method to set mc_SUPFilePath and mc_CanDLLPath).

   This method goes through the following steps, mostly openSYDE core functionality:
   * unpack Service Update Package
   * if active bus is CAN bus:
      * check if DLL path exists
      * open CAN DLL
      * initialize CAN
   * initialize sequence
   * activate Flashloader
   * update system
   * reset system and close CAN DLL

   We only go one step further if the previous one was successful.

   \return
   // for detailed error description see C_SYDEsup::m_PrintStringFromError

   // results from h_UnpackPackage
   eERR_PACKAGE_ERASE_TARG_PATH
   eERR_PACKAGE_CORE_C_RANGE
   eERR_PACKAGE_CORE_C_OVERFLOW
   eERR_PACKAGE_UNZIP
   eERR_PACKAGE_CORE_C_NOACT
   eERR_PACKAGE_NOT_FOUND

   // result from check if DLL file exists
   eERR_DLL_NOT_FOUND

   // results from c_CanDispatcher.DLL_Open
   eERR_DLL_ALREADY_OPENED
   eERR_DLL_INIT_FAILED
   eERR_DLL_FORMAT

   // results from c_CanDispatcher.CAN_Init
   eERR_DLL_C_UNKNOWN
   eERR_DLL_CHANNEL
   eERR_DLL_NOT_OPENED

   // results from c_Sequence.Init
   eERR_SEQUENCE_ROUTING
   eERR_SEQUENCE_UNKNOWN_CONFIG
   eERR_SEQUENCE_DLL_NOT_FOUND
   eERR_SEQUENCE_INVALID_PARAM
   eERR_SEQUENCE_CAN_INIT
   eERR_SEQUENCE_C_CONFIG
   eERR_SEQUENCE_BUFF_OVERFLOW

   // results from c_Sequence.ActivateFlashloader
   eERR_ACTIVATE_FLASHLOADER_FAILED
   eERR_ACTIVATE_FLASHLOADER_C_COM
   eERR_ACTIVATE_FLASHLOADER_SYSDEF

   // results from c_Sequence.UpdateSystem
   eERR_UPDATE_ABORTED Procedure aborted by user.
   eERR_UPDATE_C_OVERFLOW
   eERR_UPDATE_C_RD_RW
   eERR_UPDATE_C_NOACT
   eERR_UPDATE_C_COM
   eERR_UPDATE_SYSDEF

   //results from c_Sequence.ResetSystem
   eERR_RESET_C_COM
   eERR_RESET_SYSDEF

   // general results
   eOK
   eERR_UNKNOWN

   \created     27.08.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsup::Update(void) const
{
   E_Result e_Result = eOK;
   sint32 s32_Return;
   C_OSCSystemDefinition c_SystemDefinition;
   uint32 u32_ActiveBusIndex = 0;
   C_SCLStringList c_WarningMessages;
   C_SCLString c_ErrorMessage;
   C_OSCIpDispatcherWinSock c_IpDispatcher;
   C_CAN c_CanDispatcher;
   C_SUPSuSequences c_Sequence;
   bool q_CanLoaded = false; // flag
   bool q_ResetSystem = false;

   std::vector<uint8> c_ActiveNodes;
   std::vector<uint32> c_NodesUpdateOrder;
   std::vector<C_OSCSuSequences::C_DoFlash> c_ApplicationsToWrite;

   // unpack Service Update Package which was created with openSYDE (also checks if paths are valid)
   s32_Return = C_OSCSuServiceUpdatePackage::h_UnpackPackage(mc_SUPFilePath, mc_UnzipPath,
                                                             c_SystemDefinition, u32_ActiveBusIndex,
                                                             c_ActiveNodes, c_NodesUpdateOrder,
                                                             c_ApplicationsToWrite,
                                                             c_WarningMessages, c_ErrorMessage);

   // report success or translate errors
   switch (s32_Return) // here s32_Return is result of h_UnpackPackage
   {
   case C_NO_ERR:
      h_WriteLog("Unzip Package", "Service update package unzipped to \"\\" + mc_UnzipPath + "\\\".");
      break;
   case C_BUSY:
      e_Result = eERR_PACKAGE_ERASE_TARG_PATH;
      break;
   case C_RANGE:
      e_Result = eERR_PACKAGE_CORE_C_RANGE;
      break;
   case C_OVERFLOW:
      e_Result = eERR_PACKAGE_CORE_C_OVERFLOW;
      break;
   case C_RD_WR:
      e_Result = eERR_PACKAGE_UNZIP;
      break;
   case C_NOACT:
      e_Result = eERR_PACKAGE_CORE_C_NOACT;
      break;
   case C_CONFIG:
      e_Result = eERR_PACKAGE_NOT_FOUND;
      break;
   default:
      e_Result = eERR_UNKNOWN;
      break;
   }

   // go on if previous step was successful
   if (e_Result == eOK)
   {
      // do CAN stuff if CAN bus
      if (c_SystemDefinition.c_Buses[u32_ActiveBusIndex].e_Type == C_OSCSystemBus::eCAN)
      {
         // check if CAN DLL exists
         if (TGL_FileExists(mc_CanDLLPath) == false)
         {
            e_Result = eERR_DLL_NOT_FOUND;
         }
         else
         {
            h_WriteLog("CAN DLL", "CAN DLL \"" + mc_CanDLLPath + "\" found.");
            // setup/load CAN dispatcher
            c_CanDispatcher.SetDLLName(mc_CanDLLPath);
            s32_Return = c_CanDispatcher.DLL_Open();
         }

         // initialize CAN DLL if previous step was successful and return errors else
         if (e_Result == eOK)
         {
            switch (s32_Return) // here s32_Return is result of c_CanDispatcher.DLL_Open
            {
            case C_NO_ERR:
               h_WriteLog("Setup CAN", "CAN DLL loaded.");
               q_CanLoaded = true;
               // initialize CAN dispatcher
               s32_Return = c_CanDispatcher.CAN_Init();
               break;
            case CAN_COMP_ERR_DLL_ALREADY_OPENED:
               e_Result = eERR_DLL_ALREADY_OPENED;
               break;
            case CAN_COMP_ERR_DLL_INIT:
               e_Result = eERR_DLL_INIT_FAILED;
               break;
            case CAN_COMP_ERR_DLL_FORMAT:
               e_Result = eERR_DLL_FORMAT;
               break;
            default:
               e_Result = eERR_UNKNOWN;
               break;
            }
         }

         // report success or translate errors
         if (e_Result == eOK)
         {
            switch (s32_Return) // here s32_Return is result of c_CanDispatcher.CAN_Init
            {
            case C_NO_ERR:
               h_WriteLog("Setup CAN", "CAN interface initialized.");
               break;
            case C_UNKNOWN_ERR:
               e_Result = eERR_DLL_C_UNKNOWN;
               break;
            case C_CONFIG:
               e_Result = eERR_DLL_CHANNEL;
               break;
            case CAN_COMP_ERR_DLL_NOT_OPENED:
               e_Result = eERR_DLL_NOT_OPENED;
               break;
            default:
               e_Result = eERR_UNKNOWN;
               break;
            }
         }
      }
   }

   // initialize sequence if previous step (unpacking if Ethernet, CAN initializing if CAN) was successful
   if (e_Result == eOK)
   {
      // initialize sequence
      s32_Return = c_Sequence.Init(c_SystemDefinition, u32_ActiveBusIndex, c_ActiveNodes, &c_CanDispatcher,
                                   &c_IpDispatcher);
      // tell report methods to not print to console
      c_Sequence.SetQuiet(mq_Quiet);
   }

   // activate Flashloader if previous step was successful and return errors else
   if (e_Result == eOK)
   {
      switch (s32_Return) // here s32_Return is result of c_Sequence.Init
      {
      case C_NO_ERR:
         h_WriteLog("Setup Sequence", "Sequence initialized.");
         // activate Flashloader
         s32_Return = c_Sequence.ActivateFlashloader();
         break;
      case C_RANGE:
         e_Result = eERR_SEQUENCE_ROUTING;
         break;
      case C_OVERFLOW:
         e_Result = eERR_SEQUENCE_UNKNOWN_CONFIG;
         break;
      case C_RD_WR:
         e_Result = eERR_SEQUENCE_DLL_NOT_FOUND;
         break;
      case C_NOACT:
         e_Result = eERR_SEQUENCE_INVALID_PARAM;
         break;
      case C_COM:
         e_Result = eERR_SEQUENCE_CAN_INIT;
         break;
      case C_CONFIG:
         e_Result = eERR_SEQUENCE_C_CONFIG;
         break;
      case C_CHECKSUM:
         e_Result = eERR_SEQUENCE_BUFF_OVERFLOW;
         break;
      default:
         e_Result = eERR_UNKNOWN;
         break;
      }
   }

   // update system if previous step was successful and return errors else
   if (e_Result == eOK)
   {
      switch (s32_Return) // here s32_Return is result of c_Sequence.ActivateFlashloader
      {
      case C_NO_ERR:
         h_WriteLog("Activate Flashloader", "Flashloader activated.");
         // update system
         s32_Return = c_Sequence.UpdateSystem(c_ApplicationsToWrite, c_NodesUpdateOrder);
         break;
      case C_WARN:
         e_Result = eERR_ACTIVATE_FLASHLOADER_FAILED;
         q_ResetSystem = true;
         break;
      case C_COM:
         e_Result = eERR_ACTIVATE_FLASHLOADER_C_COM;
         q_ResetSystem = true;
         break;
      case C_CONFIG:
         e_Result = eERR_ACTIVATE_FLASHLOADER_SYSDEF;
         q_ResetSystem = true;
         break;
      default:
         e_Result = eERR_UNKNOWN;
         q_ResetSystem = true;
         break;
      }
   }

   // check if update worked, inform user in success case and adapt return value else
   if (e_Result == eOK)
   {
      switch (s32_Return) // here s32_Return is result of c_Sequence.UpdateSystem
      {
      case C_NO_ERR:
         std::cout << "\n";
         h_WriteLog("System Update", "System update successful!");
         std::cout << "\n";
         q_ResetSystem = true;
         break;
      case C_BUSY:
         e_Result = eERR_UPDATE_ABORTED;
         break;
      case C_OVERFLOW:
         e_Result = eERR_UPDATE_C_OVERFLOW;
         break;
      case C_RD_WR:
         e_Result = eERR_UPDATE_C_RD_RW;
         break;
      case C_NOACT:
         e_Result = eERR_UPDATE_C_NOACT;
         break;
      case C_COM:
         e_Result = eERR_UPDATE_C_COM;
         break;
      case C_CONFIG:
         e_Result = eERR_UPDATE_SYSDEF;
         break;
      default:
         e_Result = eERR_UNKNOWN;
         break;
      }
   }

   // inform user about errors
   this->m_PrintStringFromError(e_Result);

   // conclude (reset system and close CAN DLL)
   this->m_Conclude(c_CanDispatcher, c_Sequence, q_CanLoaded, q_ResetSystem);

   return e_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Write to log file and print to console.

   Write detailed information with logging engine to log file but
   only print concise details to console.

   \param[in]     orc_Activity    Activity
   \param[in]     orc_Text        Information text
   \param[in]     orq_IsError     True: log as error; false: log as information

   \created     29.08.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_SYDEsup::h_WriteLog(const stw_scl::C_SCLString & orc_Activity, const stw_scl::C_SCLString & orc_Text,
                           const bool & orq_IsError, const bool & orq_Quiet)
{
   if (orq_IsError == true)
   {
      osc_write_log_error(orc_Activity, orc_Text);
      std::cout << "Error: " << orc_Text.c_str() << "\n";
   }
   else
   {
      osc_write_log_info(orc_Activity, orc_Text);
      // do not write information if user wants me to be quiet
      if (orq_Quiet == false)
      {
         std::cout << orc_Text.c_str() << "\n";
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get resource version number of a file as an C_SCLString

   Extracts the windows version number of the specified file and returns it
    in the commonly used STW format: "Vx.yyrz".
   This function is Windows specific and needs to be replaced by another solution
    when porting to a non-Windows system

   \param[in]   orc_FileName    file name to get version from

   \return
   string with version information ("V?.??r?" on error)

   \created     20.05.2008  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SYDEsup::h_GetApplicationVersion(const C_SCLString & orc_FileName)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Print information about command line parameters and application version to console.

   \created     31.08.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_SYDEsup::m_PrintInformation(const C_SCLString & orc_Version) const
{
   // show version
   std::cout << "Version: " << orc_Version.c_str() << "\n\n";

   // show parameter help
   std::cout << "Command line parameters:\n\n";
   std::cout <<
      "Parameter   Alternative notation   Description                          Default         Example\n";
   std::cout <<
      "-----------------------------------------------------------------------------------------------------------------\n";
   std::cout <<
      "-h          --help                 Print help                                           -h\n";
   std::cout <<
      "-q          --quiet                Print less console output                            -q\n";
   std::cout <<
      "-p          --packagefile          Path to Service Update Package file  <none>          -p d:\\MyPackage.syde_sup\n";
   std::cout <<
      "-i          --caninterface         CAN interface (Path to CAN DLL)      <none>          -i d:\\MyCan.dll\n";
   std::cout <<
      "-z          --unzipdir             Directory where files get unzipped   <packagefile>   -z d:\\MyUnzipDir\n";
   std::cout <<
      "-l          --logdir               Directory for log files              .\\Logs          -l d:\\MyLogDir\n";
   std::cout << "\n";
   std::cout << "The package file parameter \"-p\" is mandatory, all others are optional. \n"
      "If the active bus in the given Service Update Package is of CAN type, a CAN interface must be provided.\n\n";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Define log file location.

   Create log file name depending on date and time and set log file path
   to WorkingDirectory/Logs/log-file-name.syde_log.
   Only call this method once!

   \return
   log file name and location.

   \created     28.08.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_SCLString C_SYDEsup::m_GetLogFileLocation(void) const
{
   C_SCLString c_LogFilePath;
   C_TGLDateTime c_DateTime;

   TGL_GetDateTimeNow(c_DateTime);

   c_LogFilePath = C_OSCLoggingHandler::h_UtilConvertDateTimeToString(c_DateTime).c_str();

   // Convert  2018-08-28 09:47:50.459 to  2018-08-28_09-47-50
   // i.e. replace " " with "_", replace ":" with "-" and cut decimals (number of characters is always the same)
   c_LogFilePath = c_LogFilePath.Insert("_", 11); // " "
   c_LogFilePath = c_LogFilePath.Delete(12, 1);
   c_LogFilePath = c_LogFilePath.Insert("-", 14); // first ":"
   c_LogFilePath = c_LogFilePath.Delete(15, 1);
   c_LogFilePath = c_LogFilePath.Insert("-", 17); //second ":"
   c_LogFilePath = c_LogFilePath.Delete(18, 1);
   c_LogFilePath = c_LogFilePath.Delete(20, 4); // remove ".123"

   c_LogFilePath = mc_LogPath + c_LogFilePath + ".syde_log";

   return c_LogFilePath;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize optional parameters unzip path and logging path

   Check if path exists and replace it with default location if necessary.
   Set logging file.

   \created     28.08.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_SYDEsup::m_InitOptionalParameters()
{
   // unzip path:
   if (TGL_DirectoryExists(mc_UnzipPath) == false)
   {
      // default location is update package file path "converted" to directory
      mc_UnzipPath =  TGL_ChangeFileExtension(mc_SUPFilePath, ""); // note: unzipping checks if this location is valid
   }

   // log path (gets created, so just check for non empty):
   if (mc_LogPath == "")
   {
      // default location is "here"
      mc_LogPath = ".\\Logs\\";
   }

   // add trailing path delimiter in case there is none
   mc_LogPath = TGL_FileIncludeTrailingDelimiter(mc_LogPath);

   // log to file
   mc_LogFile = this->m_GetLogFileLocation();
   C_OSCLoggingHandler::h_SetCompleteLogFileLocation(mc_LogFile);
   std::cout << "The following output also is logged to the file: " << mc_LogFile.c_str() << "\n\n";
   C_OSCLoggingHandler::h_SetWriteToConsoleActive(false);
   C_OSCLoggingHandler::h_SetWriteToFileActive(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Print error description from error result

   \param[in]     ore_Result     error enum

   \created     31.08.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_SYDEsup::m_PrintStringFromError(const E_Result & ore_Result) const
{
   C_SCLString c_Activity;
   C_SCLString c_Error;

   switch (ore_Result)
   {
   case eERR_PARSE_COMMAND_LINE:
      // console output only (no writing to log file)
      // Invalid or missing command line parameters.
      break;

   // results from h_UnpackPackage (comment is corresponding core return value)
   case eERR_PACKAGE_ERASE_TARG_PATH: //C_BUSY
      c_Activity = "Unzip Package";
      c_Error = "Could not erase preexisting target path (note: can result in partially erased target path).";
      break;
   case eERR_PACKAGE_CORE_C_RANGE: //C_RANGE
      c_Activity = "Unzip Package";
      c_Error =
         "Error code of a called core function (should not occur for valid and compatible Service Update Package).";
      break;
   case eERR_PACKAGE_CORE_C_OVERFLOW: //C_OVERFLOW
      c_Activity = "Unzip Package";
      c_Error =
         "Error code of a called core function (should not occur for valid and compatible Service Update Package).";
      break;
   case eERR_PACKAGE_UNZIP: //C_RD_WR
      c_Activity = "Unzip Package";
      c_Error = "Could not unzip update package from disk to target path.";
      break;
   case eERR_PACKAGE_CORE_C_NOACT: //C_NOACT
      c_Activity = "Unzip Package";
      c_Error =
         "Error code of a called core function (should not occur for valid and compatible Service Update Package).";
      break;
   case eERR_PACKAGE_NOT_FOUND: //C_CONFIG
      c_Activity = "Unzip Package";
      c_Error = "Could not find update package archive.";
      break;

   // result from check if DLL file exists
   case eERR_DLL_NOT_FOUND:
      c_Activity = "CAN DLL";
      c_Error = "Could not find CAN DLL file: \"" + mc_CanDLLPath + "\".";
      break;

   // results from c_CanDispatcher.DLL_Open (comment is corresponding core return value)
   case eERR_DLL_ALREADY_OPENED: //CAN_COMP_ERR_DLL_ALREADY_OPENED
      c_Activity = "Open CAN DLL";
      c_Error = "CAN DLL already opened.";
      break;
   case eERR_DLL_INIT_FAILED: //CAN_COMP_ERR_DLL_INIT
      c_Activity = "Open CAN DLL";
      c_Error = "CAN DLL initialization failed.";
      break;
   case eERR_DLL_FORMAT: //CAN_COMP_ERR_DLL_FORMAT
      c_Activity = "Open CAN DLL";
      c_Error = "CAN DLL not in STW CAN DLL format.";
      break;

   // results from c_CanDispatcher.CAN_Init (comment is corresponding core return value)
   case eERR_DLL_C_UNKNOWN: //C_UNKNOWN_ERR
      c_Activity = "Initialize CAN";
      c_Error = "DLL function returned initialization error.";
      break;
   case eERR_DLL_CHANNEL: //C_CONFIG
      c_Activity = "Initialize CAN";
      c_Error = "Channel different from 0 configured but not supported by loaded DLL.";
      break;
   case eERR_DLL_NOT_OPENED: //CAN_COMP_ERR_DLL_NOT_OPENED
      c_Activity = "Initialize CAN";
      c_Error = "CAN DLL was not yet loaded.";
      break;

   // results from c_Sequence.Init (comment is corresponding core return value)
   case eERR_SEQUENCE_ROUTING: //C_RANGE
      c_Activity = "Initialize Sequence";
      c_Error = "Routing configuration failed (can all nodes marked as active be reached from the defined bus?)";
      break;
   case eERR_SEQUENCE_UNKNOWN_CONFIG: //C_OVERFLOW
      c_Activity = "Initialize Sequence";
      c_Error = "Unknown transport protocol or unknown diagnostic server for at least one node.";
      break;
   case eERR_SEQUENCE_DLL_NOT_FOUND: //C_RD_RW
      c_Activity = "Initialize Sequence";
      c_Error = "Configured communication DLL not found.";
      break;
   case eERR_SEQUENCE_INVALID_PARAM: //C_NOACT
      c_Activity = "Initialize Sequence";
      c_Error = "Parameter invalid or no active nodes.";
      break;
   case eERR_SEQUENCE_CAN_INIT: //C_COM
      c_Activity = "Initialize Sequence";
      c_Error = "CAN initialization failed or no active node.";
      break;
   case eERR_SEQUENCE_C_CONFIG: //C_CONFIG
      c_Activity = "Initialize Sequence";
      c_Error = "Invalid system definition for parameters or\n"
                "active bus index refers to a bus that is not part of the system definition or\n"
                "length of active nodes vector is not identical to number of nodes in system definition or\n"
                "no STW Flashloader devices and no openSYDE devices are active or\n"
                "no STW Flashloader devices on active bus and no openSYDE devices are active, but STW Flashloader on"
                "other buses are active";
      break;
   case eERR_SEQUENCE_BUFF_OVERFLOW: //C_CHECKSUM
      c_Activity = "Initialize Sequence";
      c_Error = "Internal buffer overflow detected.";
      break;

   // results from c_Sequence.ActivateFlashloader (comment is corresponding core return value)
   case eERR_ACTIVATE_FLASHLOADER_FAILED: //C_WARN
      c_Activity = "Activate Flashloader";
      c_Error = "Activation for at least one device failed.";
      break;
   case eERR_ACTIVATE_FLASHLOADER_C_COM: //C_COM
      c_Activity = "Activate Flashloader";
      c_Error = "Communication driver reported problem.";
      break;
   case eERR_ACTIVATE_FLASHLOADER_SYSDEF: //C_CONFIG
      c_Activity = "Activate Flashloader";
      c_Error = "System definition is NULL (Init() not called)";
      break;

   // results from c_Sequence.UpdateSystem (comment is corresponding core return value)
   case eERR_UPDATE_ABORTED: //C_BUSY
      c_Activity = "Update System";
      c_Error = "Procedure aborted by user.";
      break;
   case eERR_UPDATE_C_OVERFLOW: //C_OVERFLOW
      c_Activity = "Update System";
      c_Error = "Size of applications to write differs from size of nodes.\n"
                "Or for address based targets: device name of device does not match name contained in HEX file.";
      break;
   case eERR_UPDATE_C_RD_RW: //C_RD_RW
      c_Activity = "Update System";
      c_Error = "File referenced by application does not exist or\n"
                "for address based targets: one of the files is not a valid Intel or Motorola HEX file or\n"
                "for address based targets: could not split up HEX file in individual areas";
      break;
   case eERR_UPDATE_C_NOACT: //C_NOACT
      c_Activity = "Update System";
      c_Error = "There exists an application-to-write for a node that was not set as active or\n"
                "there exists an application-to-write for a node that has no position in the update order or\n"
                "there exists an application-to-write for a node that has more than one position in the update order or\n"
                "for address based targets: could not extract device name from HEX file.";
      break;
   case eERR_UPDATE_C_COM: //C_COM
      c_Activity = "Update System";
      c_Error = "Communication driver reported problem.";
      break;
   case eERR_UPDATE_SYSDEF: //C_CONFIG
      c_Activity = "Update System";
      c_Error = "System definition is NULL (Init() not called) or \n"
                "for address based targets: no signature block found in HEX file.";
      break;

   // general results (comment is corresponding core return value)
   case eOK: //C_NO_ERR
      // no error, everything worked fine (and user already got informed about success)
      break;
   case eERR_UNKNOWN: //undocumented and unexpected result in called functions
      c_Activity = "SYDEsup";
      c_Error = "Unknown error occurred.";
      break;
   }

   // write to console and log file
   if ((ore_Result == eERR_PARSE_COMMAND_LINE) || (ore_Result == eOK))
   {
      // do nothing (parsing errors already printed to console/success already reported)
   }
   else
   {
      h_WriteLog(c_Activity, c_Error, true);
      std::cout << "\n";
      h_WriteLog("System Update", "Could not update system!", true);
      std::cout << "See log file for information: " << mc_LogFile.c_str() << "\n";
      std::cout << "\n";
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Conclude (reset system and close CAN DLL)

   \param[in]     orc_CanDispatcher    CAN dispatcher
   \param[in]     orc_Sequence         system update sequence
   \param[in]     orq_CanLoaded        true: orc_CanDispatcher.CAN_Open() successfully called; false: else
   \param[in]     orq_ResetSystem      true: Flashloader activation failed or system update succeeded; false: else

   \created     04.09.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_SYDEsup::m_Conclude(C_CAN & orc_CanDispatcher, C_SUPSuSequences & orc_Sequence, const bool & orq_CanLoaded,
                           const bool & orq_ResetSystem) const
{
   sint32 s32_Result;

   // reset system (if Flashloader activation failed or system update succeeded)
   if (orq_ResetSystem == true)
   {
      s32_Result = orc_Sequence.ResetSystem();
      if (s32_Result == C_NO_ERR)
      {
         h_WriteLog("Reset System", "System reset successful.", false, mq_Quiet);
      }
      else
      {
         h_WriteLog("Reset System", "Could not reset system.", true, mq_Quiet);
      }
   }

   // close CAN (if CAN was loaded)
   if (orq_CanLoaded == true)
   {
      s32_Result = orc_CanDispatcher.DLL_Close();
      if (s32_Result == C_NO_ERR)
      {
         h_WriteLog("Close CAN DLL", "CAN DLL successfully closed.", false, mq_Quiet);
      }
      else
      {
         h_WriteLog("Close CAN DLL", "Could not close CAN DLL.", true, mq_Quiet);
      }
   }
}
