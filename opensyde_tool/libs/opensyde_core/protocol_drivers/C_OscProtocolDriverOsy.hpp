//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol driver

   \class       stw::opensyde_core::C_OscProtocolDriverOsy
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
#ifndef C_OSCPROTOCOLDRIVEROSY_HPP
#define C_OSCPROTOCOLDRIVEROSY_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <map>

#include "stwtypes.hpp"
#include "C_OscProtocolSerialNumber.hpp"
#include "C_OscProtocolDriverOsyTpBase.hpp"
#include "stw_can.hpp" //for CAN message type

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscProtocolDriverOsy
{
private:
   typedef void (* PR_OsyTunnelCanMessageReceived)(void * const opv_Instance, const uint8_t ou8_CanChannel,
                                                   const stw::can::T_STWCAN_Msg_RX & orc_CanMessage);
   typedef void (* PR_OsyHandleWaitTime)(void * const opv_Instance);

   stw::tgl::C_TglCriticalSection mc_LockReception; ///< for locking reception handling

   PR_OsyTunnelCanMessageReceived mpr_OnOsyTunnelCanMessageReceived;
   void * mpv_OnAsyncTunnelCanMessageInstance;
   PR_OsyHandleWaitTime mpr_OnOsyWaitTime;
   void * mpv_OnOsyWaitTimeInstance;

   C_OscProtocolDriverOsyTpBase * mpc_TransportProtocol; ///< installed transport protocol

   C_OscProtocolDriverOsyNode mc_ClientId; ///< our own ID
   C_OscProtocolDriverOsyNode mc_ServerId; ///< ID of server we are connected to

   //timeouts:
   uint32_t mu32_TimeoutPollingMs; ///< timeout used when "polling" services

   //maximum service size including header (used in WriteMemoryByAddress):
   uint16_t mu16_MaxServiceSize;

   std::map<uint8_t, uint8_t> mc_DataPoolMappingClientToServer;
   std::map<uint8_t, uint8_t> mc_DataPoolMappingServerToClient;

   void m_LogServiceError(const stw::scl::C_SclString & orc_Service, const int32_t os32_ReturnCode,
                          const uint8_t ou8_NrCode) const;

   //check for exactly one incoming non-async service:
   int32_t m_Cycle(const bool oq_CheckForSpecificServiceId = false, const uint8_t ou8_ExpectedServiceId = 0xFFU,
                   C_OscProtocolDriverOsyService * const opc_ReceivedService = NULL);
   //poll for exactly one incoming service:
   int32_t m_PollForSpecificServiceResponse(const uint8_t ou8_ExpectedServiceId, const uint16_t ou16_ExpectedSize,
                                            C_OscProtocolDriverOsyService & orc_Service, uint8_t & oru8_NrCode,
                                            const bool oq_ExactSizeExpected = true, const std::vector<uint8_t> * const opc_ExpectedErrData =
                                               NULL);
   int32_t m_ReadDataByIdentifier(const uint16_t ou16_Identifier, const uint16_t ou16_ExpectedPayloadSize,
                                  const bool oq_ExactSizeExpected, std::vector<uint8_t> & orc_ReadData,
                                  uint8_t & oru8_NrCode);
   int32_t m_ReadStringDataIdentifier(const uint16_t ou16_DataIdentifier, stw::scl::C_SclString & orc_String,
                                      uint8_t & oru8_NrCode);
   int32_t m_WriteDataByIdentifier(const uint16_t ou16_Identifier, const std::vector<uint8_t> & orc_WriteData,
                                   uint8_t & oru8_NrCode);
   int32_t m_PackDataPoolIdentifier(const uint8_t ou8_DataPoolIndex,
                                    const uint16_t ou16_ListIndex,
                                    const uint16_t ou16_ElementIndex,
                                    uint8_t(&orau8_PackedId)[3]) const;
   void m_UnpackDataPoolIdentifier(const uint8_t(&orau8_PackedId)[3], uint8_t & oru8_DataPoolIndex,
                                   uint16_t & oru16_ListIndex, uint16_t & oru16_ElementIndex) const;
   int32_t m_RoutineControl(const uint16_t ou16_RoutineIdentifier, const uint8_t ou8_SubFunction,
                            const std::vector<uint8_t> & orc_SendData, const uint16_t ou16_ExpectedPayloadSize,
                            const bool oq_ExactSizeExpected, std::vector<uint8_t> & orc_ReadData, uint8_t & oru8_NrCode,
                            const bool oq_CanTransferWithoutFlowControl = false);
   int32_t m_SecurityAccess(const uint8_t ou8_SubFunction, const std::vector<uint8_t> & orc_SendData,
                            const uint16_t ou16_SendPayloadSize, const uint16_t ou16_ExpectedPayloadSize,
                            std::vector<uint8_t> & orc_ReadData, uint8_t & oru8_NrCode);

   int32_t m_HandleAsyncResponse(const C_OscProtocolDriverOsyService & orc_ReceivedService);
   int32_t m_HandleAsyncOsyReadDataPoolDataEvent(const C_OscProtocolDriverOsyService & orc_ReceivedService);
   int32_t m_HandleAsyncOsyReadDataPoolDataErrorEvent(const C_OscProtocolDriverOsyService & orc_ReceivedService);
   int32_t m_HandleAsyncOsyTunnelCanMessagesEvent(const C_OscProtocolDriverOsyService & orc_ReceivedService);

   uint8_t m_GetDataPoolIndexClientToServer(const uint8_t ou8_ClientDataPoolIndex) const;
   uint8_t m_GetDataPoolIndexServerToClient(const uint8_t ou8_ServerDataPoolIndex) const;

   static void mh_ConvertVariableToNecessaryBytes(const uint32_t ou32_Variable, std::vector<uint8_t> & orc_Bytes);

   //service IDs:
   static const uint8_t mhu8_OSY_SI_DIAGNOSTIC_SESSION_CONTROL = 0x10U;
   static const uint8_t mhu8_OSY_SI_ECU_RESET                  = 0x11U;
   static const uint8_t mhu8_OSY_SI_READ_DATA_BY_IDENTIFIER    = 0x22U;
   static const uint8_t mhu8_OSY_SI_READ_MEMORY_BY_ADDRESS     = 0x23U;
   static const uint8_t mhu8_OSY_SI_SECURITY_ACCESS            = 0x27U;
   static const uint8_t mhu8_OSY_SI_WRITE_DATA_BY_IDENTIFIER   = 0x2EU;
   static const uint8_t mhu8_OSY_SI_ROUTINE_CONTROL            = 0x31U;
   static const uint8_t mhu8_OSY_SI_REQUEST_DOWNLOAD           = 0x34U;
   static const uint8_t mhu8_OSY_SI_TRANSFER_DATA              = 0x36U;
   static const uint8_t mhu8_OSY_SI_REQUEST_TRANSFER_EXIT      = 0x37U;
   static const uint8_t mhu8_OSY_SI_REQUEST_FILE_TRANSFER      = 0x38U;
   static const uint8_t mhu8_OSY_SI_WRITE_MEMORY_BY_ADDRESS    = 0x3DU;
   static const uint8_t mhu8_OSY_SI_TESTER_PRESENT             = 0x3EU;
   static const uint8_t mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN = 0xBAU;
   static const uint8_t mhu8_OSY_SI_READ_DATA_POOL_DATA_BY_ID  = 0xBBU;
   static const uint8_t mhu8_OSY_SI_WRITE_DATA_POOL_DATA_BY_ID = 0xBCU;
   static const uint8_t mhu8_OSY_SI_READ_SERIAL_NUMBER         = 0xBDU;
   static const uint8_t mhu8_OSY_SI_TUNNEL_CAN_MESSAGE         = 0xBEU;

   //negative response ID:
   static const uint8_t mhu8_OSY_NR_SI = 0x7FU;

   //data identifiers:
   static const uint16_t mhu16_OSY_DI_BOOT_SOFTWARE_IDENTIFICATION     = 0xF180U;
   static const uint16_t mhu16_OSY_DI_APPLICATION_SOFTWARE_FINGERPRINT = 0xF184U;
   static const uint16_t mhu16_OSY_DI_ACTIVE_DIAGNOSTIC_SESSION        = 0xF186U;
   static const uint16_t mhu16_OSY_DI_ECU_SERIAL_NUMBER                = 0xF18CU;
   static const uint16_t mhu16_OSY_DI_SYS_SUPPLIER_ECU_HW_NUMBER       = 0xF192U;
   static const uint16_t mhu16_OSY_DI_SYS_SUPPLIER_ECU_HW_VERSION      = 0xF193U;
   static const uint16_t mhu16_OSY_DI_LIST_OF_FEATURES                 = 0xA800U;
   static const uint16_t mhu16_OSY_DI_MAX_NUMBER_OF_BLOCK_LENGTH       = 0xA801U;
   static const uint16_t mhu16_OSY_DI_DATARATE_1                       = 0xA810U;
   static const uint16_t mhu16_OSY_DI_DATARATE_2                       = 0xA811U;
   static const uint16_t mhu16_OSY_DI_DATARATE_3                       = 0xA812U;
   static const uint16_t mhu16_OSY_DI_PROTOCOL_VERSION                 = 0xA813U;
   static const uint16_t mhu16_OSY_DI_PROTOCOL_DRIVER_IMPLEMENTATION_VERSION = 0xA814U;
   static const uint16_t mhu16_OSY_DI_FLASHLOADER_PROTOCOL_VERSION     = 0xA815U;
   static const uint16_t mhu16_OSY_DI_MAX_NUM_ASYNC                    = 0xA817U;
   static const uint16_t mhu16_OSY_DI_FLASH_COUNT                      = 0xA819U;
   static const uint16_t mhu16_OSY_DI_DEVICE_NAME                      = 0xA81AU;
   static const uint16_t mhu16_OSY_DI_APPLICATION_NAME                 = 0xA81BU;
   static const uint16_t mhu16_OSY_DI_APPLICATION_VERSION              = 0xA81CU;
   static const uint16_t mhu16_OSY_DI_FILE_BASED_TRANSFER_EXIT_RESULT  = 0xA81DU;
   static const uint16_t mhu16_OSY_DI_ECU_SERIAL_NUMBER_EXT            = 0xA81EU;
   static const uint16_t mhu16_OSY_DI_SUB_NODE_ID                      = 0xA81FU;
   static const uint16_t mhu16_OSY_DI_CERTIFICATE_SERIAL_NUMBER        = 0xA820U;
   static const uint16_t mhu16_OSY_DI_SECURITY_ACTIVATION              = 0xA821U;
   static const uint16_t mhu16_OSY_DI_DEBUGGER_ACTIVATION              = 0xA822U;
   static const uint16_t mhu16_OSY_DI_SECURITY_KEY                     = 0xA823U;
   static const uint16_t mhu16_OSY_DI_CERTIFICATE_SERIAL_NUMBER_L7     = 0xA824U;
   //routine identifiers
   static const uint16_t mhu16_OSY_RC_SID_ROUTE_DIAGNOSIS_COMMUNICATION     = 0x0202U;
   static const uint16_t mhu16_OSY_RC_SID_SEND_CAN_MESSAGE                  = 0x0203U;
   static const uint16_t mhu16_OSY_RC_SID_TUNNEL_CAN_MESSAGE                = 0x0204U;
   static const uint16_t mhu16_OSY_RC_SID_ROUTE_IP_2_IP_COMMUNICATION       = 0x0205U;
   static const uint16_t mhu16_OSY_RC_SID_REQUEST_PROGRAMMING               = 0x0206U;
   static const uint16_t mhu16_OSY_RC_RC_SID_SET_BITRATE                    = 0x0207U;
   static const uint16_t mhu16_OSY_RC_SID_CHECK_FLASH_MEMORY_AVAILABILITY   = 0x0208U;
   static const uint16_t mhu16_OSY_RC_SID_READ_FLASH_BLOCK_DATA             = 0x0209U;
   static const uint16_t mhu16_OSY_RC_SID_CONFIGURE_FLASHLOADER_COMMUNICATION_CHANNEL = 0x0210U;
   static const uint16_t mhu16_OSY_RC_SID_READ_DATAPOOL_META_DATA           = 0x0211U;
   static const uint16_t mhu16_OSY_RC_SID_VERIFY_DATAPOOL                   = 0x0212U;
   static const uint16_t mhu16_OSY_RC_SID_NOTIFY_NVM_DATA_CHANGED           = 0x0213U;
   static const uint16_t mhu16_OSY_RC_SID_SET_NODE_ID_FOR_CHANNEL           = 0x0214U;
   static const uint16_t mhu16_OSY_RC_SID_SET_IP_ADDRESS_FOR_CHANNEL        = 0x0215U;
   static const uint16_t mhu16_OSY_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART1  = 0x0216U;
   static const uint16_t mhu16_OSY_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART2  = 0x0217U;
   static const uint16_t mhu16_OSY_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART3  = 0x0218U;
   static const uint16_t mhu16_OSY_RC_SID_FACTORY_MODE                      = 0x0225U;

   //routine sub-functions
   static const uint8_t mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE           = 0x01U;
   static const uint8_t mhu8_OSY_RC_SUB_FUNCTION_STOP_ROUTINE            = 0x02U;
   static const uint8_t mhu8_OSY_RC_SUB_FUNCTION_REQUEST_ROUTINE_RESULTS = 0x03U;

   //limits for data pool access functions
   static const uint8_t mhu8_OSY_MAX_NUM_DATA_POOLS      = 32U;
   static const uint8_t mhu8_OSY_MAX_NUM_DATA_POOL_LISTS = 128U;
   static const uint16_t mhu16_OSY_MAX_NUM_DATA_POOL_LIST_ELEMENTS = 2048U;

protected:
   void m_LogErrorWithHeader(const stw::scl::C_SclString & orc_Activity, const stw::scl::C_SclString & orc_Information,
                             const char_t * const opcn_Function, const bool oq_AsError = true) const;

   virtual void m_OsyReadDataPoolDataEventReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                   const uint16_t ou16_ElementIndex,
                                                   const std::vector<uint8_t> & orc_Value);
   virtual void m_OsyReadDataPoolDataEventErrorReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                        const uint16_t ou16_ElementIndex, const uint8_t ou8_NrCode);

