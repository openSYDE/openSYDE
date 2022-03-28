//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol CAN transport-protocol

   For details cf. documentation in .h file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cstring>
#include <iostream>
#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLTime.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCProtocolDriverOsyTpCan.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_can;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize class fields with defaults
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCProtocolDriverOsyTpCan::C_ServiceState::C_ServiceState(void) :
   u16_TransmissionIndex(0U),
   u8_SequenceNumber(0U),
   e_Status(eIDLE),
   u32_StartTimeMs(0U),
   u32_SendCfTimeout(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Set up class

   \param[in]  ou16_MaxServiceQueueSize   maximum number of entries in Tx/Rx queue
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCProtocolDriverOsyTpCan::C_OSCProtocolDriverOsyTpCan(const uint16 ou16_MaxServiceQueueSize) :
   C_OSCProtocolDriverOsyTpBase(ou16_MaxServiceQueueSize),
   mpc_CanDispatcher(NULL),
   mu16_DispatcherClientHandle(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   clean up ...
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCProtocolDriverOsyTpCan::~C_OSCProtocolDriverOsyTpCan(void)
{
   //remove us from dispatcher queue:
   if (mpc_CanDispatcher != NULL)
   {
      //ignore return value; nothing we can do about it
      (void)mpc_CanDispatcher->RemoveClient(mu16_DispatcherClientHandle);
      mpc_CanDispatcher = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming single frame

   Decode and place into Rx service queue.
   We already know
   * the CAN message has a DLC of >= 1
   * the CAN message is addressed to us
   * the type of frame

   Invalid frames will be ignored.

   \param[in]  orc_CanMessage   incoming CAN message

   \return
   C_NO_ERR     no problems; service added to Rx queue
   C_CONFIG     frame invalid (DLC does not match length in PCI byte)
   C_OVERFLOW   could not add incoming service to Rx queue
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_HandleIncomingSingleFrame(const T_STWCAN_Msg_RX & orc_CanMessage)
{
   C_OSCProtocolDriverOsyService c_Service;
   sint32 s32_Return = C_NO_ERR;
   const uint8 u8_Size = orc_CanMessage.au8_Data[0] & 0x0FU;

   c_Service.c_Data.resize(u8_Size);
   if (orc_CanMessage.u8_DLC >= (c_Service.c_Data.size() + 1))
   {
      //extract data:
      if (c_Service.c_Data.size() > 0)
      {
         (void)std::memcpy(&c_Service.c_Data[0], &orc_CanMessage.au8_Data[1], c_Service.c_Data.size());
      }
      //add to queue:
      s32_Return = m_AddToRxQueue(c_Service);
      if (s32_Return != C_NO_ERR)
      {
         m_LogWarningWithHeader("Rx Queue overflow. Incoming single frame dumped.", TGL_UTIL_FUNC_ID);
         s32_Return = C_OVERFLOW;
      }
   }
   else
   {
      //malformed service -> ignore
      m_LogWarningWithHeader("Single frame with invalid DLC received. Ignoring.", TGL_UTIL_FUNC_ID);
      s32_Return = C_CONFIG;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming single frame

   Decode and place into Rx service queue.
   We already know
   * the CAN message has a DLC of >= 1
   * the CAN message is addressed to us
   * the type of frame

   Invalid frames will be ignored.

   \param[in]  orc_CanMessage   incoming CAN message

   \return
   C_NO_ERR     no problems; service added to Rx queue
   C_CONFIG     frame invalid (DLC does not match length in PCI byte)
   C_OVERFLOW   could not add incoming service to Rx queue
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_HandleIncomingOsySpecificSingleFrame(const T_STWCAN_Msg_RX & orc_CanMessage)
{
   C_OSCProtocolDriverOsyService c_Service;
   sint32 s32_Return = C_NO_ERR;
   const uint8 u8_Size = orc_CanMessage.u8_DLC;

   // message without request SID 0xFA
   if ((orc_CanMessage.au8_Data[0] & 0x0FU) == mhu8_OSY_OSF_TYPE_EVENT_DRIVEN_DP_SINGLE_FRAME)
   {
      c_Service.c_Data.resize(u8_Size);
      //remap to service in the format expected by upper layer ...
      c_Service.c_Data[0] = 0xFAU;
      (void)std::memcpy(&c_Service.c_Data[1], &orc_CanMessage.au8_Data[1], static_cast<uintn>(u8_Size) - 1);

      //add to queue:
      s32_Return = m_AddToRxQueue(c_Service);
      if (s32_Return != C_NO_ERR)
      {
         m_LogWarningWithHeader("Rx Queue overflow. Incoming single frame dumped.", TGL_UTIL_FUNC_ID);
         s32_Return = C_OVERFLOW;
      }
   }
   else
   {
      m_LogWarningWithHeader("Unexpected openSYDE single frame message type received. Ignoring.", TGL_UTIL_FUNC_ID);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming first frame

   Set up new Rx state machine and send out flow control.
   Invalid frames will be ignored.
   If a previous Rx transfer was in progress it will be overwritten.
   This is unspecified behavior: ongoing segmented Rx transfers must not be interrupted through other segmented
    Rx transfers as it is not possible to match consecutive frames to the transfer.

   We already know
   * the CAN message has a DLC of >= 1
   * the CAN message is addressed to us
   * the type of frame

   \param[in]  orc_CanMessage   incoming CAN message

   \return
   C_NO_ERR     no problems
   C_CONFIG     frame invalid (DLC is not 8)
   C_COM        could not send out flow control
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_HandleIncomingFirstFrame(const T_STWCAN_Msg_RX & orc_CanMessage)
{
   sint32 s32_Return = C_NO_ERR;

   if (mc_RxService.e_Status != C_ServiceState::eIDLE)
   {
      m_LogWarningWithHeader("Unexpected first frame received. Incomplete transfer dumped.", TGL_UTIL_FUNC_ID);
      mc_RxService.e_Status = C_ServiceState::eIDLE;
   }

   if (orc_CanMessage.u8_DLC == 8U)
   {
      T_STWCAN_Msg_TX t_TxMsg;
      //get total size:
      const uint8 u8_Size = orc_CanMessage.au8_Data[0] & 0x0FU;
      const uint16 u16_ServiceSize = static_cast<uint16>(static_cast<uint16>(u8_Size) << 8U) +
                                     orc_CanMessage.au8_Data[1];
      mc_RxService.c_ServiceData.c_Data.resize(u16_ServiceSize);
      (void)std::memcpy(&mc_RxService.c_ServiceData.c_Data[0], &orc_CanMessage.au8_Data[2], 6U);
      mc_RxService.u16_TransmissionIndex = 6U;
      mc_RxService.u8_SequenceNumber = 1U;

      //send flow control:
      t_TxMsg.u32_ID = m_GetTxIdentifier();
      t_TxMsg.u8_XTD = 1U;
      t_TxMsg.u8_RTR = 0U;
      t_TxMsg.u8_DLC = 3U;
      t_TxMsg.au8_Data[0] = mhu8_ISO15765_N_PCI_FC;
      t_TxMsg.au8_Data[1] = 0U; //no block limits (BS)
      t_TxMsg.au8_Data[2] = 0U; //no separation time (STmin)

      s32_Return = mpc_CanDispatcher->CAN_Send_Msg(t_TxMsg); //lint !e613  //caller is responsible for valid dispatcher
      if (s32_Return != C_NO_ERR)
      {
         m_LogWarningWithHeader("Could not send flow control CAN message.", TGL_UTIL_FUNC_ID);
         s32_Return = C_COM;
      }
      else
      {
         mc_RxService.e_Status = C_ServiceState::eWAITING_FOR_CONSECUTIVE_FRAME;
      }
   }
   else
   {
      //malformed service -> ignore
      m_LogWarningWithHeader("First frame with invalid DLC received. Ignoring.", TGL_UTIL_FUNC_ID);
      s32_Return = C_CONFIG;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send next consecutive frames

   Try to send as many CFs as left in the Tx state machine.
   Advance the Tx state machine accordingly
   If the dispatcher reports a problem leave the state machine as it is.

   Only to be called when the state machine is in state eMORE_CONSECUTIVE_FRAMES_TO_SEND

   \return
   C_NO_ERR     no problems: all pending CFs sent; Tx state machine set to eIDLE again
   C_COM        could not send out consecutive frame (one/some might have been sent, however)
                Tx state machine still at eMORE_CONSECUTIVE_FRAMES_TO_SEND
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_SendNextConsecutiveFrames(void)
{
   uint8 u8_NumBytesToSend;
   sint32 s32_Return = C_NO_ERR;

   //continue where we left:
   for (; mc_TxService.u16_TransmissionIndex < mc_TxService.c_ServiceData.c_Data.size();
        mc_TxService.u16_TransmissionIndex += 7)
   {
      T_STWCAN_Msg_TX t_TxMsg;
      u8_NumBytesToSend = static_cast<uint8>
                          (((mc_TxService.c_ServiceData.c_Data.size() - mc_TxService.u16_TransmissionIndex) > 7U) ? 7U :
                           (mc_TxService.c_ServiceData.c_Data.size() - mc_TxService.u16_TransmissionIndex));
      t_TxMsg.u32_ID = m_GetTxIdentifier();
      t_TxMsg.u8_XTD = 1U;
      t_TxMsg.u8_RTR = 0U;
      t_TxMsg.u8_DLC = 1U + u8_NumBytesToSend;
      t_TxMsg.au8_Data[0] = mhu8_ISO15765_N_PCI_CF + mc_TxService.u8_SequenceNumber;
      //lint -e{670} //std::vector reference returned by [] is guaranteed to have linear data in memory
      (void)std::memcpy(&t_TxMsg.au8_Data[1],
                        &mc_TxService.c_ServiceData.c_Data[mc_TxService.u16_TransmissionIndex],
                        u8_NumBytesToSend);

      //send message:
      //lint -e{613}  //caller is responsible for valid dispatcher
      s32_Return = mpc_CanDispatcher->CAN_Send_Msg(t_TxMsg);
      if (s32_Return != C_NO_ERR)
      {
         //most likely Tx buffer is full; but we cannot be 100% sure, so write a log entry
         m_LogWarningWithHeader("Could not send consecutive frame CAN message (Tx buffer full ?).", TGL_UTIL_FUNC_ID);
         s32_Return = C_COM; //probably the Tx queue is full, we'll retry later
         break;
      }

      //set sequence number for next block:
      mc_TxService.u8_SequenceNumber++;
      if (mc_TxService.u8_SequenceNumber == 16U)
      {
         mc_TxService.u8_SequenceNumber = 0U;
      }
   }
   //finished with this transfer ?
   if (s32_Return == C_NO_ERR)
   {
      mc_TxService.e_Status = C_ServiceState::eIDLE;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming flow control

   Continue sending ongoing Tx transfer consecutive frames when waiting for flow control.
   Invalid and unexpected frames will be ignored.

   We already know
   * the CAN message has a DLC of >= 1
   * the CAN message is addressed to us
   * the type of frame

   \param[in]  orc_CanMessage   incoming CAN message

   \return
   C_NO_ERR     no problems
   C_NOACT      unexpected flow control
   C_CONFIG     frame invalid (DLC is not 3)
   C_OVERFLOW   invalid STmin or BS (only 0 supported for each)
   C_COM        could not send out following consecutive frames
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_HandleIncomingFlowControl(const T_STWCAN_Msg_RX & orc_CanMessage)
{
   sint32 s32_Return = C_NO_ERR;

   //are we in a segmented Tx transfer ?
   if (mc_TxService.e_Status == C_ServiceState::eWAITING_FOR_FLOW_CONTROL)
   {
      //only accept STmin=0 and BS=0
      if (orc_CanMessage.u8_DLC == 3U)
      {
         if ((orc_CanMessage.au8_Data[1] == 0U) && (orc_CanMessage.au8_Data[2] == 0U))
         {
            //set total timeout value for sending all CFs (kicks in if the Tx buffer of the CAN dispatcher is full)
            //N_As is ISO 15765-2 is per CAN frame (and quite high at 1000ms ...)
            //For the total transfer we use the total number of CAN frames, assume the lowest supported bitrate
            // in openSYDE (100 kbit/s) and an alien busload of 50%
            //So we'll have around 2 ms/message
            //But we'll add a lower limit of 100ms to compensate for client side timing constraints
            mc_TxService.u32_SendCfTimeout = static_cast<uint32>((mc_TxService.c_ServiceData.c_Data.size() / 7U) * 2U);
            if (mc_TxService.u32_SendCfTimeout < 100U)
            {
               mc_TxService.u32_SendCfTimeout = 100U;
            }
            //offset with current system time:
            mc_TxService.u32_SendCfTimeout += TGL_GetTickCount();
            mc_TxService.e_Status = C_ServiceState::eMORE_CONSECUTIVE_FRAMES_TO_SEND;

            s32_Return = m_SendNextConsecutiveFrames();
         }
         else
         {
            m_LogWarningWithHeader("Flow control with unsupported STmin or BS received. Ignoring.", TGL_UTIL_FUNC_ID);
            s32_Return = C_OVERFLOW;
         }
      }
      else
      {
         m_LogWarningWithHeader("Flow control with incorrect DLC received. Ignoring.", TGL_UTIL_FUNC_ID);
         s32_Return = C_CONFIG;
      }
   }
   else
   {
      //this was unexpected -> ignore
      m_LogWarningWithHeader("Unexpected flow control received. Ignoring.", TGL_UTIL_FUNC_ID);
      s32_Return = C_NOACT;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming consecutive frame

   Add received data to ongoing Rx transfer.
   After full service data has been received add full service to Rx service queue
   and the Rx state machine will be reset.

   Unexpected frames will be ignored.
   Invalid frames will reset the Rx state machine.

   We already know
   * the CAN message has a DLC of >= 1
   * the CAN message is addressed to us
   * the type of frame

   \param[in]  orc_CanMessage   incoming CAN message

   \return
   C_NO_ERR     no problems
   C_NOACT      unexpected consecutive frame
   C_CONFIG     frame invalid (DLC is too little)
   C_RANGE      invalid sequence number
   C_OVERFLOW   could not add full service to Rx queue
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_HandleIncomingConsecutiveFrame(const T_STWCAN_Msg_RX & orc_CanMessage)
{
   sint32 s32_Return = C_NO_ERR;

   if (mc_RxService.e_Status == C_ServiceState::eWAITING_FOR_CONSECUTIVE_FRAME)
   {
      if (mc_RxService.u8_SequenceNumber == (orc_CanMessage.au8_Data[0] & 0x0FU))
      {
         //correct sequence; copy data.
         const uint8 u8_NumBytes = (orc_CanMessage.u8_DLC - 1);
         if (u8_NumBytes >= (mc_RxService.c_ServiceData.c_Data.size() - mc_RxService.u16_TransmissionIndex))
         {
            //enough data received to fill our buffer:
            (void)std::memcpy(&mc_RxService.c_ServiceData.c_Data[mc_RxService.u16_TransmissionIndex],
                              &orc_CanMessage.au8_Data[1],
                              mc_RxService.c_ServiceData.c_Data.size() - mc_RxService.u16_TransmissionIndex);
            //add to Rx queue:
            s32_Return = m_AddToRxQueue(mc_RxService.c_ServiceData);
            if (s32_Return != C_NO_ERR)
            {
               m_LogWarningWithHeader("Rx Queue overflow. Incoming consecutive frame dumped.", TGL_UTIL_FUNC_ID);
               s32_Return = C_OVERFLOW;
            }
            mc_RxService.e_Status = C_ServiceState::eIDLE;
         }
         else if (orc_CanMessage.u8_DLC == 8U)
         {
            (void)std::memcpy(&mc_RxService.c_ServiceData.c_Data[mc_RxService.u16_TransmissionIndex],
                              &orc_CanMessage.au8_Data[1], 7U);
            mc_RxService.u16_TransmissionIndex += 7U;
            //set next expected sequence number:
            mc_RxService.u8_SequenceNumber++;
            if (mc_RxService.u8_SequenceNumber == 16U)
            {
               mc_RxService.u8_SequenceNumber = 0U;
            }
            //no need to check whether we need to send a flow control: we always use STmin=0 and BS=0
         }
         else
         {
            //not all data available but DLC < 8; this is messed up: abort
            mc_RxService.e_Status = C_ServiceState::eIDLE;
            m_LogWarningWithHeader("Consecutive frame with incorrect DLC received.", TGL_UTIL_FUNC_ID);
            s32_Return = C_CONFIG;
         }
      }
      else
      {
         C_SCLString c_Tmp;
         //incorrect sequence: abort
         mc_RxService.e_Status = C_ServiceState::eIDLE;
         c_Tmp.PrintFormatted("Consecutive frame with incorrect sequence number received. Expected: %i, Received: %i",
                              mc_RxService.u8_SequenceNumber, orc_CanMessage.au8_Data[0] & 0x0FU);
         m_LogWarningWithHeader(c_Tmp.c_str(), TGL_UTIL_FUNC_ID);
         s32_Return = C_RANGE;
      }
   }
   else
   {
      //this was unexpected -> ignore
      m_LogWarningWithHeader("Unexpected consecutive frame received. Ignoring.", TGL_UTIL_FUNC_ID);
      s32_Return = C_NOACT;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming special openSYDE multi frame

   Set up new Rx state machine and send out flow control.
   Invalid frames will be ignored.
   If a previous Rx transfer was in progress it will be overwritten.
   This is unspecified behavior: ongoing segmented Rx transfers must not be interrupted through other segmented
    Rx transfers as it is not possible to match consecutive frames to the transfer.

   We already know
   * the CAN message has a DLC of >= 1
   * the CAN message is addressed to us
   * the type of frame

   \param[in]  orc_CanMessage   incoming CAN message

   \return
   C_NO_ERR     no problems
   C_CONFIG     frame invalid (DLC is not 8)
   C_OVERFLOW   Rx Queue overflow. Incoming consecutive frame dumped
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_HandleIncomingOsySpecificMultiFrame(const T_STWCAN_Msg_RX & orc_CanMessage)
{
   sint32 s32_Return = C_NO_ERR;

   if (mc_RxService.e_Status != C_ServiceState::eIDLE)
   {
      m_LogWarningWithHeader("OMF: Unexpected first frame received. Incomplete transfer dumped.", TGL_UTIL_FUNC_ID);
      mc_RxService.e_Status = C_ServiceState::eIDLE;
   }

   // first frame
   if ((orc_CanMessage.au8_Data[0] & 0x0FU) == 0U)
   {
      if (orc_CanMessage.u8_DLC == 8U)
      {
         //get total size:
         const uint16 u16_ServiceSize = orc_CanMessage.au8_Data[1];
         mc_RxService.c_ServiceData.c_Data.resize(u16_ServiceSize);
         (void)memcpy(&mc_RxService.c_ServiceData.c_Data[0], &orc_CanMessage.au8_Data[2], 6U);
         mc_RxService.u16_TransmissionIndex = 6U;
         mc_RxService.u8_SequenceNumber = 1U;
      }
      else
      {
         //malformed service -> ignore
         m_LogWarningWithHeader("OMF: First frame with invalid DLC received. Ignoring.", TGL_UTIL_FUNC_ID);
         s32_Return = C_CONFIG;
      }
   }
   // consecutive frame
   else
   {
      if (mc_RxService.u8_SequenceNumber == (orc_CanMessage.au8_Data[0] & 0x0FU))
      {
         //correct sequence; copy data.
         const uint8 u8_NumBytes = (orc_CanMessage.u8_DLC - 1);
         if (u8_NumBytes >= (mc_RxService.c_ServiceData.c_Data.size() - mc_RxService.u16_TransmissionIndex))
         {
            //enough data received to fill our buffer:
            (void)memcpy(&mc_RxService.c_ServiceData.c_Data[mc_RxService.u16_TransmissionIndex],
                         &orc_CanMessage.au8_Data[1],
                         mc_RxService.c_ServiceData.c_Data.size() - mc_RxService.u16_TransmissionIndex);
            //add to Rx queue:
            s32_Return = m_AddToRxQueue(mc_RxService.c_ServiceData);
            if (s32_Return != C_NO_ERR)
            {
               m_LogWarningWithHeader("OMF: Rx Queue overflow. Incoming consecutive frame dumped.", TGL_UTIL_FUNC_ID);
               s32_Return = C_OVERFLOW;
            }
            mc_RxService.e_Status = C_ServiceState::eIDLE;
            mc_RxService.u8_SequenceNumber = 0U;
         }
         else if (orc_CanMessage.u8_DLC == 8U)
         {
            (void)memcpy(&mc_RxService.c_ServiceData.c_Data[mc_RxService.u16_TransmissionIndex],
                         &orc_CanMessage.au8_Data[1], 7U);
            mc_RxService.u16_TransmissionIndex += 7U;
            //set next expected sequence number:
            mc_RxService.u8_SequenceNumber++;
            if (mc_RxService.u8_SequenceNumber == 16U)
            {
               mc_RxService.u8_SequenceNumber = 1U;
            }
         }
         else
         {
            //not all data available but DLC < 8; this is messed up: abort
            mc_RxService.e_Status = C_ServiceState::eIDLE;
            mc_RxService.u8_SequenceNumber = 0U;
            m_LogWarningWithHeader("OMF: Consecutive frame with incorrect DLC received.", TGL_UTIL_FUNC_ID);
            s32_Return = C_CONFIG;
         }
      }
      else
      {
         //incorrect sequence: abort
         mc_RxService.e_Status = C_ServiceState::eIDLE;
         mc_RxService.u8_SequenceNumber = 0U;
         m_LogWarningWithHeader("OMF: Consecutive frame with incorrect sequence number received.", TGL_UTIL_FUNC_ID);
         s32_Return = C_RANGE;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send broadcast service EnterDiagnosticSession

   Only sends one request and returns immediately. Results are not reported.
   Purpose: hit into the time window when the devices are started to keep the devices in flashloader.

   The suppressPosRspMsgIndicationBit will be set to avoid flooding the bus with response messages.

   \return
   C_NO_ERR   no problems
   C_COM      could not send request
   C_CONFIG   no dispatcher installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_BroadcastSendDiagnosticSessionControl(const uint8 ou8_Session) const
{
   sint32 s32_Return;

   if (mpc_CanDispatcher == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //set up request
      C_OSCProtocolDriverOsyService c_Service;
      T_STWCAN_Msg_TX c_Msg;
      c_Service.c_Data.resize(2);
      c_Service.c_Data[0] = mhu8_OSY_BC_SI_DIAGNOSTIC_SESSION_CONTROL;
      // We do not want any answer for this broadcast. Use the suppressPosRspMsgIndicationBit
      c_Service.c_Data[1] = ou8_Session | 0x80U;
      m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);

      s32_Return = mpc_CanDispatcher->CAN_Send_Msg(c_Msg);
      if (s32_Return != C_NO_ERR)
      {
         m_LogWarningWithHeader("Could not send single frame broadcast CAN message.", TGL_UTIL_FUNC_ID);
         s32_Return = C_COM;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: compose single frame for sending

   Compose single frame from specified parameters.
   Function does not check for validity of parameters (e.g. valid service size).
   Caller is responsible.

   \param[in]  orc_Service       service to put into frame
   \param[in]  ou32_Identifier   CAN identifier to use
   \param[out] orc_CanMessage    resulting CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsyTpCan::m_ComposeSingleFrame(const C_OSCProtocolDriverOsyService & orc_Service,
                                                       const uint32 ou32_Identifier,
                                                       T_STWCAN_Msg_TX & orc_CanMessage) const
{
   orc_CanMessage.u32_ID = ou32_Identifier;
   orc_CanMessage.u8_XTD = 1U;
   orc_CanMessage.u8_RTR = 0U;
   orc_CanMessage.u8_DLC = static_cast<uint8>(orc_Service.c_Data.size() + 1U);

   //set PCI:
   orc_CanMessage.au8_Data[0] = static_cast<uint8>(mhu8_ISO15765_N_PCI_SF + (orc_Service.c_Data.size()));
   if (orc_Service.c_Data.size() > 0)
   {
      (void)std::memcpy(&orc_CanMessage.au8_Data[1], &orc_Service.c_Data[0], orc_Service.c_Data.size());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handling the response to the services BroadcastSetNodeIdBySerialNumber/-Extended

   Incoming responses to other services will be dumped: we are strictly handshaking here ...
   (why would someone send other broadcast requests in parallel ?)

   \param[in]    ou8_RoutineIdMsb     MSB part of the last sent routine identifier for matching the correct response
   \param[in]    ou8_RoutineIdLsb     LSB part of the last sent routine identifier for matching the correct response
   \param[out]   opu8_NrCode          if not NULL: negative response code (if C_WARN is returned)

   \return
   C_NO_ERR    no problems; one positive response received
   C_WARN      negative response received and no positive response received
   C_TIMEOUT   no response within timeout (was SetNodeIdentifiersForBroadcasts() called ?)
   C_OVERFLOW  multiple positive responses received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_HandleBroadcastSetNodeIdBySerialNumberResponse(const uint8 ou8_RoutineIdMsb,
                                                                                     const uint8 ou8_RoutineIdLsb,
                                                                                     uint8 * const opu8_NrCode) const
{
   sint32 s32_Return;
   //check for responses
   const uint32 u32_StartTime = TGL_GetTickCount();
   T_STWCAN_Msg_RX c_Response;
   sint32 s32_ReturnLocal;
   bool q_PositiveResponseReceived = false;
   bool q_MultiplePositiveResponsesReceived = false;
   bool q_NegativeResponseReceived = false;

   // No further abort condition. Wait always the entire timeout time to get all positive and negative responses
   while ((TGL_GetTickCount() - mu32_BroadcastTimeoutMs) < u32_StartTime)
   {
      //trigger dispatcher
      //ignore return value: we cannot be sure some other client did not check before us
      (void)mpc_CanDispatcher->DispatchIncoming();

      s32_ReturnLocal = mpc_CanDispatcher->ReadFromQueue(mu16_DispatcherClientHandle, c_Response);
      if (s32_ReturnLocal == C_NO_ERR)
      {
         //format OK ?
         if ((c_Response.u8_DLC == 5U) && (c_Response.au8_Data[0] == (mhu8_ISO15765_N_PCI_SF + 4U)) &&
             (c_Response.au8_Data[1] == (mhu8_OSY_BC_SI_ROUTINE_CONTROL | 0x40U)) &&
             (c_Response.au8_Data[2] == mhu8_OSY_BC_RC_SUB_FUNCTION_START_ROUTINE) &&
             (c_Response.au8_Data[3] == ou8_RoutineIdMsb) &&
             (c_Response.au8_Data[4] == ou8_RoutineIdLsb))
         {
            //looks legit ...
            if (q_PositiveResponseReceived == false)
            {
               q_PositiveResponseReceived = true;
            }
            else
            {
               //multiple responses -> report
               q_MultiplePositiveResponsesReceived = true;
            }
         }
         else if ((c_Response.u8_DLC == 4U) && (c_Response.au8_Data[0] == (mhu8_ISO15765_N_PCI_SF + 3U)) &&
                  (c_Response.au8_Data[1] == mhu8_BC_OSY_NR_SI) &&
                  (c_Response.au8_Data[2] == mhu8_OSY_BC_SI_ROUTINE_CONTROL))
         {
            //negative response detected
            q_NegativeResponseReceived = true;
            //Node id was not set, node has security activated.
            if (c_Response.au8_Data[3] == 0x22U)
            {
               m_LogWarningWithHeader("Broadcast: Negative response received. Node has security activated.",
                                      TGL_UTIL_FUNC_ID);
            }
            else
            {
               m_LogWarningWithHeader("Broadcast: Negative response received. Probably a node with an other SN.",
                                      TGL_UTIL_FUNC_ID);
            }
            if (opu8_NrCode != NULL)
            {
               (*opu8_NrCode) = c_Response.au8_Data[3];
            }
         }
         else
         {
            m_LogWarningWithHeader("Broadcast: unexpected response received. Ignoring.", TGL_UTIL_FUNC_ID);
         }
      }
   }

   if (q_MultiplePositiveResponsesReceived == true)
   {
      // Multiple positive responses received. At least two nodes with same SN. Error case.
      s32_Return = C_OVERFLOW;
   }
   else if (q_PositiveResponseReceived == true)
   {
      // One positive response received. All negative responses can be ignored
      s32_Return = C_NO_ERR;
   }
   else if (q_NegativeResponseReceived == true)
   {
      // No positive, only negative responses. Error case.
      s32_Return = C_WARN;
   }
   else
   {
      // No relevant response received
      s32_Return = C_TIMEOUT;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform cyclic communication tasks for CAN-TP

   Perform cyclic communication tasks for CAN-TP.
   - sending and evaluation of flow controls
   - updating state machines of ongoing services
   - place completed incoming services in Rx Queue
   - detect timeouts for ongoing Tx transfers

   Configuration problems will be reported via the function return value.
   Ongoing communication problems will be written to the class's log text.

   \return
   C_NO_ERR   cycle finished
   C_CONFIG   no dispatcher installed
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::Cycle(void)
{
   sint32 s32_ReturnFunc = C_NO_ERR;
   sint32 s32_Return;

   if (mpc_CanDispatcher == NULL)
   {
      s32_ReturnFunc = C_CONFIG;
   }
   else
   {
      if (mc_TxService.e_Status == C_ServiceState::eIDLE)
      {
         //do we have more to send ?
         s32_Return = C_NO_ERR;
         while (s32_Return == C_NO_ERR)
         {
            s32_Return = this->m_GetFromTxQueue(mc_TxService.c_ServiceData);
            if (s32_Return == C_NO_ERR)
            {
               if (mc_TxService.c_ServiceData.c_Data.size() <= 7)
               {
                  //simple single frame:
                  T_STWCAN_Msg_TX t_Msg;
                  m_ComposeSingleFrame(mc_TxService.c_ServiceData, m_GetTxIdentifier(), t_Msg);

                  s32_Return = mpc_CanDispatcher->CAN_Send_Msg(t_Msg);
                  if (s32_Return != C_NO_ERR)
                  {
                     m_LogWarningWithHeader("Could not send single frame CAN message.", TGL_UTIL_FUNC_ID);
                     s32_Return = C_COM; //terminate loop
                     s32_ReturnFunc = C_COM;
                  }
               }
               else if (mc_TxService.c_ServiceData.q_CanTransferWithoutFlowControl == true)
               {
                  // openSYDE specific multi frame without flow control
                  T_STWCAN_Msg_TX t_TxMsg;
                  const uintn un_CountBytes = mc_TxService.c_ServiceData.c_Data.size();

                  t_TxMsg.u32_ID = m_GetTxIdentifier();
                  t_TxMsg.u8_XTD = 1U;
                  t_TxMsg.u8_RTR = 0U;
                  t_TxMsg.u8_DLC = 8U;

                  // First frame
                  mc_TxService.u8_SequenceNumber = 0U;
                  // The number of bytes in the first frame. Start with this byte in the following frames
                  mc_TxService.u16_TransmissionIndex = 6U;

                  t_TxMsg.au8_Data[0] = mhu8_ISO15765_N_PCI_OMF;
                  t_TxMsg.au8_Data[1] = static_cast<uint8>(un_CountBytes & 0xFFU);
                  (void)std::memcpy(&t_TxMsg.au8_Data[2], &mc_TxService.c_ServiceData.c_Data[0], 6U);
                  s32_Return = mpc_CanDispatcher->CAN_Send_Msg(t_TxMsg);

                  if (s32_Return != C_NO_ERR)
                  {
                     m_LogWarningWithHeader("Could not send first frame CAN message.", TGL_UTIL_FUNC_ID);
                  }
                  else
                  {
                     // Following frames
                     do
                     {
                        uint32 u32_BytesForNextMessage =
                           static_cast<uint32>(un_CountBytes - mc_TxService.u16_TransmissionIndex);

                        // Maximum are 7 bytes payload for each message
                        if (u32_BytesForNextMessage > 7U)
                        {
                           u32_BytesForNextMessage = 7U;
                        }

                        mc_TxService.u8_SequenceNumber++;
                        mc_TxService.u8_SequenceNumber %= 16U;

                        t_TxMsg.au8_Data[0] = mhu8_ISO15765_N_PCI_OMF + mc_TxService.u8_SequenceNumber;
                        //std::vector reference returned by [] is guaranteed to have linear data in memory:
                        //lint -e{670}
                        (void)std::memcpy(&t_TxMsg.au8_Data[1],
                                          &mc_TxService.c_ServiceData.c_Data[mc_TxService.u16_TransmissionIndex],
                                          u32_BytesForNextMessage);

                        t_TxMsg.u8_DLC = static_cast<uint8>(u32_BytesForNextMessage + 1U);
                        s32_Return = mpc_CanDispatcher->CAN_Send_Msg(t_TxMsg);
                        if (s32_Return != C_NO_ERR)
                        {
                           //We do not handle Tx buffer issues with the OMF-CFs (like we do with regular CFs)
                           //The OMF mechanism is only used for smaller services with low numbers of messages without
                           // handshakes (typical maximum: STW Flashloader flashing -> 37 * 3 messages)
                           //Assumption: this number of Tx buffers is available on all targets
                           m_LogWarningWithHeader("Could not send consecutive CAN message.", TGL_UTIL_FUNC_ID);
                           break;
                        }

                        mc_TxService.u16_TransmissionIndex += static_cast<uint16>(u32_BytesForNextMessage);
                     }
                     while (mc_TxService.u16_TransmissionIndex < un_CountBytes);
                  }
               }
               else
               {
                  //multi-frame; compose first frame:
                  T_STWCAN_Msg_TX t_TxMsg;
                  t_TxMsg.u32_ID = m_GetTxIdentifier();
                  t_TxMsg.u8_XTD = 1U;
                  t_TxMsg.u8_RTR = 0U;
                  t_TxMsg.u8_DLC = 8U;

                  t_TxMsg.au8_Data[0] = static_cast<uint8>(mhu8_ISO15765_N_PCI_FF +
                                                           ((mc_TxService.c_ServiceData.c_Data.size() >> 8U) & 0x0FU));
                  t_TxMsg.au8_Data[1] = static_cast<uint8>(mc_TxService.c_ServiceData.c_Data.size() & 0xFFU);
                  (void)std::memcpy(&t_TxMsg.au8_Data[2], &mc_TxService.c_ServiceData.c_Data[0], 6U);
                  mc_TxService.u16_TransmissionIndex = 6U;
                  mc_TxService.u8_SequenceNumber = 1U;
                  s32_Return = mpc_CanDispatcher->CAN_Send_Msg(t_TxMsg);
                  if (s32_Return != C_NO_ERR)
                  {
                     m_LogWarningWithHeader("Could not send first frame CAN message.", TGL_UTIL_FUNC_ID);
                  }
                  else
                  {
                     mc_TxService.e_Status = C_ServiceState::eWAITING_FOR_FLOW_CONTROL;
                     mc_TxService.u32_StartTimeMs = TGL_GetTickCount();
                  }

                  break; //not yet finished with this transfer
               }
            }
         }
      }
      else if (mc_TxService.e_Status == C_ServiceState::eWAITING_FOR_FLOW_CONTROL)
      {
         //check for Tx timeout:
         if ((TGL_GetTickCount() - mhu16_NBS_TIMEOUTS_MS) > mc_TxService.u32_StartTimeMs)
         {
            //transfer timed out ...
            m_LogWarningWithHeader("N_Bs timeout reached before receiving flow control. Aborting ongoing Tx transfer.",
                                   TGL_UTIL_FUNC_ID);
            mc_TxService.e_Status = C_ServiceState::eIDLE;
         }
      }
      else if (mc_TxService.e_Status == C_ServiceState::eMORE_CONSECUTIVE_FRAMES_TO_SEND)
      {
         //try to send more frames:
         //result checked indirectly via state machine status
         (void)m_SendNextConsecutiveFrames();
         //still not finished ?
         if (mc_TxService.e_Status == C_ServiceState::eMORE_CONSECUTIVE_FRAMES_TO_SEND)
         {
            if (TGL_GetTickCount() > mc_TxService.u32_SendCfTimeout)
            {
               //transfer timed out ...
               m_LogWarningWithHeader("Could not send all CFs within timeout. Aborting ongoing Tx transfer.",
                                      TGL_UTIL_FUNC_ID);
               mc_TxService.e_Status = C_ServiceState::eIDLE;
            }
         }
      }
      else
      {
         //no state we care about ...
      }

      //now check Rx messages:
      T_STWCAN_Msg_RX t_Msg;
      //check for incoming messages
      //ignore return value: we cannot be sure some other client did not check before us
      (void)mpc_CanDispatcher->DispatchIncoming();

      //read all incoming messages:
      s32_Return = C_NO_ERR;
      while (s32_Return == C_NO_ERR)
      {
         s32_Return = mpc_CanDispatcher->ReadFromQueue(mu16_DispatcherClientHandle, t_Msg);
         if ((s32_Return == C_NO_ERR) && (t_Msg.u8_DLC > 0U))
         {
            //return values of frame handler functions are ignored
            //- problem details are reported there in the log
            //- we need to continue parsing incoming data even after a hiccup
            switch (t_Msg.au8_Data[0] & 0xF0U)
            {
            case mhu8_ISO15765_N_PCI_SF:
               (void)m_HandleIncomingSingleFrame(t_Msg);
               break;
            case mhu8_ISO15765_N_PCI_FF:
               (void)m_HandleIncomingFirstFrame(t_Msg);
               break;
            case mhu8_ISO15765_N_PCI_CF:
               (void)m_HandleIncomingConsecutiveFrame(t_Msg);
               break;
            case mhu8_ISO15765_N_PCI_FC: // flow control
               (void)m_HandleIncomingFlowControl(t_Msg);
               break;
            case mhu8_ISO15765_N_PCI_OSF: // openSYDE specific
               (void)m_HandleIncomingOsySpecificSingleFrame(t_Msg);
               break;
            case mhu8_ISO15765_N_PCI_OMF:
               (void)m_HandleIncomingOsySpecificMultiFrame(t_Msg);
               break;
            default:
               m_LogWarningWithHeader("Unexpected frame type received. Ignoring.", TGL_UTIL_FUNC_ID);
               break;
            }
         }
      }
   }

   return s32_ReturnFunc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Make server and client identifiers known to TP.

   * remember IDs
   * clear Rx queue
   * recalculate and change receive filter of C_CAN_Dispatcher accordingly.

   \param[in]  orc_ClientIdentifier   new client (i.e.: our own) identifier
   \param[in]  orc_ServerIdentifier   new server identifier

   \return
   C_NO_ERR   no problems
   C_RANGE    client and/or server identifier out of range
   C_NOACT    could not reconfigure Rx filters
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::SetNodeIdentifiers(const C_OSCProtocolDriverOsyNode & orc_ClientIdentifier,
                                                       const C_OSCProtocolDriverOsyNode & orc_ServerIdentifier)
{
   sint32 s32_Return;

   s32_Return = C_OSCProtocolDriverOsyTpBase::SetNodeIdentifiers(orc_ClientIdentifier, orc_ServerIdentifier);
   if ((s32_Return == C_NO_ERR) && (mpc_CanDispatcher != NULL))
   {
      //Clear Rx queue; we are no longer interested in that old stuff:
      mpc_CanDispatcher->ClearQueue(mu16_DispatcherClientHandle);

      //reconfigure Rx filter:
      s32_Return = this->m_SetRxFilter(false);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
   }
   else
   {
      //finished ...
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Configure this class for broadcasting

   * remember client IDs
   * clear Rx queue
   * recalculate and change receive filter of C_CAN_Dispatcher to be able to receive responses from all nodes on our
      bus

   \param[in]  orc_ClientIdentifier   new client (i.e.: our own) identifier

   \return
   C_NO_ERR   no problems
   C_RANGE    client and/or server identifier out of range
   C_NOACT    could not reconfigure Rx filters
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::SetNodeIdentifiersForBroadcasts(
   const C_OSCProtocolDriverOsyNode & orc_ClientIdentifier)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyNode c_ServerId;

   c_ServerId.u8_BusIdentifier = orc_ClientIdentifier.u8_BusIdentifier;
   c_ServerId.u8_NodeIdentifier = C_OSCProtocolDriverOsyNode::mhu8_NODE_ID_BROADCASTS;

   s32_Return = C_OSCProtocolDriverOsyTpBase::SetNodeIdentifiers(orc_ClientIdentifier, c_ServerId);
   if ((s32_Return == C_NO_ERR) && (mpc_CanDispatcher != NULL))
   {
      //Clear Rx queue; we are no longer interested in that old stuff:
      mpc_CanDispatcher->ClearQueue(mu16_DispatcherClientHandle);

      //reconfigure Rx filter:
      s32_Return = this->m_SetRxFilter(true);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
   }
   else
   {
      //finished ...
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate Tx ID to send request on.

   Calculate Tx ID to use depending on client and server address.

   \return
   CAN message ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCProtocolDriverOsyTpCan::m_GetTxIdentifier(void) const
{
   uint32 u32_Identifier;

   //is routing required ?
   if (mc_ServerId.u8_BusIdentifier == mc_ClientId.u8_BusIdentifier)
   {
      //same bus; no routing -> we use normal fixed addressing
      u32_Identifier = static_cast<uint32>(0x18DA0000U) +
                       (static_cast<uint32>(mc_ServerId.u8_NodeIdentifier) << 8U) +
                       mc_ClientId.u8_NodeIdentifier;
   }
   else
   {
      //different bus; routing -> we use 15765-3 section 8.3 IDs
      u32_Identifier = static_cast<uint32>(0x1BC00000U) +
                       (static_cast<uint32>(mc_ClientId.u8_BusIdentifier) << 18U) +
                       (static_cast<uint32>(mc_ClientId.u8_NodeIdentifier) << 11U) +
                       (static_cast<uint32>(mc_ServerId.u8_BusIdentifier) << 7U) +
                       mc_ServerId.u8_NodeIdentifier;
   }
   return u32_Identifier;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate Tx ID to send broadcasts on.

   Calculate Tx ID to use for broadcasts.
   Uses the client node-id as sender and the "all nodes" address as targets.
   Broadcasts only work in the local network, so normal fixed functional addressing is used.

   \return
   CAN message ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCProtocolDriverOsyTpCan::m_GetTxBroadcastIdentifier(void) const
{
   return static_cast<uint32>(0x18DB0000U +
                              ((static_cast<uint32>(C_OSCProtocolDriverOsyNode::mhu8_NODE_ID_BROADCASTS)) << 8U) +
                              mc_ClientId.u8_NodeIdentifier);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set CAN dispatcher Rx filter

   Configure CAN dispatcher Rx filter to receive only TP messages that we are interested in.

   For non-broadcasts:
   - from mc_ServerId
   - to mc_ClientId (us)

   For broadcasts:
   - from any node on the local bus
   - to mc_ClientId (us)

   Precondition: installed dispatcher must be valid

   \return
   C_NO_ERR   no problems
   C_RANGE    invalid dispatcher handle
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::m_SetRxFilter(const bool oq_ForBroadcast)
{
   C_CAN_RXFilter c_Filter;

   c_Filter.q_RTR = false; //pfuideifi !
   c_Filter.q_RTRMustMatch = true;
   c_Filter.q_XTD = true; //only 29bit IDs in this protocol
   c_Filter.q_XTDMustMatch = true;

   if (oq_ForBroadcast == false)
   {
      //is routing required ?
      if (mc_ServerId.u8_BusIdentifier == mc_ClientId.u8_BusIdentifier)
      {
         //same bus; no routing -> we use normal fixed addressing (physical)
         c_Filter.u32_Code = static_cast<uint32>(0x18DA0000U) +
                             (static_cast<uint32>(mc_ClientId.u8_NodeIdentifier) << 8U) +
                             mc_ServerId.u8_NodeIdentifier;
         c_Filter.u32_Mask = 0x1FFFFFFFU; //must be exactly for us and from specific sender
      }
      else
      {
         //different bus; routing -> we use 15765-3 section 8.3 IDs
         c_Filter.u32_Code = static_cast<uint32>(0x1BC00000U) +
                             (static_cast<uint32>(mc_ServerId.u8_BusIdentifier) << 18U) +
                             (static_cast<uint32>(mc_ServerId.u8_NodeIdentifier) << 11U) +
                             (static_cast<uint32>(mc_ClientId.u8_BusIdentifier) << 7U) +
                             mc_ClientId.u8_NodeIdentifier;
         c_Filter.u32_Mask = 0x1FFFFFFFU; //must be exactly for us and from specific sender
      }
   }
   else
   {
      //normal fixed addressing physical (responses will not be addresses as broadcasts)
      c_Filter.u32_Code = static_cast<uint32>(0x18DA0000U) +
                          (static_cast<uint32>(mc_ClientId.u8_NodeIdentifier) << 8U);
      c_Filter.u32_Mask = 0x1FFFFF80U; //must be exactly for us; but sender may be anyone
   }
   //lint -e{613}  //caller is responsible for valid dispatcher
   return mpc_CanDispatcher->SetRXFilter(mu16_DispatcherClientHandle, c_Filter);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Make communication dispatcher to use known

   - remember pointer
   - register us as client with Rx filter matching our own address

   \param[in]    opc_Dispatcher  CAN dispatcher to use for communication

   \return
   C_NO_ERR   no problems
   C_CONFIG   could not register with dispatcher
   C_NOACT    could not configure Rx filter
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::SetDispatcher(C_CAN_Dispatcher * const opc_Dispatcher)
{
   sint32 s32_Return = C_NO_ERR;

   //was there a dispatcher installed previously ?
   if (mpc_CanDispatcher != NULL)
   {
      //yes -> unregister; ignore return value (nothing much we can do ...)
      (void)mpc_CanDispatcher->RemoveClient(mu16_DispatcherClientHandle);
   }

   mpc_CanDispatcher = opc_Dispatcher;

   //register with new dispatcher:
   if (mpc_CanDispatcher != NULL)
   {
      s32_Return = mpc_CanDispatcher->RegisterClient(mu16_DispatcherClientHandle);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_CONFIG;
      }
      else
      {
         //set reception filters:
         s32_Return = m_SetRxFilter(
            (mc_ServerId.u8_NodeIdentifier == C_OSCProtocolDriverOsyNode::mhu8_NODE_ID_BROADCASTS) ? true : false);
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_NOACT;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read serial number of all devices on local bus

   Send broadcasts to read device serial number in standard and extended format.
   Report back list of nodes that sent a response (with the sent serial number and node IDs).
   The function will wait for responses for the time configured with SetBroadcastTimeout()
   Only positive responses are reported. Negative responses are ignored.

   Incoming responses to other services will be dumped: we are strictly handshaking here ...
   (why would someone send other broadcast requests in parallel ?)

   \param[out]    orc_Responses           information about all nodes that sent a response
   \param[out]    orc_ExtendedResponses   information about all nodes that sent an extended response

   \return
   C_NO_ERR   no problems; zero or more responses received; data placed in orc_Responses
   C_COM      could not send request
   C_CONFIG   no dispatcher installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::BroadcastReadSerialNumber(
   std::vector<C_BroadcastReadEcuSerialNumberResults> & orc_Responses,
   std::vector<C_BroadcastReadEcuSerialNumberExtendedResults> & orc_ExtendedResponses) const
{
   sint32 s32_Return;

   orc_Responses.clear();
   orc_ExtendedResponses.clear();
   if (mpc_CanDispatcher == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //set up request
      C_OSCProtocolDriverOsyService c_Service;
      T_STWCAN_Msg_TX c_Msg;
      T_STWCAN_Msg_RX c_Response;
      sint32 s32_ReturnLocal;

      c_Service.c_Data.resize(1);
      c_Service.c_Data[0] = mhu8_OSY_BC_SI_READ_SERIAL_NUMBER;
      m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);

      s32_Return = mpc_CanDispatcher->CAN_Send_Msg(c_Msg);
      if (s32_Return != C_NO_ERR)
      {
         m_LogWarningWithHeader("Could not send single frame broadcast CAN message.", TGL_UTIL_FUNC_ID);
         s32_Return = C_COM;
      }
      else
      {
         //check for responses
         const uint32 u32_StartTimeStd = TGL_GetTickCount();

         while ((TGL_GetTickCount() - mu32_BroadcastTimeoutMs) < u32_StartTimeStd)
         {
            //trigger dispatcher
            //ignore return value: we cannot be sure some other client did not check before us
            (void)mpc_CanDispatcher->DispatchIncoming();

            s32_ReturnLocal = mpc_CanDispatcher->ReadFromQueue(mu16_DispatcherClientHandle, c_Response);
            if (s32_ReturnLocal == C_NO_ERR)
            {
               //format OK (frame type; number of bytes; service) ?
               if ((c_Response.u8_DLC == 8U) &&
                   (c_Response.au8_Data[0] == (mhu8_ISO15765_N_PCI_SF + 7U)) &&
                   (c_Response.au8_Data[1] == (mhu8_OSY_BC_SI_READ_SERIAL_NUMBER | 0x40U)))
               {
                  //looks legit; extract payload ...
                  C_BroadcastReadEcuSerialNumberResults c_Result;
                  uint8 au8_SerialNumber[6];
                  (void)std::memcpy(&au8_SerialNumber[0], &c_Response.au8_Data[2], 6U);
                  c_Result.c_SerialNumber.SetPosSerialNumber(au8_SerialNumber);

                  //extract node-id from sender
                  //must be local addressing ...
                  c_Result.c_SenderId.u8_BusIdentifier = mc_ClientId.u8_BusIdentifier; //same as ours ...
                  c_Result.c_SenderId.u8_NodeIdentifier = static_cast<uint8>(c_Response.u32_ID & 0x7FU);
                  orc_Responses.push_back(c_Result);
               }
               else if ((c_Response.u8_DLC == 4U) && (c_Response.au8_Data[0] == (mhu8_ISO15765_N_PCI_SF + 3U)) &&
                        (c_Response.au8_Data[1] == mhu8_BC_OSY_NR_SI) &&
                        (c_Response.au8_Data[2] == mhu8_OSY_BC_SI_READ_SERIAL_NUMBER))
               {
                  //negative response detected
                  m_LogWarningWithHeader("Broadcast: Negative response received. Probably a node with an extended SN.",
                                         TGL_UTIL_FUNC_ID);
               }
               else
               {
                  m_LogWarningWithHeader("Broadcast: unexpected response received. Ignoring.", TGL_UTIL_FUNC_ID);
               }
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         bool q_Continue = false;
         std::map<uint32, C_BroadcastReadEcuSerialNumberExtendedResults> c_UniqueIdToResult;

         // Send the extended variant
         c_Service.c_Data.resize(3);
         c_Service.c_Data[1] = 0U; // Block number. Start with the first block
         c_Service.c_Data[2] = 0U; // Reserved byte
         m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);

         do
         {
            q_Continue = false;

            s32_Return = mpc_CanDispatcher->CAN_Send_Msg(c_Msg);
            if (s32_Return != C_NO_ERR)
            {
               m_LogWarningWithHeader("Could not send single frame broadcast CAN message.", TGL_UTIL_FUNC_ID);
               s32_Return = C_COM;
            }
            else
            {
               //check for responses
               const uint32 u32_StartTimeExt = TGL_GetTickCount();

               while ((TGL_GetTickCount() - mu32_BroadcastTimeoutMs) < u32_StartTimeExt)
               {
                  //trigger dispatcher
                  //ignore return value: we cannot be sure some other client did not check before us
                  (void)mpc_CanDispatcher->DispatchIncoming();

                  s32_ReturnLocal = mpc_CanDispatcher->ReadFromQueue(mu16_DispatcherClientHandle, c_Response);
                  if (s32_ReturnLocal == C_NO_ERR)
                  {
                     //format OK (frame type; number of bytes; service) ?
                     if ((c_Response.u8_DLC == 8U) &&
                         (c_Response.au8_Data[0] == (mhu8_ISO15765_N_PCI_SF + 7U)) &&
                         (c_Response.au8_Data[1] == (mhu8_OSY_BC_SI_READ_SERIAL_NUMBER | 0x40U)))
                     {
                        //looks legit; extract payload ...
                        // The block information
                        const uint8 u8_ReceivedBlockNumber = c_Response.au8_Data[2] & 0x0FU;
                        const uint8 u8_ReceivedSubNodeId = c_Response.au8_Data[2] >> 4U;

                        if (u8_ReceivedBlockNumber == c_Service.c_Data[1])
                        {
                           const uint32 u32_UniqueId = (static_cast<uint32>(c_Response.au8_Data[3]) << 16U) +
                                                       (static_cast<uint32>(c_Response.au8_Data[4]) << 8U) +
                                                       static_cast<uint32>(c_Response.au8_Data[5]);

                           // The serial number payload in the last two bytes will contain following information
                           // separated in the different block
                           // - Security Activated (1 bit of first byte)
                           // - Reserved (Bit 1-7 of first byte; set all bits to zero)
                           // - SerialNumberManufacturerFormat (1 byte)
                           // - SerialNumber Length (1 byte)
                           // - SerialNumber (1 .. 29 bytes)

                           if (u8_ReceivedBlockNumber == 0U)
                           {
                              // First block. Add the initial result instance
                              C_BroadcastReadEcuSerialNumberExtendedResults c_ResultExt;

                              //extract node-id from sender
                              //must be local addressing ...
                              c_ResultExt.c_SenderId.u8_BusIdentifier = mc_ClientId.u8_BusIdentifier; //same as ours ...
                              c_ResultExt.c_SenderId.u8_NodeIdentifier = static_cast<uint8>(c_Response.u32_ID & 0x7FU);
                              c_ResultExt.u8_SubNodeId = u8_ReceivedSubNodeId;
                              c_ResultExt.c_SerialNumber.u8_SerialNumberByteLength = 0U; // Will be filled in the
                                                                                         // next
                                                                                         // block

                              // Serial number payload of first block
                              // First byte: Bit 0 security flag, all other bits are reserved
                              c_ResultExt.q_SecurityActivated = ((c_Response.au8_Data[6] & 0x01U) == 0x01U);

                              // Last byte of payload in first block
                              c_ResultExt.c_SerialNumber.u8_SerialNumberManufacturerFormat = c_Response.au8_Data[7];

                              // It is in any case the extended format
                              c_ResultExt.c_SerialNumber.q_ExtFormatUsed = true;

                              if (c_ResultExt.c_SerialNumber.u8_SerialNumberManufacturerFormat == 0U)
                              {
                                 // Special case: POS serial number with extended
                                 c_ResultExt.c_SerialNumber.q_FsnSerialNumber = false;
                              }
                              else
                              {
                                 c_ResultExt.c_SerialNumber.q_FsnSerialNumber = true;
                              }

                              // Add the initial result to the map with its unique id as key
                              c_UniqueIdToResult[u32_UniqueId] = c_ResultExt;

                              // At least two blocks are always necessary
                              q_Continue = true;
                           }
                           else
                           {
                              std::map<uint32, C_BroadcastReadEcuSerialNumberExtendedResults>::iterator c_ItResult;
                              // Search the matching extended result to complete the result
                              c_ItResult = c_UniqueIdToResult.find(u32_UniqueId);

                              if (c_ItResult != c_UniqueIdToResult.end())
                              {
                                 C_BroadcastReadEcuSerialNumberExtendedResults & rc_CurrentResult = c_ItResult->second;
                                 if (rc_CurrentResult.u8_SubNodeId == u8_ReceivedSubNodeId)
                                 {
                                    if (u8_ReceivedBlockNumber == 1U)
                                    {
                                       // Serial number payload of second block
                                       // First byte Serial number length
                                       rc_CurrentResult.c_SerialNumber.u8_SerialNumberByteLength =
                                          c_Response.au8_Data[6];

                                       if ((rc_CurrentResult.c_SerialNumber.u8_SerialNumberByteLength > 0) &&
                                           (rc_CurrentResult.c_SerialNumber.u8_SerialNumberByteLength <= 29))
                                       {
                                          // Second byte of payload is first byte of serial number
                                          if (rc_CurrentResult.c_SerialNumber.q_FsnSerialNumber == true)
                                          {
                                             // FSN format
                                             rc_CurrentResult.c_SerialNumber.c_SerialNumberExt =
                                                static_cast<charn>(c_Response.au8_Data[7]);
                                          }
                                          else
                                          {
                                             // POS format
                                             rc_CurrentResult.c_SerialNumber.au8_SerialNumber[0] =
                                                c_Response.au8_Data[7];
                                          }

                                          if (rc_CurrentResult.c_SerialNumber.u8_SerialNumberByteLength > 1)
                                          {
                                             // At least one further block is necessary
                                             q_Continue = true;
                                          }
                                       }
                                    }
                                    else
                                    {
                                       // Building the serial number
                                       // The first block has no part of the serial number and the
                                       // second block had only the first sign.
                                       const uint8 u8_SnrBlock = u8_ReceivedBlockNumber - 2U;
                                       uint32 u32_NextSnrSignIndex = (static_cast<uint32>(u8_SnrBlock) * 2U) + 1U;

                                       if (u32_NextSnrSignIndex <
                                           rc_CurrentResult.c_SerialNumber.u8_SerialNumberByteLength)
                                       {
                                          if (rc_CurrentResult.c_SerialNumber.q_FsnSerialNumber == true)
                                          {
                                             // FSN format
                                             rc_CurrentResult.c_SerialNumber.c_SerialNumberExt +=
                                                static_cast<charn>(c_Response.au8_Data[6]);
                                          }
                                          else if (u32_NextSnrSignIndex < 6)
                                          {
                                             // POS format
                                             rc_CurrentResult.c_SerialNumber.au8_SerialNumber[u32_NextSnrSignIndex] =
                                                c_Response.au8_Data[6];
                                          }
                                          else
                                          {
                                             // Nothing to do
                                          }
                                       }

                                       // Check the next sign
                                       ++u32_NextSnrSignIndex;

                                       // It is possible that the serial number has a length which does not need the
                                       // last byte of the last necessary block
                                       if (u32_NextSnrSignIndex <
                                           rc_CurrentResult.c_SerialNumber.u8_SerialNumberByteLength)
                                       {
                                          if (rc_CurrentResult.c_SerialNumber.q_FsnSerialNumber == true)
                                          {
                                             rc_CurrentResult.c_SerialNumber.c_SerialNumberExt +=
                                                static_cast<charn>(c_Response.au8_Data[7]);
                                          }
                                          else if (u32_NextSnrSignIndex < 6U)
                                          {
                                             // POS format
                                             rc_CurrentResult.c_SerialNumber.
                                             au8_SerialNumber[u32_NextSnrSignIndex] = c_Response.au8_Data[7];
                                          }
                                          else
                                          {
                                             // Nothing to do
                                          }
                                       }

                                       if (rc_CurrentResult.c_SerialNumber.q_FsnSerialNumber == true)
                                       {
                                          if (rc_CurrentResult.c_SerialNumber.c_SerialNumberExt.Length() <
                                              rc_CurrentResult.c_SerialNumber.u8_SerialNumberByteLength)
                                          {
                                             // Serial number is not finished yet
                                             q_Continue = true;
                                          }
                                       }
                                       else
                                       {
                                          if (u32_NextSnrSignIndex < 5U)
                                          {
                                             // Serial number is not finished yet. Exact 6 are necessary for POS
                                             q_Continue = true;
                                          }
                                       }
                                    }
                                 }
                                 else
                                 {
                                    // Should not happen. Same UniqueId but different SubNodeId
                                    m_LogWarningWithHeader(
                                       "Broadcast: Different SubNode Id with same UniqueId in response received. Ignoring.",
                                       TGL_UTIL_FUNC_ID);
                                 }
                              }
                              else
                              {
                                 // u32_UniqueId not found. Error on transmitting or calculation of CRC or
                                 // the device did not responded on the first block
                                 m_LogWarningWithHeader(
                                    "Broadcast: unexpected UniqueId in response received. Ignoring.",
                                    TGL_UTIL_FUNC_ID);
                              }
                           }
                        }
                        else
                        {
                           // Not requested block number
                           m_LogWarningWithHeader("Broadcast: unexpected block number  in response received. Ignoring.",
                                                  TGL_UTIL_FUNC_ID);
                        }
                     }
                     else if ((c_Response.u8_DLC == 4U) && (c_Response.au8_Data[0] == (mhu8_ISO15765_N_PCI_SF + 3U)) &&
                              (c_Response.au8_Data[1] == mhu8_BC_OSY_NR_SI) &&
                              (c_Response.au8_Data[2] == mhu8_OSY_BC_SI_READ_SERIAL_NUMBER))
                     {
                        //negative response detected
                        m_LogWarningWithHeader(
                           "Broadcast: Negative response received. Probably a node with a standard SN.",
                           TGL_UTIL_FUNC_ID);
                     }
                     else
                     {
                        m_LogWarningWithHeader("Broadcast: unexpected response received. Ignoring.", TGL_UTIL_FUNC_ID);
                     }
                  }
               }

               if (q_Continue == true)
               {
                  // Next bock number for next request
                  c_Service.c_Data[1] = c_Service.c_Data[1] + 1U;
                  m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);
               }
            }
         }
         while ((q_Continue == true) && (s32_Return == C_NO_ERR));

         // Add the extended results to the output
         if (s32_Return == C_NO_ERR)
         {
            std::map<uint32, C_BroadcastReadEcuSerialNumberExtendedResults>::iterator c_ItResult;

            for (c_ItResult = c_UniqueIdToResult.begin(); c_ItResult != c_UniqueIdToResult.end(); ++c_ItResult)
            {
               // Check the result for a valid serial number length
               if ((c_ItResult->second.c_SerialNumber.u8_SerialNumberByteLength > 0U) &&
                   (c_ItResult->second.c_SerialNumber.u8_SerialNumberByteLength <= 29) &&
  // Check for FSN serial number ext length
                   (((c_ItResult->second.c_SerialNumber.q_FsnSerialNumber == true) &&
                     (c_ItResult->second.c_SerialNumber.c_SerialNumberExt.Length() ==
                      c_ItResult->second.c_SerialNumber.u8_SerialNumberByteLength)) ||
  // Check for POS serial number length for exact 6 byte
                    ((c_ItResult->second.c_SerialNumber.q_FsnSerialNumber == false) &&
                     (c_ItResult->second.c_SerialNumber.u8_SerialNumberByteLength == 6U))))
               {
                  // Valid result
                  c_ItResult->second.c_SerialNumber.q_IsValid = true;

                  orc_ExtendedResponses.push_back(c_ItResult->second);
               }
               else
               {
                  // A not finished result
                  m_LogWarningWithHeader("An extended serial number had not the correct length.",
                                         TGL_UTIL_FUNC_ID);
               }
            }
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request all nodes on the bus to stay in the flashloader

   Send broadcast to set the "stay-in-flashloader" flag.
   Report back list of nodes that sent a response (with the status and node IDs).
   The function will wait for responses for the time configured with SetBroadcastTimeout()

   Incoming responses to other services will be dumped: we are strictly handshaking here ...
   (why would someone send other broadcast requests in parallel ?)

   \param[out]    orc_Results   information about all nodes that sent a response

   \return
   C_NO_ERR   no problems; zero or more responses received; data placed in orc_Results
   C_COM      could not send request
   C_CONFIG   no dispatcher installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::BroadcastRequestProgramming(
   std::vector<C_BroadcastRequestProgrammingResults> & orc_Results) const
{
   sint32 s32_Return;

   orc_Results.clear();
   if (mpc_CanDispatcher == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //set up request
      C_OSCProtocolDriverOsyService c_Service;
      T_STWCAN_Msg_TX c_Msg;
      c_Service.c_Data.resize(4);
      c_Service.c_Data[0] = mhu8_OSY_BC_SI_ROUTINE_CONTROL;
      c_Service.c_Data[1] = mhu8_OSY_BC_RC_SUB_FUNCTION_START_ROUTINE;
      c_Service.c_Data[2] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_REQUEST_PROGRAMMING >> 8U);
      c_Service.c_Data[3] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_REQUEST_PROGRAMMING & 0xFFU);
      m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);

      s32_Return = mpc_CanDispatcher->CAN_Send_Msg(c_Msg);
      if (s32_Return != C_NO_ERR)
      {
         m_LogWarningWithHeader("Could not send single frame broadcast CAN message.", TGL_UTIL_FUNC_ID);
         s32_Return = C_COM;
      }
      else
      {
         //check for responses
         const uint32 u32_StartTime = TGL_GetTickCount();
         T_STWCAN_Msg_RX c_Response;
         sint32 s32_ReturnLocal;

         while ((TGL_GetTickCount() - mu32_BroadcastTimeoutMs) < u32_StartTime)
         {
            //trigger dispatcher
            //ignore return value: we cannot be sure some other client did not check before us
            (void)mpc_CanDispatcher->DispatchIncoming();

            s32_ReturnLocal = mpc_CanDispatcher->ReadFromQueue(mu16_DispatcherClientHandle, c_Response);
            if (s32_ReturnLocal == C_NO_ERR)
            {
               C_BroadcastRequestProgrammingResults c_Result;
               bool q_ResultReceived = true;
               //format OK (frame type; number of bytes; service; routine ID)?
               if ((c_Response.u8_DLC == 5U) && (c_Response.au8_Data[0] == (mhu8_ISO15765_N_PCI_SF + 4U)) &&
                   (c_Response.au8_Data[1] == (mhu8_OSY_BC_SI_ROUTINE_CONTROL | 0x40U)) &&
                   (c_Response.au8_Data[2] == mhu8_OSY_BC_RC_SUB_FUNCTION_START_ROUTINE) &&
                   (c_Response.au8_Data[3] == c_Service.c_Data[2]) &&
                   (c_Response.au8_Data[4] == c_Service.c_Data[3]))
               {
                  //looks legit; extract payload ...
                  c_Result.q_RequestAccepted = true;
               }
               else if ((c_Response.u8_DLC == 4U) && (c_Response.au8_Data[0] == (mhu8_ISO15765_N_PCI_SF + 3U)) &&
                        (c_Response.au8_Data[1] == mhu8_BC_OSY_NR_SI) &&
                        (c_Response.au8_Data[2] == mhu8_OSY_BC_SI_ROUTINE_CONTROL))
               {
                  //negative response detected; interpret as "could not set flag"
                  c_Result.q_RequestAccepted = false;
               }
               else
               {
                  q_ResultReceived = false;
                  m_LogWarningWithHeader("Broadcast: unexpected response received. Ignoring.", TGL_UTIL_FUNC_ID);
               }
               if (q_ResultReceived == true)
               {
                  //extract node-id from sender
                  //must be local addressing ...
                  c_Result.c_SenderId.u8_BusIdentifier = mc_ClientId.u8_BusIdentifier; //same as ours ...
                  c_Result.c_SenderId.u8_NodeIdentifier = static_cast<uint8>(c_Response.u32_ID & 0x7FU);
                  orc_Results.push_back(c_Result);
               }
            }
         }
         s32_Return = C_NO_ERR;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node-id of node specified by serial number

   Send broadcast to set node-id by serial number.
   The function will wait for responses for the time configured with SetBroadcastTimeout().

   As specified the serial number is expected to be unique and non-addressed nodes shall keep their gob shut.
   So
   * exactly one response is expected and reported
   * multiple responses are interpreted as error

   Incoming responses to other services will be dumped: we are strictly handshaking here ...
   (why would someone send other broadcast requests in parallel ?)

   The service consists of three individual single frame services.
   A response is only expected after the third single frame.

   \param[in]    orc_SerialNumber     serial number of node to address
   \param[in]    orc_NewNodeId        node ID to set
   \param[out]   opu8_NrCode          if not NULL: negative response code (if C_WARN is returned)

   \return
   C_NO_ERR    no problems; one positive response received
   C_RANGE     invalid node ID (bus ID or node-ID out of range); "0x7F" is not permitted as node ID as it's reserved for
                broadcasts
               invalid serial number
   C_WARN      negative response received and no positive response received
   C_COM       could not send requests
   C_CONFIG    no dispatcher installed
   C_TIMEOUT   no response within timeout (was SetNodeIdentifiersForBroadcasts() called ?)
   C_OVERFLOW  multiple positive responses received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::BroadcastSetNodeIdBySerialNumber(const C_OSCProtocolSerialNumber & orc_SerialNumber,
                                                                     const C_OSCProtocolDriverOsyNode & orc_NewNodeId,
                                                                     uint8 * const opu8_NrCode) const
{
   sint32 s32_Return;

   if (mpc_CanDispatcher == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else if ((orc_NewNodeId.u8_BusIdentifier > C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) ||
            (orc_NewNodeId.u8_NodeIdentifier >= C_OSCProtocolDriverOsyNode::mhu8_MAX_NODE) ||
            (orc_SerialNumber.q_IsValid == false) ||
            (orc_SerialNumber.q_ExtFormatUsed == true))
   {
      s32_Return = C_RANGE;
   }
   else
   {
      //set up request
      C_OSCProtocolDriverOsyService c_Service;
      T_STWCAN_Msg_TX c_Msg;
      c_Service.c_Data.resize(7);
      c_Service.c_Data[0] = mhu8_OSY_BC_SI_ROUTINE_CONTROL;
      c_Service.c_Data[1] = mhu8_OSY_BC_RC_SUB_FUNCTION_START_ROUTINE;
      c_Service.c_Data[2] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART1 >> 8U);
      c_Service.c_Data[3] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART1 & 0xFFU);
      c_Service.c_Data[4] = orc_SerialNumber.au8_SerialNumber[0];
      c_Service.c_Data[5] = orc_SerialNumber.au8_SerialNumber[1];
      c_Service.c_Data[6] = orc_SerialNumber.au8_SerialNumber[2];
      m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);

      s32_Return = mpc_CanDispatcher->CAN_Send_Msg(c_Msg);
      if (s32_Return != C_NO_ERR)
      {
         m_LogWarningWithHeader("Could not send single frame broadcast CAN message.", TGL_UTIL_FUNC_ID);
         s32_Return = C_COM;
      }
      else
      {
         c_Service.c_Data[2] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART2 >> 8U);
         c_Service.c_Data[3] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART2 & 0xFFU);
         c_Service.c_Data[4] = orc_SerialNumber.au8_SerialNumber[3];
         c_Service.c_Data[5] = orc_SerialNumber.au8_SerialNumber[4];
         c_Service.c_Data[6] = orc_SerialNumber.au8_SerialNumber[5];
         m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);

         s32_Return = mpc_CanDispatcher->CAN_Send_Msg(c_Msg);
         if (s32_Return != C_NO_ERR)
         {
            m_LogWarningWithHeader("Could not send single frame broadcast CAN message.", TGL_UTIL_FUNC_ID);
            s32_Return = C_COM;
         }
         else
         {
            c_Service.c_Data.resize(6);
            c_Service.c_Data[0] = mhu8_OSY_BC_SI_ROUTINE_CONTROL;
            c_Service.c_Data[1] = mhu8_OSY_BC_RC_SUB_FUNCTION_START_ROUTINE;
            c_Service.c_Data[2] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART3 >> 8U);
            c_Service.c_Data[3] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_PART3 & 0xFFU);
            c_Service.c_Data[4] = orc_NewNodeId.u8_BusIdentifier;
            c_Service.c_Data[5] = orc_NewNodeId.u8_NodeIdentifier;
            m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);

            s32_Return = mpc_CanDispatcher->CAN_Send_Msg(c_Msg);
            if (s32_Return != C_NO_ERR)
            {
               m_LogWarningWithHeader("Could not send single frame broadcast CAN message.", TGL_UTIL_FUNC_ID);
               s32_Return = C_COM;
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = this->m_HandleBroadcastSetNodeIdBySerialNumberResponse(c_Service.c_Data[2],
                                                                             c_Service.c_Data[3],
                                                                             opu8_NrCode);
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node-id of node specified by serial number extended

   Send broadcast to set node-id by serial number extended.
   The function will wait for responses for the time configured with SetBroadcastTimeout().

   As specified the serial number is expected to be unique and non-addressed nodes shall keep their gob shut.
   So
   * exactly one response is expected and reported
   * multiple responses are interpreted as error

   Incoming responses to other services will be dumped: we are strictly handshaking here ...
   (why would someone send other broadcast requests in parallel ?)

   The service consists of at least 3 and maximum 12 individual single frame services.
   A response is only expected after the last single frame.
   The number of used single frames depends of the length of the serial number (1 to 29 bytes)

   \param[in]    orc_SerialNumber                     serial number of node to address (1 to 29 bytes allowed)
   \param[in]    ou8_SubNodeId                        sub node id of sub node to address (in case of a device without sub nodes: 0)
   \param[in]    orc_NewNodeId                        node ID to set
   \param[out]   opu8_NrCode                          if not NULL: negative response code (if C_WARN is returned)

   \return
   C_NO_ERR    no problems; one positive response received
   C_RANGE     invalid node ID (bus ID or node-ID out of range); "0x7F" is not permitted as node ID as it's reserved for
                broadcasts
               Serial number is empty or has more than 29 bytes
   C_WARN      negative response received and no positive response received
   C_COM       could not send requests
   C_CONFIG    no dispatcher installed
   C_TIMEOUT   no response within timeout (was SetNodeIdentifiersForBroadcasts() called ?)
   C_OVERFLOW  multiple positive responses received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::BroadcastSetNodeIdBySerialNumberExtended(
   const C_OSCProtocolSerialNumber & orc_SerialNumber, const uint8 ou8_SubNodeId,
   const C_OSCProtocolDriverOsyNode & orc_NewNodeId, uint8 * const opu8_NrCode) const
{
   sint32 s32_Return;

   if (mpc_CanDispatcher == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else if ((orc_NewNodeId.u8_BusIdentifier > C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) ||
            (orc_NewNodeId.u8_NodeIdentifier >= C_OSCProtocolDriverOsyNode::mhu8_MAX_NODE) ||
            (orc_SerialNumber.q_IsValid == false) ||
            (orc_SerialNumber.q_ExtFormatUsed == false))
   {
      s32_Return = C_RANGE;
   }
   else
   {
      // The payload will filled by:
      // - reserved; 1 byte (set to zero)
      // - parameters to set:
      // -- bus ID to set; 1 byte (0..15)
      // -- node ID to set; 1 byte (0..126)
      // - parameters to identify target device:
      // -- SubNodeId; 1 byte (0..15); set to zero if the device does not have logical sub-nodes
      // -- SerialNumberManufacturerFormat (1 byte)
      // -- SerialNumberLength (1 byte)
      // -- SerialNumber (1 .. 29 bytes)

      //set up request
      C_OSCProtocolDriverOsyService c_Service;
      T_STWCAN_Msg_TX c_Msg;
      uint16 u16_PartCounter = 0U;
      uint8 u8_SerialNumberBytesSent = 0U;
      const std::vector<uint8> c_RawSerialNumber = orc_SerialNumber.GetSerialNumberAsRawData();
      c_Service.c_Data.resize(7);
      c_Service.c_Data[0] = mhu8_OSY_BC_SI_ROUTINE_CONTROL;
      c_Service.c_Data[1] = mhu8_OSY_BC_RC_SUB_FUNCTION_START_ROUTINE;
      c_Service.c_Data[2] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_EXT_START >> 8U);

      do
      {
         c_Service.c_Data[3] = static_cast<uint8>(mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_EXT_START & 0xFFU) +
                               static_cast<uint8>(u16_PartCounter);

         if (u16_PartCounter == 0U)
         {
            c_Service.c_Data[4] = 0U; // reserved
            c_Service.c_Data[5] = orc_NewNodeId.u8_BusIdentifier;
            c_Service.c_Data[6] = orc_NewNodeId.u8_NodeIdentifier;
         }
         else if (u16_PartCounter == 1U)
         {
            c_Service.c_Data[4] = ou8_SubNodeId;
            c_Service.c_Data[5] = orc_SerialNumber.u8_SerialNumberManufacturerFormat;
            c_Service.c_Data[6] = orc_SerialNumber.u8_SerialNumberByteLength;
         }
         else
         {
            // Now the serial number will be part of the parts
            const uint8 u8_BytesLeft = orc_SerialNumber.u8_SerialNumberByteLength - u8_SerialNumberBytesSent;
            const uint8 u8_BytesToCopy = (u8_BytesLeft >= 3U) ? 3U : (u8_BytesLeft % 3U);

            if (u8_BytesToCopy < 3U)
            {
               // Set the not needed bytes to 0
               memset(&c_Service.c_Data[static_cast<uintn>(u8_BytesToCopy) + 4U], 0U,
                      3U - static_cast<uintn>(u8_BytesToCopy));
            }

            memcpy(&c_Service.c_Data[4], &c_RawSerialNumber[u8_SerialNumberBytesSent], u8_BytesToCopy);
            u8_SerialNumberBytesSent += u8_BytesToCopy;
         }

         m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);

         s32_Return = mpc_CanDispatcher->CAN_Send_Msg(c_Msg);
         if (s32_Return != C_NO_ERR)
         {
            m_LogWarningWithHeader("Could not send single frame broadcast CAN message.", TGL_UTIL_FUNC_ID);
            s32_Return = C_COM;
         }
         else
         {
         }

         // Check break conditions
         if ((u8_SerialNumberBytesSent >= orc_SerialNumber.u8_SerialNumberByteLength) ||
             ((mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_EXT_START + u16_PartCounter) >=
              mhu16_OSY_BC_RC_SID_SET_NODEID_BY_SERIALNUMBER_EXT_LAST))
         {
            break;
         }
         else
         {
            // At least one further part necessary
            ++u16_PartCounter;
         }
      }
      while (s32_Return == C_NO_ERR);

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = this->m_HandleBroadcastSetNodeIdBySerialNumberResponse(c_Service.c_Data[2],
                                                                             c_Service.c_Data[3],
                                                                             opu8_NrCode);
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send network reset broadcast

   Send broadcast to reset all nodes.
   There will be no response to this service.

   \param[in]  ou8_ResetType        Reset type (0x02: keyOffOnReset, 0x60: resetToFlashloader)

   \return
   C_NO_ERR   no problems
   C_COM      could not send request
   C_CONFIG   no dispatcher installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::BroadcastEcuReset(const uint8 ou8_ResetType) const
{
   sint32 s32_Return;

   if (mpc_CanDispatcher == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //set up request
      C_OSCProtocolDriverOsyService c_Service;
      T_STWCAN_Msg_TX c_Msg;
      c_Service.c_Data.resize(2);
      c_Service.c_Data[0] = mhu8_OSY_BC_SI_ECU_RESET;
      c_Service.c_Data[1] = ou8_ResetType;
      m_ComposeSingleFrame(c_Service, m_GetTxBroadcastIdentifier(), c_Msg);

      s32_Return = mpc_CanDispatcher->CAN_Send_Msg(c_Msg);
      if (s32_Return != C_NO_ERR)
      {
         m_LogWarningWithHeader("Could not send single frame broadcast CAN message.", TGL_UTIL_FUNC_ID);
         s32_Return = C_COM;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send broadcast service EnterDiagnosticSession(PreProgramming)

   Only sends one request and returns immediately. Results are not reported.
   Purpose: hit into the time window when the devices are started to keep the devices in flashloader.

   The suppressPosRspMsgIndicationBit will be set to avoid flooding the bus with response messages.

   \return
   C_NO_ERR   no problems
   C_COM      could not send request
   C_CONFIG   no dispatcher installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::BroadcastSendEnterPreProgrammingSession(void) const
{
   return this->m_BroadcastSendDiagnosticSessionControl(0x60U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send broadcast service EnterDiagnosticSession(Default)

   Only sends one request and returns immediately. Results are not reported.
   Purpose: hit into the time window when the devices are started to keep the devices in flashloader.

   The suppressPosRspMsgIndicationBit will be set to avoid flooding the bus with response messages.

   \return
   C_NO_ERR   no problems
   C_COM      could not send request
   C_CONFIG   no dispatcher installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpCan::BroadcastSendEnterDefaultSession(void) const
{
   return this->m_BroadcastSendDiagnosticSessionControl(0x01U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Dump all messages of receive queue of CAN dispatcher
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsyTpCan::ClearDispatcherQueue(void)
{
   if (this->mpc_CanDispatcher != NULL)
   {
      this->mpc_CanDispatcher->ClearQueue(this->mu16_DispatcherClientHandle);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Print logging text

   Add information about this class and the current server-id configuration and call logging handler function
    to perform the actual logging.

   \param[in]     orc_Information     text to log
   \param[in]     opcn_Function       function name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsyTpCan::m_LogWarningWithHeader(const stw_scl::C_SCLString & orc_Information,
                                                         const charn * const opcn_Function) const
{
   C_OSCLoggingHandler::h_WriteLogError("openSYDE CAN-TP", "openSYDE CAN-TP node " + C_SCLString::IntToStr(
                                           mc_ServerId.u8_BusIdentifier) + "." +
                                        C_SCLString::IntToStr(
                                           mc_ServerId.u8_NodeIdentifier) + ": " + orc_Information, __FILE__,
                                        opcn_Function);
}
