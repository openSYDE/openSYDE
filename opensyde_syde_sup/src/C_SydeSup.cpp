//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class of SYDEsup (implementation)

   This is the base class and contains most functionality of SYDEsup.
   SYDEsup is a console application for updating your system with a Service
   Update Package created with openSYDE. It also can create this package
   from an openSYDE project.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>
#include <getopt.h> //note: as we use getopt.h this application is not portable to all compilers

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "C_SydeSup.hpp"
#include "C_OscSupServiceUpdatePackageV1.hpp"
#include "C_OscSupServiceUpdatePackageLoad.hpp"
#include "C_OscLoggingHandler.hpp"
#include "TglTime.hpp"
#include "TglFile.hpp"
#include "C_SupSuSequences.hpp"
#include "C_SupCreatePackage.hpp"
#include "C_OscBinaryHash.hpp"
#include "C_OscHexFile.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;
using namespace stw::can;

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
C_SydeSup::C_SydeSup(void) :
   mpc_CanDispatcher(NULL),
   mpc_EthDispatcher(NULL),
   mq_Quiet(false),
   mq_OnlyNecessaryFiles(false),
   me_OperationMode(eMODE_UPDATE),
   mc_OperationMode(""),
   mc_SupFilePath(""),
   mc_CanDriver(""),
   mc_LogPath(""),
   mc_LogFile(""),
   mc_UnzipPath(""),
   mc_CertFolderPath(""),
   mc_OsyProjectPath(""),
   mc_ViewName(""),
   mc_DeviceDefPath("")

