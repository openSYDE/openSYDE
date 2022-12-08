//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_XFLFlashWrite

   Class for performing the actual write flash procedure

   \copyright   Copyright 2002 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CXFLFLASHWRITEH
#define CXFLFLASHWRITEH

#include "stwtypes.hpp"
#include "CXFLProtocol.hpp"
#include "CXFLHexFile.hpp"
#include "CXFLActions.hpp"
#include "C_SclString.hpp"
#include "DiagLib_config.hpp"
#include "CXFLFlashWriteParameters.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
{
//----------------------------------------------------------------------------------------------------------------------

/// User interaction reasons passed to TRG_UserInteraction by C_XFLDownload::Execute:
enum E_XFLUserInteractionReason
{
   /// let user know that the hex-file is invalid  \n
   /// used with C_XFLDownloadParameters::u8_IgnoreInvalidHexfileError = 2
   eXFL_USER_INTERACTION_REASON_HEXFILE_OVERLAP = 0,

   /// get device-id from hex-file failed \n
   /// used with C_XFLDownloadParameters::E_XFLDevTypeCheckGetIDFailReaction = ASK
   eXFL_USER_INTERACTION_REASON_GETID_FAIL      = 1,

   /// device-id from hex-file does not match the one of target \n
   /// used with C_XFLDownloadParameters::E_XFLDevTypeCheckMatchIDFailReaction = ASK
   eXFL_USER_INTERACTION_REASON_MATCHID_FAIL    = 2,

   /// flashing finished, ask for action \n
   /// used with C_XFLDownloadParameters::E_XFLFlashFinishedAction = XFLFlashFinishedActionAskUser \n
   /// opu32_AdditionalReturnValue out: action to perform (E_XFLFlashFinishedAction)
   eXFL_USER_INTERACTION_REASON_FINISHED        = 3
};

//----------------------------------------------------------------------------------------------------------------------
///Class for performing the actual write flash procedure
class STW_DIAGLIB_EXPORT C_XFLFlashWrite :
   public C_XFLActions
{
private:
   void m_ReportVerboseStatus(const stw::scl::C_SclString & orc_StatusText);

   int32_t m_SetUserDefinedSectors(const stw::scl::C_SclString & orc_Sectors);
   int32_t m_FlashESXLokalID(const stw::scl::C_SclString & orc_DeviceId,
                             const C_XFLFingerPrintSupportedIndexes & orc_FingerPrintIndexes,
                             const C_XFLFlashWriteParameters & orc_Params, const uint16_t ou16_ProtocolVersion);
   int32_t m_SetSectorsToErase(stw::hex_file::C_HexFile & orc_HexFile, const stw::scl::C_SclString & orc_DeviceId,
                               const C_XFLFlashWriteParameters & orc_Params, C_XFLFlashInformation & orc_FlashInfo,
                               const uint16_t ou16_ProtocolVersion);
   int32_t m_SetAutoSectors(stw::hex_file::C_HexFile & orc_HexFile, const bool oq_AllButProtected,
                            C_XFLFlashInformation & orc_FlashInfo);

   void m_FlashMapStructToArray(const C_XFLFlashInformation & orc_FlashMapping,
                                stw::scl::C_SclDynamicArray<C_XFLFlashSector> & orc_Sectors) const;
   int32_t m_WriteFingerPrintCheckSum(const bool oq_FingerPrintAvailable, const uint32_t ou32_Checksum);

   void m_InitProtocol(const C_XFLFlashWriteParameters & orc_Params);
   int32_t m_Wakeup(const C_XFLWakeupParameters & orc_Params);

   int32_t m_WriteFlashChecksums(const uint8_t ou8_Mode, const C_XFLWakeupParameters & orc_WakeupConfig);
   int32_t m_WriteBlockBasedChecksums(void);

   int32_t m_GetVersionNumber(uint8_t & oru8_Version, bool & orq_SectorBasedCRCsSupported,
                              const bool oq_DivertStreamDesired);
   int32_t m_WriteStartFingerPrint(const C_XFLFingerPrintSupportedIndexes & orc_FingerPrintIndexes);
   void m_FlashingFinished(const E_XFLFlashFinishedAction oe_Action);
   int32_t m_ScanDeviceIDFromHexFile(const uint32_t ou32_StartAddress, const bool oq_FailIfNotFoundAtStartAddress,
                                     C_XFLHexFile & orc_HexFile, stw::scl::C_SclString & orc_DeviceId) const;
   int32_t m_DoDeviceIDXCheck(C_XFLHexFile & orc_HexFile, const stw::scl::C_SclString & orc_DeviceId,
                              const C_XFLFlashWriteParameters & orc_Params, const uint16_t ou16_ProtocolVersion);

   C_XFLImplementedServices mt_AvailableServices;

   uint8_t mu8_ActualLocalID;
   bool mq_VerboseMode;

   stw::scl::C_SclDynamicArray<uint8_t> mau8_SectorsToErase;

protected:
   ///inherit from this class and overload the following functions to implement user interaction if required
   //Used when feedback from application is required; only required with "ASK" configurations \n
   //return C_NO_ERR to ignore and continue
   virtual int32_t TRG_UserInteraction(const E_XFLUserInteractionReason oe_Reason,
                                       const stw::scl::C_SclString & orc_MessageText, uint32_t & oru32_AdditionalValue);

public:
   int32_t ExecuteWrite(const C_XFLFlashWriteParameters & orc_Params);
   virtual ~C_XFLFlashWrite(void);
};

//----------------------------------------------------------------------------------------------------------------------
}
}

//----------------------------------------------------------------------------------------------------------------------
#endif
