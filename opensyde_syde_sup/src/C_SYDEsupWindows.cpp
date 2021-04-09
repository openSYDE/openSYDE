//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Windows class of SYDEsup (implementation)

   Implements the Windows-specific functions for SYDEsup.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLFile.h"
#include "C_SYDEsupWindows.h"

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
C_SYDEsupWindows::C_SYDEsupWindows(void) :
   C_SYDEsup(),
   mq_CanDllLoaded(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsupWindows::~C_SYDEsupWindows(void)
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
C_SYDEsup::E_Result C_SYDEsupWindows::m_OpenCan(const C_SCLString & orc_CanDriver, const uint64 ou64_BitrateBps)
{
   C_SYDEsup::E_Result e_Result = eOK;
   sint32 s32_Return = C_NO_ERR;

   if (mpc_CanDispatcher == NULL)
   {
      mpc_CanDispatcher = &mc_CanDispatcher;
   }

   // check if CAN DLL exists
   if (TGL_FileExists(orc_CanDriver) == false)
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
         s32_Return = mc_CanDispatcher.CAN_Init(static_cast<sint32>(ou64_BitrateBps / 1000UL));
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
void C_SYDEsupWindows::m_CloseCan(void)
{
   sint32 s32_Result;

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
C_SYDEsup::E_Result C_SYDEsupWindows::m_OpenEthernet(void)
{
   mpc_EthDispatcher = &mc_EthDispatcher;
   return eOK;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name of application as an C_SCLString

   Return the version number of the running application
    in the commonly used STW format: "Vx.yyrz".

   Here for Windows: read from file's ressources.

   \param[in]  orc_ApplicationFileName    file name of running application (can help to implement on some targets)

   \return
   string with version information ("V?.??r?" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_SYDEsupWindows::m_GetApplicationVersion(const C_SCLString & orc_ApplicationFileName) const
{
   VS_FIXEDFILEINFO * pt_Info;
   uintn un_ValSize;
   sint32 s32_InfoSize;
   uint8 * pu8_Buffer;
   C_SCLString c_Version;

   c_Version = "V?.\?\?r?";

   s32_InfoSize = GetFileVersionInfoSizeA(orc_ApplicationFileName.c_str(), NULL);
   if (s32_InfoSize != 0)
   {
      pu8_Buffer = new uint8[static_cast<uintn>(s32_InfoSize)];
      if (GetFileVersionInfoA(orc_ApplicationFileName.c_str(), 0, s32_InfoSize, pu8_Buffer) != FALSE)
      {
         //reinterpret_cast required due to function interface
         if (VerQueryValueA(pu8_Buffer, "\\",
                            reinterpret_cast<PVOID *>(&pt_Info), //lint !e929 !e9176
                            &un_ValSize) != FALSE)
         {
            c_Version.PrintFormatted("V%d.%02dr%d", (pt_Info->dwFileVersionMS >> 16U),
                                     pt_Info->dwFileVersionMS & 0x0000FFFFUL,
                                     (pt_Info->dwFileVersionLS >> 16U));
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
C_SCLString C_SYDEsupWindows::m_GetDefaultLogLocation(void) const
{
   return ".\\Logs";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CAN interface command line flag "-i" usage example

   \return
   usage example
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_SYDEsupWindows::m_GetCanInterfaceUsageExample(void) const
{
   return "-i .\\MyCan.dll";
}
