//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol IP transport-protocol

   \class       stw::opensyde_core::C_OscProtocolDriverOsyTpIp
   \brief       openSYDE protocol IP transport-protocol

   Client implementation of IP-TP

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROTOCOLDRIVEROSYTPIPH
#define C_OSCPROTOCOLDRIVEROSYTPIPH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.hpp"
#include "C_OscProtocolDriverOsyTpBase.hpp"
#include "C_OscProtocolSerialNumber.hpp"
#include "C_OscIpDispatcher.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscProtocolDriverOsyTpIp :
   public C_OscProtocolDriverOsyTpBase
{
private:
   ///DoIp protocol header
   class C_DoIpHeader
   {
   public:
      //constants:
      static const uint8_t hu8_DOIP_HEADER_SIZE = 0x08U; ///< size of DoIp header in bytes
      static const uint16_t hu16_PAYLOAD_TYPE_DIAGNOSTIC_MESSAGE  = 0x8001U;
      static const uint16_t hu16_PAYLOAD_TYPE_OSY_GET_DEVICE_INFO_REQ = 0xF000U;
      static const uint16_t hu16_PAYLOAD_TYPE_OSY_GET_DEVICE_INFO_RES = 0xF001U;
      static const uint16_t hu16_PAYLOAD_TYPE_OSY_GET_DEVICE_INFO_EXT_RES = 0xF009U;
      static const uint16_t hu16_PAYLOAD_TYPE_SET_IP_ADDRESS_MESSAGE_REQ = 0xF002U;
      static const uint16_t hu16_PAYLOAD_TYPE_SET_IP_ADDRESS_MESSAGE_RES = 0xF003U;
      static const uint16_t hu16_PAYLOAD_TYPE_SET_IP_ADDRESS_MESSAGE_EXT_REQ = 0xF00BU;
      static const uint16_t hu16_PAYLOAD_TYPE_SET_IP_ADDRESS_MESSAGE_EXT_RES = 0xF00CU;
      static const uint16_t hu16_PAYLOAD_TYPE_NET_RESET_MESSAGE_REQ = 0xF004U;
      static const uint16_t hu16_PAYLOAD_TYPE_REQUEST_PROGRAMMING_REQ = 0xF006U;
      static const uint16_t hu16_PAYLOAD_TYPE_REQUEST_PROGRAMMING_RES = 0xF007U;
      static const uint16_t hu16_PAYLOAD_TYPE_CAN_ROUTING_WITHOUT_FC_REQ = 0xF100U;

      C_DoIpHeader(void);
      C_DoIpHeader(const uint16_t ou16_PayloadType, const uint32_t ou32_PayloadSize);

      uint16_t u16_PayloadType;
      uint32_t u32_PayloadSize;

      int32_t DecodeHeader(const std::vector<uint8_t> & orc_Header);
      void ComposeHeader(std::vector<uint8_t> & orc_Header) const;
   };

   ///information about ongoing TCP Rx transmission
   class C_TcpRxServiceState
   {
   public:
      C_TcpRxServiceState(void);

      enum E_ServiceStatus
      {
         eIDLE,
         ePARTIAL
      };

      C_DoIpHeader c_ServiceHeader;

      E_ServiceStatus e_Status;
   };

   C_TcpRxServiceState mc_RxState;

   C_OscIpDispatcher * mpc_Dispatcher; ///< driver to use for accessing local IP stack
   uint32_t mu32_DispatcherHandle;     ///< handler to use for dispatcher TDP functions

   void m_ComposeRequest(const C_OscProtocolDriverOsyService & orc_Service, std::vector<uint8_t> & orc_Request) const;

protected:
   void m_LogWarningWithHeader(const stw::scl::C_SclString & orc_Information, const char_t * const opcn_Function) const;
   void m_LogWarningWithHeaderAndIp(const stw::scl::C_SclString & orc_Information, const char_t * const opcn_Function,
                                    const uint8_t(&orau8_Ip)[4]) const;

public:
   ///container for results reported by "GetDeviceInfo" UDP service
   class C_BroadcastGetDeviceInfoResults
   {
   public:
      virtual ~C_BroadcastGetDeviceInfoResults(void);

      uint8_t au8_IpAddress[4];                 ///< IP address of device
      C_OscProtocolDriverOsyNode c_NodeId;      ///< node ID of device
      C_OscProtocolSerialNumber c_SerialNumber; ///< serial number of device
      stw::scl::C_SclString c_DeviceName;       ///< name of device

      virtual bool operator ==(const C_BroadcastGetDeviceInfoResults & orc_Cmp) const;
      virtual bool operator <(const C_BroadcastGetDeviceInfoResults & orc_Cmp) const;
      virtual void ParseFromArray(const std::vector<uint8_t> & orc_Data, const uint8_t ou8_DataStartIndex);
   };

   class C_BroadcastGetDeviceInfoExtendedResults :
      public C_BroadcastGetDeviceInfoResults
   {
   public:
      uint8_t u8_SubNodeId;     ///< ID of sub node in case of multi CPU devices. Default is 0
      bool q_SecurityActivated; ///< flag if node has security feature activated

      virtual bool operator ==(const C_BroadcastGetDeviceInfoExtendedResults & orc_Cmp) const;
      virtual bool operator <(const C_BroadcastGetDeviceInfoExtendedResults & orc_Cmp) const;
      virtual void ParseFromArray(const std::vector<uint8_t> & orc_Data, const uint8_t ou8_DataStartIndex);
   };

   ///container for results reported by "RequestProgramming" UDP service
   class C_BroadcastRequestProgrammingResults
   {
   public:
      uint8_t au8_IpAddress[4];            ///< IP address of device
      C_OscProtocolDriverOsyNode c_NodeId; ///< node ID of device
      bool q_RequestAccepted;              ///< true: no problems; false; flag not set
   };

   explicit C_OscProtocolDriverOsyTpIp(const uint16_t ou16_MaxServiceQueueSize = 200U);
   virtual ~C_OscProtocolDriverOsyTpIp(void);

   virtual int32_t Cycle(void);

   //Tp-specific functions:
   int32_t SetDispatcher(C_OscIpDispatcher * const opc_Dispatcher, const uint32_t ou32_DispatcherHandle);

   virtual int32_t IsConnected(void);
   virtual int32_t ReConnect(void);
   virtual int32_t Disconnect(void);

   //Tp-specific broadcast services:
   int32_t BroadcastGetDeviceInfo(std::vector<C_BroadcastGetDeviceInfoResults> & orc_DeviceInfos,
                                  std::vector<C_BroadcastGetDeviceInfoExtendedResults> & orc_DeviceExtendedInfos)
   const;
   int32_t BroadcastSetIpAddress(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber,
                                 const uint8_t(&orau8_NewIpAddress)[4],
                                 const uint8_t(&orau8_NetMask)[4],
                                 const uint8_t(&orau8_DefaultGateway)[4],
                                 const C_OscProtocolDriverOsyNode &orc_NewNodeId,
                                 uint8_t(&orau8_ResponseIp)[4],
                                 uint8_t * const opu8_ErrorResult = NULL) const;
   int32_t BroadcastSetIpAddressExtended(
      const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber,
      const uint8_t(&orau8_NewIpAddress)[4],
      const uint8_t(&orau8_NetMask)[4],
      const uint8_t(&orau8_DefaultGateway)[4],
      const C_OscProtocolDriverOsyNode &orc_NewNodeId,
      const uint8_t ou8_SubNodeId,
      uint8_t(&orau8_ResponseIp)[4],
      uint8_t * const opu8_ErrorResult = NULL) const;
   int32_t BroadcastRequestProgramming(std::vector<C_BroadcastRequestProgrammingResults> & orc_Results) const;
   int32_t BroadcastNetReset(const uint8_t ou8_ResetType, const bool oq_SpecificSerialNumberOnly = false,
                             const uint8_t (*const opau8_SerialNumber)[6] = NULL) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
