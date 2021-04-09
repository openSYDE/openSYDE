//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class with functionality of console application system updater SYDEsup (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYDESUP_H
#define C_SYDESUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_SUPSuSequences.h"
#include "CCAN.h"
#include "C_OSCIpDispatcher.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SYDEsup
{
public:
   C_SYDEsup(void);
   virtual ~C_SYDEsup(void);

   enum E_Result
   {
      // for detailed description see C_SYDEsup::m_PrintStringFromError
      eERR_PARSE_COMMAND_LINE          = 10,

      eERR_PACKAGE_ERASE_TARG_PATH     = 20,
      eERR_PACKAGE_CORE_C_RANGE        = 21,
      eERR_PACKAGE_CORE_C_OVERFLOW     = 22,
      eERR_PACKAGE_UNZIP               = 23,
      eERR_PACKAGE_CORE_C_NOACT        = 24,
      eERR_PACKAGE_NOT_FOUND           = 25,
      eERR_PACKAGE_WRONG_EXTENSION     = 26,

      eERR_CAN_IF_NOT_FOUND            = 30,
      eERR_CAN_IF_LOAD_FAILED          = 31,
      eERR_ETH_IF_LOAD_FAILED          = 32,

      eERR_SEQUENCE_ROUTING            = 40,
      eERR_SEQUENCE_UNKNOWN_CONFIG     = 41,
      eERR_SEQUENCE_CAN_IF_NOT_FOUND   = 42,
      eERR_SEQUENCE_INVALID_PARAM      = 43,
      eERR_SEQUENCE_CAN_INIT           = 44,
      eERR_SEQUENCE_C_CONFIG           = 45,
      eERR_SEQUENCE_BUFF_OVERFLOW      = 46,

      eERR_ACTIVATE_FLASHLOADER_FAILED = 51,
      eERR_ACTIVATE_FLASHLOADER_C_COM  = 52,
      eERR_ACTIVATE_FLASHLOADER_SYSDEF = 53,

      eERR_UPDATE_ABORTED              = 60,
      eERR_UPDATE_C_OVERFLOW           = 61,
      eERR_UPDATE_C_RD_RW              = 62,
      eERR_UPDATE_C_NOACT              = 63,
      eERR_UPDATE_C_COM                = 64,
      eERR_UPDATE_SYSDEF               = 65,
      eERR_UPDATE_CHECKSUM             = 66,
      eERR_UPDATE_NO_NVM               = 67,

      eERR_THREAD_UPDATE_IN_PROGRESS   = 70,
      eERR_THREAD_UPDATE_INIT_FAILED   = 71,

      eOK                              = 0,
      eERR_UNKNOWN                     = 100
   };

   E_Result ParseCommandLine(const stw_types::sintn osn_Argc, stw_types::charn * const * const oppcn_Argv);
   E_Result Update(void);
   static void h_WriteLog(const stw_scl::C_SCLString & orc_Activity, const stw_scl::C_SCLString & orc_Text,
                          const bool & orq_IsError = false, const bool & orq_Quiet = false);

protected:
   stw_can::C_CAN * mpc_CanDispatcher;
   stw_opensyde_core::C_OSCIpDispatcher * mpc_EthDispatcher;
   bool mq_Quiet;
   stw_scl::C_SCLString mc_SUPFilePath;
   stw_scl::C_SCLString mc_CanDriver;
   stw_scl::C_SCLString mc_LogPath;
   stw_scl::C_SCLString mc_LogFile;
   stw_scl::C_SCLString mc_UnzipPath;

   C_SYDEsup::E_Result m_InitOptionalParameters(void);

private:
   virtual void m_CloseCan(void) = 0;
   virtual E_Result m_OpenCan(const stw_scl::C_SCLString & orc_CanDriver, const stw_types::uint64 ou64_BitrateBps) = 0;
   virtual E_Result m_OpenEthernet(void) = 0;
   virtual stw_scl::C_SCLString m_GetApplicationVersion(const stw_scl::C_SCLString & orc_ApplicationFileName) const = 0;
   virtual stw_scl::C_SCLString m_GetDefaultLogLocation(void) const = 0;
   virtual stw_scl::C_SCLString m_GetCanInterfaceUsageExample(void) const = 0;

   void m_PrintVersion(const stw_scl::C_SCLString & orc_Version, const stw_scl::C_SCLString & orc_BinaryHash,
                       const bool oq_Detailed) const;
   void m_PrintInformation(const bool oq_Detailed) const;
   stw_scl::C_SCLString m_GetLogFileLocation(void) const;
   void m_PrintStringFromError(const C_SYDEsup::E_Result & ore_Result) const;
   void m_Conclude(C_SUPSuSequences & orc_Sequence, const bool & orq_ResetSystem);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
