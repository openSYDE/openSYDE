//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class with functionality of console application system updater SYDEsup (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYDESUP_HPP
#define C_SYDESUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SupSuSequences.hpp"
#include "C_Can.hpp"
#include "C_OscIpDispatcher.hpp"
#include "C_OscSecurityPemDatabase.hpp"
#include "C_OscSuSequences.hpp"
#include "C_OscViewData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SydeSup
{
public:
   C_SydeSup(void);
   virtual ~C_SydeSup(void);

   enum E_OperationMode
   {
      eMODE_UPDATE,
      eMODE_CREATEPACKAGE
   };

   enum E_Result
   {
      // for detailed description see C_SydeSup::m_PrintStringFromError
      eERR_PARSE_COMMAND_LINE          = 10,

      eERR_PACKAGE_ERASE_TARG_PATH     = 20,
      eERR_PACKAGE_CORE_C_RANGE        = 21,
      eERR_PACKAGE_CORE_C_OVERFLOW     = 22,
      eERR_PACKAGE_UNZIP               = 23,
      eERR_PACKAGE_CORE_C_NOACT        = 24,
      eERR_PACKAGE_NOT_FOUND           = 25,
      eERR_PACKAGE_WRONG_EXTENSION     = 26,
      eERR_PACKAGE_FILES_MISSING       = 27,

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
      eERR_UPDATE_CERTIFICATE_PATH     = 68,
      eERR_UPDATE_AUTHENTICATION       = 69,

      eERR_THREAD_UPDATE_IN_PROGRESS   = 70,
      eERR_THREAD_UPDATE_INIT_FAILED   = 71,

      eERR_CREATE_PROJ_LOAD_FAILED     = 80,
      eERR_CREATE_VIEW_NOT_FOUND       = 81,
      eERR_CREATE_ZIP_RD_RW            = 82,
      eERR_CREATE_ZIP_CONFIG           = 83,

      eOK                              = 0,
      eERR_UNKNOWN                     = 100
   };

   E_Result ParseCommandLine(const int32_t os32_Argc, char_t * const * const oppcn_Argv);
   E_Result SelectModeAndStart(void);
   E_Result Update(void);
   E_Result CreatePackage(void);
   static void h_WriteLog(const stw::scl::C_SclString & orc_Activity, const stw::scl::C_SclString & orc_Text,
                          const bool & orq_IsError = false, const bool & orq_Quiet = false);

protected:
   class C_NodeApplicationsHelperStruct
   {
   public:
      std::vector<stw::opensyde_core::C_OscSuSequences::C_ApplicationProperties> c_ClientSideApplications;
      std::vector<stw::opensyde_core::C_OscSuSequences::C_ApplicationProperties> c_ServerSideApplications;
   };

   stw::can::C_Can * mpc_CanDispatcher;
   stw::opensyde_core::C_OscIpDispatcher * mpc_EthDispatcher;
   bool mq_Quiet;
   bool mq_OnlyNecessaryFiles;
   E_OperationMode me_OperationMode;
   stw::scl::C_SclString mc_OperationMode;
   stw::scl::C_SclString mc_SupFilePath;
   stw::scl::C_SclString mc_CanDriver;
   stw::scl::C_SclString mc_LogPath;
   stw::scl::C_SclString mc_LogFile;
   stw::scl::C_SclString mc_UnzipPath;
   stw::scl::C_SclString mc_CertFolderPath;
   stw::scl::C_SclString mc_OsyProjectPath;
   stw::scl::C_SclString mc_ViewName;
   stw::scl::C_SclString mc_DeviceDefPath;

   C_SydeSup::E_Result m_InitOptionalParameters(void);

private:
   // Update system with service update package
   virtual void m_CloseCan(void) = 0;
   virtual E_Result m_OpenCan(const stw::scl::C_SclString & orc_CanDriver, const uint64_t ou64_BitrateBps) = 0;
   virtual E_Result m_OpenEthernet(void) = 0;
   virtual stw::scl::C_SclString m_GetApplicationVersion(const stw::scl::C_SclString & orc_ApplicationFileName) const =
      0;
   virtual stw::scl::C_SclString m_GetDefaultLogLocation(void) const = 0;
   virtual stw::scl::C_SclString m_GetUnzipLocationDefaultExample(void) const = 0;
   virtual stw::scl::C_SclString m_GetCanInterfaceUsageExample(void) const = 0;

   void m_PrintVersion(const stw::scl::C_SclString & orc_Version, const stw::scl::C_SclString & orc_BinaryHash,
                       const bool oq_Detailed) const;
   void m_PrintInformation(const bool oq_Detailed) const;
   void m_InitLogging(void);
   stw::scl::C_SclString m_GetLogFileLocation(void) const;
   void m_PrintStringFromError(const C_SydeSup::E_Result & ore_Result) const;
   void m_Conclude(C_SupSuSequences & orc_Sequence, const bool & orq_ResetSystem);
   int32_t m_UpdateSystem(C_SupSuSequences & orc_Sequence,
                          const stw::opensyde_core::C_OscSystemDefinition & orc_SystemDefinition,
                          std::vector<uint32_t> & orc_NodesUpdateOrder, const std::vector<uint8_t> & orc_ActiveNodes,
                          std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite) const;
   std::vector<uint8_t> m_GetActiveNodeTypes(const stw::opensyde_core::C_OscSystemDefinition & orc_SystemDefinition,
                                             const std::vector<uint8_t> & orc_ActiveNodes) const;

   // Security PEM database
   stw::opensyde_core::C_OscSecurityPemDatabase mc_PemDatabase;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
