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

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_OSCProtocolSerialNumber.h"
#include "C_OSCComDriverProtocol.h"
#include "C_OSCProtocolDriverOsy.h"

#include "C_OSCFlashProtocolStwFlashloader.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

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
      C_OSCProtocolSerialNumber c_SerialNumber;           ///< serial number of node
                                                          // holds both variants: POS and FSN depending of
                                                          // c_AvailableFeatures.
                                                          // q_ExtendedSerialNumberModeImplemented
                                                          // and the manufacturer format
      stw_types::uint32 u32_EcuArticleNumber;             ///< article number of device
      stw_scl::C_SCLString c_EcuHardwareVersionNumber;
      stw_types::uint8 au8_FlashFingerprintDate[3]; ///< last date of flashing yy.mm.dd
      stw_types::uint8 au8_FlashFingerprintTime[3]; ///< last time of flashing hh.mm.ss
      stw_scl::C_SCLString c_FlashFingerprintUserName;

      C_OSCProtocolDriverOsy::C_ListOfFeatures c_AvailableFeatures; ///< Available features of flashloader
      stw_types::uint16 u16_MaxNumberOfBlockLength;                 ///< maximum size of service the server can handle

      stw_scl::C_SCLString GetEcuSerialNumber(void) const;
      stw_scl::C_SCLString GetEcuSerialNumberFormatDescription(void) const;
   };

   //Description of types see C_OSCDeviceDefinition::u32_FlashloaderResetWaitTimeXXX
   enum E_MinimumFlashloaderResetWaitTimeType
   {
      eNO_CHANGES_CAN = 0,
      eNO_CHANGES_ETHERNET,
      eNO_FUNDAMENTAL_COM_CHANGES_CAN,
      eNO_FUNDAMENTAL_COM_CHANGES_ETHERNET,
      eFUNDAMENTAL_COM_CHANGES_CAN,
      eFUNDAMENTAL_COM_CHANGES_ETHERNET
   };

   C_OSCFlashProtocolStwFlashloader::PR_ReportProgress pr_XflReportProgress;
   void * pv_XflReportProgressInstance;

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

   // openSYDE protocol configuration
   stw_types::sint32 OsySetPollingTimeout(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                          const stw_types::uint32 ou32_TimeoutMs) const;
   stw_types::sint32 OsyResetPollingTimeout(const C_OSCProtocolDriverOsyNode & orc_ServerId) const;

   stw_types::uint32 GetMinimumFlashloaderResetWaitTime(const E_MinimumFlashloaderResetWaitTimeType oe_Type) const;
   stw_types::sint32 GetMinimumFlashloaderResetWaitTime(const E_MinimumFlashloaderResetWaitTimeType oe_Type,
                                                        const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                        stw_types::uint32 & oru32_TimeValue) const;

   // openSYDE Services
   stw_types::sint32 SendOsyBroadcastRequestProgramming(bool & orq_NotAccepted) const;
   stw_types::sint32 SendOsyBroadcastEcuReset(const stw_types::uint8 ou8_ResetType) const;
   stw_types::sint32 SendOsyCanBroadcastEnterPreProgrammingSession(void) const;
   stw_types::sint32 SendOsyCanBroadcastEnterDefaultSession(void) const;
   stw_types::sint32 SendOsyCanBroadcastReadSerialNumber(
      std::vector<C_OSCProtocolDriverOsyTpCan::C_BroadcastReadEcuSerialNumberResults> & orc_Responses,
      std::vector<C_OSCProtocolDriverOsyTpCan::C_BroadcastReadEcuSerialNumberExtendedResults> & orc_ExtendedResponses)
   const;
   stw_types::sint32 SendOsyCanBroadcastSetNodeIdBySerialNumber(const C_OSCProtocolSerialNumber & orc_SerialNumber,
                                                                const C_OSCProtocolDriverOsyNode & orc_NewNodeId)
   const;
   stw_types::sint32 SendOsyCanBroadcastSetNodeIdBySerialNumberExtended(
      const C_OSCProtocolSerialNumber & orc_SerialNumber, const stw_types::uint8 ou8_SubNodeId,
      const C_OSCProtocolDriverOsyNode & orc_NewNodeId) const;
   stw_types::sint32 SendOsyEthBroadcastGetDeviceInformation(
      std::vector<C_OSCProtocolDriverOsyTpIp::C_BroadcastGetDeviceInfoResults> & orc_ReadDeviceInfoResults,
      std::vector<C_OSCProtocolDriverOsyTpIp::C_BroadcastGetDeviceInfoExtendedResults> & orc_ReadDeviceInfoExtendedResults)
   const;
   stw_types::sint32 SendOsyEthBroadcastSetIpAddress(const C_OSCProtocolSerialNumber &orc_SerialNumber,
                                                     const stw_types::uint8(&orau8_NewIpAddress)[4],
                                                     const stw_types::uint8(&orau8_NetMask)[4],
                                                     const stw_types::uint8(&orau8_DefaultGateway)[4],
                                                     const C_OSCProtocolDriverOsyNode &orc_NewNodeId,
                                                     stw_types::uint8(&orau8_ResponseIp)[4],
                                                     stw_types::uint8 * const opu8_ErrorResult = NULL) const;
   stw_types::sint32 SendOsyEthBroadcastSetIpAddressExtended(const C_OSCProtocolSerialNumber &orc_SerialNumber,
                                                             const stw_types::uint8(&orau8_NewIpAddress)[4],
                                                             const stw_types::uint8(&orau8_NetMask)[4],
                                                             const stw_types::uint8(&orau8_DefaultGateway)[4],
                                                             const C_OSCProtocolDriverOsyNode &orc_NewNodeId,
                                                             const stw_types::uint8 ou8_SubNodeId,
                                                             stw_types::uint8(&orau8_ResponseIp)[4],
                                                             stw_types::uint8 * const opu8_ErrorResult = NULL) const;

   stw_types::sint32 SendOsyReadDeviceName(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                           stw_scl::C_SCLString & orc_DeviceName,
                                           stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 SendOsyReadSerialNumber(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                             C_OSCProtocolSerialNumber & orc_SerialNumberExt,
                                             stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 SendOsyReadSerialNumberExt(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                C_OSCProtocolSerialNumber & orc_SerialNumberExt,
                                                stw_types::uint8 * const opu8_NrCode = NULL);
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
   stw_types::sint32 SendOsyRequestFileBasedTransferExitResult(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                               stw_scl::C_SCLString & orc_Result,
                                                               stw_types::uint8 * const opu8_NrCode = NULL) const;

   stw_types::sint32 SendOsyRequestProgramming(const C_OSCProtocolDriverOsyNode & orc_ServerId) const;
   stw_types::sint32 SendOsyEcuReset(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                     const stw_types::uint8 ou8_ResetType) const;
   stw_types::sint32 SendOsySetPreProgrammingMode(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                  const bool oq_SessionOnly,
                                                  stw_types::uint8 * const opu8_NrCode = NULL)  const;
   stw_types::sint32 SendOsySetProgrammingMode(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                               stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsySetSecurityLevel(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                             const stw_types::uint8 ou8_Level) const;
   stw_types::sint32 SendOsySetBitrate(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                       const stw_types::uint8 ou8_ChannelIndex, const stw_types::uint32 ou32_Bitrate,
                                       stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsySetIpAddressForChannel(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                   const stw_types::uint8 ou8_ChannelIndex,
                                                   const stw_types::uint8 (&orau8_IpAddress)[4],
                                                   const stw_types::uint8 (&orau8_NetMask)[4],
                                                   const stw_types::uint8 (&orau8_DefaultGateway)[4],
                                                   stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 SendOsySetNodeIdForChannel(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                const stw_types::uint8 ou8_ChannelType,
                                                const stw_types::uint8 ou8_ChannelIndex,
                                                const C_OSCProtocolDriverOsyNode & orc_NewNodeId,
                                                stw_types::uint8 * const opu8_NrCode = NULL) const;

   stw_types::sint32 SendOsyReadListOfFeatures(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                               C_OSCProtocolDriverOsy::C_ListOfFeatures & orc_ListOfFeatures,
                                               stw_types::uint8 * const opu8_NrCode = NULL) const;

   // STW Flashloader services
   stw_types::sint32 SendStwRequestNodeReset(void);
   stw_types::sint32 SendStwRequestNodeReset(const C_OSCProtocolDriverOsyNode & orc_ServerId);
   stw_types::sint32 SendStwSendFlash(const C_OSCProtocolDriverOsyNode & orc_ServerId);
   stw_types::sint32 SendStwSearchId(stw_types::uint8(&orau8_LocalIDs)[stw_diag_lib::XFL_NUM_DIFFERENT_LOCAL_IDS],
                                     stw_types::uint8 & oru8_NodeFounds);
   stw_types::sint32 SendStwWakeupLocalId(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                          stw_types::uint8 * const opu8_NodesFound);
   stw_types::sint32 SendStwWakeupLocalSerialNumber(const C_OSCProtocolSerialNumber & orc_SerialNumber,
                                                    stw_types::uint8 & oru8_LocalId);

   stw_types::sint32 SendStwGetSerialNumbers(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                             stw_types::uint8 * const opu8_SerialNumbers,
                                             const stw_types::uint8 ou8_NumMax, stw_types::uint8 & oru8_NumFound);
   stw_types::sint32 SendStwGetDeviceId(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                        stw_scl::C_SCLString & orc_DeviceName);

   stw_types::sint32 SendStwSetLocalId(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                       const stw_types::uint8 ou8_NewLocalId);
   stw_types::sint32 SendStwSetBitrateCan(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                          const stw_types::uint32 ou32_Bitrate);
   stw_types::sint32 SendStwNetReset(const C_OSCProtocolDriverOsyNode & orc_ServerId);
   stw_types::sint32 SendStwNetReset(void);
   stw_types::sint32 SendStwReadDeviceInformation(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                  stw_diag_lib::C_XFLInformationFromServer & orc_Information,
                                                  stw_diag_lib::C_XFLChecksumAreas & orc_ChecksumInformation) const;
   stw_types::sint32 SendStwDoFlash(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                    const stw_scl::C_SCLString & orc_HexFilePath) const;

   void PrepareForDestructionFlash(void);

protected:
   virtual bool m_GetRoutingMode(C_OSCRoutingCalculation::E_Mode & ore_Mode) const;
   virtual stw_types::uint8 m_GetRoutingSessionId(void) const;

   virtual bool m_IsRoutingSpecificNecessary(const C_OSCNode & orc_Node) const;
   virtual stw_types::sint32 m_StartRoutingSpecific(const stw_types::uint32 ou32_ActiveNode,
                                                    const C_OSCNode * const opc_Node,
                                                    const C_OSCRoutingRoutePoint & orc_LastNodeOfRouting,
                                                    C_OSCProtocolDriverOsy * const opc_ProtocolOsyOfLastNodeOfRouting,
                                                    C_OSCCanDispatcherOsyRouter ** const oppc_RoutingDispatcher);
   virtual void m_StopRoutingSpecific(const stw_types::uint32 ou32_ActiveNode);
   virtual bool m_CheckInterfaceForFunctions(const C_OSCNodeComInterfaceSettings & orc_ComItfSettings) const;

   C_OSCFlashProtocolStwFlashloader * m_GetStwFlashloaderProtocol(
      const C_OSCProtocolDriverOsyNode & orc_ServerId) const;

private:
   //constants for STW flashloader:
   static const stw_types::uint32 mhu32_XFL_ID_TX = 0x51U;
   static const stw_types::uint32 mhu32_XFL_ID_RX = 0x52U;
   static const bool mhq_XFL_ID_XTD = false;

   //Avoid call
   C_OSCComDriverFlash(const C_OSCComDriverFlash &);
   C_OSCComDriverFlash & operator =(const C_OSCComDriverFlash &); //lint !e1511 //we want to hide the base function

   void m_InitFlashProtocolStw(C_OSCFlashProtocolStwFlashloader * const opc_FlashProtocolStw,
                               const stw_types::uint8 ou8_LocalId);
   stw_types::sint32 m_InitFlashProtocol(void);

   stw_types::sint32 m_PrepareTemporaryOsyProtocol(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                   C_OSCProtocolDriverOsy & orc_OsyProtocol,
                                                   C_OSCProtocolDriverOsyTpCan & orc_CanTransportProtocol);
   stw_types::sint32 m_GetStwResetMessage(const stw_types::uint32 ou32_NodeIndex,
                                          stw_can::T_STWCAN_Msg_TX & orc_Message) const;
   stw_types::sint32 m_GetMinimumFlashloaderResetWaitTime(const E_MinimumFlashloaderResetWaitTimeType oe_Type,
                                                          const stw_types::uint32 ou32_NodeIndex,
                                                          stw_types::uint32 & oru32_TimeValue) const;

   static void mh_HandleWaitTime(void * const opv_Instance);
   void m_HandleWaitTime(void);

   std::vector<C_OSCFlashProtocolStwFlashloader *> mc_StwFlashProtocols;
   stw_diag_lib::C_XFLCompanyID mc_CompanyId;

   const bool mq_RoutingActive;
   static const stw_types::uint16 mhu16_STW_FLASHLOADER_PROTOCOL_VERSION_3_00 = 0x3000U;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
