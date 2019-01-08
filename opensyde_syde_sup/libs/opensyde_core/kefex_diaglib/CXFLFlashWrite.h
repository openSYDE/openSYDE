//-----------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_XFLFlashWrite

   Class for performing the actual write flash procedure

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     xx.xx.20xx  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef CXFLFLASHWRITEH
#define CXFLFLASHWRITEH

#include "stwtypes.h"
#include "CXFLProtocol.h"
#include "CXFLHexFile.h"
#include "CXFLActions.h"
#include "CSCLString.h"
#include "DiagLib_config.h"
#include "CXFLFlashWriteParameters.h"

//---------------------------------------------------------------------------

namespace stw_diag_lib
{

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------
///Class for performing the actual write flash procedure
class STW_DIAGLIB_EXPORT C_XFLFlashWrite : public C_XFLActions
{
private:
   void m_ReportVerboseStatus(const stw_scl::C_SCLString & orc_StatusText);

   stw_types::sint32 m_SetUserDefinedSectors(const stw_scl::C_SCLString & orc_Sectors);
   stw_types::sint32 m_FlashESXLokalID(const stw_scl::C_SCLString & orc_DeviceID,
                                       const C_XFLFingerPrintSupportedIndexes & orc_FingerPrintIndexes,
                                       const C_XFLFlashWriteParameters & orc_Params,
                                       const stw_types::uint16 ou16_ProtocolVersion);
   stw_types::sint32 m_SetSectorsToErase(stw_hex_file::C_HexFile & orc_HexFile,
                                         const stw_scl::C_SCLString & orc_DeviceID,
                                         const C_XFLFlashWriteParameters & orc_Params,
                                         C_XFLFlashInformation & orc_FlashInfo,
                                         const stw_types::uint16 ou16_ProtocolVersion);
   stw_types::sint32 m_SetAutoSectors(stw_hex_file::C_HexFile & orc_HexFile, const bool oq_AllButProtected,
                                      C_XFLFlashInformation & orc_FlashInfo);

   void m_FlashMapStructToArray(const C_XFLFlashInformation & orc_FlashMapping,
                                  stw_scl::SCLDynamicArray<C_XFLFlashSector> & orc_Sectors) const;
   stw_types::sint32 m_WriteFingerPrintCheckSum(const bool oq_FingerPrintAvailable,
                                                const stw_types::uint32 ou32_Checksum);

   void m_InitProtocol(const C_XFLFlashWriteParameters & orc_Params);
   stw_types::sint32 m_Wakeup(const C_XFLWakeupParameters & orc_Params);

   stw_types::sint32 m_WriteFlashChecksums(const stw_types::uint8 ou8_Mode,
                                           const C_XFLWakeupParameters & orc_WakeupConfig);
   stw_types::sint32 m_WriteBlockBasedChecksums(void);

   stw_types::sint32 m_GetVersionNumber(stw_types::uint8 & oru8_Version, bool & orq_SectorBasedCRCsSupported,
                                        const bool oq_DivertStreamDesired);
   stw_types::sint32 m_WriteStartFingerPrint(const C_XFLFingerPrintSupportedIndexes & orc_FingerPrintIndexes);
   void m_FlashingFinished(const E_XFLFlashFinishedAction oe_Action);
   stw_types::sint32 m_ScanDeviceIDFromHexFile(const stw_types::uint32 ou32_StartAddress,
                                                 const bool oq_FailIfNotFoundAtStartAddress,
                                                 C_XFLHexFile & orc_HexFile, stw_scl::C_SCLString & orc_DeviceID) const;
   stw_types::sint32 m_DoDeviceIDXCheck(C_XFLHexFile & orc_HexFile, const stw_scl::C_SCLString & orc_DeviceID,
                                        const C_XFLFlashWriteParameters & orc_Params,
                                        const stw_types::uint16 ou16_ProtocolVersion);

   C_XFLImplementedServices mt_AvailableServices;

   stw_types::uint8 mu8_ActualLocalID;
   bool mq_VerboseMode;

   stw_scl::SCLDynamicArray<stw_types::uint8> mau8_SectorsToErase;

protected:
   ///inherit from this class and overload the following functions to implement user interaction if required
   //Used when feedback from application is required; only required with "ASK" configurations \n
   //return C_NO_ERR to ignore and continue
   virtual stw_types::sint32 TRG_UserInteraction(const E_XFLUserInteractionReason oe_Reason,
                                                 const stw_scl::C_SCLString & orc_MessageText,
                                                 stw_types::uint32 & oru32_AdditionalValue);
public:
   stw_types::sint32 ExecuteWrite(const C_XFLFlashWriteParameters & orc_Params);
   virtual ~C_XFLFlashWrite(void);
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
#endif
