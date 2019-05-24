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

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CCANDispatcher.h"
#include "CHexFile.h"
#include "SCLDynamicArray.h"
#include "CSCLString.h"
#include "CSCLStringList.h"
#include "DiagLib_config.h"
#include "stw_can.h" //for message structures
#include "CDLReportEvents.h"
#include "CXFLECUInformation.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_diag_lib
{

/* -- Global Constants ----------------------------------------------------- */

const stw_types::uint8 XFL_DIVERT_TARGET_BABY_B = 0U;

//error resturns common to flashesx.cpp
//FlashESX.CPP Error Codes
const stw_types::sint32 XFL_ERR_DEVICE_SPECIFIC =  -20;   ///< error value can be retrieved with GetLastXFLError

//error codes.
const stw_types::uint8 XFL_ERR_UNDEFINED_ERROR       = 0x00U;
const stw_types::uint8 XFL_ERR_APP_DEF_ERR_MIN       = 0x10U;
const stw_types::uint8 XFL_ERR_APP_DEF_ERR_MAX       = 0x3FU;
const stw_types::uint8 XFL_ERR_CHECKSUM_ERR          = 0xD6U;
const stw_types::uint8 XFL_ERR_FLASH_PROG_ERR        = 0xE1U;
const stw_types::uint8 XFL_ERR_FLASH_ERASE_ERR       = 0xE2U;
const stw_types::uint8 XFL_ERR_CAN_BITTIME_ERR       = 0xE3U;
const stw_types::uint8 XFL_ERR_OUT_OF_RANGE_ERR      = 0xF7U;
const stw_types::uint8 XFL_ERR_EE_VERIFY_ERR         = 0xFBU;
const stw_types::uint8 XFL_ERR_EE_READ_ERR           = 0xFCU;
const stw_types::uint8 XFL_ERR_EE_WRITE_ERR          = 0xFDU;
const stw_types::uint8 XFL_ERR_DIVERT_STREAM_COM_ERR = 0xFEU;
const stw_types::uint8 XFL_ERR_WRONG_REC_TYPE        = 0xFFU;

//non-configured checksum block
const stw_types::uint32 XFL_CHK_BLOCK_END_INVALID = 0x00000000UL;

//number of different local IDs
const stw_types::uint16 XFL_NUM_DIFFERENT_LOCAL_IDS = 256U; ///< 0..255 (255 is reserved, but we never know)

//default protocol parameters:
const stw_types::uint8 XFL_DEFAULT_SEND_ID    = 0x51U;

/* -- Types ---------------------------------------------------------------- */

///Information about in protected flash sector
class C_XFLProtectedSectorInfo
{
public:
   stw_types::uint8 u8_ICIndex;
   stw_types::uint16 u16_SectorNumber;
};

//---------------------------------------------------------------------------
///Information about aliased memory
class C_XFLAliasedRange
{
public:
   stw_types::uint32 u32_PhysicalAddress;
   stw_types::uint32 u32_Size;
   stw_types::uint32 u32_AliasedAddress;

   stw_types::sint64 GetOffset(void) const;
};

//---------------------------------------------------------------------------
///Information about multiple aliased memory ranges
class C_XFLAliasedRanges : public stw_scl::SCLDynamicArray<C_XFLAliasedRange>
{
public:
   stw_types::sint32 GetRangeOccupiedByAddress(const stw_types::uint32 ou32_Address, stw_types::uint8 & oru8_Range)
      const;
};

//---------------------------------------------------------------------------
///Information about checksum block
class C_XFLChecksumBlock
{
public:
   stw_types::uint32 u32_StartAddress;  //start address of block
   stw_types::uint32 u32_EndAddress;    //end address of block (= highest occupied address)
   bool q_BlockDefinitionValid;         //true -> valid information contained in start and end address
};

//---------------------------------------------------------------------------
///Raw company ID data
class C_XFLCompanyID
{
public:
   stw_types::uint8 au8_Data[5];
   stw_types::uint8 u8_NumBytes; ///< number of actually used bytes (only 2 or 5 are valid)
};

//---------------------------------------------------------------------------
///Information about flashloader services available on the server
//use a big struct and not a bitfield:
//- memory use is not a big issue on PC
//- we must not rely on bit positions anyway
//lint -sem(stw_diag_lib::C_XFLImplementedServices::Clear,initializer)
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
   bool q_Reserved9; ///< DSP specific
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

//---------------------------------------------------------------------------
///Information about which fingerprint information is supported by the server
//lint -sem(stw_diag_lib::C_XFLFingerPrintSupportedIndexes::Clear,initializer)
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

//---------------------------------------------------------------------------
///Basic flashloader protocol settings
class C_XFLProtocolConfig
{
public:
   //do not prevent copy and assignment operator: we really do want a flat copy here !

   C_XFLProtocolConfig(void);

   stw_can::C_CAN_Dispatcher * pc_CANDispatcher;

   stw_types::uint32  u32_SendID;    ///< TX comm ID to use for protocol
   stw_types::uint32  u32_ReceiveID; ///< RX comm ID to use for protocol
   stw_types::uint8   u8_LocalID;    ///< target's local ID
   stw_types::uint32  u32_Bitrate;   ///< communication bitrate in kBit/s (used for busload reduction calculations)
   bool               q_XtdID;       ///< true -> 29bit IDs; false 11bit IDs
};

//---------------------------------------------------------------------------
/// STW Flashloader protocol driver
class STW_DIAGLIB_EXPORT C_XFLProtocol : virtual public C_DLReportEvents
{
protected:

   static const stw_types::uint16 TIMEOUT_ERASE_MS  = 15500U; //must be >15s for AK4 application with old ESX2 flashloader
   static const stw_types::uint16 TIMEOUT_FLASH_MS =  1000U; //(total timeout = Value * (MAX_TRIES-1))

   C_XFLProtocolConfig mc_Config;
   stw_types::uint16 mu16_CANDispatcherHandle;
   void m_UpdateDispatcherRXFilter(void);

   stw_can::T_STWCAN_Msg_TX mc_CanWriteMessage;

   stw_types::uint8 mu8_LastError;

   stw_types::sint32 m_SendCompanyIDExt(const stw_types::uint8 ou8_LocalID, const C_XFLCompanyID & orc_CompIDIn,
                                        stw_types::uint8 & oru8_NumFound,
                                        C_XFLCompanyID * const opc_CompIDOut = NULL);

   stw_types::sint32 m_SendMessageWithIDs(void);
   stw_types::sint32 m_SendFlashMessage(void);

   stw_types::sint32 m_SendHexLine(const stw_types::uint8 * const opu8_HexLine, const stw_types::uint16 ou16_LineSize,
                                   const stw_types::uint32 ou32_InterFrameDelayUs,
                                   const stw_types::uint32 ou32_TimeOut);

   stw_types::sint32 m_CANSendMessage(void);
   stw_types::sint32 m_CANSendMessage(const stw_can::T_STWCAN_Msg_TX & orc_MSG);
   stw_types::sint32 m_CANGetResponse(stw_can::T_STWCAN_Msg_RX & orc_MSG);
   stw_types::sint32 m_WaitForResponse(const stw_types::uint8 ou8_ExpectedLocalID,
                                       const stw_types::uint32 ou32_TimeOutMS,
                                       const stw_types::uint8 (& orau8_Match)[8],
                                       const stw_types::uint8 ou8_NumExpectedToMatch,
                                       stw_can::T_STWCAN_Msg_RX * const opc_MSG,
                                       const stw_types::sint16 os16_CheckDLC = -1);
   stw_types::sint32 m_GetDeviceIDBlock(const stw_types::uint8 ou8_BlockIndex, stw_types::charn (& oracn_Data)[4]);

   stw_types::sint32 m_NodeFlashFunc(const stw_types::uint8 ou8_SubCommand);

   stw_types::sint32 m_GetDeviceInfoAddress(const stw_types::uint8 ou8_Index, stw_types::uint8 (& orau8_Result)[4]);

   ///to be overloaded if user interaction is required:
   ///Report progress in 1/1000; return a value != C_NO_ERR to signal the flashing procedure to abort.
   ///This is just for informational purposes and not neccessarily exactly linear to the actual progress.
   virtual stw_types::sint32 TRG_ReportProgress(const stw_types::uint16 ou16_Progress1_1000,
                                                const stw_scl::C_SCLString & orc_AdditionalText);
   ///report status as text; can be used for logging what's going on
   virtual void TRG_ReportStatus(const stw_scl::C_SCLString & orc_Text, const stw_types::uint8 ou8_Type);

   //Functions affecting timing and multithreading behaviour. Have default implementations suitable for most cases
   // but can be overloaded for special requirements.
   virtual void TRG_WaitMicroSeconds(const stw_types::uint32 ou32_NumUs); //active waiting
   virtual void TRG_HandleSystemMessages(void);   //perform operating system message handling

   static stw_types::uint8 mh_BCD2DEC(const stw_types::uint8 ou8_BCDValue);
   static stw_types::uint8 mh_DEC2BCD(const stw_types::uint8 ou8_DECValue);
   static stw_types::uint32 mh_AU8_2_U32_LE(const stw_types::uint8 * const opu8_Data);
   static stw_types::uint16 mh_AU8_2_U16_LE(const stw_types::uint8 * const opu8_Data);

public:
   C_XFLProtocol(const C_XFLProtocolConfig & orc_Config);
   C_XFLProtocol(void);
   virtual ~C_XFLProtocol(void);

   stw_types::uint8 GetLastXFLError(void) const;

   //configuration of this class:
   /// CAN message TX and RX IDs used in flashloader protocol
   /// RX-ID = 0xffffffff -> allow any ID (e.g. for RS232 flashing)
   void CfgSetFlashId(const stw_types::uint32 ou32_SendId, const stw_types::uint32 ou32_ReceiveId);
   /// false: use 11bit CAN message IDs; true: use 29bit CAN message IDs
   void CfgSetXtdId(const bool oq_XtdOn);
   /// target node's local ID
   void CfgSetLocalId(const stw_types::uint8 ou8_LocalID);
   void CfgSetBitrate(const stw_types::uint32 ou32_Bitrate);

   /// pointer to CAN dispatcher (initialization must be done by the application !)
   void CfgSetCommDispatcher(stw_can::C_CAN_Dispatcher * const opc_CANDispatcher);

   //protocol functions:
   //Flash functions:
   stw_types::sint32 EraseSector(const stw_types::uint16 ou16_SectorNumber,
                                 const stw_types::uint32 ou32_TimeOut = TIMEOUT_ERASE_MS);
   stw_types::sint32 ProgFlash(void);

   stw_types::sint32 NodeSleep(void);
   stw_types::sint32 NodeReturn(void);
   stw_types::sint32 NodeReset(void);

   stw_types::sint32 NodeSleep(const stw_types::uint8 ou8_LocalID);
   stw_types::sint32 ReadFlash(const stw_types::uint8 ou8_NumBytes, const stw_types::uint32 ou32_StartAddress,
                               stw_types::uint8 * const opu8_Data);
   stw_types::sint32 SendMultiHexLine(stw_types::uint16 & oru16_NumLines,
                                      const stw_types::uint32 ou32_InterFrameDelayUs,
                                      stw_types::uint32 & oru32_NumBytesSent, stw_hex_file::C_HexFile & orc_HexFile,
                                      const stw_types::uint32 ou32_TimeOut = TIMEOUT_FLASH_MS);

   //Utility functions:
   stw_types::sint32 SendFLASH(const stw_types::uint32 ou32_StartTimeMs, const stw_types::uint8 ou8_FLASHIntervalMs);
   stw_types::sint32 SearchId(stw_types::uint8 (& orau8_LocalIDs)[XFL_NUM_DIFFERENT_LOCAL_IDS],
                              stw_types::uint8 * const opu8_NumIDs, const stw_types::uint32 ou32_StartTime,
                              const stw_types::uint8 ou8_FLASHInterval);
   stw_types::sint32 WakeupLocalId(const C_XFLCompanyID & orc_CompanyID, stw_types::uint8 * const opu8_NumFound = NULL,
                                   C_XFLCompanyID * const opc_CompanyIDOut = NULL);
   stw_types::sint32 WakeupSerialNumber(const stw_types::uint8 (& orau8_SerialNumber)[6],
                                        const C_XFLCompanyID & orc_CompanyID, stw_types::uint8 & oru8_LocalID,
                                        C_XFLCompanyID * const opc_CompanyIDOut = NULL);
   stw_types::sint32 SendSingleMessage(const stw_can::T_STWCAN_Msg_TX & orc_Msg);
   stw_types::sint32 DivertStream(const stw_types::uint8 ou8_Device, const stw_types::uint8 ou8_Position,
                                  const stw_types::uint8 ou8_State);
   stw_types::sint32 DivertStream(const stw_types::uint8 ou8_Device, const stw_types::uint8 ou8_Position,
                                  const stw_types::uint8 ou8_State, const stw_types::uint8 ou8_HopHandle);

   //EEPROM services
   stw_types::sint32 EEPROMRead(const stw_types::uint8 ou8_NumBytes, const stw_types::uint16 ou16_StartAddress,
                                stw_types::uint8 * const opu8_Data);
   stw_types::sint32 EEPROMWrite(const stw_types::uint8 ou8_NumBytes, const stw_types::uint16 ou16_StartAddress,
                                 const stw_types::uint8 * const opu8_Data);

   //get services
   stw_types::sint32 GetLocalID(stw_types::uint8 & oru8_LocalID);
   stw_types::sint32 GetLocalIDExt(stw_types::uint8 * const opu8_LocalId, const stw_types::uint8 ou8_NumMaxLocalId,
                                   stw_types::uint8 & oru8_NumFoundECUs);
   stw_types::sint32 GetSectorCount(stw_types::uint16 & oru16_NumSectors);
   stw_types::sint32 GetVersionNumber(stw_types::uint8 (& orau8_Version)[5], stw_types::uint8 & oru8_VersionSize);
   stw_types::sint32 GetEraseCount(stw_types::uint32 & oru32_EraseCount);
   stw_types::sint32 GetDeviceID(const bool oq_LongID, stw_scl::C_SCLString & orc_DeviceID);
   stw_types::sint32 GetControlID(stw_types::uint32 & oru32_ControlID);
   stw_types::sint32 GetSecCRC(const stw_types::uint16 ou16_Sector, stw_types::uint16 & oru16_CRC,
                               stw_types::uint16 & oru16_EECRC);
   stw_types::sint32 GetModeCompare(const stw_types::uint16 ou16_Sector, stw_types::uint8 & oru8_ModeCompare);

   stw_types::sint32 GetFlashInformation(const stw_types::uint8 ou8_Index, const stw_types::uint8 ou8_NumParamsToUse,
                                         const stw_types::uint8 ou8_Parameter1, const stw_types::uint8 ou8_Parameter2,
                                         stw_types::uint8 (&orau8_Result)[4]);
   stw_types::sint32 GetFlashInformationNumberOfICs(stw_types::uint8 & oru8_ICCount);
   stw_types::sint32 GetFlashInformationTotalMemorySize(const stw_types::uint8 ou8_ICIndex,
                                                        stw_types::uint32 & oru32_Size);
   stw_types::sint32 GetFlashInformationProtectedSectors(
                                           stw_scl::SCLDynamicArray <C_XFLProtectedSectorInfo> & orc_ProtectedSectors);
   stw_types::sint32 GetFlashInformationOffsetSector0(const stw_types::uint8 ou8_ICIndex,
                                                      stw_types::uint32 & oru32_Offset);
   stw_types::sint32 GetFlashInformationNumberRegions(const stw_types::uint8 ou8_ICIndex,
                                                      stw_types::uint8 & oru8_RegionCount);
   stw_types::sint32 GetFlashInformationRegionInformation(const stw_types::uint8 ou8_ICIndex,
                                                          const stw_types::uint8 ou8_RegionIndex,
                                                          stw_types::uint32 & oru32_BlockSize,
                                                          stw_types::uint16 & oru16_BlockCount);
   stw_types::sint32 GetFlashInformationEraseTime(const stw_types::uint8 ou8_ICIndex,
                                                  stw_types::uint32 & oru32_EraseTime);
   stw_types::sint32 GetFlashInformationWriteTime(const stw_types::uint8 ou8_ICIndex,
                                                  stw_types::uint32 & oru32_WriteTime);
   stw_types::sint32 GetFlashInformationAliases(C_XFLAliasedRanges & orc_Aliases);

   stw_types::sint32 GetImplementationInformation(const stw_types::uint8 ou8_Index,
                                                  stw_types::uint8 (&orau8_Result)[4]);
   stw_types::sint32 GetImplementationInformationProtocolVersion(stw_types::uint16 & oru16_Version);
   stw_types::sint32 GetImplementationInformationServices(C_XFLImplementedServices & orc_ImplementedServices);
   stw_types::sint32 GetImplementationInformationHexRecords(stw_types::uint8 & oru8_MaxRecordLength,
                                                            stw_types::uint8 & oru8_Granularity);

   stw_types::sint32 GetFingerPrint(const stw_types::uint8 ou8_Index, stw_types::uint8 (& orau8_Result)[4]);
   stw_types::sint32 GetFingerPrintSupportedIndexes(C_XFLFingerPrintSupportedIndexes & orc_SupportedIndexes);
   stw_types::sint32 GetFingerPrintProgrammingTime(stw_types::uint8 & oru8_Hours, stw_types::uint8 & oru8_Minutes,
                                                   stw_types::uint8 & oru8_Seconds);
   stw_types::sint32 GetFingerPrintProgrammingDate(stw_types::uint8 & oru8_Years, stw_types::uint8 & oru8_Months,
                                                   stw_types::uint8 & oru8_Days);
   stw_types::sint32 GetFingerPrintChecksum(stw_types::uint32 & oru32_CheckSum);
   stw_types::sint32 GetFingerPrintUserName(stw_scl::C_SCLString & orc_UserName);

   stw_types::sint32 GetDeviceInfoAddresses(stw_scl::SCLDynamicArray<stw_types::uint32> & orc_Addresses,
                                            const stw_types::uint16 ou16_ProtocolVersion);
   stw_types::sint32 GetDeviceInfoBlock(const stw_types::uint32 ou32_Address, C_XFLECUInformation & orc_Data);

   stw_types::sint32 GetBlockAddresses(const stw_types::uint8 ou8_BlockNumber, C_XFLChecksumBlock & orc_ChecksumBlock);
   stw_types::sint32 GetBlockAddressesAll(stw_scl::SCLDynamicArray<C_XFLChecksumBlock> & orc_ChecksumBlocks);
   stw_types::sint32 GetBlockChecksum(const stw_types::uint8 ou8_BlockNumber, const stw_types::uint8 ou8_ChecksumType,
                                      stw_types::uint32 & oru32_Checksum);
   stw_types::sint32 GetBlockCompareMode(const stw_types::uint8 ou8_BlockNumber, bool & orq_CheckAtStartup,
                                         bool & orq_CheckAfterFlashing);

   stw_types::sint32 GetSNR(stw_types::uint8 (&orau8_SNR)[6]);
   stw_types::sint32 GetSNRExt(stw_types::uint8 * const opu8_SNR, const stw_types::uint8 ou8_NumMax,
                               stw_types::uint8 & oru8_NumFound);

   //set services
   stw_types::sint32 SetLocalID(const stw_types::uint8 ou8_LocalIDNew);
   stw_types::sint32 SetBitrateCAN(const stw_types::uint32 ou32_Bitrate, const bool oq_32BitBitrate);
   stw_types::sint32 SetCANID(const stw_types::uint32 ou32_CANID);
   stw_types::sint32 SetCANType(const bool oq_XtdOn);
   //not implemented yet: SetControlId
   stw_types::sint32 SetSecCRC (const stw_types::uint16 ou16_Sector, stw_types::uint16 & oru16_CRC);
   stw_types::sint32 SetModeCompare(const stw_types::uint16 ou16_Sector, const stw_types::uint8 ou8_ModeCompare);
   stw_types::sint32 SetDivertParameter(const stw_types::uint8 ou8_Device, const stw_types::uint8 ou8_Position,
                                        const stw_types::uint8 ou8_ParameterNumber,
                                        const stw_types::uint8 (& orau8_Data)[2]);
   stw_types::sint32 SetXflExchange(void);
   stw_types::sint32 SetFingerPrint(const stw_types::uint8 ou8_Index, const stw_types::uint8 (& orau8_Param)[4]);
   stw_types::sint32 SetFingerPrintProgrammingTime(const stw_types::uint8 ou8_Hour, const stw_types::uint8 ou8_Minute,
                                                   const stw_types::uint8 ou8_Second);
   stw_types::sint32 SetFingerPrintProgrammingDate(const stw_types::uint8 ou8_Year, const stw_types::uint8 ou8_Month,
                                                   const stw_types::uint8 ou8_Day);
   stw_types::sint32 SetFingerPrintChecksum(const stw_types::uint32 ou32_CheckSum);
   stw_types::sint32 SetFingerPrintUserName(const stw_scl::C_SCLString & orc_UserName);
   stw_types::sint32 SetTempBitrate(const stw_types::uint32 ou32_TempBitrate);
   stw_types::sint32 SetBlockAddresses(const stw_types::uint8 ou8_BlockNumber,
                                       const stw_types::uint32 ou32_StartAddress,
                                       const stw_types::uint32 ou32_EndAddress,
                                       const bool oq_StartAddressOnly = false);
   stw_types::sint32 SetBlockChecksum(const stw_types::uint8 ou8_BlockNumber, stw_types::uint32 &
                                      oru32_CalculatedChecksum);
   stw_types::sint32 SetBlockCompareMode(const stw_types::uint8 ou8_BlockNumber, const bool oq_CheckAtStartup,
                                         const bool oq_CheckAfterFlashing);

   //network services
   stw_types::sint32 NetReset(void);
   stw_types::sint32 NetStart(void);

   //ESX2 specific functions
   stw_types::sint32 BBWakeup(const stw_types::uint8 ou8_Position,
                              const stw_types::uint8 (& orau8_LastUserSignature)[2]);
   stw_types::sint32 GetTimeoutFactor(const stw_types::uint8 ou8_Type, stw_types::uint8 & oru8_Factor);
   stw_types::sint32 SetTimeoutFactor(const stw_types::uint8 ou8_Type, const stw_types::uint8 ou8_Factor);
   stw_types::sint32 GetLastUser(stw_types::uint8 (& orau8_LastUser)[2]);
};

/* -- Extern Global Variables ---------------------------------------------- */

} //end of namespace

#endif

