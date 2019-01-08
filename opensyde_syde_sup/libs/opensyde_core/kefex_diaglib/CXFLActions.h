//-----------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_XFLActions

   Higher level comfort layer on top of the flashloader protocol driver

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     xx.xx.200x  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef CXFLActionsH
#define CXFLActionsH

#include "stwtypes.h"
#include "CXFLProtocol.h"
#include "CSCLString.h"
#include "CSCLIniFile.h"
#include "SCLDynamicArray.h"
#include "DiagLib_config.h"

namespace stw_diag_lib
{

//---------------------------------------------------------------------------
///One parameter for divert stream mechanism
class C_XFLDivertParameter
{
public:
   stw_types::uint8 u8_ParameterIndex;   ///parameter index used in protocol
   stw_scl::C_SCLString c_ParameterName; ///name of parameter
   stw_types::uint16 u16_ParameterValue; ///current value of parameter
};

//---------------------------------------------------------------------------
///General parameter definition for divert stream mechanism
class C_XFLDivertParameters
{
public:
   C_XFLDivertParameters(void);
   C_XFLDivertParameters & operator =(const C_XFLDivertParameters & orc_Source);

   stw_types::uint8 u8_DeviceIndex;                                ///target device index (0 = CAN; 1 = RS232)
   stw_scl::SCLDynamicArray<stw_scl::C_SCLString> c_PositionNames; ///available positions e.g. "CAN_BUS_[n]"
   stw_scl::SCLDynamicArray<C_XFLDivertParameter> c_Parameters;    ///list of parameters for target device
   stw_types::uint8 u8_SelectedPosition;                           ///target position (e.g. "0" for CAN_1)

   void LoadFromINI(stw_scl::C_SCLIniFile & orc_IniFile, const stw_scl::C_SCLString & orc_Section);
   stw_types::sint32 SaveToINI(stw_scl::C_SCLIniFile & orc_IniFile, const stw_scl::C_SCLString & orc_Section);
};

//---------------------------------------------------------------------------
///Parameters for divert stream CAN target busses
class C_XFLDivertParametersCAN : public C_XFLDivertParameters
{
private:
   ///indexes in this table (do not neccessarily match the parameter index used in the protocol)
   static const stw_types::uint8 mu8_PARAM_INDEX_CLIENT_ID_TYPE     =  0U;
   static const stw_types::uint8 mu8_PARAM_INDEX_CLIENT_BUS_BITRATE =  1U;
   static const stw_types::uint8 mu8_PARAM_INDEX_CLIENT_MASK_STD    =  2U;
   static const stw_types::uint8 mu8_PARAM_INDEX_CLIENT_LW_MASK_EXT =  3U;
   static const stw_types::uint8 mu8_PARAM_INDEX_CLIENT_HW_MASK_EXT =  4U;
   static const stw_types::uint8 mu8_PARAM_INDEX_CLIENT_ID_STD      =  5U;
   static const stw_types::uint8 mu8_PARAM_INDEX_CLIENT_LW_ID_EXT   =  6U;
   static const stw_types::uint8 mu8_PARAM_INDEX_CLIENT_HW_ID_EXT   =  7U;
   static const stw_types::uint8 mu8_PARAM_INDEX_TARGET_ID_TYPE     =  8U;
   static const stw_types::uint8 mu8_PARAM_INDEX_TARGET_BUS_BITRATE =  9U;
   static const stw_types::uint8 mu8_PARAM_INDEX_TARGET_MASK_STD    = 10U;
   static const stw_types::uint8 mu8_PARAM_INDEX_TARGET_LW_MASK_EXT = 11U;
   static const stw_types::uint8 mu8_PARAM_INDEX_TARGET_HW_MASK_EXT = 12U;
   static const stw_types::uint8 mu8_PARAM_INDEX_TARGET_ID_STD      = 13U;
   static const stw_types::uint8 mu8_PARAM_INDEX_TARGET_LW_ID_EXT   = 14U;
   static const stw_types::uint8 mu8_PARAM_INDEX_TARGET_HW_ID_EXT   = 15U;

public:
   ///set device-index, parameter names, indexes, positions and names
   C_XFLDivertParametersCAN(const stw_types::uint8 ou8_NumPositions);

