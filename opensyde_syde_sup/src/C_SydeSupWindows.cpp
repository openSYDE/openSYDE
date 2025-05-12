//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Windows class of SYDEsup (implementation)

   Implements the Windows-specific functions for SYDEsup.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "TglFile.hpp"
#include "C_SydeSupWindows.hpp"

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
C_SydeSupWindows::C_SydeSupWindows(void) :
   C_SydeSup(),
   mq_CanDllLoaded(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSupWindows::~C_SydeSupWindows(void)
{
}

//-------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize CAN driver.

   The implementation shall
   * initialize a specific implementation of C_CANDispatcher
   * do whatever is required to set that dispatcher up (connect to CAN bus)
   * set mpc_CanDispatcher to point to that dispatcher

   \param[in]  orc_CanDriver     path to CAN driver as passed on command line
   \param[in]  ou64_BitrateBps   bitrate to initialize CAN bus with

   \return  error code
   eOK                        driver initialized
   eERR_CAN_IF_NOT_FOUND      driver not found
   eERR_CAN_IF_LOAD_FAILED    driver load failed (details in log)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSup::E_Result C_SydeSupWindows::m_OpenCan(const C_SclString & orc_CanDriver, const uint64_t ou64_BitrateBps)
{
   C_SydeSup::E_Result e_Result = eOK;
   int32_t s32_Return = C_NO_ERR;

   if (mpc_CanDispatcher == NULL)
   {
      mpc_CanDispatcher = &mc_CanDispatcher;
   }

   // check if CAN DLL exists
   if (TglFileExists(orc_CanDriver) == false)
   {
      e_Result = eERR_CAN_IF_NOT_FOUND;
   }
   else
   {
      h_WriteLog("Setup CAN", "CAN driver \"" + orc_CanDriver + "\" found.");
      // setup/load CAN dispatcher
      s32_Return = mc_CanDispatcher.DLL_Open(orc_CanDriver);
   }

   // initialize CAN driver if previous step was successful and return errors else
   if (e_Result == eOK)
   {
      switch (s32_Return) // here s32_Return is result of c_CanDispatcher.DLL_Open
      {
      case C_NO_ERR:
         h_WriteLog("Setup CAN", "CAN driver loaded.");
         mq_CanDllLoaded = true;
         // initialize CAN dispatcher
         s32_Return = mc_CanDispatcher.CAN_Init(static_cast<int32_t>(ou64_BitrateBps / 1000UL));
         break;
      case CAN_COMP_ERR_DLL_ALREADY_OPENED:
         h_WriteLog("Setup CAN", "CAN opening failed because DLL is already opened.", true);
         e_Result = eERR_CAN_IF_LOAD_FAILED;
         break;
      case CAN_COMP_ERR_DLL_INIT:
         h_WriteLog("Setup CAN", "CAN opening failed because DLL initialization failed.", true);
         e_Result = eERR_CAN_IF_LOAD_FAILED;
         break;
      case CAN_COMP_ERR_DLL_FORMAT:
         h_WriteLog("Setup CAN", "CAN opening failed because DLL is not in correct format.", true);
         e_Result = eERR_CAN_IF_LOAD_FAILED;
         break;
      default:
         h_WriteLog("Setup CAN", "CAN opening failed because of unknown reason.", true);
         e_Result = eERR_CAN_IF_LOAD_FAILED;
         break;
      }
   }

   if (e_Result == eOK)
   {
      switch (s32_Return) // here s32_Return is result of c_CanDispatcher.CAN_Init
      {
      case C_NO_ERR:
         h_WriteLog("Setup CAN", "CAN interface initialized.");
         break;
      case C_UNKNOWN_ERR:
         h_WriteLog("Setup CAN", "CAN initialization failed because DLL returned internal error.", true);
         e_Result = eERR_CAN_IF_LOAD_FAILED;
         break;
      case C_CONFIG:
         h_WriteLog("Setup CAN",
                    "CAN initialization failed because channel is not supported by loaded DLL.", true);
         e_Result = eERR_CAN_IF_LOAD_FAILED;
         break;
      case CAN_COMP_ERR_DLL_NOT_OPENED:
         h_WriteLog("Setup CAN", "CAN initialization failed because DLL was not yet opened.", true);
         e_Result = eERR_CAN_IF_LOAD_FAILED;
         break;
      default:
         h_WriteLog("Setup CAN", "CAN initialization failed because of unknown reason.", true);
         e_Result = eERR_CAN_IF_LOAD_FAILED;
         break;
      }
   }

   return e_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Close CAN interface

   Disconnect from CAN bus and CAN driver
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SydeSupWindows::m_CloseCan(void)
{
   int32_t s32_Result;

   (void)mc_CanDispatcher.CAN_Exit();

   if (mq_CanDllLoaded == true)
   {
      s32_Result = mc_CanDispatcher.DLL_Close();
      if (s32_Result == C_NO_ERR)
      {
         h_WriteLog("Close CAN driver", "CAN driver successfully closed.", false, mq_Quiet);
      }
      else
      {
         h_WriteLog("Close CAN driver", "Could not close CAN driver.", true, mq_Quiet);
      }
      mq_CanDllLoaded = false;
   }

   mpc_CanDispatcher = NULL; //not valid any more
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize Ethernet driver.

   The implementation shall
   * initialize a specific implementation of C_OSCIpDispatcher
   * do whatever is required to set that dispatcher up (nothing in most cases)
   * set mpc_EthDispatcher to point to that dispatcher

   \return  error code
   eOK                        driver initialized
   eERR_ETH_IF_LOAD_FAILED    driver load failed
*/
//----------------------------------------------------------------------------------------------------------------------
C_SydeSup::E_Result C_SydeSupWindows::m_OpenEthernet(void)
{
   mpc_EthDispatcher = &mc_EthDispatcher;
   return eOK;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name of application as an C_SclString

   Return the version number of the running application
    in the commonly used STW format: "Vx.yyrz".

   Here for Windows: read from file's ressources.

   \param[in]  orc_ApplicationFileName    file name of running application (can help to implement on some targets)

   \return
   string with version information ("V?.??r?" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SydeSupWindows::m_GetApplicationVersion(const C_SclString & orc_ApplicationFileName) const
{
   VS_FIXEDFILEINFO * pc_Info;
   uint32_t u32_ValSize;
   int32_t s32_InfoSize;
   uint8_t * pu8_Buffer;
   C_SclString c_Version;

   c_Version = "V?.\?\?r?";

   s32_InfoSize = GetFileVersionInfoSizeA(orc_ApplicationFileName.c_str(), NULL);
   if (s32_InfoSize != 0)
   {
      pu8_Buffer = new uint8_t[static_cast<uint32_t>(s32_InfoSize)];
      if (GetFileVersionInfoA(orc_ApplicationFileName.c_str(), 0, s32_InfoSize, pu8_Buffer) != FALSE)
      {
         //reinterpret_cast required due to function interface
         if (VerQueryValueA(pu8_Buffer, "\\",
                            reinterpret_cast<PVOID *>(&pc_Info), //lint !e929 !e9176
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
/*! \brief  Get default log location

   \return
   Default log location
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SydeSupWindows::m_GetDefaultLogLocation(void) const
{
   return ".\\Logs";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get default example for unzip location (user's temporary folder of operating system)

   \return
   Example unzip location
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SydeSupWindows::m_GetUnzipLocationDefaultExample(void) const
{
   return "%TEMP%";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CAN interface command line flag "-i" usage example

   \return
   usage example
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SydeSupWindows::m_GetCanInterfaceUsageExample(void) const
{
   return "-i .\\MyCan.dll";
}
