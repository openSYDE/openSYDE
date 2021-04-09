//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Linux class of SYDEsup (implementation)

   Implements the Linux-specific functions for SYDEsup.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"
#include "version_config.h"

#include "stwerrors.h"
#include "C_SYDEsupLinux.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

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
C_SYDEsupLinux::C_SYDEsupLinux(void) :
   C_SYDEsup(),
   mq_CanDllLoaded(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsupLinux::~C_SYDEsupLinux(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \brief   Setup parameters for the SYSDEsup client.

   This function may be used by the SYSDEsup library, instead of ParseCommandLine().

   \param[in]   orc_SupFilePath     Path and filename of the update package
   \param[in]   orc_CanInterface    CAN interface name
   \param[in]   orc_LogPath         Path for log file output
   \param[in]   orc_UnzipPath       Path to unpack the update package to

   \retval eOK                       Success
   \retval eERR_PARSE_COMMAND_LINE   Update package file not specified or unzip folder invalid
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsupLinux::InitParameters(const stw_scl::C_SCLString & orc_SupFilePath,
                                                   const stw_scl::C_SCLString & orc_CanInterface,
                                                   const stw_scl::C_SCLString & orc_LogPath,
                                                   const stw_scl::C_SCLString & orc_UnzipPath)
{
   E_Result e_Return;

   mc_SUPFilePath = orc_SupFilePath;
   mc_CanDriver = orc_CanInterface;
   mc_LogPath = orc_LogPath;
   mc_UnzipPath = orc_UnzipPath;

   if (mc_SUPFilePath == "")
   {
      e_Return = eERR_PARSE_COMMAND_LINE;
   }
   else
   {
      // Initialize optional parameters
      e_Return = this->m_InitOptionalParameters();
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \brief   Set the update file path.

   This function changes the update package path and filename. It may be used after a InitParameters() call, but
   does not replace it.

   \param[in]   orc_SupFilePath     Path and filename of the update package
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SYDEsupLinux::SetUpdateFilePath(const stw_scl::C_SCLString & orc_SupFilePath)
{
   mc_SUPFilePath = orc_SupFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \brief   Start a update process.

   This function creates a thread in which the update process is executed and starts the update process
   (call C_SYDEsup::Update()).
   Use UpdateTaskCheckResult() to check for progress.

   \retval eOK                              Success
   \retval eERR_THREAD_UPDATE_IN_PROGRESS   Another update process already in progress
   \retval eERR_THREAD_UPDATE_INIT_FAILED   Could not create new thread (no resources)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsupLinux::UpdateTaskStart(void)
{
   E_Result e_Result = eERR_THREAD_UPDATE_IN_PROGRESS;
   sintn sn_Ret;

   if (mq_UpdateTaskRunning == false)
   {
      // Preset update status variable before starting the update thread to make sure UpdateTaskCheckResult()
      // returns "in progress" while the update thread is running.
      me_UpdateTaskResult = eERR_THREAD_UPDATE_IN_PROGRESS;

      // Prepare info text variable
      mun_InfoIndex = 0u;
      mac_UpdateInfo.SetLength(0u);

      sn_Ret = pthread_create(&mt_UpdateTaskHandle, NULL, &mh_UpdateTask, this);
      if (sn_Ret == 0)
      {
         e_Result = eOK;
         mq_UpdateTaskRunning = true;
      }
      else
      {
         e_Result = eERR_THREAD_UPDATE_INIT_FAILED;
      }
   }

   return e_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \brief   Check the status of the update process.

   This function returns the status and progress of a update process. It may be called cyclically until the
   update process has been finished.

   \param[out]  oru8_Progress       Update proccess progress in 0..100%

   \retval eERR_THREAD_UPDATE_IN_PROGRESS  Update process still in progress
   \retval eOK                      Update process finished successfully
   \retval other                    Update process finished with error, see C_SYDEsup::Update()
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsupLinux::UpdateTaskCheckResult(stw_types::uint8 & oru8_Progress)
{
   E_Result e_Result = eERR_UPDATE_C_NOACT;

   if (mq_UpdateTaskRunning == true)
   {
      if (me_UpdateTaskResult == eERR_THREAD_UPDATE_IN_PROGRESS)
      {
         e_Result = eERR_THREAD_UPDATE_IN_PROGRESS;
      }
      else
      {
         // Update task has finished
         (void)pthread_join(mt_UpdateTaskHandle, NULL);
         mq_UpdateTaskRunning = false;
         e_Result = me_UpdateTaskResult;
      }
      oru8_Progress = mu8_UpdateProgress;
   }
   else
   {
      oru8_Progress = 0u;
   }

   return e_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \brief   Return the information text of an update process line by line.

   \param[out]  orc_Info   Information text line

   \retval eERR_UPDATE_C_NOACT      No more lines to read
   \retval eOK                      One line read
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsupLinux::GetNextInfoText(stw_scl::C_SCLString & orc_Info)
{
   E_Result e_Result = eERR_UPDATE_C_NOACT;
   uintn un_InfoLen = mac_UpdateInfo.GetLength();

   if (mun_InfoIndex < un_InfoLen)
   {
      orc_Info = mac_UpdateInfo[mun_InfoIndex];
      mun_InfoIndex++;
      e_Result = eOK;
   }

   return e_Result;
}
//----------------------------------------------------------------------------------------------------------------------
/*!
   \brief   Pthread wrapper for C_SYDEsup::Update() function.

   This function is installed as the update process thread by UpdateTaskStart().
   The result of the update process is written to variable me_UpdateTaskResult.
*/
//----------------------------------------------------------------------------------------------------------------------
void * C_SYDEsupLinux::mh_UpdateTask(void * opv_Arg)
{
   C_SYDEsupLinux * pt_This = (C_SYDEsupLinux *)opv_Arg;

   pt_This->me_UpdateTaskResult = pt_This->Update();

   return NULL;
}

//-------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize CAN driver.

   The implementation shall
   * initialize a specific implementation of C_CANDispatcher
   * do whatever is required to set that dispatcher up (connect to CAN bus)
   * set mpc_CanDispatcher to point to that dispatcher

   \param[in]  orc_CanDriverPath    path to CAN driver as passed on command line
   \param[in]  ou64_BitrateBps      bitrate to initialize CAN bus with

   \return  error code
   eOK                        driver initialized
   eERR_CAN_IF_LOAD_FAILED    driver already open
*/
//----------------------------------------------------------------------------------------------------------------------
C_SYDEsup::E_Result C_SYDEsupLinux::m_OpenCan(const C_SCLString & orc_CanDriver, const uint64 ou64_BitrateBps)
{
   C_SYDEsup::E_Result e_Result = eOK;
   sint32 s32_Return = C_NO_ERR;

   (void)ou64_BitrateBps; // Bitrate is set by interface settings outside

   if (mpc_CanDispatcher == NULL)
   {
      mpc_CanDispatcher = &mc_CanDispatcher;
   }

   s32_Return = mc_CanDispatcher.CAN_Init(orc_CanDriver, 1);

   if (s32_Return != C_NO_ERR)
   {
      e_Result = eERR_SEQUENCE_CAN_INIT;
   }

   return e_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Close CAN interface

   Disconnect from CAN bus and CAN driver
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SYDEsupLinux::m_CloseCan(void)
{
   (void)mc_CanDispatcher.CAN_Exit();

   if (mq_CanDllLoaded == true)
   {
      (void)mc_CanDispatcher;
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
C_SYDEsup::E_Result C_SYDEsupLinux::m_OpenEthernet(void)
{
   mpc_EthDispatcher = &mc_EthDispatcher;
   return eOK;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name of application as an C_SCLString

   Return the version number of the running application
    in the commonly used STW format: "Vx.yyrz".

   Here for Linux: use configuration in version_config.h

   \param[in]  orc_ApplicationFileName    file name of running application (can help to implement on some targets)

   \return
   string with version information
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_SYDEsupLinux::m_GetApplicationVersion(const C_SCLString & orc_ApplicationFileName) const
{
   C_SCLString c_Version;

   (void)orc_ApplicationFileName;

   c_Version.PrintFormatted("V%d.%02dr%d",
                            PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR,
                            PROJECT_VERSION_RELEASE, PROJECT_VERSION_BUILD);

   return c_Version;
}

/*! \brief  Get default log location

   \return
   Default log location
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_SYDEsupLinux::m_GetDefaultLogLocation(void) const
{
   return "/var/log";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CAN interface command line flag "-i" usage example

   \return
   usage example
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_SYDEsupLinux::m_GetCanInterfaceUsageExample(void) const
{
   return "-i can0";
}