   void SetValueClientIDType(const bool oq_StdIDs, const bool oq_ExtIDs);
   void SetValueClientBitrate(const stw_types::uint16 ou16_Bitrate_kBitS);
   void SetValueClientMaskStandard(const stw_types::uint16 ou16_Mask);
   void SetValueClientMaskExtended(const stw_types::uint32 ou32_Mask);
   void SetValueClientIDStandard(const stw_types::uint16 ou16_ID);
   void SetValueClientIDExtended(const stw_types::uint32 ou32_ID);

   void SetValueTargetIDType(const bool oq_StdIDs, const bool oq_ExtIDs);
   void SetValueTargetBitrate(const stw_types::uint16 ou16_Bitrate_kBitS);
   void SetValueTargetMaskStandard(const stw_types::uint16 ou16_Mask);
   void SetValueTargetMaskExtended(const stw_types::uint32 ou32_Mask);
   void SetValueTargetIDStandard(const stw_types::uint16 ou16_ID);
   void SetValueTargetIDExtended(const stw_types::uint32 ou32_ID);
};

//---------------------------------------------------------------------------
///Information about one node found by "searching" for it on the bus
class C_XFLFoundNode
{
public:
   C_XFLFoundNode(void);
   stw_types::uint8 u8_NodeID;      ///< Local ID of node
   stw_scl::C_SCLString c_SNR;      ///< serial number of node (String with "." separators, e.g. "11.123456.1001")
   stw_scl::C_SCLString c_DeviceID; ///< name of device
   bool q_Selected;
};

//---------------------------------------------------------------------------
///Information about one flash memory region
class C_XFLFlashRegionInformation
{
public:
   stw_types::uint32 u32_BlockSize;   ///< size of each block in this region in bytes
   stw_types::uint16 u16_NumBlocks;   ///< number of blocks in this region (i.e. blocks of the same size)
};

//---------------------------------------------------------------------------
///Information about one flash IC
class C_XFLFlashICInformation
{
public:
   stw_types::uint32 u32_TotalSize;       ///< total size of IC in [bytes]
   stw_types::uint32 u32_Sector0Offset;   ///< start address offset of sector zero in [bytes]
   stw_types::uint32 u32_SectorEraseTime; ///< time in [ms] that it takes to erase the "slowest" sector
   stw_types::uint32 u32_ProgrammingTime; ///< time in [ms] that it takes to write up to 255 bytes
   stw_scl::SCLDynamicArray<C_XFLFlashRegionInformation> c_Regions; ///< individual regions within the IC

   stw_types::uint16 GetNumberOfSectors(void) const;
};

//---------------------------------------------------------------------------
///Information about one flash memory sector
class C_XFLFlashSector
{
public:
   stw_types::uint32 u32_LowestAddress;   ///< Start address of sector in [bytes]
   stw_types::uint32 u32_HighestAddress;  ///< End address of sector in [bytes]
   bool q_IsProtected;                    ///< true: sector can not be modified by flashloader
   stw_types::uint8 u8_ICIndex;           ///< Index of flash IC the sector resides in

   bool IsAddressWithinSector(const stw_types::uint32 ou32_Address) const;
};

//---------------------------------------------------------------------------
///Information about a number of flash memory sectors
class C_XFLFlashSectors : public stw_scl::SCLDynamicArray<C_XFLFlashSector>
{
public:
   stw_types::sint32 GetSectorOccupiedByAddress(const stw_types::uint32 ou32_Address, stw_types::uint16 & oru16_Sector)
      const;
};

//---------------------------------------------------------------------------
///Information about server's flash memory layout and protected sectors
class C_XFLFlashInformation
{
public:
   stw_scl::SCLDynamicArray<C_XFLProtectedSectorInfo> c_ProtectedSectors;
   stw_scl::SCLDynamicArray<C_XFLFlashICInformation> c_ICs;
   C_XFLAliasedRanges c_Aliases;

