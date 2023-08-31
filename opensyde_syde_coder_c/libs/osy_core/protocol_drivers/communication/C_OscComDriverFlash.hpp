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
#include "stwtypes.hpp"

#include "C_OscProtocolSerialNumber.hpp"
#include "C_OscComDriverProtocol.hpp"
#include "C_OscProtocolDriverOsy.hpp"

#include "C_OscFlashProtocolStwFlashloader.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComDriverFlash :
   public C_OscComDriverProtocol
{
public:
   ///collection of information that can be read from the flashloader
   class C_FlashloaderInformation
   {
   public:
      uint8_t au8_FlashloaderSoftwareVersion[3]; ///< Mmr
      uint8_t au8_FlashloaderProtocolVersion[3]; ///< Mmr
      uint8_t au8_ProtocolVersion[3];            ///< Mmr
      uint32_t u32_FlashCount;                   ///< how often was this device flashed already ?
      C_OscProtocolSerialNumber c_SerialNumber;  ///< serial number of node
                                                 // holds both variants: POS and FSN depending of
                                                 // c_AvailableFeatures.
                                                 // q_ExtendedSerialNumberModeImplemented
                                                 // and the manufacturer format
      uint32_t u32_EcuArticleNumber;             ///< article number of device
      stw::scl::C_SclString c_EcuHardwareVersionNumber;
      uint8_t au8_FlashFingerprintDate[3]; ///< last date of flashing yy.mm.dd
      uint8_t au8_FlashFingerprintTime[3]; ///< last time of flashing hh.mm.ss
      stw::scl::C_SclString c_FlashFingerprintUserName;

      C_OscProtocolDriverOsy::C_ListOfFeatures c_AvailableFeatures; ///< Available features of flashloader
      uint16_t u16_MaxNumberOfBlockLength;                          ///< maximum size of service the server can handle

      stw::scl::C_SclString GetEcuSerialNumber(void) const;
      stw::scl::C_SclString GetEcuSerialNumberFormatDescription(void) const;
   };

   //Description of types see C_OscDeviceDefinition::u32_FlashloaderResetWaitTimeXXX
   enum E_MinimumFlashloaderResetWaitTimeType
   {
      eNO_CHANGES_CAN = 0,
      eNO_CHANGES_ETHERNET,
      eNO_FUNDAMENTAL_COM_CHANGES_CAN,
      eNO_FUNDAMENTAL_COM_CHANGES_ETHERNET,
      eFUNDAMENTAL_COM_CHANGES_CAN,
      eFUNDAMENTAL_COM_CHANGES_ETHERNET
   };

   C_OscFlashProtocolStwFlashloader::PR_ReportProgress pr_XflReportProgress;
   void * pv_XflReportProgressInstance;

   C_OscComDriverFlash(const bool oq_RoutingActive, const bool oq_UpdateRoutingMode,
                       const C_OscFlashProtocolStwFlashloader::PR_ReportProgress opr_XflReportProgress,
                       void * const opv_XflReportProgressInstance);
   virtual ~C_OscComDriverFlash(void);

   virtual int32_t Init(const C_OscSystemDefinition & orc_SystemDefinition, const uint32_t ou32_ActiveBusIndex,
                        const std::vector<uint8_t> & orc_ActiveNodes,
                        stw::can::C_CanDispatcher * const opc_CanDispatcher, C_OscIpDispatcher * const opc_IpDispatcher,
                        C_OscSecurityPemDatabase * const opc_SecurityPemDb);

   int32_t InitCanAndSetCanBitrate(const uint32_t ou32_Bitrate);

   // openSYDE protocol configuration
   int32_t OsySetPollingTimeout(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint32_t ou32_TimeoutMs) const;
   int32_t OsyResetPollingTimeout(const C_OscProtocolDriverOsyNode & orc_ServerId) const;

   uint32_t GetMinimumFlashloaderResetWaitTime(const E_MinimumFlashloaderResetWaitTimeType oe_Type) const;
   int32_t GetMinimumFlashloaderResetWaitTime(const E_MinimumFlashloaderResetWaitTimeType oe_Type,
                                              const C_OscProtocolDriverOsyNode & orc_ServerId,
                                              uint32_t & oru32_TimeValue) const;

   // ETH specific functions for connect when no protocol and/or IP address is initialized
   int32_t EthConnectNode(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint8_t (&orau8_IpAddress)[4],
                          C_OscProtocolDriverOsy & orc_TemporaryProtocol, C_OscProtocolDriverOsyTpIp & orc_TpIp);
   static int32_t h_EthDisconnectNode(C_OscProtocolDriverOsy & orc_TemporaryProtocol);

   // openSYDE Services
   int32_t SendOsyBroadcastRequestProgramming(bool & orq_NotAccepted) const;
   int32_t SendOsyBroadcastEcuReset(const uint8_t ou8_ResetType) const;
   int32_t SendOsyCanBroadcastEnterPreProgrammingSession(void) const;
   int32_t SendOsyCanBroadcastEnterDefaultSession(void) const;
   int32_t SendOsyCanBroadcastReadSerialNumber(
      std::vector<C_OscProtocolDriverOsyTpCan::C_BroadcastReadEcuSerialNumberResults> & orc_Responses,
      std::vector<C_OscProtocolDriverOsyTpCan::C_BroadcastReadEcuSerialNumberExtendedResults> & orc_ExtendedResponses)
   const;
   int32_t SendOsyCanBroadcastSetNodeIdBySerialNumber(const C_OscProtocolSerialNumber & orc_SerialNumber,
                                                      const C_OscProtocolDriverOsyNode & orc_NewNodeId,
                                                      uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyCanBroadcastSetNodeIdBySerialNumberExtended(const C_OscProtocolSerialNumber & orc_SerialNumber,
                                                              const uint8_t ou8_SubNodeId,
                                                              const C_OscProtocolDriverOsyNode & orc_NewNodeId,
                                                              uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyEthBroadcastGetDeviceInformation(
      std::vector<C_OscProtocolDriverOsyTpIp::C_BroadcastGetDeviceInfoResults> & orc_ReadDeviceInfoResults,
      std::vector<C_OscProtocolDriverOsyTpIp::C_BroadcastGetDeviceInfoExtendedResults> & orc_ReadDeviceInfoExtendedResults)
   const;
   int32_t SendOsyEthBroadcastSetIpAddress(const C_OscProtocolSerialNumber &orc_SerialNumber,
                                           const uint8_t(&orau8_NewIpAddress)[4],
                                           const uint8_t(&orau8_NetMask)[4],
                                           const uint8_t(&orau8_DefaultGateway)[4],
                                           const C_OscProtocolDriverOsyNode &orc_NewNodeId,
                                           uint8_t(&orau8_ResponseIp)[4],
                                           uint8_t * const opu8_ErrorResult = NULL) const;
   int32_t SendOsyEthBroadcastSetIpAddressExtended(const C_OscProtocolSerialNumber &orc_SerialNumber,
                                                   const uint8_t(&orau8_NewIpAddress)[4],
                                                   const uint8_t(&orau8_NetMask)[4],
                                                   const uint8_t(&orau8_DefaultGateway)[4],
                                                   const C_OscProtocolDriverOsyNode &orc_NewNodeId,
                                                   const uint8_t ou8_SubNodeId,
                                                   uint8_t(&orau8_ResponseIp)[4],
                                                   uint8_t * const opu8_ErrorResult = NULL) const;

   int32_t SendOsyReadDeviceName(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                 stw::scl::C_SclString & orc_DeviceName, uint8_t * const opu8_NrCode = NULL);
   int32_t SendOsyReadSerialNumber(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                   C_OscProtocolSerialNumber & orc_SerialNumberExt, uint8_t * const opu8_NrCode = NULL);
   int32_t SendOsyReadSerialNumberExt(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                      C_OscProtocolSerialNumber & orc_SerialNumberExt,
                                      uint8_t * const opu8_NrCode = NULL);
   int32_t SendOsyReadActiveDiagnosticSession(const C_OscProtocolDriverOsyNode & orc_ServerId, uint8_t & oru8_SessionId,
                                              uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyReadAllFlashBlockData(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                        std::vector<C_OscProtocolDriverOsy::C_FlashBlockInfo> & orc_BlockInfo,
                                        uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyReadInformationFromFlashloader(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                                 C_FlashloaderInformation & orc_Information,
                                                 uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyCheckFlashMemoryAvailable(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                            const uint32_t ou32_StartAddress, const uint32_t ou32_Size,
                                            uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyWriteApplicationSoftwareFingerprint(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                                      const uint8_t (&orau8_Date)[3], const uint8_t (&orau8_Time)[3],
                                                      const stw::scl::C_SclString & orc_Username,
                                                      uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyRequestDownload(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint32_t ou32_StartAddress,
                                  const uint32_t ou32_Size, uint32_t & oru32_MaxBlockLength,
                                  uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyRequestFileTransfer(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                      const stw::scl::C_SclString & orc_FilePath, const uint32_t ou32_FileSize,
                                      uint32_t & oru32_MaxBlockLength, uint8_t * const opu8_NrCode = NULL) const;

   int32_t SendOsyTransferData(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint8_t ou8_BlockSequenceCounter,
                               std::vector<uint8_t> & orc_Data, uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyRequestTransferExitAddressBased(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                                  const bool oq_SendSignatureBlockAddress,
                                                  const uint32_t ou32_SignatureBlockAddress,
                                                  uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyRequestTransferExitFileBased(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                               const uint32_t ou32_CrcOverData,
                                               uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyRequestFileBasedTransferExitResult(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                                     stw::scl::C_SclString & orc_Result,
                                                     uint8_t * const opu8_NrCode = NULL) const;

   int32_t SendOsyRequestProgramming(const C_OscProtocolDriverOsyNode & orc_ServerId) const;
   int32_t SendOsyEcuReset(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint8_t ou8_ResetType) const;
   int32_t SendOsySetPreProgrammingMode(C_OscProtocolDriverOsy & orc_Protocol, const bool oq_SessionOnly,
                                        uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsySetPreProgrammingMode(const C_OscProtocolDriverOsyNode & orc_ServerId, const bool oq_SessionOnly,
                                        uint8_t * const opu8_NrCode = NULL);
   int32_t SendOsySetProgrammingMode(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                     const uint8_t * const opu8_SecurityLevel = NULL,
                                     uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsySetSecurityLevel(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint8_t ou8_Level,
                                   uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsySetBitrate(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint8_t ou8_ChannelIndex,
                             const uint32_t ou32_Bitrate, uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsySetIpAddressForChannel(C_OscProtocolDriverOsy & orc_Protocol, const uint8_t ou8_ChannelIndex,
                                         const uint8_t (&orau8_IpAddress)[4], const uint8_t (&orau8_NetMask)[4],
                                         const uint8_t (&orau8_DefaultGateway)[4], uint8_t * const opu8_NrCode = NULL);
   int32_t SendOsySetIpAddressForChannel(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                         const uint8_t ou8_ChannelIndex, const uint8_t (&orau8_IpAddress)[4],
                                         const uint8_t (&orau8_NetMask)[4], const uint8_t (&orau8_DefaultGateway)[4],
                                         uint8_t * const opu8_NrCode = NULL);
   int32_t SendOsySetNodeIdForChannel(C_OscProtocolDriverOsy & orc_Protocol, const uint8_t ou8_ChannelType,
                                      const uint8_t ou8_ChannelIndex, const C_OscProtocolDriverOsyNode & orc_NewNodeId,
                                      uint8_t * const opu8_NrCode = NULL);
   int32_t SendOsySetNodeIdForChannel(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint8_t ou8_ChannelType,
                                      const uint8_t ou8_ChannelIndex, const C_OscProtocolDriverOsyNode & orc_NewNodeId,
                                      uint8_t * const opu8_NrCode = NULL);

   int32_t SendOsyReadListOfFeatures(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                     C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ListOfFeatures,
                                     uint8_t * const opu8_NrCode = NULL) const;

   int32_t SendOsyReadCertificateSerialNumber(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                              std::vector<uint8_t> & orc_SerialNumber,
                                              uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyWriteSecurityKey(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                   const std::vector<uint8_t> & orc_PublicKeyModulus,
                                   const std::vector<uint8_t> & orc_PublicKeyExponent,
                                   const std::vector<uint8_t> & orc_CertificateSerialNumber,
                                   uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyReadSecurityActivation(const C_OscProtocolDriverOsyNode & orc_ServerId, bool & orq_SecurityOn,
                                         uint8_t & oru8_SecurityAlgorithm, uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyWriteSecurityActivation(const C_OscProtocolDriverOsyNode & orc_ServerId, const bool oq_SecurityOn,
                                          const uint8_t ou8_SecurityAlgorithm,
                                          uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyReadDebuggerEnabled(const C_OscProtocolDriverOsyNode & orc_ServerId, bool & orq_DebuggerEnabled,
                                      uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyWriteDebuggerEnabled(const C_OscProtocolDriverOsyNode & orc_ServerId, const bool oq_DebuggerEnabled,
                                       uint8_t * const opu8_NrCode = NULL) const;
   int32_t SendOsyFactoryModeMasterReset(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                         uint8_t * const opu8_NrCode = NULL) const;

   // STW Flashloader services
   int32_t SendStwRequestNodeReset(void);
   int32_t SendStwRequestNodeReset(const C_OscProtocolDriverOsyNode & orc_ServerId);
   int32_t SendStwSendFlash(const C_OscProtocolDriverOsyNode & orc_ServerId);
   int32_t SendStwSearchId(uint8_t(&orau8_LocalIds)[stw::diag_lib::XFL_NUM_DIFFERENT_LOCAL_IDS],
                           uint8_t & oru8_NodeFounds);
   int32_t SendStwWakeupLocalId(const C_OscProtocolDriverOsyNode & orc_ServerId, uint8_t * const opu8_NodesFound);
   int32_t SendStwWakeupLocalSerialNumber(const C_OscProtocolSerialNumber & orc_SerialNumber, uint8_t & oru8_LocalId);

   int32_t SendStwGetSerialNumbers(const C_OscProtocolDriverOsyNode & orc_ServerId, uint8_t * const opu8_SerialNumbers,
                                   const uint8_t ou8_NumMax, uint8_t & oru8_NumFound);
   int32_t SendStwGetDeviceId(const C_OscProtocolDriverOsyNode & orc_ServerId, stw::scl::C_SclString & orc_DeviceName);

   int32_t SendStwSetLocalId(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint8_t ou8_NewLocalId);
   int32_t SendStwSetBitrateCan(const C_OscProtocolDriverOsyNode & orc_ServerId, const uint32_t ou32_Bitrate);
   int32_t SendStwNetReset(const C_OscProtocolDriverOsyNode & orc_ServerId);
   int32_t SendStwNetReset(void);
   int32_t SendStwReadDeviceInformation(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                        stw::diag_lib::C_XFLInformationFromServer & orc_Information,
                                        stw::diag_lib::C_XFLChecksumAreas & orc_ChecksumInformation) const;
   int32_t SendStwDoFlash(const C_OscProtocolDriverOsyNode & orc_ServerId,
                          const stw::scl::C_SclString & orc_HexFilePath) const;

   void PrepareForDestructionFlash(void);

protected:
   virtual bool m_GetRoutingMode(C_OscRoutingCalculation::E_Mode & ore_Mode) const;
   virtual uint8_t m_GetRoutingSessionId(void) const;

   virtual bool m_IsRoutingSpecificNecessary(const C_OscNode & orc_Node) const;
   virtual int32_t m_StartRoutingSpecific(const uint32_t ou32_ActiveNode, const C_OscNode * const opc_Node,
                                          const C_OscRoutingRoutePoint & orc_LastNodeOfRouting,
                                          C_OscProtocolDriverOsy * const opc_ProtocolOsyOfLastNodeOfRouting,
                                          C_OscCanDispatcherOsyRouter ** const oppc_RoutingDispatcher);
   virtual void m_StopRoutingSpecific(const uint32_t ou32_ActiveNode);
   virtual bool m_CheckInterfaceForFunctions(const C_OscNodeComInterfaceSettings & orc_ComItfSettings) const;

   C_OscFlashProtocolStwFlashloader * m_GetStwFlashloaderProtocol(
      const C_OscProtocolDriverOsyNode & orc_ServerId) const;

private:
   //constants for STW flashloader:
   static const uint32_t mhu32_XFL_ID_TX = 0x51U;
   static const uint32_t mhu32_XFL_ID_RX = 0x52U;
   static const bool mhq_XFL_ID_XTD = false;

   //Avoid call
   C_OscComDriverFlash(const C_OscComDriverFlash &);
   C_OscComDriverFlash & operator =(const C_OscComDriverFlash &); //lint !e1511 //we want to hide the base function

   void m_InitFlashProtocolStw(C_OscFlashProtocolStwFlashloader * const opc_FlashProtocolStw,
                               const uint8_t ou8_LocalId);
   int32_t m_InitFlashProtocol(void);

   int32_t m_PrepareTemporaryOsyProtocol(const C_OscProtocolDriverOsyNode & orc_ServerId,
                                         C_OscProtocolDriverOsy & orc_OsyProtocol,
                                         C_OscProtocolDriverOsyTpCan & orc_CanTransportProtocol);
   int32_t m_GetStwResetMessage(const uint32_t ou32_NodeIndex, stw::can::T_STWCAN_Msg_TX & orc_Message) const;
   int32_t m_GetMinimumFlashloaderResetWaitTime(const E_MinimumFlashloaderResetWaitTimeType oe_Type,
                                                const uint32_t ou32_NodeIndex, uint32_t & oru32_TimeValue) const;

   static void mh_HandleWaitTime(void * const opv_Instance);
   void m_HandleWaitTime(void);

   std::vector<C_OscFlashProtocolStwFlashloader *> mc_StwFlashProtocols;
   stw::diag_lib::C_XFLCompanyID mc_CompanyId;

   const bool mq_RoutingActive;
   const bool mq_UpdateRoutingMode;
   static const uint16_t mhu16_STW_FLASHLOADER_PROTOCOL_VERSION_3_00 = 0x3000U;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
