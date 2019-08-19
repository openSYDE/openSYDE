//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol driver

   \class       stw_opensyde_core::C_OSCProtocolDriverOsy
   \brief       openSYDE protocol driver

   Implements the services as defined for the openSYDE protocol.

   General sequence for "polled" services:
   - send request
   - call Cycle function of installed transport protocol
   - inbetween calls rescind CPU time to other threads
   - "wait" until we get a response or reach the timeout

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROTOCOLDRIVEROSYH
#define C_OSCPROTOCOLDRIVEROSYH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.h"
#include "C_OSCProtocolDriverOsyTpBase.h"
#include "stw_can.h" //for CAN message type

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCProtocolDriverOsy
{
private:
   typedef void (* PR_OsyTunnelCanMessageReceived)(void * const opv_Instance, const stw_types::uint8 ou8_CanChannel,
                                                   const stw_can::T_STWCAN_Msg_RX & orc_CanMessage);
   typedef void (* PR_OsyHandleWaitTime)(void * const opv_Instance);

   stw_tgl::C_TGLCriticalSection mc_LockReception; ///< for locking reception handling

   PR_OsyTunnelCanMessageReceived mpr_OnOsyTunnelCanMessageReceived;
   void * mpv_OnAsyncTunnelCanMessageInstance;
   PR_OsyHandleWaitTime mpr_OnOsyWaitTime;
   void * mpv_OnOsyWaitTimeInstance;

   C_OSCProtocolDriverOsyTpBase * mpc_TransportProtocol; ///< installed transport protocol

   C_OSCProtocolDriverOsyNode mc_ClientId; ///< our own ID
   C_OSCProtocolDriverOsyNode mc_ServerId; ///< ID of server we are connected to

   //timeouts:
   stw_types::uint32 mu32_TimeoutPollingMs; ///< timeout used when "polling" services

   //maximum service size including header (used in WriteMemoryByAddress):
   stw_types::uint16 mu16_MaxServiceSize;

   void m_LogServiceError(const stw_scl::C_SCLString & orc_Service, const stw_types::sint32 os32_ReturnCode,
                          const stw_types::uint8 ou8_NrCode) const;

   //check for exactly one incoming non-async service:
   stw_types::sint32 m_Cycle(const bool oq_CheckForSpecificServiceId = false,
                             const stw_types::uint8 ou8_ExpectedServiceId = 0xFFU,
                             C_OSCProtocolDriverOsyService * const opc_ReceivedService = NULL);
   //poll for exactly one incoming service:
   stw_types::sint32 m_PollForSpecificServiceResponse(const stw_types::uint8 ou8_ExpectedServiceId,
                                                      const stw_types::uint16 ou16_ExpectedSize,
                                                      C_OSCProtocolDriverOsyService & orc_Service,
                                                      stw_types::uint8 & oru8_NrCode,
                                                      const bool oq_ExactSizeExpected = true, const std::vector<stw_types::uint8> * const opc_ExpectedErrData =
                                                         NULL);
   stw_types::sint32 m_ReadDataByIdentifier(const stw_types::uint16 ou16_Identifier,
                                            const stw_types::uint16 ou16_ExpectedPayloadSize,
                                            const bool oq_ExactSizeExpected,
                                            std::vector<stw_types::uint8> & orc_ReadData,
                                            stw_types::uint8 & oru8_NrCode);
   stw_types::sint32 m_WriteDataByIdentifier(const stw_types::uint16 ou16_Identifier,
                                             std::vector<stw_types::uint8> & orc_WriteData,
                                             stw_types::uint8 & oru8_NrCode);
   stw_types::sint32 m_PackDataPoolIdentifier(const stw_types::uint8 ou8_DataPoolIndex,
                                              const stw_types::uint16 ou16_ListIndex,
                                              const stw_types::uint16 ou16_ElementIndex,
                                              stw_types::uint8(&orau8_PackedId)[3]) const;
   void m_UnpackDataPoolIdentifier(const stw_types::uint8(&orau8_PackedId)[3], stw_types::uint8 & oru8_DataPoolIndex,
                                   stw_types::uint16 & oru16_ListIndex, stw_types::uint16 & oru16_ElementIndex) const;
   stw_types::sint32 m_RoutineControl(const stw_types::uint16 ou16_RoutineIdentifier,
                                      const stw_types::uint8 ou8_SubFunction,
                                      const std::vector<stw_types::uint8> & orc_SendData,
                                      const stw_types::uint16 ou16_ExpectedPayloadSize, const bool oq_ExactSizeExpected,
                                      std::vector<stw_types::uint8> & orc_ReadData, stw_types::uint8 & oru8_NrCode,
                                      const bool oq_CanTransferWithoutFlowControl = false);
   stw_types::sint32 m_SecurityAccess(const stw_types::uint8 ou8_SubFunction,
                                      const std::vector<stw_types::uint8> & orc_SendData,
                                      const stw_types::uint16 ou16_SendPayloadSize,
                                      const stw_types::uint16 ou16_ExpectedPayloadSize,
                                      std::vector<stw_types::uint8> & orc_ReadData, stw_types::uint8 & oru8_NrCode);

   stw_types::sint32 m_HandleAsyncResponse(const C_OSCProtocolDriverOsyService & orc_ReceivedService);
   stw_types::sint32 m_HandleAsyncOsyReadDataPoolDataEvent(const C_OSCProtocolDriverOsyService & orc_ReceivedService);
   stw_types::sint32 m_HandleAsyncOsyReadDataPoolDataErrorEvent(
      const C_OSCProtocolDriverOsyService & orc_ReceivedService);
   stw_types::sint32 m_HandleAsyncOsyTunnelCanMessagesEvent(const C_OSCProtocolDriverOsyService & orc_ReceivedService);

   static void mh_ConvertVariableToNecessaryBytes(const stw_types::uint32 ou32_Variable,
                                                  std::vector<stw_types::uint8> & orc_Bytes);

   //service IDs:
   static const stw_types::uint8 mhu8_OSY_SI_DIAGNOSTIC_SESSION_CONTROL = 0x10U;
   static const stw_types::uint8 mhu8_OSY_SI_ECU_RESET                  = 0x11U;
   static const stw_types::uint8 mhu8_OSY_SI_READ_DATA_BY_IDENTIFIER    = 0x22U;
   static const stw_types::uint8 mhu8_OSY_SI_READ_MEMORY_BY_ADDRESS     = 0x23U;
   static const stw_types::uint8 mhu8_OSY_SI_SECURITY_ACCESS            = 0x27U;
   static const stw_types::uint8 mhu8_OSY_SI_WRITE_DATA_BY_IDENTIFIER   = 0x2EU;
   static const stw_types::uint8 mhu8_OSY_SI_ROUTINE_CONTROL            = 0x31U;
   static const stw_types::uint8 mhu8_OSY_SI_REQUEST_DOWNLOAD           = 0x34U;
   static const stw_types::uint8 mhu8_OSY_SI_TRANSFER_DATA              = 0x36U;
   static const stw_types::uint8 mhu8_OSY_SI_REQUEST_TRANSFER_EXIT      = 0x37U;
   static const stw_types::uint8 mhu8_OSY_SI_REQUEST_FILE_TRANSFER      = 0x38U;
   static const stw_types::uint8 mhu8_OSY_SI_WRITE_MEMORY_BY_ADDRESS    = 0x3DU;
   static const stw_types::uint8 mhu8_OSY_SI_TESTER_PRESENT             = 0x3EU;
   static const stw_types::uint8 mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN = 0xBAU;
   static const stw_types::uint8 mhu8_OSY_SI_READ_DATA_POOL_DATA_BY_ID  = 0xBBU;
   static const stw_types::uint8 mhu8_OSY_SI_WRITE_DATA_POOL_DATA_BY_ID = 0xBCU;
   static const stw_types::uint8 mhu8_OSY_SI_READ_SERIAL_NUMBER         = 0xBDU;
   static const stw_types::uint8 mhu8_OSY_SI_TUNNEL_CAN_MESSAGE         = 0xBEU;

   //negative response ID:
   static const stw_types::uint8 mhu8_OSY_NR_SI = 0x7FU;

   //data identifiers:
   static const stw_types::uint16 mhu16_OSY_DI_BOOT_SOFTWARE_IDENTIFICATION = 0xF180U;
   static const stw_types::uint16 mhu16_OSY_DI_APPLICATION_SOFTWARE_FINGERPRINT = 0xF184U;
   static const stw_types::uint16 mhu16_OSY_DI_ACTIVE_DIAGNOSTIC_SESSION   = 0xF186U;
   static const stw_types::uint16 mhu16_OSY_DI_ECU_SERIAL_NUMBER           = 0xF18CU;
   static const stw_types::uint16 mhu16_OSY_DI_SYS_SUPPLIER_ECU_HW_NUMBER  = 0xF192U;
   static const stw_types::uint16 mhu16_OSY_DI_SYS_SUPPLIER_ECU_HW_VERSION = 0xF193U;
   static const stw_types::uint16 mhu16_OSY_DI_LIST_OF_FEATURES            = 0xA800U;
   static const stw_types::uint16 mhu16_OSY_DI_MAX_NUMBER_OF_BLOCK_LENGTH  = 0xA801U;
   static const stw_types::uint16 mhu16_OSY_DI_DATARATE_1                  = 0xA810U;
   static const stw_types::uint16 mhu16_OSY_DI_DATARATE_2                  = 0xA811U;
   static const stw_types::uint16 mhu16_OSY_DI_DATARATE_3                  = 0xA812U;
   static const stw_types::uint16 mhu16_OSY_DI_PROTOCOL_VERSION            = 0xA813U;
   static const stw_types::uint16 mhu16_OSY_DI_PROTOCOL_DRIVER_IMPLEMENTATION_VERSION = 0xA814U;
   static const stw_types::uint16 mhu16_OSY_DI_FLASHLOADER_PROTOCOL_VERSION = 0xA815U;
   static const stw_types::uint16 mhu16_OSY_DI_MAX_NUM_ASYNC               = 0xA817U;
   static const stw_types::uint16 mhu16_OSY_DI_FLASH_COUNT                 = 0xA819U;
   static const stw_types::uint16 mhu16_OSY_DI_DEVICE_NAME                 = 0xA81AU;
   static const stw_types::uint16 mhu16_OSY_DI_APPLICATION_NAME            = 0xA81BU;
   static const stw_types::uint16 mhu16_OSY_DI_APPLICATION_VERSION         = 0xA81CU;
   //routine identifiers
   static const stw_types::uint16 mhu16_OSY_RC_SID_ROUTE_DIAGNOSIS_COMMUNICATION     = 0x0202U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_SEND_CAN_MESSAGE                  = 0x0203U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_TUNNEL_CAN_MESSAGE                = 0x0204U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_ROUTE_IP_2_IP_COMMUNICATION       = 0x0205U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_REQUEST_PROGRAMMING               = 0x0206U;
   static const stw_types::uint16 mhu16_OSY_RC_RC_SID_SET_BITRATE                    = 0x0207U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_CHECK_FLASH_MEMORY_AVAILABILITY   = 0x0208U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_READ_FLASH_BLOCK_DATA             = 0x0209U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_CONFIGURE_FLASHLOADER_COMMUNICATION_CHANNEL = 0x0210U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_READ_DATAPOOL_META_DATA           = 0x0211U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_VERIFY_DATAPOOL                   = 0x0212U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_NOTIFY_NVM_DATA_CHANGED           = 0x0213U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_SET_NODE_ID_FOR_CHANNEL           = 0x0214U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_SET_IP_ADDRESS_FOR_CHANNEL        = 0x0215U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART1  = 0x0216U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART2  = 0x0217U;
   static const stw_types::uint16 mhu16_OSY_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART3  = 0x0218U;

   //routine sub-functions
   static const stw_types::uint8 mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE           = 0x01U;
   static const stw_types::uint8 mhu8_OSY_RC_SUB_FUNCTION_STOP_ROUTINE            = 0x02U;
   static const stw_types::uint8 mhu8_OSY_RC_SUB_FUNCTION_REQUEST_ROUTINE_RESULTS = 0x03U;

   //limits for data pool access functions
   static const stw_types::uint8 mhu8_OSY_MAX_NUM_DATA_POOLS      = 32U;
   static const stw_types::uint8 mhu8_OSY_MAX_NUM_DATA_POOL_LISTS = 128U;
   static const stw_types::uint16 mhu16_OSY_MAX_NUM_DATA_POOL_LIST_ELEMENTS = 2048U;

protected:
   void m_LogErrorWithHeader(const stw_scl::C_SCLString & orc_Activity, const stw_scl::C_SCLString & orc_Information,
                             const stw_types::charn * const opcn_Function, const bool oq_AsError = true) const;

   virtual void m_OsyReadDataPoolDataEventReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                                   const stw_types::uint16 ou16_ListIndex,
                                                   const stw_types::uint16 ou16_ElementIndex,
                                                   const std::vector<stw_types::uint8> & orc_Value);
   virtual void m_OsyReadDataPoolDataEventErrorReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                                        const stw_types::uint16 ou16_ListIndex,
                                                        const stw_types::uint16 ou16_ElementIndex,
                                                        const stw_types::uint8 ou8_NrCode);