   stw_types::uint32 GetEraseTimeByLinearSectorNumber(const stw_types::uint16 ou16_Sector) const;
   stw_types::uint32 GetWriteTimeOfSlowestIC(void) const;

   void ConvertToFlashSectorTable(C_XFLFlashSectors & orc_Sectors) const;
};

//---------------------------------------------------------------------------

///which features are available (one layer of abstraction above T_XFL_ImplementedServices)
//lint -sem(stw_diag_lib::C_XFLFeaturesAvailable::Clear,initializer)
class C_XFLFeaturesAvailable
{
public:
   C_XFLFeaturesAvailable(void);

   void Clear(void);
   void DetermineFeatures(const C_XFLImplementedServices & orc_Services, const stw_types::uint16 ou16_ProtocolVersion);

   bool q_ProtocolVersionGE3; ///< protocol version >= V3.00r0 ?
   bool q_SMMHandling;        ///< ESX2 shadow memory module handling
   bool q_EraseCount;         ///< get_erase_count service
   bool q_SectorBasedCRCs;       ///< sector based CRCs supported
   bool q_BlockBasedCRCsEEPROM;  ///< block based CRCs in EEPROM are supported
   bool q_BlockBasedCRCsFlash;   ///< block based CRCs in Flash are supported
   bool q_ListAvailableServices;
   bool q_FlashInformation;
   bool q_FingerPrint;
   bool q_DeviceInfo;
};

//---------------------------------------------------------------------------

///finger print information that can be read from a flashloader server
class C_XFLFingerPrintInformationFromServer
{
public:
   C_XFLFingerPrintInformationFromServer(void);

   C_XFLFingerPrintSupportedIndexes c_SupportedIndexes;
   bool q_SupportedIndexesValid;
   stw_types::uint8 u8_ProgrammingDateYear;
   stw_types::uint8 u8_ProgrammingDateMonth;
   stw_types::uint8 u8_ProgrammingDateDay;
   bool q_ProgrammingDateValid;
   stw_types::uint8 u8_ProgrammingTimeHour;
   stw_types::uint8 u8_ProgrammingTimeMinute;
   stw_types::uint8 u8_ProgrammingTimeSecond;
   bool q_ProgrammingTimeValid;
   stw_scl::C_SCLString c_UserName;
   bool q_UserNameValid;
   stw_types::uint32 u32_Checksum;
   bool q_ChecksumValid;
};

//---------------------------------------------------------------------------

///all the stuff that can be read from a flashloader server:
class C_XFLInformationFromServer
{
public:
   C_XFLInformationFromServer(void);

   stw_types::uint16 u16_ProtocolVersion;
   bool q_ProtocolVersionValid;

   stw_types::uint8  u8_LocalID;
   bool q_LocalIDValid;

   stw_scl::C_SCLString c_SerialNumber;
   bool q_SerialNumberValid;

   stw_types::uint16 u16_SectorCount;  ///< total number of flash sectors
   bool q_SectorCountValid;

   stw_types::uint32 u32_ControlID;
   bool q_ControlIDValid;

   stw_scl::C_SCLString c_DeviceID;
   bool q_DeviceIDValid;

   stw_scl::C_SCLString c_FlashloaderVersion;
   bool q_FlashloaderVersionValid;

   stw_types::uint32 u32_EraseCount;
   bool q_EraseCountValid;

