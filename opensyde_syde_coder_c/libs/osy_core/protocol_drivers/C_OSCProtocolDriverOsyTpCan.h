//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol CAN transport-protocol

   \class       stw_opensyde_core::C_OSCProtocolDriverOsyTpCan
   \brief       openSYDE protocol CAN transport-protocol

   Client implementation of CAN-TP

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROTOCOLDRIVEROSYTPCANH
#define C_OSCPROTOCOLDRIVEROSYTPCANH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OSCProtocolDriverOsyTpBase.h"
#include "CCANDispatcher.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCProtocolDriverOsyTpCan :
   public C_OSCProtocolDriverOsyTpBase
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

      C_OSCProtocolDriverOsyService c_ServiceData;
      stw_types::uint16 u16_TransmissionIndex; ///< read resp. write index in service data
      stw_types::uint8 u8_SequenceNumber;      ///< next sequence number to send or expect
      E_ServiceStatus e_Status;                ///< status of service
      stw_types::uint32 u32_StartTimeMs;       ///< Tx: time the transmission was started; used for checking for "N_Bs"
                                               // timeout
      stw_types::uint32 u32_SendCfTimeout;     ///< Tx:time at which sending all CFs should have been finished; set upon
      ///< reception of FC
   };

   ///CAN-TP PCI types:
   static const stw_types::uint8 mhu8_ISO15765_N_PCI_SF  = 0x00U; // single frame <=8bytes
   static const stw_types::uint8 mhu8_ISO15765_N_PCI_FF  = 0x10U; // first frame <=4095bytes
   static const stw_types::uint8 mhu8_ISO15765_N_PCI_CF  = 0x20U; // consecutive frame
   static const stw_types::uint8 mhu8_ISO15765_N_PCI_FC  = 0x30U; // flow control
   static const stw_types::uint8 mhu8_ISO15765_N_PCI_OSF = 0xF0U; // openSYDE single frame (special handling of message
                                                                  // depending on lower nibble)
   static const stw_types::uint8 mhu8_ISO15765_N_PCI_OMF = 0xE0U; // openSYDE multi frame
                                                                  // (no flow control, SN 0, SN 15->1)

   ///openSYDE single frame message types (lower nibble)
   static const stw_types::uint8 mhu8_OSY_OSF_TYPE_EVENT_DRIVEN_DP_SINGLE_FRAME = 0x01U; // openSYDE single frame
                                                                                         // without response SID=0xFA

   ///constants for broadcast services (same as for non-broadcast for this TP)
   static const stw_types::uint8 mhu8_OSY_BC_SI_DIAGNOSTIC_SESSION_CONTROL = 0x10U;
   static const stw_types::uint8 mhu8_OSY_BC_SI_ECU_RESET                  = 0x11U;
   static const stw_types::uint8 mhu8_OSY_BC_SI_ROUTINE_CONTROL            = 0x31U;
   static const stw_types::uint8 mhu8_OSY_BC_SI_READ_SERIAL_NUMBER         = 0xBDU;
   static const stw_types::uint8 mhu8_OSY_BC_RC_SUB_FUNCTION_START_ROUTINE = 0x01U;
   static const stw_types::uint16 mhu16_OSY_BC_RC_SID_REQUEST_PROGRAMMING               = 0x0206U;
   static const stw_types::uint16 mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART1  = 0x0216U;
   static const stw_types::uint16 mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART2  = 0x0217U;
   static const stw_types::uint16 mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART3  = 0x0218U;
   static const stw_types::uint8 mhu8_BC_OSY_NR_SI = 0x7FU;

   ///timeout when waiting for flow control; lower but more realistic than the 1s specified in 15765
   static const stw_types::uint16 mhu16_NBsTimeoutMs = 100U;

   stw_can::C_CAN_Dispatcher * mpc_CanDispatcher; ///< CAN dispatcher to use for communication
   stw_types::uint16 mu16_DispatcherClientHandle; ///< our handle for dispatcher interaction

   C_ServiceState mc_TxService; ///< status of Tx service currently ongoing
   C_ServiceState mc_RxService; ///< status of Rx service currently ongoing

   stw_types::sint32 m_SetRxFilter(const bool oq_ForBroadcast);
   stw_types::uint32 m_GetTxIdentifier(void) const;
   stw_types::uint32 m_GetTxBroadcastIdentifier(void) const;

   stw_types::sint32 m_HandleIncomingSingleFrame(const stw_can::T_STWCAN_Msg_RX & orc_CanMessage);
   stw_types::sint32 m_HandleIncomingOsySpecificSingleFrame(const stw_can::T_STWCAN_Msg_RX & orc_CanMessage);
   stw_types::sint32 m_HandleIncomingFirstFrame(const stw_can::T_STWCAN_Msg_RX & orc_CanMessage);
   stw_types::sint32 m_HandleIncomingFlowControl(const stw_can::T_STWCAN_Msg_RX & orc_CanMessage);
   stw_types::sint32 m_HandleIncomingConsecutiveFrame(const stw_can::T_STWCAN_Msg_RX & orc_CanMessage);
   stw_types::sint32 m_HandleIncomingOsySpecificMultiFrame(const stw_can::T_STWCAN_Msg_RX & orc_CanMessage);
   stw_types::sint32 m_BroadcastSendDiagnosticSessionControl(const stw_types::uint8 ou8_Session) const;

   stw_types::sint32 m_SendNextConsecutiveFrames(void);

   void m_ComposeSingleFrame(const C_OSCProtocolDriverOsyService & orc_Service, const stw_types::uint32 ou32_Identifier,
                             stw_can::T_STWCAN_Msg_TX & orc_CanMessage) const;