{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSup::~C_SydeSup(void)
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
   * -o for operation mode (one of: update, createpackage)
   * -n for only transfer files if necessary
   * -q for quiet
   * -p for package file path
   * -i for CAN interface information (optional but needed if active bus in package is of type CAN, not Ethernet)
   * -z for path to unzip directory (optional)
   * -l for path to log file (optional)
   * -s for openSYDE project file path
   * -d for device definition file path
   * -w for view name
   * -k for PEM file
   * -x for password
   * -c for certificate files

   \param[in]  os32_Argc   number of command line arguments
   \param[in]  oppcn_Argv  command line arguments

   \return
   eOK                        initialization worked
   eERR_PARSE_COMMAND_LINE    missing or invalid command line parameters or help requested
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSup::E_Result C_SydeSup::ParseCommandLine(const int32_t os32_Argc, char_t * const * const oppcn_Argv)
{
   C_SydeSup::E_Result e_Return = eOK;
   int32_t s32_Result;
   bool q_ParseError = false;
   bool q_ShowHelp = false;
   bool q_ShowManPage = false;
   bool q_ShowVersionOnly = false;
   const C_SclString c_Version = m_GetApplicationVersion(TglGetExePath());
   const C_SclString c_BinaryHash = C_OscBinaryHash::h_CreateBinaryHash();

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
         "necessaryfiles",    no_argument,         NULL,    'n'
      },
      {
         "operationmode",     required_argument,   NULL,    'o'
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
         "opensydeproject",   required_argument,   NULL,    's'
      },
      {
         "devicedefinition",  required_argument,   NULL,    'd'
      },
      {
         "systemview",        required_argument,   NULL,    'w'
      },
      {
         "pemfile",           required_argument,   NULL,    'k'
      },
      {
         "password",          required_argument,   NULL,    'x'
      },
      {
         NULL,                0,                   NULL,    0
      }
   };

   do
   {
      int32_t s32_Index;
      s32_Result = getopt_long(os32_Argc, oppcn_Argv, "hmvqnp:o:i:z:l:c:s:w:d:k:x:", &ac_Options[0], &s32_Index);
      if (s32_Result != -1)
      {
         switch (s32_Result)
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
         case 'n':
            mq_OnlyNecessaryFiles = true;
            break;
         case 'o':
            mc_OperationMode = optarg;
            break;
         case 'p':
            mc_SupFilePath = optarg;
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
         case 's':
            mc_OsyProjectPath = optarg;
            break;
         case 'd':
            mc_DeviceDefPath = optarg;
            break;
         case 'w':
            mc_ViewName = optarg;
            break;
         case 'k':
            mc_PubKeyPemPath = optarg;
            break;
         case 'x':
            mc_Password = optarg;
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
      else if ((mc_SupFilePath == "") || (q_ParseError == true))
      {
         // logging not yet set up -> print directly to console
         this->m_PrintVersion(c_Version, c_BinaryHash, !mq_Quiet);
         std::cout << "Error: Invalid or missing command line parameters, try -h." << &std::endl;
         e_Return = eERR_PARSE_COMMAND_LINE;
      }
      else if ((mc_PubKeyPemPath == "") && (mc_Password != ""))
      {
         // logging not yet set up -> print directly to console
         this->m_PrintVersion(c_Version, c_BinaryHash, !mq_Quiet);
         std::cout <<
            "Error: Parameter \"-x\" is optional and only allowed in combination with a given pem file (\"-k\")" <<
            &std::endl;
         e_Return = eERR_PARSE_COMMAND_LINE;
      }
      else
      {
         const C_SclString c_Date = __DATE__;
         const C_SclString c_Time = __TIME__;

         // Initialize optional parameters and setup logging
         e_Return = this->m_InitOptionalParameters();

         // log extended version information
         h_WriteLog("SYDEsup Version", "SYDEsup Version: " + c_Version + ", MD5-Checksum: " + c_BinaryHash);
         h_WriteLog("SYDEsup Version", "   Binary: " + TglGetExePath(), false, mq_Quiet);
         h_WriteLog("SYDEsup Version", "   Build date: " + c_Date + " " + c_Time, false, mq_Quiet);

         // log command line call and parameters
         h_WriteLog("SYDEsup call",
                    "Command line: \"" + C_OscUtils::h_GetCommandLineAsString(os32_Argc, oppcn_Argv) + "\"");

         // After initialization of optional parameters: now we know the operation mode.
         // So let's check further parameters are provided for create-package-mode.
         if ((e_Return == eOK) && (me_OperationMode == eMODE_CREATEPACKAGE))
         {
            if ((mc_OsyProjectPath == "") || (mc_DeviceDefPath == "") || (mc_ViewName == ""))
            {
               e_Return = eERR_PARSE_COMMAND_LINE;
               h_WriteLog("Initialize Parameters",
                          "Missing command line parameter for creating a Service Update Package, try -h. ", true);
            }
         }
      }
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select operation mode and start the requested action (update system or create service update package).

   We support two different modes. Either use SYDEsup for updating your system with an existing service update package
   or use SYDEsup for creating a service update package.

   This method is the entrance point for SYDEsup and triggers the actual requested action.

   \return
   see C_SydeSup::Update and C_SydeSup::CreatePackage for return values
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSup::E_Result C_SydeSup::SelectModeAndStart(void)
{
   E_Result e_Result;

   if (me_OperationMode == eMODE_CREATEPACKAGE)
   {
      e_Result = this->CreatePackage();
   }
   else
   {
      e_Result = this->Update();
   }

   return e_Result;
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
   // for detailed error description see C_SydeSup::m_PrintStringFromError

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
C_SydeSup::E_Result C_SydeSup::Update(void)
{
   E_Result e_Result = eOK;
   int32_t s32_Return = C_NO_ERR;
   C_OscSystemDefinition c_SystemDefinition;
   uint32_t u32_ActiveBusIndex = 0;
   C_SclStringList c_WarningMessages;
   C_SclString c_ErrorMessage;
   C_SupSuSequences c_Sequence;
   bool q_ResetSystem = false;

   bool q_PackageIsZip = false;

   std::vector<uint8_t> c_ActiveNodes;
   std::vector<uint32_t> c_NodesUpdateOrder;
   std::vector<C_OscSuSequences::C_DoFlash> c_ApplicationsToWrite;

   //if file extension is not empty we can assume it's a file and we further need to check whether the extension
   //matches, otherwise mc_SUPFilePath is a directory
   if (TglExtractFileExtension(this->mc_SupFilePath) != "")
   {
      if (TglExtractFileExtension(this->mc_SupFilePath) == ".syde_sup")
      {
         q_PackageIsZip = true;
      }
      else
      {
         e_Result = eERR_PACKAGE_WRONG_EXTENSION;
      }
   }
   else
   {
      //no file extension -> plain directory -> nothing to here: q_PackageIsZip still false
   }

   //We don't really check for weird cmd line parameter combinations/different file version/etc right after tool start.
   //The Core engine decides if everything the user gave the tool is valid. So we just pass everything
   //we got from the user into the "ProcessPackage" function below.
   if (e_Result == eOK)
   {
      //see documentation of h_ProcessPackageUsingPemFiles for more information about the following variables
      //nodes that are encrypted with a password and shall be decrypted
      std::vector<uint8_t> c_DecryptNodes;
      //passwords for the nodes which shall be decrypted
      std::vector<C_SclString> c_PasswordsForDecryption;
      //pem files for the nodes which signatures shall be checked
      std::vector<C_SclString> c_PemFilesForNodes;

      //we only support "one pem rules them all" at the moment. Thus only one pem goes into the vector.
      //Later on there might be a "one pem/node" approach.
      c_PemFilesForNodes.push_back(mc_PubKeyPemPath);

      //we only support "all nodes are encrypted" at the moment.
      //Thus one "1" in c_DecryptNodes means "decrypt all nodes"
      c_DecryptNodes.push_back(1U);

      //the engine below will handle if an empty password was given
      c_PasswordsForDecryption.push_back(mc_Password);

      s32_Return = C_OscSupServiceUpdatePackageLoad::h_ProcessPackageUsingPemFiles(mc_SupFilePath, mc_UnzipPath,
                                                                                   c_SystemDefinition,
                                                                                   u32_ActiveBusIndex,
                                                                                   c_ActiveNodes, c_NodesUpdateOrder,
                                                                                   c_ApplicationsToWrite,
                                                                                   c_WarningMessages, c_ErrorMessage,
                                                                                   q_PackageIsZip, c_DecryptNodes,
                                                                                   c_PasswordsForDecryption,
                                                                                   c_PemFilesForNodes);

      // report success or translate errors
      switch (s32_Return) // here s32_Return is result of h_ProcessPackageUsingPemFiles
      {
      case C_NO_ERR:
         if (q_PackageIsZip)
         {
            h_WriteLog("Unzip Package", "Service Update Package unzipped to \"" + mc_UnzipPath + "\".");
         }
         else
         {
            h_WriteLog("Process Package", "Service Update Package was processed from \"" + mc_SupFilePath + "\".");
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
         //C_CONFIG can mean a lot of things through all the layers so we log the additional error message
         e_Result = eERR_PACKAGE_INVALID;
         h_WriteLog("Process Package", c_ErrorMessage, true);
         break;
      case C_DEFAULT:
         e_Result = eERR_PACKAGE_FILES_MISSING;
         break;
      case C_CHECKSUM:
         //can mean that the password is not correct or that the list of nodes to be decrypted/signature-checked does
         //not match with system definition, thus additional logging.
         e_Result = eERR_SEC_PAC_DECRYPTING;
         h_WriteLog("Process Package", c_ErrorMessage, true);
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
         if (c_SystemDefinition.c_Buses[u32_ActiveBusIndex].e_Type == C_OscSystemBus::eCAN)
         {
            e_Result = m_OpenCan(mc_CanDriver, c_SystemDefinition.c_Buses[u32_ActiveBusIndex].u64_BitRate);
            if (e_Result == eOK)
            {
               h_WriteLog("Open CAN", "CAN driver initialized.");
               tgl_assert(mpc_CanDispatcher != NULL);
            }
         }
         else if (c_SystemDefinition.c_Buses[u32_ActiveBusIndex].e_Type == C_OscSystemBus::eETHERNET)
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
         s32_Return = this->m_UpdateSystem(c_Sequence, c_SystemDefinition, c_NodesUpdateOrder, c_ActiveNodes,
                                           c_ApplicationsToWrite);
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
/*! \brief  Create service update package from given project

   Previous call to ParseCommandLineParameters required.

   \return
   eERR_CREATE_PROJ_LOAD_FAILED     Could not load project (system definition and/or system views)
   eERR_CREATE_VIEW_NOT_FOUND       Could not find a view with the name provided by user
   eERR_CREATE_ZIP_RD_RW            File writing problems occurred on output file creation or deletion
   eERR_CREATE_ZIP_CONFIG           Configuration problems occurred on update package creation
                                     (see C_OscSupServiceUpdatePackageV1::h_CreatePackage for further details)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSup::E_Result C_SydeSup::CreatePackage(void)
{
   E_Result e_Result;
   C_SupCreatePackage c_CreatePackage(mq_Quiet, mc_SupFilePath, mc_OsyProjectPath, mc_ViewName,
                                      mc_DeviceDefPath, mc_UnzipPath);

   e_Result = c_CreatePackage.Create();

   // inform if successful
   if (e_Result == eOK)
   {
      std::cout << "\n";
      h_WriteLog("Create Update Package", "Successfully created update package \"" + mc_SupFilePath + "\".");
      std::cout << &std::endl;
   }

   // inform user about errors
   this->m_PrintStringFromError(e_Result);

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
void C_SydeSup::h_WriteLog(const C_SclString & orc_Activity, const C_SclString & orc_Text, const bool & orq_IsError,
                           const bool & orq_Quiet)
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
   eERR_PARSE_COMMAND_LINE    unzip directory is provided but does not exist and/or
                              operation mode is provided but unknown
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSup::E_Result C_SydeSup::m_InitOptionalParameters(void)
{
   E_Result e_Return = eOK;

   // first setup logging
   this->m_InitLogging();

   // operation mode
   if ((mc_OperationMode == "") || (mc_OperationMode == "update"))
   {
      me_OperationMode = eMODE_UPDATE;
   }
   else if (mc_OperationMode == "createpackage")
   {
      me_OperationMode = eMODE_CREATEPACKAGE;
   }
   else
   {
      // User provided unknown mode, which is an invalid command line parameter
      e_Return = eERR_PARSE_COMMAND_LINE;
      h_WriteLog("Initialize Parameters", "\"" +  this->mc_OperationMode + "\" is no known operation mode.", true);
   }

   // unzip path:
   if (mc_UnzipPath == "")
   {
      if (this->me_OperationMode == eMODE_UPDATE)
      {
         // default location for updating is Service Update Package file path "converted" to directory
         mc_UnzipPath = TglChangeFileExtension(mc_SupFilePath, ""); // note: unzipping checks if this location is valid
      }
      // default location for package creation mode is handled by C_OscSupServiceUpdatePackageV1::h_CreatePackage
   }
   else if (TglDirectoryExists(mc_UnzipPath) == true)
   {
      if (this->me_OperationMode == eMODE_UPDATE)
      {
         // unzip to sub directory with name of update package
         mc_UnzipPath = TglFileIncludeTrailingDelimiter(mc_UnzipPath) +
                        TglChangeFileExtension(TglExtractFileName(mc_SupFilePath), "");
      }
      // sub directory for package creation mode is handled by C_OscSupServiceUpdatePackageV1::h_CreatePackage
   }
   else
   {
      // User provided not-existing directory, which is an invalid command line parameter
      e_Return = eERR_PARSE_COMMAND_LINE;
      h_WriteLog("Initialize Parameters", "\"" + this->mc_UnzipPath + "\" is no existing directory.", true);
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Print version and MD5-Checksum directly to console without logging.

   \param[in]  orc_Version       Version
   \param[in]  orc_BinaryHash    Binary hash
   \param[in]  oq_Detailed       Show details
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SydeSup::m_PrintVersion(const C_SclString & orc_Version, const C_SclString & orc_BinaryHash,
                               const bool oq_Detailed) const
{
   std::cout << "SYDEsup Version: " << orc_Version.c_str() << ", " <<
      "MD5-Checksum: " << orc_BinaryHash.c_str() << &std::endl;

   if (oq_Detailed == true)
   {
      std::cout << "   Binary: " << TglGetExePath().c_str() <<
         "\n   Build date: " << __DATE__ << " " << __TIME__ << &std::endl;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Print information about application and command line parameters.

   Print directly to console, as logging is probably not set up yet and not necessary though.

   \param[out]  oq_Detailed   Print detailed application description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SydeSup::m_PrintInformation(const bool oq_Detailed) const
{
   const C_SclString c_PathWithTrailingDelimiter = TglFileIncludeTrailingDelimiter("x");
   const C_SclString c_PathDelimiter = c_PathWithTrailingDelimiter.SubString(2, c_PathWithTrailingDelimiter.Length());

   if (oq_Detailed == true)
   {
      std::cout <<
         "\nThis tool \"SYDEsup\" is part of the openSYDE tool chain by STW (Sensor-Technik Wiedemann GmbH).\n"
         "It allows to update an openSYDE compatible system of multiple nodes in a single step.\n"
         "The update data must be provided in the openSYDE Service Update Package "
         "format (*.syde_sup), which can be created with the openSYDE PC tool.\n"
         "SYDEsup furthermore provides the possibility to create an openSYDE Service Update Package from an openSYDE "
         "project.\n\n"
         "For further information take a look at openSYDE user manual." << &std::endl;
   }

   // show parameter help
   std::cout << "\nCommand Line Parameters:\n--------------------------------\n\n"
      "Flag   Alternative         Description                                     Default         Example\n"
      "---------------------------------------------------------------------------------------------------------------\n"
      "General\n"
      "---------------------\n"
      "-h     --help              Print help                                                      -h\n"
      "-m     --manpage           Print manual page                                               -m\n"
      "-v     --version           Print version                                                   -v\n"
      "-q     --quiet             Print less console output                                       -q\n"
      "-l     --logdir            Directory for log files                         " <<
      this->m_GetDefaultLogLocation().c_str() <<
  //for fitting into table layout pad the log path to length 16 (if it is longer than 16 just leave as it is):
      std::string(static_cast<uint32_t>(
                     (this->m_GetDefaultLogLocation().Length() < 16) ?
                     (16 - this->m_GetDefaultLogLocation().Length()) : 0), ' ') <<
      "-l ." << c_PathDelimiter.c_str() << "MyLogDir\n"
      "-o     --operationmode     Set mode: \"update\" or \"createpackage\"           update          -o createpackage\n\n"
      "Package Creation\n"
      "---------------------\n"
      "-s     --opensydeproject   Path to openSYDE project file (SYDE file)       <none>          -s ." <<
      c_PathDelimiter.c_str() << "MyProject.syde\n"
      "-d     --devicedefinition  Path to device definitions file                 <none>          -d ." <<
      c_PathDelimiter.c_str() << "openSYDE" << c_PathDelimiter.c_str() << "devices" << c_PathDelimiter.c_str() <<
      "devices.ini\n"
      "-w     --systemview        Name of view in openSYDE project                <none>          -w ViewCAN1\n"
      "-p     --packagefile       Path for resulting Service Update Package file  <none>          -p ." <<
      c_PathDelimiter.c_str() << "MyPackage.syde_sup\n\n"
      "In createpackage mode the parameters packagefile \"-p\", opensydeproject \"-s\",\n"
      "systemview \"-w\" and devicedefinition \"-d\" are mandatory.\n\n"
      "Update\n"
      "---------------------\n"
      "-n     --necessaryfiles    Only transfer files if necessary.                               -n\n"
      "                           Files already on address based target will \n"
      "                           be skipped.            \n"
      "-p     --packagefile       Path to existing Service Update Package file    <none>          -p ." <<
      c_PathDelimiter.c_str() << "MyPackage.syde_sup\n"
      "-i     --caninterface      CAN interface                                   <none>          " <<
      this->m_GetCanInterfaceUsageExample().c_str() << "\n"
      "-z     --unzipdir          Existing directory for temporary files          <packagefile>   -z "  <<
      this->m_GetUnzipLocationDefaultExample().c_str() << "\n\n"
      "In update mode the package file parameter \"-p\" is mandatory, all others are optional.\n"
      "If the active bus in the given Service Update Package is of CAN type, a CAN interface must be provided.\n\n"
      "Secure Authentication\n"
      "---------------------\n"
      "-c     --certificatesdir   Directory for certificates (PEM files) for      <none>          -c ." <<
      c_PathDelimiter.c_str() << "MyCertificatesDir\n"
      "                           secure authentication with the openSYDE server\n\n"
      "Secure Update\n"
      "---------------------\n"
      "-k     --publickey         Path to PEM file holding the public key for     <none>          -k public_crt.pem\n"
      "                           verifying the authenticity of a signed\n"
      "                           Service Update Package\n"
      "-x     --password          Password for the encrypted Service Update       <none>          -x unh4ckab1e\n"
      "                           Package. Needed for secure update.\n\n"
      "If a signed Service Update Package shall be loaded the parameter publickey \"-k\" is mandatory.\n"
      "The parameter password \"-x\" is only used in combination with the parameter publickey \"-k\".\n"
      "Both parameters must match the provided Service Update Package.\n" << &std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize logging engine.

   Log file location is an optional parameter, so this is a part of initialization of optional parameters!
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SydeSup::m_InitLogging(void)
{
   // log path (gets created, so just check for non empty):
   if (mc_LogPath == "")
   {
      // default location
      mc_LogPath = this->m_GetDefaultLogLocation();
   }

   // add trailing path delimiter in case there is none
   mc_LogPath = TglFileIncludeTrailingDelimiter(mc_LogPath);

   // log to file
   mc_LogFile = this->m_GetLogFileLocation();
   C_OscLoggingHandler::h_SetCompleteLogFileLocation(mc_LogFile);
   std::cout << "The following output also is logged to the file: " << mc_LogFile.c_str() << &std::endl;
   C_OscLoggingHandler::h_SetWriteToConsoleActive(false);
   C_OscLoggingHandler::h_SetWriteToFileActive(true);
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
C_SclString C_SydeSup::m_GetLogFileLocation(void) const
{
   C_SclString c_LogFilePath;
   C_TglDateTime c_DateTime;

   TglGetDateTimeNow(c_DateTime);

   c_LogFilePath = C_OscLoggingHandler::h_UtilConvertDateTimeToString(c_DateTime).c_str();

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
void C_SydeSup::m_PrintStringFromError(const E_Result & ore_Result) const
{
   C_SclString c_Activity;
   C_SclString c_Error;

   switch (ore_Result)
   {
   case eERR_PARSE_COMMAND_LINE:
      // console output only (no writing to log file)
      // Invalid or missing command line parameters.
      break;

   // results from C_OscSupServiceUpdatePackageV1::h_ProcessPackage (comment is corresponding core return value)
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
   case eERR_PACKAGE_INVALID: //C_CONFIG
      c_Activity = "Unzip Package";
      c_Error = "Either could not find Service Update Package, Service Update Package file version is unknown or System"
                " Definition content is invalid or incomplete.";
      break;
   case eERR_PACKAGE_WRONG_EXTENSION:
      c_Activity = "Unzip Package";
      c_Error = "Service Update Package has wrong file extension. File extension must be *.syde_sup.";
      break;
   case eERR_PACKAGE_FILES_MISSING: //C_DEFAULT
      c_Activity = "Unzip Package";
      c_Error = "Service Update Package misses mandatory files. See manual to learn about mandatory files.";
      break;

   // results from C_OscSupServiceUpdatePackageLoad::h_ProcessPackage (comment is corresponding core return value)
   case eERR_SEC_PAC_NO_ZIP:
      c_Activity = "Unzip Secure Package";
      c_Error = "Tried to open a Secure Update Package from an unpacked folder / directory.";
      break;
   case eERR_SEC_PAC_INVALID_PACKAGE:
      c_Activity = "Unzip Secure Package";
      c_Error = "Secure Update Package must only contain signed \"*.syde_suc\" packages";
      break;
   case eERR_SEC_PAC_DECRYPTING:
      c_Activity = "Unzip Secure Package";
      c_Error = "Error while decrypting Secure Update Package. See log for details.";
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

   // results regarding creation of service update package
   case eERR_CREATE_PROJ_LOAD_FAILED:
      c_Activity = "Create Update Package";
      c_Error = "Could not load openSYDE project files from \"" + this->mc_OsyProjectPath +
                "\" with device definition \"" + this->mc_DeviceDefPath + "\".";
      break;
   case eERR_CREATE_VIEW_NOT_FOUND:
      c_Activity = "Create Update Package";
      c_Error = "Could not find view \"" + this->mc_ViewName + "\" in given openSYDE project.";
      break;
   case eERR_CREATE_ZIP_RD_RW:
      c_Activity = "Create Update Package";
      c_Error = "File reading or writing problem occured. E.g. could not delete existing Service Update Package file "
                "or could not create temporary folder or flash files are missing or could not write zip file. "
                "Possible reasons: incorrect path, insufficient write permission, maximum path length exceeded, ...";
      break;
   case eERR_CREATE_ZIP_CONFIG:
      c_Activity = "Create Update Package";
      c_Error = "Could not create zip package because configuration is invalid. "
                "Possible issues: node update position misconfigured, active nodes and/or application mismatch, "
                "bus index not found, ...";
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
      if (me_OperationMode == eMODE_CREATEPACKAGE)
      {
         h_WriteLog("Create Update Package", "Could not create Service Update Package! Tool result code: " +
                    C_SclString::IntToStr(static_cast<int32_t>(ore_Result)), true);
      }
      else
      {
         h_WriteLog("System Update", "Could not update system! Tool result code: " +
                    C_SclString::IntToStr(static_cast<int32_t>(ore_Result)), true);
      }
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
void C_SydeSup::m_Conclude(C_SupSuSequences & orc_Sequence, const bool & orq_ResetSystem)
{
   int32_t s32_Result;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update function of SYDEsup with optional X-Check feature to update only necessary files of

            address based targets

   Precondition for X-Check feature is that the orc_ApplicationsToWrite and the ReadDeviceInformation sequence order
   of nodes is the same. This was verified by various tests.

   \param[in,out]  orc_Sequence              SYDEsup sequence used for main update functionality
   \param[in]      orc_SystemDefinition      openSYDE system definition to get info of address/file based targets
   \param[in]      orc_NodesUpdateOrder      nodes update order for main update functionality
   \param[in]      orc_ActiveNodes           active nodes (0 .. node is inactive, 1 .. node is active)
   \param[in,out]  orc_ApplicationsToWrite   in case of used X-Check feature only necessary files are updated of
                                             address based targets

   \return
   C_NO_ERR    successful execution of update
   else        see error codes of UpdateSystem and SYDEsup log
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SydeSup::m_UpdateSystem(C_SupSuSequences & orc_Sequence, const C_OscSystemDefinition & orc_SystemDefinition,
                                  std::vector<uint32_t> & orc_NodesUpdateOrder,
                                  const std::vector<uint8_t> & orc_ActiveNodes,
                                  std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite) const
{
   int32_t s32_Result = C_NO_ERR;

   const uint16_t u16_DeviceCount = static_cast<uint16_t>(orc_ApplicationsToWrite.size());

   if (mq_OnlyNecessaryFiles == true)
   {
      std::vector<C_OscSuSequences::C_OsyDeviceInformation> c_ActiveOsyDeviceInformation;
      std::vector<C_OscSuSequences::C_XflDeviceInformation> c_ActiveXflDeviceInformation;
      std::vector<uint8_t> c_ActiveNodesTypes = this->m_GetActiveNodeTypes(orc_SystemDefinition,
                                                                           orc_ActiveNodes);

      //Collector for information from all nodes
      std::vector<C_NodeApplicationsHelperStruct> c_NodeApplicationsHelperStruct;
      c_NodeApplicationsHelperStruct.resize(orc_ActiveNodes.size());

      orc_Sequence.ClearActiveDeviceInformation();
      s32_Result = orc_Sequence.ReadDeviceInformation();

      if (s32_Result == C_NO_ERR)
      {
         std::vector<uint16_t> c_OsyDeviceIndexes;
         std::vector<uint16_t> c_XflDeviceIndexes;

         // Get server application information of active devices.
         // At this point we will get all openSYDE devices (file based and address based):
         c_ActiveOsyDeviceInformation = orc_Sequence.GetActiveOsyDeviceInformation(c_OsyDeviceIndexes);
         c_ActiveXflDeviceInformation = orc_Sequence.GetActiveXflDeviceInformation(c_XflDeviceIndexes);

         //Fill in information about openSYDE devices into collector list:
         for (uint16_t u16_IterActiveOsyDevices = 0; u16_IterActiveOsyDevices < c_ActiveOsyDeviceInformation.size();
              ++u16_IterActiveOsyDevices)
         {
            std::vector<C_OscSuSequences::C_ApplicationProperties> c_ServerSideApplications; // of current node

            // remember all reported application blocks in common data structure:
            std::vector<C_OscProtocolDriverOsy::C_FlashBlockInfo>::iterator c_IterApplications;
            for (c_IterApplications = c_ActiveOsyDeviceInformation[u16_IterActiveOsyDevices].c_Applications.begin();
                 c_IterApplications != c_ActiveOsyDeviceInformation[u16_IterActiveOsyDevices].c_Applications.end();
                 ++c_IterApplications)
            {
               C_OscSuSequences::C_ApplicationProperties c_Temp;

               c_Temp.c_Name = c_IterApplications->c_ApplicationName;
               c_Temp.c_Version = c_IterApplications->c_ApplicationVersion;
               c_Temp.c_BuildDate = c_IterApplications->c_BuildDate;
               c_Temp.c_BuildTime = c_IterApplications->c_BuildTime;

               c_ServerSideApplications.push_back(c_Temp);
            }

            // store server side application info
            c_NodeApplicationsHelperStruct[c_OsyDeviceIndexes[u16_IterActiveOsyDevices]].c_ServerSideApplications =
               c_ServerSideApplications;
         }
         //Fill in information about STW Flashloader devices into collector list:
         for (uint16_t u16_IterActiveXflDevices = 0; u16_IterActiveXflDevices < c_ActiveXflDeviceInformation.size();
              ++u16_IterActiveXflDevices)
         {
            std::vector<C_OscSuSequences::C_ApplicationProperties> c_ServerSideApplications; // of current node

            // remember all reported application blocks in common data structure:
            for (int32_t s32_Application = 0U;
                 s32_Application <
                 c_ActiveXflDeviceInformation[u16_IterActiveXflDevices].c_BasicInformation.c_DeviceInfoBlocks.GetLength();
                 s32_Application++)
            {
               const stw::diag_lib::C_XFLECUInformation & rc_Application =
                  c_ActiveXflDeviceInformation[u16_IterActiveXflDevices].c_BasicInformation.c_DeviceInfoBlocks[
                     s32_Application];
               C_OscSuSequences::C_ApplicationProperties c_Temp;

               c_Temp.c_Name = rc_Application.acn_ProjectName;
               c_Temp.c_Version = rc_Application.acn_ProjectVersion;
               c_Temp.c_BuildDate = rc_Application.acn_Date;
               c_Temp.c_BuildTime = rc_Application.acn_Time;

               c_ServerSideApplications.push_back(c_Temp);
            }

            // store server side application info
            c_NodeApplicationsHelperStruct[c_XflDeviceIndexes[u16_IterActiveXflDevices]].c_ServerSideApplications =
               c_ServerSideApplications;
         }

         // get client application information of active address based devices
         for (uint16_t u16_IterDevices = 0; u16_IterDevices < u16_DeviceCount; ++u16_IterDevices)
         {
            //for active address based devices we expect .hex files:
            if (c_ActiveNodesTypes[u16_IterDevices] == 1)
            {
               std::vector<C_OscSuSequences::C_ApplicationProperties> c_ClientSideApplications; // of current node

               std::vector<C_SclString>::iterator c_IterFiles;
               for (c_IterFiles = orc_ApplicationsToWrite[u16_IterDevices].c_FilesToFlash.begin();
                    c_IterFiles != orc_ApplicationsToWrite[u16_IterDevices].c_FilesToFlash.end();
                    ++c_IterFiles)
               {
                  // Path is already relative to the execution folder
                  const C_SclString c_Path = c_IterFiles->c_str();

                  C_OscHexFile c_HexFile;

                  const uint32_t u32_Result = c_HexFile.LoadFromFile(c_Path.c_str());
                  if (u32_Result == stw::hex_file::NO_ERR)
                  {
                     stw::diag_lib::C_XFLECUInformation c_FileApplicationInfo;
                     s32_Result = c_HexFile.ScanApplicationInformationBlockFromHexFile(c_FileApplicationInfo);
                     if ((s32_Result == C_NO_ERR) || (s32_Result == C_WARN))
                     {
                        C_OscSuSequences::C_ApplicationProperties c_Temp;

                        c_Temp.c_Name = c_FileApplicationInfo.GetProjectName();
                        c_Temp.c_Version = c_FileApplicationInfo.GetProjectVersion();
                        c_Temp.c_BuildDate = c_FileApplicationInfo.GetDate();
                        c_Temp.c_BuildTime = c_FileApplicationInfo.GetTime();

                        c_ClientSideApplications.push_back(c_Temp);
                        s32_Result = C_NO_ERR;
                     }
                  }
                  else
                  {
                     const C_SclString c_Text = "Could not open HEX file \"" +
                                                c_Path + "\" Details: " +
                                                c_HexFile.ErrorCodeToErrorText(u32_Result);
                     osc_write_log_warning("X-Check feature", c_Text);
                     s32_Result = C_WARN;
                     break;
                  }
               }

               // store client side application info
               c_NodeApplicationsHelperStruct[u16_IterDevices].c_ClientSideApplications = c_ClientSideApplications;
            }
            else
            {
               //no relevant files for file based or inactive devices -> skip
            }
         }
      }

      if (s32_Result == C_NO_ERR)
      {
         // now we are ready to compare client to server side applications
         for (uint16_t u16_IterDevices = 0; u16_IterDevices < u16_DeviceCount; ++u16_IterDevices)
         {
            // Device active and address based ?
            if (c_ActiveNodesTypes[u16_IterDevices] == 1)
            {
               std::vector<uint8_t> c_ApplicationsPresentOnServer;
               std::vector<C_SclString> c_FilesToFlashTemp;

               // check for changed applications
               C_OscSuSequences::h_CheckForChangedApplications(
                  c_NodeApplicationsHelperStruct[u16_IterDevices].c_ClientSideApplications,
                  c_NodeApplicationsHelperStruct[u16_IterDevices].c_ServerSideApplications,
                  c_ApplicationsPresentOnServer);

               for (uint16_t u16_IterApplications = 0;
                    u16_IterApplications <
                    c_NodeApplicationsHelperStruct[u16_IterDevices].c_ClientSideApplications.size();
                    ++u16_IterApplications)
               {
                  const C_SclString c_Temp =
                     orc_ApplicationsToWrite[u16_IterDevices].c_FilesToFlash[u16_IterApplications];
                  if (c_ApplicationsPresentOnServer[u16_IterApplications] == 0)
                  {
                     // current application needs to be updated on current device
                     const C_SclString c_Text = "File \"" + c_Temp + "\" not present on device. Needs updating ...";

                     c_FilesToFlashTemp.push_back(c_Temp);
                     osc_write_log_info("X-Check feature", c_Text);
                  }
                  else
                  {
                     const C_SclString c_Text = "File \"" + c_Temp +
                                                "\" present on device. No update needed -> skipping ...";
                     osc_write_log_info("X-Check feature", c_Text);
                  }
               }
               // erase operations on vectors are expensive, do it easy with clear and assign
               orc_ApplicationsToWrite[u16_IterDevices].c_FilesToFlash.clear();
               orc_ApplicationsToWrite[u16_IterDevices].c_FilesToFlash = c_FilesToFlashTemp;

               // check if we have to delete device in NodesOrder for update
               if ((orc_ApplicationsToWrite[u16_IterDevices].c_FilesToFlash.size() == 0) &&
                   (orc_ApplicationsToWrite[u16_IterDevices].c_FilesToWriteToNvm.size() == 0) &&
                   (orc_ApplicationsToWrite[u16_IterDevices].c_PemFile == ""))
               {
                  // remove entry in orc_NodesUpdateOrder (index is update position, value is node index of devices
                  // in system definition, in our case here u16_IterDevices)
                  // Be aware: if device not found here in nodes update order it must be an osy device which is active
                  //           but has no applications to flash. This is a valid possible scenario.
                  std::vector<uint32_t>::iterator c_IterUpdateOrder;
                  c_IterUpdateOrder = std::find(orc_NodesUpdateOrder.begin(),
                                                orc_NodesUpdateOrder.end(), u16_IterDevices);
                  if (c_IterUpdateOrder != orc_NodesUpdateOrder.end())
                  {
                     // delete device in NodesOrder
                     orc_NodesUpdateOrder.erase(c_IterUpdateOrder);
                  }
               }
            }
         }
      }
   }

   if ((s32_Result == C_NO_ERR) || (s32_Result == C_WARN))
   {
      s32_Result = orc_Sequence.UpdateSystem(orc_ApplicationsToWrite, orc_NodesUpdateOrder);
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper function return types of active nodes

   \param[in]  orc_SystemDefinition    openSYDE system definition to get info of address/file based targets
   \param[in]  orc_ActiveNodes         complete list of nodes with active and inactive information
                                       (0 .. node is inactive
                                       1 .. node is active  )

   \return
   for each element of vector
      0  ..   is not an active openSYDE address based or STW Flashloader node
      1  ..   is an active openSYDE address based or STW Flashloader node
      2  ..   is an active openSYDE file based node
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint8_t> C_SydeSup::m_GetActiveNodeTypes(const C_OscSystemDefinition & orc_SystemDefinition,
                                                     const std::vector<uint8_t> & orc_ActiveNodes) const
{
   std::vector<uint8_t> c_ActiveNodeTypes = orc_ActiveNodes;

   tgl_assert(orc_ActiveNodes.size() == orc_SystemDefinition.c_Nodes.size());

   //are all nodes supposed to get flashed active ?
   for (uint16_t u16_Node = 0U; u16_Node < orc_SystemDefinition.c_Nodes.size(); u16_Node++)
   {
      c_ActiveNodeTypes[u16_Node] = 0U;

      if (orc_ActiveNodes[u16_Node] == 1U)
      {
         const C_OscDeviceDefinition * const pc_DeviceDefinition =
            orc_SystemDefinition.c_Nodes[u16_Node].pc_DeviceDefinition;

         tgl_assert(pc_DeviceDefinition != NULL);
         if (pc_DeviceDefinition != NULL)
         {
            const uint32_t u32_SubDeviceIndex = orc_SystemDefinition.c_Nodes[u16_Node].u32_SubDeviceIndex;
            //Do we have one of the Flashloader types that support version checking ?
            if ((pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderStwCan == true) ||
                (((pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeCan == true) ||
                  (pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeEthernet == true)) &&
                 (pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeIsFileBased == false)))
            {
               // node has address based openSYDE Flashloader or STW Flashloader
               c_ActiveNodeTypes[u16_Node] = 1U;
            }
            else if (((pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeCan == true) ||
                      (pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeEthernet == true)) &&
                     (pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeIsFileBased == true))
            {
               c_ActiveNodeTypes[u16_Node] = 2U;
            }
            else
            {
               //nothing we know ...
            }
         }
      }
   }

   return c_ActiveNodeTypes;
}
