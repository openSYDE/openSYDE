//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol CAN transport-protocol

   \class       stw::opensyde_core::C_OscProtocolDriverOsyTpCan
   \brief       openSYDE protocol CAN transport-protocol

   Client implementation of CAN-TP

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROTOCOLDRIVEROSYTPCAN_HPP
#define C_OSCPROTOCOLDRIVEROSYTPCAN_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscProtocolDriverOsyTpBase.hpp"
#include "C_OscProtocolSerialNumber.hpp"
#include "C_CanDispatcher.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscProtocolDriverOsyTpCan :
   public C_OscProtocolDriverOsyTpBase
{
private:
   ///information about ongoing transmission or reception
   class C_ServiceState
   {
   public:
      C_ServiceState(void);

      enum E_ServiceStatus
      {
         eIDLE,
         eWAITING_FOR_FLOW_CONTROL,       //Tx only
         eWAITING_FOR_CONSECUTIVE_FRAME,  //Rx only
         eMORE_CONSECUTIVE_FRAMES_TO_SEND //Tx only
      };

      C_OscProtocolDriverOsyService c_ServiceData;
      uint16_t u16_TransmissionIndex; ///< read resp. write index in service data
      uint8_t u8_SequenceNumber;      ///< next sequence number to send or expect
      E_ServiceStatus e_Status;       ///< status of service
      uint32_t u32_StartTimeMs;       ///< Tx: time the transmission was started; used for checking for "N_Bs"
      // timeout
      uint32_t u32_SendCfTimeout; ///< Tx:time at which sending all CFs should have been finished; set upon
      ///< reception of FC
   };

   ///CAN-TP PCI types:
   static const uint8_t mhu8_ISO15765_N_PCI_SF  = 0x00U; // single frame <=8bytes
   static const uint8_t mhu8_ISO15765_N_PCI_FF  = 0x10U; // first frame <=4095bytes
   static const uint8_t mhu8_ISO15765_N_PCI_CF  = 0x20U; // consecutive frame
   static const uint8_t mhu8_ISO15765_N_PCI_FC  = 0x30U; // flow control
   static const uint8_t mhu8_ISO15765_N_PCI_OSF = 0xF0U; // openSYDE single frame (special handling of message
   // depending on lower nibble)
   static const uint8_t mhu8_ISO15765_N_PCI_OMF = 0xE0U; // openSYDE multi frame
   // (no flow control, SN 0, SN 15->1)

   ///openSYDE single frame message types (lower nibble)
   static const uint8_t mhu8_OSY_OSF_TYPE_EVENT_DRIVEN_DP_SINGLE_FRAME = 0x01U; // openSYDE single frame
   // without response SID=0xFA

   ///constants for broadcast services (same as for non-broadcast for this TP)
   static const uint8_t mhu8_OSY_BC_SI_DIAGNOSTIC_SESSION_CONTROL = 0x10U;
   static const uint8_t mhu8_OSY_BC_SI_ECU_RESET                  = 0x11U;
   static const uint8_t mhu8_OSY_BC_SI_ROUTINE_CONTROL            = 0x31U;
   static const uint8_t mhu8_OSY_BC_SI_READ_SERIAL_NUMBER         = 0xBDU;
   static const uint8_t mhu8_OSY_BC_RC_SUB_FUNCTION_START_ROUTINE = 0x01U;
   static const uint16_t mhu16_OSY_BC_RC_SID_REQUEST_PROGRAMMING               = 0x0206U;
   static const uint16_t mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART1  = 0x0216U;
   static const uint16_t mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART2  = 0x0217U;
   static const uint16_t mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART3  = 0x0218U;
   static const uint16_t mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_EXT_START  = 0x0219U;
   static const uint16_t mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_EXT_LAST  = 0x0224U;
   static const uint8_t mhu8_BC_OSY_NR_SI = 0x7FU;

   ///timeout when waiting for flow control; lower but more realistic than the 1s specified in 15765
   static const uint16_t mhu16_NBS_TIMEOUTS_MS = 100U;

   stw::can::C_CanDispatcher * mpc_CanDispatcher; ///< CAN dispatcher to use for communication
   uint16_t mu16_DispatcherClientHandle;          ///< our handle for dispatcher interaction

   C_ServiceState mc_TxService; ///< status of Tx service currently ongoing
   C_ServiceState mc_RxService; ///< status of Rx service currently ongoing

   int32_t m_SetRxFilter(const bool oq_ForBroadcast);
   uint32_t m_GetTxIdentifier(void) const;
   uint32_t m_GetTxBroadcastIdentifier(void) const;

   int32_t m_HandleIncomingSingleFrame(const stw::can::T_STWCAN_Msg_RX & orc_CanMessage);
   int32_t m_HandleIncomingOsySpecificSingleFrame(const stw::can::T_STWCAN_Msg_RX & orc_CanMessage);
   int32_t m_HandleIncomingFirstFrame(const stw::can::T_STWCAN_Msg_RX & orc_CanMessage);
   int32_t m_HandleIncomingFlowControl(const stw::can::T_STWCAN_Msg_RX & orc_CanMessage);
   int32_t m_HandleIncomingConsecutiveFrame(const stw::can::T_STWCAN_Msg_RX & orc_CanMessage);
   int32_t m_HandleIncomingOsySpecificMultiFrame(const stw::can::T_STWCAN_Msg_RX & orc_CanMessage);
   int32_t m_BroadcastSendDiagnosticSessionControl(const uint8_t ou8_Session) const;

   int32_t m_SendNextConsecutiveFrames(void);

   static void mh_ComposeSingleFrame(const C_OscProtocolDriverOsyService & orc_Service, const uint32_t ou32_Identifier,
                                     stw::can::T_STWCAN_Msg_TX & orc_CanMessage);

   int32_t m_HandleBroadcastSetNodeIdBySerialNumberResponse(const uint8_t ou8_RoutineIdMsb,
                                                            const uint8_t ou8_RoutineIdLsb,
                                                            uint8_t * const opu8_NrCode = NULL) const;

protected:
   void m_LogWarningWithHeader(const stw::scl::C_SclString & orc_Information, const char_t * const opcn_Function) const;

public:
   ///container for results reported by "ReadSerialNumber" broadcast service
   class C_BroadcastReadEcuSerialNumberResults
   {
   public:
      C_OscProtocolDriverOsyNode c_SenderId;    ///< bus ID and node ID of sender
      C_OscProtocolSerialNumber c_SerialNumber; ///< serial number of sender
   };

   class C_BroadcastReadEcuSerialNumberExtendedResults :
      public C_BroadcastReadEcuSerialNumberResults
   {
   public:
      uint8_t u8_SubNodeId;     ///< sub node id of sender in case of a multiple CPU device
      bool q_SecurityActivated; ///< flag if node has security feature activated
   };

   ///container for results reported by "RequestProgramming" broadcast service
   class C_BroadcastRequestProgrammingResults
   {
   public:
      C_OscProtocolDriverOsyNode c_SenderId; ///< bus ID and node ID of sender
      bool q_RequestAccepted;                ///< true: no problems; false; flag not set
   };

   explicit C_OscProtocolDriverOsyTpCan(const uint16_t ou16_MaxServiceQueueSize = 200U);
   virtual ~C_OscProtocolDriverOsyTpCan(void);

   virtual int32_t Cycle(void);
   virtual int32_t SetNodeIdentifiers(const C_OscProtocolDriverOsyNode & orc_ClientIdentifier,
                                      const C_OscProtocolDriverOsyNode & orc_ServerIdentifier);
   int32_t SetNodeIdentifiersForBroadcasts(const C_OscProtocolDriverOsyNode & orc_ClientIdentifier);

   int32_t SetDispatcher(stw::can::C_CanDispatcher * const opc_Dispatcher);

   //Tp-specific broadcast services:
   int32_t BroadcastReadSerialNumber(std::vector<C_BroadcastReadEcuSerialNumberResults> & orc_Responses,
                                     std::vector<C_BroadcastReadEcuSerialNumberExtendedResults> & orc_ExtendedResponses)
   const;
   int32_t BroadcastRequestProgramming(std::vector<C_BroadcastRequestProgrammingResults> & orc_Results) const;
   int32_t BroadcastSetNodeIdBySerialNumber(const C_OscProtocolSerialNumber & orc_SerialNumber,
                                            const C_OscProtocolDriverOsyNode & orc_NewNodeId,
                                            uint8_t * const opu8_NrCode = NULL) const;
   int32_t BroadcastSetNodeIdBySerialNumberExtended(const C_OscProtocolSerialNumber & orc_SerialNumber,
                                                    const uint8_t ou8_SubNodeId,
                                                    const C_OscProtocolDriverOsyNode & orc_NewNodeId,
                                                    uint8_t * const opu8_NrCode = NULL) const;
   int32_t BroadcastEcuReset(const uint8_t ou8_ResetType) const;
   int32_t BroadcastSendEnterPreProgrammingSession(void) const;
   int32_t BroadcastSendEnterDefaultSession(void) const;

   void ClearDispatcherQueue(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