public:
   ///information about a flash data block
   //lint -sem(stw::opensyde_core::C_OscProtocolDriverOsy::C_FlashBlockInfo::ClearContent,initializer)
   class C_FlashBlockInfo
   {
   public:
      C_FlashBlockInfo(void);
      virtual ~C_FlashBlockInfo(void);

      //lint -sem(stw::opensyde_core::C_OscProtocolDriverOsy::C_FlashBlockInfo::ClearContent,initializer)
      void ClearContent(void);

      stw::scl::C_SclString c_ApplicationName;
      stw::scl::C_SclString c_ApplicationVersion;
      stw::scl::C_SclString c_BuildDate;
      stw::scl::C_SclString c_BuildTime;
      uint32_t u32_BlockStartAddress; ///< first occupied address
      uint32_t u32_BlockEndAddress;   ///< highest occupied address
      uint8_t u8_SignatureValid;      ///< 0 = valid; 1 = invalid
      stw::scl::C_SclString c_AdditionalInformation;

      static const uint8_t hu8_ID_BLOCK_ADDRESSES        = 1U;
      static const uint8_t hu8_ID_RESULT_SIGNATURE       = 2U;
      static const uint8_t hu8_ID_APPLICATION_VERSION    = 3U;
      static const uint8_t hu8_ID_BUILD_TIMESTAMP        = 4U;
      static const uint8_t hu8_ID_APPLICATION_NAME       = 5U;
      static const uint8_t hu8_ID_ADDITIONAL_INFORMATION = 6U;
   };

   ///meta information for a data pool
   class C_DataPoolMetaData
   {
   public:
      C_DataPoolMetaData(void);

      uint8_t au8_Version[3];       ///< 0:Major, 1:Minor, 2:Release
      stw::scl::C_SclString c_Name; ///< Data pool name
   };

   ///list of features supported by server
   class C_ListOfFeatures
   {
   public:
      C_ListOfFeatures(void);

      bool q_FlashloaderCanWriteToNvm;             ///< set to true in Flashloader to show that "Writing to NVM" is
                                                   // supported
      bool q_MaxNumberOfBlockLengthAvailable;      ///< true: MaxNumberOfBlockLength can be read
      bool q_EthernetToEthernetRoutingSupported;   ///< true: E2E routing supported
      bool q_FileBasedTransferExitResultAvailable; ///< true: FileBasedTransferExitResult can be read
      bool q_ExtendedSerialNumberModeImplemented;  ///< true: The device has the extended serial number format
      bool q_SupportsSecurity;                     ///< true: The device supports the security feature
      bool q_SupportsDebuggerOff;                  ///< true: The debugger interface of the device can be deactivated
      bool q_SupportsDebuggerOn;                   ///< true: The debugger interface of the device can be activated
   };

   C_OscProtocolDriverOsy(void);
   virtual ~C_OscProtocolDriverOsy(void);

   void SetTimeoutPolling(const uint32_t ou32_TimeoutPollingMs);
   void ResetTimeoutPolling(void);
   void SetMaxServiceSize(const uint16_t ou16_MaxServiceSize);

   void InitializeTunnelCanMessage(const PR_OsyTunnelCanMessageReceived opr_OsyTunnelCanMessageReceived,
                                   void * const opv_Instance);
   void InitializeHandleWaitTime(const PR_OsyHandleWaitTime opr_OsyHandleWaitTime, void * const opv_Instance);

   int32_t IsConnected(void);
   int32_t ReConnect(void);
   int32_t Disconnect(void);
   int32_t Cycle(void);

   int32_t SetTransportProtocol(C_OscProtocolDriverOsyTpBase * const opc_TransportProtocol);
   int32_t SetNodeIdentifiers(const C_OscProtocolDriverOsyNode & orc_ClientId,
                              const C_OscProtocolDriverOsyNode & orc_ServerId);

   C_OscProtocolDriverOsyTpBase * GetTransportProtocol(void);
   void GetNodeIdentifiers(C_OscProtocolDriverOsyNode & orc_ClientId, C_OscProtocolDriverOsyNode & orc_ServerId) const;

   void RegisterDataPoolMapping(const std::map<uint8_t, uint8_t> & orc_Mapping);
   void ClearDataPoolMapping(void);

   static stw::scl::C_SclString h_GetOpenSydeServiceErrorDetails(const int32_t os32_FunctionResult,
                                                                 const uint8_t ou8_NrCode,
                                                                 bool * const opq_IsHardError = NULL);

   //openSYDE protocol services:
   //Session management:
   int32_t OsyDiagnosticSessionControl(const uint8_t ou8_SessionId, uint8_t * const opu8_NrCode = NULL);

   //Device information services:
   int32_t OsyReadEcuSerialNumber(C_OscProtocolSerialNumber & orc_SerialNumber, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadHardwareNumber(uint32_t & oru32_HardwareNumber, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadHardwareVersionNumber(stw::scl::C_SclString & orc_HardwareVersionNumber,
                                        uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadListOfFeatures(C_ListOfFeatures & orc_ListOfFeatures, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadMaxNumberOfBlockLength(uint16_t & oru16_MaxNumberOfBlockLength, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadDeviceName(stw::scl::C_SclString & orc_DeviceName, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadApplicationName(stw::scl::C_SclString & orc_ApplicationName, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadApplicationVersion(stw::scl::C_SclString & orc_ApplicationVersion,
                                     uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadActiveDiagnosticSession(uint8_t & oru8_SessionId, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadBootSoftwareIdentification(uint8_t(&orau8_Version)[3],
                                             uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadMaxNumOfEventDrivenTransmissions(uint16_t & oru16_MaxNum, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadProtocolDriverImplementationVersion(uint8_t(&orau8_Version)[3],
                                                      uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadProtocolVersion(uint8_t(&orau8_Version)[3],
                                  uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadFlashloaderProtocolVersion(uint8_t(&orau8_Version)[3],
                                             uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadFlashCount(uint32_t & oru32_FlashCount, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadApplicationSoftwareFingerprint(uint8_t(&orau8_Date)[3],
                                                 uint8_t(&orau8_Time)[3],
                                                 stw::scl::C_SclString & orc_Username,
                                                 uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadFileBasedTransferExitResult(stw::scl::C_SclString & orc_TransferExitResult,
                                              uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadEcuSerialNumberExt(C_OscProtocolSerialNumber & orc_SerialNumberExt,
                                     uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadSubNodeId(uint8_t & oru8_SubNodeId, uint8_t * const opu8_NrCode = NULL);

   //Device configuration
   int32_t OsySetNodeIdForChannel(const uint8_t ou8_ChannelType, const uint8_t ou8_ChannelIndex,
                                  const C_OscProtocolDriverOsyNode & orc_NewNodeId, uint8_t * const opu8_NrCode = NULL);
   int32_t OsySetBitrate(const uint8_t ou8_ChannelType, const uint8_t ou8_ChannelIndex, const uint32_t ou32_Bitrate,
                         uint8_t * const opu8_NrCode = NULL);
   int32_t OsyConfigureFlashloaderCommunicationChannel(const uint8_t ou8_ChannelType, const uint8_t ou8_ChannelIndex,
                                                       const bool oq_Activated, uint8_t * const opu8_NrCode = NULL);
   int32_t OsySetIpAddressForChannel(const uint8_t ou8_ChannelType, const uint8_t ou8_ChannelIndex,
                                     const uint8_t (&orau8_IpAddress)[4], const uint8_t (&orau8_NetMask)[4],
                                     const uint8_t(&orau8_DefaultGateway)[4], uint8_t * const opu8_NrCode = NULL);

   //Application layer routing for "legacy" protocols
   int32_t OsySetRouteDiagnosisCommunication(const uint8_t ou8_InputChannelType, const uint8_t ou8_InputChannelIndex,
                                             const uint8_t ou8_OutputChannelType, const uint8_t ou8_OutputChannelIndex,
                                             const uint8_t ou8_SourceBusId, const uint8_t ou8_TargetBusId,
                                             uint8_t * const opu8_NrCode = NULL);
   int32_t OsyStopRouteDiagnosisCommunication(uint8_t * const opu8_NrCode = NULL);
   int32_t OsySetRouteIp2IpCommunication(const uint8_t ou8_OutputChannelType, const uint8_t ou8_OutputChannelIndex,
                                         const uint8_t ou8_SourceBusId, const uint8_t ou8_TargetBusId,
                                         const uint8_t (&orau8_IpAddress)[4], uint8_t * const opu8_NrCode = NULL);
   int32_t OsyCheckRouteIp2IpCommunication(uint8_t & oru8_Status, uint8_t * const opu8_NrCode = NULL);
   int32_t OsySendCanMessage(const uint8_t ou8_ChannelIndex, const stw::can::T_STWCAN_Msg_TX & orc_CanMessage,
                             uint8_t * const opu8_NrCode = NULL);
   int32_t OsySetTunnelCanMessages(const uint8_t ou8_CanChannelIndex, const uint32_t ou32_FilterId,
                                   const uint32_t ou32_FilterMask, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyStopTunnelCanMessages(uint8_t * const opu8_NrCode = NULL);

   //Data pool access:
   int32_t OsyReadDataPoolData(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                               const uint16_t ou16_ElementIndex, std::vector<uint8_t> & orc_ReadData,
                               uint8_t * const opu8_NrCode = NULL);
   int32_t OsyWriteDataPoolData(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                const uint16_t ou16_ElementIndex, const std::vector<uint8_t> & orc_DataToWrite,
                                uint8_t * const opu8_NrCode = NULL);
   int32_t OsyWriteDataPoolEventDataRate(const uint8_t ou8_TransmissionRail, const uint16_t ou16_DataRate,
                                         uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadDataPoolDataCyclic(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                     const uint16_t ou16_ElementIndex, const uint8_t ou8_TransmissionRail,
                                     uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadDataPoolDataChangeDriven(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                           const uint16_t ou16_ElementIndex, const uint8_t ou8_TransmissionRail,
                                           const uint32_t ou32_Hysteresis, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyStopDataPoolEvents(uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadDataPoolMetaData(const uint8_t ou8_DataPoolIndex, C_DataPoolMetaData & orc_MetaData,
                                   uint8_t * const opu8_NrCode = NULL);
   int32_t OsyVerifyDataPool(const uint8_t ou8_DataPoolIndex, const uint32_t ou32_DataPoolChecksum, bool & orq_Match,
                             uint8_t * const opu8_NrCode = NULL);
   //NVM access:
   int32_t OsyReadMemoryByAddress(const uint32_t ou32_MemoryAddress, std::vector<uint8_t> & orc_DataRecord,
                                  uint8_t * const opu8_NrCode = NULL);
   int32_t OsyWriteMemoryByAddress(const uint32_t ou32_MemoryAddress, const std::vector<uint8_t> & orc_DataRecord,
                                   uint8_t * const opu8_NrCode = NULL);
   int32_t OsyNotifyNvmDataChanges(const uint8_t ou8_DataPoolIndex, const uint8_t ou8_ListIndex,
                                   bool & orq_ApplicationAcknowledge, uint8_t * const opu8_NrCode = NULL);

   //Flashloader
   int32_t OsyRequestProgramming(uint8_t * const opu8_NrCode = NULL);
   int32_t OsyCheckFlashMemoryAvailable(const uint32_t ou32_StartAddress, const uint32_t ou32_Size,
                                        uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadFlashBlockData(const uint8_t ou8_FlashBlock, C_FlashBlockInfo & orc_BlockInfo,
                                 uint8_t * const opu8_NrCode = NULL);
   int32_t OsySecurityAccessRequestSeed(const uint8_t ou8_SecurityLevel, bool & orq_SecureMode, uint64_t & oru64_Seed,
                                        uint8_t & oru8_SecurityAlgorithm, uint8_t * const opu8_NrCode = NULL);
   int32_t OsySecurityAccessSendKey(const uint8_t ou8_SecurityLevel, const uint32_t ou32_Key,
                                    uint8_t * const opu8_NrCode = NULL);
   int32_t OsySecurityAccessSendKey(const uint8_t ou8_SecurityLevel, const std::vector<uint8_t> & orc_Key,
                                    uint8_t * const opu8_NrCode = NULL);
   int32_t OsyRequestDownload(const uint32_t ou32_StartAddress, const uint32_t ou32_Size,
                              uint32_t & oru32_MaxBlockLength, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyRequestFileTransfer(const stw::scl::C_SclString & orc_FilePath, const uint32_t ou32_FileSize,
                                  uint32_t & oru32_MaxBlockLength, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyTransferData(const uint8_t ou8_BlockSequenceCounter, std::vector<uint8_t> & orc_Data,
                           uint8_t * const opu8_NrCode = NULL);
   int32_t OsyRequestTransferExitAddressBased(const bool oq_SendSignatureBlockAddress,
                                              const uint32_t ou32_SignatureBlockAddress,
                                              uint8_t * const opu8_NrCode = NULL);
   int32_t OsyRequestTransferExitFileBased(const uint8_t(&orau8_Signature)[8], uint8_t * const opu8_NrCode = NULL);
   int32_t OsyWriteApplicationSoftwareFingerprint(const uint8_t (&orau8_Date)[3], const uint8_t (&orau8_Time)[3],
                                                  const stw::scl::C_SclString & orc_UserName,
                                                  uint8_t * const opu8_NrCode = NULL);
   int32_t OsyFactoryMode(const uint8_t ou8_Operation, uint8_t * const opu8_NrCode = NULL);

   //Security
   int32_t OsyReadCertificateSerialNumber(std::vector<uint8_t> & orc_SerialNumber, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadCertificateSerialNumberL7(std::vector<uint8_t> & orc_SerialNumber,
                                            uint8_t * const opu8_NrCode = NULL);
   int32_t OsyWriteSecurityKey(const std::vector<uint8_t> & orc_PublicKeyModulus,
                               const std::vector<uint8_t> & orc_PublicKeyExponent,
                               const std::vector<uint8_t> & orc_CertificateSerialNumber,
                               uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadSecurityActivation(bool & orq_SecurityOn, uint8_t & oru8_SecurityAlgorithm,
                                     uint8_t * const opu8_NrCode = NULL);
   int32_t OsyWriteSecurityActivation(const bool oq_SecurityOn, const uint8_t ou8_SecurityAlgorithm,
                                      uint8_t * const opu8_NrCode = NULL);
   int32_t OsyReadDebuggerEnabled(bool & orq_DebuggerEnabled, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyWriteDebuggerEnabled(const bool oq_DebuggerEnabled, uint8_t * const opu8_NrCode = NULL);

   //Common
   int32_t OsyTesterPresent(const uint8_t ou8_SuppressResponseMsg, uint8_t * const opu8_NrCode = NULL);
   int32_t OsyEcuReset(const uint8_t ou8_ResetType = C_OscProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_KEY_OFF_ON);

   // Default Timeout
   static const uint32_t hu32_DEFAULT_TIMEOUT = 1000U; // In ms
   // Cyclic time till the registered function mpr_OnOsyWaitTime will be called in m_PollForSpecificServiceResponse
   static const uint32_t hu32_DEFAULT_HANDLE_WAIT_TIME = 2000U; // In ms
   // Sessions for service OsyDiagnosticSessionControl
   static const uint8_t hu8_DIAGNOSTIC_SESSION_DEFAULT            = 0x01U;
   static const uint8_t hu8_DIAGNOSTIC_SESSION_EXTENDED_DIAGNOSIS = 0x03U;
   static const uint8_t hu8_DIAGNOSTIC_SESSION_PREPROGRAMMING     = 0x60U;
   static const uint8_t hu8_DIAGNOSTIC_SESSION_PROGRAMMING        = 0x02U;

   //negative response codes:
   static const uint8_t hu8_NR_CODE_SERVICE_NOT_SUPPORTED              = 0x11U;
   static const uint8_t hu8_NR_CODE_SUB_FUNCTION_NOT_SUPPORTED         = 0x12U;
   static const uint8_t hu8_NR_CODE_INCORRECT_MESSAGE_LENGTH_OR_FORMAT = 0x13U;
   static const uint8_t hu8_NR_CODE_RESPONSE_TOO_LONG                  = 0x14U;
   static const uint8_t hu8_NR_CODE_CONDITIONS_NOT_CORRECT             = 0x22U;
   static const uint8_t hu8_NR_CODE_REQUEST_SEQUENCE_ERROR             = 0x24U;
   static const uint8_t hu8_NR_CODE_REQUEST_OUT_OF_RANGE               = 0x31U;
   static const uint8_t hu8_NR_CODE_SECURITY_ACCESS_DENIED             = 0x33U;
   static const uint8_t hu8_NR_CODE_INVALID_KEY                        = 0x35U;
   static const uint8_t hu8_NR_CODE_EXCEEDED_NUMBER_OF_ATTEMPTS        = 0x36U;
   static const uint8_t hu8_NR_CODE_REQUIRED_TIME_DELAY_NOT_EXPIRED    = 0x37U;
   static const uint8_t hu8_NR_CODE_UPLOAD_DOWNLOAD_NOT_ACCEPTED       = 0x70U;
   static const uint8_t hu8_NR_CODE_GENERAL_PROGRAMMING_FAILURE        = 0x72U;
   static const uint8_t hu8_NR_CODE_RESPONSE_PENDING                   = 0x78U;
   static const uint8_t hu8_NR_CODE_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION = 0x7FU;

   //result status of routine OSY_RC_SID_ROUTE_IP_2_IP_COMMUNICATION
   static const uint8_t hu8_OSY_IP_2_IP_STATUS_IDLE        = 0U;
   static const uint8_t hu8_OSY_IP_2_IP_STATUS_IN_PROGRESS = 1U;
   static const uint8_t hu8_OSY_IP_2_IP_STATUS_ERROR       = 2U;
   static const uint8_t hu8_OSY_IP_2_IP_STATUS_CONNECTED   = 3U;

   // factory mode operations
   static const uint8_t hu8_OSY_FACTORY_MODE_MASTER_RESET = 0U;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif // C_OscPROTOCOLDRIVEROSY_HPP
