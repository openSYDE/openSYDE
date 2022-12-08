//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_XFLProtocol

   Encapsulate STW flashloader services.

   \copyright   Copyright 1999 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CXFLPROTOCOLH
#define CXFLPROTOCOLH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_CanDispatcher.hpp"
#include "C_HexFile.hpp"
#include "C_SclDynamicArray.hpp"
#include "C_SclString.hpp"
#include "C_SclStringList.hpp"
#include "DiagLib_config.hpp"
#include "stw_can.hpp" //for message structures
#include "CDLReportEvents.hpp"
#include "CXFLECUInformation.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace diag_lib
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

const uint8_t XFL_DIVERT_TARGET_BABY_B = 0U;

//error resturns common to flashesx.cpp
//FlashESX.CPP Error Codes
const int32_t XFL_ERR_DEVICE_SPECIFIC =  -20; ///< error value can be retrieved with GetLastXFLError

//error codes.
const uint8_t XFL_ERR_UNDEFINED_ERROR       = 0x00U;
const uint8_t XFL_ERR_APP_DEF_ERR_MIN       = 0x10U;
const uint8_t XFL_ERR_APP_DEF_ERR_MAX       = 0x3FU;
const uint8_t XFL_ERR_CHECKSUM_ERR          = 0xD6U;
const uint8_t XFL_ERR_FLASH_PROG_ERR        = 0xE1U;
const uint8_t XFL_ERR_FLASH_ERASE_ERR       = 0xE2U;
const uint8_t XFL_ERR_CAN_BITTIME_ERR       = 0xE3U;
const uint8_t XFL_ERR_OUT_OF_RANGE_ERR      = 0xF7U;
const uint8_t XFL_ERR_EE_VERIFY_ERR         = 0xFBU;
const uint8_t XFL_ERR_EE_READ_ERR           = 0xFCU;
const uint8_t XFL_ERR_EE_WRITE_ERR          = 0xFDU;
const uint8_t XFL_ERR_DIVERT_STREAM_COM_ERR = 0xFEU;
const uint8_t XFL_ERR_WRONG_REC_TYPE        = 0xFFU;

//non-configured checksum block
const uint32_t XFL_CHK_BLOCK_END_INVALID = 0x00000000UL;

//number of different local IDs
const uint16_t XFL_NUM_DIFFERENT_LOCAL_IDS = 256U; ///< 0..255 (255 is reserved, but we never know)

//default protocol parameters:
const uint8_t XFL_DEFAULT_SEND_ID    = 0x51U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Information about in protected flash sector
class C_XFLProtectedSectorInfo
{
public:
   uint8_t u8_ICIndex;
   uint16_t u16_SectorNumber;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about aliased memory
class C_XFLAliasedRange
{
public:
   uint32_t u32_PhysicalAddress;
   uint32_t u32_Size;
   uint32_t u32_AliasedAddress;

