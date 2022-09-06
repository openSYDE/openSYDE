//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class of SYDEsup (implementation)

   This is the base class and contains most functionality of SYDEsup.
   SYDEsup is a console application for updating your system with a Service
   Update Package created with openSYDE.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <iostream>
#include <getopt.h> //note: as we use getopt.h this application is not portable to all compilers

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "C_SYDEsup.h"
#include "C_OSCSuServiceUpdatePackage.h"
#include "C_OSCLoggingHandler.h"
#include "TGLTime.h"
#include "TGLFile.h"
#include "C_SUPSuSequences.h"
#include "C_OSCBinaryHash.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;
using namespace stw_can;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::C_SYDEsup(void) :
   mpc_CanDispatcher(NULL),
   mpc_EthDispatcher(NULL),
   mq_Quiet(false),
   mc_SUPFilePath(""),
   mc_CanDriver(""),
   mc_LogPath(""),
   mc_LogFile(""),
   mc_UnzipPath(""),
   mc_CertFolderPath("")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::~C_SYDEsup(void)
{
   mpc_CanDispatcher = NULL;
   mpc_EthDispatcher = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get options from command line

   Parse command line parameters.
   If required parameters are not present print a list of options to the console.

   Parameters are:
   * -h for help
   * -v for version
   * -m for manual page
   * -q for quiet
   * -p for package file path
   * -i for CAN interface information (optional but needed if active bus in package is of type CAN, not Ethernet)
   * -z for path to unzip directory (optional)
   * -l for path to log file (optional)

   \param[in]  osn_Argc    number of command line arguments
   \param[in]  oppcn_Argv  command line arguments

   \return
   eOK                        initialization worked
   eERR_PARSE_COMMAND_LINE    missing or invalid command line parameters or help requested
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsup::ParseCommandLine(const sintn osn_Argc, charn * const * const oppcn_Argv)
{
   C_SYDEsup::E_Result e_Return = eOK;
   sintn sn_Result;
   bool q_ParseError = false;
   bool q_ShowHelp = false;
   bool q_ShowManPage = false;
   bool q_ShowVersionOnly = false;
   const C_SCLString c_Version = m_GetApplicationVersion(TGL_GetExePath());
   const C_SCLString c_BinaryHash = stw_opensyde_core::C_OSCBinaryHash::h_CreateBinaryHash();

   mq_Quiet = false;

   const struct option ac_Options[] =
   {
      /* name, has_arg, flag, val */
      {
         "help",              no_argument,         NULL,    'h'
      },
      {
         "man",               no_argument,         NULL,    'm'
      },
      {
         "version",           no_argument,         NULL,    'v'
      },
      {
         "quiet",             no_argument,         NULL,    'q'
      },
      {
         "packagefile",       required_argument,   NULL,    'p'
      },
      {
         "caninterface",      required_argument,   NULL,    'i'
      },
      {
         "unzipdir",          required_argument,   NULL,    'z'
      },
      {
         "logdir",            required_argument,   NULL,    'l'
      },
      {
         "certificatesdir",   required_argument,   NULL,    'c'
      },
      {
         NULL,           0,                   NULL,    0
      }
   };

   do
   {
      sintn sn_Index;
      sn_Result = getopt_long(osn_Argc, oppcn_Argv, "hmvqp:i:z:l:c:", &ac_Options[0], &sn_Index);
      if (sn_Result != -1)
      {
         switch (sn_Result)
         {
         case 'h':
            q_ShowHelp = true;
            break;
         case 'm':
            q_ShowHelp = true;
            q_ShowManPage = true;
            break;
         case 'v':
            q_ShowVersionOnly = true;
            break;
         case 'q':
            mq_Quiet = true;
            break;
         case 'p':
            mc_SUPFilePath = optarg;
            break;
         case 'i':
            mc_CanDriver = optarg;
            break;
         case 'z':
            mc_UnzipPath = optarg;
            break;
         case 'l':
            mc_LogPath = optarg;
            break;
         case 'c':
            mc_CertFolderPath = optarg;
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

   if ((q_ShowVersionOnly == true) && (q_ShowHelp == false))
   {
      this->m_PrintVersion(c_Version, c_BinaryHash, false);
      e_Return = eERR_PARSE_COMMAND_LINE;
   }
   else
   {
      if (mq_Quiet == false)
      {
         // print banner
         std::cout <<
            " ________       ___    ___ ________  _______   ________  ___  ___  ________\n"
            "|\\   ____\\     |\\  \\  /  /|\\   ___ \\|\\  ___ \\ |\\   ____\\|\\  \\|\\  \\|\\   __  \\\n"
            "\\ \\  \\___|_    \\ \\  \\/  / | \\  \\_|\\ \\ \\   __/|\\ \\  \\___|\\ \\  \\\\\\  \\ \\  \\|\\  \\\n"
            " \\ \\_____  \\    \\ \\    / / \\ \\  \\ \\\\ \\ \\  \\_|/_\\ \\_____  \\ \\  \\\\\\  \\ \\   ____\\\n"
            "  \\|____|\\  \\    \\/  /  /   \\ \\  \\_\\\\ \\ \\  \\_|\\ \\|____|\\  \\ \\  \\\\\\  \\ \\  \\___|\n"
            "    ____\\_\\  \\ __/  / /      \\ \\_______\\ \\_______\\____\\_\\  \\ \\_______\\ \\__\\\n"
            "   |\\_________\\\\___/ /        \\|_______|\\|_______|\\_________\\|_______|\\|__|\n"
            "   \\|_________\\|___|/                            \\|_________|\n" << &std::endl;
      }

      // handle helper command line options and errors
      if (q_ShowHelp == true)
      {
         // logging not yet set up -> print directly to console
         this->m_PrintVersion(c_Version, c_BinaryHash, !mq_Quiet);
         this->m_PrintInformation(q_ShowManPage);
         e_Return = eERR_PARSE_COMMAND_LINE;
      }
      else if ((mc_SUPFilePath == "") || (q_ParseError == true))
      {
         // logging not yet set up -> print directly to console
         this->m_PrintVersion(c_Version, c_BinaryHash, !mq_Quiet);
         std::cout << "Error: Invalid or missing command line parameters, try -h." << &std::endl;
         e_Return = eERR_PARSE_COMMAND_LINE;
      }
      else
      {
         const stw_scl::C_SCLString c_Date = __DATE__;
         const stw_scl::C_SCLString c_Time = __TIME__;

         // Initialize optional parameters and setup logging
         e_Return = this->m_InitOptionalParameters(); // only error here is not-existing unzip directory

         // log extended version information
         h_WriteLog("SYDEsup Version", "SYDEsup Version: " + c_Version + ", MD5-Checksum: " + c_BinaryHash);
         h_WriteLog("SYDEsup Version", "   Binary: " + stw_tgl::TGL_GetExePath(), false, mq_Quiet);
         h_WriteLog("SYDEsup Version", "   Build date: " + c_Date + " " + c_Time, false, mq_Quiet);

         if (e_Return != eOK)
         {
            h_WriteLog("Initialize Parameters",  this->mc_UnzipPath + " is no existing directory.", true);
         }
      }
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update to the given Service Update Package.

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
   eERR_PACKAGE_WRONG_EXTENSION

   // results from opening CAN interface
   eERR_CAN_IF_NOT_FOUND
   eERR_CAN_IF_LOAD_FAILED

   // results from opening ETH driver
   eERR_ETH_IF_LOAD_FAILED

   // results from c_Sequence.Init
   eERR_SEQUENCE_ROUTING
   eERR_SEQUENCE_UNKNOWN_CONFIG
   eERR_SEQUENCE_CAN_IF_NOT_FOUND
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
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsup::Update(void)
{
   E_Result e_Result = eOK;
   sint32 s32_Return = C_NO_ERR;
   C_OSCSystemDefinition c_SystemDefinition;
   uint32 u32_ActiveBusIndex = 0;
   C_SCLStringList c_WarningMessages;
   C_SCLString c_ErrorMessage;
   C_SUPSuSequences c_Sequence;
   bool q_ResetSystem = false;

   bool q_PackageIsZip = false;

   std::vector<uint8> c_ActiveNodes;
   std::vector<uint32> c_NodesUpdateOrder;
   std::vector<C_OSCSuSequences::C_DoFlash> c_ApplicationsToWrite;

   //if file extension is not empty we can assume it's a file and we further need to check whether the extension
   //matches, otherwise it's mc_SUPFilePath is a directory
   if ((TGL_ExtractFileExtension(this->mc_SUPFilePath) != "") &&
       (TGL_ExtractFileExtension(this->mc_SUPFilePath) == ".syde_sup"))
   {
      q_PackageIsZip = true;
   }
   else if ((TGL_ExtractFileExtension(this->mc_SUPFilePath) != "") &&
            (TGL_ExtractFileExtension(this->mc_SUPFilePath) != ".syde_sup"))
   {
      e_Result = eERR_PACKAGE_WRONG_EXTENSION;
   }
   else
   {
      //no file extension -> plain directory -> nothing to here: q_PackageIsZip still false
   }

   if (e_Result == eOK)
   {
      // unpack Service Update Package which was created with openSYDE (also checks if paths are valid)
      s32_Return = C_OSCSuServiceUpdatePackage::h_ProcessPackage(mc_SUPFilePath, mc_UnzipPath,
                                                                 c_SystemDefinition, u32_ActiveBusIndex,
                                                                 c_ActiveNodes, c_NodesUpdateOrder,
                                                                 c_ApplicationsToWrite,
                                                                 c_WarningMessages, c_ErrorMessage, q_PackageIsZip);

      // report success or translate errors
      switch (s32_Return) // here s32_Return is result of h_UnpackPackage
      {
      case C_NO_ERR:
         if (q_PackageIsZip)
         {
            h_WriteLog("Unzip Package", "Service Update Package unzipped to \"" + mc_UnzipPath + "\".");
         }
         else
         {
            h_WriteLog("Process Package", "Service Update Package was processed from \"" + mc_SUPFilePath + "\".");
         }
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
      case C_DEFAULT:
         e_Result = eERR_PACKAGE_FILES_MISSING;
         break;
      default:
         e_Result = eERR_UNKNOWN;
         break;
      }
   }

   // go on if previous step was successful
   if (e_Result == eOK)
   {
      tgl_assert(u32_ActiveBusIndex < c_SystemDefinition.c_Buses.size());

      // do CAN stuff if CAN bus
      if (u32_ActiveBusIndex < c_SystemDefinition.c_Buses.size())
      {
         if (c_SystemDefinition.c_Buses[u32_ActiveBusIndex].e_Type == C_OSCSystemBus::eCAN)
         {
            e_Result = m_OpenCan(mc_CanDriver, c_SystemDefinition.c_Buses[u32_ActiveBusIndex].u64_BitRate);
            if (e_Result == eOK)
            {
               h_WriteLog("Open CAN", "CAN driver initialized.");
               tgl_assert(mpc_CanDispatcher != NULL);
            }
         }
         else if (c_SystemDefinition.c_Buses[u32_ActiveBusIndex].e_Type == C_OSCSystemBus::eETHERNET)
         {
            e_Result = m_OpenEthernet();
            if (e_Result == eOK)
            {
               h_WriteLog("Open Ethernet", "Ethernet driver initialized.");
               tgl_assert(mpc_EthDispatcher != NULL);
            }
         }
         else
         {
            tgl_assert(false);
         }
      }
   }

   // optional step: load pem database if cmd line parameter is not empty
   if (e_Result == eOK)
   {
      if (s32_Return == C_NO_ERR)
      {
         //parse pem database if cmd line parameter is not empty
         if (mc_CertFolderPath != "")
         {
            s32_Return = mc_PemDatabase.ParseFolder(mc_CertFolderPath.c_str());

            if (s32_Return != C_NO_ERR)
            {
               e_Result = eERR_UPDATE_CERTIFICATE_PATH;
               s32_Return = C_DEFAULT;
               h_WriteLog("Load PEM database",
                          "Could not load certificates (PEM files) at path \"" + this->mc_CertFolderPath + "\"");
            }
         }
      }
   }

   // initialize sequence if previous step (unpacking if Ethernet, CAN initializing if CAN) was successful
   if (e_Result == eOK)
   {
      if (s32_Return == C_NO_ERR)
      {
         // initialize sequence
         s32_Return = c_Sequence.Init(c_SystemDefinition, u32_ActiveBusIndex, c_ActiveNodes, mpc_CanDispatcher,
                                      mpc_EthDispatcher, &this->mc_PemDatabase);
         // tell report methods to not print to console
         c_Sequence.SetQuiet(mq_Quiet);
      }
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
         std::cout << &std::endl;
         q_ResetSystem = true;
         break;
      case C_DEFAULT:
         e_Result = eERR_UPDATE_CHECKSUM;
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
      case C_RANGE:
         e_Result = eERR_UPDATE_NO_NVM;
         break;
      case C_CHECKSUM:
         e_Result = eERR_UPDATE_AUTHENTICATION;
         break;
      default:
         e_Result = eERR_UNKNOWN;
         break;
      }
   }

   // inform user about errors
   this->m_PrintStringFromError(e_Result);

   // conclude (reset system and close CAN driver)
   this->m_Conclude(c_Sequence, q_ResetSystem);

   return e_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write to log file and print to console.

   Write detailed information with logging engine to log file but
   only print concise details to console.

   \param[in]  orc_Activity   Activity
   \param[in]  orc_Text       Information text
   \param[in]  orq_IsError    True: log as error; false: log as information
   \param[in]  orq_Quiet      Quiet flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SYDEsup::h_WriteLog(const stw_scl::C_SCLString & orc_Activity, const stw_scl::C_SCLString & orc_Text,
                           const bool & orq_IsError, const bool & orq_Quiet)
{
   if (orq_IsError == true)
   {
      osc_write_log_error(orc_Activity, orc_Text);
      std::cout << "Error: " << orc_Text.c_str() << &std::endl;
   }
   else
   {
      osc_write_log_info(orc_Activity, orc_Text);
      // do not write information if user wants me to be quiet
      if (orq_Quiet == false)
      {
         std::cout << orc_Text.c_str() << &std::endl;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize optional parameters unzip path and logging path

   Check if path exists and replace it with default location if necessary.
   Set logging file.

   \return
   eOK                        initialization worked
   eERR_PARSE_COMMAND_LINE    unzip directory is provided but does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsup::m_InitOptionalParameters(void)
{
   E_Result e_Return = eOK;

   // unzip path:
   if (mc_UnzipPath == "")
   {
      // default location is Service Update Package file path "converted" to directory
      mc_UnzipPath = TGL_ChangeFileExtension(mc_SUPFilePath, ""); // note: unzipping checks if this location is valid
   }
   else if (TGL_DirectoryExists(mc_UnzipPath) == true)
   {
      // unzip to sub directory with name of update package
      mc_UnzipPath = TGL_FileIncludeTrailingDelimiter(mc_UnzipPath) +
                     TGL_ChangeFileExtension(TGL_ExtractFileName(mc_SUPFilePath), "");
   }
   else
   {
      // User provided not-existing directory, which is an invalid command line parameter
      e_Return = C_SYDEsup::eERR_PARSE_COMMAND_LINE;
   }

   // log path (gets created, so just check for non empty):
   if (mc_LogPath == "")
   {
      // default location
      mc_LogPath = this->m_GetDefaultLogLocation();
   }

   // add trailing path delimiter in case there is none
   mc_LogPath = TGL_FileIncludeTrailingDelimiter(mc_LogPath);

   // log to file
   mc_LogFile = this->m_GetLogFileLocation();
   C_OSCLoggingHandler::h_SetCompleteLogFileLocation(mc_LogFile);
   std::cout << "The following output also is logged to the file: " << mc_LogFile.c_str() << &std::endl;
   C_OSCLoggingHandler::h_SetWriteToConsoleActive(false);
   C_OSCLoggingHandler::h_SetWriteToFileActive(true);

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Print version and MD5-Checksum directly to console without logging.

   \param[in]  orc_Version       Version
   \param[in]  orc_BinaryHash    Binary hash
   \param[in]  oq_Detailed       Show details
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SYDEsup::m_PrintVersion(const C_SCLString & orc_Version, const C_SCLString & orc_BinaryHash,
                               const bool oq_Detailed) const
{
   std::cout << "SYDEsup Version: " << orc_Version.c_str() << ", " <<
      "MD5-Checksum: " << orc_BinaryHash.c_str() << &std::endl;

   if (oq_Detailed == true)
   {
      std::cout << "   Binary: " << stw_tgl::TGL_GetExePath().c_str() <<
         "\n   Build date: " << __DATE__ << " " << __TIME__ << &std::endl;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Print information about application and command line parameters.

   Print directly to console, as logging is probably not set up yet and not necessary though.

   \param[out]  oq_Detailed   Print detailed application description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SYDEsup::m_PrintInformation(const bool oq_Detailed) const
{
   const C_SCLString c_PathWithTrailingDelimiter = TGL_FileIncludeTrailingDelimiter("x");
   const C_SCLString c_PathDelimiter = c_PathWithTrailingDelimiter.SubString(2, c_PathWithTrailingDelimiter.Length());

   if (oq_Detailed == true)
   {
      std::cout <<
         "\nThis tool \"SYDEsup\" is part of the openSYDE tool chain by STW (Sensor-Technik Wiedemann GmbH).\n"
         "It allows to update a openSYDE compatible system of multiple nodes in a single step.\n"
         "The update data must be provided in the openSYDE Service Update Package"
         "format (*.syde_sup), which can be created with the openSYDE PC tool.\n\n"
         "For further information take a look at openSYDE user manual." << &std::endl;
   }

   // show parameter help
   std::cout << "\nCommand line parameters:\n\n"
      "Flag   Alternative         Description                                     Default         Example\n"
      "---------------------------------------------------------------------------------------------------------------\n"
      "-h     --help              Print help                                                      -h\n"
      "-m     --manpage           Print manual page                                               -m\n"
      "-v     --version           Print version                                                   -v\n"
      "-q     --quiet             Print less console output                                       -q\n"
      "-p     --packagefile       Path to Service Update Package file             <none>          -p ." <<
      c_PathDelimiter.c_str() << "MyPackage.syde_sup\n"
      "-i     --caninterface      CAN interface                                   <none>          " <<
      this->m_GetCanInterfaceUsageExample().c_str() << "\n"
      "-z     --unzipdir          Existing directory where files get unzipped     <packagefile>   -z ." <<
      c_PathDelimiter.c_str() << "MyUnzipDir\n"
      "-l     --logdir            Directory for log files                         " <<
      this->m_GetDefaultLogLocation().c_str() <<      "          -l ." << c_PathDelimiter.c_str() << "MyLogDir\n"
      "-c     --certificatesdir   Directory for certificates (PEM-files)          <none>          -c ." <<
      c_PathDelimiter.c_str() << "MyCertificatesDir\n"
      "The package file parameter \"-p\" is mandatory, all others are optional. \n"
      "If the active bus in the given Service Update Package is of CAN type, a CAN interface must be provided." <<
      &std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Define log file location.

   Create log file name depending on date and time and set log file path
   to WorkingDirectory/Logs/log-file-name.syde_log.
   Only call this method once!

   \return
   log file name and location.
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Print error description from error result

   \param[in]  ore_Result  error enum
*/
//----------------------------------------------------------------------------------------------------------------------
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
      c_Error = "Could not unzip Service Update Package.";
      break;
   case eERR_PACKAGE_CORE_C_NOACT: //C_NOACT
      c_Activity = "Unzip Package";
      c_Error =
         "Error code of a called core function (should not occur for valid and compatible Service Update Package).";
      break;
   case eERR_PACKAGE_NOT_FOUND: //C_CONFIG
      c_Activity = "Unzip Package";
      c_Error = "Either could not find Service Update Package or System Definition content is invalid or incomplete.";
      break;
   case eERR_PACKAGE_WRONG_EXTENSION:
      c_Activity = "Unzip Package";
      c_Error = "Service Update Package has wrong file extension. File extension must be *.syde_sup.";
      break;
   case eERR_PACKAGE_FILES_MISSING: //C_DEFAULT
      c_Activity = "Unzip Package";
      c_Error = "Service Update Package misses mandatory files. See manual to learn about mandatory files.";
      break;

   // result from check if driver exists
   case eERR_CAN_IF_NOT_FOUND:
      c_Activity = "CAN interface";
      c_Error = "Could not find CAN driver: \"" + mc_CanDriver + "\".";
      break;
   // results from m_OpenCan
   case eERR_CAN_IF_LOAD_FAILED:
      c_Activity = "Load CAN driver";
      c_Error = "CAN driver loading or initialization failed.";
      break;
   // results from m_OpenEthernet
   case eERR_ETH_IF_LOAD_FAILED:
      c_Activity = "Load ethernet driver";
      c_Error = "Ethernet driver loading or initialization failed.";
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
   case eERR_SEQUENCE_CAN_IF_NOT_FOUND: //C_RD_RW
      c_Activity = "Initialize Sequence";
      c_Error = "Configured communication driver not found.";
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
      c_Error = "Invalid system definition for parameters or "
                "active bus index refers to a bus that is not part of the system definition or "
                "length of active nodes vector is not identical to number of nodes in system definition or "
                "no STW Flashloader devices and no openSYDE devices are active or "
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
      c_Error = "Size of applications to write differs from size of nodes. "
                "Or for address based targets: device name of device does not match name contained in HEX file.";
      break;
   case eERR_UPDATE_C_RD_RW: //C_RD_RW
      c_Activity = "Update System";
      c_Error = "File referenced does not exist. "
                "Or for address based targets: file is not a valid Intel or Motorola HEX file or "
                "could not split up HEX file in individual areas. "
                "Or for parameter writing: one of the files is not a valid .psi_syde file.";
      break;
   case eERR_UPDATE_C_NOACT: //C_NOACT
      c_Activity = "Update System";
      c_Error = "There exist flash or NVM files for a node that was not set as active or "
                "there exist flash or NVM files for a node that has no position in the update order or "
                "there exist flash or NVM files for a node that has more than one position in the update order. "
                "Or for address based targets: could not extract device name from HEX file.";
      break;
   case eERR_UPDATE_C_COM: //C_COM
      c_Activity = "Update System";
      c_Error = "Communication driver reported problem.";
      break;
   case eERR_UPDATE_SYSDEF: //C_CONFIG
      c_Activity = "Update System";
      c_Error = "System definition is NULL (Init() not called). "
                "Or for address based targets: no signature block found in HEX file. "
                "Or for parameter writing: one of the files contains data for zero or more than one device "
                "(expected: data for exactly one device).";
      break;
   case eERR_UPDATE_CHECKSUM: // C_CHECKSUM
      c_Activity = "Update System";
      c_Error = "For parameter writing one of the files has an invalid checksum.";
      break;
   case eERR_UPDATE_NO_NVM: // C_RANGE
      c_Activity = "Update System";
      c_Error = "At least one feature of the openSYDE Flashloader is not available for NVM writing.";
      break;
   case eERR_UPDATE_CERTIFICATE_PATH:
      c_Activity = "Load Certificates";
      c_Error = "Could not load certificates (PEM files) at path \"" + this->mc_CertFolderPath + "\"";
      break;
   case eERR_UPDATE_AUTHENTICATION:
      c_Activity = "Update System";
      c_Error = "Authentication between sydesup and device(s) has failed. Access denied."
                "Possible reasons:\n"
                "- Associated private key (*.pem) not found in certificates folder (most common)\n"
                "- Failure during authenfication process";
      break;

   // results regarding thread process issues
   case  eERR_THREAD_UPDATE_IN_PROGRESS:
      c_Activity = "Thread Process";
      c_Error = "Update task in thread is in progress.";
      break;
   case eERR_THREAD_UPDATE_INIT_FAILED:
      c_Activity = "Thread Process";
      c_Error = "Could not initialize thread for update task.";
      break;

   // general results
   case eOK: //C_NO_ERR
      // no error, everything worked fine (and user already got informed about success)
      break;
   case eERR_UNKNOWN: //undocumented and unexpected result in called functions
      c_Activity = "SYDEsup";
      c_Error = "Unknown error occurred.";
      break;
   default:
      tgl_assert(false); //all cases are expected to be handled explicitly
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
      h_WriteLog("System Update", "Could not update system! Tool result code: " +
                 C_SCLString::IntToStr(static_cast<sintn>(ore_Result)), true);
      std::cout << "See openSYDE user manual or log file for information: " << mc_LogFile.c_str()  << "\n" <<
         &std::endl;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Conclude (reset system and close CAN DLL)

   \param[in]  orc_Sequence      system update sequence
   \param[in]  orq_ResetSystem   true: Flashloader activation failed or system update succeeded; false: else
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SYDEsup::m_Conclude(C_SUPSuSequences & orc_Sequence, const bool & orq_ResetSystem)
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

   // close CAN
   this->m_CloseCan();
}
