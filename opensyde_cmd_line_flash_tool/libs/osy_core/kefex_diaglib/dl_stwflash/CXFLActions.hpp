//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_XFLActions

   Higher level comfort layer on top of the flashloader protocol driver

   \copyright   Copyright 2002 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CXFLACTIONSHPP
#define CXFLACTIONSHPP

#include "stwtypes.hpp"
#include "CXFLProtocol.hpp"
#include "C_SclString.hpp"
#include "C_SclIniFile.hpp"
#include "C_SclDynamicArray.hpp"
#include "DiagLib_config.hpp"

namespace stw
{
namespace diag_lib
{
//----------------------------------------------------------------------------------------------------------------------
///One parameter for divert stream mechanism
class C_XFLDivertParameter
{
public:
   uint8_t u8_ParameterIndex;             ///parameter index used in protocol
   stw::scl::C_SclString c_ParameterName; ///name of parameter
   uint16_t u16_ParameterValue;           ///current value of parameter
};

//----------------------------------------------------------------------------------------------------------------------
///General parameter definition for divert stream mechanism
class C_XFLDivertParameters
{
public:
   C_XFLDivertParameters(void);
   C_XFLDivertParameters & operator =(const C_XFLDivertParameters & orc_Source);

   uint8_t u8_DeviceIndex;                                             ///target device index (0 = CAN; 1 = RS232)
   stw::scl::C_SclDynamicArray<stw::scl::C_SclString> c_PositionNames; ///available positions e.g. "CAN_BUS_[n]"
   stw::scl::C_SclDynamicArray<C_XFLDivertParameter> c_Parameters;     ///list of parameters for target device
   uint8_t u8_SelectedPosition;                                        ///target position (e.g. "0" for CAN_1)

   void LoadFromINI(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);
   int32_t SaveToINI(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);
};

//----------------------------------------------------------------------------------------------------------------------
///Parameters for divert stream CAN target busses
class C_XFLDivertParametersCAN :
   public C_XFLDivertParameters
{
private:
   ///indexes in this table (do not neccessarily match the parameter index used in the protocol)
   static const uint8_t mu8_PARAM_INDEX_CLIENT_ID_TYPE     =  0U;
   static const uint8_t mu8_PARAM_INDEX_CLIENT_BUS_BITRATE =  1U;
   static const uint8_t mu8_PARAM_INDEX_CLIENT_MASK_STD    =  2U;
   static const uint8_t mu8_PARAM_INDEX_CLIENT_LW_MASK_EXT =  3U;
   static const uint8_t mu8_PARAM_INDEX_CLIENT_HW_MASK_EXT =  4U;
   static const uint8_t mu8_PARAM_INDEX_CLIENT_ID_STD      =  5U;
   static const uint8_t mu8_PARAM_INDEX_CLIENT_LW_ID_EXT   =  6U;
   static const uint8_t mu8_PARAM_INDEX_CLIENT_HW_ID_EXT   =  7U;
   static const uint8_t mu8_PARAM_INDEX_TARGET_ID_TYPE     =  8U;
   static const uint8_t mu8_PARAM_INDEX_TARGET_BUS_BITRATE =  9U;
   static const uint8_t mu8_PARAM_INDEX_TARGET_MASK_STD    = 10U;
   static const uint8_t mu8_PARAM_INDEX_TARGET_LW_MASK_EXT = 11U;
   static const uint8_t mu8_PARAM_INDEX_TARGET_HW_MASK_EXT = 12U;
   static const uint8_t mu8_PARAM_INDEX_TARGET_ID_STD      = 13U;
   static const uint8_t mu8_PARAM_INDEX_TARGET_LW_ID_EXT   = 14U;
   static const uint8_t mu8_PARAM_INDEX_TARGET_HW_ID_EXT   = 15U;

public:
   ///set device-index, parameter names, indexes, positions and names
   C_XFLDivertParametersCAN(const uint8_t ou8_NumPositions);

   void SetValueClientIDType(const bool oq_StdIDs, const bool oq_ExtIDs);
   void SetValueClientBitrate(const uint16_t ou16_Bitrate_kBitS);
   void SetValueClientMaskStandard(const uint16_t ou16_Mask);
   void SetValueClientMaskExtended(const uint32_t ou32_Mask);
   void SetValueClientIDStandard(const uint16_t ou16_ID);
   void SetValueClientIDExtended(const uint32_t ou32_ID);

   void SetValueTargetIDType(const bool oq_StdIDs, const bool oq_ExtIDs);
   void SetValueTargetBitrate(const uint16_t ou16_Bitrate_kBitS);
   void SetValueTargetMaskStandard(const uint16_t ou16_Mask);
   void SetValueTargetMaskExtended(const uint32_t ou32_Mask);
   void SetValueTargetIDStandard(const uint16_t ou16_ID);
   void SetValueTargetIDExtended(const uint32_t ou32_ID);
};

//----------------------------------------------------------------------------------------------------------------------
///Information about one node found by "searching" for it on the bus
class C_XFLFoundNode
{
public:
   C_XFLFoundNode(void);
   uint8_t u8_NodeId;                ///< Local ID of node
   stw::scl::C_SclString c_SNR;      ///< serial number of node (String with "." separators, e.g. "11.123456.1001")
   stw::scl::C_SclString c_DeviceId; ///< name of device
   bool q_Selected;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about one flash memory region
class C_XFLFlashRegionInformation
{
public:
   uint32_t u32_BlockSize; ///< size of each block in this region in bytes
   uint16_t u16_NumBlocks; ///< number of blocks in this region (i.e. blocks of the same size)
};

//----------------------------------------------------------------------------------------------------------------------
///Information about one flash IC
class C_XFLFlashICInformation
{
public:
   uint32_t u32_TotalSize;                                             ///< total size of IC in [bytes]
   uint32_t u32_Sector0Offset;                                         ///< start address offset of sector zero in
                                                                       // [bytes]
   uint32_t u32_SectorEraseTime;                                       ///< time in [ms] that it takes to erase the
                                                                       // "slowest" sector
   uint32_t u32_ProgrammingTime;                                       ///< time in [ms] that it takes to write up to
                                                                       // 255 bytes
   stw::scl::C_SclDynamicArray<C_XFLFlashRegionInformation> c_Regions; ///< individual regions within the IC

