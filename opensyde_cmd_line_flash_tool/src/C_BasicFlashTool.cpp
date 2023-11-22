//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class of openSYDE simple command line flash tool

   This contains the main functionality of the openSYDE command line flash tool.
   See about.txt and build.txt for detailed descriptions upon functionality and usage.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <getopt.h> //note: as we use getopt.h this application is not portable to all compilers
#include <conio.h>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglFile.hpp"
#include "TglTime.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscBinaryHash.hpp"
#include "C_BasicUpdateSequence.hpp"
#include "C_BasicFlashTool.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_BasicFlashTool::C_BasicFlashTool(void) :
   mu8_NodeId(0),
   ms32_CanBitrate(125),
   mu32_FlashloaderResetWaitTime(1000),
   mu32_RequestDownloadTimeout(40000),
   mu32_TransferDataTimeout(1000),
   mc_HexFilePath(""),
   mc_CanDriver(""),
   mq_ExitApplOnError(true),
   mq_StartAppl(true),
   mpc_CanDispatcher(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_BasicFlashTool::~C_BasicFlashTool()
{
   mpc_CanDispatcher = NULL;
   delete mpc_CanDispatcher;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization on tool startup

   Show some information on command line and setup logging
*/
//----------------------------------------------------------------------------------------------------------------------
void C_BasicFlashTool::Init(void)
{
   C_SclString c_LogFile;
   C_TglDateTime c_DateTime;

   std::cout << "This is a very simple openSYDE tool for updating one device with one hex file." << std::endl;
   std::cout << "Version: " << mh_GetApplicationVersion(TglGetExePath()).c_str() << std::endl;
   std::cout << "MD5-Checksum: " << C_OscBinaryHash::h_CreateBinaryHash().c_str() << std::endl;

   // setup logging
   TglGetDateTimeNow(c_DateTime);
   c_LogFile = C_OscLoggingHandler::h_UtilConvertDateTimeToString(c_DateTime);
   // Convert  2023-08-28 09:47:50.459 to  2023-08-28_09-47-50
   // i.e. replace " " with "_", replace ":" with "-" and cut decimals (number of characters is always the same)
   c_LogFile = c_LogFile.Insert("_", 11); // " "
   c_LogFile = c_LogFile.Delete(12, 1);
   c_LogFile = c_LogFile.Insert("-", 14); // first ":"
   c_LogFile = c_LogFile.Delete(15, 1);
   c_LogFile = c_LogFile.Insert("-", 17); //second ":"
   c_LogFile = c_LogFile.Delete(18, 1);
   c_LogFile = c_LogFile.Delete(20, 4); // remove ".123"
   c_LogFile = ".\\Logs\\" + c_LogFile + ".syde_log";

   C_OscLoggingHandler::h_SetCompleteLogFileLocation(c_LogFile);
   C_OscLoggingHandler::h_SetWriteToFileActive(true);
   C_OscLoggingHandler::h_SetWriteToConsoleActive(false);

   std::cout << "Logging to file: " << c_LogFile.c_str() << "\n" << std::endl;

   osc_write_log_info("Start", "Simple openSYDE command line tool for updating one device with one hex file started.");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse command line

   Parse command line arguments and write to internal data structure. Check that mandatory parameters got provided.
   Log read data resp. used data in default case to log file for easier troubleshooting.

   \param[in]  os32_Argc   Number of command line arguments
   \param[in]  oppcn_Argv  Command line arguments

   \return
   eRESULT_OK                 everything ok
   eRESULT_HELP_REQUESTED     command line option -h found -> only print help and do not continue with flashing
   eERR_PARSE_COMMAND_LINE    invalid or missing command line parameters
*/
//----------------------------------------------------------------------------------------------------------------------
C_BasicFlashTool::E_Result C_BasicFlashTool::ParseCommandLine(const int32_t os32_Argc,
                                                              char_t * const * const oppcn_Argv)
{
   E_Result e_Result = eRESULT_OK;
   int32_t s32_Result;
   bool q_ShowHelp = false;
   bool q_ParseError = false;

   const struct option ac_Options[] =
   {
      /* name, has_arg, flag, val */
      {
         "help",                       no_argument,         NULL,    'h'
      },
      {
         "nodeid",                     required_argument,   NULL,    'n'
      },
      {
         "hexfile",                    required_argument,   NULL,    'f'
      },
      {
         "caninterface",               required_argument,   NULL,    'i'
      },
      {
         "bitrate",                    required_argument,   NULL,    'b'
      },
      {
         "dontexitonerror",            no_argument,         NULL,    'e'
      },
      {
         "dontstartapplication",       no_argument,         NULL,    'a'
      },
      {
         "flashloaderresetwaittime",   no_argument,         NULL,    'w'
      },
      {
         "requestdownloadtimeout",     no_argument,         NULL,    'r'
      },
      {
         "transferdatatimeout",        no_argument,         NULL,    't'
      },
      {
         NULL,                         0,                   NULL,    0
      }
   };

   do
   {
      int32_t s32_Index;
      s32_Result = getopt_long(os32_Argc, oppcn_Argv, "hean:i:b:f:w:r:t:", &ac_Options[0], &s32_Index);
      if (s32_Result != -1)
      {
         switch (s32_Result)
         {
         case 'h':
            q_ShowHelp = true;
            break;
         case 'n':
            try
            {
               mu8_NodeId = static_cast<uint8_t>(static_cast<C_SclString>(optarg).ToInt64());
            }
            catch (...)
            {
               q_ParseError = true;
            }
            break;
         case 'f':
            mc_HexFilePath = optarg;
            break;
         case 'i':
            mc_CanDriver = optarg;
            break;
         case 'b':
            try
            {
               ms32_CanBitrate = static_cast<C_SclString>(optarg).ToInt();
            }
            catch (...)
            {
               q_ParseError = true;
            }
            break;
         case 'e':
            mq_ExitApplOnError = false;
            break;
         case 'a':
            mq_StartAppl = false;
            break;
         case 'w':
            try
            {
               mu32_FlashloaderResetWaitTime = static_cast<C_SclString>(optarg).ToInt();
            }
            catch (...)
            {
               q_ParseError = true;
            }
            break;
         case 'r':
            try
            {
               mu32_RequestDownloadTimeout = static_cast<C_SclString>(optarg).ToInt();
            }
            catch (...)
            {
               q_ParseError = true;
            }
            break;
         case 't':
            try
            {
               mu32_TransferDataTimeout = static_cast<C_SclString>(optarg).ToInt();
            }
            catch (...)
            {
               q_ParseError = true;
            }
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

   if (q_ShowHelp == true)
   {
      m_PrintHelp();
      e_Result = eRESULT_HELP_REQUESTED;
   }
   else if ((q_ParseError == true) || (mc_HexFilePath == "") || (mc_CanDriver == ""))
   {
      m_PrintHelp();
      std::cout << "Error: Invalid or missing command line parameters.\n\n";
      e_Result = eERR_PARSE_COMMAND_LINE;
   }
   else
   {
      const C_SclString c_LogActivity = "Parameter Value";
      const C_SclString c_ExitApplonError = (mq_ExitApplOnError == true) ? "yes" : "no";
      const C_SclString c_StartAppl = (mq_StartAppl == true) ? "yes" : "no";
      osc_write_log_info(c_LogActivity, "Node ID: " + C_SclString::IntToStr(mu8_NodeId));
      osc_write_log_info(c_LogActivity, "HEX file path: " + mc_HexFilePath);
      osc_write_log_info(c_LogActivity, "CAN driver: " + mc_CanDriver);
      osc_write_log_info(c_LogActivity, "Bitrate in kBit/s: " + C_SclString::IntToStr(ms32_CanBitrate));
      osc_write_log_info(c_LogActivity, "Flashloader reset wait time: " +
                         C_SclString::IntToStr(mu32_FlashloaderResetWaitTime));
      osc_write_log_info(c_LogActivity, "Request download timeout: " +
                         C_SclString::IntToStr(mu32_RequestDownloadTimeout));
      osc_write_log_info(c_LogActivity, "Transfer data timeout: " +
                         C_SclString::IntToStr(mu32_TransferDataTimeout));
      osc_write_log_info(c_LogActivity, "Exit application on error: " + c_ExitApplonError);
      osc_write_log_info(c_LogActivity, "Start application: " + c_StartAppl);
   }

   return e_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Flash node of given ID by using openSYDE's basic update sequence

   \return
   eERR_INITIALIZATION_FAILED    Error in call to C_OscBuSequences::Init
   eERR_ACTIVATE_FLASHLOADER     Error in call to C_OscBuSequences::ActivateFlashLoader
   eERR_READ_DEVICE_INFO         Error in call to C_OscBuSequences::ReadDeviceInformation
   eERR_UPDATE                   Error in call to C_OscBuSequences::UpdateNode
   eERR_RESET                    Error in call to C_OscBuSequences::ResetSystem
*/
//----------------------------------------------------------------------------------------------------------------------
C_BasicFlashTool::E_Result C_BasicFlashTool::Flash(void)
{
   E_Result e_Result = eRESULT_OK;
   C_BasicUpdateSequence c_TheSequence;
   int32_t s32_Return;

   s32_Return = c_TheSequence.Init(mc_CanDriver, ms32_CanBitrate, mu8_NodeId);
   if (s32_Return != C_NO_ERR)
   {
      e_Result = eERR_INITIALIZATION_FAILED;
   }

   if (e_Result == eRESULT_OK)
   {
      s32_Return = c_TheSequence.ActivateFlashLoader(mu32_FlashloaderResetWaitTime);
      if (s32_Return != C_NO_ERR)
      {
         e_Result = eERR_ACTIVATE_FLASHLOADER;
      }
   }

   if (e_Result == eRESULT_OK)
   {
      s32_Return = c_TheSequence.ReadDeviceInformation();
      if (s32_Return != C_NO_ERR)
      {
         e_Result = eERR_READ_DEVICE_INFO;
      }
   }

   if (e_Result == eRESULT_OK)
   {
      s32_Return = c_TheSequence.UpdateNode(mc_HexFilePath, mu32_RequestDownloadTimeout, mu32_TransferDataTimeout);
      if (s32_Return != C_NO_ERR)
      {
         e_Result = eERR_UPDATE;
      }
   }

   if ((e_Result == eRESULT_OK) && (mq_StartAppl == true))
   {
      s32_Return = c_TheSequence.ResetSystem();
      if (s32_Return != C_NO_ERR)
      {
         e_Result = eERR_RESET;
      }
   }

   // update log file to recent information
   C_OscLoggingHandler::h_Flush();

   if (e_Result == eRESULT_OK)
   {
      std::cout << "\nUpdate of node with ID " << C_SclString::IntToStr(mu8_NodeId).c_str() << " successful! " <<
         std::endl;
   }
   else
   {
      std::cout << "\nError: Update of node with ID " << C_SclString::IntToStr(mu8_NodeId).c_str() <<
         " failed! Result code: " << static_cast<int32_t>(e_Result) << ". See log file for details. " << std::endl;

      if (mq_ExitApplOnError == false)
      {
         m_Pause();
      }
   }
   return e_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get resource version number of a file as an C_SCLString

   Extracts the windows version number of the specified file and returns it
    in the commonly used STW format: "Vx.yyrz".
   This function is Windows specific and needs to be replaced by another solution
    when porting to a non-Windows system

   \param[in]  orc_FileName   file name to get version from

   \return
   string with version information ("V?.??r?" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_BasicFlashTool::mh_GetApplicationVersion(const stw::scl::C_SclString & orc_FileName)
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
            c_Version.PrintFormatted("V%d.%02dr%d", (pc_Info->dwFileVersionMS >> 16U),
                                     pc_Info->dwFileVersionMS & 0x0000FFFFUL,
                                     (pc_Info->dwFileVersionLS >> 16U));
         }
      }
      delete[] pu8_Buffer;
   }
   return c_Version;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Print help information to command line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_BasicFlashTool::m_PrintHelp(void)
{
   std::cout << "Command line parameters:\n\n";
   std::cout << "Parameter   Alternative notation        Description                            Default     Example\n";
   std::cout <<
      "============================================================================================================\n";
   std::cout << "-h          --help                      Show help                                          -h\n";
   std::cout << "-n          --nodeid                    Node ID                                <none>      -n 8\n";
   std::cout <<
      "-f          --hexfile                   Path to hex file                       <none>      -f D:\\myfile.hex\n";
   std::cout <<
      "-i          --caninterface              Path to CAN DLL                        <none>      -i D:\\stwpeak2\\stwpeak2.dll\n";
   std::cout << "-b          --bitrate                   Bitrate in kBit/s                      125         -b 1000\n";
   std::cout << "-e          --dontexitonerror           Don't exit application on error        do exit     -e\n";
   std::cout << "-a          --dontstartapplication      Don't start application after update   do start    -a\n";
   std::cout << "-w          --flashloaderresetwaittime  Flashloader reset wait time            1000        -w 2000\n";
   std::cout << "-r          --requestdownloadtimeout    Request download timeout               40000       -r 10000\n";
   std::cout << "-t          --transferdatatimeout       Transfer data timeout                  1000        -t 3000\n";
   std::cout << "\n";
   std::cout << "Parameters that have a \"Default\" are optional. All others are mandatory.\n";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Pause the tool and wait for user interaction to go on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_BasicFlashTool::m_Pause(void)
{
   std::cout << "Press any key to quit ..." << std::endl;
   bool q_Stay = true;
   while (q_Stay == true)
   {
      // wait a moment, we are very fast
      TglSleep(1);

      // check user input
      if (kbhit() > 0)
      {
         std::cout << "Breaking ...\n" << std::endl;
         q_Stay = false;
      }
   }
}
