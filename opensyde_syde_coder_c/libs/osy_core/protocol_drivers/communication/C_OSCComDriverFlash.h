//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Core communication driver for flashloader protocols (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSYCOMDRIVERFLASH_H
#define C_OSYCOMDRIVERFLASH_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

#include "C_OSCComDriverProtocol.h"

#include "C_OSCFlashProtocolStwFlashloader.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCComDriverFlash :
   public C_OSCComDriverProtocol
{
public:
   ///collection of information that can be read from the flashloader
   class C_FlashloaderInformation
   {
   public:
      stw_types::uint8 au8_FlashloaderSoftwareVersion[3]; ///< Mmr
      stw_types::uint8 au8_FlashloaderProtocolVersion[3]; ///< Mmr
      stw_types::uint8 au8_ProtocolVersion[3];            ///< Mmr
      stw_types::uint32 u32_FlashCount;                   ///< how often was this device flashed already ?
      stw_types::uint8 au8_EcuSerialNumber[6];            ///< BCD format
      stw_types::uint32 u32_EcuArticleNumber;             ///< article number of device
      stw_scl::C_SCLString c_EcuHardwareVersionNumber;
      stw_types::uint8 au8_FlashFingerprintDate[3]; ///< last date of flashing yy.mm.dd
      stw_types::uint8 au8_FlashFingerprintTime[3]; ///< last time of flashing hh.mm.ss
      stw_scl::C_SCLString c_FlashFingerprintUserName;
   };

   C_OSCFlashProtocolStwFlashloader::PR_ReportProgress mpr_XflReportProgress;
   void * mpv_XflReportProgressInstance;

   C_OSCComDriverFlash(const bool oq_RoutingActive,
                       const C_OSCFlashProtocolStwFlashloader::PR_ReportProgress opr_XflReportProgress,
                       void * const opv_XflReportProgressInstance);
   virtual ~C_OSCComDriverFlash(void);

   virtual stw_types::sint32 Init(const C_OSCSystemDefinition & orc_SystemDefinition,
                                  const stw_types::uint32 ou32_ActiveBusIndex,
                                  const std::vector<stw_types::uint8> & orc_ActiveNodes,
                                  stw_can::C_CAN_Dispatcher * const opc_CanDispatcher,
                                  C_OSCIpDispatcher * const opc_IpDispatcher);

   stw_types::sint32 InitCanAndSetCanBitrate(const stw_types::uint32 ou32_Bitrate);

   // openSYDE protocol configuration (TBD: where and how do we set the timeout ?)
   stw_types::sint32 OsySetPollingTimeout(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                          const stw_types::uint32 ou32_TimeoutMs) const;
   stw_types::sint32 OsyResetPollingTimeout(const C_OSCProtocolDriverOsyNode & orc_ServerId) const;

   // openSYDE Services
   stw_types::sint32 SendOsyBroadcastRequestProgramming(bool & orq_NotAccepted) const;
   stw_types::sint32 SendOsyBroadcastEcuReset(const stw_types::uint8 ou8_ResetType) const;
   stw_types::sint32 SendOsyCanBroadcastEnterPreProgrammingSession(void) const;
   stw_types::sint32 SendOsyCanBroadcastEnterDefaultSession(void) const;
   stw_types::sint32 SendOsyCanBroadcastReadSerialNumber(
      std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyTpCan::C_BroadcastReadEcuSerialNumberResults> & orc_Responses)
   const;
   stw_types::sint32 SendOsyCanBroadcastSetNodeIdBySerialNumber(const stw_types::uint8 (&orau8_SerialNumber)[6],
                                                                const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_NewNodeId)
   const;
   stw_types::sint32 SendOsyEthBroadcastGetDeviceInformation(
      std::vector<C_OSCProtocolDriverOsyTpIp::C_BroadcastGetDeviceInfoResults> & orc_ReadDeviceInfoResults) const;
   stw_types::sint32 SendOsyEthBroadcastSetIpAddress(const stw_types::uint8(&orau8_SerialNumber)[6],
                                                     const stw_types::uint8(&orau8_NewIpAddress)[4],
                                                     const stw_types::uint8(&orau8_NetMask)[4],
                                                     const stw_types::uint8(&orau8_DefaultGateway)[4],
                                                     const C_OSCProtocolDriverOsyNode &orc_NewNodeId,
                                                     stw_types::uint8(&orau8_ResponseIp)[4],
                                                     stw_types::uint8 * const opu8_ErrorResult = NULL) const;

   stw_types::sint32 SendOsyReadDeviceName(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                           stw_scl::C_SCLString & orc_DeviceName,
                                           stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 SendOsyReadSerialNumber(const C_OSCProtocolDriverOsyNode &orc_ServerId,
                                             stw_types::uint8(
                                                &orau8_SerialNumber)[6], stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 SendOsyReadActiveDiagnosticSession(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                        stw_types::uint8 & oru8_SessionId,
                                                        stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsyReadAllFlashBlockData(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                  std::vector<C_OSCProtocolDriverOsy::C_FlashBlockInfo> & orc_BlockInfo,
                                                  stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsyReadInformationFromFlashloader(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                           C_FlashloaderInformation & orc_Information,
                                                           stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsyCheckFlashMemoryAvailable(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                      const stw_types::uint32 ou32_StartAddress,
                                                      const stw_types::uint32 ou32_Size,
                                                      stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsyWriteApplicationSoftwareFingerprint(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                                const stw_types::uint8 (&orau8_Date)[3],
                                                                const stw_types::uint8 (&orau8_Time)[3],
                                                                const stw_scl::C_SCLString & orc_Username,
                                                                stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsyRequestDownload(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                            const stw_types::uint32 ou32_StartAddress,
                                            const stw_types::uint32 ou32_Size, stw_types::uint32 & oru32_MaxBlockLength,
                                            stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsyRequestFileTransfer(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                const stw_scl::C_SCLString & orc_FilePath,
                                                const stw_types::uint32 ou32_FileSize,
                                                stw_types::uint32 & oru32_MaxBlockLength,
                                                stw_types::uint8 * const opu8_NrCode = NULL) const;

   stw_types::sint32 SendOsyTransferData(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                         const stw_types::uint8 ou8_BlockSequenceCounter,
                                         std::vector<stw_types::uint8> & orc_Data,
                                         stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsyRequestTransferExitAddressBased(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                            const bool oq_SendSignatureBlockAddress,
                                                            const stw_types::uint32 ou32_SignatureBlockAddress,
                                                            stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsyRequestTransferExitFileBased(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                         const stw_types::uint32 ou32_CrcOverData,
                                                         stw_types::uint8 * const opu8_NrCode = NULL) const;

   stw_types::sint32 SendOsyRequestProgramming(
      const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId) const;
   stw_types::sint32 SendOsyEcuReset(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                     const stw_types::uint8 ou8_ResetType) const;
   stw_types::sint32 SendOsySetPreProgrammingMode(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                  const bool oq_SessionOnly,
                                                  stw_types::uint8 * const opu8_NrCode = NULL)  const;
   stw_types::sint32 SendOsySetProgrammingMode(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                               stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsySetSecurityLevel(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                             const stw_types::uint8 ou8_Level) const;
   stw_types::sint32 SendOsySetBitrate(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                       const stw_types::uint8 ou8_ChannelIndex, const stw_types::uint32 ou32_Bitrate,
                                       stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsySetIpAddressForChannel(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                   const stw_types::uint8 ou8_ChannelIndex,
                                                   const stw_types::uint8 (&orau8_IpAddress)[4],
                                                   const stw_types::uint8 (&orau8_NetMask)[4],
                                                   const stw_types::uint8 (&orau8_DefaultGateway)[4],
                                                   stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsySetNodeIdForChannel(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                const stw_types::uint8 ou8_ChannelType,
                                                const stw_types::uint8 ou8_ChannelIndex,
                                                const C_OSCProtocolDriverOsyNode & orc_NewNodeId,
                                                stw_types::uint8 * const opu8_NrCode = NULL) const;

   // STW Flashloader services
   stw_types::sint32 SendStwRequestNodeReset(void);
   stw_types::sint32 SendStwRequestNodeReset(const C_OSCProtocolDriverOsyNode & orc_ServerId);
   stw_types::sint32 SendStwSendFlash(const C_OSCProtocolDriverOsyNode & orc_ServerId);
   stw_types::sint32 SendStwSearchId(stw_types::uint8(&orau8_LocalIDs)[stw_diag_lib::XFL_NUM_DIFFERENT_LOCAL_IDS],
                                     stw_types::uint8 & oru8_NodeFounds);
   stw_types::sint32 SendStwWakeupLocalId(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                          stw_types::uint8 * const opu8_NodesFound);
   stw_types::sint32 SendStwWakeupLocalSerialNumber(const stw_types::uint8 (&orau8_SerialNumber)[6],
                                                    stw_types::uint8 & oru8_LocalId);

   stw_types::sint32 SendStwGetSerialNumbers(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                             stw_types::uint8 * const opu8_SerialNumbers,
                                             const stw_types::uint8 ou8_NumMax, stw_types::uint8 & oru8_NumFound);
   stw_types::sint32 SendStwGetDeviceId(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                        stw_scl::C_SCLString & orc_DeviceName);

   stw_types::sint32 SendStwSetLocalId(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                       const stw_types::uint8 ou8_NewLocalId);
   stw_types::sint32 SendStwSetBitrateCan(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                          const stw_types::uint32 ou32_Bitrate);
   stw_types::sint32 SendStwNetReset(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId);
   stw_types::sint32 SendStwNetReset(void);
   stw_types::sint32 SendStwReadDeviceInformation(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                  stw_diag_lib::C_XFLInformationFromServer & orc_Information,
                                                  stw_diag_lib::C_XFLChecksumAreas & orc_ChecksumInformation) const;
   stw_types::sint32 SendStwDoFlash(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                    const stw_scl::C_SCLString & orc_HexFilePath) const;

   void PrepareForDestructionFlash(void);

protected:
   virtual bool m_GetRoutingMode(stw_opensyde_core::C_OSCRoutingCalculation::E_Mode & ore_Mode) const;
   virtual stw_types::uint8 m_GetRoutingSessionId(void) const;

   virtual bool m_IsRoutingSpecificNecessary(const stw_opensyde_core::C_OSCNode & orc_Node) const;
   virtual stw_types::sint32 m_StartRoutingSpecific(const stw_types::uint32 ou32_ActiveNode,
                                                    const stw_opensyde_core::C_OSCNode * const opc_Node,
                                                    const stw_opensyde_core::C_OSCRoutingRoutePoint & orc_LastNodeOfRouting, C_OSCProtocolDriverOsy * const opc_ProtocolOsyOfLastNodeOfRouting, stw_opensyde_core::C_OSCCanDispatcherOsyRouter ** const oppc_RoutingDispatcher);
   virtual void m_StopRoutingSpecific(const stw_types::uint32 ou32_ActiveNode);
   virtual bool m_CheckInterfaceForFunctions(const C_OSCNodeComInterfaceSettings & orc_ComItfSettings) const;

   stw_opensyde_core::C_OSCFlashProtocolStwFlashloader * m_GetStwFlashloaderProtocol(
      const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId) const;

private:
   //constants for STW flashloader:
   static const stw_types::uint32 mhu32_XFL_ID_TX = 0x51U;
   static const stw_types::uint32 mhu32_XFL_ID_RX = 0x52U;
   static const bool mhq_XFL_ID_XTD = false;

   //Avoid call
   C_OSCComDriverFlash(const C_OSCComDriverFlash &);
   C_OSCComDriverFlash & operator =(const C_OSCComDriverFlash &);

   void m_InitFlashProtocolStw(stw_opensyde_core::C_OSCFlashProtocolStwFlashloader * const opc_FlashProtocolStw,
                               const stw_types::uint8 ou8_LocalId);
   stw_types::sint32 m_InitFlashProtocol(void);

   stw_types::sint32 m_PrepareTemporaryOsyProtocol(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                   stw_opensyde_core::C_OSCProtocolDriverOsy & orc_OsyProtocol,
                                                   stw_opensyde_core::C_OSCProtocolDriverOsyTpCan & orc_CanTransportProtocol);
   stw_types::sint32 m_GetStwResetMessage(const stw_types::uint32 ou32_NodeIndex,
                                          stw_can::T_STWCAN_Msg_TX & orc_Message) const;

   std::vector<stw_opensyde_core::C_OSCFlashProtocolStwFlashloader *> mc_StwFlashProtocols;
   stw_diag_lib::C_XFLCompanyID mc_CompanyId;

   const bool mq_RoutingActive;
   static const stw_types::uint16 mhu16_STW_FLASHLOADER_PROTOCOL_VERSION_3_00 = 0x3000U;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