   uint16_t GetNumberOfSectors(void) const;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about one flash memory sector
class C_XFLFlashSector
{
public:
   uint32_t u32_LowestAddress;  ///< Start address of sector in [bytes]
   uint32_t u32_HighestAddress; ///< End address of sector in [bytes]
   bool q_IsProtected;          ///< true: sector can not be modified by flashloader
   uint8_t u8_ICIndex;          ///< Index of flash IC the sector resides in

   bool IsAddressWithinSector(const uint32_t ou32_Address) const;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about a number of flash memory sectors
class C_XFLFlashSectors :
   public stw::scl::C_SclDynamicArray<C_XFLFlashSector>
{
public:
   int32_t GetSectorOccupiedByAddress(const uint32_t ou32_Address, uint16_t & oru16_Sector)
   const;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about server's flash memory layout and protected sectors
class C_XFLFlashInformation
{
public:
   stw::scl::C_SclDynamicArray<C_XFLProtectedSectorInfo> c_ProtectedSectors;
   stw::scl::C_SclDynamicArray<C_XFLFlashICInformation> c_ICs;
   C_XFLAliasedRanges c_Aliases;

   uint32_t GetEraseTimeByLinearSectorNumber(const uint16_t ou16_Sector) const;
   uint32_t GetWriteTimeOfSlowestIC(void) const;

   void ConvertToFlashSectorTable(C_XFLFlashSectors & orc_Sectors) const;
};

//----------------------------------------------------------------------------------------------------------------------

///which features are available (one layer of abstraction above T_XFL_ImplementedServices)
//lint -sem(stw::diag_lib::C_XFLFeaturesAvailable::Clear,initializer)
class C_XFLFeaturesAvailable
{
public:
   C_XFLFeaturesAvailable(void);

   void Clear(void);
   void DetermineFeatures(const C_XFLImplementedServices & orc_Services, const uint16_t ou16_ProtocolVersion);

   bool q_ProtocolVersionGE3;   ///< protocol version >= V3.00r0 ?
   bool q_SMMHandling;          ///< ESX2 shadow memory module handling
   bool q_EraseCount;           ///< get_erase_count service
   bool q_SectorBasedCRCs;      ///< sector based CRCs supported
   bool q_BlockBasedCRCsEEPROM; ///< block based CRCs in EEPROM are supported
   bool q_BlockBasedCRCsFlash;  ///< block based CRCs in Flash are supported
   bool q_ListAvailableServices;
   bool q_FlashInformation;
   bool q_FingerPrint;
   bool q_DeviceInfo;
};

//----------------------------------------------------------------------------------------------------------------------

///finger print information that can be read from a flashloader server
class C_XFLFingerPrintInformationFromServer
{
public:
   C_XFLFingerPrintInformationFromServer(void);

   C_XFLFingerPrintSupportedIndexes c_SupportedIndexes;
   bool q_SupportedIndexesValid;
   uint8_t u8_ProgrammingDateYear;
   uint8_t u8_ProgrammingDateMonth;
   uint8_t u8_ProgrammingDateDay;
   bool q_ProgrammingDateValid;
   uint8_t u8_ProgrammingTimeHour;
   uint8_t u8_ProgrammingTimeMinute;
   uint8_t u8_ProgrammingTimeSecond;
   bool q_ProgrammingTimeValid;
   stw::scl::C_SclString c_UserName;
   bool q_UserNameValid;
   uint32_t u32_Checksum;
   bool q_ChecksumValid;
};

//----------------------------------------------------------------------------------------------------------------------

///all the stuff that can be read from a flashloader server:
class C_XFLInformationFromServer
{
public:
   C_XFLInformationFromServer(void);

   uint16_t u16_ProtocolVersion;
   bool q_ProtocolVersionValid;

   uint8_t u8_LocalID;
   bool q_LocalIDValid;

   stw::scl::C_SclString c_SerialNumber;
   bool q_SerialNumberValid;

   uint16_t u16_SectorCount; ///< total number of flash sectors
   bool q_SectorCountValid;

   uint32_t u32_ControlID;
   bool q_ControlIDValid;

   stw::scl::C_SclString c_DeviceId;
   bool q_DeviceIDValid;

   stw::scl::C_SclString c_FlashloaderVersion;
   bool q_FlashloaderVersionValid;

   uint32_t u32_EraseCount;
   bool q_EraseCountValid;

   //Implementation information
   C_XFLImplementedServices c_ImplementationInformationAvailableServices;
   bool q_ImplementationInformationAvailableServicesValid;
   uint8_t u8_ImplementationInformationMaxHexRecordLength;
   uint8_t u8_ImplementationInformationHexRecordGranularity;
   bool q_ImplementationInformationHexFileInformationValid;
   C_XFLFeaturesAvailable c_AvailableFeatures; ///< "compiled" information from T_XFLImplementedServices

   //finger print data:
   C_XFLFingerPrintInformationFromServer c_FingerPrintData;

   //device information:
   stw::scl::C_SclDynamicArray<uint32_t> c_DeviceInfoAddresses;
   bool q_DeviceInfoAddressesValid;
   stw::scl::C_SclDynamicArray<C_XFLECUInformation> c_DeviceInfoBlocks; ///< from target's flash area
   stw::scl::C_SclDynamicArray<uint8_t> c_DeviceInfoBlocksValid;

   //flash mapping information:
   C_XFLFlashInformation c_FlashMappingInformation;
   bool q_FlashMappingInformationValid;

   //ESX2 specific:
   uint8_t u8_SMMEraseTimeoutFactor;
   bool q_SMMEraseTimeoutFactorValid;
   uint8_t u8_SMMWriteTimeoutFactor;
   bool q_SMMWriteTimeoutFactorValid;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about one checksum area
class C_XFLChecksumArea
{
public:
   uint32_t u32_ChecksumEEP;  ///< checksum from server EEPROM
   uint32_t u32_ChecksumCalc; ///< checksum calculated by server
   bool q_CheckAtStartup;     ///< true: flashloader shall check checksum against flash content at startup
   bool q_CheckAfterFlashing; ///< true: flashloader shall check checksum against flash content after flashing
};

//----------------------------------------------------------------------------------------------------------------------
///Information about all checksum areas
class C_XFLChecksumAreas
{
public:
   stw::scl::C_SclDynamicArray<C_XFLChecksumBlock> c_BlockConfig; ///< start / end address (not available if sector
                                                                  // based)
   stw::scl::C_SclDynamicArray<C_XFLChecksumArea> c_Areas;        ///< array with individual checksum areas
   bool q_IsBlockBased;                                           ///< false -> sector based
};

//----------------------------------------------------------------------------------------------------------------------
///Defines how the server node shall be addressed during wakeup
enum E_XFLWakeupMode
{
   eXFL_WAKEUP_MODE_LID = 0,      ///< address node by its local ID
   eXFL_WAKEUP_MODE_SNR,          ///< address node by its serial-number
   eXFL_WAKEUP_MODE_LID_THEN_SNR, ///< address node by its local ID, if there are multiple nodes with the same local ID
   ///<   use the SNR to distinguish those
   eXFL_WAKEUP_MODE_NO_WAKEUP ///< do not perform any wakeup (can be used if the node is already in wakeup mode)
};

//----------------------------------------------------------------------------------------------------------------------
///Parameters for performing an ECU wakeup
class C_XFLWakeupParameters
{
public:
   C_XFLWakeupParameters(void);

   E_XFLWakeupMode e_WakeupMode; ///< identify ECU by local ID or SNR

   bool q_SendResetRQ;                   ///< send reset request message ? true -> send; false -> don't send
   stw::can::T_STWCAN_Msg_TX t_ResetMsg; ///< reset request message (only used if q_SendResetRQ = true)

   bool q_SendFLASHRequired;   ///< true -> send "FLASH" at startup
   uint32_t u32_StartTimeMs;   ///< in ms (defines how long the "FLASH" command should be sent)
   uint8_t u8_FLASHIntervalMs; ///< interval between individual "FLASH" commands in ms.
   ///< set to 10 for most target nodes

   uint8_t u8_LocalID; ///< target node's local ID (only required with e_WakeupMode == eXFL_WAKEUP_MODE_LID)
   uint8_t au8_SNR[6]; ///< target node's serial number (only required with e_WakeupMode == eXFL_FLASH_MODE_SNR)

   stw::scl::C_SclString c_CompanyID; ///< target node's company ID (2,3 or 5 characters)
};

//----------------------------------------------------------------------------------------------------------------------
///Higher level comfort layer on top of the flashloader protocol driver
class C_XFLActions :
   public C_XFLProtocol
{
private:
   bool m_AllowSTWCID(const C_XFLCompanyID & orc_ConfiguredCompanyID, const C_XFLCompanyID & orc_RespondedCompanyID);

   void m_ReadFingerPrintInformation(C_XFLInformationFromServer & orc_Information);
   void m_ReadDeviceInfo(C_XFLInformationFromServer & orc_Information);

   int32_t m_WakeupLocalID(const C_XFLCompanyID & orc_CompanyID, const bool oq_MultiResponsesOK = false);
   int32_t m_WakeupSNR(const C_XFLCompanyID & orc_CompanyID, const uint8_t (&orau8_SNR)[6], uint8_t & oru8_LocalID);
   int32_t m_WakeupLocalIDAndSNR(const C_XFLCompanyID & orc_CompanyID, const uint8_t (&orau8_SNR)[6]);

protected:
public:
   C_XFLActions(void);

   int32_t RequestNodeReset(const stw::can::T_STWCAN_Msg_TX * const opt_ResetMessage, const bool oq_SingleNode = true);

   int32_t DivertStreamOnOffBBB(const bool oq_OnOff, const uint8_t ou8_TargetPosition,
                                const uint8_t (&orau8_UserID)[2]);
   int32_t DivertStreamOnOff(const bool oq_On, const C_XFLDivertParameters & orc_DivertParams,
                             const bool oq_UseHopHandle, const uint8_t ou8_HopHandle);

   int32_t PerformWakeup(const C_XFLWakeupParameters & orc_Parameters, uint8_t * const opu8_ActualLocalID = NULL);

   int32_t ReadFlashInformation(C_XFLFlashInformation & orc_Information, stw::scl::C_SclString & orc_ErrorText);
   int32_t SearchNodes(const stw::scl::C_SclString & orc_CompanyID, const uint32_t ou32_StartTime,
                       const uint8_t ou8_FlashInterval, stw::scl::C_SclDynamicArray<C_XFLFoundNode> & orc_FoundNodes);

   int32_t ReadServerInformation(C_XFLInformationFromServer & orc_Info);
   int32_t ReadServerSectorChecksumInformation(const uint16_t ou16_SectorCount,
                                               C_XFLChecksumAreas & orc_ChecksumInformation);
   int32_t ReadServerBlockChecksumInformation(C_XFLChecksumAreas & orc_ChecksumInformation);

   static int32_t CompIDStringToStruct(const stw::scl::C_SclString & orc_String, C_XFLCompanyID & orc_Data);
   static int32_t CompIDStructToString(const C_XFLCompanyID & orc_Data, stw::scl::C_SclString & orc_String);
   static stw::scl::C_SclString SNRBytesToString(const uint8_t (&orau8_Data)[6], const bool oq_IncludeDots);
   static int32_t SNRStringToBytes(const stw::scl::C_SclString & orc_Text, uint8_t(&orau8_Data)[6]);

   static stw::scl::C_SclString XFLProtocolErrorToText(const int32_t os32_ReturnValue, const uint8_t ou8_ErrorCode);
};

//----------------------------------------------------------------------------------------------------------------------
}
}

//----------------------------------------------------------------------------------------------------------------------

#endif