   //Implementation information
   C_XFLImplementedServices c_ImplementationInformationAvailableServices;
   bool q_ImplementationInformationAvailableServicesValid;
   stw_types::uint8 u8_ImplementationInformationMaxHexRecordLength;
   stw_types::uint8 u8_ImplementationInformationHexRecordGranularity;
   bool q_ImplementationInformationHexFileInformationValid;
   C_XFLFeaturesAvailable c_AvailableFeatures; ///< "compiled" information from T_XFLImplementedServices

   //finger print data:
   C_XFLFingerPrintInformationFromServer c_FingerPrintData;

   //device information:
   stw_scl::SCLDynamicArray<stw_types::uint32> c_DeviceInfoAddresses;
   bool q_DeviceInfoAddressesValid;
   stw_scl::SCLDynamicArray<C_XFLECUInformation> c_DeviceInfoBlocks;  ///< from target's flash area
   stw_scl::SCLDynamicArray<stw_types::uint8> c_DeviceInfoBlocksValid;

   //flash mapping information:
   C_XFLFlashInformation c_FlashMappingInformation;
   bool q_FlashMappingInformationValid;

   //ESX2 specific:
   stw_types::uint8 u8_SMMEraseTimeoutFactor;
   bool q_SMMEraseTimeoutFactorValid;
   stw_types::uint8 u8_SMMWriteTimeoutFactor;
   bool q_SMMWriteTimeoutFactorValid;
};

//---------------------------------------------------------------------------
///Information about one checksum area
class C_XFLChecksumArea
{
public:
   stw_types::uint32 u32_ChecksumEEP;  ///< checksum from server EEPROM
   stw_types::uint32 u32_ChecksumCalc; ///< checksum calculated by server
   bool q_CheckAtStartup;              ///< true: flashloader shall check checksum against flash content at startup
   bool q_CheckAfterFlashing;          ///< true: flashloader shall check checksum against flash content after flashing
};

//---------------------------------------------------------------------------
///Information about all checksum areas
class C_XFLChecksumAreas
{
public:
   stw_scl::SCLDynamicArray<C_XFLChecksumBlock> c_BlockConfig; ///< start / end address (not available if sector based)
   stw_scl::SCLDynamicArray<C_XFLChecksumArea> c_Areas; ///< array with individual checksum areas
   bool q_IsBlockBased; ///< false -> sector based
};

//---------------------------------------------------------------------------
///Defines how the server node shall be addressed during wakeup
enum E_XFLWakeupMode
{
   eXFL_WAKEUP_MODE_LID = 0,      ///< address node by its local ID
   eXFL_WAKEUP_MODE_SNR,          ///< address node by its serial-number
   eXFL_WAKEUP_MODE_LID_THEN_SNR, ///< address node by its local ID, if there are multiple nodes with the same local ID
                                  ///<   use the SNR to distinguish those
   eXFL_WAKEUP_MODE_NO_WAKEUP     ///< do not perform any wakeup (can be used if the node is already in wakeup mode)
};

//---------------------------------------------------------------------------
///Parameters for performing an ECU wakeup
class STW_DIAGLIB_EXPORT C_XFLWakeupParameters
{
public:
   C_XFLWakeupParameters(void);

   E_XFLWakeupMode e_WakeupMode;       ///< identify ECU by local ID or SNR

   bool q_SendResetRQ;                  ///< send reset request message ? true -> send; false -> don't send
   stw_can::T_STWCAN_Msg_TX t_ResetMsg; ///< reset request message (only used if q_SendResetRQ = true)

   bool q_SendFLASHRequired;             ///< true -> send "FLASH" at startup
   stw_types::uint32 u32_StartTimeMs;    ///< in ms (defines how long the "FLASH" command should be sent)
   stw_types::uint8  u8_FLASHIntervalMs; ///< interval between individual "FLASH" commands in ms.
                                         ///< set to 10 for most target nodes

   stw_types::uint8 u8_LocalID;  ///< target node's local ID (only required with e_WakeupMode == eXFL_WAKEUP_MODE_LID)
   stw_types::uint8 au8_SNR[6];  ///< target node's serial number (only required with e_WakeupMode == eXFL_FLASH_MODE_SNR)

