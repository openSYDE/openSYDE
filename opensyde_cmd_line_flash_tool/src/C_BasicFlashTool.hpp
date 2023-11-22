//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class of openSYDE simple command line flash tool
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_BASICFLASHTOOL_HPP
#define C_BASICFLASHTOOL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_Can.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_BasicFlashTool
{
public:
   enum E_Result
   {
      eRESULT_OK = 0,
      eRESULT_HELP_REQUESTED = 1,
      eERR_PARSE_COMMAND_LINE = 10,
      eERR_INITIALIZATION_FAILED = 20,
      eERR_ACTIVATE_FLASHLOADER = 30,
      eERR_READ_DEVICE_INFO = 40,
      eERR_UPDATE = 50,
      eERR_RESET = 60
  // description of error codes in about.txt
   };

   C_BasicFlashTool(void);
   virtual ~C_BasicFlashTool(void);

   void Init(void);
   E_Result ParseCommandLine(const int32_t os32_Argc, char_t * const * const oppcn_Argv);
   E_Result Flash(void);

private:
   uint8_t mu8_NodeId;
   int32_t ms32_CanBitrate;
   uint32_t mu32_FlashloaderResetWaitTime;
   uint32_t mu32_RequestDownloadTimeout;
   uint32_t mu32_TransferDataTimeout;
   stw::scl::C_SclString mc_HexFilePath;
   stw::scl::C_SclString mc_CanDriver;
   bool mq_ExitApplOnError;
   bool mq_StartAppl;

   stw::can::C_Can * mpc_CanDispatcher;

   static stw::scl::C_SclString mh_GetApplicationVersion(const stw::scl::C_SclString & orc_FileName);
   void m_PrintHelp(void);
   void m_Pause(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
