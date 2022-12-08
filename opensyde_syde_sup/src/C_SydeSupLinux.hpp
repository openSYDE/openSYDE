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

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_Can.hpp"
#include "C_OscIpDispatcherLinuxSock.hpp"
#include "C_SydeSup.hpp"

// Symbols declared with this attribute shall be visible as the API of the library
// No other symbols shall be visible to avoid namin conflicts with other libaries
// using same modules (e.g. SCL or TGL modules).
#define LIB_PUBLIC __attribute__ ((visibility("default")))

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class LIB_PUBLIC C_SydeSupLinux :
   public C_SydeSup
{
public:
   C_SydeSupLinux(void);
   virtual ~C_SydeSupLinux(void);

   E_Result InitParameters(const stw::scl::C_SclString & orc_SupFilePath, const stw::scl::C_SclString & orc_CanInterface,
                           const stw::scl::C_SclString & orc_LogPath, const stw::scl::C_SclString & orc_UnzipPath);
   void SetUpdateFilePath(const stw::scl::C_SclString & orc_SupFilePath);
   E_Result UpdateTaskStart(void);
   E_Result UpdateTaskCheckResult(uint8_t & oru8_Progress);
   E_Result GetNextInfoText(stw::scl::C_SclString & orc_Info);

private:
   pthread_t mx_UpdateTaskHandle; //lint !e8080 //interfacing with library
   bool mq_UpdateTaskRunning = false;
   E_Result me_UpdateTaskResult;
   uint8_t mu8_UpdateProgress = 0U;
   stw::scl::C_SclDynamicArray<stw::scl::C_SclString> mac_UpdateInfo;
   uint32_t mu32_InfoIndex = 0U;

   static void * mh_UpdateTask(void * const opv_Arg);

   virtual void m_CloseCan(void);
   virtual E_Result m_OpenCan(const stw::scl::C_SclString & orc_CanDriver, const uint64_t ou64_BitrateBps);
   virtual E_Result m_OpenEthernet(void);
   virtual stw::scl::C_SclString m_GetApplicationVersion(const stw::scl::C_SclString & orc_ApplicationFileName) const;
   virtual stw::scl::C_SclString m_GetDefaultLogLocation(void) const;
   virtual stw::scl::C_SclString m_GetCanInterfaceUsageExample(void) const;

   bool mq_CanDllLoaded;
   stw::can::C_Can mc_CanDispatcher;
   stw::opensyde_core::C_OscIpDispatcherLinuxSock mc_EthDispatcher;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
