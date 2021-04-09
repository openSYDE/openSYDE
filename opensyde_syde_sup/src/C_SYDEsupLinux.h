//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Linux class of SYDEsup (header)

   See cpp file for detailed description

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYDESUPLINUX_H
#define C_SYDESUPLINUX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <pthread.h>

#include "stwtypes.h"
#include "CSCLString.h"
#include "CCAN.h"
#include "C_OSCIpDispatcherLinuxSock.h"
#include "C_SYDEsup.h"

// Symbols declared with this attribute shall be visible as the API of the library
// No other symbols shall be visible to avoid namin conflicts with other libaries
// using same modules (e.g. SCL or TGL modules).
#define LIB_PUBLIC __attribute__ ((visibility("default")))

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class LIB_PUBLIC C_SYDEsupLinux :
   public C_SYDEsup
{
public:
   C_SYDEsupLinux(void);
   virtual ~C_SYDEsupLinux(void);

   E_Result InitParameters(const stw_scl::C_SCLString & orc_SupFilePath, const stw_scl::C_SCLString & orc_CanInterface,
                           const stw_scl::C_SCLString & orc_LogPath, const stw_scl::C_SCLString & mc_UnzipPath);
   void SetUpdateFilePath(const stw_scl::C_SCLString & orc_SupFilePath);
   E_Result UpdateTaskStart(void);
   E_Result UpdateTaskCheckResult(stw_types::uint8 & oru8_Progress);
   E_Result GetNextInfoText(stw_scl::C_SCLString & orc_Info);

private:
   pthread_t mt_UpdateTaskHandle;
   bool mq_UpdateTaskRunning = false;
   E_Result me_UpdateTaskResult;
   stw_types::uint8 mu8_UpdateProgress = 0u;
   stw_scl::SCLDynamicArray<stw_scl::C_SCLString> mac_UpdateInfo;
   stw_types::uintn mun_InfoIndex = 0u;

   static void * mh_UpdateTask(void * opv_Arg);

   virtual void m_CloseCan(void);
   virtual E_Result m_OpenCan(const stw_scl::C_SCLString & orc_CanDriver, const stw_types::uint64 ou64_BitrateBps);
   virtual E_Result m_OpenEthernet(void);
   virtual stw_scl::C_SCLString m_GetApplicationVersion(const stw_scl::C_SCLString & orc_ApplicationFileName) const;
   virtual stw_scl::C_SCLString m_GetDefaultLogLocation(void) const;
   virtual stw_scl::C_SCLString m_GetCanInterfaceUsageExample(void) const;

   bool mq_CanDllLoaded;
   stw_can::C_CAN mc_CanDispatcher;
   stw_opensyde_core::C_OSCIpDispatcherLinuxSock mc_EthDispatcher;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