public:
   ///information about a flash data block
   //lint -sem(stw_opensyde_core::C_OSCProtocolDriverOsy::C_FlashBlockInfo::ClearContent,initializer)
   class C_FlashBlockInfo
   {
   public:
      C_FlashBlockInfo(void);
      virtual ~C_FlashBlockInfo(void);

      //lint -sem(stw_opensyde_core::C_OSCProtocolDriverOsy::C_FlashBlockInfo::ClearContent,initializer)
      void ClearContent(void);

      stw_scl::C_SCLString c_ApplicationName;
      stw_scl::C_SCLString c_ApplicationVersion;
      stw_scl::C_SCLString c_BuildDate;
      stw_scl::C_SCLString c_BuildTime;
      stw_types::uint32 u32_BlockStartAddress; ///< first occupied address
      stw_types::uint32 u32_BlockEndAddress;   ///< highest occupied address
      stw_types::uint8 u8_SignatureValid;      ///< 0 = valid; 1 = invalid
      stw_scl::C_SCLString c_AdditionalInformation;

      static const stw_types::uint8 hu8_IdBlockAddresses        = 1U;
      static const stw_types::uint8 hu8_IdResultSignature       = 2U;
      static const stw_types::uint8 hu8_IdApplicationVersion    = 3U;
      static const stw_types::uint8 hu8_IdBuildTimestamp        = 4U;
      static const stw_types::uint8 hu8_IdApplicationName       = 5U;
      static const stw_types::uint8 hu8_IdAdditionalInformation = 6U;
   };

   ///meta information for a data pool
   class C_DataPoolMetaData
   {
   public:
      C_DataPoolMetaData(void);

      stw_types::uint8 au8_Version[3]; ///< 0:Major, 1:Minor, 2:Release
      stw_scl::C_SCLString c_Name;     ///< Data pool name
   };

   ///list of features supported by server
   class C_ListOfFeatures
   {
   public:
      bool q_FlashloaderCanWriteToNvm;           ///< set to true in Flashloader to show that "Writing to NVM" is
                                                 // supported
      bool q_MaxNumberOfBlockLengthAvailable;    ///< true: MaxNumberOfBlockLength can be read
      bool q_EthernetToEthernetRoutingSupported; ///< true: E2E routing supported
   };

   C_OSCProtocolDriverOsy(void);
   virtual ~C_OSCProtocolDriverOsy(void);

   void SetTimeoutPolling(const stw_types::uint32 ou32_TimeoutPollingMs);
   void ResetTimeoutPolling(void);
   void SetMaxServiceSize(const stw_types::uint16 ou16_MaxServiceSize);

   void InitializeTunnelCanMessage(PR_OsyTunnelCanMessageReceived const opr_OsyTunnelCanMessageReceived,
                                   void * const opv_Instance);
   void InitializeHandleWaitTime(PR_OsyHandleWaitTime const opr_OsyHandleWaitTime, void * const opv_Instance);

   stw_types::sint32 IsConnected(void);
   stw_types::sint32 ReConnect(void);
   stw_types::sint32 Disconnect(void);
   stw_types::sint32 Cycle(void);

   stw_types::sint32 SetTransportProtocol(C_OSCProtocolDriverOsyTpBase * const opc_TransportProtocol);
   stw_types::sint32 SetNodeIdentifiers(const C_OSCProtocolDriverOsyNode & orc_ClientId,
                                        const C_OSCProtocolDriverOsyNode & orc_ServerId);

   C_OSCProtocolDriverOsyTpBase * GetTransportProtocol(void);
   void GetNodeIdentifiers(C_OSCProtocolDriverOsyNode & orc_ClientId, C_OSCProtocolDriverOsyNode & orc_ServerId) const;

   static stw_scl::C_SCLString h_GetOpenSydeServiceErrorDetails(const stw_types::sint32 os32_FunctionResult,
                                                                const stw_types::uint8 ou8_NrCode,
                                                                bool * const opq_IsHardError = NULL);

   //openSYDE protocol services:
   //Session management:
   stw_types::sint32 OsyDiagnosticSessionControl(const stw_types::uint8 ou8_SessionId,
                                                 stw_types::uint8 * const opu8_NrCode = NULL);

   //Device information services:
   stw_types::sint32 OsyReadEcuSerialNumber(stw_types::uint8(&orau8_SerialNumber)[6],
                                            stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadHardwareNumber(stw_types::uint32 & oru32_HardwareNumber,
                                           stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadHardwareVersionNumber(stw_scl::C_SCLString & orc_HardwareVersionNumber,
                                                  stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadListOfFeatures(C_ListOfFeatures & orc_ListOfFeatures,
                                           stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadMaxNumberOfBlockLength(stw_types::uint16 & oru16_MaxNumberOfBlockLength,
                                                   stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadDeviceName(stw_scl::C_SCLString & orc_DeviceName,
                                       stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadApplicationName(stw_scl::C_SCLString & orc_ApplicationName,
                                            stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadApplicationVersion(stw_scl::C_SCLString & orc_ApplicationVersion,
                                               stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadActiveDiagnosticSession(stw_types::uint8 & oru8_SessionId,
                                                    stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadBootSoftwareIdentification(stw_types::uint8(&orau8_Version)[3],
                                                       stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadMaxNumOfEventDrivenTransmissions(stw_types::uint16 & oru16_MaxNum,
                                                             stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadProtocolDriverImplementationVersion(stw_types::uint8(&orau8_Version)[3],
                                                                stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadProtocolVersion(stw_types::uint8(&orau8_Version)[3],
                                            stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadFlashloaderProtocolVersion(stw_types::uint8(&orau8_Version)[3],
                                                       stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadFlashCount(stw_types::uint32 & oru32_FlashCount,
                                       stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadApplicationSoftwareFingerprint(stw_types::uint8(&orau8_Date)[3],
                                                           stw_types::uint8(&orau8_Time)[3],
                                                           stw_scl::C_SCLString & orc_Username,
                                                           stw_types::uint8 * const opu8_NrCode = NULL);

   //Device configuration
   stw_types::sint32 OsySetNodeIdForChannel(const stw_types::uint8 ou8_ChannelType,
                                            const stw_types::uint8 ou8_ChannelIndex,
                                            const C_OSCProtocolDriverOsyNode & orc_NewNodeId,
                                            stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsySetBitrate(const stw_types::uint8 ou8_ChannelType, const stw_types::uint8 ou8_ChannelIndex,
                                   const stw_types::uint32 ou32_Bitrate, stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyConfigureFlashloaderCommunicationChannel(const stw_types::uint8 ou8_ChannelType,
                                                                 const stw_types::uint8 ou8_ChannelIndex,
                                                                 const bool oq_Activated,
                                                                 stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsySetIpAddressForChannel(const stw_types::uint8 ou8_ChannelType,
                                               const stw_types::uint8 ou8_ChannelIndex,
                                               const stw_types::uint8 (&orau8_IpAddress)[4],
                                               const stw_types::uint8 (&orau8_NetMask)[4],
                                               const stw_types::uint8(&orau8_DefaultGateway)[4],
                                               stw_types::uint8 * const opu8_NrCode = NULL);

   //Application layer routing for "legacy" protocols
   stw_types::sint32 OsySetRouteDiagnosisCommunication(const stw_types::uint8 ou8_InputChannelType,
                                                       const stw_types::uint8 ou8_InputChannelIndex,
                                                       const stw_types::uint8 ou8_OutputChannelType,
                                                       const stw_types::uint8 ou8_OutputChannelIndex,
                                                       const stw_types::uint8 ou8_SourceBusId,
                                                       const stw_types::uint8 ou8_TargetBusId,
                                                       stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyStopRouteDiagnosisCommunication(stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsySetRouteIp2IpCommunication(const stw_types::uint8 ou8_OutputChannelType,
                                                   const stw_types::uint8 ou8_OutputChannelIndex,
                                                   const stw_types::uint8 ou8_SourceBusId,
                                                   const stw_types::uint8 ou8_TargetBusId,
                                                   const stw_types::uint8 (&orau8_IpAddress)[4],
                                                   stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyCheckRouteIp2IpCommunication(stw_types::uint8 & oru8_Status,
                                                     stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsySendCanMessage(const stw_types::uint8 ou8_ChannelIndex,
                                       const stw_can::T_STWCAN_Msg_TX & orc_CanMessage,
                                       stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsySetTunnelCanMessages(const stw_types::uint8 ou8_CanChannelIndex,
                                             const stw_types::uint32 ou32_FilterId,
                                             const stw_types::uint32 ou32_FilterMask,
                                             stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyStopTunnelCanMessages(stw_types::uint8 * const opu8_NrCode = NULL);

   //Data pool access:
   stw_types::sint32 OsyReadDataPoolData(const stw_types::uint8 ou8_DataPoolIndex,
                                         const stw_types::uint16 ou16_ListIndex,
                                         const stw_types::uint16 ou16_ElementIndex,
                                         std::vector<stw_types::uint8> & orc_ReadData,
                                         stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyWriteDataPoolData(const stw_types::uint8 ou8_DataPoolIndex,
                                          const stw_types::uint16 ou16_ListIndex,
                                          const stw_types::uint16 ou16_ElementIndex,
                                          const std::vector<stw_types::uint8> & orc_DataToWrite,
                                          stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyWriteDataPoolEventDataRate(const stw_types::uint8 ou8_TransmissionRail,
                                                   const stw_types::uint16 ou16_DataRate,
                                                   stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadDataPoolDataCyclic(const stw_types::uint8 ou8_DataPoolIndex,
                                               const stw_types::uint16 ou16_ListIndex,
                                               const stw_types::uint16 ou16_ElementIndex,
                                               const stw_types::uint8 ou8_TransmissionRail,
                                               stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadDataPoolDataChangeDriven(const stw_types::uint8 ou8_DataPoolIndex,
                                                     const stw_types::uint16 ou16_ListIndex,
                                                     const stw_types::uint16 ou16_ElementIndex,
                                                     const stw_types::uint8 ou8_TransmissionRail,
                                                     const stw_types::uint32 ou32_Hysteresis,
                                                     stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyStopDataPoolEvents(stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadDataPoolMetaData(const stw_types::uint8 ou8_DataPoolIndex,
                                             C_DataPoolMetaData & orc_MetaData,
                                             stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyVerifyDataPool(const stw_types::uint8 ou8_DataPoolIndex,
                                       const stw_types::uint32 ou32_DataPoolChecksum, bool & orq_Match,
                                       stw_types::uint8 * const opu8_NrCode = NULL);
   //NVM access:
   stw_types::sint32 OsyReadMemoryByAddress(const stw_types::uint32 ou32_MemoryAddress,
                                            std::vector<stw_types::uint8> & orc_DataRecord,
                                            stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyWriteMemoryByAddress(const stw_types::uint32 ou32_MemoryAddress,
                                             const std::vector<stw_types::uint8> & orc_DataRecord,
                                             stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyNotifyNvmDataChanges(const stw_types::uint8 ou8_DataPoolIndex,
                                             const stw_types::uint8 ou8_ListIndex, bool & orq_ApplicationAcknowledge,
                                             stw_types::uint8 * const opu8_NrCode = NULL);

   //Flashloader
   stw_types::sint32 OsyRequestProgramming(stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyCheckFlashMemoryAvailable(const stw_types::uint32 ou32_StartAddress,
                                                  const stw_types::uint32 ou32_Size,
                                                  stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyReadFlashBlockData(const stw_types::uint8 ou8_FlashBlock, C_FlashBlockInfo & orc_BlockInfo,
                                           stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsySecurityAccessRequestSeed(const stw_types::uint8 ou8_SecurityLevel,
                                                  stw_types::uint32 & oru32_Seed,
                                                  stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsySecurityAccessSendKey(const stw_types::uint8 ou8_SecurityLevel,
                                              const stw_types::uint32 ou32_Key,
                                              stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyRequestDownload(const stw_types::uint32 ou32_StartAddress, const stw_types::uint32 ou32_Size,
                                        stw_types::uint32 & oru32_MaxBlockLength,
                                        stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyRequestFileTransfer(const stw_scl::C_SCLString & orc_FilePath,
                                            const stw_types::uint32 ou32_FileSize,
                                            stw_types::uint32 & oru32_MaxBlockLength,
                                            stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyTransferData(const stw_types::uint8 ou8_BlockSequenceCounter,
                                     std::vector<stw_types::uint8> & orc_Data,
                                     stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyRequestTransferExitAddressBased(const bool oq_SendSignatureBlockAddress,
                                                        const stw_types::uint32 ou32_SignatureBlockAddress,
                                                        stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyRequestTransferExitFileBased(const stw_types::uint8(&orau8_Signature)[8],
                                                     stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyWriteApplicationSoftwareFingerprint(const stw_types::uint8 (&orau8_Date)[3],
                                                            const stw_types::uint8 (&orau8_Time)[3],
                                                            const stw_scl::C_SCLString & orc_UserName,
                                                            stw_types::uint8 * const opu8_NrCode = NULL);

   //Common
   stw_types::sint32 OsyTesterPresent(const stw_types::uint8 ou8_SuppressResponseMsg,
                                      stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 OsyEcuReset(
      const stw_types::uint8 ou8_ResetType = C_OSCProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_KEY_OFF_ON);

   // Default Timeout
   static const stw_types::uint32 hu32_DEFAULT_TIMEOUT = 1000U; // In ms
   // Cyclic time till the registered function mpr_OnOsyWaitTime will be called in m_PollForSpecificServiceResponse
   static const stw_types::uint32 hu32_DEFAULT_HANDLE_WAIT_TIME = 2000U; // In ms
   // Sessions for service OsyDiagnosticSessionControl
   static const stw_types::uint8 hu8_DIAGNOSTIC_SESSION_DEFAULT            = 0x01U;
   static const stw_types::uint8 hu8_DIAGNOSTIC_SESSION_EXTENDED_DIAGNOSIS = 0x03U;
   static const stw_types::uint8 hu8_DIAGNOSTIC_SESSION_PREPROGRAMMING     = 0x60U;
   static const stw_types::uint8 hu8_DIAGNOSTIC_SESSION_PROGRAMMING        = 0x02U;

   //negative response codes:
   static const stw_types::uint8 hu8_NR_CODE_SERVICE_NOT_SUPPORTED       = 0x11U;
   static const stw_types::uint8 hu8_NR_CODE_SUB_FUNCTION_NOT_SUPPORTED  = 0x12U;
   static const stw_types::uint8 hu8_NR_CODE_INCORRECT_MESSAGE_LENGTH_OR_FORMAT = 0x13U;
   static const stw_types::uint8 hu8_NR_CODE_RESPONSE_TOO_LONG           = 0x14U;
   static const stw_types::uint8 hu8_NR_CODE_CONDITIONS_NOT_CORRECT      = 0x22U;
   static const stw_types::uint8 hu8_NR_CODE_REQUEST_SEQUENCE_ERROR      = 0x24U;
   static const stw_types::uint8 hu8_NR_CODE_REQUEST_OUT_OF_RANGE        = 0x31U;
   static const stw_types::uint8 hu8_NR_CODE_SECURITY_ACCESS_DENIED      = 0x33U;
   static const stw_types::uint8 hu8_NR_CODE_INVALID_KEY                 = 0x35U;
   static const stw_types::uint8 hu8_NR_CODE_UPLOAD_DOWNLOAD_NOT_ACCEPTED = 0x70U;
   static const stw_types::uint8 hu8_NR_CODE_GENERAL_PROGRAMMING_FAILURE = 0x72U;
   static const stw_types::uint8 hu8_NR_CODE_RESPONSE_PENDING            = 0x78U;
   static const stw_types::uint8 hu8_NR_CODE_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION = 0x7FU;

   //result status of routine OSY_RC_SID_ROUTE_IP_2_IP_COMMUNICATION
   static const stw_types::uint8 hu8_OSY_IP_2_IP_STATUS_IDLE        = 0U;
   static const stw_types::uint8 hu8_OSY_IP_2_IP_STATUS_IN_PROGRESS = 1U;
   static const stw_types::uint8 hu8_OSY_IP_2_IP_STATUS_ERROR       = 2U;
   static const stw_types::uint8 hu8_OSY_IP_2_IP_STATUS_CONNECTED   = 3U;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif // C_OSCPROTOCOLDRIVEROSYH