   int64_t GetOffset(void) const;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about multiple aliased memory ranges
class C_XFLAliasedRanges :
   public stw::scl::C_SclDynamicArray<C_XFLAliasedRange>
{
public:
   int32_t GetRangeOccupiedByAddress(const uint32_t ou32_Address, uint8_t & oru8_Range)
   const;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about checksum block
class C_XFLChecksumBlock
{
public:
   uint32_t u32_StartAddress;   //start address of block
   uint32_t u32_EndAddress;     //end address of block (= highest occupied address)
   bool q_BlockDefinitionValid; //true -> valid information contained in start and end address
};

//----------------------------------------------------------------------------------------------------------------------
///Raw company ID data
class C_XFLCompanyID
{
public:
   uint8_t au8_Data[5];
   uint8_t u8_NumBytes; ///< number of actually used bytes (only 2 or 5 are valid)
};

//----------------------------------------------------------------------------------------------------------------------
///Information about flashloader services available on the server
//use a big struct and not a bitfield:
//- memory use is not a big issue on PC
//- we must not rely on bit positions anyway
//lint -sem(stw::diag_lib::C_XFLImplementedServices::Clear,initializer)
class C_XFLImplementedServices
{
public:
   //IDX = 2
   bool q_EERead;
   bool q_EEWrite;
   bool aq_Reserved1[6];
   bool q_NetStart;
   bool q_NetReset;
   bool q_NetSleep;
   bool aq_Reserved2[5];
   bool q_GetSNR;
   bool q_NodeWakeupLID;
   bool q_NodeWakeupSNR;
   bool aq_Reserved3[5];
   bool aq_Reserved4[8];

   //IDX = 3
   bool q_GetLocalID;
   bool q_GetSectorCount;
   bool q_GetVersionNumber;
   bool q_GetDownloadCount;
   bool q_GetDeviceID;
   bool q_GetControlID;
   bool q_GetSectorChecksum;
   bool q_GetSectorModeCompare;
   bool q_GetTimeOutFactor; ///< ESX2 specific
   bool q_GetLastUser;      ///< ESX2 specific
   bool q_GetFlashInformation;
   bool q_GetImplementationInformation;
   bool q_GetFingerPrint;
   bool q_GetDeviceInfoAddress;
   bool q_GetBlockStartAddress;
   bool q_GetBlockEndAddress;
   bool q_GetBlockChecksum;
   bool q_GetBlockCompareMode;
   bool aq_Reserved6[14];

   //IDX = 4
   bool q_SetLocalID;
   bool q_SetCommBitrate;
   bool q_SetCommID;
   bool q_SetCommType;
   bool q_SetControlID;
   bool q_SetSectorChecksum;
   bool q_SetSectorModeCompare;
   bool q_SetTimeoutFactor; ///< ESX2 specific
   bool q_SetGatewayParameter;
   bool q_SetXflExchange;
   bool q_SetFingerPrint;
   bool q_SetTempBitrate;
   bool q_SetBlockStartAddress;
   bool q_SetBlockEndAddress;
   bool q_SetBlockChecksum;
   bool q_SetBlockCompareMode;
   bool aq_Reserved7[16];

   //IDX = 5
   bool q_EraseSector;
   bool q_ProgFlash;
   bool q_NodeSleep;
   bool q_NodeReset;
   bool q_NodeReturn;
   bool q_ReadFlash;
   bool q_NodeCompID;
   bool q_DivertStream;
   bool q_WakeupDivertClient; ///< ESX2 specific
   bool q_Reserved9;          ///< DSP specific
   bool q_ReadSeed;
   bool q_WriteKey;
   bool q_ResetLocks;
   bool q_EnterBSL;
   bool aq_Reserved8[18];

   //IDX = 6
   bool aq_TargetSpecific[32];

   void Clear(void);
   C_XFLImplementedServices(void);
};

//----------------------------------------------------------------------------------------------------------------------
///Information about which fingerprint information is supported by the server
//lint -sem(stw::diag_lib::C_XFLFingerPrintSupportedIndexes::Clear,initializer)
class C_XFLFingerPrintSupportedIndexes
{
public:
   C_XFLFingerPrintSupportedIndexes(void);
   void Clear(void);

   bool q_ProgrammingTime;
   bool q_ProgrammingDate;
   bool q_ApplicationCheckSum;
   bool q_UsernamePart1;
   bool q_UsernamePart2;
   bool q_UsernamePart3;
   bool q_UsernamePart4;
   bool aq_Reserved[25];
};

//----------------------------------------------------------------------------------------------------------------------
///Basic flashloader protocol settings
class C_XFLProtocolConfig
{
public:
   //do not prevent copy and assignment operator: we really do want a flat copy here !

   C_XFLProtocolConfig(void);

   stw::can::C_CanDispatcher * pc_CANDispatcher;

   uint32_t u32_SendID;    ///< TX comm ID to use for protocol
   uint32_t u32_ReceiveID; ///< RX comm ID to use for protocol
   uint8_t u8_LocalID;     ///< target's local ID
   uint32_t u32_Bitrate;   ///< communication bitrate in kBit/s (used for busload reduction calculations)
   bool q_XtdID;           ///< true -> 29bit IDs; false 11bit IDs
};

//----------------------------------------------------------------------------------------------------------------------
/// STW Flashloader protocol driver
class STW_DIAGLIB_EXPORT C_XFLProtocol :
   virtual public C_DLReportEvents
{
protected:
   static const uint16_t TIMEOUT_ERASE_MS  = 15500U; //must be >15s for AK4 application with old ESX2 flashloader
   static const uint16_t TIMEOUT_FLASH_MS =  1000U;  //(total timeout = Value * (MAX_TRIES-1))

   C_XFLProtocolConfig mc_Config;
   uint16_t mu16_CANDispatcherHandle;
   void m_UpdateDispatcherRXFilter(void);

   stw::can::T_STWCAN_Msg_TX mc_CanWriteMessage;

   uint8_t mu8_LastError;

   int32_t m_SendCompanyIDExt(const uint8_t ou8_LocalID, const C_XFLCompanyID & orc_CompIDIn, uint8_t & oru8_NumFound,
                              C_XFLCompanyID * const opc_CompIDOut = NULL);

   int32_t m_SendMessageWithIDs(void);
   int32_t m_SendFlashMessage(void);

   int32_t m_SendHexLine(const uint8_t * const opu8_HexLine, const uint16_t ou16_LineSize,
                         const uint32_t ou32_InterFrameDelayUs, const uint32_t ou32_TimeOut);

   int32_t m_CANSendMessage(void);
   int32_t m_CANSendMessage(const stw::can::T_STWCAN_Msg_TX & orc_MSG);
   int32_t m_CANGetResponse(stw::can::T_STWCAN_Msg_RX & orc_MSG);
   int32_t m_WaitForResponse(const uint8_t ou8_ExpectedLocalID, const uint32_t ou32_TimeOutMS,
                             const uint8_t (&orau8_Match)[8], const uint8_t ou8_NumExpectedToMatch,
                             stw::can::T_STWCAN_Msg_RX * const opc_MSG, const int16_t os16_CheckDLC = -1);
   int32_t m_GetDeviceIDBlock(const uint8_t ou8_BlockIndex, char_t(&oracn_Data)[4]);

   int32_t m_NodeFlashFunc(const uint8_t ou8_SubCommand);

   int32_t m_GetDeviceInfoAddress(const uint8_t ou8_Index, uint8_t(&orau8_Result)[4]);

   ///to be overloaded if user interaction is required:
   ///Report progress in 1/1000; return a value != C_NO_ERR to signal the flashing procedure to abort.
   ///This is just for informational purposes and not neccessarily exactly linear to the actual progress.
   virtual int32_t TRG_ReportProgress(const uint16_t ou16_Progress1_1000,
                                      const stw::scl::C_SclString & orc_AdditionalText);
   ///report status as text; can be used for logging what's going on
   virtual void TRG_ReportStatus(const stw::scl::C_SclString & orc_Text, const uint8_t ou8_Type);

   //Functions affecting timing and multithreading behaviour. Have default implementations suitable for most cases
   // but can be overloaded for special requirements.
   virtual void TRG_WaitMicroSeconds(const uint32_t ou32_NumUs); //active waiting
   virtual void TRG_HandleSystemMessages(void);                  //perform operating system message handling

   static uint8_t mh_BCD2DEC(const uint8_t ou8_BCDValue);
   static uint8_t mh_DEC2BCD(const uint8_t ou8_DECValue);
   static uint32_t mh_AU8ToU32LE(const uint8_t * const opu8_Data);
   static uint16_t mh_AU8ToU16LE(const uint8_t * const opu8_Data);

public:
   C_XFLProtocol(const C_XFLProtocolConfig & orc_Config);
   C_XFLProtocol(void);
   virtual ~C_XFLProtocol(void);

   uint8_t GetLastXFLError(void) const;

   //configuration of this class:
   /// CAN message TX and RX IDs used in flashloader protocol
   /// RX-ID = 0xffffffff -> allow any ID (e.g. for RS232 flashing)
   void CfgSetFlashId(const uint32_t ou32_SendId, const uint32_t ou32_ReceiveId);
   /// false: use 11bit CAN message IDs; true: use 29bit CAN message IDs
   void CfgSetXtdId(const bool oq_XtdOn);
   /// target node's local ID
   void CfgSetLocalId(const uint8_t ou8_LocalID);
   void CfgSetBitrate(const uint32_t ou32_Bitrate);

   /// pointer to CAN dispatcher (initialization must be done by the application !)
   void CfgSetCommDispatcher(stw::can::C_CanDispatcher * const opc_CANDispatcher);

   //protocol functions:
   //Flash functions:
   int32_t EraseSector(const uint16_t ou16_SectorNumber, const uint32_t ou32_TimeOut = TIMEOUT_ERASE_MS);
   int32_t ProgFlash(void);

   int32_t NodeSleep(void);
   int32_t NodeReturn(void);
   int32_t NodeReset(void);

   int32_t NodeSleep(const uint8_t ou8_LocalID);
   int32_t ReadFlash(const uint8_t ou8_NumBytes, const uint32_t ou32_StartAddress, uint8_t * const opu8_Data);
   int32_t SendMultiHexLine(uint16_t & oru16_NumLines, const uint32_t ou32_InterFrameDelayUs,
                            uint32_t & oru32_NumBytesSent, stw::hex_file::C_HexFile & orc_HexFile,
                            const uint32_t ou32_TimeOut = TIMEOUT_FLASH_MS);

   //Utility functions:
   int32_t SendFLASH(const uint32_t ou32_StartTimeMs, const uint8_t ou8_FLASHIntervalMs);
   int32_t SearchId(uint8_t(&orau8_LocalIds)[XFL_NUM_DIFFERENT_LOCAL_IDS],
                    uint8_t * const opu8_NumIDs, const uint32_t ou32_StartTime,
                    const uint8_t ou8_FLASHInterval);
   int32_t WakeupLocalId(const C_XFLCompanyID & orc_CompanyID, uint8_t * const opu8_NumFound = NULL,
                         C_XFLCompanyID * const opc_CompanyIDOut = NULL);
   int32_t WakeupSerialNumber(const uint8_t (&orau8_SerialNumber)[6], const C_XFLCompanyID & orc_CompanyID,
                              uint8_t & oru8_LocalID, C_XFLCompanyID * const opc_CompanyIDOut = NULL);
   int32_t SendSingleMessage(const stw::can::T_STWCAN_Msg_TX & orc_Msg);
   int32_t DivertStream(const uint8_t ou8_Device, const uint8_t ou8_Position, const uint8_t ou8_State);
   int32_t DivertStream(const uint8_t ou8_Device, const uint8_t ou8_Position, const uint8_t ou8_State,
                        const uint8_t ou8_HopHandle);

   //EEPROM services
   int32_t EEPROMRead(const uint8_t ou8_NumBytes, const uint16_t ou16_StartAddress, uint8_t * const opu8_Data);
   int32_t EEPROMWrite(const uint8_t ou8_NumBytes, const uint16_t ou16_StartAddress, const uint8_t * const opu8_Data);

   //get services
   int32_t GetLocalID(uint8_t & oru8_LocalID);
   int32_t GetLocalIDExt(uint8_t * const opu8_LocalId, const uint8_t ou8_NumMaxLocalId, uint8_t & oru8_NumFoundECUs);
   int32_t GetSectorCount(uint16_t & oru16_NumSectors);
   int32_t GetVersionNumber(uint8_t(&orau8_Version)[5], uint8_t & oru8_VersionSize);
   int32_t GetEraseCount(uint32_t & oru32_EraseCount);
   int32_t GetDeviceID(const bool oq_LongID, stw::scl::C_SclString & orc_DeviceId);
   int32_t GetControlID(uint32_t & oru32_ControlID);
   int32_t GetSecCRC(const uint16_t ou16_Sector, uint16_t & oru16_CRC, uint16_t & oru16_EECRC);
   int32_t GetModeCompare(const uint16_t ou16_Sector, uint8_t & oru8_ModeCompare);

   int32_t GetFlashInformation(const uint8_t ou8_Index, const uint8_t ou8_NumParamsToUse,
                               const uint8_t ou8_Parameter1, const uint8_t ou8_Parameter2,
                               uint8_t(&orau8_Result)[4]);
   int32_t GetFlashInformationNumberOfICs(uint8_t & oru8_ICCount);
   int32_t GetFlashInformationTotalMemorySize(const uint8_t ou8_ICIndex, uint32_t & oru32_Size);
   int32_t GetFlashInformationProtectedSectors(
      stw::scl::C_SclDynamicArray<C_XFLProtectedSectorInfo> & orc_ProtectedSectors);
   int32_t GetFlashInformationOffsetSector0(const uint8_t ou8_ICIndex, uint32_t & oru32_Offset);
   int32_t GetFlashInformationNumberRegions(const uint8_t ou8_ICIndex, uint8_t & oru8_RegionCount);
   int32_t GetFlashInformationRegionInformation(const uint8_t ou8_ICIndex, const uint8_t ou8_RegionIndex,
                                                uint32_t & oru32_BlockSize, uint16_t & oru16_BlockCount);
   int32_t GetFlashInformationEraseTime(const uint8_t ou8_ICIndex, uint32_t & oru32_EraseTime);
   int32_t GetFlashInformationWriteTime(const uint8_t ou8_ICIndex, uint32_t & oru32_WriteTime);
   int32_t GetFlashInformationAliases(C_XFLAliasedRanges & orc_Aliases);

   int32_t GetImplementationInformation(const uint8_t ou8_Index,
                                        uint8_t(&orau8_Result)[4]);
   int32_t GetImplementationInformationProtocolVersion(uint16_t & oru16_Version);
   int32_t GetImplementationInformationServices(C_XFLImplementedServices & orc_ImplementedServices);
   int32_t GetImplementationInformationHexRecords(uint8_t & oru8_MaxRecordLength, uint8_t & oru8_Granularity);

   int32_t GetFingerPrint(const uint8_t ou8_Index, uint8_t(&orau8_Result)[4]);
   int32_t GetFingerPrintSupportedIndexes(C_XFLFingerPrintSupportedIndexes & orc_SupportedIndexes);
   int32_t GetFingerPrintProgrammingTime(uint8_t & oru8_Hours, uint8_t & oru8_Minutes, uint8_t & oru8_Seconds);
   int32_t GetFingerPrintProgrammingDate(uint8_t & oru8_Years, uint8_t & oru8_Months, uint8_t & oru8_Days);
   int32_t GetFingerPrintChecksum(uint32_t & oru32_CheckSum);
   int32_t GetFingerPrintUserName(stw::scl::C_SclString & orc_UserName);

   int32_t GetDeviceInfoAddresses(stw::scl::C_SclDynamicArray<uint32_t> & orc_Addresses,
                                  const uint16_t ou16_ProtocolVersion);
   int32_t GetDeviceInfoBlock(const uint32_t ou32_Address, C_XFLECUInformation & orc_Data);

   int32_t GetBlockAddresses(const uint8_t ou8_BlockNumber, C_XFLChecksumBlock & orc_ChecksumBlock);
   int32_t GetBlockAddressesAll(stw::scl::C_SclDynamicArray<C_XFLChecksumBlock> & orc_ChecksumBlocks);
   int32_t GetBlockChecksum(const uint8_t ou8_BlockNumber, const uint8_t ou8_ChecksumType, uint32_t & oru32_Checksum);
   int32_t GetBlockCompareMode(const uint8_t ou8_BlockNumber, bool & orq_CheckAtStartup, bool & orq_CheckAfterFlashing);

   int32_t GetSNR(uint8_t(&orau8_SNR)[6]);
   int32_t GetSNRExt(uint8_t * const opu8_SNR, const uint8_t ou8_NumMax, uint8_t & oru8_NumFound);

   //set services
   int32_t SetLocalID(const uint8_t ou8_LocalIDNew);
   int32_t SetBitrateCAN(const uint32_t ou32_Bitrate, const bool oq_32BitBitrate);
   int32_t SetCANID(const uint32_t ou32_CANID);
   int32_t SetCANType(const bool oq_XtdOn);
   //not implemented yet: SetControlId
   int32_t SetSecCRC (const uint16_t ou16_Sector, uint16_t & oru16_CRC);
   int32_t SetModeCompare(const uint16_t ou16_Sector, const uint8_t ou8_ModeCompare);
   int32_t SetDivertParameter(const uint8_t ou8_Device, const uint8_t ou8_Position, const uint8_t ou8_ParameterNumber,
                              const uint8_t (&orau8_Data)[2]);
   int32_t SetXflExchange(void);
   int32_t SetFingerPrint(const uint8_t ou8_Index, const uint8_t (&orau8_Param)[4]);
   int32_t SetFingerPrintProgrammingTime(const uint8_t ou8_Hour, const uint8_t ou8_Minute, const uint8_t ou8_Second);
   int32_t SetFingerPrintProgrammingDate(const uint8_t ou8_Year, const uint8_t ou8_Month, const uint8_t ou8_Day);
   int32_t SetFingerPrintChecksum(const uint32_t ou32_CheckSum);
   int32_t SetFingerPrintUserName(const stw::scl::C_SclString & orc_UserName);
   int32_t SetTempBitrate(const uint32_t ou32_TempBitrate);
   int32_t SetBlockAddresses(const uint8_t ou8_BlockNumber, const uint32_t ou32_StartAddress,
                             const uint32_t ou32_EndAddress, const bool oq_StartAddressOnly = false);
   int32_t SetBlockChecksum(const uint8_t ou8_BlockNumber, uint32_t &
                            oru32_CalculatedChecksum);
   int32_t SetBlockCompareMode(const uint8_t ou8_BlockNumber, const bool oq_CheckAtStartup,
                               const bool oq_CheckAfterFlashing);

   //network services
   int32_t NetReset(void);
   int32_t NetStart(void);

   //ESX2 specific functions
   int32_t BBWakeup(const uint8_t ou8_Position, const uint8_t (&orau8_LastUserSignature)[2]);
   int32_t GetTimeoutFactor(const uint8_t ou8_Type, uint8_t & oru8_Factor);
   int32_t SetTimeoutFactor(const uint8_t ou8_Type, const uint8_t ou8_Factor);
   int32_t GetLastUser(uint8_t(&orau8_LastUser)[2]);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