   stw_scl::C_SCLString c_CompanyID; ///< target node's company ID (2,3 or 5 characters)
};

//---------------------------------------------------------------------------
///Higher level comfort layer on top of the flashloader protocol driver
class STW_DIAGLIB_EXPORT C_XFLActions : public C_XFLProtocol
{
private:
   bool m_AllowSTWCID(const C_XFLCompanyID & orc_ConfiguredCompanyID, const C_XFLCompanyID & orc_RespondedCompanyID);

   void m_ReadFingerPrintInformation(C_XFLInformationFromServer & orc_Information);
   void m_ReadDeviceInfo(C_XFLInformationFromServer & orc_Information);

   stw_types::sint32 m_WakeupLocalID(const C_XFLCompanyID & orc_CompanyID, const bool oq_MultiResponsesOK = false);
   stw_types::sint32 m_WakeupSNR(const C_XFLCompanyID & orc_CompanyID, const stw_types::uint8 (& orau8_SNR)[6],
                                 stw_types::uint8 & oru8_LocalID);
   stw_types::sint32 m_WakeupLocalIDAndSNR(const C_XFLCompanyID & orc_CompanyID,
                                           const stw_types::uint8 (& orau8_SNR)[6]);

protected:
public:
   C_XFLActions(void);

   stw_types::sint32 RequestNodeReset(const stw_can::T_STWCAN_Msg_TX * const opt_ResetMessage,
                                      const bool oq_SingleNode = true);

   stw_types::sint32 DivertStreamOnOffBBB(const bool oq_OnOff, const stw_types::uint8 ou8_TargetPosition,
                                          const stw_types::uint8 (& orau8_UserID)[2]);
   stw_types::sint32 DivertStreamOnOff(const bool oq_On, const C_XFLDivertParameters & orc_DivertParams,
                                       const bool oq_UseHopHandle, const stw_types::uint8 ou8_HopHandle);

   stw_types::sint32 PerformWakeup(const C_XFLWakeupParameters & orc_Parameters,
                                   stw_types::uint8 * const opu8_ActualLocalID = NULL);

   stw_types::sint32 ReadFlashInformation(C_XFLFlashInformation &orc_Information, stw_scl::C_SCLString & orc_ErrorText);
   stw_types::sint32 SearchNodes(const stw_scl::C_SCLString & orc_CompanyID, const stw_types::uint32 ou32_StartTime,
                                 const stw_types::uint8 ou8_FlashInterval,
                                 stw_scl::SCLDynamicArray<C_XFLFoundNode> & orc_FoundNodes);

   stw_types::sint32 ReadServerInformation(C_XFLInformationFromServer & orc_Info);
   stw_types::sint32 ReadServerSectorChecksumInformation(const stw_types::uint16 ou16_SectorCount,
                                                         C_XFLChecksumAreas & orc_ChecksumInformation);
   stw_types::sint32 ReadServerBlockChecksumInformation(C_XFLChecksumAreas & orc_ChecksumInformation);

   static stw_types::sint32 CompIDStringToStruct(const stw_scl::C_SCLString & orc_String, C_XFLCompanyID & orc_Data);
   static stw_types::sint32 CompIDStructToString(const C_XFLCompanyID & orc_Data, stw_scl::C_SCLString & orc_String);
   static stw_scl::C_SCLString SNRBytesToString(const stw_types::uint8 (& orau8_Data)[6], const bool oq_IncludeDots);
   static stw_types::sint32 SNRStringToBytes(const stw_scl::C_SCLString & orc_Text, stw_types::uint8 (& orau8_Data)[6]);

   static stw_scl::C_SCLString XFLProtocolErrorToText(const stw_types::sint32 os32_ReturnValue,
                                                      const stw_types::uint8 ou8_ErrorCode);
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif
