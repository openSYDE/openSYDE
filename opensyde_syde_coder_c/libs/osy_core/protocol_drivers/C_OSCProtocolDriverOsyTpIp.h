//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol IP transport-protocol

   \class       stw_opensyde_core::C_OSCProtocolDriverOsyTpIp
   \brief       openSYDE protocol IP transport-protocol

   Client implementation of IP-TP

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROTOCOLDRIVEROSYTPIPH
#define C_OSCPROTOCOLDRIVEROSYTPIPH

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "stwtypes.h"
#include "C_OSCProtocolDriverOsyTpBase.h"
#include "C_OSCIpDispatcher.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCProtocolDriverOsyTpIp :
   public C_OSCProtocolDriverOsyTpBase
{
private:
   ///DoIp protocol header
   class C_DoIpHeader
   {
   public:
      //constants:
      static const stw_types::uint8 hu8_DOIP_HEADER_SIZE = 0x08U; ///< size of DoIp header in bytes
      static const stw_types::uint16 hu16_PAYLOAD_TYPE_DIAGNOSTIC_MESSAGE  = 0x8001U;
      static const stw_types::uint16 hu16_PAYLOAD_TYPE_OSY_GET_DEVICE_INFO_REQ = 0xF000U;
      static const stw_types::uint16 hu16_PAYLOAD_TYPE_OSY_GET_DEVICE_INFO_RES = 0xF001U;
      static const stw_types::uint16 hu16_PAYLOAD_TYPE_SET_IP_ADDRESS_MESSAGE_REQ = 0xF002U;
      static const stw_types::uint16 hu16_PAYLOAD_TYPE_SET_IP_ADDRESS_MESSAGE_RES = 0xF003U;
      static const stw_types::uint16 hu16_PAYLOAD_TYPE_NET_RESET_MESSAGE_REQ = 0xF004U;
      static const stw_types::uint16 hu16_PAYLOAD_TYPE_REQUEST_PROGRAMMING_REQ = 0xF006U;
      static const stw_types::uint16 hu16_PAYLOAD_TYPE_REQUEST_PROGRAMMING_RES = 0xF007U;
      static const stw_types::uint16 hu16_PAYLOAD_TYPE_CAN_ROUTING_WITHOUT_FC_REQ = 0xF100U;

      C_DoIpHeader(void);
      C_DoIpHeader(const stw_types::uint16 ou16_PayloadType, const stw_types::uint32 ou32_PayloadSize);

      stw_types::uint16 u16_PayloadType;
      stw_types::uint32 u32_PayloadSize;

      stw_types::sint32 DecodeHeader(const std::vector<stw_types::uint8> & orc_Header);
      void ComposeHeader(std::vector<stw_types::uint8> & orc_Header) const;
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

   C_OSCIpDispatcher * mpc_Dispatcher;      ///< driver to use for accessing local IP stack
   stw_types::uint32 mu32_DispatcherHandle; ///< handler to use for dispatcher TDP functions

   void m_ComposeRequest(const C_OSCProtocolDriverOsyService & orc_Service,
                         std::vector<stw_types::uint8> & orc_Request) const;

protected:
   void m_LogWarningWithHeader(const stw_scl::C_SCLString & orc_Information,
                               const stw_types::charn * const opcn_Function) const;
   void m_LogWarningWithHeaderAndIp(const stw_scl::C_SCLString & orc_Information,
                                    const stw_types::charn * const opcn_Function,
                                    const stw_types::uint8(&orau8_Ip)[4]) const;

public:
   ///container for results reported by "GetDeviceInfo" UDP service
   class C_BroadcastGetDeviceInfoResults
   {
   public:
      stw_types::uint8 au8_IpAddress[4];    ///< IP address of device
      C_OSCProtocolDriverOsyNode c_NodeId;  ///< node ID of device
      stw_types::uint8 au8_SerialNumber[6]; ///< serial number of device
      stw_scl::C_SCLString c_DeviceName;    ///< name of device

      bool operator ==(const C_BroadcastGetDeviceInfoResults & orc_Cmp) const;
      bool operator <(const C_BroadcastGetDeviceInfoResults & orc_Cmp) const;
      void ParseFromArray(const std::vector<stw_types::uint8> & orc_Data, const stw_types::uint8 ou8_DataStartIndex);
   };

   ///container for results reported by "RequestProgramming" UDP service
   class C_BroadcastRequestProgrammingResults
   {
   public:
      stw_types::uint8 au8_IpAddress[4];   ///< IP address of device
      C_OSCProtocolDriverOsyNode c_NodeId; ///< node ID of device
      bool q_RequestAccepted;              ///< true: no problems; false; flag not set
   };

   C_OSCProtocolDriverOsyTpIp(const stw_types::uint16 ou16_MaxServiceQueueSize = 200U);
   virtual ~C_OSCProtocolDriverOsyTpIp(void);

   virtual stw_types::sint32 Cycle(void);

   //Tp-specific functions:
   stw_types::sint32 SetDispatcher(C_OSCIpDispatcher * const opc_Dispatcher,
                                   const stw_types::uint32 ou32_DispatcherHandle);

   virtual stw_types::sint32 IsConnected(void);
   virtual stw_types::sint32 ReConnect(void);
   virtual stw_types::sint32 Disconnect(void);

   //Tp-specific broadcast services:
   stw_types::sint32 BroadcastGetDeviceInfo(std::vector<C_BroadcastGetDeviceInfoResults> & orc_DeviceInfos) const;
   stw_types::sint32 BroadcastSetIpAddress(const stw_types::uint8(&orau8_SerialNumber)[6],
                                           const stw_types::uint8(&orau8_NewIpAddress)[4],
                                           const stw_types::uint8(&orau8_NetMask)[4],
                                           const stw_types::uint8(&orau8_DefaultGateway)[4],
                                           const C_OSCProtocolDriverOsyNode &orc_NewNodeId,
                                           stw_types::uint8(&orau8_ResponseIp)[4],
                                           stw_types::uint8 * const opu8_ErrorResult = NULL) const;
   stw_types::sint32 BroadcastRequestProgramming(std::vector<C_BroadcastRequestProgrammingResults> & orc_Results) const;
   stw_types::sint32 BroadcastNetReset(const stw_types::uint8 ou8_ResetType,
                                       const bool oq_SpecificSerialNumberOnly = false,
                                       const stw_types::uint8 (*const opau8_SerialNumber)[6] = NULL) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