protected:
   void m_LogWarningWithHeader(const stw_scl::C_SCLString & orc_Information,
                               const stw_types::charn * const opcn_Function) const;

public:
   ///container for results reported by "ReadSerialNumber" broadcast service
   class C_BroadcastReadEcuSerialNumberResults
   {
   public:
      C_OSCProtocolDriverOsyNode c_SenderId; ///< bus ID and node ID of sender
      stw_types::uint8 au8_SerialNumber[6];  ///< serial number of sender
   };

   ///container for results reported by "RequestProgramming" broadcast service
   class C_BroadcastRequestProgrammingResults
   {
   public:
      C_OSCProtocolDriverOsyNode c_SenderId; ///< bus ID and node ID of sender
      bool q_RequestAccepted;                ///< true: no problems; false; flag not set
   };

   C_OSCProtocolDriverOsyTpCan(const stw_types::uint16 ou16_MaxServiceQueueSize = 200U);
   virtual ~C_OSCProtocolDriverOsyTpCan(void);

   virtual stw_types::sint32 Cycle(void);
   virtual stw_types::sint32 SetNodeIdentifiers(const C_OSCProtocolDriverOsyNode & orc_ClientIdentifier,
                                                const C_OSCProtocolDriverOsyNode & orc_ServerIdentifier);
   stw_types::sint32 SetNodeIdentifiersForBroadcasts(const C_OSCProtocolDriverOsyNode & orc_ClientIdentifier);

   stw_types::sint32 SetDispatcher(stw_can::C_CAN_Dispatcher * const opc_Dispatcher);

   //Tp-specific broadcast services:
   stw_types::sint32 BroadcastReadSerialNumber(
      std::vector<C_BroadcastReadEcuSerialNumberResults> & orc_Responses) const;
   stw_types::sint32 BroadcastRequestProgramming(std::vector<C_BroadcastRequestProgrammingResults> & orc_Results) const;
   stw_types::sint32 BroadcastSetNodeIdBySerialNumber(const stw_types::uint8 (&orau8_SerialNumber)[6],
                                                      const C_OSCProtocolDriverOsyNode & orc_NewNodeId,
                                                      stw_types::uint8 * const opu8_NrCode = NULL) const;
   stw_types::sint32 BroadcastEcuReset(const stw_types::uint8 ou8_ResetType) const;
   stw_types::sint32 BroadcastSendEnterPreProgrammingSession(void) const;
   stw_types::sint32 BroadcastSendEnterDefaultSession(void) const;

   void ClearDispatcherQueue(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
