//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_XFLFlashWriteParameters

   Parameter class for C_XFLFlashWrite::Execute.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CXFLFLASHWRITEPARAMETERSH
#define CXFLFLASHWRITEPARAMETERSH

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclIniFile.hpp"
#include "DiagLib_config.hpp"
#include "CXFLActions.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
{
//1.02r0 -> replaced all remaining function pointers; these are now overloadable functions in C_XFLDownload
//       -> CAN access to be set through C_XFLProtocol::CfgSetCommDispatcher
//       -> aggregated all parameters required for wakeup into a sub-structure
//       -> renamed "e_TypeOfHexFile" to "e_EraseMode" (also renamed enum values correspondingly)
//1.01r2 -> PR_XFLFinishedAskUserFunction replaces PR_XFLAskYesNoQuestion and PR_XFLFinishedAskUserFunction
//1.01r1 -> company ID is now a C_SclString (was: array of uint8)
//1.01r0 -> cleaned up interface; added some callbacks to resolve VCL dependencies
//1.00r1 -> removed q_TryWakeupTwice: never publically documented; not useful; workaround for comm interface issues
//           should be done in higher layer
//          Replaced all AnsiStrings by C_SclString in an effort to become more platform independent
//          Added status callback and instance pointer to structure (instead of ::Execute parameters)
//1.00r0 -> callbacks changed to regular C++ pointer along with instance pointers (not closures anymore)
//          also changed all prefixes to match STW C-Rules 3.x (m_ExecuteStatusCallbackas API was changed anyway ...)
//          even changed name of parameter structure ...
//0.01r1 -> abCompanyID now consists of up to 5 bytes
//0.01r0 -> added e_DevTypeCheck, e_DevTypeCheckGetIDFailedReaction, e_DevTypeCheckMatchIDFailedReaction
//0.00r4 -> added q_IgnoreInvalidHexfileError
//0.00r3 -> added q_XFLExchange
static const uint16_t CXFLFLASHWRITE_VERSION = 0x1020U;

enum E_XFLEraseMode ///< determines which flash sectors shall be erased before flashing
{
   eXFL_ERASE_MODE_C_APPLICATION = 0,   ///< erase all sectors except flashloader area
   eXFL_ERASE_MODE_CANOPEN_CONFIG,      ///< erase only CANopen configuration data area
   eXFL_ERASE_MODE_IEC_APPLICATION,     ///< erase only CoDeSys application area
   eXFL_ERASE_MODE_IEC_RUN_TIME_SYSTEM, ///< erase only CoDeSys run-time-system area
   eXFL_ERASE_MODE_USER_DEFINED,        ///< sectors to be erased can be set manually
   eXFL_ERASE_MODE_AUTOMATIC            ///< automatically detect which sectors to erase (only from protocol version
                                        // V3.00r0 on)
};

enum E_XFLFlashFinishedAction ///< which action shall be taken after flashing has finished ?
{
   eXFL_FLASH_FINISHED_ACTION_ASK_USER = 0, ///< ask user what to do
   eXFL_FLASH_FINISHED_ACTION_NODE_RETURN,  ///< start application
   eXFL_FLASH_FINISHED_ACTION_NODE_RESET,   ///< reset node
   eXFL_FLASH_FINISHED_ACTION_NODE_SLEEP,   ///< go back to sleep mode
   eXFL_FLASH_FINISHED_ACTION_NET_START,    ///< start all nodes
   eXFL_FLASH_FINISHED_ACTION_NET_RESET,    ///< reset all nodes
   eXFL_FLASH_FINISHED_ACTION_NONE          ///< no action
};

/// define whether the device type in the hex-file shall be
///  X-checked with the device type on the ECU before commencing download
/// cf. flashloader-spec chapter "get_device_info_address" for details
enum E_XFLDevTypeCheck
{
   eXFL_DEV_TYPE_CHECK_NONE          = 0,         ///< no X-check
   eXFL_DEV_TYPE_CHECK_SCAN_HEX_FILE = 1,         ///< scan hexfile for T_XFLECUInformation block
   eXFL_DEV_TYPE_CHECK_GET_ADDRESS_THEN_SCAN = 2, ///< read the expected address from ECU
   ///<  if there's no data at this address in the hex-file
   ///<   (or: the service is not available on the ECU)
   ///<   (or: the service returns an error)
   ///<  then scan it
   eXFL_DEV_TYPE_CHECK_GET_ADDRESS_THEN_FAIL = 3 ///< read the expected address from ECU
                                                 ///<  if there's no data at this address in the hex-file
                                                 ///<   (or: the service is not available on the ECU)
                                                 ///<   (or: the service returns an error)
                                                 ///<  then fail
};

/// define how to react to failed X-check (could not get device ID from hex-file; or could not get address from server)
enum E_XFLDevTypeCheckGetIDFailReaction
{
   eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_IGNORE = 0, ///< ignore (but protocol as text) -> no x-check
   eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_FAIL   = 1, ///< fail: do not start downloading
   eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_ASK    = 2  ///< ask user whether to continue  -> no x-check if he/she
                                                       // selects "yes"
};

/// define how to react to failed X-check (device ID does not match  )
enum E_XFLDevTypeCheckMatchIDFailReaction
{
   eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_IGNORE = 0, ///< ignore (but protocol as text)
   eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_FAIL   = 1, ///< fail: do not start downloading
   eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_ASK    = 2  ///< ask user whether to continue  -> ok if he/she selects
                                                         // "yes"
};

//----------------------------------------------------------------------------------------------------------------------
///Parameters for perfoming flash write procedure
class C_XFLFlashWriteParameters
{
public:
   C_XFLFlashWriteParameters(void);
   virtual ~C_XFLFlashWriteParameters(void);

   uint16_t u16_Version;            ///< used to check consistency between header-file and library, use
                                    // CXFLWRITEFLASH_VERSION
   stw::scl::C_SclString c_HexFile; ///< full path the hex file

   C_XFLWakeupParameters c_WakeupConfig; ///< parameters defining how to address the target node

   E_XFLEraseMode e_EraseMode;                 ///< determines the erase mode
   stw::scl::C_SclString c_UserDefinedSectors; ///< comma separated (e.g. "10,11,4"), only used if e_EraseMode =
   ///<  XFLFileTypeUserDefined

   E_XFLFlashFinishedAction e_FlashFinishedAction; ///< what to do after flashing has finished ?

   uint16_t u16_InterFrameDelayUs; ///< delay in us between individual frames when sending hex-lines.
   ///< helpful for low-performance targets at high bitrates.
   ///< set to 0 for most communication interfaces and target nodes

   uint8_t u8_HexRecordLength; ///< patch hexfile records to different length; 0 = don't modify

   bool q_XtdID;           ///< false: use 11bit CAN message IDs; true: use 29bit CAN message IDs
   uint32_t u32_SendID;    ///< CAN message TX ID used in flashloader protocol
   uint32_t u32_ReceiveID; ///< CAN message RX ID used in flashloader protocol (usually TX-ID + 1).
   ///< 0xffffffff -> accept response on any ID (e.g. for RS232 flashing)

   ///< ESX2 Babyboard B compatibility entries (not required for any other targets (set "q_Divertstream = false;")
   bool q_DivertStream;          ///< true: target ESX2 babyboard B; false -> target mainboard
   uint8_t u8_DivertTargetIndex; ///< slot of babyboard B
   uint8_t au8_DivertUserID[2];  ///< key to be able to write to ESX2 babyboard B

   bool q_WriteCRCsIfSupported; ///< true -> write flash sector checksums; false -> don't

   bool q_VerboseMode; ///< true -> report some more details about progress ...

   bool q_XFLExchange;                   ///< false -> regular download; true -> send XFL_EXCHANGE request
   uint8_t u8_IgnoreInvalidHexfileError; ///< behaviour on erroneous hexfile  \n
   ///< 0 -> always fail  \n
   ///< 1 -> always ignore \n
   ///< 2 -> fail; on overlap ask user

   E_XFLDevTypeCheck e_DevTypeCheck;                                         ///< cf. comments at enum definition
   E_XFLDevTypeCheckGetIDFailReaction e_DevTypeCheckGetIDFailedReaction;     ///< cf. comments at enum definition
   E_XFLDevTypeCheckMatchIDFailReaction e_DevTypeCheckMatchIDFailedReaction; ///< cf. comments at enum definition

   void LoadFromIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);
   int32_t SaveToIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section) const;
};

//----------------------------------------------------------------------------------------------------------------------
}
}

//----------------------------------------------------------------------------------------------------------------------
#endif
